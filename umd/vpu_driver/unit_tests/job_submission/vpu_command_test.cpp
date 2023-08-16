/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/command/vpu_ts_command.hpp"
#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/command/vpu_graph_init_command.hpp"
#include "vpu_driver/source/command/vpu_graph_exe_command.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"
#include "vpu_driver/source/command/vpu_barrier_command.hpp"
#include "vpu_driver/source/command/vpu_query_command.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include "gtest/gtest.h"

using namespace VPU;

struct VPUCommandTest : public ::testing::Test {
    void TearDown() override {
        if (ctx) {
            ASSERT_EQ(ctx->getBuffersCount(), 0u);
        }
    }

    MockOsInterfaceImp osInfc;
    std::unique_ptr<MockVPUDevice> vpuDevice = MockVPUDevice::createWithDefaultHardwareInfo(osInfc);
    std::unique_ptr<VPUDeviceContext> deviceContext = vpuDevice->createDeviceContext();
    VPUDeviceContext *ctx = deviceContext.get();
};

TEST_F(VPUCommandTest, timestampCommandShouldReturnExpectedProperties) {
    void *mem = ctx->createSharedMemAlloc(sizeof(uint64_t));

    // Time stamp command.
    VPUTimeStampCommand tsCmd(ctx, static_cast<uint64_t *>(mem));
    EXPECT_EQ(VPU_CMD_TIMESTAMP, tsCmd.getCommandType());
    EXPECT_EQ(sizeof(vpu_cmd_timestamp_t), tsCmd.getCommitSize());
    EXPECT_TRUE(tsCmd.isForwardCommand());

    // Compare associated pointers
    VPUBufferObject *bo = ctx->findBuffer(mem);
    ASSERT_NE(bo, nullptr);
    EXPECT_EQ(tsCmd.getAssociateBufferObjects().size(), 1u);
    EXPECT_EQ(tsCmd.getAssociateBufferObjects().at(0), bo);

    // Compare command stream return value in byte wise.
    vpu_cmd_timestamp_t expKMDTsCmd = {};
    expKMDTsCmd.header = {VPU_CMD_TIMESTAMP, sizeof(vpu_cmd_timestamp_t)};
    expKMDTsCmd.timestamp_address = ctx->getBufferVPUAddress(mem);

    EXPECT_EQ(memcmp(&expKMDTsCmd, tsCmd.getCommitStream(), tsCmd.getCommitSize()), 0);

    EXPECT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(VPUCommandTest, copyCommandShouldReturnExpectedProperties) {
    void *srcPtr = ctx->createSharedMemAlloc(sizeof(uint64_t));
    void *dstPtr = ctx->createSharedMemAlloc(sizeof(uint64_t));

    // Copy command.
    std::shared_ptr<VPUCommand> copyCmd =
        VPUCopyCommand::create(ctx, srcPtr, dstPtr, sizeof(uint64_t));
    ASSERT_NE(copyCmd, nullptr);

    EXPECT_EQ(VPU_CMD_COPY_LOCAL_TO_LOCAL, copyCmd->getCommandType());
    EXPECT_EQ(sizeof(vpu_cmd_copy_buffer_t), copyCmd->getCommitSize());
    EXPECT_TRUE(copyCmd->isBackwardCommand());

    // Compare buffer handle
    auto copyCmdAssocVec = copyCmd->getAssociateBufferObjects();
    ASSERT_EQ(copyCmdAssocVec.size(), 2u);
    VPUBufferObject *bo = ctx->findBuffer(srcPtr);
    ASSERT_NE(bo, nullptr);
    EXPECT_EQ(copyCmdAssocVec.at(0), bo);
    bo = ctx->findBuffer(dstPtr);
    ASSERT_NE(bo, nullptr);
    EXPECT_EQ(copyCmdAssocVec.at(1), bo);

    // Compare command stream return value in byte wise.
    vpu_cmd_copy_buffer_t
        expKMDCopyCmd{{VPU_CMD_COPY_LOCAL_TO_LOCAL, sizeof(vpu_cmd_copy_buffer_t)}, 0u, 0u, 1u, 0u};
    uint8_t *exp = reinterpret_cast<uint8_t *>(&expKMDCopyCmd);

    EXPECT_EQ(memcmp(exp, copyCmd->getCommitStream(), copyCmd->getCommitSize()), 0);

    EXPECT_TRUE(ctx->freeMemAlloc(srcPtr));
    EXPECT_TRUE(ctx->freeMemAlloc(dstPtr));
}

TEST_F(VPUCommandTest, graphInitCommandWithoutGraphShouldReturnExpectedProperties) {
    const size_t blobSize = 4 * 1024;
    const size_t bufferCount = 4;
    uint8_t blobData[blobSize] = {};

    std::vector<uint32_t> numArgsVec{1u, 2u};

    uint64_t blobId = 0u;
    // Graph Init command.
    std::shared_ptr<VPUCommand> graphInitCmd =
        VPUGraphInitCommand::create(ctx, blobId, blobData, blobSize, blobSize, blobSize);
    ASSERT_NE(graphInitCmd, nullptr);

    EXPECT_EQ(VPU_CMD_OV_BLOB_INITIALIZE, graphInitCmd->getCommandType());
    EXPECT_EQ(sizeof(vpu_cmd_ov_blob_initialize_t), graphInitCmd->getCommitSize());
    EXPECT_TRUE(graphInitCmd->isComputeCommand());

    // Compare command stream return value in byte wise.
    vpu_cmd_ov_blob_initialize_t expKMDGraphInitCmd{
        {VPU_CMD_OV_BLOB_INITIALIZE, sizeof(vpu_cmd_ov_blob_initialize_t)},
        blobSize,
        0ul, // kernel_offset
        boost::numeric_cast<uint32_t>(2 * sizeof(vpu_cmd_resource_descriptor_table_t) +
                                      2 * sizeof(vpu_cmd_resource_descriptor_t) * bufferCount),
        0u,  // reserved_0
        0ul, // desc_table_offset
        blobId};
    uint8_t *exp = reinterpret_cast<uint8_t *>(&expKMDGraphInitCmd);

    EXPECT_EQ(memcmp(exp, graphInitCmd->getCommitStream(), graphInitCmd->getCommitSize()), 0);
}

TEST_F(VPUCommandTest, graphExecuteCommandShouldReturnExpectedProperties) {
    // Graph blob exec command.
    uint64_t blobId = 0u;

    uint32_t memSize = 0x04;
    void *inputBuffer = ctx->createHostMemAlloc(memSize);
    void *outputBuffer = ctx->createHostMemAlloc(memSize);
    std::shared_ptr<VPUCommand> graphExecCmd = VPUGraphExecuteCommand::create(
        ctx,
        blobId,
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(inputBuffer, memSize)}},
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(outputBuffer, memSize)}},
        {});
    ASSERT_NE(graphExecCmd, nullptr);

    EXPECT_EQ(VPU_CMD_OV_BLOB_EXECUTE, graphExecCmd->getCommandType());
    EXPECT_EQ(sizeof(vpu_cmd_ov_blob_execute_t), graphExecCmd->getCommitSize());
    EXPECT_TRUE(graphExecCmd->isComputeCommand());

    // Expected byte stream for the command with settings above.
    vpu_cmd_ov_blob_execute_t expKMDgraphExecCmd{
        {VPU_CMD_OV_BLOB_EXECUTE, sizeof(vpu_cmd_ov_blob_execute_t)},
        boost::numeric_cast<uint32_t>(2 * sizeof(vpu_cmd_resource_descriptor_table_t) +
                                      2 * sizeof(vpu_cmd_resource_descriptor_t)),
        0,
        blobId};
    uint8_t *exp = reinterpret_cast<uint8_t *>(&expKMDgraphExecCmd);
    EXPECT_EQ(memcmp(exp, graphExecCmd->getCommitStream(), graphExecCmd->getCommitSize()), 0);
    EXPECT_TRUE(ctx->freeMemAlloc(inputBuffer));
    EXPECT_TRUE(ctx->freeMemAlloc(outputBuffer));
}

TEST_F(VPUCommandTest, graphExecuteCommandWithProfilingOutput) {
    // Graph blob exec command.
    uint64_t blobId = 0u;

    uint32_t memSize = 0x04;
    void *inputBuffer = ctx->createHostMemAlloc(memSize);
    void *outputBuffer = ctx->createHostMemAlloc(memSize);
    void *profilingOutputBuffer = ctx->createHostMemAlloc(memSize);
    size_t profilingOutputSize = 4 * 1024;

    std::shared_ptr<VPUCommand> graphExecCmd = VPUGraphExecuteCommand::create(
        ctx,
        blobId,
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(inputBuffer, memSize)}},
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(outputBuffer, memSize)}},
        {},
        profilingOutputSize,
        profilingOutputBuffer);
    ASSERT_NE(graphExecCmd, nullptr);

    EXPECT_EQ(VPU_CMD_OV_BLOB_EXECUTE, graphExecCmd->getCommandType());
    EXPECT_EQ(sizeof(vpu_cmd_ov_blob_execute_t), graphExecCmd->getCommitSize());
    EXPECT_TRUE(graphExecCmd->isComputeCommand());

    // Expected byte stream for the command with settings above.
    vpu_cmd_ov_blob_execute_t expKMDgraphExecCmd{
        {VPU_CMD_OV_BLOB_EXECUTE, sizeof(vpu_cmd_ov_blob_execute_t)},
        3 * sizeof(vpu_cmd_resource_descriptor_table_t) + 3 * sizeof(vpu_cmd_resource_descriptor_t),
        0,
        blobId};
    uint8_t *exp = reinterpret_cast<uint8_t *>(&expKMDgraphExecCmd);
    EXPECT_EQ(memcmp(exp, graphExecCmd->getCommitStream(), graphExecCmd->getCommitSize()), 0);
    EXPECT_TRUE(ctx->freeMemAlloc(inputBuffer));
    EXPECT_TRUE(ctx->freeMemAlloc(outputBuffer));
    EXPECT_TRUE(ctx->freeMemAlloc(profilingOutputBuffer));
}

TEST_F(VPUCommandTest, graphCommandsShouldPassContextIDInGraphBlobIDToKMD) {
    uint64_t umdBlobId = 0xdeadbeef00000001;
    uint8_t mem[128] = {};
    uint32_t memSize = sizeof(uint8_t) * 128;
    void *inputBuffer = ctx->createHostMemAlloc(memSize);
    void *outputBuffer = ctx->createHostMemAlloc(memSize);

    std::shared_ptr<VPUCommand> graphInitCmd =
        VPUGraphInitCommand::create(ctx, umdBlobId, mem, memSize, memSize, memSize);
    ASSERT_NE(graphInitCmd, nullptr);

    const uint8_t *initCmdStrm = graphInitCmd->getCommitStream();
    auto initStruct = reinterpret_cast<const vpu_cmd_ov_blob_initialize_t *>(initCmdStrm);
    EXPECT_EQ(umdBlobId, initStruct->blob_id);

    std::shared_ptr<VPUCommand> graphExeCmd = VPUGraphExecuteCommand::create(
        ctx,
        umdBlobId,
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(inputBuffer, memSize)}},
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(outputBuffer, memSize)}},
        {});
    ASSERT_NE(graphExeCmd, nullptr);

    const uint8_t *exeCmdStrm = graphExeCmd->getCommitStream();
    auto exeStruct = reinterpret_cast<const vpu_cmd_ov_blob_execute_t *>(exeCmdStrm);
    EXPECT_EQ(umdBlobId, exeStruct->blob_id);
    EXPECT_TRUE(ctx->freeMemAlloc(inputBuffer));
    EXPECT_TRUE(ctx->freeMemAlloc(outputBuffer));
}

TEST_F(VPUCommandTest, barrierCommandShouldReturnExpectedProperties) {
    std::shared_ptr<VPUCommand> barrierCmd = VPUBarrierCommand::create();
    ASSERT_NE(barrierCmd, nullptr);

    EXPECT_EQ(VPU_CMD_BARRIER, barrierCmd->getCommandType());
    EXPECT_EQ(sizeof(vpu_cmd_barrier_t), barrierCmd->getCommitSize());
    EXPECT_TRUE(barrierCmd->isBackwardCommand());

    // Compare command stream return value in byte wise.
    vpu_cmd_barrier_t expKMDBarrierCmd{{VPU_CMD_BARRIER, sizeof(vpu_cmd_barrier_t)}, 0};
    uint8_t *exp = reinterpret_cast<uint8_t *>(&expKMDBarrierCmd);

    EXPECT_EQ(memcmp(exp, barrierCmd->getCommitStream(), barrierCmd->getCommitSize()), 0);
}

TEST_F(VPUCommandTest, queryBeginShouldReturnExpectedProperties) {
    void *mem = ctx->createSharedMemAlloc(sizeof(uint64_t));

    std::shared_ptr<VPUCommand> queryBeginCmd = VPUQueryBeginCommand::create(ctx, 0u, mem);
    ASSERT_NE(queryBeginCmd, nullptr);
    EXPECT_EQ(VPU_CMD_METRIC_QUERY_BEGIN, queryBeginCmd->getCommandType());
    EXPECT_EQ(sizeof(vpu_cmd_metric_query_t), queryBeginCmd->getCommitSize());
    EXPECT_TRUE(queryBeginCmd->isForwardCommand());

    // Compare command stream return value in byte wise.
    vpu_cmd_metric_query_t expKMDQBeginCmd{
        {VPU_CMD_METRIC_QUERY_BEGIN, sizeof(vpu_cmd_metric_query_t)},
        0u,
        ctx->getBufferVPUAddress(mem)};
    uint8_t *exp = reinterpret_cast<uint8_t *>(&expKMDQBeginCmd);

    EXPECT_EQ(memcmp(exp, queryBeginCmd->getCommitStream(), queryBeginCmd->getCommitSize()), 0);
    EXPECT_TRUE(ctx->freeMemAlloc(mem));
}

TEST_F(VPUCommandTest, queryEndShouldReturnExpectedProperties) {
    void *mem = ctx->createSharedMemAlloc(sizeof(uint64_t));

    std::shared_ptr<VPUCommand> queryEndCmd = VPUQueryEndCommand::create(ctx, 0u, mem);
    ASSERT_NE(queryEndCmd, nullptr);

    EXPECT_EQ(VPU_CMD_METRIC_QUERY_END, queryEndCmd->getCommandType());
    EXPECT_EQ(sizeof(vpu_cmd_metric_query_t), queryEndCmd->getCommitSize());
    EXPECT_TRUE(queryEndCmd->isBackwardCommand());

    // Compare command stream return value in byte wise.
    vpu_cmd_metric_query_t expKMDQEndCmd{{VPU_CMD_METRIC_QUERY_END, sizeof(vpu_cmd_metric_query_t)},
                                         0u,
                                         ctx->getBufferVPUAddress(mem)};
    uint8_t *exp = reinterpret_cast<uint8_t *>(&expKMDQEndCmd);

    EXPECT_EQ(memcmp(exp, queryEndCmd->getCommitStream(), queryEndCmd->getCommitSize()), 0);
    EXPECT_TRUE(ctx->freeMemAlloc(mem));
}

struct VPUEventCommandTest : public VPUCommandTest {
    void SetUp() {
        VPUCommandTest::SetUp();

        eventBuffer = ctx->createInternalBufferObject(4096, VPUBufferObject::Type::CachedLow);
        ASSERT_TRUE(eventBuffer);

        cmdBufferHeader.fence_heap_base_address = ctx->getVPULowBaseAddress();
        eventHeapPtr = reinterpret_cast<decltype(eventHeapPtr)>(eventBuffer->getBasePointer());
    }

    void TearDown() {
        if (ctx && eventBuffer) {
            EXPECT_TRUE(ctx->freeMemAlloc(eventBuffer));
        }

        VPUCommandTest::TearDown();
    }

    VPUBufferObject *eventBuffer = nullptr;
    VPUEventCommand::KMDEventDataType *eventHeapPtr = nullptr;
    vpu_cmd_buffer_header_t cmdBufferHeader = {};
};

TEST_F(VPUEventCommandTest, eventWaitCommandsShouldReturnExpectedProperties) {
    // 0 offset, Event wait command.
    std::shared_ptr<VPUCommand> waitCmd = VPUEventWaitCommand::create(ctx, eventHeapPtr);
    ASSERT_NE(waitCmd, nullptr);

    // Check expected command contents.
    EXPECT_EQ(VPU_CMD_FENCE_WAIT, waitCmd->getCommandType());
    EXPECT_EQ(sizeof(vpu_cmd_fence_t), waitCmd->getCommitSize());
    EXPECT_TRUE(waitCmd->isForwardCommand());

    const vpu_cmd_fence_t *actual =
        reinterpret_cast<const vpu_cmd_fence_t *>(waitCmd->getCommitStream());
    EXPECT_EQ(VPU_CMD_FENCE_WAIT, actual->header.type);
    EXPECT_EQ(sizeof(vpu_cmd_fence_t), actual->header.size);
    EXPECT_EQ(0u, actual->offset);
    EXPECT_EQ(VPUEventCommand::STATE_DEVICE_SIGNAL, actual->value);

    // 64bits offsetted event wait command.
    VPUEventCommand::KMDEventDataType *offsetEventHeapPtr = eventHeapPtr + 1;

    std::shared_ptr<VPUCommand> waitCmd2 = VPUEventWaitCommand::create(ctx, offsetEventHeapPtr);
    ASSERT_NE(waitCmd2, nullptr);

    actual = reinterpret_cast<const vpu_cmd_fence_t *>(waitCmd2->getCommitStream());
    EXPECT_EQ(VPU_CMD_FENCE_WAIT, actual->header.type);
    EXPECT_EQ(sizeof(vpu_cmd_fence_t), actual->header.size);
    EXPECT_EQ(8u, actual->offset);
    EXPECT_EQ(VPUEventCommand::STATE_DEVICE_SIGNAL, actual->value);
}

TEST_F(VPUEventCommandTest, eventSignalCommandsShouldReturnExpectedProperties) {
    std::shared_ptr<VPUCommand> signalCmd = VPUEventSignalCommand::create(ctx, eventHeapPtr);
    ASSERT_NE(signalCmd, nullptr);

    // Check expected command contents.
    EXPECT_EQ(VPU_CMD_FENCE_SIGNAL, signalCmd->getCommandType());
    EXPECT_EQ(sizeof(vpu_cmd_fence_t), signalCmd->getCommitSize());
    EXPECT_TRUE(signalCmd->isSynchronizeCommand());

    // Compare command stream return value in byte wise.
    vpu_cmd_fence_t expKMDSignalCmd = {};
    expKMDSignalCmd.header = {VPU_CMD_FENCE_SIGNAL, sizeof(vpu_cmd_fence_t)};
    expKMDSignalCmd.offset = 0;
    expKMDSignalCmd.value = VPUEventCommand::STATE_DEVICE_SIGNAL;

    EXPECT_EQ(memcmp(&expKMDSignalCmd, signalCmd->getCommitStream(), signalCmd->getCommitSize()),
              0);

    // 64bits offsetted event signal command.
    VPUEventCommand::KMDEventDataType *offsetEventHeapPtr = eventHeapPtr + 1;
    std::shared_ptr<VPUCommand> signalCmd2 = VPUEventSignalCommand::create(ctx, offsetEventHeapPtr);
    ASSERT_NE(signalCmd2, nullptr);

    expKMDSignalCmd.offset = 8;
    EXPECT_EQ(memcmp(&expKMDSignalCmd, signalCmd2->getCommitStream(), signalCmd2->getCommitSize()),
              0);
}

TEST_F(VPUEventCommandTest, eventResetCommandsShouldReturnExpectedProperties) {
    std::shared_ptr<VPUCommand> resetCmd = VPUEventResetCommand::create(ctx, eventHeapPtr);
    ASSERT_NE(resetCmd, nullptr);

    // Check expected command contents.
    EXPECT_EQ(VPU_CMD_FENCE_SIGNAL, resetCmd->getCommandType());
    EXPECT_EQ(sizeof(vpu_cmd_fence_t), resetCmd->getCommitSize());
    EXPECT_TRUE(resetCmd->isBackwardCommand());

    // Compare command stream return value in byte wise.
    vpu_cmd_fence_t expKMDResetCmd = {};
    expKMDResetCmd.header = {VPU_CMD_FENCE_SIGNAL, sizeof(vpu_cmd_fence_t)};
    expKMDResetCmd.offset = 0;
    expKMDResetCmd.value = VPUEventCommand::State::STATE_DEVICE_RESET;

    EXPECT_EQ(memcmp(&expKMDResetCmd, resetCmd->getCommitStream(), resetCmd->getCommitSize()), 0);
}
