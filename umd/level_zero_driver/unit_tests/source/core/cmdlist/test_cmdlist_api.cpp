/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "api/vpu_jsm_job_cmd_api.h"
#include "gtest/gtest.h"
#include "level_zero_driver/source/cmdlist.hpp"
#include "level_zero_driver/source/cmdqueue.hpp"
#include "level_zero_driver/source/context.hpp"
#include "level_zero_driver/source/device.hpp"
#include "level_zero_driver/source/event.hpp"
#include "level_zero_driver/source/eventpool.hpp"
#include "level_zero_driver/source/ext/graph.hpp"
#include "level_zero_driver/source/metric.hpp"
#include "level_zero_driver/source/metric_query.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "level_zero_driver/unit_tests/options.hpp"
#include "level_zero_driver/unit_tests/utils.hpp"
#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <array>
#include <filesystem>
#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_ext.h>
#include <level_zero/ze_graph_profiling_ext.h>
#include <level_zero/zet_api.h>
#include <memory>
#include <string>
#include <vector>

namespace L0 {
namespace ult {

struct CommandListFixture : CommandQueueFixture {
    void SetUp() override {
        CommandQueueFixture::SetUp();

        ze_command_list_desc_t cmdListDesc = {};
        cmdListDesc.commandQueueGroupOrdinal = 0u;
        ze_command_list_handle_t hCommandList = nullptr;

        ze_result_t result = L0::CommandList::create(context, device, &cmdListDesc, &hCommandList);
        EXPECT_EQ(ZE_RESULT_SUCCESS, result);
        commandList = L0::CommandList::fromHandle(hCommandList);

        // Alloc 4KB device mem.
        ptrAlloc = reinterpret_cast<uint64_t *>(
            ctx->createMemAlloc(testAllocSize,
                                VPU::VPUBufferObject::Type::CachedFw,
                                VPU::VPUBufferObject::Location::Shared));
        ptrAlloc2 = reinterpret_cast<uint64_t *>(
            ctx->createMemAlloc(testAllocSize,
                                VPU::VPUBufferObject::Type::CachedFw,
                                VPU::VPUBufferObject::Location::Shared));
        ASSERT_NE(nullptr, ptrAlloc);
        ASSERT_NE(nullptr, ptrAlloc2);

        // Prepare event sync objects (size of 5 events pool)
        ze_event_pool_desc_t eventPoolDesc = {.stype = ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                              .pNext = nullptr,
                                              .flags = ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                              .count = 5};
        auto hDevice = device->toHandle();
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  L0::EventPool::create(context, &eventPoolDesc, 1, &hDevice, &eventPool));

        ze_event_desc_t eventDesc = {.stype = ZE_STRUCTURE_TYPE_EVENT_DESC,
                                     .pNext = nullptr,
                                     .index = 0,
                                     .signal = ZE_EVENT_SCOPE_FLAG_HOST,
                                     .wait = ZE_EVENT_SCOPE_FLAG_HOST};
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  L0::EventPool::fromHandle(eventPool)->createEvent(&eventDesc, &event0));
        ASSERT_NE(nullptr, event0);
        eventDesc.index = 1;
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  L0::EventPool::fromHandle(eventPool)->createEvent(&eventDesc, &event1));
        ASSERT_NE(nullptr, event1);
        eventDesc.index = 2;
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  L0::EventPool::fromHandle(eventPool)->createEvent(&eventDesc, &event2));
        ASSERT_NE(nullptr, event2);
        eventDesc.index = 3;
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  L0::EventPool::fromHandle(eventPool)->createEvent(&eventDesc, &event3));
        ASSERT_NE(nullptr, event3);
        eventDesc.index = 4;
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  L0::EventPool::fromHandle(eventPool)->createEvent(&eventDesc, &event4));
        ASSERT_NE(nullptr, event4);
    }

    void TearDown() override {
        ze_result_t result = commandList->destroy();
        EXPECT_EQ(ZE_RESULT_SUCCESS, result);

        // Events / event pool.
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(event0)->destroy());
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(event1)->destroy());
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(event2)->destroy());
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(event3)->destroy());
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(event4)->destroy());
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::EventPool::fromHandle(eventPool)->destroy());

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

struct CommandListGraphFixture : CommandListFixture {
    void SetUp() override {
        CommandListFixture::SetUp();

        ASSERT_FALSE(TestOptions::blobPath.empty()) << "Blob path has not been provided";

        loadBlobFromFile(TestOptions::blobPath, blob);
        ASSERT_NE(0u, blob.size());

        const ze_graph_desc_2_t graphDesc = {.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES,
                                             .pNext = nullptr,
                                             .format = ZE_GRAPH_FORMAT_NATIVE,
                                             .inputSize = blob.size(),
                                             .pInput = blob.data(),
                                             .pBuildFlags = nullptr,
                                             .flags = 0};

        auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
        ASSERT_EQ(ZE_RESULT_SUCCESS, res);
        ASSERT_NE(nullptr, hGraph);
        pGraph = L0::Graph::fromHandle(hGraph);
    }

    void TearDown() override {
        if (pGraph != nullptr) {
            pGraph->destroy();
        }

        CommandListFixture::TearDown();
    }

    L0::Graph *pGraph = nullptr;
    std::vector<uint8_t> blob;
    ze_graph_handle_t hGraph = nullptr;
};

struct CommandListMetricFixture : CommandListFixture {
    void SetUp() override {
        CommandListFixture::SetUp();

        ASSERT_EQ(device->metricGroupGet(&metricGroupCount, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_GT(metricGroupCount, 0u);

        metricGroups.resize(metricGroupCount);
        ASSERT_EQ(device->metricGroupGet(&metricGroupCount, metricGroups.data()),
                  ZE_RESULT_SUCCESS);
        ASSERT_NE(*metricGroups.data(), nullptr);

        // Activate 1st metric group
        ASSERT_EQ(context->activateMetricGroups(device->toHandle(), 1, &metricGroups[0]),
                  ZE_RESULT_SUCCESS);

        ASSERT_EQ(context->createMetricQueryPool(device->toHandle(),
                                                 metricGroups[0],
                                                 &desc,
                                                 &hMetricQueryPool),
                  ZE_RESULT_SUCCESS);
        ASSERT_NE(hMetricQueryPool, nullptr);

        ASSERT_EQ(
            L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(0u, &hMetricQuery),
            ZE_RESULT_SUCCESS);
        ASSERT_NE(hMetricQuery, nullptr);
    }

    void TearDown() override {
        if (context != nullptr && device != nullptr) {
            if (hMetricQuery) {
                ASSERT_EQ(L0::MetricQuery::fromHandle(hMetricQuery)->destroy(), ZE_RESULT_SUCCESS);
            }

            if (hMetricQueryPool) {
                ASSERT_EQ(L0::MetricQueryPool::fromHandle(hMetricQueryPool)->destroy(),
                          ZE_RESULT_SUCCESS);
            }

            // Deactivate all metric groups previously activated
            ASSERT_EQ(context->activateMetricGroups(device->toHandle(), 0, nullptr),
                      ZE_RESULT_SUCCESS);
        }

        CommandListFixture::TearDown();
    }

    uint32_t metricGroupCount = 0;
    std::vector<zet_metric_group_handle_t> metricGroups;
    std::vector<zet_metric_group_properties_t> groupProperties;

    zet_metric_query_pool_handle_t hMetricQueryPool = nullptr;
    zet_metric_query_pool_desc_t desc = {.stype = ZET_STRUCTURE_TYPE_METRIC_QUERY_POOL_DESC,
                                         .pNext = nullptr,
                                         .type = ZET_METRIC_QUERY_POOL_TYPE_PERFORMANCE,
                                         .count = 1u};

    zet_metric_query_handle_t hMetricQuery = nullptr;
};

using CommandListApiTest = Test<CommandListFixture>;
using CommandListGraphApiTest = Test<CommandListGraphFixture>;
using CommandListMetricsApiTest = Test<CommandListMetricFixture>;

TEST_F(CommandListApiTest, whenCalledCloseSuccessIsReturned) {
    auto result = commandList->close();
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
}

TEST_F(CommandListApiTest, whenCalledAppendWriteGlobalTimestampWithInvalidParamsFailureIsReturned) {
    auto result = L0::CommandList::fromHandle(commandList)
                      ->appendWriteGlobalTimestamp(nullptr, nullptr, 0, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, result);

    result = L0::CommandList::fromHandle(commandList)
                 ->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 1, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_SIZE, result);
}

TEST_F(CommandListApiTest, whenCalledAppendWriteGlobalTimestampSuccessIsReturned) {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              commandList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));

    auto result = commandList->close();
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    /* Timestamp is split by UMD to two commands aligned TS and copy */
    EXPECT_EQ(2UL, commandList->getNumCommands());
    EXPECT_EQ(VPU_CMD_TIMESTAMP, commandList->getCommands()[0]->getCommandType());
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

    // 3 wait on + 2 TS command + 1 signal.
    ASSERT_EQ(6UL, commandList->getNumCommands());

    // Expected command order.
    //  3 x Wait on event | TS | signal event.
    std::array<vpu_cmd_type, 6> expectedCommandTypes = {
        VPU_CMD_FENCE_WAIT,
        VPU_CMD_FENCE_WAIT,
        VPU_CMD_FENCE_WAIT,
        VPU_CMD_TIMESTAMP,           // TS to aligned buf
        VPU_CMD_COPY_LOCAL_TO_LOCAL, // Copy result of TS to unaligned buf
        VPU_CMD_FENCE_SIGNAL};

    ASSERT_EQ(expectedCommandTypes.size(), commandList->getCommands().size());
    for (size_t i = 0; i < expectedCommandTypes.size(); i++)
        ASSERT_EQ(expectedCommandTypes[i], commandList->getCommands()[i]->getCommandType());
}

TEST_F(CommandListApiTest, whenCalledAppendMemoryCopyWithCorrectProgramSequenceSuccessIsReturned) {
    // Testing that appendMemoryCopy will fail due to unallocated srcptr
    void *srcPtr = nullptr;
    ze_result_t result =
        commandList->appendMemoryCopy((void *)ptrAlloc, srcPtr, testAllocSize, nullptr, 0, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, result);

    // Creating new unique ptr and heap resident for source pointer
    srcPtr = ctx->createMemAlloc(testAllocSize,
                                 VPU::VPUBufferObject::Type::CachedShave,
                                 VPU::VPUBufferObject::Location::Host);
    EXPECT_NE(ptrAlloc, srcPtr);

    result =
        commandList->appendMemoryCopy((void *)ptrAlloc, srcPtr, testAllocSize, nullptr, 0, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    EXPECT_TRUE(ctx->freeMemAlloc(srcPtr));
}

TEST_F(CommandListApiTest, eventSyncObjectsAttachedWithMemoryCopyCommand) {
    // Append copy command with sync objects.
    ze_event_handle_t waitOnEvents[] = {event1, event2, event3};
    void *srcPtr = ctx->createMemAlloc(testAllocSize,
                                       VPU::VPUBufferObject::Type::CachedFw,
                                       VPU::VPUBufferObject::Location::Shared);
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

    ASSERT_EQ(expectedCommandTypes.size(), commandList->getCommands().size());
    for (size_t i = 0; i < expectedCommandTypes.size(); i++)
        ASSERT_EQ(expectedCommandTypes[i], commandList->getCommands()[i]->getCommandType());

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

TEST_F(CommandListMetricsApiTest,
       whenCalledAppendMetricQueryBeginEndWithDeactivatedGroupsFailureIsReturned) {
    // Deactivate all metric groups previously activated
    ASSERT_EQ(context->activateMetricGroups(device->toHandle(), 0, nullptr), ZE_RESULT_SUCCESS);

    ze_result_t result = commandList->appendMetricQueryBegin(hMetricQuery);
    EXPECT_EQ(ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE, result);

    result = commandList->appendMetricQueryEnd(hMetricQuery, nullptr, 0u, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE, result);

    EXPECT_EQ(0UL, commandList->getNumCommands());
}

TEST_F(CommandListMetricsApiTest,
       whenCalledAppendMetricQueryBeginEndWithInitializedQuerySuccessIsReturned) {
    ze_result_t result = commandList->appendMetricQueryBegin(hMetricQuery);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    result = commandList->appendMetricQueryEnd(hMetricQuery, nullptr, 1u, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_SIZE, result);

    result = commandList->appendMetricQueryEnd(hMetricQuery, nullptr, 0u, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    EXPECT_EQ(2UL, commandList->getNumCommands());
}

TEST_F(CommandListApiTest, whenCalledAppendGraphInitializeWithoutInitGraphFailureIsReturned) {
    auto result = commandList->appendGraphInitialize(nullptr, nullptr, 0u, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, result);
}

TEST_F(CommandListGraphApiTest,
       givenCallAppendGraphInitializeAndExecuteWithEventGetExpectedResults) {
    auto result = commandList->appendGraphInitialize(hGraph, nullptr, 0u, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = commandList->appendGraphInitialize(hGraph, nullptr, 1u, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_SIZE, result);
    result = commandList->appendGraphInitialize(hGraph, nullptr, 0u, &event0);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    result = commandList->appendGraphInitialize(hGraph, nullptr, 1u, &event0);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
    result = commandList->appendGraphInitialize(hGraph, event1, 1u, &event0);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    // The value depends on the buffer size returned by the elf loader
    const size_t argsAllocSize = 147 * 1024;

    uint64_t *inPtrAlloc =
        reinterpret_cast<uint64_t *>(ctx->createMemAlloc(argsAllocSize,
                                                         VPU::VPUBufferObject::Type::CachedFw,
                                                         VPU::VPUBufferObject::Location::Shared));

    uint64_t *outPtrAlloc =
        reinterpret_cast<uint64_t *>(ctx->createMemAlloc(argsAllocSize,
                                                         VPU::VPUBufferObject::Type::CachedFw,
                                                         VPU::VPUBufferObject::Location::Shared));
    ASSERT_NE(nullptr, inPtrAlloc);
    ASSERT_NE(nullptr, outPtrAlloc);

    EXPECT_EQ(ZE_RESULT_SUCCESS, pGraph->setArgumentValue(0, reinterpret_cast<void *>(inPtrAlloc)));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              pGraph->setArgumentValue(1, reinterpret_cast<void *>(outPtrAlloc)));

    result = commandList->appendGraphExecute(hGraph, nullptr, nullptr, 0u, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = commandList->appendGraphExecute(hGraph, nullptr, nullptr, 1u, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_SIZE, result);
    result = commandList->appendGraphExecute(hGraph, nullptr, nullptr, 0u, &event0);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    result = commandList->appendGraphExecute(hGraph, nullptr, nullptr, 1u, &event0);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
    result = commandList->appendGraphExecute(hGraph, nullptr, event1, 1u, &event0);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    EXPECT_TRUE(ctx->freeMemAlloc(inPtrAlloc));
    EXPECT_TRUE(ctx->freeMemAlloc(outPtrAlloc));
}

TEST_F(CommandListGraphApiTest,
       resetCommandListAfterGraphInitThenAppendingGraphExecAndExecuteReturnsSuccess) {
    ze_command_queue_handle_t hCommandQueue = createCommandQueue();
    ASSERT_NE(hCommandQueue, nullptr);

    auto commandQueue = L0::CommandQueue::fromHandle(hCommandQueue);
    ASSERT_TRUE(commandQueue);

    auto result = commandList->appendGraphInitialize(hGraph, nullptr, 0u, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
    commandList->close();

    auto cmdListHandle = commandList->toHandle();
    result = commandQueue->executeCommandLists(1, &cmdListHandle, nullptr);
    EXPECT_EQ(result, ZE_RESULT_SUCCESS);

    result = commandList->reset();
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    // The value depends on the buffer size returned by the elf loader
    const size_t argsAllocSize = 147 * 1024;

    uint64_t *inPtrAlloc =
        reinterpret_cast<uint64_t *>(ctx->createMemAlloc(argsAllocSize,
                                                         VPU::VPUBufferObject::Type::CachedFw,
                                                         VPU::VPUBufferObject::Location::Shared));

    uint64_t *outPtrAlloc =
        reinterpret_cast<uint64_t *>(ctx->createMemAlloc(argsAllocSize,
                                                         VPU::VPUBufferObject::Type::CachedFw,
                                                         VPU::VPUBufferObject::Location::Shared));
    ASSERT_NE(nullptr, inPtrAlloc);
    ASSERT_NE(nullptr, outPtrAlloc);

    EXPECT_EQ(ZE_RESULT_SUCCESS, pGraph->setArgumentValue(0, reinterpret_cast<void *>(inPtrAlloc)));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              pGraph->setArgumentValue(1, reinterpret_cast<void *>(outPtrAlloc)));

    result = commandList->appendGraphExecute(hGraph, nullptr, nullptr, 0u, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
    result = commandList->close();
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    EXPECT_EQ(1u, commandList->getCommands().size());
    EXPECT_EQ(VPU_CMD_INFERENCE_EXECUTE, commandList->getCommands()[0]->getCommandType());

    result = commandQueue->executeCommandLists(1, &cmdListHandle, nullptr);
    EXPECT_EQ(result, ZE_RESULT_SUCCESS);

    result = commandQueue->destroy();
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    EXPECT_TRUE(ctx->freeMemAlloc(inPtrAlloc));
    EXPECT_TRUE(ctx->freeMemAlloc(outPtrAlloc));
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
                  L0::EventPool::create(hContext, &evPoolDesc, 1, &(hDevice), &hEvPool));
        ASSERT_NE(nullptr, hEvPool);

        // Event.
        ze_event_desc_t evDesc = {};
        evDesc.stype = ZE_STRUCTURE_TYPE_EVENT_DESC;
        evDesc.index = 0;
        evDesc.signal = ZE_EVENT_SCOPE_FLAG_HOST;
        evDesc.wait = ZE_EVENT_SCOPE_FLAG_HOST;

        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  L0::EventPool::fromHandle(hEvPool)->createEvent(&evDesc, &hEvent));
        ASSERT_NE(nullptr, hEvent);
    }

    void TearDown() override {
        if (hEvent != nullptr) {
            ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(hEvent)->destroy());
        }
        if (hEvPool != nullptr) {
            ASSERT_EQ(ZE_RESULT_SUCCESS, L0::EventPool::fromHandle(hEvPool)->destroy());
        }

        CommandListFixture::TearDown();
    }

    static constexpr uint32_t evPoolCap = 5;
    ze_event_pool_handle_t hEvPool = nullptr;
    ze_event_handle_t hEvent = nullptr;
};

TEST_F(CommandListEventApiTest, givenCallAppendSignalEventSuccessIsReturned) {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              L0::CommandList::fromHandle(commandList->toHandle())
                  ->CommandList::appendSignalEvent(hEvent));
    EXPECT_EQ(ZE_RESULT_SUCCESS, L0::CommandList::fromHandle(commandList->toHandle())->close());

    EXPECT_EQ(1u, commandList->getCommands().size());
    EXPECT_EQ(VPU_CMD_FENCE_SIGNAL, commandList->getCommands()[0]->getCommandType());
}

TEST_F(CommandListEventApiTest, givenCallAppendSignalEventWithInvalidParamsReturnFiailure) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE,
              L0::CommandList::fromHandle(commandList->toHandle())
                  ->CommandList::appendSignalEvent(nullptr));
}

TEST_F(CommandListEventApiTest, givenCallAppendResetEventSuccessIsReturned) {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              L0::CommandList::fromHandle(commandList->toHandle())->appendEventReset(hEvent));
    EXPECT_EQ(ZE_RESULT_SUCCESS, L0::CommandList::fromHandle(commandList->toHandle())->close());

    EXPECT_EQ(1u, commandList->getCommands().size());
    EXPECT_EQ(VPU_CMD_FENCE_SIGNAL, commandList->getCommands()[0]->getCommandType());
}

TEST_F(CommandListEventApiTest, givenCallAppendResetEventWithInvalidParamsReturnFiailure) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE,
              L0::CommandList::fromHandle(commandList->toHandle())->appendEventReset(nullptr));
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
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  L0::EventPool::fromHandle(hEvPool)->createEvent(&evDesc, &phEvent[i]));
    }
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              L0::CommandList::fromHandle(commandList->toHandle())
                  ->CommandList::appendWaitOnEvents(evPoolCap, phEvent));
    EXPECT_EQ(ZE_RESULT_SUCCESS, L0::CommandList::fromHandle(commandList->toHandle())->close());

    EXPECT_EQ(evPoolCap, commandList->getCommands().size());
    for (const auto &cmd : commandList->getCommands())
        EXPECT_EQ(VPU_CMD_FENCE_WAIT, cmd->getCommandType());

    for (uint32_t i = 1; i < evPoolCap; ++i)
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(phEvent[i])->destroy());
}

TEST_F(CommandListEventApiTest, givenCallAppendWaitEventWithInvalidParamsReturnFiailure) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              L0::CommandList::fromHandle(commandList->toHandle())
                  ->CommandList::appendWaitOnEvents(1, nullptr));
}

} // namespace ult
} // namespace L0
