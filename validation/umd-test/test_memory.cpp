/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <memory>
#include <thread>
#include <vector>

class MemoryAllocation : public UmdTest, public ::testing::WithParamInterface<uint64_t> {
  public:
    void SetUp() override {
        UmdTest::SetUp();
        pMemAllocProperties.stype = ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES;
    }

    void TearDown() override { UmdTest::TearDown(); }

    void *mem = nullptr;

    size_t size = 10;
    void *ptr = nullptr;

    size_t pSize = 0;
    void *basePtr = nullptr;

    ze_memory_allocation_properties_t pMemAllocProperties = {};
};

INSTANTIATE_TEST_SUITE_P(Sizes,
                         MemoryAllocation,
                         ::testing::Values(2 * KB, // force line break
                                           16 * MB,
                                           64 * MB,
                                           128 * MB,
                                           255 * MB),
                         [](const testing::TestParamInfo<uint64_t> &cmd) {
                             return memSizeToStr(cmd.param);
                         });

TEST_P(MemoryAllocation, AllocDeviceMemory) {
    uint64_t size = GetParam();
    auto mem = AllocDeviceMemory(size);
    ASSERT_TRUE(mem.get()) << "Failed to allocate device memory using size " << size;
}

TEST_P(MemoryAllocation, AllocHostMemory) {
    uint64_t size = GetParam();
    auto mem = AllocHostMemory(size);
    ASSERT_TRUE(mem.get()) << "Failed to allocate host memory using size " << size;
}

TEST_F(MemoryAllocation, QueryContextMemory) {
    uint64_t size = 48 * KB;
    auto mem = AllocHostMemory(size);
    ASSERT_TRUE(mem.get());

    ze_graph_memory_query_t memQuery = {};
    EXPECT_EQ(
        zeGraphDDITableExt->pfnQueryContextMemory(zeContext, ZE_GRAPH_QUERY_MEMORY_DDR, &memQuery),
        ZE_RESULT_SUCCESS);
    EXPECT_EQ(memQuery.allocated, size);

    TRACE("MemoryQuery.allocated: %lu\n", memQuery.allocated);
    TRACE("MemoryQuery.total: %lu\n", memQuery.total);
}

TEST_F(MemoryAllocation, GetMemAddressRangeWithoutMemInputExpectSuccess) {
    auto mem = AllocHostMemory(size);
    ASSERT_NE(nullptr, ptr = mem.get());

    ASSERT_EQ(
        ZE_RESULT_SUCCESS,
        zeMemGetAddressRange(zeContext, static_cast<char *>(ptr) + size - 1, nullptr, nullptr));
    EXPECT_NE(ptr, basePtr);
    EXPECT_NE(size, pSize);
}

TEST_F(MemoryAllocation, GetMemAddressRangeWithMemInputExpectSuccess) {
    auto mem = AllocHostMemory(size);
    EXPECT_NE(nullptr, ptr = mem.get());

    ASSERT_EQ(
        ZE_RESULT_SUCCESS,
        zeMemGetAddressRange(zeContext, static_cast<char *>(ptr) + size - 1, &basePtr, &pSize));
    EXPECT_EQ(ptr, basePtr);
    EXPECT_EQ(size, pSize);
}

TEST_F(MemoryAllocation, GetMemPropertiesWithAllocHostMemExpectSuccess) {
    auto mem = AllocHostMemory(size);
    ASSERT_NE(nullptr, ptr = mem.get());

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeMemGetAllocProperties(zeContext,
                                      static_cast<char *>(ptr) + size - 1,
                                      &pMemAllocProperties,
                                      nullptr));
    EXPECT_EQ(pMemAllocProperties.stype, ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES);
    EXPECT_EQ(pMemAllocProperties.pNext, nullptr);
    EXPECT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_HOST);
    EXPECT_GT(pMemAllocProperties.id, 0u);
    EXPECT_EQ(pMemAllocProperties.pageSize, size);
}

TEST_F(MemoryAllocation, GetMemPropertiesWithAllocDeviceMemExpectSuccess) {
    auto mem = AllocDeviceMemory(size);
    ASSERT_NE(nullptr, ptr = mem.get());

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeMemGetAllocProperties(zeContext,
                                      static_cast<char *>(ptr) + size - 1,
                                      &pMemAllocProperties,
                                      nullptr));
    EXPECT_EQ(pMemAllocProperties.stype, ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES);
    EXPECT_EQ(pMemAllocProperties.pNext, nullptr);
    EXPECT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_DEVICE);
    EXPECT_GT(pMemAllocProperties.id, 0u);
    EXPECT_EQ(pMemAllocProperties.pageSize, size);
}

class MemoryExecution : public MemoryAllocation {
  public:
    ze_command_queue_desc_t cmdQueueDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                         .pNext = nullptr,
                                         .ordinal = 0,
                                         .index = 0,
                                         .flags = 0,
                                         .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                         .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};
    ze_command_list_desc_t cmdListDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                          .pNext = nullptr,
                                          .commandQueueGroupOrdinal = 0,
                                          .flags = 0};

    void SetUp() override {
        MemoryAllocation::SetUp();

        ze_result_t ret;
        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();
    }

    void TearDown() override { MemoryAllocation::TearDown(); }

    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
};

INSTANTIATE_TEST_SUITE_P(Sizes,
                         MemoryExecution,
                         ::testing::Values(2 * KB, // force line break
                                           16 * MB,
                                           63 * MB, // -1MB to save space for command buffer
                                           126 * MB // -1MB to save space for command buffer
                                           ),
                         [](const testing::TestParamInfo<uint64_t> &cmd) {
                             return memSizeToStr(cmd.param);
                         });

TEST_P(MemoryExecution, ExecuteTimestampCommandInMemoryLowRange) {
    size_t size = GetParam();

    auto mem = AllocSharedMemory(size);
    ASSERT_TRUE(mem.get()) << "Failed to allocate shared memory";

    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}

TEST_P(MemoryExecution, ExecuteCopyCommandInMemoryLowRange) {
    size_t size = GetParam();

    auto mem = AllocSharedMemory(size);
    ASSERT_TRUE(mem.get()) << "Failed to allocate shared memory";

    uint64_t *copyDst = static_cast<uint64_t *>(mem.get()) + 64;
    uint64_t *copySrc = static_cast<uint64_t *>(mem.get());

    *copySrc = 0xdeadbeef;

    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            copyDst,
                                            copySrc,
                                            sizeof(uint64_t),
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(*copySrc, *copyDst) << "Value after copy should be equal";
}

TEST_P(MemoryExecution, ExecuteCopyCommandInMemoryHighRange) {
    size_t size = GetParam();

    auto mem = AllocHostMemory(size);
    ASSERT_TRUE(mem.get()) << "Failed to allocate shared memory";

    uint64_t *copyDst = static_cast<uint64_t *>(mem.get()) + 64;
    uint64_t *copySrc = static_cast<uint64_t *>(mem.get());

    *copySrc = 0xdeadbeef;

    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            copyDst,
                                            copySrc,
                                            sizeof(uint64_t),
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(*copySrc, *copyDst) << "Value after copy should be equal";
}

// TODO: Allow copy from user pointer, EISW-19284
TEST_F(MemoryExecution, DISABLED_CopyingFromUnpinnedHostMemoryShouldBeAllowed) {
    size_t size = 4 * 1024;

    // Unpinned host memory.
    std::vector<uint8_t> hostMem(size);
    auto destDevMem = AllocSharedMemory(size);
    memset(hostMem.data(), 0xCD, size);

    EXPECT_EQ(zeCommandListAppendMemoryCopy(list,
                                            destDevMem.get(),
                                            hostMem.data(),
                                            size,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS)
        << "Failed to pin memory using size " << size;
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(memcmp(destDevMem.get(), hostMem.data(), size), 0);
}

TEST_F(MemoryExecution, MultipleCopies) {
    const size_t size = 1024;
    const size_t numIter = 4;

    auto hostMem = AllocHostMemory(size * (numIter + 1));
    auto devMem = AllocSharedMemory(size * (numIter + 1));

    memset(hostMem.get(), 0xAB, size);

    uint8_t *hostPtr = static_cast<uint8_t *>(hostMem.get());
    uint8_t *devPtr = static_cast<uint8_t *>(devMem.get());
    void *dst = nullptr;
    void *src = nullptr;
    for (size_t i = 0; i < (numIter * 3); i++) {
        if (i % 3 == 0) {
            src = hostPtr;
            dst = devPtr;
        } else if (i % 3 == 1) {
            src = devPtr;
            devPtr += 1024;
            dst = devPtr;
        } else {
            src = devPtr;
            hostPtr += 1024;
            dst = hostPtr;
        }
        ASSERT_EQ(zeCommandListAppendMemoryCopy(list, dst, src, size, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
    }

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);

    EXPECT_EQ(memcmp(hostMem.get(), static_cast<uint8_t *>(hostMem.get()) + size * numIter, size),
              0);
}

TEST_F(MemoryExecution, CheckQueryContextMemory) {
    ze_graph_memory_query_t memAllocated = {};
    size_t testAllocSize = 20000;

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeGraphDDITableExt->pfnQueryContextMemory(zeContext,
                                                        ZE_GRAPH_QUERY_MEMORY_DDR,
                                                        &memAllocated));
    size_t initialMem = memAllocated.allocated;

    auto mem = AllocHostMemory(testAllocSize);
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeGraphDDITableExt->pfnQueryContextMemory(zeContext,
                                                        ZE_GRAPH_QUERY_MEMORY_DDR,
                                                        &memAllocated));
    size_t userAllocatedMem = memAllocated.allocated;
    EXPECT_EQ(userAllocatedMem, initialMem + testAllocSize);

    ASSERT_EQ(zeCommandListAppendBarrier(list, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeGraphDDITableExt->pfnQueryContextMemory(zeContext,
                                                        ZE_GRAPH_QUERY_MEMORY_DDR,
                                                        &memAllocated));

    /* Expected some internal allocations for command buffer*/
    EXPECT_GT(memAllocated.allocated, userAllocatedMem);
}

class MemoryAllocationThreaded : public UmdTest {
  public:
    void SetUp() override {
        mem.resize(numOfThreadedTasks);
        UmdTest::SetUp();
    }
    void TearDown() override { UmdTest::TearDown(); }

    ze_host_mem_alloc_desc_t hostMemAllocDesc = {.stype = ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC,
                                                 .pNext = nullptr,
                                                 .flags = 0};

    ze_device_mem_alloc_desc_t deviceMemAllocDesc = {.stype =
                                                         ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
                                                     .pNext = nullptr,
                                                     .flags = 0,
                                                     .ordinal = 0};

    void allocHostMem(void **ptr) {
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeMemAllocHost(zeContext, &hostMemAllocDesc, size, 0, ptr));
    }

    void allocDeviceMem(void **ptr) {
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeMemAllocDevice(zeContext, &deviceMemAllocDesc, size, 0, zeDevice, ptr));
    }

    void freeMem(void *ptr) { ASSERT_EQ(ZE_RESULT_SUCCESS, zeMemFree(zeContext, ptr)); }

    uint16_t numOfThreadedTasks = 32;
    std::vector<std::unique_ptr<std::thread>> tasks;
    std::vector<void *> mem;
    uint64_t size = 8192;
};

TEST_F(MemoryAllocationThreaded, HostMem) {
    for (auto &m : mem) {
        tasks.push_back(
            std::make_unique<std::thread>(&MemoryAllocationThreaded::allocHostMem, this, &m));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
    for (const auto &m : mem) {
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeMemFree(zeContext, m));
    }
}

TEST_F(MemoryAllocationThreaded, DeviceMem) {
    for (auto &m : mem) {
        tasks.push_back(
            std::make_unique<std::thread>(&MemoryAllocationThreaded::allocDeviceMem, this, &m));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
    for (const auto &m : mem) {
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeMemFree(zeContext, m));
    }
}

TEST_F(MemoryAllocationThreaded, FreeMem) {
    for (auto &m : mem) {
        allocDeviceMem(&m);
    }
    for (auto &m : mem) {
        tasks.push_back(std::make_unique<std::thread>(&MemoryAllocationThreaded::freeMem, this, m));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
}
