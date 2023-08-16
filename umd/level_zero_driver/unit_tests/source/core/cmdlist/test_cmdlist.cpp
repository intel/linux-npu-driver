/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gtest/gtest.h"
#include "vpu_driver/unit_tests/test_macros/test.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
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
    EXPECT_EQ(ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY,
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

    /* Timestamp is split by UMD to two commands aligned TS and copy */
    EXPECT_EQ(10u, cmdList->getNumCommands());

    /* In the buffer is expected TS command and COPY_LOCAL_TO_LOCAL */
    for (const auto &cmd : cmdList->getCommands()) {
        EXPECT_TRUE(cmd->getCommandType() == VPU_CMD_TIMESTAMP ||
                    cmd->getCommandType() == VPU_CMD_COPY_LOCAL_TO_LOCAL);
        if (cmd->getCommandType() == VPU_CMD_TIMESTAMP)
            EXPECT_EQ(sizeof(vpu_cmd_timestamp_t), cmd->getCommitSize());
        else
            EXPECT_EQ(sizeof(vpu_cmd_copy_buffer_t), cmd->getCommitSize());
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
    /* Timestamp is split by UMD to two commands aligned TS and copy */
    EXPECT_EQ(6u, cmdList->getNumCommands());

    // Reset CommandList
    EXPECT_EQ(ZE_RESULT_SUCCESS, cmdList->reset());
    EXPECT_EQ(0u, cmdList->getNumCommands());

    // Reuse same CommandList
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              cmdList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              cmdList->appendWriteGlobalTimestamp(ptrAlloc, nullptr, 0, nullptr));
    /* Timestamp is split by UMD to two commands aligned TS and copy */
    EXPECT_EQ(4u, cmdList->getNumCommands());

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
    EXPECT_EQ(1u, nnCmdlist->getJob()->getCommandBuffers().size());
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

    /* Timestamp is split by UMD to two commands aligned TS and copy */
    EXPECT_EQ(6u, nnCmdlist->getNumCommands());
    EXPECT_EQ(1u, nnCmdlist->getJob()->getCommandBuffers().size());
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

    /* Timestamp is split by UMD to two commands aligned TS and copy */
    EXPECT_EQ(8u, nnCmdlist->getNumCommands());
    EXPECT_EQ(2u, nnCmdlist->getJob()->getCommandBuffers().size());
}

TEST_F(CommandListCommitSizeTest, l2lCopyAndL2SCopyCommands) {
    // S2L copy | L2L copy | L2S copy
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem1, hostMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem2, shareMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(hostMem2, shareMem2, allocSize, nullptr, 0, nullptr));

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    EXPECT_EQ(3u, nnCmdlist->getNumCommands());
    /* Expected only Compute is used */
    EXPECT_EQ(1u, nnCmdlist->getJob()->getCommandBuffers().size());
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
    EXPECT_EQ(6u, nnCmdlist->getNumCommands());

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());
    /* Only Compute should be used */
    EXPECT_EQ(1u, nnCmdlist->getJob()->getCommandBuffers().size());
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
    EXPECT_EQ(6u, nnCmdlist->getNumCommands());

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());
    /* Only Compute should be used */
    EXPECT_EQ(2u, nnCmdlist->getJob()->getCommandBuffers().size());
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
    EXPECT_EQ(4u, nnCmdlist->getCommands().size());
    EXPECT_EQ(2u, nnCmdlist->getJob()->getCommandBuffers().size());

    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              cpCmdlist->appendMemoryCopy(shareMem2, hostMem1, allocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->appendWaitOnEvents(1, &hEvent1));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->close());

    // CP list.
    // barrier | [S2L] | wait ev
    EXPECT_EQ(3u, cpCmdlist->getCommands().size());
    EXPECT_EQ(1u, cpCmdlist->getJob()->getCommandBuffers().size());

    // Reset
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->reset());
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->reset());
    ASSERT_EQ(0u, nnCmdlist->getCommands().size());
    ASSERT_EQ(0u, cpCmdlist->getCommands().size());

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendBarrier(nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(static_cast<uint64_t *>(hostMem2),
                                                    nullptr,
                                                    0,
                                                    nullptr)); // To Copy Engine, cmdlist#1
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem2,
                                          hostMem1,
                                          allocSize,
                                          nullptr,
                                          0,
                                          nullptr)); // To Copy Engine, cmdlist#1
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy(shareMem2,
                                          shareMem1,
                                          allocSize,
                                          nullptr,
                                          0,
                                          nullptr)); // To Compute Engine, cmdlist#2
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendBarrier(nullptr, 0, nullptr)); // To Compute Engine, cmdlist#2
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(static_cast<uint64_t *>(hostMem2),
                                                    nullptr,
                                                    0,
                                                    nullptr)); // To Copy Engine, cmdlist#3
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    /* Timestamp is split by UMD to two commands aligned TS and copy */
    EXPECT_EQ(8u, nnCmdlist->getCommands().size());
    EXPECT_EQ(1u, nnCmdlist->getJob()->getCommandBuffers().size());

    // Reset
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->reset());
    ASSERT_EQ(0u, nnCmdlist->getCommands().size());
    EXPECT_EQ(0u, nnCmdlist->getJob()->getCommandBuffers().size());

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

    /* Timestamp is split by UMD to two commands aligned TS and copy */
    EXPECT_EQ(9u, nnCmdlist->getCommands().size());
    EXPECT_EQ(2u, nnCmdlist->getJob()->getCommandBuffers().size());

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

    EXPECT_EQ(9u, cpCmdlist->getCommands().size());
    EXPECT_EQ(2u, cpCmdlist->getJob()->getCommandBuffers().size());
}

} // namespace ult
} // namespace L0
