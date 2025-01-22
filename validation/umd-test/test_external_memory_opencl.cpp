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
#include "umd_dma_heap_system.hpp"

#include <CL/cl.h>
#include <CL/cl_ext.h>

static cl_platform_id getIntelOpenCLPlatform();

using ExternalMemoryOpenCL = UmdTest;

//
// +------------+                +--------------------------+            +------------+
// | NPU mem 1. | ->   NPU    -> | NPU mem 1. -> GPU mem 1. | ->  GPU -> | GPU mem 2. |
// |            |    Mem Fill    |  (Export)     (Import)   |    Copy    |            |
// +------------+                +--------------------------+            +------------+
//
TEST_F(ExternalMemoryOpenCL, NpuToGpu) {
    if (!isVPU37xx()) {
        SKIP_("BuffersImport test is supported on MTL platform only");
    }

    const size_t size = 1024u;

    // Alloc NPU buffer
    ze_external_memory_export_desc_t descExport = {};
    descExport.stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_DESC;
    descExport.flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF;

    ze_device_mem_alloc_desc_t descAlloc = {};
    descAlloc.stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC;
    descAlloc.pNext = &descExport;

    void *zePtr;
    ASSERT_EQ(zeMemAllocDevice(zeContext, &descAlloc, size, 0, zeDevice, &zePtr),
              ZE_RESULT_SUCCESS);

    ze_command_queue_desc_t commandQueueDesc{
        .stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
        .pNext = nullptr,
        .ordinal = 0u,
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
        .commandQueueGroupOrdinal = 0u,
        .flags = 0,
    };
    ze_command_list_handle_t zeCommandList;
    ASSERT_EQ(zeCommandListCreate(zeContext, zeDevice, &commandListDesc, &zeCommandList),
              ZE_RESULT_SUCCESS);

    // Fill buffer with pattern on NPU
    uint8_t pattern = 0x9A;
    ASSERT_EQ(zeCommandListAppendMemoryFill(zeCommandList,
                                            zePtr,
                                            &pattern,
                                            sizeof(pattern),
                                            size,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(zeCommandList), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(zeQueue, 1, &zeCommandList, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueSynchronize(zeQueue, syncTimeout), ZE_RESULT_SUCCESS);

    // Export NPU buffer
    ze_external_memory_export_fd_t externalExportFdDesc = {
        .stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_FD,
        .pNext = nullptr,
        .flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF,
        .fd = -1};

    ze_memory_allocation_properties_t pMemAllocProperties = {
        .stype = ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES,
        .pNext = &externalExportFdDesc,
        .type = ZE_MEMORY_TYPE_UNKNOWN,
        .id = 0,
        .pageSize = 0};

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeMemGetAllocProperties(zeContext, zePtr, &pMemAllocProperties, nullptr));

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

    // Import NPU buffer as OpenCL input
    cl_mem_properties memProps[] = {
        static_cast<cl_mem_properties>(CL_EXTERNAL_MEMORY_HANDLE_DMA_BUF_KHR),
        static_cast<cl_mem_properties>(externalExportFdDesc.fd),
        0};
    cl_mem oclInputMem = clCreateBufferWithProperties(oclContext,
                                                      memProps,
                                                      CL_MEM_READ_ONLY,
                                                      size,
                                                      NULL,
                                                      &oclResult);
    ASSERT_EQ(CL_SUCCESS, oclResult);

    // Create OpenCL output buffer
    cl_mem oclOutputMem = clCreateBuffer(oclContext, CL_MEM_WRITE_ONLY, size, NULL, &oclResult);
    ASSERT_EQ(CL_SUCCESS, oclResult);

    // Copy input to output
    const char *kernelSource = "kernel void copy(__global char* input, __global char* output) {\n"
                               "    int i = get_global_id(0);\n"
                               "    output[i] = input[i];\n"
                               "}\n";
    size_t kernelSize = strlen(kernelSource);
    cl_program oclProgram =
        clCreateProgramWithSource(oclContext, 1, &kernelSource, &kernelSize, &oclResult);
    ASSERT_EQ(CL_SUCCESS, oclResult);

    ASSERT_EQ(clBuildProgram(oclProgram, 1, &oclDevice, nullptr, nullptr, nullptr), CL_SUCCESS);

    cl_kernel oclKernel = clCreateKernel(oclProgram, "copy", &oclResult);
    ASSERT_EQ(CL_SUCCESS, oclResult);

    ASSERT_EQ(clSetKernelArg(oclKernel, 0, sizeof(oclInputMem), &oclInputMem), CL_SUCCESS);
    ASSERT_EQ(clSetKernelArg(oclKernel, 1, sizeof(oclOutputMem), &oclOutputMem), CL_SUCCESS);

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

    uint8_t outputBuffer[size] = {};
    ASSERT_EQ(
        clEnqueueReadBuffer(oclQueue, oclOutputMem, CL_TRUE, 0, size, outputBuffer, 0, NULL, NULL),
        CL_SUCCESS);

    // Validate output buffer
    for (size_t i = 0; i < size; i++)
        ASSERT_EQ(outputBuffer[i], pattern) << "index=" << i;

    // Release resources
    ASSERT_EQ(zeMemFree(zeContext, zePtr), ZE_RESULT_SUCCESS);
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

    std::vector<cl_platform_id> platforms(numPlatforms);
    result = clGetPlatformIDs(numPlatforms, platforms.data(), nullptr);
    if (result != CL_SUCCESS)
        return nullptr;

    for (cl_uint i = 0; i < numPlatforms; i++) {
        size_t size = 0;
        result = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 0, nullptr, &size);
        if (result != CL_SUCCESS)
            return nullptr;

        std::vector<char> vendor(size + 1, 0);
        result = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, size, vendor.data(), nullptr);
        if (result != CL_SUCCESS)
            return nullptr;

        if (std::string(vendor.data()) == "Intel(R) Corporation") {
            return platforms[i];
        }
    }

    return nullptr;
}
