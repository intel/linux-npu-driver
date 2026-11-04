/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <future>
#include <ze_ddi.h>

struct Event : public UmdTest {
    void SetUp() override {
        UmdTest::SetUp();

        const ze_event_pool_desc_t eventPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                                    nullptr,
                                                    ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                                    Event::eventPoolCap};
        ze_result_t ret;
        scopedEventPool = zeScope::eventPoolCreate(zeContext, eventPoolDesc, 1, zeDevice, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        eventPool = scopedEventPool.get();
    }

    void TearDown() override { UmdTest::TearDown(); }

    ze_event_pool_handle_t eventPool = nullptr;
    // Size of event pool.
    static const uint32_t eventPoolCap = 3;

  private:
    zeScope::SharedPtr<ze_event_pool_handle_t> scopedEventPool = nullptr;
};

TEST_F(Event, CreateEventWithInvalidParameters) {
    ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 nullptr,
                                 0,
                                 ZE_EVENT_SCOPE_FLAG_HOST,
                                 ZE_EVENT_SCOPE_FLAG_HOST};
    ze_event_handle_t event = nullptr;

    EXPECT_EQ(zeEventCreate(eventPool, nullptr, &event), ZE_RESULT_ERROR_INVALID_NULL_POINTER);
    EXPECT_EQ(zeEventCreate(eventPool, &eventDesc, nullptr), ZE_RESULT_ERROR_INVALID_NULL_POINTER);
    EXPECT_EQ(zeEventCreate(eventPool, nullptr, nullptr), ZE_RESULT_ERROR_INVALID_NULL_POINTER);
}

TEST_F(Event, AllocateEventTwiceExpectError) {
    ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 nullptr,
                                 0,
                                 ZE_EVENT_SCOPE_FLAG_HOST,
                                 ZE_EVENT_SCOPE_FLAG_HOST};
    ze_result_t ret;

    auto scopedEvent = zeScope::eventCreate(eventPool, eventDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);

    auto expectNullptr = zeScope::eventCreate(eventPool, eventDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_ERROR_INVALID_ARGUMENT);
    EXPECT_EQ(expectNullptr, nullptr);
}

TEST_F(Event, CreateEventShouldBeSuccessful) {
    ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 nullptr,
                                 0,
                                 ZE_EVENT_SCOPE_FLAG_HOST,
                                 ZE_EVENT_SCOPE_FLAG_HOST};
    ze_result_t ret;

    // Event index 0.
    auto scopedEvent0 = zeScope::eventCreate(eventPool, eventDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);

    eventDesc.index = 1;
    auto scopedEvent1 = zeScope::eventCreate(eventPool, eventDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);

    // Event index 2.
    eventDesc.index = 2;
    auto scopedEvent2 = zeScope::eventCreate(eventPool, eventDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);
}

/*CounterBasedEvents testing */

struct CounterBasedEvent : public UmdTest {
    ze_event_counter_based_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_COUNTER_BASED_DESC,
                                               nullptr,
                                               ZE_EVENT_COUNTER_BASED_FLAG_NON_IMMEDIATE,
                                               ZE_EVENT_SCOPE_FLAG_HOST,
                                               ZE_EVENT_SCOPE_FLAG_HOST};
    ze_command_queue_desc_t cmdQueueDesc = {ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                            nullptr,
                                            0,
                                            0,
                                            0,
                                            ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS,
                                            ZE_COMMAND_QUEUE_PRIORITY_NORMAL};
    ze_command_list_desc_t cmdListDesc = {ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC, nullptr, 0, 0};

    void SetUp() override {
        UmdTest::SetUp();
        SKIP_NO_COUNTER_BASED_EVENTS();
    }
    void TearDown() override { UmdTest::TearDown(); }
};

TEST_F(CounterBasedEvent, CreateCountedEventShouldBeSuccessful) {
    ze_event_handle_t event = nullptr;

    ASSERT_EQ(zeEventCounterBasedCreate(zeContext, zeDevice, &eventDesc, &event),
              ZE_RESULT_SUCCESS);
    ASSERT_NE(event, nullptr);
    // Verify if created in signalled state
    EXPECT_EQ(zeEventQueryStatus(event), ZE_RESULT_SUCCESS);
    // Verify that counted event synchronize with host is successful
    EXPECT_EQ(zeEventHostSynchronize(event, syncTimeout), ZE_RESULT_SUCCESS);
    // HostSignal is not allowed for counted events, so it should return not available
    EXPECT_EQ(zeEventHostSignal(event), ZE_RESULT_ERROR_NOT_AVAILABLE);
    // HostReset is not allowed for counted events, so it should return not available
    EXPECT_EQ(zeEventHostReset(event), ZE_RESULT_ERROR_NOT_AVAILABLE);

    // Get device and host address of the counted event
    uint64_t completionValue = UINT64_MAX;
    uint64_t deviceAddress = 0;
    EXPECT_EQ(zeEventCounterBasedGetDeviceAddress(event, &completionValue, &deviceAddress),
              ZE_RESULT_SUCCESS);
    EXPECT_NE(deviceAddress, 0);

    // Get creation flags of the counted event
    ze_event_counter_based_flags_t creationFlags = 0;
    EXPECT_EQ(zeEventGetCounterBasedFlags(event, &creationFlags), ZE_RESULT_SUCCESS);
    EXPECT_EQ(creationFlags, eventDesc.flags);
    // Destroy the counted event
    EXPECT_EQ(zeEventDestroy(event), ZE_RESULT_SUCCESS);
}

TEST_F(CounterBasedEvent, CreateCountedEventWithInvalidParameters) {
    ze_event_handle_t event = nullptr;

    EXPECT_EQ(zeEventCounterBasedCreate(zeContext, nullptr, &eventDesc, &event),
              ZE_RESULT_ERROR_INVALID_NULL_HANDLE);
    EXPECT_EQ(zeEventCounterBasedCreate(zeContext, zeDevice, nullptr, &event),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);
    EXPECT_EQ(zeEventCounterBasedCreate(zeContext, zeDevice, &eventDesc, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);
}

TEST_F(CounterBasedEvent, CheckHostSynchronize) {
    // 1. Create Counted Event.
    ze_result_t ret;

    auto scopedEvent = zeScope::counterBasedEventCreate(zeContext, zeDevice, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event = scopedEvent.get();
    ASSERT_NE(event, nullptr);

    // 3. Create two command queues
    auto scopedQueue1 = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto queue1 = scopedQueue1.get();

    auto scopedQueue2 = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto queue2 = scopedQueue2.get();

    // 2. Create two commandlists (one is signalling, the other is waiting) and append timestamp
    // command to each commandlist.

    auto scopedList1 = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto listSignal = scopedList1.get();

    auto scopedList2 = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto listWait = scopedList2.get();

    auto mem1 = zeMemory::allocHost(zeContext, sizeof(uint64_t));
    auto mem2 = zeMemory::allocHost(zeContext, sizeof(uint64_t));
    uint64_t *tsFromWait = static_cast<uint64_t *>(mem1.get());
    uint64_t *tsFromSignal = static_cast<uint64_t *>(mem2.get());

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(listSignal, tsFromSignal, event, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(listSignal), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(listWait, tsFromWait, event, 1, &event),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(listWait), ZE_RESULT_SUCCESS);

    // 4. Execute created command list on these command queues, one command list on each
    // command queue.
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue2, 1, &listWait, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeEventQueryStatus(event), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue1, 1, &listSignal, nullptr),
              ZE_RESULT_SUCCESS);

    // 5. Synchronize by zeEventHostSynchronize.
    EXPECT_EQ(zeEventHostSynchronize(event, syncTimeout), ZE_RESULT_SUCCESS);
    // Both command queues should be synchronized after event is signalled, so synchronize both
    // command queues to verify that they are synchronized.
    EXPECT_EQ(zeCommandQueueSynchronize(queue1, 0), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeCommandQueueSynchronize(queue2, 0), ZE_RESULT_SUCCESS);

    // 6. Verify the timestamp value is updated in the host memory after synchronization.
    EXPECT_NE(*tsFromSignal, 0llu) << "Timestamp for the signalling command list should not be 0";
    EXPECT_NE(*tsFromWait, 0llu) << "Timestamp for the waiting command list should not be 0";
    EXPECT_GT(*tsFromWait, *tsFromSignal) << "Timestamp for the waiting command list should be "
                                             "greater than the signalling command list";
}

TEST_F(CounterBasedEvent, DestroyEventAndSynchronizeCommandQueues) {
    ze_result_t ret;
    // 1. Create Counted Event.
    auto scopedEvent = zeScope::counterBasedEventCreate(zeContext, zeDevice, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event = scopedEvent.get();
    ASSERT_NE(event, nullptr);

    // 2. Create two command queues
    auto scopedQueue1 = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto queue1 = scopedQueue1.get();

    auto scopedQueue2 = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto queue2 = scopedQueue2.get();

    // 3. Create two commandlists(one is signalling, the other is waiting)
    auto scopedList1 = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto listSignal = scopedList1.get();

    auto scopedList2 = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto listWait = scopedList2.get();

    auto mem1 = zeMemory::allocHost(zeContext, sizeof(uint64_t));
    auto mem2 = zeMemory::allocHost(zeContext, sizeof(uint64_t));
    uint64_t *tsFromWait = static_cast<uint64_t *>(mem1.get());
    uint64_t *tsFromSignal = static_cast<uint64_t *>(mem2.get());

    // 4.Verify that event is created in signalled state and does not block the command queue
    // execution.
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(listWait, tsFromWait, nullptr, 1, &event),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(listWait), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue2, 1, &listWait, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue2, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*tsFromWait, 0llu) << "Timestamp for the waiting command list should not be 0";
    *tsFromWait = 0llu;
    zeCommandListReset(listWait);

    // 5. Now append timestamp command to both lists and event signal to the signalling command list
    // and event wait to the waiting command list.
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(listSignal, tsFromSignal, event, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(listSignal), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(listWait, tsFromWait, nullptr, 1, &event),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(listWait), ZE_RESULT_SUCCESS);

    // 4.Event is no longer needed, destroy it to test if command queues can still synchronize
    // correctly.
    scopedEvent.reset();

    // 5. Execute created command list on these command queues, one command list on each
    // command queue.
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue2, 1, &listWait, nullptr), ZE_RESULT_SUCCESS);
    // Sleep to ensure that the command list is executed, and reach fence
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue1, 1, &listSignal, nullptr),
              ZE_RESULT_SUCCESS);

    // 5. Synchronize only waiting on event command queue.
    ASSERT_EQ(zeCommandQueueSynchronize(queue2, syncTimeout), ZE_RESULT_SUCCESS);
    // 6. Verify the timestamp value is updated in the host memory after synchronization.
    EXPECT_NE(*tsFromSignal, 0llu) << "Timestamp for the signalling command list should not be 0";
    EXPECT_NE(*tsFromWait, 0llu) << "Timestamp for the waiting command list should not be 0";
    EXPECT_GT(*tsFromWait, *tsFromSignal) << "Timestamp for the waiting command list should be "
                                             "greater than the signalling command list";
}

TEST_F(CounterBasedEvent, UseCounterBasedEventWithMetricStreamerExpectFailed) {
    const uint32_t samplingPeriodMs = 10;
    const uint32_t nReportsNotification = 20;

    zet_metric_streamer_desc_t metricStreamerDesc = {
        .stype = ZET_STRUCTURE_TYPE_METRIC_STREAMER_DESC,
        .pNext = nullptr,
        .notifyEveryNReports = nReportsNotification,
        .samplingPeriod = samplingPeriodMs * 1'000'000, // [ns]
    };

    uint32_t count = 0;
    ze_result_t ret = setupMetrics();
    if (ret == ZE_RESULT_ERROR_UNSUPPORTED_FEATURE) {
        SKIP_("Metrics are not supported");
    }

    EXPECT_EQ(zetMetricGroupGet(zeDevice, &count, nullptr), ZE_RESULT_SUCCESS);

    std::vector<zet_metric_group_handle_t> groups(count);
    EXPECT_EQ(zetMetricGroupGet(zeDevice, &count, groups.data()), ZE_RESULT_SUCCESS);
    EXPECT_GT(count, 0) << "No metric groups found on the device";

    ze_event_handle_t hEvent = nullptr;
    EXPECT_EQ(zeEventCounterBasedCreate(zeContext, zeDevice, &eventDesc, &hEvent),
              ZE_RESULT_SUCCESS);
    EXPECT_NE(hEvent, nullptr);
    zet_metric_streamer_handle_t hMetricStreamer = nullptr;
    EXPECT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 1, &groups[0]),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(zetMetricStreamerOpen(zeContext,
                                    zeDevice,
                                    groups[0],
                                    &metricStreamerDesc,
                                    hEvent,
                                    &hMetricStreamer),
              ZE_RESULT_ERROR_INVALID_ARGUMENT);
    EXPECT_EQ(zeEventDestroy(hEvent), ZE_RESULT_SUCCESS);
    zetContextActivateMetricGroups(zeContext, zeDevice, 0u, nullptr);
    if (hMetricStreamer) {
        zetMetricStreamerClose(hMetricStreamer);
    }
    tearDownMetrics();
}

/* Test uses two queues and two memory locations. The test pattern
 * is copied between two memory locations according to the pattern
 * mem1[0]->mem2[0]->mem1[1]->mem2[1] etc. by series of copy operations. CounterBasedEvents are used
 * for synchronization to keep proper order of execution two command queues. It is expected that
 * when execution was in order the pattern will be present at the end of first buffer.
 */
TEST_F(CounterBasedEvent, RunMultiCopyOperationsAppendSynchronization) {
    ze_result_t ret;
    const size_t iterations = 200;
    uint32_t pattern = 0x12345678;
    const size_t allocSize = (iterations + 1) * sizeof(pattern);

    auto scopedQueue1 = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto scopedQueue2 = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    auto mem1 = AllocSharedMemory(allocSize);
    auto mem2 = AllocSharedMemory(allocSize);

    ASSERT_NE(mem1.get(), nullptr);
    ASSERT_NE(mem2.get(), nullptr);
    memset(mem1.get(), 0, allocSize);
    memset(mem2.get(), 0, allocSize);

    /* Set pattern on first position of mem1 buffer */
    memcpy(mem1.get(), &pattern, sizeof(pattern));

    decltype(pattern) *cpyPtr1 = static_cast<decltype(pattern) *>(mem1.get());
    decltype(pattern) *cpyPtr2 = static_cast<decltype(pattern) *>(mem2.get());
    std::vector<ze_command_list_handle_t> cmdListHandlersForQueue1;
    std::vector<ze_command_list_handle_t> cmdListHandlersForQueue2;
    std::vector<zeScope::SharedPtr<ze_command_list_handle_t>> cmdListScopedHandles;

    /*Create CounterBasedEvents and wrap them into shared_ptrs*/
    auto hEvent1Scoped = zeScope::counterBasedEventCreate(zeContext, zeDevice, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto hEvent1 = hEvent1Scoped.get();
    ASSERT_NE(hEvent1, nullptr);
    auto hEvent2Scoped = zeScope::counterBasedEventCreate(zeContext, zeDevice, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto hEvent2 = hEvent2Scoped.get();
    ASSERT_NE(hEvent2, nullptr);

    for (size_t i = 0; i < iterations; i++) {
        auto cmdListQueue1 = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        // Append copy pattern fom mem1[cpyPtr1]->mem2[cpyPtr2], Event1 is initially signaled, so
        // copy should be executed immediately
        ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdListQueue1.get(),
                                                cpyPtr2,
                                                cpyPtr1,
                                                sizeof(pattern),
                                                hEvent2,
                                                1,
                                                &hEvent1),
                  ZE_RESULT_SUCCESS);
        // When Event2 signalled, move mem1[cpyPtr1 + 1](is 0) to mem1[cpyPtr1], it means we zero
        // previous pattern location
        ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdListQueue1.get(),
                                                cpyPtr1,
                                                cpyPtr1 + 1,
                                                sizeof(pattern),
                                                hEvent1,
                                                1,
                                                &hEvent2),
                  ZE_RESULT_SUCCESS);

        zeCommandListClose(cmdListQueue1.get());
        cmdListHandlersForQueue1.push_back(cmdListQueue1.get());
        cmdListScopedHandles.push_back(std::move(cmdListQueue1));

        auto cmdListQueue2 = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        /*Pattern is on second location mem2[Ptr2] copy it back one location later to mem1[cpyPtr1 +
         * 1] */
        ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdListQueue2.get(),
                                                cpyPtr1 + 1,
                                                cpyPtr2,
                                                sizeof(pattern),
                                                hEvent2,
                                                1,
                                                &hEvent1),
                  ZE_RESULT_SUCCESS);
        /*And zero in the original location mem2[cpyPtr2] */
        ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdListQueue2.get(),
                                                cpyPtr2,
                                                cpyPtr2 + 1,
                                                sizeof(pattern),
                                                hEvent1,
                                                1,
                                                &hEvent2),
                  ZE_RESULT_SUCCESS);
        zeCommandListClose(cmdListQueue2.get());
        cmdListHandlersForQueue2.push_back(cmdListQueue2.get());
        cmdListScopedHandles.push_back(std::move(cmdListQueue2));

        // move to next location for next iteration
        cpyPtr1++;
        cpyPtr2++;
    }

    auto testPtr = static_cast<decltype(pattern) *>(mem1.get()) + iterations;
    /* Verify that there is 0 at expected pattern location before execution*/
    EXPECT_EQ(*testPtr, 0);

    /* Execute asynchronously, resubmits are expected synchronous execution could block */
    std::future<ze_result_t> Queue1Executor = std::async(std::launch::async, [&] {
        return zeCommandQueueExecuteCommandLists(scopedQueue1.get(),
                                                 cmdListHandlersForQueue1.size(),
                                                 cmdListHandlersForQueue1.data(),
                                                 nullptr);
    });
    std::future<ze_result_t> Queue2Executor = std::async(std::launch::async, [&] {
        return zeCommandQueueExecuteCommandLists(scopedQueue2.get(),
                                                 cmdListHandlersForQueue2.size(),
                                                 cmdListHandlersForQueue2.data(),
                                                 nullptr);
    });

    ASSERT_EQ(Queue1Executor.get(), ZE_RESULT_SUCCESS);

    ASSERT_EQ(Queue2Executor.get(), ZE_RESULT_SUCCESS);

    /* Wait only for default, synchronization should guarantee all copy should be finished */
    ASSERT_EQ(zeCommandQueueSynchronize(scopedQueue2.get(), graphSyncTimeout), ZE_RESULT_SUCCESS);

    std::vector<char> zeroBuf(allocSize, 0);

    /* Second buffer should be all empty, first buffer should be empty except the last position
       where should be present pattern.
     */
    EXPECT_EQ(memcmp(mem2.get(), zeroBuf.data(), allocSize), 0);
    EXPECT_EQ(memcmp(mem1.get(), zeroBuf.data(), allocSize - sizeof(pattern)), 0);
    /* At the end of first buffer is expected test pattern*/
    EXPECT_EQ(*testPtr, pattern);
}
