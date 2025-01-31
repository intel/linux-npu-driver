/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "test_app.h"
#include "umd_test.h"

#include <level_zero/ze_api.h>

class ExternalMemoryZe : public UmdTest {
  public:
    struct Xpu {
        ze_device_handle_t dev;
        ze_context_handle_t ctx;
        uint32_t ordinal;
    } Npu, Gpu;

    void SetUp() override {
        UmdTest::SetUp();
        Npu = {.dev = zeDevice, .ctx = zeContext, .ordinal = 0u};
        Gpu = {.dev = zeDeviceGpu, .ctx = zeContextGpu, .ordinal = copyGrpOrdinalGpu};
    }

    void ExportImport(struct Xpu x0, struct Xpu x1);
};

void ExternalMemoryZe::ExportImport(struct Xpu x0, struct Xpu x1) {
    ze_device_handle_t dev0 = x0.dev;
    ze_context_handle_t dev0Ctx = x0.ctx;
    uint32_t dev0Ordinal = x0.ordinal;

    ze_device_handle_t dev1 = x1.dev;
    ze_context_handle_t dev1Ctx = x1.ctx;
    uint32_t dev1Ordinal = x1.ordinal;

    const size_t sz = 1024u;
    ze_result_t ret;

    if (!isVPU37xx())
        SKIP_("Test for MTL (37xx) platform only.");

    if (!test_vars::test_with_gpu)
        SKIP_("Flag --test_with_gpu not set.");

    // --- dev0
    ze_device_external_memory_properties_t propMem = {};
    ASSERT_EQ(zeDeviceGetExternalMemoryProperties(dev0, &propMem), ZE_RESULT_SUCCESS);

    if (!(propMem.memoryAllocationExportTypes & ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF))
        GTEST_FAIL() << "Device does not support exporting DMA_BUF";

    ze_external_memory_export_desc_t descExport = {};
    descExport.stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_DESC;
    descExport.flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF;

    ze_device_mem_alloc_desc_t descAlloc = {};
    descAlloc.stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC;
    descAlloc.pNext = &descExport;

    std::shared_ptr<void> dev0Mem = zeScope::memAllocDevice(dev0Ctx, descAlloc, sz, 0, dev0, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    ze_command_queue_handle_t dev0Queue;
    ze_command_queue_desc_t descQueue{
        .stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
        .pNext = nullptr,
        .ordinal = dev0Ordinal,
        .index = 0,
        .flags = 0,
        .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
        .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL,
    };
    ASSERT_EQ(zeCommandQueueCreate(dev0Ctx, dev0, &descQueue, &dev0Queue), ZE_RESULT_SUCCESS);

    ze_command_list_handle_t dev0List;
    ze_command_list_desc_t descList{
        .stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
        .pNext = nullptr,
        .commandQueueGroupOrdinal = dev0Ordinal,
        .flags = 0,
    };
    ASSERT_EQ(zeCommandListCreate(dev0Ctx, dev0, &descList, &dev0List), ZE_RESULT_SUCCESS);

    uint8_t pattern = 0xAB;
    std::vector<typeof(pattern)> ref_buf((sz + sizeof(pattern)) / sizeof(pattern), pattern);

    ASSERT_EQ(zeCommandListAppendMemoryFill(dev0List,
                                            dev0Mem.get(),
                                            &pattern,
                                            sizeof(pattern),
                                            sz,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(dev0List), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(dev0Queue, 1, &dev0List, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(dev0Queue, syncTimeout), ZE_RESULT_SUCCESS);

    ze_external_memory_export_fd_t fdExport = {};
    fdExport.stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_FD;
    fdExport.flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF;

    ze_memory_allocation_properties_t propAlloc = {};
    propAlloc.stype = ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES;
    propAlloc.pNext = &fdExport;

    ASSERT_EQ(zeMemGetAllocProperties(dev0Ctx, dev0Mem.get(), &propAlloc, &dev0),
              ZE_RESULT_SUCCESS);
    ASSERT_NE(fdExport.fd, 0);

    // --- dev1
    memset(&propMem, 0, sizeof(propMem));
    ASSERT_EQ(zeDeviceGetExternalMemoryProperties(dev1, &propMem), ZE_RESULT_SUCCESS);

    if (!(propMem.memoryAllocationImportTypes & ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF))
        GTEST_SKIP() << "Device does not support importing DMA_BUF";

    ze_external_memory_import_fd_t fdImport = {};
    fdImport.stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_FD;
    fdImport.flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF;
    fdImport.fd = fdExport.fd;

    memset(&descAlloc, 0, sizeof(descAlloc));
    descAlloc.stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC;
    descAlloc.pNext = &fdImport;

    std::shared_ptr<void> dev1MemIn = zeScope::memAllocDevice(dev1Ctx, descAlloc, sz, 0, dev1, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);

    std::shared_ptr<void> dev1MemOut = zeMemory::allocShared(dev1Ctx, dev1, sz);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);

    ze_command_queue_handle_t dev1Queue;
    descQueue.ordinal = dev1Ordinal;
    ASSERT_EQ(zeCommandQueueCreate(dev1Ctx, dev1, &descQueue, &dev1Queue), ZE_RESULT_SUCCESS);

    ze_command_list_handle_t dev1List;
    descList.commandQueueGroupOrdinal = dev1Ordinal;
    ASSERT_EQ(zeCommandListCreate(dev1Ctx, dev1, &descList, &dev1List), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendMemoryCopy(dev1List,
                                            dev1MemOut.get(),
                                            dev1MemIn.get(),
                                            sz,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(dev1List), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(dev1Queue, 1, &dev1List, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(dev1Queue, syncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_EQ(memcmp(dev1MemOut.get(), ref_buf.data(), sz), 0);
}

//
// +------------+                +--------------------------+            +------------+
// | GPU mem 1. | ->   GPU    -> | GPU mem 1. -> NPU mem 1. | ->  NPU -> | NPU mem 2. |
// |            |    Mem Fill    |  (Export)     (Import)   |    Copy    |            |
// +------------+                +--------------------------+            +------------+
//
TEST_F(ExternalMemoryZe, GpuZeFillToNpuZeCopy) {
    ExportImport(Gpu, Npu);
}

// +------------+                +--------------------------+            +------------+
// | NPU mem 1. | ->   NPU    -> | NPU mem 1. -> GPU mem 1. | ->  GPU -> | GPU mem 2. |
// |            |    Mem Fill    |  (Export)     (Import)   |    Copy    |            |
// +------------+                +--------------------------+            +------------+
//
TEST_F(ExternalMemoryZe, NpuZeFillToGpuZeCopy) {
    ExportImport(Npu, Gpu);
}
