/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "perf_counter.h"
#include "umd_test.h"

#include <future>

class Command : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();
        scopedQueueVec.reserve(4);
        scopedListVec.reserve(4);

        scopedQueueVec.push_back(
            zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret));
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueueVec.back().get();

        scopedListVec.push_back(zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret));
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedListVec.back().get();
    }

#ifndef ANDROID
    // Functor for threaded use case of zeCommandQueueSync with a promise as parameter
    // umd-test will utilize std::thread to spawn a new thread with this functor to
    // perform zeCommandQueueSynchronize with BLOCKING scenario (UINT64_MAX)
    // While the std::promise is used to check for the zeCommandQueueSynchronize
    // result in the main thread with timeout.
    void threadedCommandQueueSyncWrapper(std::promise<_ze_result_t> &&promise) {
        // This thread has to be killed instantly, otherwise SEGFAULT could happen
        // if cancellation is DEFERRED post handleFailure()
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        ASSERT_NE(nullptr, queue);
        promise.set_value(zeCommandQueueSynchronize(queue, UINT64_MAX));
    }
#endif

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

    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;

  protected:
    const size_t size = sizeof(uint64_t);
    std::vector<zeScope::SharedPtr<ze_command_queue_handle_t>> scopedQueueVec;
    std::vector<zeScope::SharedPtr<ze_command_list_handle_t>> scopedListVec;
    ze_result_t ret;
};

TEST_F(Command, CreateCloseAndDestroyList) {
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
}

TEST_F(Command, CreateCloseResetAndDestroyList) {
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
}

TEST_F(Command, CreateSynchronizeAndDestroyQueue) {
    ASSERT_EQ(zeCommandQueueSynchronize(queue, 0), ZE_RESULT_SUCCESS);
}

TEST_F(Command, CreateExecuteSynchronizeAndDestroyQueueErrorHandle) {
    EXPECT_EQ(zeCommandQueueExecuteCommandLists(queue, 0, &list, nullptr),
              ZE_RESULT_ERROR_INVALID_SIZE);
    EXPECT_EQ(zeCommandQueueSynchronize(queue, 0), ZE_RESULT_SUCCESS);
}

TEST_F(Command, CreateMultipleCommandQueuesUsingSameOrdinal) {
    ze_result_t ret;
    std::array<zeScope::SharedPtr<ze_command_queue_handle_t>, 3> scopedQueues = {};
    for (auto &q : scopedQueues) {
        q = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    }
}

TEST_F(Command, CreateTwoCommandListUsingSameOrdinal) {
    ze_result_t ret;
    scopedListVec.push_back(zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret));
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
}

TEST_F(Command, PassTwoCommandListsToCommandQueueExecuteCommandLists) {
    auto mem = AllocSharedMemory(size);
    ASSERT_TRUE(mem.get()) << "Failed to allocate shared memory";

    auto mem2 = AllocSharedMemory(size);
    ASSERT_TRUE(mem2.get()) << "Failed to allocate shared memory";

    uint64_t *ts1 = static_cast<uint64_t *>(mem.get());
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts1, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ze_result_t ret;
    scopedListVec.push_back(zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret));
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto list2 = scopedListVec.back().get();

    uint64_t *ts2 = static_cast<uint64_t *>(mem2.get());
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list2, ts2, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list2), ZE_RESULT_SUCCESS);

    std::vector<ze_command_list_handle_t> listVec{list, list2};
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, listVec.size(), listVec.data(), nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts1, 0llu) << "Timestamp1 should be different from 0";
    EXPECT_NE(*ts2, 0llu) << "Timestamp2 should be different from 0";
    EXPECT_NE(*ts1, *ts2) << "Timestamps should be different";
}

TEST_F(Command, SynchronizeCommandWithTimeoutBeingZero) {
    auto mem = AllocSharedMemory(size);
    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_TRUE(ts) << "Failed to allocate host memory";

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ze_result_t result = zeCommandQueueSynchronize(queue, 0u);
    EXPECT_TRUE(result == ZE_RESULT_SUCCESS || result == ZE_RESULT_NOT_READY);
}

#ifndef ANDROID
TEST_F(Command, SynchronizeCommandWithTimeoutBeingMaxUint64) {
    auto mem = AllocSharedMemory(size);
    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_TRUE(ts) << "Failed to allocate shared memory";

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);

    // Use std::promise to track if zeCommandQueueSynchronize completed in separate
    // thread. In scenario that blocking call with UINT64_MAX or TDR mechanism
    // unable to fail gracefully for UMD, userspace app can rely on timeout
    // to kill the zeCommandQueueSynchronize thread.
    std::promise<_ze_result_t> promise;
    std::future<_ze_result_t> futureSync = promise.get_future();
    std::thread cmdQueueSyncThread(&Command::threadedCommandQueueSyncWrapper,
                                   this,
                                   std::move(promise));

    // Keep the native pthread handle to cancellation, prior to detach()
    pthread_t threadID = cmdQueueSyncThread.native_handle();
    cmdQueueSyncThread.detach();

    // Userspace app defined timeout for decision on killing the zeCommandQueueSynchronize
    // thread
    std::chrono::steady_clock::time_point asyncTimeOut =
        std::chrono::steady_clock::now() + std::chrono::microseconds(syncTimeout);

    ze_result_t result = ZE_RESULT_NOT_READY;
    if (std::future_status::ready == futureSync.wait_until(asyncTimeOut)) {
        result = futureSync.get();
    } else {
        // asyncTimeOut and zeCommandQueueSynchronize hasn't returned
        // userspace app needs to cancel the thread prior to handleFailure()
        // which free the objects
        pthread_cancel(threadID);
    }
    ASSERT_EQ(result, ZE_RESULT_SUCCESS) << "TIMEOUT from threadedCommandQueueSyncWrapper";
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
}
#endif

TEST_F(Command, UseZeCommandQueueSynchronizeInLoop) {
    auto mem = AllocSharedMemory(sizeof(uint64_t));
    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_TRUE(ts) << "Failed to allocate shared memory";

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);

    auto timeout = std::chrono::steady_clock::now() + std::chrono::nanoseconds(syncTimeout);
    while (zeCommandQueueSynchronize(queue, 0) == ZE_RESULT_NOT_READY) {
        if (std::chrono::steady_clock::now() > timeout)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    ASSERT_EQ(zeCommandQueueSynchronize(queue, 0), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}

TEST_F(Command, MultipleCommandQueueSubmissionStressTest) {
    auto mem = AllocSharedMemory(size);
    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_TRUE(ts) << "Failed to allocate shared memory";

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    uint64_t prev_ts = 0u;
    for (int i = 0; i <= 5; i++) {
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);

        EXPECT_GT(*ts, prev_ts) << "Timestamp should keep increasing";
        prev_ts = *ts;
    }
}

TEST_F(Command, SynchronousCommandQueueSubmissionTest) {
    const size_t numCommands = isSilicon() ? 1000 : 50;
    auto mem = AllocSharedMemory(size);
    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_TRUE(ts) << "Failed to allocate shared memory";
    *ts = 0ULL;

    cmdQueueDesc.mode = ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS;
    auto sQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    for (size_t i = 0; i <= numCommands; i++) {
        ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
    }
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    /* Execute single command list on synchronous mode */
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(sQueue.get(), 1, &list, nullptr),
              ZE_RESULT_SUCCESS);
    /* The command list should be ready immediately after execution */
    ASSERT_EQ(zeCommandQueueSynchronize(sQueue.get(), 0), ZE_RESULT_SUCCESS);
    EXPECT_GT(*ts, 0);

    /* Execute three times without synchronization */
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(sQueue.get(), 1, &list, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(sQueue.get(), 1, &list, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(sQueue.get(), 1, &list, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(sQueue.get(), 0), ZE_RESULT_SUCCESS);

    /* Execute queue created in default mode, NOT_READY expected for timeout set to 0 */
    *ts = 0;
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, 0), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_GT(*ts, 0);
}

class CommandTimestamp : public Command {};

TEST_F(CommandTimestamp, AppendGlobalTimestampAndSynchronize) {
    auto mem = AllocSharedMemory(size);
    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_TRUE(ts) << "Failed to allocate device memory";

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}

TEST_F(CommandTimestamp, AppendGlobalTimestampOffsetNotZeroAndSynchronize) {
    const size_t allocSize = pageSize;
    auto mem = AllocSharedMemory(allocSize);
    ASSERT_TRUE(mem.get()) << "Failed to allocate shared memory";

    uint64_t *ts = static_cast<uint64_t *>(mem.get()) + 16;
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}

TEST_F(CommandTimestamp, AppendGlobalTimestampTwiceAndSynchronize) {
    auto mem = AllocSharedMemory(size);
    ASSERT_TRUE(mem.get()) << "Failed to allocate shared memory";

    auto mem2 = AllocSharedMemory(size);
    ASSERT_TRUE(mem2.get()) << "Failed to allocate shared memory";

    uint64_t *ts1 = static_cast<uint64_t *>(mem.get());
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts1, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    uint64_t *ts2 = static_cast<uint64_t *>(mem2.get());
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts2, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts1, 0llu) << "Timestamp1 should be different from 0";
    EXPECT_NE(*ts2, 0llu) << "Timestamp2 should be different from 0";
    EXPECT_NE(*ts1, *ts2) << "Timestamps should be different";
}

TEST_F(CommandTimestamp,
       AppendGlobalTimestampCloseListResetListAppendGlobalTimestampAndSynchronize) {
    auto mem = AllocSharedMemory(size);
    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_TRUE(ts) << "Failed to allocate shared memory";

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}

TEST_F(CommandTimestamp, AppendGlobalTimestampAndSynchronizeReuseCommandListAndRepeat) {
    auto mem = AllocSharedMemory(size);
    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_TRUE(ts) << "Failed to allocate shared memory";

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";

    uint64_t timestamp1 = *ts;

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";

    EXPECT_GT(*ts, timestamp1) << "Second timestamp value should be greater than first";
}

TEST_F(CommandTimestamp, TwoCommandQueusWithinSameGroupExecuteTimestampAndSynchronizePerQueue) {
    std::array<ze_command_queue_handle_t, 2> queues = {nullptr, nullptr};
    for (ze_command_queue_handle_t &q : queues) {
        scopedQueueVec.push_back(
            zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret));
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        q = scopedQueueVec.back().get();
    }

    auto mem = AllocSharedMemory(size * 2);
    uint64_t *ts = static_cast<uint64_t *>(mem.get());

    for (size_t i = 0; i < queues.size(); i++) {
        ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts + i, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queues[i], 1, &list, nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queues[i], syncTimeout), ZE_RESULT_SUCCESS);

        EXPECT_NE(*(ts + i), 0llu) << "Timestamp should be different from 0";

        ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    }

    EXPECT_LT(*ts, *(ts + 1)) << "Second timestamp should be greater than first";
}

TEST_F(CommandTimestamp, ExecuteTimestampCopyAndTimestampCommands) {
    ze_result_t ret;

    scopedQueueVec.push_back(zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret));
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    queue = scopedQueueVec.back().get();

    scopedListVec.push_back(zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret));
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    list = scopedListVec.back().get();

    auto mem = AllocSharedMemory(size);
    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_TRUE(ts) << "Failed to allocate shared memory";

    auto mem2 = AllocHostMemory(size);
    void *memInputHost = mem2.get();
    ASSERT_NE(memInputHost, nullptr) << "Failed to allocate host memory";
    uint64_t *copyInputHost = static_cast<uint64_t *>(memInputHost);
    *copyInputHost = 0xdeadbeef;

    auto mem3 = AllocSharedMemory(size);
    void *memOutputDev = mem3.get();
    ASSERT_NE(memOutputDev, nullptr) << "Failed to allocate shared memory";
    uint64_t *copyOutputDev = static_cast<uint64_t *>(memOutputDev);

    auto mem4 = AllocSharedMemory(size);
    uint64_t *ts2 = static_cast<uint64_t *>(mem4.get());
    ASSERT_TRUE(ts2) << "Failed to allocate shared memory";

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(
        zeCommandListAppendMemoryCopy(list, memOutputDev, memInputHost, size, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts2, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
    EXPECT_EQ(*copyInputHost, *copyOutputDev) << "Copy failed, numbers should be equal";
    EXPECT_NE(*ts2, 0llu) << "Timestamp should be different from 0";
    EXPECT_NE(*ts, *ts2) << "Timestamps should be different";
}

TEST_F(CommandTimestamp, TwoCommandQueusWithinSameGroupExecuteTimestampAndSynchronize) {
    std::array<ze_command_queue_handle_t, 2> queues = {nullptr, nullptr};
    for (ze_command_queue_handle_t &q : queues) {
        scopedQueueVec.push_back(
            zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret));
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        q = scopedQueueVec.back().get();
    }

    std::array<ze_command_list_handle_t, 2> lists = {nullptr, nullptr};
    for (ze_command_list_handle_t &l : lists) {
        scopedListVec.push_back(zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret));
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        l = scopedListVec.back().get();
    }

    auto mem = AllocSharedMemory(size * 2);
    uint64_t *ts = static_cast<uint64_t *>(mem.get());

    for (size_t i = 0; i < queues.size(); i++) {
        ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(lists[i], ts + i, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(lists[i]), ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queues[i], 1, &lists[i], nullptr),
                  ZE_RESULT_SUCCESS);
    }

    for (size_t i = 0; i < queues.size(); i++) {
        ASSERT_EQ(zeCommandQueueSynchronize(queues[i], syncTimeout), ZE_RESULT_SUCCESS);

        EXPECT_NE(*(ts + i), 0llu) << "Timestamp should be different from 0";
    }
}

TEST_F(CommandTimestamp, CommandTimestampStressTest) {
    auto checkTimestamp = [&]() {
        ze_result_t ret;
        ze_context_desc_t contextDesc = {.stype = ZE_STRUCTURE_TYPE_CONTEXT_DESC,
                                         .pNext = nullptr,
                                         .flags = 0};

        auto devContext = zeScope::contextCreate(zeDriver, contextDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        auto cmdQueue = zeScope::commandQueueCreate(devContext.get(), zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

        auto cmdList = zeScope::commandListCreate(devContext.get(), zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

        auto mem = zeMemory::allocShared(devContext.get(), zeDevice, size, 0);
        uint64_t *ts = static_cast<uint64_t *>(mem.get());

        ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(cmdList.get(), ts, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeCommandListClose(cmdList.get()), ZE_RESULT_SUCCESS);

        auto list = cmdList.get();

        PerfCounter counter(isSilicon() ? 5000 : 100);
        counter.start();

        while (!counter.isTimedOut()) {
            EXPECT_EQ(zeCommandQueueExecuteCommandLists(cmdQueue.get(), 1, &list, nullptr),
                      ZE_RESULT_SUCCESS);
            EXPECT_EQ(zeCommandQueueSynchronize(cmdQueue.get(), syncTimeout * 2),
                      ZE_RESULT_SUCCESS);
            EXPECT_GT(*ts, 0ULL);
        }
    };

    const size_t threadsNum = 20;
    std::vector<std::future<void>> tasks(threadsNum);
    for (auto &task : tasks) {
        task = std::async(std::launch::async, checkTimestamp);
    }
    for (const auto &task : tasks) {
        task.wait();
    }
}
class CommandCopy : public Command {};

TEST_F(CommandCopy, AppendMemoryCopyLocalToLocalAndSynchronize) {
    auto mem = AllocSharedMemory(size);
    void *memInputDev = mem.get();
    ASSERT_NE(memInputDev, nullptr) << "Failed to allocate shared memory";
    uint64_t *copyInputDev = static_cast<uint64_t *>(memInputDev);
    *copyInputDev = 0xdeadbeef;

    auto mem2 = AllocSharedMemory(size);
    void *memOutputDev = mem2.get();
    ASSERT_NE(memOutputDev, nullptr) << "Failed to allocate shared memory";
    uint64_t *copyOutputDev = static_cast<uint64_t *>(memOutputDev);

    ASSERT_EQ(
        zeCommandListAppendMemoryCopy(list, memOutputDev, memInputDev, size, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(*copyInputDev, *copyOutputDev) << "Copy failed, numbers should be equal";
}

TEST_F(CommandCopy, AppendMemoryCopyLocalToSystemAndSynchronize) {
    auto mem = AllocSharedMemory(size);
    void *memInputDev = mem.get();
    ASSERT_NE(memInputDev, nullptr) << "Failed to allocate shared memory";
    uint64_t *copyInputDev = static_cast<uint64_t *>(memInputDev);
    *copyInputDev = 0xdeadbeef;

    auto mem2 = AllocHostMemory(size);
    void *memOutputHost = mem2.get();
    ASSERT_NE(memOutputHost, nullptr) << "Failed to allocate host memory";
    uint64_t *copyOutputHost = static_cast<uint64_t *>(memOutputHost);

    ASSERT_EQ(
        zeCommandListAppendMemoryCopy(list, memOutputHost, memInputDev, size, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(*copyInputDev, *copyOutputHost) << "Copy failed, numbers should be equal";
}

TEST_F(CommandCopy, AppendMemoryCopySystemToLocalAndSynchronize) {
    auto mem = AllocHostMemory(size);
    void *memInputHost = mem.get();
    ASSERT_NE(memInputHost, nullptr) << "Failed to allocate host memory";
    uint64_t *copyInputHost = static_cast<uint64_t *>(memInputHost);
    *copyInputHost = 0xabcdabcd;

    auto mem2 = AllocSharedMemory(size);
    void *memOutputDev = mem2.get();
    ASSERT_NE(memOutputDev, nullptr) << "Failed to allocate shared memory";
    uint64_t *copyOutputDev = static_cast<uint64_t *>(memOutputDev);

    ASSERT_EQ(
        zeCommandListAppendMemoryCopy(list, memOutputDev, memInputHost, size, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(*copyInputHost, *copyOutputDev) << "Copy failed, numbers should be equal";
}

TEST_F(CommandCopy, AppendMemoryCopySystemToSystemAndSynchronize) {
    auto mem = AllocHostMemory(size);
    void *memInputHost = mem.get();
    ASSERT_NE(memInputHost, nullptr) << "Failed to allocate host memory";
    uint64_t *copyInputHost = static_cast<uint64_t *>(memInputHost);
    *copyInputHost = 0xabcdabcd;

    auto mem2 = AllocHostMemory(size);
    void *memOutputHost = mem2.get();
    ASSERT_NE(memOutputHost, nullptr) << "Failed to allocate host memory";
    uint64_t *copyOutputHost = static_cast<uint64_t *>(memOutputHost);

    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            memOutputHost,
                                            copyInputHost,
                                            size,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(*copyInputHost, *copyOutputHost) << "Copy failed, numbers should be equal";
}

TEST_F(CommandCopy, AppendMemoryCopyAndSynchronizeReuseCommandListAndRepeat) {
    auto mem = AllocHostMemory(size);
    void *memInputHost = mem.get();
    ASSERT_NE(memInputHost, nullptr) << "Failed to allocate host memory";
    uint64_t *copyInputHost = static_cast<uint64_t *>(memInputHost);
    *copyInputHost = 0xdeadbeef;

    auto mem2 = AllocSharedMemory(size);
    void *memOutputDev = mem2.get();
    ASSERT_NE(memOutputDev, nullptr) << "Failed to allocate shared memory";
    uint64_t *copyOutputDev = static_cast<uint64_t *>(memOutputDev);

    ASSERT_EQ(
        zeCommandListAppendMemoryCopy(list, memOutputDev, memInputHost, size, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(*copyInputHost, *copyOutputDev) << "Copy #1 failed, numbers should be equal";

    *copyInputHost = 0x00;

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(
        zeCommandListAppendMemoryCopy(list, memInputHost, memOutputDev, size, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(*copyInputHost, *copyOutputDev) << "Copy #2 failed, numbers should be equal";
}

TEST_F(CommandCopy, MultipleCopyOperationsUsingSameAllocatedMemory) {
    auto devMem = AllocSharedMemory(size);
    auto hostMem = AllocHostMemory(size);
    memset(hostMem.get(), 0x10, size);

    ASSERT_EQ(
        zeCommandListAppendMemoryCopy(list, devMem.get(), hostMem.get(), size, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(memcmp(devMem.get(), hostMem.get(), size), 0)
        << "Copy #1 failed, numbers should be equal";
    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

    memset(devMem.get(), 0x20, size);
    ASSERT_EQ(
        zeCommandListAppendMemoryCopy(list, hostMem.get(), devMem.get(), size, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(memcmp(devMem.get(), hostMem.get(), size), 0)
        << "Copy #2 failed, numbers should be equal";
}

TEST_F(CommandCopy, MultipleCopyOperationsWithinOneAllocatedMemory) {
    auto devMem = AllocHostMemory(size * 4);
    memset(devMem.get(), 0x10, size);
    uint64_t *ptr = static_cast<uint64_t *>(devMem.get());

    ASSERT_EQ(zeCommandListAppendMemoryCopy(list, ptr + 1, ptr, size, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(memcmp(ptr, ptr + 1, size), 0) << "Copy #1 failed, numbers should be equal";
    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

    memset(ptr + 2, 0x20, size);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(list, ptr + 3, ptr + 2, size, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(memcmp(ptr + 3, ptr + 2, size), 0) << "Copy #2 failed, numbers should be equal";
}

TEST_F(CommandCopy, TwoCommandQueuesWithinSameGroupExecuteCopyAndSynchronize) {
    std::array<ze_command_queue_handle_t, 2> queues = {nullptr, nullptr};
    for (auto &q : queues) {
        scopedQueueVec.push_back(
            zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret));
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        q = scopedQueueVec.back().get();
    }

    std::array<ze_command_list_handle_t, 2> lists = {nullptr, nullptr};
    for (auto &l : lists) {
        scopedListVec.push_back(zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret));
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        l = scopedListVec.back().get();
    }

    auto mem = AllocHostMemory(size);
    uint64_t *src = static_cast<uint64_t *>(mem.get());
    memset(src, 0xAB, size);
    auto mem2 = AllocHostMemory(size);
    uint64_t *src1 = static_cast<uint64_t *>(mem2.get());
    memset(src1, 0xCD, size);

    auto mem3 = AllocSharedMemory(size);
    uint64_t *dst = static_cast<uint64_t *>(mem3.get());
    auto mem4 = AllocSharedMemory(size);
    uint64_t *dst1 = static_cast<uint64_t *>(mem4.get());

    std::vector<uint64_t *> srcVec = {src, src1};
    std::vector<uint64_t *> dstVec = {dst, dst1};

    for (size_t i = 0; i < queues.size(); i++) {
        ASSERT_EQ(zeCommandListAppendMemoryCopy(lists[i],
                                                dstVec[i],
                                                srcVec[i],
                                                size,
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(lists[i]), ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queues[i], 1, &lists[i], nullptr),
                  ZE_RESULT_SUCCESS);
    }

    for (auto &q : queues) {
        ASSERT_EQ(zeCommandQueueSynchronize(q, syncTimeout), ZE_RESULT_SUCCESS);
    }

    EXPECT_EQ(memcmp(dstVec[0], srcVec[0], size), 0) << "Memory copy #1 failed.";
    EXPECT_EQ(memcmp(dstVec[1], srcVec[1], size), 0) << "Memory copy #2 failed.";
}

class CommandBarrier : public Command {};

TEST_F(CommandBarrier, CopyCommandsUtilizeBarriers) {
    const size_t allocSize = 16 * KB;
    std::vector<std::shared_ptr<void>> devMemList(6, nullptr);
    std::vector<std::shared_ptr<void>> hostMemList(6, nullptr);

    for (int i = 0; i < 6; i++) {
        auto devMem = AllocSharedMemory(allocSize);
        auto hostMem = AllocHostMemory(allocSize);
        ASSERT_TRUE(devMem) << "Failed to allocate shared memory";
        ASSERT_TRUE(hostMem) << "Failed to allocate host memory";

        devMemList[i] = std::move(devMem);
        hostMemList[i] = std::move(hostMem);
    }
    memset(devMemList[0].get(), 0xAB, allocSize);

    // Commandlist structure
    // Copy | Barrier | Copy | NN | Copy | Barrier | Copy | NN | Barrier | NN

    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            hostMemList[0].get(),
                                            devMemList[0].get(),
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendBarrier(list, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            devMemList[1].get(),
                                            hostMemList[0].get(),
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            devMemList[2].get(),
                                            devMemList[1].get(),
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            hostMemList[1].get(),
                                            devMemList[2].get(),
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendBarrier(list, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            devMemList[3].get(),
                                            hostMemList[1].get(),
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            devMemList[4].get(),
                                            devMemList[3].get(),
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendBarrier(list, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            devMemList[5].get(),
                                            devMemList[4].get(),
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);

    EXPECT_EQ(memcmp(devMemList[5].get(), devMemList[0].get(), allocSize), 0)
        << "Memory copy failed.";
}

using PatternType = std::variant<uint8_t, uint16_t, uint32_t>;

class CommandMemoryFill : public Command, public ::testing::WithParamInterface<PatternType> {
  public:
    /* The driver split memfill commands into multiple operations if size is larger than 8MB */
    const size_t size = (8 << 20) * 2 + 12345u;
};

INSTANTIATE_TEST_SUITE_P(,
                         CommandMemoryFill,
                         ::testing::Values<PatternType>(static_cast<uint8_t>(0xCD),
                                                        static_cast<uint16_t>(0xABCD),
                                                        static_cast<uint32_t>(0xDEADBEEF)));

TEST_P(CommandMemoryFill, FillMemoryWithPattern) {
    std::visit(
        [&](auto pattern) {
            /* The reference buffer must consist one more pattern,
             * otherwise the last unaligned element will be not provided and test will fail
             */
            std::vector<typeof(pattern)> ref_buf((size + sizeof(pattern)) / sizeof(pattern),
                                                 pattern);

            auto mem = AllocSharedMemory(size);

            ASSERT_EQ(zeCommandListAppendMemoryFill(list,
                                                    mem.get(),
                                                    &pattern,
                                                    sizeof(pattern),
                                                    size,
                                                    nullptr,
                                                    0,
                                                    nullptr),
                      ZE_RESULT_SUCCESS);

            ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

            ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr),
                      ZE_RESULT_SUCCESS);
            ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);

            ASSERT_EQ(memcmp(mem.get(), ref_buf.data(), size), 0);
        },
        GetParam());
}

class CommandStress : public Command {};

TEST_F(CommandStress, MultipleVPUCommandBuffers) {
    /*
     * Using COPY_LOCAL_TO_LOCAL and COPY_SYSTEM_TO_SYSTEM alternately driver is force to create new
     * VPUCommandBuffer for every copy commmand. VPUCommandBuffer is a job submitted to VPU.
     */
    const size_t copyCount = 32;
    const uint64_t referenceValue = 0x1234567890abcd00;
    const uint64_t copySize = 4 * KB;
    const size_t allocSize = copySize * copyCount + copySize;
    const size_t pointerStep = copySize / sizeof(referenceValue);

    auto devMem = AllocSharedMemory(allocSize);
    auto hostMem = AllocHostMemory(allocSize);
    ASSERT_TRUE(devMem) << "Failed to allocate shared memory";
    ASSERT_TRUE(hostMem) << "Failed to allocate host memory";

    uint64_t *src = static_cast<uint64_t *>(devMem.get());
    uint64_t *tmp = static_cast<uint64_t *>(devMem.get()) + pointerStep;
    uint64_t *dst = static_cast<uint64_t *>(hostMem.get());

    *src = referenceValue;
    for (size_t i = 0; i < copyCount;
         i++, src += pointerStep, tmp += pointerStep, dst += pointerStep) {
        ASSERT_EQ(zeCommandListAppendMemoryCopy(list, tmp, src, copySize, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListAppendMemoryCopy(list, dst, tmp, copySize, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
    }

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout * 10), ZE_RESULT_SUCCESS);

    dst = static_cast<uint64_t *>(hostMem.get()) + pointerStep * (copyCount - 1);
    EXPECT_EQ(*dst, referenceValue);
    TRACE_BUF(devMem.get(), allocSize);
    TRACE_BUF(hostMem.get(), allocSize);
}

TEST_F(CommandStress, MultipleVPUJobs) {
    /*
     * VPUJob is created for every zeCommandList. The test is similar to MultipleVPUCommandBuffers.
     */
    const size_t listCount = 16;
    const size_t copyCount = 7;
    const uint64_t referenceValue = 0x1234567890abcd00;
    const uint64_t copySize = 4 * KB;
    const size_t allocSize = copySize * copyCount + copySize;
    const size_t pointerStep = copySize / sizeof(referenceValue);

    std::vector<std::shared_ptr<void>> devMems;
    std::vector<std::shared_ptr<void>> hostMems;
    std::vector<zeScope::SharedPtr<ze_command_list_handle_t>> scopedLists;

    for (size_t listIndex = 0; listIndex < listCount; listIndex++) {
        ze_result_t ret;
        scopedLists.push_back(zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret));
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        auto list2 = scopedLists.back().get();

        devMems.push_back(AllocSharedMemory(allocSize));
        hostMems.push_back(AllocHostMemory(allocSize));
        ASSERT_TRUE(devMems[listIndex]) << "Failed to allocate shared memory";
        ASSERT_TRUE(hostMems[listIndex]) << "Failed to allocate host memory";

        uint64_t *src = static_cast<uint64_t *>(devMems[listIndex].get());
        uint64_t *tmp = static_cast<uint64_t *>(devMems[listIndex].get()) + pointerStep;
        uint64_t *dst = static_cast<uint64_t *>(hostMems[listIndex].get());

        *src = referenceValue | listIndex;
        for (size_t i = 0; i < copyCount;
             i++, src += pointerStep, tmp += pointerStep, dst += pointerStep) {
            ASSERT_EQ(zeCommandListAppendMemoryCopy(list2, tmp, src, copySize, nullptr, 0, nullptr),
                      ZE_RESULT_SUCCESS);
            ASSERT_EQ(zeCommandListAppendMemoryCopy(list2, dst, tmp, copySize, nullptr, 0, nullptr),
                      ZE_RESULT_SUCCESS);
        }

        ASSERT_EQ(zeCommandListClose(list2), ZE_RESULT_SUCCESS);
    }

    std::vector<ze_command_list_handle_t> commandLists;
    for (auto &scopedList : scopedLists)
        commandLists.push_back(scopedList.get());

    ASSERT_EQ(
        zeCommandQueueExecuteCommandLists(queue, commandLists.size(), commandLists.data(), nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);

    for (size_t listIndex = 0; listIndex < listCount; listIndex++) {
        uint64_t *dst =
            static_cast<uint64_t *>(hostMems[listIndex].get()) + pointerStep * (copyCount - 1);
        EXPECT_EQ(*dst, referenceValue | listIndex);
        TRACE_BUF(hostMems[listIndex].get(), allocSize);
    }
}
