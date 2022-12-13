/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gtest/gtest.h"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/tools/source/metrics/metric_query.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"

#include <level_zero/ze_api.h>
#include <firmware/vpu_jsm_job_cmd_api.h>

namespace L0 {
namespace ult {

struct CommandListFixture : CommandQueueFixture {
    void SetUp() override {
        CommandQueueFixture::SetUp();

        ze_command_list_desc_t cmdlistDesc = {};
        cmdlistDesc.commandQueueGroupOrdinal = getComputeQueueOrdinal();
        ze_command_list_handle_t hCommandList = nullptr;

        ze_result_t result = context->createCommandList(device, &cmdlistDesc, &hCommandList);
        EXPECT_EQ(ZE_RESULT_SUCCESS, result);
        commandList = L0::CommandList::fromHandle(hCommandList);

        // Alloc 4KB device mem.
        ptrAlloc = (uint64_t *)ctx->createSharedMemAlloc(testAllocSize);
        ptrAlloc2 = (uint64_t *)ctx->createSharedMemAlloc(testAllocSize);
        ASSERT_NE(nullptr, ptrAlloc);
        ASSERT_NE(nullptr, ptrAlloc2);

        // Prepare event sync objects (size of 5 events pool)
        ze_event_pool_desc_t eventPoolDesc = {.stype = ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                              .pNext = nullptr,
                                              .flags = ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                              .count = 5};
        auto hDevice = device->toHandle();
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  context->createEventPool(&eventPoolDesc, 1, &hDevice, &eventPool));

        ze_event_desc_t eventDesc = {.stype = ZE_STRUCTURE_TYPE_EVENT_DESC,
                                     .pNext = nullptr,
                                     .index = 0,
                                     .signal = ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                     .wait = ZE_EVENT_POOL_FLAG_HOST_VISIBLE};
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventCreate(eventPool, &eventDesc, &event0));
        ASSERT_NE(nullptr, event0);
        eventDesc.index = 1;
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventCreate(eventPool, &eventDesc, &event1));
        ASSERT_NE(nullptr, event1);
        eventDesc.index = 2;
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventCreate(eventPool, &eventDesc, &event2));
        ASSERT_NE(nullptr, event2);
        eventDesc.index = 3;
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventCreate(eventPool, &eventDesc, &event3));
        ASSERT_NE(nullptr, event3);
        eventDesc.index = 4;
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventCreate(eventPool, &eventDesc, &event4));
        ASSERT_NE(nullptr, event4);
    }

    void TearDown() override {
        ze_result_t result = commandList->destroy();
        EXPECT_EQ(ZE_RESULT_SUCCESS, result);

        // Events / event pool.
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(event0));
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(event1));
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(event2));
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(event3));
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(event4));
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventPoolDestroy(eventPool));

        // Memory free'ing
        EXPECT_TRUE(ctx->freeMemAlloc(ptrAlloc));
        EXPECT_TRUE(ctx->freeMemAlloc(ptrAlloc2));

        CommandQueueFixture::TearDown();
    }
    // Events.
    ze_event_pool_handle_t eventPool = nullptr;
    ze_event_handle_t event0 = nullptr;
    ze_event_handle_t event1 = nullptr;
    ze_event_handle_t event2 = nullptr;
    ze_event_handle_t event3 = nullptr;
    ze_event_handle_t event4 = nullptr;

    L0::CommandList *commandList = nullptr;

    const size_t testAllocSize = 4 * 1024; // Default memory allocation size for testing.
    uint64_t *ptrAlloc = nullptr;
    uint64_t *ptrAlloc2 = nullptr;
};

using CommandListApiTest = Test<CommandListFixture>;

TEST_F(CommandListApiTest, whenCalledCloseSuccessIsReturned) {
    auto result = commandList->close();
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
}

TEST_F(CommandListApiTest, whenCalledAppendWriteGlobalTimestampWithInvalidParamsFailureIsReturned) {
    auto result = zeCommandListAppendWriteGlobalTimestamp(nullptr, ptrAlloc, nullptr, 0, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, result);

    result = zeCommandListAppendWriteGlobalTimestamp(commandList, nullptr, nullptr, 0, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, result);

    result = zeCommandListAppendWriteGlobalTimestamp(commandList, ptrAlloc, nullptr, 1, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_SIZE, result);
}

TEST_F(CommandListApiTest, whenCalledAppendWriteGlobalTimestampSuccessIsReturned) {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              commandList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));

    auto result = commandList->close();
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    EXPECT_EQ(1UL, commandList->getNumCommands());
    EXPECT_EQ(VPU_CMD_TIMESTAMP, commandList->getNNCommands()[0]->getCommandType());
}

TEST_F(CommandListApiTest, whenCalledAppendWriteGlobalTimestampAfterCloseFailureIsReturned) {
    // Close commandlist prematurely
    commandList->close();

    // Appending new command to same commandlist which is closed
    auto result = commandList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr);

    EXPECT_EQ(ZE_RESULT_ERROR_NOT_AVAILABLE, result);
}

TEST_F(CommandListApiTest, handleInvalidEventObjects) {
    auto result = commandList->appendWriteGlobalTimestamp(ptrAlloc, event0, 3, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_SIZE, result);

    ze_event_handle_t waitOnEvents[] = {event1, event2, event3};
    // From the L0 spec (1.2.13)
    // numWaitEvents: [in][optional] number of events to wait on before executing query;
    //  must be 0 if `nullptr == phWaitEvents`
    result = commandList->appendWriteGlobalTimestamp(ptrAlloc, event0, 0, waitOnEvents);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
}

TEST_F(CommandListApiTest, eventSyncObjectsAttachedWithTSCommand) {
    // Append TS command with sync objects.
    ze_event_handle_t waitOnEvents[] = {event1, event2, event3};
    auto result = commandList->appendWriteGlobalTimestamp(ptrAlloc, event0, 3, waitOnEvents);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    result = commandList->close();
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    // 3 wait on + 1 TS command + 1 signal.
    ASSERT_EQ(5UL, commandList->getNumCommands());

    // Expected command order.
    //  3 x Wait on event | TS | signal event.
    std::array<vpu_cmd_type, 5> expectedCommandTypes = {VPU_CMD_FENCE_WAIT,
                                                        VPU_CMD_FENCE_WAIT,
                                                        VPU_CMD_FENCE_WAIT,
                                                        VPU_CMD_TIMESTAMP,
                                                        VPU_CMD_FENCE_SIGNAL};

    ASSERT_EQ(expectedCommandTypes.size(), commandList->getNNCommands().size());
    for (size_t i = 0; i < expectedCommandTypes.size(); i++)
        ASSERT_EQ(expectedCommandTypes[i], commandList->getNNCommands()[i]->getCommandType());
}

TEST_F(CommandListApiTest, whenCalledAppendMemoryCopyWithCorrectProgramSequenceSuccessIsReturned) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE,
              zeCommandListAppendMemoryCopy(nullptr, nullptr, nullptr, 0u, nullptr, 0u, nullptr));

    // Testing that appendMemoryCopy will fail due to unallocated srcptr
    void *srcPtr = nullptr;
    ze_result_t result =
        commandList->appendMemoryCopy((void *)ptrAlloc, srcPtr, testAllocSize, nullptr, 0, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, result);

    // Creating new unique ptr and heap resident for source pointer
    srcPtr = ctx->createHostMemAlloc(testAllocSize);
    EXPECT_NE(ptrAlloc, srcPtr);

    result =
        commandList->appendMemoryCopy((void *)ptrAlloc, srcPtr, testAllocSize, nullptr, 0, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    EXPECT_TRUE(ctx->freeMemAlloc(srcPtr));
}

TEST_F(CommandListApiTest, eventSyncObjectsAttachedWithMemoryCopyCommand) {
    // Append copy command with sync objects.
    ze_event_handle_t waitOnEvents[] = {event1, event2, event3};
    void *srcPtr = ctx->createSharedMemAlloc(testAllocSize);
    ASSERT_NE(nullptr, srcPtr);

    ze_result_t result = commandList->appendMemoryCopy(static_cast<void *>(ptrAlloc),
                                                       srcPtr,
                                                       testAllocSize,
                                                       event0,
                                                       3,
                                                       waitOnEvents);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    // TS + 3 wait on + 1 copy command + 1 signal.
    ASSERT_EQ(5UL, commandList->getNumCommands());

    // Expected command order.
    //  3 x Wait on event | L2L copy | signal event.
    std::array<vpu_cmd_type, 5> expectedCommandTypes = {VPU_CMD_FENCE_WAIT,
                                                        VPU_CMD_FENCE_WAIT,
                                                        VPU_CMD_FENCE_WAIT,
                                                        VPU_CMD_COPY_LOCAL_TO_LOCAL,
                                                        VPU_CMD_FENCE_SIGNAL};

    ASSERT_EQ(expectedCommandTypes.size(), commandList->getNNCommands().size());
    for (size_t i = 0; i < expectedCommandTypes.size(); i++)
        ASSERT_EQ(expectedCommandTypes[i], commandList->getNNCommands()[i]->getCommandType());

    EXPECT_TRUE(ctx->freeMemAlloc(srcPtr));
}

TEST_F(CommandListApiTest, whenCalledAppendBarrierWithCorrectConditionsSuccessIsReturned) {
    uint32_t numWaitEvents = 1;

    // Testing that appendBarrier will fail due to nullptr phWaitEvents and greater than
    // zero numWaitEvents
    ze_result_t result = commandList->appendBarrier(nullptr, numWaitEvents, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_SIZE, result);

    numWaitEvents = 0;

    result = commandList->appendBarrier(nullptr, numWaitEvents, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
}

TEST_F(CommandListApiTest, whenCalledAppendMetricQueryBeginEndWithoutProperInitFailureIsReturned) {
    ze_result_t result = commandList->appendMetricQueryBegin(nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, result);

    result = commandList->appendMetricQueryEnd(nullptr, nullptr, 0u, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, result);
}

struct CommandListEventApiTest : Test<CommandListFixture> {
    void SetUp() override {
        CommandListFixture::SetUp();
        ASSERT_NE(nullptr, commandList);

        // Event pool.
        ze_event_pool_desc_t evPoolDesc = {};
        evPoolDesc.stype = ZE_STRUCTURE_TYPE_EVENT_POOL_DESC;
        evPoolDesc.count = evPoolCap;
        evPoolDesc.flags = ZE_EVENT_POOL_FLAG_HOST_VISIBLE;

        ASSERT_NE(nullptr, context);
        ASSERT_NE(nullptr, device);
        ze_context_handle_t hContext = context->toHandle();
        ze_device_handle_t hDevice = device->toHandle();

        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeEventPoolCreate(hContext, &evPoolDesc, 1, &(hDevice), &hEvPool));
        ASSERT_NE(nullptr, hEvPool);

        // Event.
        ze_event_desc_t evDesc = {};
        evDesc.stype = ZE_STRUCTURE_TYPE_EVENT_DESC;
        evDesc.index = 0;
        evDesc.signal = ZE_EVENT_SCOPE_FLAG_HOST;
        evDesc.wait = ZE_EVENT_SCOPE_FLAG_HOST;

        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventCreate(hEvPool, &evDesc, &hEvent));
        ASSERT_NE(nullptr, hEvent);
    }

    void TearDown() override {
        if (hEvent != nullptr) {
            ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(hEvent));
        }
        if (hEvPool != nullptr) {
            ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventPoolDestroy(hEvPool));
        }

        CommandListFixture::TearDown();
    }

    const uint32_t evPoolCap = 5;
    ze_event_pool_handle_t hEvPool = nullptr;
    ze_event_handle_t hEvent = nullptr;
};

TEST_F(CommandListEventApiTest, givenCallAppendSignalEventSuccessIsReturned) {
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendSignalEvent(commandList->toHandle(), hEvent));
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(commandList->toHandle()));

    EXPECT_EQ(1u, commandList->getNNCommands().size());
    EXPECT_EQ(VPU_CMD_FENCE_SIGNAL, commandList->getNNCommands()[0]->getCommandType());
}

TEST_F(CommandListEventApiTest, givenCallAppendSignalEventWithInvalidParamsReturnFiailure) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, zeCommandListAppendSignalEvent(nullptr, hEvent));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE,
              zeCommandListAppendSignalEvent(commandList->toHandle(), nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE,
              zeCommandListAppendSignalEvent(nullptr, nullptr));
}

TEST_F(CommandListEventApiTest, givenCallAppendResetEventSuccessIsReturned) {
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListAppendEventReset(commandList->toHandle(), hEvent));
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(commandList->toHandle()));

    EXPECT_EQ(1u, commandList->getNNCommands().size());
    EXPECT_EQ(VPU_CMD_FENCE_SIGNAL, commandList->getNNCommands()[0]->getCommandType());
}

TEST_F(CommandListEventApiTest, givenCallAppendResetEventWithInvalidParamsReturnFiailure) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, zeCommandListAppendEventReset(nullptr, hEvent));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE,
              zeCommandListAppendEventReset(commandList->toHandle(), nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, zeCommandListAppendEventReset(nullptr, nullptr));
}

TEST_F(CommandListEventApiTest, givenCallAppendWaitEventSuccessIsReturned) {
    ze_event_desc_t evDesc = {};
    evDesc.stype = ZE_STRUCTURE_TYPE_EVENT_DESC;
    evDesc.index = 0;
    evDesc.signal = ZE_EVENT_SCOPE_FLAG_HOST;
    evDesc.wait = ZE_EVENT_SCOPE_FLAG_HOST;

    ze_event_handle_t phEvent[evPoolCap];
    phEvent[0] = hEvent;
    for (uint32_t i = 1; i < evPoolCap; ++i) {
        evDesc.index = i;
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventCreate(hEvPool, &evDesc, &phEvent[i]));
    }
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListAppendWaitOnEvents(commandList->toHandle(), evPoolCap, phEvent));
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(commandList->toHandle()));

    EXPECT_EQ(evPoolCap, commandList->getNNCommands().size());
    for (const auto &cmd : commandList->getNNCommands())
        EXPECT_EQ(VPU_CMD_FENCE_WAIT, cmd->getCommandType());

    for (uint32_t i = 1; i < evPoolCap; ++i)
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(phEvent[i]));
}

TEST_F(CommandListEventApiTest, givenCallAppendWaitEventWithInvalidParamsReturnFiailure) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE,
              zeCommandListAppendWaitOnEvents(nullptr, 1, &hEvent));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              zeCommandListAppendWaitOnEvents(commandList->toHandle(), 1, nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE,
              zeCommandListAppendWaitOnEvents(nullptr, 1, nullptr));
}

} // namespace ult
} // namespace L0
