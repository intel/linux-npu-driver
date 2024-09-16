/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <fcntl.h>
#include <gtest/gtest.h>
#include <linux/dma-heap.h>
#include <linux/kernel.h>
#include <numeric>
#include <sys/ioctl.h>

#define CL_TARGET_OPENCL_VERSION 300
#include "level_zero/ze_api.h"
#include "umd_prime_buffers.h"

#include <CL/cl.h>
#include <CL/cl_ext.h>

static cl_platform_id getIntelOpenCLPlatform();

using BuffersImport = UmdTest;

//                                     buffer allocated
//                                       on dma_heap
// +---------------+                  +----------------+                +-------------+
// |  oclInputMem  | -> GPU kernel -> |  oclOutputMem  | -> NPU copy -> | zeOutputPtr |
// +---------------+                  |       or       |                +-------------+
//                                    |   zeInputPtr   |
//                                    +----------------+
//
TEST_F(BuffersImport, GPUclKernelToNPUzeCopy) {
    if (!isVPU37xx()) {
        SKIP_("BuffersImport test is supported on MTL platform only");
    }

    PrimeBufferHelper primeBufferHelper;
    if (!primeBufferHelper.hasDMABufferSupport()) {
        SKIP_("Prime buffers support is not available. Probably need root privileges");
    }

    // OCL init
    cl_platform_id platform = getIntelOpenCLPlatform();
    if (platform == nullptr) {
        SKIP_("Unable to find Intel OpenCL platform");
    }

    cl_device_id oclDevice;
    ASSERT_EQ(clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &oclDevice, nullptr), CL_SUCCESS);

    cl_context_properties contextProps[] = {
        CL_CONTEXT_PLATFORM,
        reinterpret_cast<cl_context_properties>(platform),
        0,
    };
    cl_int oclResult;
    cl_context oclContext =
        clCreateContext(contextProps, 1, &oclDevice, nullptr, nullptr, &oclResult);
    ASSERT_EQ(CL_SUCCESS, oclResult);

    cl_command_queue oclQueue =
        clCreateCommandQueueWithProperties(oclContext, oclDevice, nullptr, &oclResult);
    ASSERT_EQ(CL_SUCCESS, oclResult);

    // create input buffer
    size_t size = 20;
    uint8_t inputBuffer[size];
    std::iota(inputBuffer, inputBuffer + size, 1);

    cl_mem oclInputMem = clCreateBuffer(oclContext,
                                        CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
                                        size,
                                        inputBuffer,
                                        &oclResult);
    ASSERT_EQ(CL_SUCCESS, oclResult);

    int bufferFd = -1;
    ASSERT_TRUE(primeBufferHelper.createDMABuffer(size, bufferFd));

    cl_mem_properties memProps[] = {
        static_cast<cl_mem_properties>(CL_EXTERNAL_MEMORY_HANDLE_DMA_BUF_KHR),
        static_cast<cl_mem_properties>(bufferFd),
        0};
    cl_mem oclOutputMem = clCreateBufferWithProperties(oclContext,
                                                       memProps,
                                                       CL_MEM_READ_WRITE,
                                                       size,
                                                       NULL,
                                                       &oclResult);
    ASSERT_EQ(CL_SUCCESS, oclResult);

    // create a kernel that adds 2 to the input buffer
    const char *kernelSource = "kernel void add(__global char* input, __global char* output) {\n"
                               "    int i = get_global_id(0);\n"
                               "    output[i] = input[i] + 2;\n"
                               "}\n";
    size_t kernelSize = strlen(kernelSource);
    cl_program oclProgram =
        clCreateProgramWithSource(oclContext, 1, &kernelSource, &kernelSize, &oclResult);
    ASSERT_EQ(CL_SUCCESS, oclResult);

    ASSERT_EQ(clBuildProgram(oclProgram, 1, &oclDevice, nullptr, nullptr, nullptr), CL_SUCCESS);

    cl_kernel oclKernel = clCreateKernel(oclProgram, "add", &oclResult);
    ASSERT_EQ(CL_SUCCESS, oclResult);

    ASSERT_EQ(clSetKernelArg(oclKernel, 0, sizeof(oclInputMem), &oclInputMem), CL_SUCCESS);
    ASSERT_EQ(clSetKernelArg(oclKernel, 1, sizeof(oclOutputMem), &oclOutputMem), CL_SUCCESS);
    // End of OCL init

    // L0 init
    ze_command_queue_desc_t commandQueueDesc{
        .stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
        .pNext = nullptr,
        .ordinal = copyGrpOrdinal,
        .index = 0,
        .flags = 0,
        .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
        .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL,
    };
    ze_command_queue_handle_t zeQueue;
    ASSERT_EQ(zeCommandQueueCreate(zeContext, zeDevice, &commandQueueDesc, &zeQueue),
              ZE_RESULT_SUCCESS);

    ze_command_list_desc_t commandListDesc{
        .stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
        .pNext = nullptr,
        .commandQueueGroupOrdinal = copyGrpOrdinal,
        .flags = 0,
    };
    ze_command_list_handle_t zeCommandList;
    ASSERT_EQ(zeCommandListCreate(zeContext, zeDevice, &commandListDesc, &zeCommandList),
              ZE_RESULT_SUCCESS);

    // import the buffer from heapAllocationData.fd
    ze_external_memory_import_fd_t memoryImportDesc = {
        .stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_FD,
        .pNext = nullptr,
        .flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF,
        .fd = static_cast<int>(bufferFd),
    };
    ze_device_mem_alloc_desc_t deviceMemAllocDescInput{
        .stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
        .pNext = &memoryImportDesc,
        .flags = 0,
        .ordinal = 0,
    };
    void *zeInputPtr;
    ASSERT_EQ(zeMemAllocDevice(zeContext, &deviceMemAllocDescInput, size, 0, zeDevice, &zeInputPtr),
              ZE_RESULT_SUCCESS);

    ze_device_mem_alloc_desc_t deviceMemAllocDescOutput{
        .stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
        .pNext = nullptr,
        .flags = 0,
        .ordinal = 0,
    };
    ze_host_mem_alloc_desc_t hostMemAllocDescOutput{
        .stype = ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC,
        .pNext = nullptr,
        .flags = 0,
    };
    void *zeOutputPtr;
    ASSERT_EQ(zeMemAllocShared(zeContext,
                               &deviceMemAllocDescOutput,
                               &hostMemAllocDescOutput,
                               size,
                               0,
                               zeDevice,
                               &zeOutputPtr),
              ZE_RESULT_SUCCESS);
    // End of L0 init

    // Enqueue kernel on GPU
    ASSERT_EQ(clEnqueueNDRangeKernel(oclQueue,
                                     oclKernel,
                                     1,
                                     nullptr,
                                     &size,
                                     nullptr,
                                     0,
                                     nullptr,
                                     nullptr),
              CL_SUCCESS);

    // Append memory copy on NPU
    ASSERT_EQ(zeCommandListAppendMemoryCopy(zeCommandList,
                                            zeOutputPtr,
                                            zeInputPtr,
                                            size,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(zeCommandList), ZE_RESULT_SUCCESS);

    // Execute the kernel on GPU
    ASSERT_EQ(clFinish(oclQueue), CL_SUCCESS);

    // Execute the memory copy on NPU
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(zeQueue, 1, &zeCommandList, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueSynchronize(zeQueue, syncTimeout), ZE_RESULT_SUCCESS);

    uint8_t expected[size];
    for (size_t i = 0; i < size; i++)
        expected[i] = inputBuffer[i] + 2;
    ASSERT_EQ(0, memcmp(expected, zeOutputPtr, size));

    // Release resources
    ASSERT_EQ(zeMemFree(zeContext, zeOutputPtr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeMemFree(zeContext, zeInputPtr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListDestroy(zeCommandList), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueDestroy(zeQueue), ZE_RESULT_SUCCESS);

    ASSERT_EQ(clReleaseProgram(oclProgram), CL_SUCCESS);
    ASSERT_EQ(clReleaseKernel(oclKernel), CL_SUCCESS);
    ASSERT_EQ(clReleaseMemObject(oclOutputMem), CL_SUCCESS);
    ASSERT_EQ(clReleaseMemObject(oclInputMem), CL_SUCCESS);
    ASSERT_EQ(clReleaseCommandQueue(oclQueue), CL_SUCCESS);
    ASSERT_EQ(clReleaseContext(oclContext), CL_SUCCESS);
    ASSERT_EQ(clReleaseDevice(oclDevice), CL_SUCCESS);
}

cl_platform_id getIntelOpenCLPlatform() {
    cl_uint numPlatforms = 0;
    cl_int result = clGetPlatformIDs(0, nullptr, &numPlatforms);
    if (result != CL_SUCCESS)
        return nullptr;

    cl_platform_id platforms[numPlatforms];
    result = clGetPlatformIDs(numPlatforms, platforms, nullptr);
    if (result != CL_SUCCESS)
        return nullptr;

    for (cl_uint i = 0; i < numPlatforms; i++) {
        size_t size = 0;
        result = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 0, nullptr, &size);
        if (result != CL_SUCCESS)
            return nullptr;

        char vendor[size + 1];
        result = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, size, vendor, nullptr);
        if (result != CL_SUCCESS)
            return nullptr;

        if (std::string(vendor) == "Intel(R) Corporation") {
            return platforms[i];
        }
    }

    return nullptr;
}
