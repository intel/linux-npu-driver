/*
 * Copyright (C) 2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"
#include "ze_scope.hpp"

#include <chrono>
#include <future>
#include <ze_api.h>

class CommandQueuePriority : public UmdTest {
  public:
    ze_result_t createCommandQueue(ze_command_queue_priority_t priority,
                                   ze_command_queue_handle_t *handle) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        ze_command_queue_desc_t desc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                        .pNext = nullptr,
                                        .ordinal = 0,
                                        .index = 0,
                                        .flags = 0,
                                        .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                        .priority = priority};
        auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, desc, ret);
        std::lock_guard<std::mutex> lock(mutex);
        scopedQueueVec.push_back(std::move(scopedQueue));
        *handle = scopedQueueVec.back().get();
        return ret;
    }

    ze_result_t createCommandList(ze_command_list_handle_t *handle) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        ze_command_list_desc_t desc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                       .pNext = nullptr,
                                       .commandQueueGroupOrdinal = 0,
                                       .flags = 0};
        auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, desc, ret);
        std::lock_guard<std::mutex> lock(mutex);
        scopedListVec.push_back(std::move(scopedList));
        *handle = scopedListVec.back().get();
        return ret;
    }

    ze_result_t createEvent(ze_event_handle_t *handle) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        const ze_event_pool_desc_t eventPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                                    nullptr,
                                                    ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                                    1};

        auto scopedEventPool = zeScope::eventPoolCreate(zeContext, eventPoolDesc, 1, zeDevice, ret);
        if (ret != ZE_RESULT_SUCCESS)
            return ret;

        ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                     nullptr,
                                     0,
                                     ZE_EVENT_SCOPE_FLAG_HOST,
                                     ZE_EVENT_SCOPE_FLAG_HOST};
        auto scopedEvent = zeScope::eventCreate(scopedEventPool.get(), eventDesc, ret);

        std::lock_guard<std::mutex> lock(mutex);
        scopedEventPoolVec.push_back(std::move(scopedEventPool));
        scopedEventVec.push_back(std::move(scopedEvent));
        *handle = scopedEventVec.back().get();
        return ret;
    }

    void
    executeCopyCommand(ze_command_queue_priority_t priority, size_t memSize, size_t timeExecMs) {
        ze_command_queue_handle_t hQueue = nullptr;
        EXPECT_EQ(createCommandQueue(priority, &hQueue), ZE_RESULT_SUCCESS);

        ze_command_list_handle_t hList = nullptr;
        EXPECT_EQ(createCommandList(&hList), ZE_RESULT_SUCCESS);

        auto srcMem = AllocDeviceMemory(memSize);
        memset(srcMem.get(), 'c', memSize);
        auto dstMem = AllocDeviceMemory(memSize);

        EXPECT_EQ(zeCommandListAppendMemoryCopy(hList,
                                                dstMem.get(),
                                                srcMem.get(),
                                                memSize,
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);
        EXPECT_EQ(zeCommandListClose(hList), ZE_RESULT_SUCCESS);

        auto timeout = std::chrono::steady_clock::now() + std::chrono::microseconds(timeExecMs);
        do {
            EXPECT_EQ(zeCommandQueueExecuteCommandLists(hQueue, 1, &hList, nullptr),
                      ZE_RESULT_SUCCESS);
            EXPECT_EQ(zeCommandQueueSynchronize(hQueue, syncTimeout), ZE_RESULT_SUCCESS);
            EXPECT_EQ(memcmp(dstMem.get(), srcMem.get(), memSize), ZE_RESULT_SUCCESS);
        } while (std::chrono::steady_clock::now() < timeout);
    }

    std::mutex mutex;
    std::vector<zeScope::SharedPtr<ze_command_queue_handle_t>> scopedQueueVec;
    std::vector<zeScope::SharedPtr<ze_command_list_handle_t>> scopedListVec;
    std::vector<zeScope::SharedPtr<ze_event_pool_handle_t>> scopedEventPoolVec;
    std::vector<zeScope::SharedPtr<ze_event_handle_t>> scopedEventVec;

    std::vector<ze_command_queue_priority_t> priorities = {
        ZE_COMMAND_QUEUE_PRIORITY_NORMAL,
        ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_LOW,
        ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_HIGH,
    };
};

TEST_F(CommandQueuePriority, createQueueWithDifferentPriority) {
    ze_command_queue_handle_t handle = nullptr;
    for (const auto &priority : priorities) {
        EXPECT_EQ(createCommandQueue(priority, &handle), ZE_RESULT_SUCCESS);
    }
}

TEST_F(CommandQueuePriority, executeCopyUsingDifferentPrioritySequentially) {
    for (const auto &priority : priorities) {
        executeCopyCommand(priority, 16 * KB, 20);
    }
}

TEST_F(CommandQueuePriority, executeCopyUsingDifferentPrioritySimultaneously) {
    auto job = [this](ze_command_queue_priority_t priority) {
        executeCopyCommand(priority, 16 * KB, 20);
    };
    std::vector<std::future<void>> tasks;
    for (const auto &priority : priorities) {
        tasks.push_back(std::async(std::launch::async, job, priority));
    }
    for (const auto &task : tasks) {
        task.wait();
    }
}

TEST_F(CommandQueuePriority, highPriorityJobTimestampPrecedesLowPriorityJobs) {
    SKIP_NO_HWS("Test can be run in HWS mode");

    size_t lowJobCount = 4;
    size_t tsOffset = 64 / sizeof(uint64_t);
    auto tsMem = AllocDeviceMemory((lowJobCount + 1) * tsOffset * sizeof(uint64_t));
    ASSERT_NE(tsMem, nullptr);
    uint64_t *ts = static_cast<uint64_t *>(tsMem.get());

    // 1st event signals about job is loaded to the NPU queue
    ze_event_handle_t hEventHighPriorSubmit = nullptr;
    ASSERT_EQ(createEvent(&hEventHighPriorSubmit), ZE_RESULT_SUCCESS);

    // 2nd event signals about job is waiting in the NPU queue
    ze_event_handle_t hEventHighPriorJobReady = nullptr;
    ASSERT_EQ(createEvent(&hEventHighPriorJobReady), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventHostSignal(hEventHighPriorJobReady), ZE_RESULT_SUCCESS);

    // 3rd event holds the low priority jobs until the high priority job is in the NPU queue
    ze_event_handle_t hEventBlock = nullptr;
    ASSERT_EQ(createEvent(&hEventBlock), ZE_RESULT_SUCCESS);

    std::vector<ze_command_queue_handle_t> hQueueLows(lowJobCount, nullptr);
    for (auto &hQueueLow : hQueueLows) {
        ASSERT_EQ(createCommandQueue(ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_LOW, &hQueueLow),
                  ZE_RESULT_SUCCESS);
    }

    ze_command_queue_handle_t hQueueHigh = nullptr;
    ASSERT_EQ(createCommandQueue(ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_HIGH, &hQueueHigh),
              ZE_RESULT_SUCCESS);

    std::vector<ze_command_list_handle_t> hListLows(lowJobCount, nullptr);
    for (auto &hListLow : hListLows) {
        ASSERT_EQ(createCommandList(&hListLow), ZE_RESULT_SUCCESS);
    }

    ze_command_list_handle_t hListHigh = nullptr;
    ASSERT_EQ(createCommandList(&hListHigh), ZE_RESULT_SUCCESS);

    for (size_t i = 0; i < lowJobCount; i++) {
        ASSERT_EQ(zeCommandListAppendWaitOnEvents(hListLows[i], 1, &hEventBlock),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(hListLows[i],
                                                          ts + tsOffset * i,
                                                          nullptr,
                                                          0,
                                                          nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(hListLows[i]), ZE_RESULT_SUCCESS);
    }

    // Linux NPU driver breaks down the command list into 2 or more jobs if signal event is used.
    // To avoid this behavior, we can use event reset instead of signal event.
    ASSERT_EQ(zeCommandListAppendSignalEvent(hListHigh, hEventHighPriorSubmit), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendEventReset(hListHigh, hEventHighPriorJobReady), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendWaitOnEvents(hListHigh, 1, &hEventBlock), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(hListHigh,
                                                      ts + tsOffset * lowJobCount,
                                                      nullptr,
                                                      0,
                                                      nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(hListHigh), ZE_RESULT_SUCCESS);

    for (size_t i = 0; i < lowJobCount; i++) {
        EXPECT_EQ(zeCommandQueueExecuteCommandLists(hQueueLows[i], 1, &hListLows[i], nullptr),
                  ZE_RESULT_SUCCESS);
    }
    EXPECT_EQ(zeCommandQueueExecuteCommandLists(hQueueHigh, 1, &hListHigh, nullptr),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(zeEventHostSynchronize(hEventHighPriorSubmit, syncTimeout), ZE_RESULT_SUCCESS);
    int retries = 10;
    while (zeEventQueryStatus(hEventHighPriorJobReady) != ZE_RESULT_NOT_READY && retries-- > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    EXPECT_EQ(zeEventQueryStatus(hEventHighPriorJobReady), ZE_RESULT_NOT_READY)
        << "High priority wait job might not have reached the NPU queue yet";
    EXPECT_EQ(zeEventHostSignal(hEventBlock), ZE_RESULT_SUCCESS);

    for (auto &hQueueLow : hQueueLows) {
        EXPECT_EQ(zeCommandQueueSynchronize(hQueueLow, syncTimeout), ZE_RESULT_SUCCESS);
    }
    EXPECT_EQ(zeCommandQueueSynchronize(hQueueHigh, syncTimeout), ZE_RESULT_SUCCESS);

    uint64_t tsHigh = *(ts + tsOffset * lowJobCount);
    for (size_t i = 0; i < lowJobCount; i++) {
        uint64_t tsLow = *(ts + tsOffset * i);
        EXPECT_LT(tsHigh, tsLow) << "Low priority job[" << i
                                 << "] set timestamp earlier then job with high priority";
    }
}

TEST_F(CommandQueuePriority, setPriorityExtUpdatesQueuePriority) {
    if (!Environment::getInstance()->isDriverExtensionSupported(
            ZE_COMMAND_QUEUE_SET_PRIORITY_EXT_NAME,
            ZE_COMMAND_QUEUE_SET_PRIORITY_EXT_VERSION_1_0)) {
        GTEST_SKIP() << "Kernel driver does not support changing command queue priority";
    }

    ze_command_queue_handle_t hQueue = nullptr;
    ASSERT_EQ(createCommandQueue(ZE_COMMAND_QUEUE_PRIORITY_NORMAL, &hQueue), ZE_RESULT_SUCCESS);

    ze_command_queue_priority_t priority = ZE_COMMAND_QUEUE_PRIORITY_FORCE_UINT32;
    ASSERT_EQ(zeCommandQueueGetPriority(hQueue, &priority), ZE_RESULT_SUCCESS);
    ASSERT_EQ(priority, ZE_COMMAND_QUEUE_PRIORITY_NORMAL);

    EXPECT_EQ(zeCommandQueueSetPriorityExt(hQueue, ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_HIGH),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeCommandQueueGetPriority(hQueue, &priority), ZE_RESULT_SUCCESS);
    EXPECT_EQ(priority, ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_HIGH);

    EXPECT_EQ(zeCommandQueueSetPriorityExt(hQueue, ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_LOW),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeCommandQueueGetPriority(hQueue, &priority), ZE_RESULT_SUCCESS);
    EXPECT_EQ(priority, ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_LOW);

    EXPECT_EQ(zeCommandQueueSetPriorityExt(hQueue, ZE_COMMAND_QUEUE_PRIORITY_FORCE_UINT32),
              ZE_RESULT_ERROR_INVALID_ENUMERATION);
    EXPECT_EQ(zeCommandQueueSetPriorityExt(nullptr, ZE_COMMAND_QUEUE_PRIORITY_NORMAL),
              ZE_RESULT_ERROR_INVALID_NULL_HANDLE);
}

TEST_F(CommandQueuePriority, executeCopyAfterSetPriorityExt) {
    if (!Environment::getInstance()->isDriverExtensionSupported(
            ZE_COMMAND_QUEUE_SET_PRIORITY_EXT_NAME,
            ZE_COMMAND_QUEUE_SET_PRIORITY_EXT_VERSION_1_0)) {
        GTEST_SKIP() << "Kernel driver does not support changing command queue priority";
    }

    for (const auto &priority : priorities) {
        ze_command_queue_handle_t hQueue = nullptr;
        ASSERT_EQ(createCommandQueue(ZE_COMMAND_QUEUE_PRIORITY_NORMAL, &hQueue), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSetPriorityExt(hQueue, priority), ZE_RESULT_SUCCESS);

        ze_command_list_handle_t hList = nullptr;
        ASSERT_EQ(createCommandList(&hList), ZE_RESULT_SUCCESS);

        size_t memSize = 16 * KB;
        auto srcMem = AllocDeviceMemory(memSize);
        memset(srcMem.get(), 'c', memSize);
        auto dstMem = AllocDeviceMemory(memSize);

        ASSERT_EQ(zeCommandListAppendMemoryCopy(hList,
                                                dstMem.get(),
                                                srcMem.get(),
                                                memSize,
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(hList), ZE_RESULT_SUCCESS);

        EXPECT_EQ(zeCommandQueueExecuteCommandLists(hQueue, 1, &hList, nullptr), ZE_RESULT_SUCCESS);
        EXPECT_EQ(zeCommandQueueSynchronize(hQueue, syncTimeout), ZE_RESULT_SUCCESS);
        EXPECT_EQ(memcmp(dstMem.get(), srcMem.get(), memSize), ZE_RESULT_SUCCESS);
    }
}
