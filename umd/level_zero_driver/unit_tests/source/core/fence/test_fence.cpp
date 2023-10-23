/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gtest/gtest.h"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include "level_zero_driver/core/source/fence/fence.hpp"
#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/core/source/cmdqueue/cmdqueue.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"

#include <chrono>

namespace L0 {
namespace ult {

struct FenceTest : public ContextFixture, public testing::Test {
    void SetUp() override {
        ContextFixture::SetUp();

        ze_command_queue_handle_t hCommandQueue = nullptr;
        EXPECT_EQ(CommandQueue::create(context, device, &cmdQueDesc, &hCommandQueue),
                  ZE_RESULT_SUCCESS);
        ASSERT_NE(hCommandQueue, nullptr);

        cmdQue = CommandQueue::fromHandle(hCommandQueue);
        EXPECT_EQ(cmdQue->createFence(&fenceDesc, &hFence), ZE_RESULT_SUCCESS);

        fence = static_cast<Fence *>(hFence);
        EXPECT_NE(nullptr, fence);
    }

    void TearDown() override {
        if (fence) {
            EXPECT_EQ(fence->destroy(), ZE_RESULT_SUCCESS);
        }
        if (cmdQue) {
            EXPECT_EQ(cmdQue->destroy(), ZE_RESULT_SUCCESS);
        }
        ContextFixture::TearDown();
    }

    // Descs.
    ze_command_queue_desc_t cmdQueDesc = {};
    ze_fence_desc_t fenceDesc = {.stype = ZE_STRUCTURE_TYPE_FENCE_DESC,
                                 .pNext = nullptr,
                                 .flags = 0u};

    // Handles.
    ze_fence_handle_t hFence;
    Fence *fence;

    // Command queue.
    CommandQueue *cmdQue;
};

TEST_F(FenceTest, zeroTimoutShouldReturnTheSameAsQueryStatus) {
    // Timeout 0 shell return the same result as the queryStatus()
    auto syncResult = fence->hostSynchronize(0);
    auto queryResult = fence->queryStatus();

    EXPECT_EQ(syncResult, queryResult);
}

struct MockFence : public Fence {
    MockFence(Context *pContext, const ze_fence_desc_t *desc)
        : Fence(pContext, desc) {}

    void updateSignal(bool state) { signaled = state; }
    bool getSignal() { return signaled; }
};

TEST_F(FenceTest, givenCallResetShouldClearHostSyncData) {
    ze_fence_desc_t desc = {.stype = ZE_STRUCTURE_TYPE_FENCE_DESC, .pNext = nullptr, .flags = 0u};
    MockFence mockFence(context, &desc);
    // Assume the sync status has been updated by KMD.
    mockFence.updateSignal(true);

    ze_result_t res = mockFence.reset();
    EXPECT_EQ(ZE_RESULT_SUCCESS, res);
    EXPECT_EQ(false, mockFence.getSignal());
}

} // namespace ult
} // namespace L0
