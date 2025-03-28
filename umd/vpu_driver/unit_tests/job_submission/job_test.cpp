/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "gtest/gtest.h"
#include "vpu_driver/source/command/vpu_command_buffer.hpp"
#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"
#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/command/vpu_ts_command.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include <memory>
#include <set>
#include <string>
#include <vector>

using namespace VPU;

static size_t getExpBufferCount(const std::vector<uint32_t> &cmd_handles) {
    std::set<uint32_t> uniqueHandles(cmd_handles.begin(), cmd_handles.end());
    return uniqueHandles.size();
}

struct VPUJobTest : public ::testing::Test {
    void SetUp() override {}
    void TearDown() override { ASSERT_EQ(ctx->getBuffersCount(), 0u); }

    MockOsInterfaceImp osInfc;
    std::unique_ptr<MockVPUDevice> vpuDevice = MockVPUDevice::createWithDefaultHardwareInfo(osInfc);
    std::unique_ptr<MockVPUDeviceContext> deviceContext = vpuDevice->createMockDeviceContext();
    MockVPUDeviceContext *ctx = deviceContext.get();

    const uint32_t allocSize = 4 * 1024;
};

TEST_F(VPUJobTest, closeCommandsOnJobWithoutCommandsReturnTrue) {
    auto job = std::make_unique<VPUJob>(ctx);
    EXPECT_TRUE(job->closeCommands());
}

TEST_F(VPUJobTest, afterCloseCommandsCallAppendCommandReturnFalse) {
    auto job = std::make_unique<VPUJob>(ctx);
    EXPECT_TRUE(job->closeCommands());

    void *mem = ctx->createSharedMemAlloc(sizeof(uint64_t));
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(mem);

    EXPECT_FALSE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));
    EXPECT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(VPUJobTest, createJobWithTimestampAndDoNotCloseItExpectNoCommandBuffers) {
    auto job = std::make_unique<VPUJob>(ctx);

    void *mem = ctx->createSharedMemAlloc(sizeof(uint64_t));
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(mem);

    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));
    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));
    EXPECT_EQ(0u, job->getCommandBuffers().size());

    EXPECT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(VPUJobTest, createJobWithTimestampCommands) {
    const int cmdCount = 4;

    void *mem = ctx->createSharedMemAlloc(sizeof(uint64_t) * cmdCount);
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(mem);

    auto job = std::make_unique<VPUJob>(ctx);
    for (int i = 0; i < cmdCount; i++)
        EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap++)));
    EXPECT_TRUE(job->closeCommands());

    EXPECT_EQ(1u, job->getCommandBuffers().size());
    for (const auto &cmdBuffer : job->getCommandBuffers()) {
        EXPECT_EQ(getExpBufferCount(cmdBuffer->getBufferHandles()),
                  cmdBuffer->getBufferHandles().size());
    }

    EXPECT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(VPUJobTest, createJobWithCopyCommands) {
    int cmdCount = 3;

    void *destPtr = ctx->createSharedMemAlloc(allocSize);
    void *srcPtr = ctx->createHostMemAlloc(allocSize);

    auto job = std::make_unique<VPUJob>(ctx);
    for (int i = 0; i < cmdCount; i++)
        EXPECT_TRUE(job->appendCommand(VPUCopyCommand::create(ctx,
                                                              srcPtr,
                                                              ctx->findBufferObject(srcPtr),
                                                              destPtr,
                                                              ctx->findBufferObject(destPtr),
                                                              4096)));
    EXPECT_TRUE(job->closeCommands());

    EXPECT_EQ(1u, job->getCommandBuffers().size());
    for (const auto &cmdBuffer : job->getCommandBuffers()) {
        EXPECT_EQ(getExpBufferCount(cmdBuffer->getBufferHandles()),
                  cmdBuffer->getBufferHandles().size());
    }

    EXPECT_TRUE(ctx->freeMemAlloc(srcPtr));
    EXPECT_TRUE(ctx->freeMemAlloc(destPtr));
}

TEST_F(VPUJobTest, createJobWithDifferentTypesOfCommandExpectSuccess) {
    VPUBufferObject *event =
        ctx->createInternalBufferObject(sizeof(VPUEventCommand::KMDEventDataType),
                                        VPU::VPUBufferObject::Type::CachedFw);
    ASSERT_TRUE(event);
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(ctx->createSharedMemAlloc(sizeof(uint64_t)));
    ASSERT_NE(tsHeap, nullptr);

    void *shareMem = ctx->createSharedMemAlloc(allocSize);
    void *hostMem = ctx->createHostMemAlloc(allocSize);

    auto job = std::make_unique<VPUJob>(ctx);
    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));
    EXPECT_TRUE(job->appendCommand(VPUCopyCommand::create(ctx,
                                                          shareMem,
                                                          ctx->findBufferObject(shareMem),
                                                          shareMem,
                                                          ctx->findBufferObject(shareMem),
                                                          allocSize)));

    // Add internal events because of engine switch
    VPUEventCommand::KMDEventDataType *eventPtr =
        reinterpret_cast<decltype(eventPtr)>(event->getBasePointer());

    // VPUEventWaitCommand is forward type
    EXPECT_TRUE(job->appendCommand(VPUEventWaitCommand::create(ctx, eventPtr)));
    EXPECT_TRUE(job->appendCommand(VPUCopyCommand::create(ctx,
                                                          hostMem,
                                                          ctx->findBufferObject(hostMem),
                                                          shareMem,
                                                          ctx->findBufferObject(shareMem),
                                                          allocSize)));
    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));

    // Add internal events because of engine switch
    EXPECT_TRUE(job->appendCommand(VPUCopyCommand::create(ctx,
                                                          shareMem,
                                                          ctx->findBufferObject(shareMem),
                                                          shareMem,
                                                          ctx->findBufferObject(shareMem),
                                                          allocSize)));
    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));

    EXPECT_TRUE(job->closeCommands());

    /* Expected Copy command conversion to Compute compatible and only one CommandBuf */
    EXPECT_EQ(1u, job->getCommandBuffers().size());
    for (size_t i = 0; i < job->getCommandBuffers().size(); i++) {
        const auto &cmdBuffer = job->getCommandBuffers()[i];
        EXPECT_EQ(getExpBufferCount(cmdBuffer->getBufferHandles()),
                  cmdBuffer->getBufferHandles().size());
    }

    EXPECT_TRUE(ctx->freeMemAlloc(hostMem));
    EXPECT_TRUE(ctx->freeMemAlloc(shareMem));
    EXPECT_TRUE(ctx->freeMemAlloc(tsHeap));
    EXPECT_TRUE(ctx->freeMemAlloc(event));
}

TEST_F(VPUJobTest, checkJobStatus) {
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(ctx->createSharedMemAlloc(sizeof(uint64_t)));

    auto job = std::make_unique<VPUJob>(ctx);
    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));
    EXPECT_TRUE(job->closeCommands());

    osInfc.mockFailNextJobWait();
    EXPECT_EQ(false, job->waitForCompletion(0));
    EXPECT_EQ(false, job->isSuccess());

    osInfc.mockFailNextJobStatus();
    EXPECT_EQ(true, job->waitForCompletion(0));
    EXPECT_EQ(false, job->isSuccess());

    EXPECT_EQ(true, job->waitForCompletion(0));
    EXPECT_EQ(true, job->isSuccess());

    EXPECT_TRUE(ctx->freeMemAlloc(tsHeap));
}
