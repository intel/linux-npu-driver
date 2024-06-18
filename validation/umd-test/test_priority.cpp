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

TEST_F(CommandQueuePriority, executeManyLowPriorityJobsExpectHighPriorityJobCompletesFirst) {
    SKIP_NO_HWS("Test can be run in HWS mode");

    size_t lowJobCount = 4;
    size_t tsOffset = 16;
    auto tsMem = AllocDeviceMemory((lowJobCount + 1) * tsOffset * sizeof(uint64_t));
    uint64_t *ts = static_cast<uint64_t *>(tsMem.get());

    ze_event_handle_t hDeviceEvent = nullptr;
    EXPECT_EQ(createEvent(&hDeviceEvent), ZE_RESULT_SUCCESS);

    ze_event_handle_t hHostEvent = nullptr;
    EXPECT_EQ(createEvent(&hHostEvent), ZE_RESULT_SUCCESS);

    std::vector<ze_command_queue_handle_t> hQueueLows(lowJobCount, nullptr);
    for (auto &hQueueLow : hQueueLows) {
        EXPECT_EQ(createCommandQueue(ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_LOW, &hQueueLow),
                  ZE_RESULT_SUCCESS);
    }

    ze_command_queue_handle_t hQueueHigh = nullptr;
    EXPECT_EQ(createCommandQueue(ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_HIGH, &hQueueHigh),
              ZE_RESULT_SUCCESS);

    std::vector<ze_command_list_handle_t> hListLows(lowJobCount, nullptr);
    for (auto &hListLow : hListLows) {
        EXPECT_EQ(createCommandList(&hListLow), ZE_RESULT_SUCCESS);
    }

    ze_command_list_handle_t hListHigh = nullptr;
    EXPECT_EQ(createCommandList(&hListHigh), ZE_RESULT_SUCCESS);

    for (size_t i = 0; i < lowJobCount; i++) {
        EXPECT_EQ(zeCommandListAppendWaitOnEvents(hListLows[i], 1, &hHostEvent), ZE_RESULT_SUCCESS);
        EXPECT_EQ(zeCommandListAppendWriteGlobalTimestamp(hListLows[i],
                                                          ts + tsOffset * i,
                                                          nullptr,
                                                          0,
                                                          nullptr),
                  ZE_RESULT_SUCCESS);
        EXPECT_EQ(zeCommandListClose(hListLows[i]), ZE_RESULT_SUCCESS);
    }

    EXPECT_EQ(zeCommandListAppendSignalEvent(hListHigh, hDeviceEvent), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeCommandListAppendWaitOnEvents(hListHigh, 1, &hHostEvent), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeCommandListAppendWriteGlobalTimestamp(hListHigh,
                                                      ts + tsOffset * lowJobCount,
                                                      nullptr,
                                                      0,
                                                      nullptr),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeCommandListClose(hListHigh), ZE_RESULT_SUCCESS);

    for (size_t i = 0; i < lowJobCount; i++) {
        EXPECT_EQ(zeCommandQueueExecuteCommandLists(hQueueLows[i], 1, &hListLows[i], nullptr),
                  ZE_RESULT_SUCCESS);
    }
    EXPECT_EQ(zeCommandQueueExecuteCommandLists(hQueueHigh, 1, &hListHigh, nullptr),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(zeEventHostSynchronize(hDeviceEvent, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeEventHostSignal(hHostEvent), ZE_RESULT_SUCCESS);

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
