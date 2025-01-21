/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "gtest/gtest.h"
#include "level_zero_driver/source/cmdlist.hpp"
#include "level_zero_driver/source/cmdqueue.hpp"
#include "level_zero_driver/source/context.hpp"
#include "level_zero_driver/source/device.hpp"
#include "level_zero_driver/source/event.hpp"
#include "level_zero_driver/source/eventpool.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <level_zero/ze_api.h>
#include <string>

namespace L0 {
namespace ult {

using CommandQueueCreate = Test<CommandQueueFixture>;

TEST_F(CommandQueueCreate, givenCallGetCommandQueueGroupPropertiesHandleParameters) {
    ze_result_t re = device->getCommandQueueGroupProperties(nullptr, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, re);

    uint32_t pCnt = 0;
    re = device->getCommandQueueGroupProperties(&pCnt, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, re);
    EXPECT_EQ(1u, pCnt);

    ze_command_queue_group_properties_t queGrpProps = {};
    re = device->getCommandQueueGroupProperties(&pCnt, &queGrpProps);
    EXPECT_EQ(ZE_RESULT_SUCCESS, re);

    EXPECT_FALSE(queGrpProps.flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COOPERATIVE_KERNELS);
    EXPECT_FALSE(queGrpProps.flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_METRICS);

    EXPECT_TRUE(queGrpProps.flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COPY);
    EXPECT_TRUE(queGrpProps.flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE);

    EXPECT_EQ(1u, queGrpProps.numQueues);
}

TEST_F(CommandQueueCreate, createErrors) {
    ze_command_queue_handle_t hCommandQueue = nullptr;
    ze_command_queue_desc_t cmdQueueDesc = {};

    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE,
              L0::CommandQueue::create(nullptr, nullptr, &cmdQueueDesc, &hCommandQueue));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE,
              L0::CommandQueue::create(context, nullptr, &cmdQueueDesc, &hCommandQueue));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE,
              L0::CommandQueue::create(nullptr, device, &cmdQueueDesc, &hCommandQueue));

    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              L0::CommandQueue::create(context, device, nullptr, &hCommandQueue));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              L0::CommandQueue::create(context, device, &cmdQueueDesc, nullptr));
}

TEST_F(CommandQueueCreate, whenCreatingAndDestroyingCommandQueueUsingContextSuccessIsReturned) {
    ze_command_queue_handle_t hCommandQueue = createCommandQueue();
    ASSERT_NE(hCommandQueue, nullptr);

    L0::CommandQueue *commandQueue = L0::CommandQueue::fromHandle(hCommandQueue);
    EXPECT_EQ(ZE_RESULT_SUCCESS, commandQueue->destroy());
}

TEST_F(CommandQueueCreate, expectCommandQueueIsDestroyOnContextDestroy) {
    ze_command_queue_handle_t hCommandQueue = createCommandQueue();
    ASSERT_NE(hCommandQueue, nullptr);
}

struct CommandQueueExecTest : Test<CommandQueueFixture> {
    void SetUp() override {
        CommandQueueFixture::SetUp();

        // NN queue.
        ze_command_queue_handle_t hNNCommandQueue = createCommandQueue();
        ASSERT_NE(hNNCommandQueue, nullptr);
        nnQue = L0::CommandQueue::fromHandle(hNNCommandQueue);

        // Event.
        ze_event_pool_desc_t evPoolDesc = {.stype = ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                           .pNext = nullptr,
                                           .flags = ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                           .count = 3};
        ASSERT_NE(nullptr, device);
        auto hDevice = device->toHandle();
        L0::EventPool::create(context, &evPoolDesc, 1, &(hDevice), &eventPool);
        ASSERT_NE(nullptr, eventPool);

        ze_event_desc_t evDesc = {
            .stype = ZE_STRUCTURE_TYPE_EVENT_DESC,
            .pNext = nullptr,
            .index = 0,
            .signal = ZE_EVENT_SCOPE_FLAG_HOST,
            .wait = ZE_EVENT_SCOPE_FLAG_HOST,
        };

        evDesc.index = 0;
        L0::EventPool::fromHandle(eventPool)->createEvent(&evDesc, &event0);
        ASSERT_NE(nullptr, event0);

        evDesc.index = 1;
        L0::EventPool::fromHandle(eventPool)->createEvent(&evDesc, &event1);
        ASSERT_NE(nullptr, event1);

        evDesc.index = 2;
        L0::EventPool::fromHandle(eventPool)->createEvent(&evDesc, &event2);
        ASSERT_NE(nullptr, event2);
    }

    void TearDown() override {
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(event0)->destroy());
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(event1)->destroy());
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::Event::fromHandle(event2)->destroy());
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::EventPool::fromHandle(eventPool)->destroy());

        ASSERT_EQ(ZE_RESULT_SUCCESS, nnQue->destroy());

        CommandQueueFixture::TearDown();
    }

    L0::CommandQueue *nnQue = nullptr;

    // Event pool.
    ze_event_pool_handle_t eventPool = nullptr;

    // Event handles.
    ze_event_handle_t event0 = nullptr;
    ze_event_handle_t event1 = nullptr;
    ze_event_handle_t event2 = nullptr;
};

TEST_F(CommandQueueExecTest, execErrors) {
    auto hCommandList = createCommandList();
    ASSERT_NE(nullptr, hCommandList);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              L0::CommandQueue::fromHandle(nnQue)->executeCommandLists(1, nullptr, nullptr));

    L0::CommandList::fromHandle(hCommandList)->destroy();
}

TEST_F(CommandQueueExecTest,
       givenCmdQueueWithBlitCopyWhenExecutingCopyBlitCommandListThenSuccessReturned) {
    auto hCommandList = createCommandList();
    ASSERT_NE(nullptr, hCommandList);

    L0::CommandList *nnCmdList = L0::CommandList::fromHandle(hCommandList);

    // Append a copy command.
    void *srcPtr = ctx->createMemAlloc(4 * 1024,
                                       VPU::VPUBufferObject::Type::CachedFw,
                                       VPU::VPUBufferObject::Location::Shared);
    void *destPtr = ctx->createMemAlloc(4 * 1024,
                                        VPU::VPUBufferObject::Type::CachedFw,
                                        VPU::VPUBufferObject::Location::Shared);
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              nnCmdList->appendMemoryCopy(destPtr, srcPtr, 4 * 1024, nullptr, 0, nullptr));

    // Close the command list.
    nnCmdList->close();

    // Execute the queue.
    auto status = nnQue->executeCommandLists(1, &hCommandList, nullptr);
    EXPECT_EQ(status, ZE_RESULT_SUCCESS);

    ctx->freeMemAlloc(srcPtr);
    ctx->freeMemAlloc(destPtr);

    nnCmdList->destroy();
}

TEST_F(CommandQueueExecTest, syncWithoutJobSubmissionReturnsSuccess) {
    EXPECT_EQ(ZE_RESULT_SUCCESS, nnQue->synchronize(0));
}

TEST_F(CommandQueueExecTest, commandListsShouldBeClosedBeforeExecute) {
    auto hCommandList = createCommandList();
    ASSERT_NE(nullptr, hCommandList);

    L0::CommandList *cmdList = L0::CommandList::fromHandle(hCommandList);

    // Append a TS command.
    uint64_t *ts =
        static_cast<uint64_t *>(ctx->createMemAlloc(64,
                                                    VPU::VPUBufferObject::Type::CachedFw,
                                                    VPU::VPUBufferObject::Location::Shared));
    EXPECT_EQ(ZE_RESULT_SUCCESS, cmdList->appendWriteGlobalTimestamp(ts, nullptr, 0, nullptr));

    // Executing the queue without closing the command list which return error.
    auto cmdListHandle = cmdList->toHandle();
    auto status = nnQue->executeCommandLists(1, &cmdListHandle, nullptr);
    EXPECT_EQ(status, ZE_RESULT_ERROR_UNINITIALIZED);

    // Retry with closing it.
    cmdList->close();
    status = nnQue->executeCommandLists(1, &cmdListHandle, nullptr);
    EXPECT_EQ(status, ZE_RESULT_SUCCESS);

    ctx->freeMemAlloc(ts);
    cmdList->destroy();
}

TEST_F(CommandQueueExecTest, jobAllocationFailureShouldBeHandled) {
    auto hCommandList = createCommandList();
    ASSERT_NE(nullptr, hCommandList);

    L0::CommandList *cmdList = L0::CommandList::fromHandle(hCommandList);

    // Append a TS command and ready to execute.
    uint64_t *ts =
        static_cast<uint64_t *>(ctx->createMemAlloc(64,
                                                    VPU::VPUBufferObject::Type::CachedFw,
                                                    VPU::VPUBufferObject::Location::Shared));
    EXPECT_EQ(ZE_RESULT_SUCCESS, cmdList->appendWriteGlobalTimestamp(ts, nullptr, 0, nullptr));

    // Expect an error to be reported upon command buffer failures.
    // (Command buffer allocation failure is not documented)
    osInfc.mockFailNextAlloc();
    EXPECT_EQ(ZE_RESULT_ERROR_UNKNOWN, cmdList->close());

    // Deallocate memory.
    EXPECT_TRUE(ctx->freeMemAlloc(ts));
    EXPECT_EQ(ZE_RESULT_SUCCESS, cmdList->destroy());
}

TEST_F(CommandQueueExecTest,
       commandQueueExecutionOnEmptyCommandListShouldCallMapAndUnmapCallForJob) {
    // Create a command list.
    auto hCommandList = createCommandList();
    ASSERT_NE(nullptr, hCommandList);

    L0::CommandList *cmdList = L0::CommandList::fromHandle(hCommandList);

    // Resetting previous call counts
    osInfc.callCntAlloc = 0;
    osInfc.callCntFree = 0;

    // At least one command is required for successful submission.
    uint64_t *ts =
        static_cast<uint64_t *>(ctx->createMemAlloc(64,
                                                    VPU::VPUBufferObject::Type::CachedFw,
                                                    VPU::VPUBufferObject::Location::Shared));
    EXPECT_EQ(ZE_RESULT_SUCCESS, cmdList->appendWriteGlobalTimestamp(ts, nullptr, 0, nullptr));
    // Memalloc causes mapping.
    // Timestamp is split by UMD to two commands expected 2 mappings
    EXPECT_EQ(2u, osInfc.callCntAlloc);
    EXPECT_EQ(0u, osInfc.callCntFree);

    // Executing the queue
    auto cmdListHandle = cmdList->toHandle();
    cmdList->close();
    EXPECT_EQ(nnQue->executeCommandLists(1, &cmdListHandle, nullptr), ZE_RESULT_SUCCESS);

    // Checking mapping call for commandbuffer
    EXPECT_EQ(3u, osInfc.callCntAlloc);

    // ::free() will be called.
    ctx->freeMemAlloc(ts);
    cmdList->destroy();
}

TEST_F(CommandQueueExecTest, eventAttachedToSingleQueuesRespectively) {
    size_t testAllocSize = 4 * 1024;

    auto srcShareMem = ctx->createMemAlloc(testAllocSize,
                                           VPU::VPUBufferObject::Type::CachedFw,
                                           VPU::VPUBufferObject::Location::Shared);
    auto destShareMem = ctx->createMemAlloc(testAllocSize,
                                            VPU::VPUBufferObject::Type::CachedFw,
                                            VPU::VPUBufferObject::Location::Shared);

    // NN queue event attaching.
    // Append wait on event 0, L2L copy and signal event 1 to the same NN queue.
    auto hNNCmdlist = createCommandList();
    ASSERT_NE(nullptr, hNNCmdlist);

    L0::CommandList *nnCmdlist = L0::CommandList::fromHandle(hNNCmdlist);

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendWaitOnEvents(1, &event0));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendMemoryCopy((void *)destShareMem,
                                          (void *)srcShareMem,
                                          testAllocSize,
                                          nullptr,
                                          0,
                                          nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->appendSignalEvent(event1));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());
    ASSERT_EQ(nnQue->executeCommandLists(1, &hNNCmdlist, nullptr), ZE_RESULT_SUCCESS);

    // Make sure a single command buffer was submitted.
    EXPECT_EQ(1U, osInfc.callCntSubmit);

    // Deallocate instances
    ctx->freeMemAlloc(srcShareMem);
    ctx->freeMemAlloc(destShareMem);

    nnCmdlist->destroy();
}

struct CommandQueueJobTest : public Test<CommandQueueFixture> {
    void SetUp() override {
        CommandQueueFixture::SetUp();

        // Create a NN command queue.
        ze_command_queue_handle_t hNNCommandQueue = createCommandQueue();
        ASSERT_NE(hNNCommandQueue, nullptr);
        nnCmdque = CommandQueue::fromHandle(hNNCommandQueue);

        // Command a NN lists.
        hNNCmdlist = createCommandList();
        ASSERT_NE(nullptr, hNNCmdlist);
        nnCmdlist = CommandList::fromHandle(hNNCmdlist);
        ASSERT_NE(nullptr, nnCmdlist);
    }

    void TearDown() override {
        if (nnCmdque != nullptr) {
            nnCmdque->destroy();
        }

        if (nnCmdlist != nullptr) {
            nnCmdlist->destroy();
        }

        CommandQueueFixture::TearDown();
    }

    const size_t memAllocSize = 4 * 1024; // Default memory allocation size.
    uint64_t syncTimeout = 2e9;           // 2seconds default time out.

    L0::CommandQueue *nnCmdque = nullptr;

    ze_command_list_handle_t hNNCmdlist = nullptr;
    L0::CommandList *nnCmdlist = nullptr;
};

TEST_F(CommandQueueJobTest, emptyCommandListDoesNotKeepJob) {
    // Empty NN command list.
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdque->executeCommandLists(1, &hNNCmdlist, nullptr));
    EXPECT_EQ(0u, osInfc.callCntSubmit);

    // Call buffer sync against failed command queue.
    EXPECT_EQ(ZE_RESULT_SUCCESS, nnCmdque->synchronize(syncTimeout));
}

TEST_F(CommandQueueJobTest, submittedJobsShouldBeKept) {
    // Execute non empty NN command list.
    uint64_t *tsDest =
        static_cast<uint64_t *>(ctx->createMemAlloc(memAllocSize,
                                                    VPU::VPUBufferObject::Type::CachedFw,
                                                    VPU::VPUBufferObject::Location::Shared));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(tsDest, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdque->executeCommandLists(1, &hNNCmdlist, nullptr));

    // 1 BB should be added in the cmd queue.
    EXPECT_EQ(1u, osInfc.callCntSubmit);

    // Deallocate memory.
    ASSERT_TRUE(ctx->freeMemAlloc(tsDest));
}

TEST_F(CommandQueueJobTest, multipleJobsMaintainedInSingleSubmitId) {
    // NN(TS) and CP(L2S) combined command will create two command buffers in execution.
    uint64_t *tsDest =
        static_cast<uint64_t *>(ctx->createMemAlloc(memAllocSize,
                                                    VPU::VPUBufferObject::Type::CachedFw,
                                                    VPU::VPUBufferObject::Location::Shared));
    void *srcShareMem = ctx->createMemAlloc(memAllocSize,
                                            VPU::VPUBufferObject::Type::CachedFw,
                                            VPU::VPUBufferObject::Location::Shared);
    void *dstHostMem = ctx->createMemAlloc(memAllocSize,
                                           VPU::VPUBufferObject::Type::CachedShave,
                                           VPU::VPUBufferObject::Location::Host);
    ASSERT_NE(nullptr, tsDest);
    ASSERT_NE(nullptr, srcShareMem);
    ASSERT_NE(nullptr, dstHostMem);
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(tsDest, nullptr, 0, nullptr));
    ASSERT_EQ(
        ZE_RESULT_SUCCESS,
        nnCmdlist->appendMemoryCopy(dstHostMem, srcShareMem, memAllocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    // Assume 2 more command lists given.
    auto hNNCmdlist1 = createCommandList();
    ASSERT_NE(nullptr, hNNCmdlist1);
    auto nnCmdlist1 = L0::CommandList::fromHandle(hNNCmdlist1);

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist1->appendWriteGlobalTimestamp(tsDest, nullptr, 0, nullptr));
    ASSERT_EQ(
        ZE_RESULT_SUCCESS,
        nnCmdlist1->appendMemoryCopy(dstHostMem, srcShareMem, memAllocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist1->close());

    auto hNNCmdlist2 = createCommandList();
    ASSERT_NE(nullptr, hNNCmdlist2);
    auto nnCmdlist2 = L0::CommandList::fromHandle(hNNCmdlist2);

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist2->appendWriteGlobalTimestamp(tsDest, nullptr, 0, nullptr));
    ASSERT_EQ(
        ZE_RESULT_SUCCESS,
        nnCmdlist2->appendMemoryCopy(dstHostMem, srcShareMem, memAllocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist2->close());

    // Run 3 command lists consists of 1 NN/1 CP commands.
    ze_command_list_handle_t nnCmdlists[] = {hNNCmdlist, hNNCmdlist1, hNNCmdlist2};
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdque->executeCommandLists(3, nnCmdlists, nullptr));

    // Expect all submitted command lists are observed within the same submit ID.
    EXPECT_EQ(3u, osInfc.callCntSubmit);

    // Deallocate the memory.
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist1->destroy());
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist2->destroy());
    ASSERT_TRUE(ctx->freeMemAlloc(tsDest));
    ASSERT_TRUE(ctx->freeMemAlloc(srcShareMem));
    ASSERT_TRUE(ctx->freeMemAlloc(dstHostMem));
}

} // namespace ult
} // namespace L0
