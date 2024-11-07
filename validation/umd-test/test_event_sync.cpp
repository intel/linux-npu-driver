/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <iostream>
#include <thread>

struct EventSync : public UmdTest {
    void SetUp() override {
        UmdTest::SetUp();

        ASSERT_EQ(createCommandQueue(ZE_COMMAND_QUEUE_PRIORITY_NORMAL, &cmdQueue),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(createCommandQueue(ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_HIGH, &cmdQueuePrior),
                  ZE_RESULT_SUCCESS);

        ASSERT_EQ(createCommandList(&cmdList), ZE_RESULT_SUCCESS);
        ASSERT_EQ(createCommandList(&cmdListPrior), ZE_RESULT_SUCCESS);

        ASSERT_EQ(createEventPool(3, &eventPool), ZE_RESULT_SUCCESS);
    }

    void TearDown() override { UmdTest::TearDown(); }

    ze_result_t createCommandQueue(ze_command_queue_priority_t priority,
                                   ze_command_queue_handle_t *handle) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        ze_command_queue_desc_t desc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                        .pNext = nullptr,
                                        .ordinal = 0u,
                                        .index = 0,
                                        .flags = 0,
                                        .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                        .priority = priority};
        auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, desc, ret);
        scopedQueues.push_back(std::move(scopedQueue));
        *handle = scopedQueues.back().get();
        return ret;
    }

    ze_result_t createCommandList(ze_command_list_handle_t *handle) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        ze_command_list_desc_t desc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                       .pNext = nullptr,
                                       .commandQueueGroupOrdinal = 0u,
                                       .flags = 0};
        auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, desc, ret);
        scopedLists.push_back(std::move(scopedList));
        *handle = scopedLists.back().get();
        return ret;
    }

    ze_result_t createEventPool(unsigned count, ze_event_pool_handle_t *handle) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        const ze_event_pool_desc_t eventPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                                    nullptr,
                                                    ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                                    count};
        auto scopedEventPool = zeScope::eventPoolCreate(zeContext, eventPoolDesc, 1, zeDevice, ret);
        scopedEventPools.push_back(std::move(scopedEventPool));
        *handle = scopedEventPools.back().get();
        return ret;
    }

    ze_result_t createEvent(ze_event_pool_handle_t &eventPoolHandle,
                            unsigned index,
                            ze_event_handle_t *handle) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                     nullptr,
                                     index,
                                     ZE_EVENT_SCOPE_FLAG_HOST,
                                     ZE_EVENT_SCOPE_FLAG_HOST};

        auto scopedEvent = zeScope::eventCreate(eventPool, eventDesc, ret);
        scopedEvents.push_back(std::move(scopedEvent));
        *handle = scopedEvents.back().get();
        return ret;
    }

    void queueWaitForSingalFromOtherQueue(bool testEventReset);

    ze_command_queue_handle_t cmdQueue = nullptr;
    ze_command_queue_handle_t cmdQueuePrior = nullptr;

    ze_command_list_handle_t cmdList = nullptr;
    ze_command_list_handle_t cmdListPrior = nullptr;

    ze_event_pool_handle_t eventPool = nullptr;

    static const size_t allocSize = PAGE_SIZE;
    static const uint8_t testPattern = 0xCD;

  private:
    std::vector<zeScope::SharedPtr<ze_command_queue_handle_t>> scopedQueues;
    std::vector<zeScope::SharedPtr<ze_command_list_handle_t>> scopedLists;
    std::vector<zeScope::SharedPtr<ze_event_pool_handle_t>> scopedEventPools;
    std::vector<zeScope::SharedPtr<ze_event_handle_t>> scopedEvents;
};

void EventSync::queueWaitForSingalFromOtherQueue(bool testEventReset) {
    ze_event_handle_t event = nullptr;
    ASSERT_EQ(createEvent(eventPool, 2, &event), ZE_RESULT_SUCCESS);

    std::shared_ptr<void> mem = AllocSharedMemory(allocSize);
    ASSERT_NE(mem, nullptr);

    std::shared_ptr<void> mem1 = AllocSharedMemory(allocSize);
    ASSERT_NE(mem1, nullptr);

    auto ts = static_cast<uint64_t *>(mem.get());
    auto ts1 = static_cast<uint64_t *>(mem1.get());

    ASSERT_EQ(zeCommandListAppendWaitOnEvents(cmdList, 1, &event), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(cmdList, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    if (testEventReset) {
        ASSERT_EQ(zeCommandListAppendEventReset(cmdList, event), ZE_RESULT_SUCCESS);
    }
    ASSERT_EQ(zeCommandListClose(cmdList), ZE_RESULT_SUCCESS);

    // Make sure the event is not signaled
    ASSERT_EQ(zeEventQueryStatus(event), ZE_RESULT_NOT_READY);

    // Execute command lists
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(cmdQueue, 1, &cmdList, nullptr), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(cmdListPrior, ts1, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendSignalEvent(cmdListPrior, event), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdListPrior), ZE_RESULT_SUCCESS);

    // Execute command lists
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(cmdQueuePrior, 1, &cmdListPrior, nullptr),
              ZE_RESULT_SUCCESS);

    // Command queue host sync
    ASSERT_EQ(zeCommandQueueSynchronize(cmdQueue, syncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(cmdQueuePrior, syncTimeout), ZE_RESULT_SUCCESS);

    // Verify timestamp results
    EXPECT_GT(*ts, *ts1) << "Timestamp relation mismatch from expected values";

    if (testEventReset) {
        EXPECT_EQ(zeEventQueryStatus(event), ZE_RESULT_NOT_READY);
    } else {
        EXPECT_EQ(zeEventQueryStatus(event), ZE_RESULT_SUCCESS);
    }
}

TEST_F(EventSync, ExecuteCommandListWithTimestampCommandThatWaitForHostSignalToComplete) {
    ze_event_handle_t event = nullptr;
    ASSERT_EQ(createEvent(eventPool, 2, &event), ZE_RESULT_SUCCESS);

    std::shared_ptr<void> mem = AllocSharedMemory(allocSize);
    ASSERT_NE(mem, nullptr);
    auto ts = static_cast<uint64_t *>(mem.get());

    ASSERT_EQ(zeCommandListAppendWaitOnEvents(cmdList, 1, &event), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(cmdList, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdList), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(cmdQueue, 1, &cmdList, nullptr), ZE_RESULT_SUCCESS);

    std::thread hostSignalThread([event] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        EXPECT_EQ(zeEventHostSignal(event), ZE_RESULT_SUCCESS);
    });

    EXPECT_EQ(zeCommandQueueSynchronize(cmdQueue, syncTimeout), ZE_RESULT_SUCCESS);
    hostSignalThread.join();

    EXPECT_EQ(zeEventQueryStatus(event), ZE_RESULT_SUCCESS);
    EXPECT_GT(*ts, 0u);
}

TEST_F(EventSync, ExecuteCommandListWithCopyCommandsThatWaitForHostSignalToComplete) {
    ze_event_handle_t event0 = nullptr;
    ASSERT_EQ(createEvent(eventPool, 0, &event0), ZE_RESULT_SUCCESS);

    ze_event_handle_t event1 = nullptr;
    ASSERT_EQ(createEvent(eventPool, 1, &event1), ZE_RESULT_SUCCESS);

    auto srcBuffMem = AllocHostMemory(allocSize);
    auto tempBuffMem = AllocDeviceMemory(allocSize);
    auto dstBuffMem = AllocHostMemory(allocSize);
    memset(srcBuffMem.get(), 0x01, allocSize);

    // Verify host reads event as unset
    EXPECT_EQ(zeEventHostSynchronize(event0, 0), ZE_RESULT_NOT_READY);

    // Execute and verify reads event
    ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdList,
                                            tempBuffMem.get(),
                                            srcBuffMem.get(),
                                            allocSize,
                                            event0,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdList,
                                            dstBuffMem.get(),
                                            tempBuffMem.get(),
                                            allocSize,
                                            nullptr,
                                            1,
                                            &event1),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdList), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(cmdQueue, 1, &cmdList, nullptr), ZE_RESULT_SUCCESS);

    // Verify copy waits for signal
    ASSERT_EQ(zeEventHostSynchronize(event0, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeEventQueryStatus(event1), ZE_RESULT_NOT_READY);

    EXPECT_NE(memcmp(srcBuffMem.get(), dstBuffMem.get(), allocSize), 0);

    EXPECT_EQ(zeEventHostSignal(event1), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(cmdQueue, syncTimeout), ZE_RESULT_SUCCESS);

    // Verify memory copy completed
    EXPECT_EQ(memcmp(srcBuffMem.get(), dstBuffMem.get(), allocSize), 0);
}

TEST_F(EventSync, QueueWaitForSignalFromOtherQueueWithPriority) {
    queueWaitForSingalFromOtherQueue(false);
}

TEST_F(EventSync, QueueWaitForSignalFromOtherQueueWithPriorityThenResetSignal) {
    queueWaitForSingalFromOtherQueue(true);
}

TEST_F(EventSync, QueueExecuteManyCommandListsThatWaitForEachOther) {
    ze_event_handle_t event0 = nullptr;
    ASSERT_EQ(createEvent(eventPool, 0, &event0), ZE_RESULT_SUCCESS);
    ze_event_handle_t event1 = nullptr;
    ASSERT_EQ(createEvent(eventPool, 1, &event1), ZE_RESULT_SUCCESS);
    ze_event_handle_t event2 = nullptr;
    ASSERT_EQ(createEvent(eventPool, 2, &event2), ZE_RESULT_SUCCESS);

    // Utilize the same shared memory test pattern
    std::shared_ptr<void> srcDeviceMem = AllocSharedMemory(allocSize);
    std::shared_ptr<void> destMem0 = AllocSharedMemory(allocSize);
    std::shared_ptr<void> destMem1 = AllocSharedMemory(allocSize);
    std::shared_ptr<void> destMem2 = AllocSharedMemory(allocSize);

    ASSERT_NE(srcDeviceMem, nullptr);
    ASSERT_NE(destMem0, nullptr);
    ASSERT_NE(destMem1, nullptr);
    ASSERT_NE(destMem2, nullptr);

    auto srcDevicePtr = static_cast<uint8_t *>(srcDeviceMem.get());
    auto destMem0Ptr = static_cast<uint8_t *>(destMem0.get());
    auto destMem1Ptr = static_cast<uint8_t *>(destMem1.get());
    auto destMem2Ptr = static_cast<uint8_t *>(destMem2.get());

    // Prepare source memory
    memset(srcDevicePtr, testPattern, allocSize);

    ze_command_list_handle_t cmdList0 = nullptr;
    ASSERT_EQ(createCommandList(&cmdList0), ZE_RESULT_SUCCESS);
    ze_command_list_handle_t cmdList1 = nullptr;
    ASSERT_EQ(createCommandList(&cmdList1), ZE_RESULT_SUCCESS);
    ze_command_list_handle_t cmdList2 = nullptr;
    ASSERT_EQ(createCommandList(&cmdList2), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendWaitOnEvents(cmdList0, 1, &event0), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdList0,
                                            destMem0Ptr,
                                            srcDevicePtr,
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendSignalEvent(cmdList0, event1), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdList0), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendWaitOnEvents(cmdList1, 1, &event1), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdList1,
                                            destMem1Ptr,
                                            srcDevicePtr,
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendSignalEvent(cmdList1, event2), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdList1), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendWaitOnEvents(cmdList2, 1, &event2), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdList2,
                                            destMem2Ptr,
                                            srcDevicePtr,
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdList2), ZE_RESULT_SUCCESS);

    // Events are not signaled yet
    ASSERT_EQ(zeEventQueryStatus(event0), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeEventQueryStatus(event1), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeEventQueryStatus(event2), ZE_RESULT_NOT_READY);

    // Execute command lists
    ze_command_list_handle_t cmdlists[] = {cmdList0, cmdList1, cmdList2};
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(cmdQueue, 3, cmdlists, nullptr), ZE_RESULT_SUCCESS);

    // Signal from host
    ASSERT_EQ(zeEventHostSignal(event0), ZE_RESULT_SUCCESS);

    // Wait to be synchronized
    ASSERT_EQ(zeCommandQueueSynchronize(cmdQueue, syncTimeout), ZE_RESULT_SUCCESS);

    // Expect all events gets signaled
    EXPECT_EQ(zeEventQueryStatus(event0), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeEventQueryStatus(event1), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeEventQueryStatus(event2), ZE_RESULT_SUCCESS);

    // Expect copy command was ran by VPU
    EXPECT_EQ(memcmp(srcDevicePtr, destMem0Ptr, allocSize), 0)
        << "Command list 0 didn't run properly.";
    EXPECT_EQ(memcmp(srcDevicePtr, destMem1Ptr, allocSize), 0)
        << "Command list 1 didn't run properly.";
    EXPECT_EQ(memcmp(srcDevicePtr, destMem2Ptr, allocSize), 0)
        << "Command list 2 didn't run properly.";
}

TEST_F(EventSync, UseEventHostSynchronizeForCommandListCompletion) {
    ze_event_handle_t event = nullptr;
    ASSERT_EQ(createEvent(eventPool, 0, &event), ZE_RESULT_SUCCESS);

    std::shared_ptr<void> mem = AllocSharedMemory(allocSize);
    ASSERT_NE(mem, nullptr);
    auto ts = static_cast<uint64_t *>(mem.get());

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(cmdList, ts, event, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdList), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(cmdQueue, 1, &cmdList, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeEventHostSynchronize(event, syncTimeout), ZE_RESULT_SUCCESS);

    EXPECT_GT(*ts, 0u);

    EXPECT_EQ(zeCommandQueueSynchronize(cmdQueue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeEventQueryStatus(event), ZE_RESULT_SUCCESS);
}

TEST_F(EventSync, AppendGlobalTimestampAndSynchronizeWithEvent) {
    ze_event_handle_t event0 = nullptr;
    ASSERT_EQ(createEvent(eventPool, 0, &event0), ZE_RESULT_SUCCESS);
    ze_event_handle_t event1 = nullptr;
    ASSERT_EQ(createEvent(eventPool, 1, &event1), ZE_RESULT_SUCCESS);
    ze_event_handle_t event2 = nullptr;
    ASSERT_EQ(createEvent(eventPool, 2, &event2), ZE_RESULT_SUCCESS);

    const size_t size = sizeof(uint64_t);
    auto mem = AllocSharedMemory(size);
    ze_event_handle_t waitEvents[] = {event0, event1};
    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_TRUE(ts) << "Failed to allocate shared memory";

    // Execute TS command with waiting event0 and event1, event2 should be signaled after the
    // completion
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(cmdList, ts, event2, 2, waitEvents),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdList), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeEventQueryStatus(event0), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeEventQueryStatus(event1), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeEventQueryStatus(event2), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(cmdQueue, 1, &cmdList, nullptr), ZE_RESULT_SUCCESS);

    // Signal waiting events from host
    ASSERT_EQ(zeEventHostSignal(event0), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventHostSignal(event1), ZE_RESULT_SUCCESS);

    // Sync with the event
    EXPECT_EQ(zeEventHostSynchronize(event2, syncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventQueryStatus(event0), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventQueryStatus(event1), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventQueryStatus(event2), ZE_RESULT_SUCCESS);

    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}

TEST_F(EventSync, AppendCopyAndBarrierThenSynchronizeWithEvent) {
    ze_event_handle_t event0 = nullptr;
    ASSERT_EQ(createEvent(eventPool, 0, &event0), ZE_RESULT_SUCCESS);
    ze_event_handle_t event1 = nullptr;
    ASSERT_EQ(createEvent(eventPool, 1, &event1), ZE_RESULT_SUCCESS);
    ze_event_handle_t event2 = nullptr;
    ASSERT_EQ(createEvent(eventPool, 2, &event2), ZE_RESULT_SUCCESS);

    ze_event_handle_t waitEvents[] = {event0, event1};
    const size_t allocSize = PAGE_SIZE;
    auto srcMem = AllocHostMemory(allocSize);
    auto destMem = AllocSharedMemory(allocSize);
    ASSERT_TRUE(srcMem) << "Failed to allocate source memory";
    ASSERT_TRUE(destMem) << "Failed to allocate destination memory";
    memset(srcMem.get(), 0xCD, allocSize);

    ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdList,
                                            destMem.get(),
                                            srcMem.get(),
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendBarrier(cmdList, event2, 2, waitEvents), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdList), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeEventQueryStatus(event0), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeEventQueryStatus(event1), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeEventQueryStatus(event2), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(cmdQueue, 1, &cmdList, nullptr), ZE_RESULT_SUCCESS);

    // Signal waiting events from host
    ASSERT_EQ(zeEventHostSignal(event0), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventHostSignal(event1), ZE_RESULT_SUCCESS);

    // Sync with the event
    EXPECT_EQ(zeEventHostSynchronize(event2, syncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventQueryStatus(event0), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventQueryStatus(event1), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventQueryStatus(event2), ZE_RESULT_SUCCESS);

    EXPECT_EQ(memcmp(destMem.get(), srcMem.get(), allocSize), 0) << "Memory copy failed.";
}
