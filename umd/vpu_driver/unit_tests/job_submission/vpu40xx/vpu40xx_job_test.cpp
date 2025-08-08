/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "gtest/gtest.h"
#include "vpu_driver/source/command/command_buffer.hpp"
#include "vpu_driver/source/command/copy_command.hpp"
#include "vpu_driver/source/command/job.hpp"
#include "vpu_driver/source/command/ts_command.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include <array>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace VPU {
class VPUCommand;
} // namespace VPU

using namespace VPU;

static size_t getExpBufferCount(const std::vector<uint32_t> &cmd_handles) {
    std::set<uint32_t> uniqueHandles(cmd_handles.begin(), cmd_handles.end());
    return uniqueHandles.size();
}

struct VPUJobTestForVPU40xx : public ::testing::Test {
    void SetUp() {}

    void TearDown() { ASSERT_EQ(ctx->getBuffersCount(), 0u); }

    std::unique_ptr<MockOsInterfaceImp> osInfc = std::make_unique<MockOsInterfaceImp>(0x643e);
    std::unique_ptr<MockVPUDevice> vpuDevice =
        MockVPUDevice::createWithDefaultHardwareInfo(*osInfc.get());
    std::unique_ptr<MockVPUDeviceContext> deviceContext = vpuDevice->createMockDeviceContext();
    MockVPUDeviceContext *ctx = deviceContext.get();

    const size_t allocSize = 4 * 1024;
};

TEST_F(VPUJobTestForVPU40xx, jobCanGenerateSingleCopyBuffer) {
    ASSERT_EQ(ctx->getPciDevId(), 0x643e);
    int cmdCount = 3;

    auto sharedMem = ctx->createSharedMemAlloc(allocSize);
    auto hostMem = ctx->createHostMemAlloc(allocSize);

    auto job = std::make_unique<VPUJob>(ctx);
    for (int i = 0; i < cmdCount; i++)
        EXPECT_TRUE(job->appendCommand(VPUCopyCommand::create(ctx,
                                                              hostMem->getBasePointer(),
                                                              hostMem,
                                                              sharedMem->getBasePointer(),
                                                              sharedMem,
                                                              allocSize)));
    EXPECT_TRUE(job->closeCommands());

    EXPECT_EQ(1u, job->getCommandBuffers().size());
    for (const auto &cmdBuffer : job->getCommandBuffers()) {
        EXPECT_EQ(getExpBufferCount(cmdBuffer->getBufferHandles()),
                  cmdBuffer->getBufferHandles().size());
    }

    EXPECT_TRUE(ctx->freeMemAlloc(hostMem->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(sharedMem->getBasePointer()));
}

TEST_F(VPUJobTestForVPU40xx, jobShouldProperlySaveAppendedCommands) {
    ASSERT_EQ(ctx->getPciDevId(), 0x643e);

    auto tsHeap = ctx->createSharedMemAlloc(sizeof(uint64_t));
    ASSERT_NE(tsHeap, nullptr);
    auto sharedMem = ctx->createSharedMemAlloc(allocSize);
    auto hostMem = ctx->createHostMemAlloc(allocSize);

    // allCmds store commands that are pushed to VPUJob
    std::vector<std::shared_ptr<VPUCommand>> allCmds = {};
    // engineCmds store expected commands segregated by VPUJob per engine
    std::array<std::vector<std::shared_ptr<VPUCommand>>, 2> engineCmds = {};

    auto job = std::make_unique<VPUJob>(ctx);

    for (int i = 0; i < 3; i++)
        EXPECT_TRUE(job->appendCommand(
            VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(tsHeap->getBasePointer()),
                                        tsHeap)));

    for (int i = 0; i < 3; i++)
        EXPECT_TRUE(job->appendCommand(VPUCopyCommand::create(ctx,
                                                              sharedMem->getBasePointer(),
                                                              sharedMem,
                                                              sharedMem->getBasePointer(),
                                                              sharedMem,
                                                              allocSize)));

    for (int i = 0; i < 3; i++)
        EXPECT_TRUE(job->appendCommand(VPUCopyCommand::create(ctx,
                                                              hostMem->getBasePointer(),
                                                              hostMem,
                                                              sharedMem->getBasePointer(),
                                                              sharedMem,
                                                              allocSize)));

    EXPECT_TRUE(job->closeCommands());

    EXPECT_EQ(1u, job->getCommandBuffers().size());
    for (size_t i = 0; i < job->getCommandBuffers().size(); i++) {
        const auto &cmdBuffer = job->getCommandBuffers()[i];
        EXPECT_EQ(getExpBufferCount(cmdBuffer->getBufferHandles()),
                  cmdBuffer->getBufferHandles().size());
    }

    EXPECT_TRUE(ctx->freeMemAlloc(sharedMem->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(hostMem->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(tsHeap->getBasePointer()));
}
