/*
 * Copyright (C) 2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"
#include "umd_prime_buffers.h"

#include <vector>
#include <memory>
#include <thread>

class PrimeBuffers : public UmdTest, public ::testing::WithParamInterface<uint64_t> {
  public:
    void TearDown() override { UmdTest::TearDown(); }
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

    PrimeBufferHelper primeHelper;
};

INSTANTIATE_TEST_SUITE_P(Sizes,
                         PrimeBuffers,
                         ::testing::Values(2 * KB, 16 * MB, 255 * MB),
                         [](const testing::TestParamInfo<uint64_t> &cmd) {
                             return memSizeToStr(cmd.param);
                         });

TEST_P(PrimeBuffers, exportDeviceMemory) {
    ze_result_t ret;
    uint64_t size = GetParam();

    /* Pepare request for external allocation */
    pDeviceMemAllocDesc.pNext = &primeHelper.externalExportDesc;

    auto scopedMem =
        zeScope::memAllocDevice(zeContext, pDeviceMemAllocDesc, size, 0, zeDevice, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_NE(scopedMem.get(), nullptr);
    const int32_t pattern = 0xDEADAABB;
    memcpy(scopedMem.get(), &pattern, sizeof(pattern));

    /* Prepare request for map allocation to fd */
    pMemAllocProperties.pNext = &primeHelper.externalExportFdDesc;
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeMemGetAllocProperties(zeContext, scopedMem.get(), &pMemAllocProperties, nullptr));
    ASSERT_GE(primeHelper.externalExportFdDesc.fd, 0);
    EXPECT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_DEVICE);
    EXPECT_EQ(pMemAllocProperties.id, 0u);
    EXPECT_EQ(pMemAllocProperties.pageSize, size);
    EXPECT_EQ(lseek(primeHelper.externalExportFdDesc.fd, 0, SEEK_END), ALLIGN_TO_PAGE(size));
    lseek(primeHelper.externalExportFdDesc.fd, 0, SEEK_CUR);

    /* mmap memory and check pattern */
    void *ptr = mmap(NULL,
                     size,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     primeHelper.externalExportFdDesc.fd,
                     0);
    ASSERT_NE(ptr, MAP_FAILED) << "error " << errno;
    EXPECT_EQ(*static_cast<int32_t *>(ptr), pattern);
    EXPECT_EQ(munmap(ptr, size), 0);
    EXPECT_EQ(close(primeHelper.externalExportFdDesc.fd), 0);
}

TEST_P(PrimeBuffers, exportHostMemory) {
    ze_result_t ret;
    uint64_t size = GetParam();

    /* Pepare request for external allocation */
    pHostMemAllocDesc.pNext = &primeHelper.externalExportDesc;

    auto scopedMem = zeScope::memAllocHost(zeContext, pHostMemAllocDesc, size, 0, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_NE(scopedMem.get(), nullptr);
    const int32_t pattern = 0xDEADAABB;
    memcpy(scopedMem.get(), &pattern, sizeof(pattern));

    /* Prepare request for map allocation to fd */
    pMemAllocProperties.pNext = &primeHelper.externalExportFdDesc;
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeMemGetAllocProperties(zeContext, scopedMem.get(), &pMemAllocProperties, nullptr));
    ASSERT_GE(primeHelper.externalExportFdDesc.fd, 0);
    EXPECT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_HOST);
    EXPECT_EQ(pMemAllocProperties.id, 0u);
    EXPECT_EQ(pMemAllocProperties.pageSize, size);
    EXPECT_EQ(lseek(primeHelper.externalExportFdDesc.fd, 0, SEEK_END), ALLIGN_TO_PAGE(size));
    lseek(primeHelper.externalExportFdDesc.fd, 0, SEEK_CUR);

    /* mmap memory and check pattern */
    void *ptr = mmap(NULL,
                     size,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     primeHelper.externalExportFdDesc.fd,
                     0);
    ASSERT_NE(ptr, MAP_FAILED) << "error " << errno;
    EXPECT_EQ(*static_cast<int32_t *>(ptr), pattern);
    EXPECT_EQ(munmap(ptr, size), 0);
    EXPECT_EQ(close(primeHelper.externalExportFdDesc.fd), 0);
}

TEST_P(PrimeBuffers, exportSharedMemory) {
    ze_result_t ret;
    uint64_t size = GetParam();

    /* Pepare request for external allocation */
    pDeviceMemAllocDesc.pNext = &primeHelper.externalExportDesc;

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
    pMemAllocProperties.pNext = &primeHelper.externalExportFdDesc;
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeMemGetAllocProperties(zeContext, scopedMem.get(), &pMemAllocProperties, nullptr));
    ASSERT_GE(primeHelper.externalExportFdDesc.fd, 0);
    EXPECT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_SHARED);
    EXPECT_EQ(pMemAllocProperties.id, 0u);
    EXPECT_EQ(pMemAllocProperties.pageSize, size);
    EXPECT_EQ(lseek(primeHelper.externalExportFdDesc.fd, 0, SEEK_END), ALLIGN_TO_PAGE(size));
    lseek(primeHelper.externalExportFdDesc.fd, 0, SEEK_CUR);

    /* mmap memory and check pattern */
    void *ptr = mmap(NULL,
                     size,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     primeHelper.externalExportFdDesc.fd,
                     0);
    ASSERT_NE(ptr, MAP_FAILED) << "error " << errno;
    EXPECT_EQ(*static_cast<int32_t *>(ptr), pattern);
    EXPECT_EQ(munmap(ptr, size), 0);
    EXPECT_EQ(close(primeHelper.externalExportFdDesc.fd), 0);
}

TEST_P(PrimeBuffers, importDeviceMemory) {
    ze_result_t ret;
    size_t dmaBufferSize = GetParam();
    int32_t dmaBufferFd = -1;

    if (!primeHelper.hasDMABufferSupport())
        GTEST_SKIP() << "Missed support or insufficient permissions for"
                     << " dma buffer allocation in the system. Skip test";

    ASSERT_TRUE(primeHelper.createDMABuffer(dmaBufferSize, dmaBufferFd));

    /* Prepare request to import dma buffer as device memory */
    primeHelper.externalImportFromFdDesc.fd = dmaBufferFd;
    pDeviceMemAllocDesc.pNext = &primeHelper.externalImportFromFdDesc;

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
    ASSERT_EQ(pMemAllocProperties.id, 0u);
    ASSERT_EQ(pMemAllocProperties.pageSize, ALLIGN_TO_PAGE(dmaBufferSize));

    /* mmap original dma buffer and write pattern to it */
    void *dmaBufferPtr = primeHelper.mmapDmaBuffer(dmaBufferFd);
    ASSERT_NE(dmaBufferPtr, MAP_FAILED) << "error " << errno;
    const int32_t pattern = 0xDEADAABB;
    memcpy(dmaBufferPtr, &pattern, sizeof(pattern));

    /*check if pattern match on imported buffer*/
    int32_t *importedPtr = static_cast<int32_t *>(scopedImportedMemory.get());
    EXPECT_EQ(*importedPtr, pattern);
}
