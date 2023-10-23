/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gtest/gtest.h"

#include "level_zero_driver/api/core/ze_cmdlist.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/core/source/fence/fence.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "level_zero_driver/core/source/cmdqueue/cmdqueue.hpp"
#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"

#include <level_zero/ze_api.h>
#include <thread>
#include <chrono>

namespace L0 {
namespace ult {

using CommandQueueCreate = Test<CommandQueueFixture>;

TEST_F(CommandQueueCreate, givenCallGetCommandQueueGroupPropertiesHandleParameters) {
    ze_result_t re = device->getCommandQueueGroupProperties(nullptr, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, re);

    // DeviceFixture assume 1NN, 1Copy engines.
    uint32_t pCnt = 0;
    re = device->getCommandQueueGroupProperties(&pCnt, nullptr);
    EXPECT_EQ(ZE_RESULT_SUCCESS, re);
    EXPECT_EQ(2u, pCnt);

    auto queGrpProps = new ze_command_queue_group_properties_t[pCnt];
    re = device->getCommandQueueGroupProperties(&pCnt, queGrpProps);
    EXPECT_EQ(ZE_RESULT_SUCCESS, re);
    EXPECT_EQ(2u, pCnt);

    // Assume no fixed order of engine groups.
    // No engines support cooperative kernel & metrics.
    EXPECT_FALSE(queGrpProps[0].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COOPERATIVE_KERNELS);
    EXPECT_FALSE(queGrpProps[1].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COOPERATIVE_KERNELS);
    EXPECT_FALSE(queGrpProps[0].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_METRICS);
    EXPECT_FALSE(queGrpProps[1].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_METRICS);
    // NN/Copy both support copy operations.
    EXPECT_TRUE(queGrpProps[0].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COPY);
    EXPECT_TRUE(queGrpProps[1].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COPY);
    // Each groups has 1 queues.
    EXPECT_EQ(1u, queGrpProps[0].numQueues);
    EXPECT_EQ(1u, queGrpProps[1].numQueues);

    // Either of it is copy only engine.
    if (queGrpProps[0].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE) {
        EXPECT_FALSE(queGrpProps[1].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE);
    } else {
        EXPECT_TRUE(queGrpProps[1].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE);
    }

    delete[] queGrpProps;
}

TEST_F(CommandQueueCreate, whenCreatingAndDestroyingCommandQueueUsingContextSuccessIsReturned) {
    ze_command_queue_handle_t hCommandQueue = createCommandQueue(getComputeQueueOrdinal());
    ASSERT_NE(hCommandQueue, nullptr);

    L0::CommandQueue *commandQueue = L0::CommandQueue::fromHandle(hCommandQueue);
    EXPECT_EQ(ZE_RESULT_SUCCESS, commandQueue->destroy());
}

TEST_F(CommandQueueCreate, expectCommandQueueIsDestroyOnContextDestroy) {
    ze_command_queue_handle_t hCommandQueue = createCommandQueue(getComputeQueueOrdinal());
    ASSERT_NE(hCommandQueue, nullptr);
}

struct CommandQueueExecTest : Test<CommandQueueFixture> {
    void SetUp() override {
        CommandQueueFixture::SetUp();

        // NN queue.
        ze_command_queue_handle_t hNNCommandQueue = createCommandQueue(getComputeQueueOrdinal());
        ASSERT_NE(hNNCommandQueue, nullptr);
        nnQue = CommandQueue::fromHandle(hNNCommandQueue);

        // Copy queue.
        ze_command_queue_handle_t hCPCommandQueue = createCommandQueue(getCopyOnlyQueueOrdinal());
        ASSERT_NE(hCPCommandQueue, nullptr);
        cpQue = CommandQueue::fromHandle(hCPCommandQueue);

        // Event.
        ze_event_pool_desc_t evPoolDesc = {.stype = ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                           .pNext = nullptr,
                                           .flags = ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                           .count = 3};
        ASSERT_NE(nullptr, device);
        auto hDevice = device->toHandle();
        zeEventPoolCreate(context, &evPoolDesc, 1, &(hDevice), &eventPool);
        ASSERT_NE(nullptr, eventPool);

        ze_event_desc_t evDesc = {
            .stype = ZE_STRUCTURE_TYPE_EVENT_DESC,
            .pNext = nullptr,
            .index = 0,
            .signal = ZE_EVENT_SCOPE_FLAG_HOST,
            .wait = ZE_EVENT_SCOPE_FLAG_HOST,
        };

        evDesc.index = 0;
        zeEventCreate(eventPool, &evDesc, &event0);
        ASSERT_NE(nullptr, event0);

        evDesc.index = 1;
        zeEventCreate(eventPool, &evDesc, &event1);
        ASSERT_NE(nullptr, event1);

        evDesc.index = 2;
        zeEventCreate(eventPool, &evDesc, &event2);
        ASSERT_NE(nullptr, event2);
    }

    void TearDown() override {
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(event0));
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(event1));
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventDestroy(event2));
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeEventPoolDestroy(eventPool));

        ASSERT_EQ(ZE_RESULT_SUCCESS, nnQue->destroy());
        ASSERT_EQ(ZE_RESULT_SUCCESS, cpQue->destroy());
        CommandQueueFixture::TearDown();
    }

    // Command queues.
    CommandQueue *nnQue = nullptr;
    CommandQueue *cpQue = nullptr;

    // Event pool.
    ze_event_pool_handle_t eventPool = nullptr;

    // Event handles.
    ze_event_handle_t event0 = nullptr;
    ze_event_handle_t event1 = nullptr;
    ze_event_handle_t event2 = nullptr;
};

TEST_F(
    CommandQueueExecTest,
    givenCmdQueueWithBlitCopyWhenExecutingNonCopyBlitCommandListThenWrongCommandListStatusReturned) {
    auto hCommandList = createCommandList(getComputeQueueOrdinal());
    ASSERT_NE(nullptr, hCommandList);

    // Execute the queue.
    auto status = cpQue->executeCommandLists(1, &hCommandList, nullptr);
    EXPECT_EQ(status, ZE_RESULT_ERROR_INVALID_COMMAND_LIST_TYPE);

    L0::zeCommandListDestroy(hCommandList);
}

TEST_F(CommandQueueExecTest,
       givenCmdQueueWithBlitCopyWhenExecutingCopyBlitCommandListThenSuccessReturned) {
    auto hCommandList = createCommandList(getComputeQueueOrdinal());
    ASSERT_NE(nullptr, hCommandList);

    CommandList *nnCmdList = CommandList::fromHandle(hCommandList);

    // Append a copy command.
    void *srcPtr = ctx->createSharedMemAlloc(4 * 1024);
    void *destPtr = ctx->createSharedMemAlloc(4 * 1024);
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
    auto hCommandList = createCommandList(getComputeQueueOrdinal());
    ASSERT_NE(nullptr, hCommandList);

    CommandList *cmdList = CommandList::fromHandle(hCommandList);

    // Append a TS command.
    uint64_t *ts = static_cast<uint64_t *>(ctx->createSharedMemAlloc(64));
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
    auto hCommandList = createCommandList(getComputeQueueOrdinal());
    ASSERT_NE(nullptr, hCommandList);

    CommandList *cmdList = CommandList::fromHandle(hCommandList);

    // Append a TS command and ready to execute.
    uint64_t *ts = static_cast<uint64_t *>(ctx->createSharedMemAlloc(64));
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
    auto hCommandList = createCommandList(getComputeQueueOrdinal());
    ASSERT_NE(nullptr, hCommandList);

    CommandList *cmdList = CommandList::fromHandle(hCommandList);

    // Resetting previous call counts
    osInfc.callCntAlloc = 0;
    osInfc.callCntFree = 0;

    // At least one command is required for successful submission.
    uint64_t *ts = static_cast<uint64_t *>(ctx->createSharedMemAlloc(64));
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

    auto srcShareMem = ctx->createSharedMemAlloc(testAllocSize);
    auto destShareMem = ctx->createSharedMemAlloc(testAllocSize);
    auto destHostMem = ctx->createHostMemAlloc(testAllocSize);

    // NN queue event attaching.
    // Append wait on event 0, L2L copy and signal event 1 to the same NN queue.
    auto hNNCmdlist = createCommandList(getComputeQueueOrdinal());
    ASSERT_NE(nullptr, hNNCmdlist);

    CommandList *nnCmdlist = CommandList::fromHandle(hNNCmdlist);

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
    EXPECT_EQ(1U, nnQue->getSubmittedJobCount());

    // COPY queue event attaching.
    // Append wait on event 0, L2S copy and signal event 1 to the same COPY queue.
    auto hCPCmdlist = createCommandList(getCopyOnlyQueueOrdinal());
    ASSERT_NE(nullptr, hCPCmdlist);

    CommandList *cpCmdlist = CommandList::fromHandle(hCPCmdlist);

    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->appendWaitOnEvents(1, &event0));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              cpCmdlist->appendMemoryCopy((void *)destHostMem,
                                          (void *)srcShareMem,
                                          testAllocSize,
                                          nullptr,
                                          0,
                                          nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->appendSignalEvent(event1));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cpCmdlist->close());
    ASSERT_EQ(cpQue->executeCommandLists(1, &hCPCmdlist, nullptr), ZE_RESULT_SUCCESS);

    // Make sure a single command buffer was submitted.
    EXPECT_EQ(1U, cpQue->getSubmittedJobCount());

    // Deallocate instances
    ctx->freeMemAlloc(srcShareMem);
    ctx->freeMemAlloc(destShareMem);
    ctx->freeMemAlloc(destHostMem);

    nnCmdlist->destroy();
    cpCmdlist->destroy();
}

struct CommandQueueJobTest : public Test<CommandQueueFixture> {
    void SetUp() override {
        CommandQueueFixture::SetUp();

        // Create a CP command queue.
        ze_command_queue_handle_t hNNCommandQueue = createCommandQueue(getComputeQueueOrdinal());
        ASSERT_NE(hNNCommandQueue, nullptr);
        nnCmdque = CommandQueue::fromHandle(hNNCommandQueue);

        // Create a NN command queue.
        ze_command_queue_handle_t hCPCommandQueue = createCommandQueue(getCopyOnlyQueueOrdinal());
        ASSERT_NE(hCPCommandQueue, nullptr);
        cpCmdque = CommandQueue::fromHandle(hCPCommandQueue);

        // Command lists.
        hNNCmdlist = createCommandList(getComputeQueueOrdinal());
        ASSERT_NE(nullptr, hNNCmdlist);
        nnCmdlist = CommandList::fromHandle(hNNCmdlist);
        ASSERT_NE(nullptr, nnCmdlist);

        hCPCmdlist = createCommandList(getCopyOnlyQueueOrdinal());
        ASSERT_NE(nullptr, hCPCmdlist);
        cpCmdlist = CommandList::fromHandle(hCPCmdlist);

        // Fence.
        ze_fence_desc_t fenceDesc{.stype = ZE_STRUCTURE_TYPE_FENCE_DESC,
                                  .pNext = nullptr,
                                  .flags = 0};
        ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdque->createFence(&fenceDesc, &hFence));
        fence = static_cast<Fence *>(hFence);
        ASSERT_NE(nullptr, hFence);
        ASSERT_NE(nullptr, fence);
    }

    void TearDown() override {
        if (nnCmdque != nullptr) {
            nnCmdque->destroy();
        }
        if (cpCmdque != nullptr) {
            cpCmdque->destroy();
        }
        if (nnCmdlist != nullptr) {
            nnCmdlist->destroy();
        }
        if (cpCmdlist != nullptr) {
            cpCmdlist->destroy();
        }
        if (fence != nullptr) {
            fence->destroy();
        }

        CommandQueueFixture::TearDown();
    }

    const size_t memAllocSize = 4 * 1024; // Default memory allocation size.
    uint64_t syncTimeout = 2e9;           // 2seconds default time out.

    ze_fence_handle_t hFence = nullptr;
    Fence *fence = nullptr;

    CommandQueue *nnCmdque = nullptr;
    CommandQueue *cpCmdque = nullptr;

    ze_command_list_handle_t hNNCmdlist = nullptr;
    ze_command_list_handle_t hCPCmdlist = nullptr;
    CommandList *nnCmdlist = nullptr;
    CommandList *cpCmdlist = nullptr;
};

TEST_F(CommandQueueJobTest, emptyCommandListDoesNotKeepJob) {
    // Empty NN command list.
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());

    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdque->executeCommandLists(1, &hNNCmdlist, nullptr));
    EXPECT_EQ(0u, nnCmdque->getSubmittedJobCount());

    // Call buffer sync against failed command queue.
    EXPECT_EQ(ZE_RESULT_SUCCESS, nnCmdque->synchronize(syncTimeout));
}

TEST_F(CommandQueueJobTest, submittedJobsShouldBeKept) {
    // Execute non empty NN command list.
    uint64_t *tsDest = static_cast<uint64_t *>(ctx->createSharedMemAlloc(memAllocSize));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(tsDest, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdque->executeCommandLists(1, &hNNCmdlist, nullptr));

    // 1 BB should be added in the cmd queue.
    EXPECT_EQ(1u, nnCmdque->getSubmittedJobCount());

    // Deallocate memory.
    ASSERT_TRUE(ctx->freeMemAlloc(tsDest));
}

TEST_F(CommandQueueJobTest, submittedJobsShouldBeKeptInFence) {
    // Execute non empty NN command list.
    uint64_t *tsDest = static_cast<uint64_t *>(ctx->createSharedMemAlloc(memAllocSize));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(tsDest, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdque->executeCommandLists(1, &hNNCmdlist, hFence));
    ASSERT_EQ(1u, nnCmdque->getSubmittedJobCount());

    // Associated fence should have the submitted command buffer.
    EXPECT_EQ(1u, fence->getTrackedJobCount());

    // Deallocate the memory.
    ASSERT_TRUE(ctx->freeMemAlloc(tsDest));
}

TEST_F(CommandQueueJobTest, submittedJobsShouldBeKeptUntilQueueDestroyed) {
    // First execution.
    uint64_t *tsDest = static_cast<uint64_t *>(ctx->createSharedMemAlloc(memAllocSize));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(tsDest, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdque->executeCommandLists(1, &hNNCmdlist, hFence));
    ASSERT_EQ(1u, nnCmdque->getSubmittedJobCount());

    // Reset the fence.
    fence->reset();
    EXPECT_EQ(1u, fence->getTrackedJobCount());

    // Second execution.
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->reset());
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist->appendWriteGlobalTimestamp(tsDest, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist->close());
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdque->executeCommandLists(1, &hNNCmdlist, hFence));
    EXPECT_EQ(2u, nnCmdque->getSubmittedJobCount());

    // Destroy command queue.
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdque->destroy());
    nnCmdque = nullptr;

    // Submitted command buffers in fence should be kept after queue destroyed.
    EXPECT_EQ(1u, fence->getTrackedJobCount());

    // Deallocate the memory.
    ctx->freeMemAlloc(tsDest);
}

TEST_F(CommandQueueJobTest, multipleJobsMaintainedInSingleSubmitId) {
    // NN(TS) and CP(L2S) combined command will create two command buffers in execution.
    uint64_t *tsDest = static_cast<uint64_t *>(ctx->createSharedMemAlloc(memAllocSize));
    void *srcShareMem = ctx->createSharedMemAlloc(memAllocSize);
    void *dstHostMem = ctx->createHostMemAlloc(memAllocSize);
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
    auto hNNCmdlist1 = createCommandList(getComputeQueueOrdinal());
    ASSERT_NE(nullptr, hNNCmdlist1);
    auto nnCmdlist1 = CommandList::fromHandle(hNNCmdlist1);

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              nnCmdlist1->appendWriteGlobalTimestamp(tsDest, nullptr, 0, nullptr));
    ASSERT_EQ(
        ZE_RESULT_SUCCESS,
        nnCmdlist1->appendMemoryCopy(dstHostMem, srcShareMem, memAllocSize, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist1->close());

    auto hNNCmdlist2 = createCommandList(getComputeQueueOrdinal());
    ASSERT_NE(nullptr, hNNCmdlist2);
    auto nnCmdlist2 = CommandList::fromHandle(hNNCmdlist2);

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
    EXPECT_EQ(3u, nnCmdque->getSubmittedJobCount());

    // Deallocate the memory.
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist1->destroy());
    ASSERT_EQ(ZE_RESULT_SUCCESS, nnCmdlist2->destroy());
    ASSERT_TRUE(ctx->freeMemAlloc(tsDest));
    ASSERT_TRUE(ctx->freeMemAlloc(srcShareMem));
    ASSERT_TRUE(ctx->freeMemAlloc(dstHostMem));
}

} // namespace ult
} // namespace L0
