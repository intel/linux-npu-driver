/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gtest/gtest.h"
#include "vpu_driver/unit_tests/test_macros/test.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"
#include "vpu_driver/source/device/device_info.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include "level_zero_driver/core/source/cmdqueue/cmdqueue.hpp"
#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/core/source/event/event.hpp"
#include "level_zero_driver/unit_tests/mocks/mock_driver.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"

namespace L0 {
namespace ult {

struct CommandListTest : public Test<CommandQueueFixture> {
    void SetUp() override { CommandQueueFixture::SetUp(); }

    void TearDown() override {
        CommandQueueFixture::TearDown();
        mockDriver.reset();
    }

    Mock<Driver> mockDriver;
    ze_command_list_handle_t hCommandList0 = nullptr;
    ze_command_list_handle_t hCommandList1 = nullptr;
};

TEST_F(CommandListTest, whenCreatingCommandListFromNullContextAndDeviceThenFailureIsReturned) {
    ze_command_list_desc_t desc = {};
    desc.commandQueueGroupOrdinal = getComputeQueueOrdinal();

    ze_result_t result = zeCommandListCreate(nullptr, device, &desc, &hCommandList0);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, result);

    result = zeCommandListCreate(context, nullptr, &desc, &hCommandList0);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, result);
}

TEST_F(CommandListTest, commandListErrorsOnInvalidParamsShouldBeProperlyHandled) {
    ze_command_list_desc_t desc = {};
    desc.commandQueueGroupOrdinal = getComputeQueueOrdinal();

    ze_result_t result = zeCommandListCreate(context, device, &desc, &hCommandList0);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandListClose(nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, result);

    result = zeCommandListReset(nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, result);

    result = zeCommandListDestroy(nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, result);

    result = zeCommandListDestroy(hCommandList0);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);
}

TEST_F(CommandListTest, whenCreatingCommandListFromContextThenSuccessIsReturned) {
    ze_command_list_desc_t desc = {};
    desc.commandQueueGroupOrdinal = getComputeQueueOrdinal();

    ze_result_t result = context->createCommandList(device, &desc, &hCommandList0);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    L0::CommandList *commandList = L0::CommandList::fromHandle(hCommandList0);
    result = commandList->destroy();
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
}

TEST_F(CommandListTest, commandListIsIteratable) {
    ze_command_list_desc_t desc = {};
    desc.commandQueueGroupOrdinal = getComputeQueueOrdinal();

    context->createCommandList(device, &desc, &hCommandList0);
    auto cmdList = L0::CommandList::fromHandle(hCommandList0);
    ASSERT_NE(nullptr, cmdList);

    uint64_t globalTS;
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED,
              cmdList->appendWriteGlobalTimestamp(&globalTS, nullptr, 0, nullptr));

    auto ptrAlloc = (uint64_t *)ctx->createSharedMemAlloc(4 * 1024);
    EXPECT_NE(nullptr, ptrAlloc);

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              cmdList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              cmdList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              cmdList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              cmdList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              cmdList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));

    auto result = cmdList->close();
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    EXPECT_EQ(5u, cmdList->getNumCommands());

    for (const auto &cmd : cmdList->getNNCommands()) {
        EXPECT_EQ(VPU_CMD_TIMESTAMP, cmd->getCommandType());
        EXPECT_EQ(sizeof(vpu_cmd_timestamp_t), cmd->getCommitSize());
    }

    cmdList->destroy();

    EXPECT_TRUE(ctx->freeMemAlloc(ptrAlloc));
}

TEST_F(CommandListTest, whenCalledCommandListResetCommandListVectorIsClearedSuccessfully) {
    ze_command_list_desc_t desc = {};
    desc.commandQueueGroupOrdinal = getComputeQueueOrdinal();

    // Get Command list imp.
    context->createCommandList(device, &desc, &hCommandList0);
    auto cmdList = L0::CommandList::fromHandle(hCommandList0);
    ASSERT_NE(nullptr, cmdList);

    // Alloc a 4KB device mem.
    auto ptrAlloc = (uint64_t *)ctx->createSharedMemAlloc(4 * 1024);
    EXPECT_NE(nullptr, ptrAlloc);
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              cmdList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              cmdList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              cmdList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));
    EXPECT_EQ(3u, cmdList->getNumCommands());

    // Reset CommandList
    EXPECT_EQ(ZE_RESULT_SUCCESS, cmdList->reset());
    EXPECT_EQ(0u, cmdList->getNumCommands());

    // Reuse same CommandList
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              cmdList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              cmdList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));
    EXPECT_EQ(2u, cmdList->getNumCommands());

    // Reset CommandList
    EXPECT_EQ(ZE_RESULT_SUCCESS, cmdList->reset());
    EXPECT_EQ(0u, cmdList->getNumCommands());

    // Deallocate.
    cmdList->destroy();

    EXPECT_TRUE(ctx->freeMemAlloc(ptrAlloc));
}

struct CommandListCommitSizeTest : public CommandListTest {
    void SetUp() override {
        CommandListTest::SetUp();

        // Device.
        hDevice = device->toHandle();
        ASSERT_NE(nullptr, hDevice);

        // Command list.
        ze_command_list_desc_t desc = {};
        desc.commandQueueGroupOrdinal = getComputeQueueOrdinal();
        context->createCommandList(device, &desc, &hNNCmdlist);
        nnCmdlist = L0::CommandList::fromHandle(hNNCmdlist);
        ASSERT_NE(nullptr, nnCmdlist);

        desc.commandQueueGroupOrdinal = getCopyOnlyQueueOrdinal();
        context->createCommandList(device, &desc, &hCPCmdlist);
        cpCmdlist = L0::CommandList::fromHandle(hCPCmdlist);
        ASSERT_NE(nullptr, cpCmdlist);

        // Events
        ze_event_pool_desc_t evPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                           nullptr,
                                           0,
                                           ZE_EVENT_POOL_FLAG_HOST_VISIBLE};
        evPoolDesc.count = 3;
        ASSERT_EQ(ZE_RESULT_SUCCESS, context->createEventPool(&evPoolDesc, 1, &hDevice, &hEvPool));
        ASSERT_NE(nullptr, hEvPool);
        evPool = EventPool::fromHandle(hEvPool);
        ASSERT_NE(nullptr, evPool);

        ze_event_desc_t evDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                  nullptr,
                                  0,
                                  0,
                                  ZE_EVENT_SCOPE_FLAG_HOST};
        ASSERT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&evDesc, &hEvent0));
        ASSERT_NE(nullptr, hEvent0);
        evDesc.index = 1;
        ASSERT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&evDesc, &hEvent1));
        ASSERT_NE(nullptr, hEvent1);
        evDesc.index = 2;
        ASSERT_EQ(ZE_RESULT_SUCCESS, evPool->createEvent(&evDesc, &hEvent2));
        ASSERT_NE(nullptr, hEvent2);

        // Memories.
        ASSERT_NE(nullptr, (shareMem1 = ctx->createSharedMemAlloc(allocSize)));
        ASSERT_NE(nullptr, (shareMem2 = ctx->createSharedMemAlloc(allocSize)));
        ASSERT_NE(nullptr, (hostMem1 = ctx->createHostMemAlloc(allocSize)));
        ASSERT_NE(nullptr, (hostMem2 = ctx->createHostMemAlloc(allocSize)));
    }

    void TearDown() override {
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(hEvent0)->destroy());
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(hEvent1)->destroy());
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(hEvent2)->destroy());
        evPool->destroy();
        nnCmdlist->destroy();
        cpCmdlist->destroy();

        ASSERT_TRUE(ctx->freeMemAlloc(shareMem1));
        ASSERT_TRUE(ctx->freeMemAlloc(shareMem2));
        ASSERT_TRUE(ctx->freeMemAlloc(hostMem1));
        ASSERT_TRUE(ctx->freeMemAlloc(hostMem2));

        CommandListTest::TearDown();
    }

    size_t allocSize = 1024;
    ze_device_handle_t hDevice = nullptr;
    ze_command_list_handle_t hNNCmdlist = nullptr;
    ze_command_list_handle_t hCPCmdlist = nullptr;
    ze_event_pool_handle_t hEvPool = nullptr;
    CommandList *nnCmdlist = nullptr;
    CommandList *cpCmdlist = nullptr;
    EventPool *evPool = nullptr;
    ze_event_handle_t hEvent0 = nullptr;
    ze_event_handle_t hEvent1 = nullptr;
    ze_event_handle_t hEvent2 = nullptr;

    void *shareMem1 = nullptr;
    void *shareMem2 = nullptr;
    void *hostMem1 = nullptr;
    void *hostMem2 = nullptr;
};

TEST_F(CommandListCommitSizeTest, l2lCopyAndbarrierCommandWithEvents) {
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem1, shareMem2, allocSize, nullptr, 0, nullptr));

    ze_event_handle_t waitEvs[] = {hEvent0, hEvent1};
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(hEvent2, 2, waitEvs));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    EXPECT_EQ(5u, nnCmdlist->getNumCommands());
    EXPECT_EQ(5u, nnCmdlist->getNNCommands().size());
    EXPECT_EQ(0u, nnCmdlist->getCopyCommands().size());
}

TEST_F(CommandListCommitSizeTest, timeStampAndS2LCopyCommands) {
    auto timestampPtr = static_cast<uint64_t *>(shareMem2);

    // Append 2 TS commands and 2 copy commands to the buffer.
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(timestampPtr, nullptr, 0, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(timestampPtr, nullptr, 0, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem1, hostMem1, allocSize, nullptr, 0, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem1, hostMem1, allocSize, nullptr, 0, nullptr));

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    EXPECT_EQ(4u, nnCmdlist->getNumCommands());
    EXPECT_EQ(0u, nnCmdlist->getNNCommands().size());
    EXPECT_EQ(4u, nnCmdlist->getCopyCommands().size());
}

TEST_F(CommandListCommitSizeTest, timeStampAndS2lCopyCommandsWithEvents) {
    auto timestampPtr = static_cast<uint64_t *>(hostMem2);

    // TS | S2L | signal ev | L2S | wait ev | L2L | reset ev
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(timestampPtr, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem1, hostMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendSignalEvent(hEvent0));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(hostMem1, shareMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendWaitOnEvents(1, &hEvent0));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem1, shareMem2, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendEventReset(hEvent0));

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    EXPECT_EQ(10u, nnCmdlist->getNumCommands());
    // NN queue.
    // TS(x) | S2L(x) | signal ev(x) | L2S(x) | wait ev(x) | internal event | L2L | reset ev
    EXPECT_EQ(5u, nnCmdlist->getNNCommands().size());

    // CP queue.
    // TS | S2L | signal ev | L2S | wait ev | internal event(x) | L2L(x) | reset ev(x)
    EXPECT_EQ(5u, nnCmdlist->getCopyCommands().size());
}

TEST_F(CommandListCommitSizeTest, l2lCopyAndL2SCopyCommands) {
    // S2L copy | L2L copy | L2S copy
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem1, hostMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem2, shareMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(hostMem2, shareMem2, allocSize, nullptr, 0, nullptr));
    EXPECT_EQ(9u, nnCmdlist->getNumCommands());

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    // L2L copy in NN
    EXPECT_EQ(4u, nnCmdlist->getNNCommands().size());

    // S2L copy | L2S copy in Copy
    EXPECT_EQ(5u, nnCmdlist->getCopyCommands().size());
}

TEST_F(CommandListCommitSizeTest, l2lCopyAndL2SCopyCommandsWithBarriers) {
    // S2L | barrier | L2L | barrier | L2S | barrier
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem1, hostMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem2, shareMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(hostMem2, shareMem2, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(nullptr, 0, nullptr));
    EXPECT_EQ(12u, nnCmdlist->getNumCommands());

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    // NN queue.
    // S2L(x) | barrier(x) | L2L | barrier | L2S(x) | barrier(x)
    EXPECT_EQ(5u, nnCmdlist->getNNCommands().size());

    // COPY queue.
    // S2L | barrier | L2L(x) | barrier(x) | L2S | barrier
    EXPECT_EQ(7u, nnCmdlist->getCopyCommands().size());
}

TEST_F(CommandListCommitSizeTest, l2lCopyAndL2SCopyCommandsWithEvents) {
    // S2L | signal ev | L2L | wait ev | L2S | signal ev
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem1, hostMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendSignalEvent(hEvent0));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem2, shareMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendWaitOnEvents(1, &hEvent0));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(hostMem2, shareMem2, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendSignalEvent(hEvent1));
    EXPECT_EQ(12u, nnCmdlist->getNumCommands());

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    // NN queue.
    // S2L(x) | signal ev(x) | L2L | wait ev(x) | L2S(x) | signal ev(x)
    EXPECT_EQ(4u, nnCmdlist->getNNCommands().size());

    // COPY queue.
    // S2L | signal ev | L2L(x) | wait ev | L2S | signal ev
    EXPECT_EQ(8u, nnCmdlist->getCopyCommands().size());
}

TEST_F(CommandListCommitSizeTest, l2lCopyAndL2SCopyCommandsWithBarriersAndEvents) {
    // S2L | barrier | signal ev | wait ev | L2L | barrier | signal ev | wait ev | L2S | barrier
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem1, hostMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendSignalEvent(hEvent0));

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendWaitOnEvents(1, &hEvent0));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem2, shareMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendSignalEvent(hEvent1));

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendWaitOnEvents(1, &hEvent1));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(hostMem2, shareMem2, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(nullptr, 0, nullptr));

    EXPECT_EQ(16u, nnCmdlist->getNumCommands());

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    // NN queue.
    // S2L(x) | barrier(x) | signal ev(x) | wait ev | L2L | barrier | signal ev | wait ev(x) |
    // L2S(x) | barrier(x)
    EXPECT_EQ(7u, nnCmdlist->getNNCommands().size());

    // COPY queue.
    // S2L | barrier | signal ev | wait ev(x) | L2L(x) | barrier(x) | signal ev(x) | wait ev | L2S |
    // barrier
    EXPECT_EQ(9u, nnCmdlist->getCopyCommands().size());
}

TEST_F(CommandListCommitSizeTest, testingVariousFollowingConditionsForDifferentCommandListTypes) {
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendEventReset(hEvent0));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendSignalEvent(hEvent0));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem2, shareMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    // NN list.
    // reset ev | signal ev | barrier | [L2L]
    EXPECT_EQ(4u, nnCmdlist->getNNCommands().size());
    EXPECT_EQ(0u, nnCmdlist->getCopyCommands().size());

    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              cpCmdlist->appendMemoryCopy(shareMem2, hostMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->appendWaitOnEvents(1, &hEvent1));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->close());

    // CP list.
    // barrier | [S2L] | wait ev
    EXPECT_EQ(0u, cpCmdlist->getNNCommands().size());
    EXPECT_EQ(3u, cpCmdlist->getCopyCommands().size());

    // Reset
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->reset());
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->reset());
    ASSERT_EQ(0u, nnCmdlist->getNNCommands().size());
    ASSERT_EQ(0u, nnCmdlist->getCopyCommands().size());
    ASSERT_EQ(0u, cpCmdlist->getNNCommands().size());
    ASSERT_EQ(0u, cpCmdlist->getCopyCommands().size());

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(static_cast<uint64_t *>(hostMem2),
                                                    nullptr,
                                                    0,
                                                    nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem2, hostMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem2, shareMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(static_cast<uint64_t *>(hostMem2),
                                                    nullptr,
                                                    0,
                                                    nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    // NN buffer.
    // wait ev | reset ev | [L2L] | barrier | ts
    EXPECT_EQ(5u, nnCmdlist->getNNCommands().size());

    // COPY buffer.
    // barrier | ts | [S2L] | signal ev
    EXPECT_EQ(4u, nnCmdlist->getCopyCommands().size());

    // Reset
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->reset());
    ASSERT_EQ(0u, nnCmdlist->getNNCommands().size());
    ASSERT_EQ(0u, nnCmdlist->getCopyCommands().size());

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(static_cast<uint64_t *>(hostMem2),
                                                    nullptr,
                                                    0,
                                                    nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(static_cast<uint64_t *>(hostMem2),
                                                    nullptr,
                                                    0,
                                                    nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendSignalEvent(hEvent0));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendWaitOnEvents(1, &hEvent2));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendEventReset(hEvent0));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    // NN list.
    // barrier | ts | barrier | ts | signal ev | wait ev | reset ev
    EXPECT_EQ(7u, nnCmdlist->getNNCommands().size());
    EXPECT_EQ(0u, nnCmdlist->getCopyCommands().size());

    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              cpCmdlist->appendWriteGlobalTimestamp(static_cast<uint64_t *>(hostMem2),
                                                    nullptr,
                                                    0,
                                                    nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              cpCmdlist->appendWriteGlobalTimestamp(static_cast<uint64_t *>(hostMem2),
                                                    nullptr,
                                                    0,
                                                    nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->appendSignalEvent(hEvent0));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->appendWaitOnEvents(1, &hEvent2));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->appendEventReset(hEvent0));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->close());

    // CP list.
    // barrier | ts | barrier | ts | signal ev | wait ev | reset ev
    EXPECT_EQ(0u, cpCmdlist->getNNCommands().size());
    EXPECT_EQ(7u, cpCmdlist->getCopyCommands().size());
}

} // namespace ult
} // namespace L0
