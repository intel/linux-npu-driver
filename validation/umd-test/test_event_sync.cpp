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

        ze_command_queue_desc_t cmdQueueDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                             .pNext = nullptr,
                                             .ordinal = 0,
                                             .index = 0,
                                             .flags = 0,
                                             .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                             .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};
        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        // NN(Compute) queue.
        cmdQueueDesc.ordinal = computeGrpOrdinal;
        scopedNnQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        nnQue = scopedNnQueue.get();

        // Copy queue.
        cmdQueueDesc.ordinal = copyGrpOrdinal;
        scopedCpQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        cpQue = scopedCpQueue.get();

        ze_command_list_desc_t cmdListDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                              .pNext = nullptr,
                                              .commandQueueGroupOrdinal = 0,
                                              .flags = 0};
        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();

        // NN(Compute) command list.
        cmdListDesc.commandQueueGroupOrdinal = computeGrpOrdinal;
        scopedNnList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        nnCmdlist = scopedNnList.get();

        // Copy command list.
        cmdListDesc.commandQueueGroupOrdinal = copyGrpOrdinal;
        scopedCpList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        cpCmdlist = scopedCpList.get();

        // Event Pool
        scopedEventPool = zeScope::eventPoolCreate(zeContext, eventPoolDesc, 1, zeDevice, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        eventPool = scopedEventPool.get();
    }

    void TearDown() override { UmdTest::TearDown(); }

    void HostSignalTask(ze_event_handle_t hEvent, uint64_t sleepMS) {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMS));
        EXPECT_EQ(zeEventHostSignal(hEvent), ZE_RESULT_SUCCESS);
    }

    /**
     * @brief Run a test case scenario. Device waits with event and gets signal from host.
     *
     * @param computeEngineWait [in]: Command list will be fed into compute engine if set true, copy
     * engine if false.
     */
    void WaitHostSignal(bool computeEngineWait);

    /**
     * @brief Run a test case scenario. One engine signals other waiting engine.
     *
     * @param computeEngineWait [in]: Compute engine waits, copy engine signals if set true. Vice
     * versa if false.
     * @param testEventReset [in]: Reset event if set true, false otherwise.
     */
    void WaitAndSignalBetweenEngines(bool computeEngineWait, bool testEventReset = false);

    /**
     * @brief Run a test case scenario that multiple command lists executed in order.
     *
     * @param runOnComputeEngine [in]: Use compute engine for the testing if true, copy engine if
     * false.
     */
    void MultiCommandListSyncOnSingleEngine(bool runOnComputeEngine);

    void UseHostSynchronizeToSynchronizeCommandExecution(bool isCompute);

    // Copy/NN queues.
    ze_command_queue_handle_t nnQue = nullptr;
    ze_command_queue_handle_t cpQue = nullptr;

    // Copy/NN command lists.
    ze_command_list_handle_t cpCmdlist = nullptr;
    ze_command_list_handle_t nnCmdlist = nullptr;

    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;

    // Event pool (size: 3)
    const ze_event_pool_desc_t eventPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                                nullptr,
                                                ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                                3};
    zeScope::SharedPtr<ze_event_pool_handle_t> scopedEventPool = nullptr;
    ze_event_pool_handle_t eventPool = nullptr;
    ze_result_t ret;

    static const size_t allocSize = PAGE_SIZE;
    static const uint8_t testPattern = 0xCD;

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedNnQueue = nullptr;
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedCpQueue = nullptr;
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedNnList = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedCpList = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
};

void EventSync::WaitHostSignal(bool computeEngineWait) {
    const ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                       nullptr,
                                       0,
                                       ZE_EVENT_SCOPE_FLAG_HOST,
                                       ZE_EVENT_SCOPE_FLAG_HOST};
    auto scopedEvent = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event = scopedEvent.get();

    std::shared_ptr<void> mem = AllocSharedMemory(allocSize);
    ASSERT_NE(mem, nullptr);
    auto ts = static_cast<uint64_t *>(mem.get());

    // Engine to use.
    ze_command_queue_handle_t queue;
    ze_command_list_handle_t cmdlist = cpCmdlist;
    if (computeEngineWait) {
        cmdlist = nnCmdlist;
        queue = nnQue;
    } else {
        cmdlist = cpCmdlist;
        queue = cpQue;
    }

    ASSERT_EQ(zeCommandListAppendWaitOnEvents(cmdlist, 1, &event), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(cmdlist, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdlist), ZE_RESULT_SUCCESS);

    // Execute command lists.
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &cmdlist, nullptr), ZE_RESULT_SUCCESS);

    std::thread hostSignalThread(&EventSync::HostSignalTask, this, event, 100);

    // Command queue host sync.
    EXPECT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);

    // Synchronize the thread
    hostSignalThread.join();

    // Check event status.
    EXPECT_EQ(zeEventQueryStatus(event), ZE_RESULT_SUCCESS);

    // Verify timestamp is greater than 0.
    EXPECT_GT(*ts, 0u);
}

void EventSync::WaitAndSignalBetweenEngines(bool computeEngineWait, bool testEventReset) {
    const ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                       nullptr,
                                       2,
                                       ZE_EVENT_SCOPE_FLAG_HOST,
                                       ZE_EVENT_SCOPE_FLAG_HOST};
    auto scopedEvent = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event = scopedEvent.get();

    ze_command_queue_handle_t waitQue = nullptr;
    ze_command_queue_handle_t signalQue = nullptr;
    ze_command_list_handle_t waitCmdlist = nullptr;
    ze_command_list_handle_t signalCmdlist = nullptr;

    std::shared_ptr<void> mem = AllocSharedMemory(allocSize);
    std::shared_ptr<void> mem1 = AllocSharedMemory(allocSize);
    ASSERT_NE(mem, nullptr);
    ASSERT_NE(mem1, nullptr);
    auto ts = static_cast<uint64_t *>(mem.get());
    auto ts1 = static_cast<uint64_t *>(mem1.get());

    if (computeEngineWait) {
        // Compute engine waits, copy engine signals.
        // Compute engine waits
        waitQue = nnQue;
        waitCmdlist = nnCmdlist;

        // Copy engine signals
        signalQue = cpQue;
        signalCmdlist = cpCmdlist;
    } else {
        // Copy engine waits, compute engine signals.
        // Copy engine waits
        waitQue = cpQue;
        waitCmdlist = cpCmdlist;

        // Compute engine signals
        signalQue = nnQue;
        signalCmdlist = nnCmdlist;
    }

    ASSERT_EQ(zeCommandListAppendWaitOnEvents(waitCmdlist, 1, &event), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(waitCmdlist, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    if (testEventReset) {
        ASSERT_EQ(zeCommandListAppendEventReset(waitCmdlist, event), ZE_RESULT_SUCCESS);
    }
    ASSERT_EQ(zeCommandListClose(waitCmdlist), ZE_RESULT_SUCCESS);

    // Make sure the event is not signaled.
    ASSERT_EQ(zeEventQueryStatus(event), ZE_RESULT_NOT_READY);

    // Execute command lists.
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(waitQue, 1, &waitCmdlist, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(signalCmdlist, ts1, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendSignalEvent(signalCmdlist, event), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(signalCmdlist), ZE_RESULT_SUCCESS);

    // Make sure the event is not signaled.
    ASSERT_EQ(zeEventQueryStatus(event), ZE_RESULT_NOT_READY);

    // Execute command lists.
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(signalQue, 1, &signalCmdlist, nullptr),
              ZE_RESULT_SUCCESS);

    // Command queue host sync.
    ASSERT_EQ(zeCommandQueueSynchronize(waitQue, syncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(signalQue, syncTimeout), ZE_RESULT_SUCCESS);

    // Verify timestamp results.
    EXPECT_GT(*ts, *ts1) << "Timestamp relation mismatch from expected values";

    if (testEventReset) {
        EXPECT_EQ(zeEventQueryStatus(event), ZE_RESULT_NOT_READY);
    } else {
        EXPECT_EQ(zeEventQueryStatus(event), ZE_RESULT_SUCCESS);
    }
}

void EventSync::MultiCommandListSyncOnSingleEngine(bool runOnComputeEngine) {
    // Event objects.
    ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 nullptr,
                                 0,
                                 ZE_EVENT_SCOPE_FLAG_HOST,
                                 ZE_EVENT_SCOPE_FLAG_HOST};

    auto scopedEvent0 = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    eventDesc.index = 1;
    auto scopedEvent1 = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    eventDesc.index = 2;
    auto scopedEvent2 = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    auto event0 = scopedEvent0.get();
    auto event1 = scopedEvent1.get();
    auto event2 = scopedEvent2.get();

    // Utilize the same shared memory test pattern.
    std::shared_ptr<void> srcDeviceMem = AllocSharedMemory(allocSize);
    std::shared_ptr<void> destMem0 = nullptr;
    std::shared_ptr<void> destMem1 = nullptr;
    std::shared_ptr<void> destMem2 = nullptr;

    ze_command_list_desc_t cmdlistDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                          .pNext = nullptr,
                                          .commandQueueGroupOrdinal = 0,
                                          .flags = 0};
    ze_command_queue_handle_t cmdQue = nullptr;
    if (runOnComputeEngine) {
        cmdQue = nnQue;
        cmdlistDesc.commandQueueGroupOrdinal = computeGrpOrdinal;
        destMem0 = AllocSharedMemory(allocSize);
        destMem1 = AllocSharedMemory(allocSize);
        destMem2 = AllocSharedMemory(allocSize);
    } else {
        cmdQue = cpQue;
        cmdlistDesc.commandQueueGroupOrdinal = copyGrpOrdinal;
        destMem0 = AllocHostMemory(allocSize);
        destMem1 = AllocHostMemory(allocSize);
        destMem2 = AllocHostMemory(allocSize);
    }
    ASSERT_NE(srcDeviceMem, nullptr);
    ASSERT_NE(destMem0, nullptr);
    ASSERT_NE(destMem1, nullptr);
    ASSERT_NE(destMem2, nullptr);

    auto srcDevicePtr = static_cast<uint8_t *>(srcDeviceMem.get());
    auto destMem0Ptr = static_cast<uint8_t *>(destMem0.get());
    auto destMem1Ptr = static_cast<uint8_t *>(destMem1.get());
    auto destMem2Ptr = static_cast<uint8_t *>(destMem2.get());

    // Prepare source memory.
    memset(srcDevicePtr, testPattern, allocSize);

    // Create 3 command lists for event sync test.
    auto scopedCmdList0 = zeScope::commandListCreate(zeContext, zeDevice, cmdlistDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto scopedCmdList1 = zeScope::commandListCreate(zeContext, zeDevice, cmdlistDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto scopedCmdList2 = zeScope::commandListCreate(zeContext, zeDevice, cmdlistDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto cmdlist0 = scopedCmdList0.get();
    auto cmdlist1 = scopedCmdList1.get();
    auto cmdlist2 = scopedCmdList2.get();

    ASSERT_EQ(zeCommandListAppendWaitOnEvents(cmdlist0, 1, &event0), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdlist0,
                                            destMem0Ptr,
                                            srcDevicePtr,
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendSignalEvent(cmdlist0, event1), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdlist0), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendWaitOnEvents(cmdlist1, 1, &event1), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdlist1,
                                            destMem1Ptr,
                                            srcDevicePtr,
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendSignalEvent(cmdlist1, event2), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdlist1), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendWaitOnEvents(cmdlist2, 1, &event2), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(cmdlist2,
                                            destMem2Ptr,
                                            srcDevicePtr,
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdlist2), ZE_RESULT_SUCCESS);

    // Events are not signaled yet.
    ASSERT_EQ(zeEventQueryStatus(event0), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeEventQueryStatus(event1), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeEventQueryStatus(event2), ZE_RESULT_NOT_READY);

    // Execute command lists.
    ze_command_list_handle_t cmdlists[] = {cmdlist0, cmdlist1, cmdlist2};
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(cmdQue, 3, cmdlists, nullptr), ZE_RESULT_SUCCESS);

    // Signal from host.
    ASSERT_EQ(zeEventHostSignal(event0), ZE_RESULT_SUCCESS);

    // Wait to be synchronized.
    ASSERT_EQ(zeCommandQueueSynchronize(cmdQue, syncTimeout), ZE_RESULT_SUCCESS);

    // Expect all events gets signaled.
    EXPECT_EQ(zeEventQueryStatus(event0), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeEventQueryStatus(event1), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeEventQueryStatus(event2), ZE_RESULT_SUCCESS);

    // Expect copy command was ran by VPU.
    EXPECT_EQ(memcmp(srcDevicePtr, destMem0Ptr, allocSize), 0)
        << "Command list 0 didn't run properly.";
    EXPECT_EQ(memcmp(srcDevicePtr, destMem1Ptr, allocSize), 0)
        << "Command list 1 didn't run properly.";
    EXPECT_EQ(memcmp(srcDevicePtr, destMem2Ptr, allocSize), 0)
        << "Command list 2 didn't run properly.";
}

void EventSync::UseHostSynchronizeToSynchronizeCommandExecution(bool isCompute) {
    const ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                       nullptr,
                                       0,
                                       ZE_EVENT_SCOPE_FLAG_HOST,
                                       ZE_EVENT_SCOPE_FLAG_HOST};
    auto scopedEvent = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event = scopedEvent.get();

    std::shared_ptr<void> mem = AllocSharedMemory(allocSize);
    ASSERT_NE(mem, nullptr);
    auto ts = static_cast<uint64_t *>(mem.get());

    ze_command_queue_handle_t queue = {};
    ze_command_list_handle_t cmdlist = {};
    if (isCompute) {
        queue = nnQue;
        cmdlist = nnCmdlist;
    } else {
        queue = cpQue;
        cmdlist = cpCmdlist;
    }

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(cmdlist, ts, event, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cmdlist), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &cmdlist, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeEventHostSynchronize(event, syncTimeout), ZE_RESULT_SUCCESS);

    EXPECT_GT(*ts, 0u);

    EXPECT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeEventQueryStatus(event), ZE_RESULT_SUCCESS);
}

TEST_F(EventSync, CopyEngineWaitForHostSignal) {
    // Host signal, copy engine waits.
    WaitHostSignal(false);
}

TEST_F(EventSync, ComputeEngineWaitForHostSignal) {
    // Host signal, compute engine waits.
    WaitHostSignal(true);
}

TEST_F(EventSync, ComputeEngineWaitForCopyEngineSignal) {
    // Compute engine waits, copy engine signal.
    WaitAndSignalBetweenEngines(true);
}

TEST_F(EventSync, ComputeEngineWaitAndResetForCopyEngineSignal) {
    // Compute engine waits - reset, copy engine signal.
    WaitAndSignalBetweenEngines(true, true);
}

TEST_F(EventSync, CopyEngineWaitForComputeEngineSignal) {
    // Copy engine waits, compute engine signal.
    WaitAndSignalBetweenEngines(false);
}

TEST_F(EventSync, CopyEngineWaitAndResetForComputeEngineSignal) {
    // Copy engine waits - reset, compute engine signal.
    WaitAndSignalBetweenEngines(false, true);
}

TEST_F(EventSync, ComputeEngineMultiCommandListSync) {
    MultiCommandListSyncOnSingleEngine(true);
}

TEST_F(EventSync, CopyEngineMultiCommandListSync) {
    MultiCommandListSyncOnSingleEngine(false);
}

TEST_F(EventSync, CopyEngineShouldRespectWaitOnEvents) {
    // Create two event instances event0 and event1.
    ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 nullptr,
                                 0,
                                 ZE_EVENT_SCOPE_FLAG_HOST,
                                 ZE_EVENT_SCOPE_FLAG_HOST};
    auto scopedEvent0 = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event0 = scopedEvent0.get();
    eventDesc.index = 1;
    auto scopedEvent1 = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event1 = scopedEvent1.get();

    auto srcBuffMem = AllocHostMemory(allocSize);
    auto tempBuffMem = AllocDeviceMemory(allocSize);
    auto dstBuffMem = AllocHostMemory(allocSize);
    memset(srcBuffMem.get(), 0x01, allocSize);

    // Verify host reads event as unset.
    EXPECT_EQ(zeEventHostSynchronize(event0, 0), ZE_RESULT_NOT_READY);

    // Execute and verify reads event
    ASSERT_EQ(zeCommandListAppendMemoryCopy(cpCmdlist,
                                            tempBuffMem.get(),
                                            srcBuffMem.get(),
                                            allocSize,
                                            event0,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(cpCmdlist,
                                            dstBuffMem.get(),
                                            tempBuffMem.get(),
                                            allocSize,
                                            nullptr,
                                            1,
                                            &event1),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cpCmdlist), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(cpQue, 1, &cpCmdlist, nullptr), ZE_RESULT_SUCCESS);

    // Verify copy waits for signal.
    ASSERT_EQ(zeEventHostSynchronize(event0, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeEventQueryStatus(event1), ZE_RESULT_NOT_READY);

    EXPECT_NE(memcmp(srcBuffMem.get(), dstBuffMem.get(), allocSize), 0);

    EXPECT_EQ(zeEventHostSignal(event1), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(cpQue, syncTimeout), ZE_RESULT_SUCCESS);

    // Verify memory copy completed.
    EXPECT_EQ(memcmp(srcBuffMem.get(), dstBuffMem.get(), allocSize), 0);
}

TEST_F(EventSync, SynchronizedAcrossDifferentEngines) {
    const ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                       nullptr,
                                       0,
                                       ZE_EVENT_SCOPE_FLAG_HOST,
                                       ZE_EVENT_SCOPE_FLAG_HOST};
    auto scopedEvent = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event = scopedEvent.get();

    auto mem = AllocSharedMemory(allocSize);
    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_TRUE(ts) << "Failed to allocate shared memory";

    auto srcBuffMem = AllocSharedMemory(allocSize);
    auto dstBuffMem = AllocHostMemory(allocSize);
    memset(srcBuffMem.get(), 0xCD, allocSize);

    // Execute and verify reads event
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(nnCmdlist, ts, event, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(cpCmdlist,
                                            dstBuffMem.get(),
                                            srcBuffMem.get(),
                                            allocSize,
                                            nullptr,
                                            1,
                                            &event),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(nnCmdlist), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(cpCmdlist), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(cpQue, 1, &cpCmdlist, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(nnQue, 1, &nnCmdlist, nullptr), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueSynchronize(nnQue, syncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(cpQue, syncTimeout), ZE_RESULT_SUCCESS);

    // Verify memory copy completed.
    EXPECT_EQ(memcmp(srcBuffMem.get(), dstBuffMem.get(), allocSize), 0);
}

TEST_F(EventSync, ComputeEngineUseEventHostSynchronizeToSynchronizeCommandExecution) {
    UseHostSynchronizeToSynchronizeCommandExecution(true);
}

TEST_F(EventSync, CopyEngineUseEventHostSynchronizeToSynchronizeCommandExecution) {
    UseHostSynchronizeToSynchronizeCommandExecution(false);
}

TEST_F(EventSync, AppendGlobalTimestampAndSynchronizeWithEvent) {
    ze_event_desc_t eventDesc = {.stype = ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 .pNext = nullptr,
                                 .index = 0,
                                 .signal = ZE_EVENT_SCOPE_FLAG_HOST,
                                 .wait = ZE_EVENT_SCOPE_FLAG_HOST};

    // Prepare events.
    auto scopedEvent0 = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event0 = scopedEvent0.get();
    eventDesc.index = 1;
    auto scopedEvent1 = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event1 = scopedEvent1.get();
    eventDesc.index = 2;
    auto scopedEvent2 = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event2 = scopedEvent2.get();

    const size_t size = sizeof(uint64_t);
    auto mem = AllocSharedMemory(size);
    ze_event_handle_t waitEvents[] = {event0, event1};
    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_TRUE(ts) << "Failed to allocate shared memory";

    // Execute TS command with waiting event0 and event1, event2 should be signaled after the
    // completion.
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, event2, 2, waitEvents),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeEventQueryStatus(event0), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeEventQueryStatus(event1), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeEventQueryStatus(event2), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);

    // Signal waiting events from host.
    ASSERT_EQ(zeEventHostSignal(event0), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventHostSignal(event1), ZE_RESULT_SUCCESS);

    // Sync with the event.
    EXPECT_EQ(zeEventHostSynchronize(event2, syncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventQueryStatus(event0), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventQueryStatus(event1), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventQueryStatus(event2), ZE_RESULT_SUCCESS);

    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}

TEST_F(EventSync, AppendCopyAndBarrierThenSynchronizeWithEvent) {
    ze_event_desc_t eventDesc = {.stype = ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 .pNext = nullptr,
                                 .index = 0,
                                 .signal = ZE_EVENT_SCOPE_FLAG_HOST,
                                 .wait = ZE_EVENT_SCOPE_FLAG_HOST};

    auto scopedEvent0 = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event0 = scopedEvent0.get();
    eventDesc.index = 1;
    auto scopedEvent1 = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event1 = scopedEvent1.get();
    eventDesc.index = 2;
    auto scopedEvent2 = zeScope::eventCreate(eventPool, eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto event2 = scopedEvent2.get();

    ze_event_handle_t waitEvents[] = {event0, event1};
    const size_t allocSize = PAGE_SIZE;
    auto srcMem = AllocHostMemory(allocSize);
    auto destMem = AllocSharedMemory(allocSize);
    ASSERT_TRUE(srcMem) << "Failed to allocate source memory";
    ASSERT_TRUE(destMem) << "Failed to allocate destination memory";
    memset(srcMem.get(), 0xCD, allocSize);

    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            destMem.get(),
                                            srcMem.get(),
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendBarrier(list, event2, 2, waitEvents), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeEventQueryStatus(event0), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeEventQueryStatus(event1), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeEventQueryStatus(event2), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);

    // Signal waiting events from host.
    ASSERT_EQ(zeEventHostSignal(event0), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventHostSignal(event1), ZE_RESULT_SUCCESS);

    // Sync with the event.
    EXPECT_EQ(zeEventHostSynchronize(event2, syncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventQueryStatus(event0), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventQueryStatus(event1), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventQueryStatus(event2), ZE_RESULT_SUCCESS);

    EXPECT_EQ(memcmp(destMem.get(), srcMem.get(), allocSize), 0) << "Memory copy failed.";
}
