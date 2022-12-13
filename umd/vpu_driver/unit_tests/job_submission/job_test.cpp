/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"

#include "vpu_driver/source/command/vpu_command_buffer.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/command/vpu_barrier_command.hpp"
#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"
#include "vpu_driver/source/command/vpu_graph_exe_command.hpp"
#include "vpu_driver/source/command/vpu_graph_init_command.hpp"
#include "vpu_driver/source/command/vpu_ts_command.hpp"

#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include "gtest/gtest.h"

#include <boost/numeric/conversion/cast.hpp>
#include <memory>

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
    std::unique_ptr<VPUDeviceContext> deviceContext = vpuDevice->createDeviceContext();
    VPUDeviceContext *ctx = deviceContext.get();

    const uint32_t allocSize = 4 * 1024;
};

TEST_F(VPUJobTest, closeCommandsOnJobWithoutCommandsReturnTrue) {
    auto job = std::make_unique<VPUJob>(ctx, false);
    EXPECT_TRUE(job->closeCommands());
}

TEST_F(VPUJobTest, afterCloseCommandsCallAppendCommandReturnFalse) {
    auto job = std::make_unique<VPUJob>(ctx, false);
    EXPECT_TRUE(job->closeCommands());

    void *mem = ctx->createSharedMemAlloc(sizeof(uint64_t));
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(mem);

    EXPECT_FALSE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));
    EXPECT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(VPUJobTest, createJobWithTimestampAndDoNotCloseItExpectNoCommandBuffers) {
    auto job = std::make_unique<VPUJob>(ctx, false);

    void *mem = ctx->createSharedMemAlloc(sizeof(uint64_t));
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(mem);

    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));
    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));
    EXPECT_EQ(0u, job->getCommandBuffers().size());

    EXPECT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(VPUJobTest,
       createJobWithOnlyCopyFlagAndTimestampCommandsAndDoNotCloseItExpectNoCommandBuffers) {
    auto job = std::make_unique<VPUJob>(ctx, true);

    void *mem = ctx->createSharedMemAlloc(sizeof(uint64_t));
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(mem);

    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));
    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));
    EXPECT_EQ(0u, job->getCommandBuffers().size());

    EXPECT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(VPUJobTest, createJobWithOnlyCopyFlagAndComputeCommandExpectAppendCommandFailure) {
    auto job = std::make_unique<VPUJob>(ctx, true);

    void *mem = ctx->createSharedMemAlloc(sizeof(uint64_t));
    EXPECT_FALSE(job->appendCommand(VPUCopyCommand::create(ctx, mem, mem, sizeof(uint64_t))));

    EXPECT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(VPUJobTest, createJobWithTimestampCommandsForComputeEngine) {
    const int cmdCount = 4;

    void *mem = ctx->createSharedMemAlloc(sizeof(uint64_t) * cmdCount);
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(mem);

    auto job = std::make_unique<VPUJob>(ctx, false);
    for (int i = 0; i < cmdCount; i++)
        EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap++)));
    EXPECT_TRUE(job->closeCommands());

    EXPECT_EQ(1u, job->getCommandBuffers().size());
    for (const auto &cmdBuffer : job->getCommandBuffers()) {
        EXPECT_EQ(boost::numeric_cast<uint32_t>(DRM_IVPU_ENGINE_COMPUTE), cmdBuffer->getEngine());
        EXPECT_EQ(getExpBufferCount(cmdBuffer->getBufferHandles()),
                  cmdBuffer->getBufferHandles().size());
    }

    EXPECT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(VPUJobTest, createJobWithTimestampCommandsForCopyEngine) {
    const int cmdCount = 3;

    void *mem = ctx->createSharedMemAlloc(sizeof(uint64_t) * cmdCount);
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(mem);

    auto job = std::make_unique<VPUJob>(ctx, true);
    for (int i = 0; i < cmdCount; i++)
        EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap++)));
    EXPECT_TRUE(job->closeCommands());

    EXPECT_EQ(1u, job->getCommandBuffers().size());
    for (const auto &cmdBuffer : job->getCommandBuffers()) {
        EXPECT_EQ(boost::numeric_cast<uint32_t>(DRM_IVPU_ENGINE_COPY), cmdBuffer->getEngine());
        EXPECT_EQ(getExpBufferCount(cmdBuffer->getBufferHandles()),
                  cmdBuffer->getBufferHandles().size());
    }

    EXPECT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(VPUJobTest, createJobWithGraphInitAndExecuteCommands) {
    uint64_t umdBlobId = 0xcafebebedeadbeef;

    const size_t blobSize = 4 * 1024;
    uint8_t blobData[blobSize] = {};
    const uint32_t scratchSize = 4 * 1024;

    void *inputData = ctx->createSharedMemAlloc(allocSize);
    void *outputData = ctx->createSharedMemAlloc(allocSize);

    auto job = std::make_unique<VPUJob>(ctx, false);
    EXPECT_TRUE(job->appendCommand(
        VPUGraphInitCommand::create(ctx, umdBlobId, blobData, blobSize, scratchSize, scratchSize)));

    auto &graphInitBufferObjects = job->getNNCommands().back()->getAssociateBufferObjects();
    EXPECT_TRUE(job->appendCommand(VPUGraphExecuteCommand::create(
        ctx,
        umdBlobId,
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(inputData, allocSize)}},
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(outputData, allocSize)}},
        graphInitBufferObjects)));
    EXPECT_TRUE(job->closeCommands());

    EXPECT_EQ(1u, job->getCommandBuffers().size());
    for (const auto &cmdBuffer : job->getCommandBuffers()) {
        EXPECT_EQ(boost::numeric_cast<uint32_t>(DRM_IVPU_ENGINE_COMPUTE), cmdBuffer->getEngine());
        EXPECT_EQ(getExpBufferCount(cmdBuffer->getBufferHandles()),
                  cmdBuffer->getBufferHandles().size());
    }

    EXPECT_TRUE(ctx->freeMemAlloc(inputData));
    EXPECT_TRUE(ctx->freeMemAlloc(outputData));
}

TEST_F(VPUJobTest, createJobWithCopyCommandsforCopyEngine) {
    int cmdCount = 3;

    void *destPtr = ctx->createSharedMemAlloc(allocSize);
    void *srcPtr = ctx->createHostMemAlloc(allocSize);

    auto job = std::make_unique<VPUJob>(ctx, true);
    for (int i = 0; i < cmdCount; i++)
        EXPECT_TRUE(job->appendCommand(VPUCopyCommand::create(ctx, srcPtr, destPtr, 4096)));
    EXPECT_TRUE(job->closeCommands());

    EXPECT_EQ(1u, job->getCommandBuffers().size());
    for (const auto &cmdBuffer : job->getCommandBuffers()) {
        EXPECT_EQ(boost::numeric_cast<uint32_t>(DRM_IVPU_ENGINE_COPY), cmdBuffer->getEngine());
        EXPECT_EQ(getExpBufferCount(cmdBuffer->getBufferHandles()),
                  cmdBuffer->getBufferHandles().size());
    }

    EXPECT_TRUE(ctx->freeMemAlloc(srcPtr));
    EXPECT_TRUE(ctx->freeMemAlloc(destPtr));
}

TEST_F(VPUJobTest, createJobWithDifferentTypesOfCommandExpectSuccess) {
    VPUBufferObject *event =
        ctx->createInternalBufferObject(sizeof(VPUEventCommand::KMDEventDataType),
                                        VPU::VPUBufferObject::Type::CachedLow);
    ASSERT_NE(event, nullptr);
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(ctx->createSharedMemAlloc(sizeof(uint64_t)));
    ASSERT_NE(tsHeap, nullptr);

    void *shareMem = ctx->createSharedMemAlloc(allocSize);
    void *hostMem = ctx->createHostMemAlloc(allocSize);

    auto job = std::make_unique<VPUJob>(ctx, false);
    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));
    EXPECT_TRUE(job->appendCommand(VPUCopyCommand::create(ctx, shareMem, shareMem, allocSize)));

    // Add internal events because of engine switch
    VPUEventCommand::KMDEventDataType *eventPtr =
        reinterpret_cast<decltype(eventPtr)>(event->getBasePointer());

    // VPUEventWaitCommand is forward type
    EXPECT_TRUE(job->appendCommand(VPUEventWaitCommand::create(ctx, eventPtr)));
    EXPECT_TRUE(job->appendCommand(VPUCopyCommand::create(ctx, hostMem, shareMem, allocSize)));
    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));

    // Add internal events because of engine switch
    EXPECT_TRUE(job->appendCommand(VPUCopyCommand::create(ctx, shareMem, shareMem, allocSize)));
    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx, tsHeap)));

    EXPECT_TRUE(job->closeCommands());

    EXPECT_EQ(2u, job->getCommandBuffers().size());
    for (size_t i = 0; i < job->getCommandBuffers().size(); i++) {
        const auto &cmdBuffer = job->getCommandBuffers()[i];
        EXPECT_EQ(getExpBufferCount(cmdBuffer->getBufferHandles()),
                  cmdBuffer->getBufferHandles().size());
        if (i == 0)
            EXPECT_EQ(boost::numeric_cast<uint32_t>(DRM_IVPU_ENGINE_COMPUTE),
                      cmdBuffer->getEngine());
        else
            EXPECT_EQ(boost::numeric_cast<uint32_t>(DRM_IVPU_ENGINE_COPY), cmdBuffer->getEngine());
    }

    EXPECT_TRUE(ctx->freeMemAlloc(hostMem));
    EXPECT_TRUE(ctx->freeMemAlloc(shareMem));
    EXPECT_TRUE(ctx->freeMemAlloc(tsHeap));
    EXPECT_TRUE(ctx->freeMemAlloc(event));
}

TEST_F(VPUJobTest, checkJobStatusWhenOneEngineIsUsed) {
    uint64_t *tsHeap = reinterpret_cast<uint64_t *>(ctx->createSharedMemAlloc(sizeof(uint64_t)));

    auto job = std::make_unique<VPUJob>(ctx, false);
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

TEST_F(VPUJobTest, checkJobStatusWhenTwoEnginesAreUsed) {
    void *devMem = ctx->createHostMemAlloc(sizeof(uint64_t));
    void *hostMem = ctx->createSharedMemAlloc(sizeof(uint64_t));

    // There are two command buffers in VPUJob - for copy and compute engine
    auto job = std::make_unique<VPUJob>(ctx, false);
    EXPECT_TRUE(
        job->appendCommand(VPUCopyCommand::create(ctx, hostMem, hostMem, sizeof(uint64_t))));
    EXPECT_TRUE(job->appendCommand(VPUCopyCommand::create(ctx, devMem, devMem, sizeof(uint64_t))));
    EXPECT_TRUE(job->closeCommands());

    // First job wait fails with timeout
    osInfc.mockFailNextJobWait();
    EXPECT_EQ(false, job->waitForCompletion(0));
    EXPECT_EQ(false, job->isSuccess());

    // Second job wait fails with timeout
    osInfc.mockSuccessNextJobWait();
    osInfc.mockFailNextJobWait();
    EXPECT_EQ(false, job->waitForCompletion(0));
    EXPECT_EQ(false, job->isSuccess());

    // First job status fails
    osInfc.mockFailNextJobStatus();
    EXPECT_EQ(true, job->waitForCompletion(0));
    EXPECT_EQ(false, job->isSuccess());

    // Second job status fails
    osInfc.mockSuccessNextJobStatus();
    osInfc.mockFailNextJobStatus();
    EXPECT_EQ(true, job->waitForCompletion(0));
    EXPECT_EQ(false, job->isSuccess());

    // Jobs are successfull
    EXPECT_EQ(true, job->waitForCompletion(0));
    EXPECT_EQ(true, job->isSuccess());

    EXPECT_TRUE(ctx->freeMemAlloc(hostMem));
    EXPECT_TRUE(ctx->freeMemAlloc(devMem));
}
