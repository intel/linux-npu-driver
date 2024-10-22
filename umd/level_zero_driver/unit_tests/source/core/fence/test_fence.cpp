/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "gtest/gtest.h"
#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/core/source/cmdqueue/cmdqueue.hpp"
#include "level_zero_driver/core/source/fence/fence.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <level_zero/ze_api.h>
#include <string>

namespace L0 {
namespace ult {

struct FenceTest : public Test<CommandQueueFixture> {
    void SetUp() override {
        CommandQueueFixture::SetUp();

        hCommandQueue = createCommandQueue();
        ASSERT_NE(hCommandQueue, nullptr);
        cmdQue = L0::CommandQueue::fromHandle(hCommandQueue);

        hCmdList = createCommandList();
        ASSERT_NE(nullptr, hCmdList);
        cmdList = L0::CommandList::fromHandle(hCmdList);

        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  L0::CommandQueue::fromHandle(hCommandQueue)->createFence(&fenceDesc, &hFence));
        ASSERT_NE(nullptr, hFence);
        fence = static_cast<Fence *>(hFence);
    }

    void TearDown() override {
        if (fence) {
            EXPECT_EQ(fence->destroy(), ZE_RESULT_SUCCESS);
        }
        if (cmdList) {
            EXPECT_EQ(cmdList->destroy(), ZE_RESULT_SUCCESS);
        }
        if (cmdQue) {
            EXPECT_EQ(cmdQue->destroy(), ZE_RESULT_SUCCESS);
        }
        CommandQueueFixture::TearDown();
    }

    ze_fence_desc_t fenceDesc{.stype = ZE_STRUCTURE_TYPE_FENCE_DESC, .pNext = nullptr, .flags = 0};
    ze_command_queue_handle_t hCommandQueue = nullptr;
    L0::CommandQueue *cmdQue = nullptr;
    ze_command_list_handle_t hCmdList = nullptr;
    L0::CommandList *cmdList = nullptr;
    ze_fence_handle_t hFence = nullptr;
    L0::Fence *fence = nullptr;

    const size_t memAllocSize = 64;
};

TEST_F(FenceTest, createErrors) {
    ze_fence_handle_t hFence = nullptr;
    ASSERT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              L0::CommandQueue::fromHandle(hCommandQueue)->createFence(nullptr, &hFence));
    ASSERT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              L0::CommandQueue::fromHandle(hCommandQueue)->createFence(&fenceDesc, nullptr));
}

TEST_F(FenceTest, synchornizeAndQueryStatusShouldReturnSameStatus) {
    EXPECT_EQ(fence->synchronize(0), fence->queryStatus());
}

TEST_F(FenceTest, emptyCommandQueueSynchronizeDoesNotChangeFenceState) {
    EXPECT_EQ(ZE_RESULT_NOT_READY, fence->queryStatus());
    EXPECT_EQ(ZE_RESULT_SUCCESS, cmdQue->synchronize(0));
    EXPECT_EQ(ZE_RESULT_NOT_READY, fence->queryStatus());
}

TEST_F(FenceTest, queryStatusReturnSuccessAfterJobIsCompleted) {
    uint64_t *mem =
        static_cast<uint64_t *>(ctx->createMemAlloc(memAllocSize,
                                                    VPU::VPUBufferObject::Type::CachedFw,
                                                    VPU::VPUBufferObject::Location::Shared));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cmdList->appendWriteGlobalTimestamp(mem, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cmdList->close());
    ASSERT_EQ(ZE_RESULT_SUCCESS, cmdQue->executeCommandLists(1, &hCmdList, hFence));
    EXPECT_EQ(ZE_RESULT_SUCCESS, fence->queryStatus());
    EXPECT_EQ(ZE_RESULT_SUCCESS, fence->synchronize(0));
    EXPECT_EQ(ZE_RESULT_SUCCESS, cmdQue->synchronize(0));

    ASSERT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(FenceTest, queryStatusReturnsNotReadyAfterFenceReset) {
    uint64_t *mem =
        static_cast<uint64_t *>(ctx->createMemAlloc(memAllocSize,
                                                    VPU::VPUBufferObject::Type::CachedFw,
                                                    VPU::VPUBufferObject::Location::Shared));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cmdList->appendWriteGlobalTimestamp(mem, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cmdList->close());
    ASSERT_EQ(ZE_RESULT_SUCCESS, cmdQue->executeCommandLists(1, &hCmdList, hFence));
    EXPECT_EQ(ZE_RESULT_SUCCESS, fence->queryStatus());
    EXPECT_EQ(ZE_RESULT_SUCCESS, fence->reset());
    EXPECT_EQ(ZE_RESULT_NOT_READY, fence->queryStatus());
    EXPECT_EQ(ZE_RESULT_SUCCESS, cmdQue->synchronize(0));
    EXPECT_EQ(ZE_RESULT_NOT_READY, fence->queryStatus());

    ASSERT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(FenceTest, commandQueueShouldNotChangeFenceStateIfFenceIsNotUsed) {
    uint64_t *mem =
        static_cast<uint64_t *>(ctx->createMemAlloc(memAllocSize,
                                                    VPU::VPUBufferObject::Type::CachedFw,
                                                    VPU::VPUBufferObject::Location::Shared));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cmdList->appendWriteGlobalTimestamp(mem, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, cmdList->close());
    ASSERT_EQ(ZE_RESULT_SUCCESS, cmdQue->executeCommandLists(1, &hCmdList, hFence));
    EXPECT_EQ(ZE_RESULT_SUCCESS, cmdQue->synchronize(0));
    EXPECT_EQ(ZE_RESULT_SUCCESS, fence->queryStatus());
    EXPECT_EQ(ZE_RESULT_SUCCESS, fence->reset());
    EXPECT_EQ(ZE_RESULT_NOT_READY, fence->queryStatus());
    ASSERT_EQ(ZE_RESULT_SUCCESS, cmdQue->executeCommandLists(1, &hCmdList, nullptr));
    EXPECT_EQ(ZE_RESULT_SUCCESS, cmdQue->synchronize(0));
    EXPECT_EQ(ZE_RESULT_NOT_READY, fence->queryStatus());

    ASSERT_TRUE(ctx->freeMemAlloc(mem));
}

} // namespace ult
} // namespace L0
