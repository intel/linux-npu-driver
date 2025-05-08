/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "gtest/gtest.h"
#include "vpu_driver/source/command/vpu_command_buffer.hpp"
#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/command/vpu_ts_command.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include <memory>
#include <string>
#include <vector>

namespace VPU {
class VPUCommand;

struct VPUCommandBufferTest : public ::testing::Test {
    void SetUp() override {}

    void TearDown() override { ASSERT_EQ(ctx->getBuffersCount(), 0u); }

    MockOsInterfaceImp osInfc;
    std::unique_ptr<MockVPUDevice> vpuDevice = MockVPUDevice::createWithDefaultHardwareInfo(osInfc);
    std::unique_ptr<MockVPUDeviceContext> deviceContext = vpuDevice->createMockDeviceContext();
    MockVPUDeviceContext *ctx = deviceContext.get();
    std::shared_ptr<VPUBufferObject> eventBo = nullptr;
};

TEST_F(VPUCommandBufferTest, allocateCommandBufferWithoutCommandExpectNullptr) {
    std::vector<std::shared_ptr<VPUCommand>> cmds;
    EXPECT_EQ(
        nullptr,
        VPUCommandBuffer::allocateCommandBuffer(ctx, cmds.begin(), cmds.end(), nullptr, eventBo));
}

TEST_F(VPUCommandBufferTest, allocateCommandBufferWithTimestampCommand) {
    auto tsHeap = ctx->createSharedMemAlloc(sizeof(uint64_t));

    std::vector<std::shared_ptr<VPUCommand>> cmds;
    cmds.emplace_back(
        VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(tsHeap->getBasePointer()),
                                    tsHeap));
    ASSERT_NE(cmds.back(), nullptr);

    EXPECT_NE(
        nullptr,
        VPUCommandBuffer::allocateCommandBuffer(ctx, cmds.begin(), cmds.end(), nullptr, eventBo));
    EXPECT_TRUE(ctx->freeMemAlloc(tsHeap->getBasePointer()));
}

TEST_F(VPUCommandBufferTest, allocateCommandBufferWithMallocFailureExpectNullptr) {
    auto tsHeap = ctx->createSharedMemAlloc(sizeof(uint64_t));

    std::vector<std::shared_ptr<VPUCommand>> cmds;
    cmds.emplace_back(
        VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(tsHeap->getBasePointer()),
                                    tsHeap));
    ASSERT_NE(cmds.back(), nullptr);

    osInfc.mockFailNextAlloc();
    EXPECT_EQ(
        nullptr,
        VPUCommandBuffer::allocateCommandBuffer(ctx, cmds.begin(), cmds.end(), nullptr, eventBo));
    EXPECT_TRUE(ctx->freeMemAlloc(tsHeap->getBasePointer()));
}

TEST_F(VPUCommandBufferTest, allocateCommandBufferWithCopyCommand) {
    auto srcBo = ctx->createSharedMemAlloc(sizeof(uint64_t));
    auto dstBo = ctx->createSharedMemAlloc(sizeof(uint64_t));

    std::vector<std::shared_ptr<VPUCommand>> cmds;
    cmds.emplace_back(VPUCopyCommand::create(ctx,
                                             srcBo->getBasePointer(),
                                             srcBo,
                                             dstBo->getBasePointer(),
                                             dstBo,
                                             sizeof(uint64_t)));
    ASSERT_NE(cmds.back(), nullptr);

    EXPECT_NE(
        nullptr,
        VPUCommandBuffer::allocateCommandBuffer(ctx, cmds.begin(), cmds.end(), nullptr, eventBo));
    EXPECT_TRUE(ctx->freeMemAlloc(srcBo->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(dstBo->getBasePointer()));
}

} // namespace VPU
