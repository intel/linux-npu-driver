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

        cmdQue = new CommandQueue(device, &cmdQueDesc, ctx, false);
        EXPECT_NE(nullptr, cmdQue);

        auto res = cmdQue->createFence(&fenceDesc, &hFence);
        EXPECT_EQ(res, ZE_RESULT_SUCCESS);

        fence = static_cast<Fence *>(hFence);
        EXPECT_NE(nullptr, fence);
    }

    void TearDown() override {
        ContextFixture::TearDown();

        ze_result_t res = fence->destroy();
        EXPECT_EQ(res, ZE_RESULT_SUCCESS);

        res = cmdQue->destroy();
        EXPECT_EQ(res, ZE_RESULT_SUCCESS);
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
    MockFence(CommandQueue *cmdQue, const ze_fence_desc_t *desc)
        : Fence(cmdQue, desc) {}

    void updateSignal(bool state) { signaled = state; }
    bool getSignal() { return signaled; }
};

TEST_F(FenceTest, givenCallResetShouldClearHostSyncData) {
    ze_fence_desc_t desc = {.stype = ZE_STRUCTURE_TYPE_FENCE_DESC, .pNext = nullptr, .flags = 0u};
    MockFence mockFence(cmdQue, &desc);
    // Assume the sync status has been updated by KMD.
    mockFence.updateSignal(true);

    ze_result_t res = mockFence.reset();
    EXPECT_EQ(ZE_RESULT_SUCCESS, res);
    EXPECT_EQ(false, mockFence.getSignal());
}

} // namespace ult
} // namespace L0
