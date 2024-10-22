/*
 * Copyright (C) 2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_dma_heap_system.hpp"
#include "umd_test.h"

class ImportMemoryUsingDmaHeap : public UmdTest, public ::testing::WithParamInterface<uint64_t> {
  public:
    /* Get allocation properties descriptor */
    ze_memory_allocation_properties_t pMemAllocProperties = {
        .stype = ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES,
        .pNext = nullptr,
        .type = ZE_MEMORY_TYPE_UNKNOWN,
        .id = 0,
        .pageSize = 0};

    /* Alloc device memory descriptor */
    ze_device_mem_alloc_desc_t pDeviceMemAllocDesc = {.stype =
                                                          ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
                                                      .pNext = nullptr,
                                                      .flags = 0,
                                                      .ordinal = 0};

    /* Descriptor to import allocation from fd */
    ze_external_memory_import_fd_t externalImportFromFdDesc = {
        .stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_FD,
        .pNext = nullptr,
        .flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF,
        .fd = -1};

    DmaHeapSystem dmaHeapSystem;
};

INSTANTIATE_TEST_SUITE_P(,
                         ImportMemoryUsingDmaHeap,
                         ::testing::Values(2 * KB, 16 * MB, 255 * MB),
                         [](const testing::TestParamInfo<uint64_t> &cmd) {
                             return memSizeToStr(cmd.param);
                         });

TEST_P(ImportMemoryUsingDmaHeap, AllocDeviceMemory) {
    ze_result_t ret;
    size_t dmaBufferSize = GetParam();

    CHECK_DMA_HEAP_SUPPORT(dmaHeapSystem);

    auto dmaBuffer = dmaHeapSystem.allocDmaHeapBuffer(dmaBufferSize);
    ASSERT_NE(dmaBuffer, nullptr);

    /* Prepare request to import dma buffer as device memory */
    externalImportFromFdDesc.fd = dmaBuffer->fd;
    pDeviceMemAllocDesc.pNext = &externalImportFromFdDesc;

    auto scopedImportedMemory =
        zeScope::memAllocDevice(zeContext, pDeviceMemAllocDesc, dmaBufferSize, 0, zeDevice, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_NE(scopedImportedMemory.get(), nullptr);

    /* Check allocation properties */
    pMemAllocProperties.pNext = nullptr;
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeMemGetAllocProperties(zeContext,
                                      scopedImportedMemory.get(),
                                      &pMemAllocProperties,
                                      nullptr));
    ASSERT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_DEVICE);
    ASSERT_GT(pMemAllocProperties.id, 0u);
    ASSERT_EQ(pMemAllocProperties.pageSize, ALLIGN_TO_PAGE(dmaBufferSize));

    /* mmap original dma buffer and write pattern to it */
    const int32_t pattern = 0xDEADAABB;
    memcpy(dmaBuffer->ptr, &pattern, sizeof(pattern));

    /*check if pattern match on imported buffer*/
    int32_t *importedPtr = static_cast<int32_t *>(scopedImportedMemory.get());
    EXPECT_EQ(*importedPtr, pattern);
}
