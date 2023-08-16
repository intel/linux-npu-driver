/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"
#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/command/vpu_barrier_command.hpp"
#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"
#include "vpu_driver/source/command/vpu_graph_exe_command.hpp"
#include "vpu_driver/source/command/vpu_graph_init_command.hpp"
#include "vpu_driver/source/command/vpu_ts_command.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include "gtest/gtest.h"

#include <memory>

namespace VPU {

struct VPUCommandBufferTest : public ::testing::Test {
    void SetUp() override {}

    void TearDown() override { ASSERT_EQ(ctx->getBuffersCount(), 0u); }

    MockOsInterfaceImp osInfc;
    std::unique_ptr<MockVPUDevice> vpuDevice = MockVPUDevice::createWithDefaultHardwareInfo(osInfc);
    std::unique_ptr<VPUDeviceContext> deviceContext = vpuDevice->createDeviceContext();
    VPUDeviceContext *ctx = deviceContext.get();
};

TEST_F(VPUCommandBufferTest, allocateCommandBufferWithoutCommandExpectNullptr) {
    EXPECT_EQ(nullptr,
              VPUCommandBuffer::allocateCommandBuffer(ctx, {}, VPUCommandBuffer::Target::COMPUTE));
}

TEST_F(VPUCommandBufferTest, allocateCommandBufferWithTimestampCommand) {
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(ctx->createSharedMemAlloc(sizeof(uint64_t)));

    std::vector<std::shared_ptr<VPUCommand>> cmds;
    cmds.emplace_back(VPUTimeStampCommand::create(ctx, tsHeap));
    ASSERT_NE(cmds.back(), nullptr);

    EXPECT_NE(
        nullptr,
        VPUCommandBuffer::allocateCommandBuffer(ctx, cmds, VPUCommandBuffer::Target::COMPUTE));

    EXPECT_TRUE(ctx->freeMemAlloc(tsHeap));
}

TEST_F(VPUCommandBufferTest, allocateCommandBufferWithMallocFailureExpectNullptr) {
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(ctx->createSharedMemAlloc(sizeof(uint64_t)));

    std::vector<std::shared_ptr<VPUCommand>> cmds;
    cmds.emplace_back(VPUTimeStampCommand::create(ctx, tsHeap));
    ASSERT_NE(cmds.back(), nullptr);

    osInfc.mockFailNextAlloc();
    EXPECT_EQ(
        nullptr,
        VPUCommandBuffer::allocateCommandBuffer(ctx, cmds, VPUCommandBuffer::Target::COMPUTE));
    EXPECT_TRUE(ctx->freeMemAlloc(tsHeap));
}

TEST_F(VPUCommandBufferTest, allocateCommandBufferWithCopyCommand) {
    void *src = ctx->createSharedMemAlloc(sizeof(uint64_t));
    void *dst = ctx->createSharedMemAlloc(sizeof(uint64_t));

    std::vector<std::shared_ptr<VPUCommand>> cmds;
    cmds.emplace_back(VPUCopyCommand::create(ctx, src, dst, sizeof(uint64_t)));
    ASSERT_NE(cmds.back(), nullptr);

    EXPECT_NE(
        nullptr,
        VPUCommandBuffer::allocateCommandBuffer(ctx, cmds, VPUCommandBuffer::Target::COMPUTE));

    EXPECT_TRUE(ctx->freeMemAlloc(src));
    EXPECT_TRUE(ctx->freeMemAlloc(dst));
}

TEST_F(VPUCommandBufferTest, allocateCommandBufferWithInitAndExecuteGraphCommands) {
    uint64_t umdBlobId = 0xcafebebedeadbeef;

    const size_t blobSize = 4 * 1024;
    uint8_t blobData[blobSize] = {};
    const uint32_t scratchSize = 4 * 1024;

    uint32_t memSize = 4096;
    void *inputData = ctx->createSharedMemAlloc(memSize);
    void *outputData = ctx->createSharedMemAlloc(memSize);

    std::vector<std::shared_ptr<VPUCommand>> cmds;
    cmds.emplace_back(
        VPUGraphInitCommand::create(ctx, umdBlobId, blobData, blobSize, scratchSize, scratchSize));
    ASSERT_NE(cmds.back(), nullptr);

    cmds.emplace_back(VPUGraphExecuteCommand::create(
        ctx,
        umdBlobId,
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(inputData, memSize)}},
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(outputData, memSize)}},
        cmds.back()->getAssociateBufferObjects()));
    ASSERT_NE(cmds.back(), nullptr);

    EXPECT_NE(
        nullptr,
        VPUCommandBuffer::allocateCommandBuffer(ctx, cmds, VPUCommandBuffer::Target::COMPUTE));

    EXPECT_TRUE(ctx->freeMemAlloc(inputData));
    EXPECT_TRUE(ctx->freeMemAlloc(outputData));
}

} // namespace VPU
