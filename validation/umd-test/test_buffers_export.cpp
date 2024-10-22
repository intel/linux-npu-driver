/*
 * Copyright (C) 2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <sys/mman.h>

class ExportMemory : public UmdTest, public ::testing::WithParamInterface<uint64_t> {
  public:
    /* Get allocation properties descriptor */
    ze_memory_allocation_properties_t pMemAllocProperties = {
        .stype = ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES,
        .pNext = nullptr,
        .type = ZE_MEMORY_TYPE_UNKNOWN,
        .id = 0,
        .pageSize = 0};

    /* Alloc host memory descriptor */
    ze_host_mem_alloc_desc_t pHostMemAllocDesc = {.stype = ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC,
                                                  .pNext = nullptr,
                                                  .flags = 0};
    /* Alloc device memory descriptor */
    ze_device_mem_alloc_desc_t pDeviceMemAllocDesc = {.stype =
                                                          ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
                                                      .pNext = nullptr,
                                                      .flags = 0,
                                                      .ordinal = 0};

    /* Descriptor for create exportable memory */
    ze_external_memory_export_desc_t externalExportDesc = {
        .stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_DESC,
        .pNext = nullptr,
        .flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF};

    /* Descriptor to retrieve Fd through get properties */
    ze_external_memory_export_fd_t externalExportFdDesc = {
        .stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_FD,
        .pNext = nullptr,
        .flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF,
        .fd = -1};
};

TEST_F(ExportMemory, GetExternalMemoryProperties) {
    ze_device_external_memory_properties_t prop = {};
    prop.stype = ZE_STRUCTURE_TYPE_DEVICE_EXTERNAL_MEMORY_PROPERTIES;

    EXPECT_EQ(zeDeviceGetExternalMemoryProperties(zeDevice, &prop), ZE_RESULT_SUCCESS);
    EXPECT_EQ(prop.memoryAllocationImportTypes & ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF,
              ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF);
    EXPECT_EQ(prop.memoryAllocationExportTypes & ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF,
              ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF);
}

INSTANTIATE_TEST_SUITE_P(,
                         ExportMemory,
                         ::testing::Values(2 * KB, 16 * MB, 255 * MB),
                         [](const testing::TestParamInfo<uint64_t> &cmd) {
                             return memSizeToStr(cmd.param);
                         });

TEST_P(ExportMemory, AllocDeviceMemory) {
    ze_result_t ret;
    uint64_t size = GetParam();

    /* Pepare request for external allocation */
    pDeviceMemAllocDesc.pNext = &externalExportDesc;

    auto scopedMem =
        zeScope::memAllocDevice(zeContext, pDeviceMemAllocDesc, size, 0, zeDevice, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_NE(scopedMem.get(), nullptr);
    const int32_t pattern = 0xDEADAABB;
    memcpy(scopedMem.get(), &pattern, sizeof(pattern));

    /* Prepare request for map allocation to fd */
    pMemAllocProperties.pNext = &externalExportFdDesc;
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeMemGetAllocProperties(zeContext, scopedMem.get(), &pMemAllocProperties, nullptr));
    ASSERT_GE(externalExportFdDesc.fd, 0);
    EXPECT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_DEVICE);
    EXPECT_GT(pMemAllocProperties.id, 0u);
    EXPECT_EQ(pMemAllocProperties.pageSize, size);
    EXPECT_EQ(lseek(externalExportFdDesc.fd, 0, SEEK_END), ALLIGN_TO_PAGE(size));
    lseek(externalExportFdDesc.fd, 0, SEEK_CUR);

    /* mmap memory and check pattern */
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, externalExportFdDesc.fd, 0);
    ASSERT_NE(ptr, MAP_FAILED) << "error " << errno;
    EXPECT_EQ(*static_cast<int32_t *>(ptr), pattern);
    EXPECT_EQ(munmap(ptr, size), 0);
    EXPECT_EQ(close(externalExportFdDesc.fd), 0);
}

TEST_P(ExportMemory, AllocHostMemory) {
    ze_result_t ret;
    uint64_t size = GetParam();

    /* Pepare request for external allocation */
    pHostMemAllocDesc.pNext = &externalExportDesc;

    auto scopedMem = zeScope::memAllocHost(zeContext, pHostMemAllocDesc, size, 0, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_NE(scopedMem.get(), nullptr);
    const int32_t pattern = 0xDEADAABB;
    memcpy(scopedMem.get(), &pattern, sizeof(pattern));

    /* Prepare request for map allocation to fd */
    pMemAllocProperties.pNext = &externalExportFdDesc;
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeMemGetAllocProperties(zeContext, scopedMem.get(), &pMemAllocProperties, nullptr));
    ASSERT_GE(externalExportFdDesc.fd, 0);
    EXPECT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_HOST);
    EXPECT_GT(pMemAllocProperties.id, 0u);
    EXPECT_EQ(pMemAllocProperties.pageSize, size);
    EXPECT_EQ(lseek(externalExportFdDesc.fd, 0, SEEK_END), ALLIGN_TO_PAGE(size));
    lseek(externalExportFdDesc.fd, 0, SEEK_CUR);

    /* mmap memory and check pattern */
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, externalExportFdDesc.fd, 0);
    ASSERT_NE(ptr, MAP_FAILED) << "error " << errno;
    EXPECT_EQ(*static_cast<int32_t *>(ptr), pattern);
    EXPECT_EQ(munmap(ptr, size), 0);
    EXPECT_EQ(close(externalExportFdDesc.fd), 0);
}

TEST_P(ExportMemory, AllocSharedMemory) {
    ze_result_t ret;
    uint64_t size = GetParam();

    /* Pepare request for external allocation */
    pDeviceMemAllocDesc.pNext = &externalExportDesc;

    auto scopedMem = zeScope::memAllocShared(zeContext,
                                             pDeviceMemAllocDesc,
                                             pHostMemAllocDesc,
                                             size,
                                             0,
                                             zeDevice,
                                             ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_NE(scopedMem.get(), nullptr);
    const int32_t pattern = 0xDEADAABB;
    memcpy(scopedMem.get(), &pattern, sizeof(pattern));

    /* Prepare request for map allocation to fd */
    pMemAllocProperties.pNext = &externalExportFdDesc;
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeMemGetAllocProperties(zeContext, scopedMem.get(), &pMemAllocProperties, nullptr));
    ASSERT_GE(externalExportFdDesc.fd, 0);
    EXPECT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_SHARED);
    EXPECT_GT(pMemAllocProperties.id, 0u);
    EXPECT_EQ(pMemAllocProperties.pageSize, size);
    EXPECT_EQ(lseek(externalExportFdDesc.fd, 0, SEEK_END), ALLIGN_TO_PAGE(size));
    lseek(externalExportFdDesc.fd, 0, SEEK_CUR);

    /* mmap memory and check pattern */
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, externalExportFdDesc.fd, 0);
    ASSERT_NE(ptr, MAP_FAILED) << "error " << errno;
    EXPECT_EQ(*static_cast<int32_t *>(ptr), pattern);
    EXPECT_EQ(munmap(ptr, size), 0);
    EXPECT_EQ(close(externalExportFdDesc.fd), 0);
}
