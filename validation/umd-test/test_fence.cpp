/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <chrono>
#include <future>
#include <thread>

class Fence : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();

        ze_command_queue_desc_t cmdQueueDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                             .pNext = nullptr,
                                             .ordinal = computeGrpOrdinal,
                                             .index = 0,
                                             .flags = 0,
                                             .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                             .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};

        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        mem = AllocSharedMemory(size);
        ts = static_cast<uint64_t *>(mem.get());
    }

    void TearDown() override { UmdTest::TearDown(); }

    const size_t size = sizeof(uint64_t);
    ze_fence_desc_t fenceDesc = {ZE_STRUCTURE_TYPE_FENCE_DESC, nullptr, 0};
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_fence_handle_t> scopedFence = nullptr;
    ze_command_queue_handle_t queue = nullptr;
    std::shared_ptr<void> mem = nullptr;
    ze_fence_handle_t fence = nullptr;
    uint64_t *ts = nullptr;
    ze_result_t ret;
};

// TODO: Validation layer is disabled. OpenVino issue: EISW-113275
TEST_F(Fence, DISABLED_CreateFenceWithQueueEqualToNull) {
    ASSERT_EQ(zeFenceCreate(nullptr, &fenceDesc, &fence), ZE_RESULT_ERROR_INVALID_NULL_HANDLE);
}

// TODO: Validation layer is disabled. OpenVino issue: EISW-113275
TEST_F(Fence, DISABLED_DestroyUsingNullptr) {
    ASSERT_EQ(zeFenceDestroy(nullptr), ZE_RESULT_ERROR_INVALID_NULL_HANDLE);
}

TEST_F(Fence, CreateFenceExpectNotReadyStatus) {
    scopedFence = zeScope::fenceCreate(queue, fenceDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    fence = scopedFence.get();

    ASSERT_EQ(zeFenceQueryStatus(fence), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeFenceHostSynchronize(fence, 0), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeFenceHostSynchronize(fence, 1), ZE_RESULT_NOT_READY);
}

TEST_F(Fence, CreateFenceInSignaledStateExpectReadyStatus) {
    fenceDesc.flags = ZE_FENCE_FLAG_SIGNALED;
    scopedFence = zeScope::fenceCreate(queue, fenceDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    fence = scopedFence.get();

    ASSERT_EQ(zeFenceQueryStatus(fence), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeFenceHostSynchronize(fence, 0), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeFenceHostSynchronize(fence, 1), ZE_RESULT_SUCCESS);
}

TEST_F(Fence, CreateFenceInSignaledStateResetFenceStateExpectNotReadyStatus) {
    fenceDesc.flags = ZE_FENCE_FLAG_SIGNALED;
    scopedFence = zeScope::fenceCreate(queue, fenceDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    fence = scopedFence.get();

    ASSERT_EQ(zeFenceReset(fence), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeFenceQueryStatus(fence), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeFenceHostSynchronize(fence, 0), ZE_RESULT_NOT_READY);
    ASSERT_EQ(zeFenceHostSynchronize(fence, 1), ZE_RESULT_NOT_READY);
}

class FenceSync : public Fence {
  public:
    void SetUp() override {
        Fence::SetUp();

        scopedFence = zeScope::fenceCreate(queue, fenceDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        fence = scopedFence.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();
    }

    void TearDown() override { Fence::TearDown(); }

    ze_command_list_desc_t cmdListDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                          .pNext = nullptr,
                                          .commandQueueGroupOrdinal = 0,
                                          .flags = 0};
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
    ze_command_list_handle_t list = nullptr;

    // Functor for threaded use case of zeFenceHostSynchronize with a promise as parameter
    // umd-test will utilize std::thread to spawn a new thread with this functor to
    // perform zeFenceHostSynchronize with BLOCKING scenario (UINT64_MAX)
    // While the std::promise is used to checked for the zeFenceHostSynchronize
    // result in the main thread with timeout.
    void threadedFenceHostSyncWrapper(std::promise<_ze_result_t> &&promise) {
        // This thread has to be killed instantly, otherwise SEGFAULT could happen
        // if cancellation is DEFERRED post handleFailure()
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        ASSERT_NE(nullptr, fence);
        promise.set_value(zeFenceHostSynchronize(fence, UINT64_MAX));
    }
};

TEST_F(FenceSync, SynchronizeCommandListExecutionUsingFenceWithMaxUint64) {
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, fence), ZE_RESULT_SUCCESS);

    // Use std::promise to track if zeFenceHostSynchronize completed in separate
    // thread. In scenario that blocking call with UINT64_MAX or TDR mechanism
    // unable to fail gracefully for UMD, userspace app can rely on timeout
    // to kill the zeFenceHostSynchronize thread.
    std::promise<_ze_result_t> promise;
    std::future<_ze_result_t> futureSync = promise.get_future();
    std::thread cmdQueueSyncThread(&FenceSync::threadedFenceHostSyncWrapper,
                                   this,
                                   std::move(promise));

    // Keep the native pthread handle to cancellation, prior to detach()
    pthread_t threadID = cmdQueueSyncThread.native_handle();
    cmdQueueSyncThread.detach();

    // Userspace app defined timeout for decision on killing the zeFenceHostSynchronize
    // thread
    std::chrono::steady_clock::time_point asyncTimeOut =
        std::chrono::steady_clock::now() + std::chrono::microseconds(syncTimeout);

    ze_result_t result = ZE_RESULT_NOT_READY;
    if (std::future_status::ready == futureSync.wait_until(asyncTimeOut)) {
        result = futureSync.get();
    } else {
        // asyncTimeOut and zeFenceHostSynchronize hasn't returned
        // userspace app needs to cancel the thread prior to handleFailure()
        // which free the objects
        pthread_cancel(threadID);
    }
    ASSERT_EQ(result, ZE_RESULT_SUCCESS) << "TIMEOUT from threadedFenceHostSyncWrapper";
    ASSERT_EQ(zeFenceHostSynchronize(fence, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeFenceQueryStatus(fence), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}

TEST_F(FenceSync, SynchronizeCommandListExecutionUsingFence) {
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, fence), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeFenceHostSynchronize(fence, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeFenceQueryStatus(fence), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}

TEST_F(FenceSync, ReuseFenceThatWasSignaled) {
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, fence), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeFenceHostSynchronize(fence, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";

    ASSERT_EQ(zeFenceReset(fence), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeFenceQueryStatus(fence), ZE_RESULT_NOT_READY);

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, fence), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeFenceHostSynchronize(fence, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}

TEST_F(FenceSync, SynchronizeCommandListExecutionUsingQueueSynchronizeExpectSignaledFence) {
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, fence), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeFenceQueryStatus(fence), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeFenceHostSynchronize(fence, 0), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}

TEST_F(FenceSync, CallFenceHostSynchronizeTwiceExpectSignaledState) {
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, fence), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeFenceHostSynchronize(fence, syncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeFenceHostSynchronize(fence, 10), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}

TEST_F(FenceSync, ExecuteAndSynchronizeMultipleCommandQueuesUsingMultipleFences) {
    ze_command_queue_desc_t copyCmdQueueDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                             .pNext = nullptr,
                                             .ordinal = copyGrpOrdinal,
                                             .index = 0,
                                             .flags = 0,
                                             .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                             .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};
    auto scopedQueue2 = zeScope::commandQueueCreate(zeContext, zeDevice, copyCmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto queue2 = scopedQueue2.get();

    ze_command_list_desc_t copycmdListDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                              .pNext = nullptr,
                                              .commandQueueGroupOrdinal = copyGrpOrdinal,
                                              .flags = 0};
    auto scopedList2 = zeScope::commandListCreate(zeContext, zeDevice, copycmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto list2 = scopedList2.get();

    auto scopedFence2 = zeScope::fenceCreate(queue, fenceDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto fence2 = scopedFence2.get();

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list2, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list2), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, fence), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeFenceHostSynchronize(fence, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";

    uint64_t prev_ts = *ts;

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue2, 1, &list2, fence2), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeFenceHostSynchronize(fence2, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, prev_ts) << "Timestamp should be different from previous value";
}

TEST_F(FenceSync, UseZeFenceHostSynchronizeInLoop) {
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, fence), ZE_RESULT_SUCCESS);

    auto timeout = std::chrono::steady_clock::now() + std::chrono::nanoseconds(syncTimeout);
    while (zeFenceHostSynchronize(fence, 0) == ZE_RESULT_NOT_READY) {
        if (std::chrono::steady_clock::now() > timeout)
            break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    ASSERT_EQ(zeFenceHostSynchronize(fence, 0), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeFenceQueryStatus(fence), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should be different from 0";
}
