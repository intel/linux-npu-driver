/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include "vpu_driver/source/command/vpu_ts_command.hpp"
#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/command/vpu_graph_init_command.hpp"
#include "vpu_driver/source/command/vpu_graph_exe_command.hpp"

#include "gtest/gtest.h"

#include <chrono>
#include <cstdint>
#include <memory>

using namespace VPU;

struct DeviceContextTest : public ::testing::Test {
    void SetUp() override {
        // Make sure no other tracking buffers exist in MM.
        ASSERT_EQ(ctx->getBuffersCount(), 0u);

        auto baseAddress = ctx->getVPULowBaseAddress();
        cmdBufferHeader.kernel_heap_base_address = baseAddress;
        cmdBufferHeader.descriptor_heap_base_address = baseAddress;
    }

    void TearDown() override {
        // No tracking memory should be left.
        ASSERT_EQ(ctx->getBuffersCount(), 0u);
    }

    uint64_t getOffset(uint64_t ptr, uint64_t base) {
        EXPECT_GE(ptr, base);
        return ptr - base;
    }

    void checkOffsets(std::vector<std::shared_ptr<VPUCommand>> &commands,
                      VPUBufferObject *descBuffer) {
        void *descTail = nullptr;
        size_t expDescOffset = 0;
        if (descBuffer) {
            descTail = descBuffer->getBasePointer();
            expDescOffset = getOffset(ctx->getBufferVPUAddress(descTail),
                                      cmdBufferHeader.descriptor_heap_base_address);
        }

        for (auto &cmd : commands) {
            EXPECT_TRUE(cmd->copyDescriptor(ctx, &descTail));

            uint64_t offset = UINT64_MAX;
            uint64_t vpuAddr = UINT64_MAX;
            switch (cmd->getCommandType()) {
            case VPU_CMD_TIMESTAMP:
                vpuAddr = reinterpret_cast<const vpu_cmd_timestamp_t *>(cmd->getCommitStream())
                              ->timestamp_address;
                EXPECT_GT(vpuAddr, 0) << "Invalid timestamp address in VPU_CMD_TIMESTAMP";
                break;
            case VPU_CMD_COPY_SYSTEM_TO_LOCAL:
            case VPU_CMD_COPY_LOCAL_TO_SYSTEM:
            case VPU_CMD_COPY_SYSTEM_TO_SYSTEM:
            case VPU_CMD_COPY_LOCAL_TO_LOCAL:
                offset = reinterpret_cast<const vpu_cmd_copy_buffer_t *>(cmd->getCommitStream())
                             ->desc_start_offset;
                EXPECT_EQ(offset, expDescOffset) << "Invalid descriptor offset in VPU_CMD_COPY_*";
                break;
            case VPU_CMD_OV_BLOB_INITIALIZE:
                offset =
                    reinterpret_cast<const vpu_cmd_ov_blob_initialize_t *>(cmd->getCommitStream())
                        ->kernel_offset;
                EXPECT_GT(offset, 0) << "Invalid kernel offset in VPU_CMD_OV_BLOB_INITIALIZE";
                offset =
                    reinterpret_cast<const vpu_cmd_ov_blob_initialize_t *>(cmd->getCommitStream())
                        ->desc_table_offset;
                EXPECT_EQ(offset, expDescOffset)
                    << "Invalid descriptor offset in VPU_CMD_OV_BLOB_INITIALIZE";
                break;
            case VPU_CMD_OV_BLOB_EXECUTE:
                offset = reinterpret_cast<const vpu_cmd_ov_blob_execute_t *>(cmd->getCommitStream())
                             ->desc_table_offset;
                EXPECT_EQ(offset, expDescOffset)
                    << "Invalid descriptor offset in VPU_CMD_OV_BLOB_EXECUTE";
                break;
            default:
                EXPECT_TRUE(false) << "Missing support for counting offset for specific command";
                break;
            }

            expDescOffset += getFwDataCacheAlign(cmd->getDescriptorSize());
        }
    }

    MockOsInterfaceImp osInfc;
    std::unique_ptr<MockVPUDevice> vpuDevice = MockVPUDevice::createWithDefaultHardwareInfo(osInfc);
    std::unique_ptr<VPUDeviceContext> deviceContext = vpuDevice->createDeviceContext();
    VPUDeviceContext *ctx = deviceContext.get();
    vpu_cmd_buffer_header_t cmdBufferHeader = {};

    const uint32_t allocSize = 4 * 1024;
};

TEST_F(DeviceContextTest, getVPULowBaseAddressExpectSuccess) {
    EXPECT_NE(0u, ctx->getVPULowBaseAddress());
}

TEST_F(DeviceContextTest, createAndFreeDeviceMemoryExpectSuccess) {
    auto ptr = ctx->createDeviceMemAlloc(allocSize);
    EXPECT_NE(nullptr, ptr);
    EXPECT_EQ(1u, ctx->getBuffersCount());
    EXPECT_TRUE(ctx->freeMemAlloc(ptr));
}

TEST_F(DeviceContextTest, createAndFreeHostMemoryExpectSuccess) {
    auto ptr = ctx->createHostMemAlloc(allocSize);
    EXPECT_NE(nullptr, ptr);
    EXPECT_EQ(1u, ctx->getBuffersCount());
    EXPECT_TRUE(ctx->freeMemAlloc(ptr));
}

TEST_F(DeviceContextTest, freeDeviceMemoryUsingNonBasePointerExpectFail) {
    auto ptr1 = ctx->createDeviceMemAlloc(allocSize);
    EXPECT_NE(nullptr, ptr1);
    EXPECT_EQ(1u, ctx->getBuffersCount());

    auto ptr_offset = (uint8_t *)ptr1 + 1000;
    EXPECT_FALSE(ctx->freeMemAlloc(ptr_offset));
    ptr_offset = (uint8_t *)ptr1 + (4 * 1024);
    EXPECT_FALSE(ctx->freeMemAlloc(ptr_offset));

    EXPECT_TRUE(ctx->freeMemAlloc(ptr1));
}

TEST_F(DeviceContextTest, createAndFreeDeviceMemoryInHighRangeExpectSuccess) {
    auto ptr = ctx->createSharedMemAlloc(allocSize, VPUBufferObject::Type::CachedHigh);
    EXPECT_NE(nullptr, ptr);
    EXPECT_EQ(1u, ctx->getBuffersCount());
    EXPECT_TRUE(ctx->freeMemAlloc(ptr));
}

TEST_F(DeviceContextTest, createAndFreeHostMemoryInHighRangeExpectSuccess) {
    auto ptr = ctx->createHostMemAlloc(allocSize, VPUBufferObject::Type::CachedHigh);
    EXPECT_NE(nullptr, ptr);
    EXPECT_EQ(1u, ctx->getBuffersCount());
    EXPECT_TRUE(ctx->freeMemAlloc(ptr));
}

TEST_F(DeviceContextTest, createHostMemoryAndGetVPUAddressExpectSuccess) {
    auto ptr = ctx->createHostMemAlloc(allocSize);
    EXPECT_NE(nullptr, ptr);

    auto vpuAddr = ctx->getBufferVPUAddress(ptr);
    EXPECT_NE(0u, vpuAddr);

    auto ptrOffset = reinterpret_cast<uint8_t *>(ptr) + allocSize / 4;
    EXPECT_EQ(vpuAddr + allocSize / 4, ctx->getBufferVPUAddress(ptrOffset));

    EXPECT_TRUE(ctx->freeMemAlloc(ptr));
}

TEST_F(DeviceContextTest, createDeviceMemoryAndGetVPUAddressExpectSuccess) {
    auto ptr = ctx->createDeviceMemAlloc(allocSize);
    EXPECT_NE(nullptr, ptr);

    auto vpuAddr = ctx->getBufferVPUAddress(ptr);
    EXPECT_NE(0u, vpuAddr);

    auto ptrOffset = reinterpret_cast<uint8_t *>(ptr) + allocSize / 4;
    EXPECT_EQ(vpuAddr + allocSize / 4, ctx->getBufferVPUAddress(ptrOffset));

    EXPECT_TRUE(ctx->freeMemAlloc(ptr));
}

TEST_F(DeviceContextTest, getVPUAddressUsingNotTrackedBufferExpectFailure) {
    EXPECT_EQ(0u, ctx->getBufferVPUAddress(nullptr));

    uint64_t var = 0u;
    EXPECT_EQ(0u, ctx->getBufferVPUAddress(&var));
}

TEST_F(DeviceContextTest, commandsInitializedProperlyShouldReturnProperIsComputeJobBoolean) {
    // Allocate Memory
    auto timestamp1 = ctx->createSharedMemAlloc(allocSize);
    auto sharedMemSrc = ctx->createSharedMemAlloc(allocSize);
    auto sharedMemDest = ctx->createSharedMemAlloc(allocSize);
    auto hostMemSrc = ctx->createHostMemAlloc(allocSize);
    auto hostMemDest = ctx->createHostMemAlloc(allocSize);

    // Timestamp (Compute Job)
    std::shared_ptr<VPUCommand> tsCmd1 =
        VPUTimeStampCommand::create(ctx, static_cast<uint64_t *>(timestamp1));
    ASSERT_NE(tsCmd1, nullptr);
    EXPECT_FALSE(tsCmd1->isComputeCommand());
    EXPECT_FALSE(tsCmd1->isCopyCommand());

    // L2L Copy (Compute Job)
    std::shared_ptr<VPUCommand> copyCmd1 =
        VPUCopyCommand::create(ctx, sharedMemSrc, sharedMemDest, allocSize);
    ASSERT_NE(copyCmd1, nullptr);
    EXPECT_TRUE(copyCmd1->isComputeCommand());

    // L2S Copy (Copy Job)
    std::shared_ptr<VPUCommand> copyCmd2 =
        VPUCopyCommand::create(ctx, sharedMemSrc, hostMemDest, allocSize);
    ASSERT_NE(copyCmd2, nullptr);
    EXPECT_FALSE(copyCmd2->isComputeCommand());

    // S2L Copy (Copy Job)
    std::shared_ptr<VPUCommand> copyCmd3 =
        VPUCopyCommand::create(ctx, hostMemSrc, sharedMemDest, allocSize);
    ASSERT_NE(copyCmd3, nullptr);
    EXPECT_FALSE(copyCmd3->isComputeCommand());

    // S2S Copy (Copy Job)
    std::shared_ptr<VPUCommand> copyCmd4 =
        VPUCopyCommand::create(ctx, hostMemSrc, hostMemDest, allocSize);
    ASSERT_NE(copyCmd4, nullptr);
    EXPECT_FALSE(copyCmd4->isComputeCommand());

    EXPECT_TRUE(ctx->freeMemAlloc(timestamp1));
    EXPECT_TRUE(ctx->freeMemAlloc(sharedMemSrc));
    EXPECT_TRUE(ctx->freeMemAlloc(hostMemSrc));
    EXPECT_TRUE(ctx->freeMemAlloc(sharedMemDest));
    EXPECT_TRUE(ctx->freeMemAlloc(hostMemDest));
}

TEST_F(DeviceContextTest,
       allocatingHeapAndUsingWithinHeapMemoryForTimestampCommandReturnsSuccessful) {
    // Allocate Memory of 4 * 64bit
    size_t tsAllocSize = 4 * sizeof(uint64_t);
    auto timestampPtr1 = ctx->createSharedMemAlloc(tsAllocSize);
    auto timestampPtr2 = ctx->createSharedMemAlloc(tsAllocSize);

    uint64_t *timestamp11 = reinterpret_cast<uint64_t *>(timestampPtr1);
    uint64_t *timestamp12 = timestamp11 + 1;
    uint64_t *timestamp13 = timestamp11 + 2;
    uint64_t *timestamp14 = timestamp11 + 3;

    uint64_t *timestamp21 = reinterpret_cast<uint64_t *>(timestampPtr2);
    uint64_t *timestamp22 = timestamp21 + 1;
    uint64_t *timestamp23 = timestamp21 + 2;

    // Creating and appending commands to commandlist
    std::vector<std::shared_ptr<VPUCommand>> commands;

    commands.emplace_back(VPUTimeStampCommand::create(ctx, timestamp11));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(ctx, timestamp12));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(ctx, timestamp13));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(ctx, timestamp14));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(ctx, timestamp22));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(ctx, timestamp23));
    ASSERT_NE(commands.back(), nullptr);

    EXPECT_EQ(commands.size(), 6u);
    checkOffsets(commands, nullptr);

    EXPECT_TRUE(ctx->freeMemAlloc(timestampPtr1));
    EXPECT_TRUE(ctx->freeMemAlloc(timestampPtr2));
}

TEST_F(DeviceContextTest,
       allocatingHeapAndUsingWithinHeapMemoryForGraphInitCommandReturnsSuccessful) {
    // Set blob size
    const size_t blobSize = 12 * 1024; // 12 KB

    // Allocate scratch and metadata heap
    const uint32_t scratchSize = 4 * 1024;          // 4 KB
    const uint32_t metadataSize = 16 * 1024 * 1024; // 16MB

    // Allocate Shared Memory
    auto inputData1 = ctx->createSharedMemAlloc(allocSize);
    auto outputData1 = ctx->createSharedMemAlloc(allocSize);
    auto inputData2 = ctx->createSharedMemAlloc(allocSize);
    auto outputData2 = ctx->createSharedMemAlloc(allocSize);

    // Allocate Host Memory
    uint8_t blobData1[blobSize];
    uint8_t blobData2[blobSize];

    // Copy raw data to allocated Host memory for graph blob
    memset(blobData1, 'A', blobSize);
    memset(blobData2, 'A', blobSize);

    // Append commands to commandlist vector
    std::vector<std::shared_ptr<VPUCommand>> commands;

    commands.emplace_back(VPUGraphInitCommand::create(ctx,
                                                      0, // BlobId : 0
                                                      blobData1,
                                                      blobSize,
                                                      scratchSize,
                                                      metadataSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUGraphExecuteCommand::create(
        ctx,
        0, // BlobId : 0
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(inputData1, allocSize)}},
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(outputData1, allocSize)}},
        commands.back()->getAssociateBufferObjects()));
    ASSERT_NE(commands.back(), nullptr);

    LOG_I("BlobData1: %p BlobData2: %p", blobData1, blobData2);

    commands.emplace_back(VPUGraphInitCommand::create(ctx,
                                                      1, // BlobId : 1
                                                      blobData2,
                                                      blobSize,
                                                      scratchSize,
                                                      metadataSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUGraphExecuteCommand::create(
        ctx,
        1, // BlobId : 1
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(inputData2, allocSize)}},
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(outputData2, allocSize)}},
        commands.back()->getAssociateBufferObjects()));
    ASSERT_NE(commands.back(), nullptr);
    EXPECT_EQ(commands.size(), 4u);

    auto descBuffer = ctx->createInternalBufferObject(allocSize, VPUBufferObject::Type::CachedLow);
    ASSERT_NE(descBuffer, nullptr);

    checkOffsets(commands, descBuffer);

    EXPECT_TRUE(ctx->freeMemAlloc(descBuffer));
    EXPECT_TRUE(ctx->freeMemAlloc(inputData1));
    EXPECT_TRUE(ctx->freeMemAlloc(outputData1));
    EXPECT_TRUE(ctx->freeMemAlloc(inputData2));
    EXPECT_TRUE(ctx->freeMemAlloc(outputData2));
}

TEST_F(DeviceContextTest, createTimestampAndCopyCommandListToCheckCommandsOffset) {
    // ======================== Scenario #1 ========================
    // Description: Usual flow of memory allocation and offset returning
    //              where all memory allocated has been assigned heap
    //              usage and passed into pinning step according to order
    //              added to command list.

    // Allocate Memory
    auto timestamp1 = ctx->createSharedMemAlloc(allocSize);
    auto copy1 = ctx->createSharedMemAlloc(allocSize);
    auto dest1 = ctx->createSharedMemAlloc(allocSize);
    auto timestamp2 = ctx->createSharedMemAlloc(allocSize);

    // Append commands to commandlist vector
    std::vector<std::shared_ptr<VPUCommand>> commands;

    commands.emplace_back(VPUTimeStampCommand::create(ctx, (uint64_t *)timestamp1));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx, copy1, dest1, allocSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(ctx, (uint64_t *)timestamp2));
    ASSERT_NE(commands.back(), nullptr);
    EXPECT_EQ(commands.size(), 3u);

    // replicating functionality from commandqueueExecuteCommandLists
    auto descBuffer = ctx->createInternalBufferObject(allocSize, VPUBufferObject::Type::CachedLow);
    ASSERT_NE(descBuffer, nullptr);

    checkOffsets(commands, descBuffer);

    EXPECT_TRUE(ctx->freeMemAlloc(descBuffer));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp1));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp2));
    EXPECT_TRUE(ctx->freeMemAlloc(copy1));
    EXPECT_TRUE(ctx->freeMemAlloc(dest1));
}

TEST_F(DeviceContextTest,
       createMemAndAppendCommandListInOrderButMissingOneCommandOffsetReturnsCorrectly) {
    // ======================== Scenario #2 ========================
    // Description: Modified flow of memory allocation and offset
    //              returning where not all memory allocated has been
    //              assigned heap usage and passed into pinning step
    //              according to order added to command list. Testing
    //              logic whereby timestamp2 is not used and checking
    //              that pinning procedure still proceeds as expected.

    // Allocate Memory
    auto timestamp1 = ctx->createSharedMemAlloc(allocSize);
    auto copy1 = ctx->createSharedMemAlloc(allocSize);
    auto dest1 = ctx->createSharedMemAlloc(allocSize);
    auto timestamp2 = ctx->createSharedMemAlloc(allocSize);

    // Append commands to commandlist vector
    std::vector<std::shared_ptr<VPUCommand>> commands;

    commands.emplace_back(VPUTimeStampCommand::create(ctx, (uint64_t *)timestamp1));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx, copy1, dest1, allocSize));
    ASSERT_NE(commands.back(), nullptr);

    EXPECT_EQ(commands.size(), 2u);

    // replicating functionality from commandqueueExecuteCommandLists
    auto descBuffer = ctx->createInternalBufferObject(allocSize, VPUBufferObject::Type::CachedLow);
    ASSERT_NE(descBuffer, nullptr);

    checkOffsets(commands, descBuffer);

    EXPECT_TRUE(ctx->freeMemAlloc(descBuffer));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp1));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp2));
    EXPECT_TRUE(ctx->freeMemAlloc(copy1));
    EXPECT_TRUE(ctx->freeMemAlloc(dest1));
}

TEST_F(DeviceContextTest, createMemAndAppendCommandListNotInOrderOffsetReturnsCorrectly) {
    // ======================== Scenario #3 ========================
    // Description: Modified flow of memory allocation and offset
    //              returning where all memory allocated has been
    //              assigned heap usage, however assignment was not
    //              done in order of allocation. This is to test that
    //              commandlist pinning procedure respects order added
    //              to commandlist and not order of allocated memory.

    // Allocate Memory
    auto timestamp1 = ctx->createSharedMemAlloc(allocSize);
    auto copy1 = ctx->createSharedMemAlloc(allocSize);
    auto dest1 = ctx->createSharedMemAlloc(allocSize);
    auto timestamp2 = ctx->createSharedMemAlloc(allocSize);

    // Append commands to commandlist vector
    std::vector<std::shared_ptr<VPUCommand>> commands;

    commands.emplace_back(VPUTimeStampCommand::create(ctx, (uint64_t *)timestamp1));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx, copy1, dest1, allocSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(ctx, (uint64_t *)timestamp2));
    ASSERT_NE(commands.back(), nullptr);

    EXPECT_EQ(commands.size(), 3u);

    // replicating functionality from commandqueueExecuteCommandLists
    auto descBuffer = ctx->createInternalBufferObject(allocSize, VPUBufferObject::Type::CachedLow);
    ASSERT_NE(descBuffer, nullptr);

    checkOffsets(commands, descBuffer);

    EXPECT_TRUE(ctx->freeMemAlloc(descBuffer));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp1));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp2));
    EXPECT_TRUE(ctx->freeMemAlloc(copy1));
    EXPECT_TRUE(ctx->freeMemAlloc(dest1));
}

TEST_F(DeviceContextTest, createMemAndAppendLargeCommandListOffsetReturnsCorrectly) {
    // ======================== Scenario #4 ========================
    // Description: Same scenario as Scenario #1 but with more commands
    //              interleaved for scaled up testing.

    // Allocate Memory
    auto timestamp1 = ctx->createSharedMemAlloc(allocSize);
    auto copy1 = ctx->createSharedMemAlloc(allocSize);
    auto dest1 = ctx->createSharedMemAlloc(allocSize);
    auto timestamp2 = ctx->createSharedMemAlloc(allocSize);
    auto copy2 = ctx->createSharedMemAlloc(allocSize);
    auto dest2 = ctx->createSharedMemAlloc(allocSize);
    auto copy3 = ctx->createSharedMemAlloc(allocSize);
    auto dest3 = ctx->createSharedMemAlloc(allocSize);
    auto timestamp3 = ctx->createSharedMemAlloc(allocSize);
    auto timestamp4 = ctx->createSharedMemAlloc(allocSize);

    // Append commands to commandlist vector
    std::vector<std::shared_ptr<VPUCommand>> commands;

    commands.emplace_back(VPUTimeStampCommand::create(ctx, (uint64_t *)timestamp1));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx, copy1, dest1, allocSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(ctx, (uint64_t *)timestamp2));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx, copy2, dest2, allocSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx, copy3, dest3, allocSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(ctx, (uint64_t *)timestamp3));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(ctx, (uint64_t *)timestamp4));
    ASSERT_NE(commands.back(), nullptr);
    EXPECT_EQ(commands.size(), 7u);

    // replicating functionality from commandqueueExecuteCommandLists
    auto descBuffer = ctx->createInternalBufferObject(allocSize, VPUBufferObject::Type::CachedLow);
    ASSERT_NE(descBuffer, nullptr);

    checkOffsets(commands, descBuffer);

    EXPECT_TRUE(ctx->freeMemAlloc(descBuffer));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp1));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp2));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp3));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp4));
    EXPECT_TRUE(ctx->freeMemAlloc(copy1));
    EXPECT_TRUE(ctx->freeMemAlloc(copy2));
    EXPECT_TRUE(ctx->freeMemAlloc(copy3));
    EXPECT_TRUE(ctx->freeMemAlloc(dest1));
    EXPECT_TRUE(ctx->freeMemAlloc(dest2));
    EXPECT_TRUE(ctx->freeMemAlloc(dest3));
}

TEST_F(DeviceContextTest, createGraphCommandOffsetsReturnsCorrectly) {
    // Testing scenario to include multiple commands
    // CommandList structure:
    //          VPUTimeStampCommand
    //          VPUCopyCommand
    //          VPUGraphInitCommand
    //          VPUGraphExecuteCommand
    //          VPUTimeStampCommand

    // Set blob size
    const size_t blobSize = 312 * 1024; // 312 KB

    // Allocate scratch and metadata heap
    const uint32_t scratchSize = 4 * 1024;          // 4 KB
    const uint32_t metadataSize = 16 * 1024 * 1024; // 16MB

    // Allocate Shared Memory
    auto timestamp1 = ctx->createSharedMemAlloc(allocSize);
    auto timestamp2 = ctx->createSharedMemAlloc(allocSize);
    auto copy1 = ctx->createSharedMemAlloc(allocSize);
    auto dest1 = ctx->createSharedMemAlloc(allocSize);
    auto copy2 = ctx->createSharedMemAlloc(allocSize);
    auto dest2 = ctx->createSharedMemAlloc(allocSize);

    // Allocate Shared Memory
    auto inputData1 = ctx->createSharedMemAlloc(allocSize);
    auto outputData1 = ctx->createSharedMemAlloc(allocSize);
    auto inputData2 = ctx->createSharedMemAlloc(allocSize);
    auto outputData2 = ctx->createSharedMemAlloc(allocSize);

    // Allocate Host Memory
    uint8_t blobData1[blobSize];
    uint8_t blobData2[blobSize];

    // Set raw data to allocated Host memory for graph blob
    memset(blobData1, 'A', blobSize);
    memset(blobData2, 'A', blobSize);

    // Append commands to commandlist vector
    std::vector<std::shared_ptr<VPUCommand>> commands;

    commands.emplace_back(VPUTimeStampCommand::create(ctx, (uint64_t *)timestamp1));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx, copy1, dest1, allocSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUGraphInitCommand::create(ctx,
                                                      0, // BlobId : 0
                                                      blobData1,
                                                      blobSize,
                                                      scratchSize,
                                                      metadataSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUGraphExecuteCommand::create(
        ctx,
        0, // BlobId : 0
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(inputData1, allocSize)}},
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(outputData1, allocSize)}},
        commands.back()->getAssociateBufferObjects()));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx, copy2, dest2, allocSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUGraphInitCommand::create(ctx,
                                                      1, // BlobId : 1
                                                      blobData2,
                                                      blobSize,
                                                      scratchSize,
                                                      metadataSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUGraphExecuteCommand::create(
        ctx,
        1, // BlobId : 1
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(inputData2, allocSize)}},
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(outputData2, allocSize)}},
        commands.back()->getAssociateBufferObjects()));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(ctx, (uint64_t *)timestamp2));
    ASSERT_NE(commands.back(), nullptr);

    EXPECT_EQ(commands.size(), 8u);

    auto descBuffer = ctx->createInternalBufferObject(allocSize, VPUBufferObject::Type::CachedLow);
    ASSERT_NE(descBuffer, nullptr);

    checkOffsets(commands, descBuffer);

    // Free'ing explicitly allocated memory
    EXPECT_TRUE(ctx->freeMemAlloc(descBuffer));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp1));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp2));
    EXPECT_TRUE(ctx->freeMemAlloc(copy1));
    EXPECT_TRUE(ctx->freeMemAlloc(copy2));
    EXPECT_TRUE(ctx->freeMemAlloc(dest1));
    EXPECT_TRUE(ctx->freeMemAlloc(dest2));
    EXPECT_TRUE(ctx->freeMemAlloc(inputData1));
    EXPECT_TRUE(ctx->freeMemAlloc(outputData1));
    EXPECT_TRUE(ctx->freeMemAlloc(inputData2));
    EXPECT_TRUE(ctx->freeMemAlloc(outputData2));
}

TEST_F(DeviceContextTest, bufferBoundaryShouldProperlyCheckedByDeviceContext) {
    void *ptr = ctx->createSharedMemAlloc(allocSize);
    uint8_t *checkPtr = (uint8_t *)ptr;

    // A pointer at 0 offset.
    EXPECT_NE(nullptr, ctx->findBuffer(checkPtr));

    // A pointer within the 4KB range.
    checkPtr += 10;
    EXPECT_NE(nullptr, ctx->findBuffer(checkPtr));

    // A pointer outside of the range.
    checkPtr = reinterpret_cast<uint8_t *>(ptr) + (4 * 1024) + 1;
    EXPECT_EQ(nullptr, ctx->findBuffer(checkPtr));

    // Free memory
    EXPECT_TRUE(ctx->freeMemAlloc(ptr));
}

TEST_F(DeviceContextTest, givenPointerDeviceContextReturnsProperValidity) {
    // nullptr is none-valid.
    EXPECT_FALSE(ctx->findBuffer(nullptr));

    // non-null yet invalid pointer.
    void *nonNullPtr = (void *)0x04;
    EXPECT_FALSE(ctx->findBuffer(nonNullPtr));

    // Dynamically allocated memory is not available to the VPU device.
    uint8_t *dynamicAlloc = new uint8_t[100];
    EXPECT_FALSE(ctx->findBuffer(dynamicAlloc));
    delete[] dynamicAlloc;

    // Local variables are not valid to VPU device.
    uint64_t localVar = 0;
    EXPECT_FALSE(ctx->findBuffer(&localVar));

    // Static / Global variables are not valid to VPU device.
    static uint64_t memTestGlobalVar;
    EXPECT_FALSE(ctx->findBuffer(&memTestGlobalVar));

    // Allocated memory is valid and visible to VPU device.
    void *allocPtr = ctx->createSharedMemAlloc(allocSize);
    EXPECT_TRUE(ctx->findBuffer(allocPtr));
    EXPECT_TRUE(ctx->freeMemAlloc(allocPtr));
}

TEST_F(DeviceContextTest, checkForCorrectReturnMemTypeInHeap) {
    // Allocate mem pointers
    void *sharedPtr = ctx->createSharedMemAlloc(allocSize);
    void *hostPtr = ctx->createHostMemAlloc(allocSize);
    EXPECT_NE(sharedPtr, hostPtr);

    EXPECT_EQ(COPY_LOCAL_TO_LOCAL, ctx->getCopyDirection(sharedPtr, sharedPtr));
    EXPECT_EQ(COPY_LOCAL_TO_SYSTEM, ctx->getCopyDirection(hostPtr, sharedPtr));
    EXPECT_EQ(COPY_SYSTEM_TO_LOCAL, ctx->getCopyDirection(sharedPtr, hostPtr));
    EXPECT_EQ(COPY_SYSTEM_TO_SYSTEM, ctx->getCopyDirection(hostPtr, hostPtr));

    // Checking with invalid mem type
    osInfc.mockFailNextAlloc();
    void *hostInvalidPtr = ctx->createHostMemAlloc(allocSize);
    EXPECT_EQ(COPY_INVALID, ctx->getCopyDirection(hostInvalidPtr, sharedPtr));

    // Free'ing memory allocated
    EXPECT_TRUE(ctx->freeMemAlloc(sharedPtr));
    EXPECT_TRUE(ctx->freeMemAlloc(hostPtr));
}

TEST_F(DeviceContextTest, memoryMangerReturnsPageAlignedSize) {
    size_t sz = ctx->getPageAlignedSize(0u);
    EXPECT_EQ(0u, sz);

    sz = ctx->getPageAlignedSize(1u);
    EXPECT_EQ(4u * 1024u, sz);

    sz = ctx->getPageAlignedSize(4u * 1024u);
    EXPECT_EQ(4u * 1024u, sz);

    sz = ctx->getPageAlignedSize(4u * 1024u + 1u);
    EXPECT_EQ(4u * 1024u * 2u, sz);
}

TEST_F(DeviceContextTest, implictlyAllocatedCopyCommandMemoryShouldBeDeallocated) {
    // Direct memory allocation
    void *hostSrcPtr = ctx->createHostMemAlloc(4096u);
    void *hostDestPtr = ctx->createHostMemAlloc(4096u);
    ASSERT_NE(nullptr, hostSrcPtr);
    ASSERT_NE(nullptr, hostDestPtr);

    // Copy command will internally create a descriptor buffer
    // which will be deallocated from VPUCopyCommand's destructor.
    auto cpCmd = VPUCopyCommand::create(ctx, hostSrcPtr, hostDestPtr, 4096);
    EXPECT_NE(cpCmd, nullptr);

    // Deallocate explictly allocated memory.
    EXPECT_TRUE(ctx->freeMemAlloc(hostSrcPtr));
    EXPECT_TRUE(ctx->freeMemAlloc(hostDestPtr));
}

TEST_F(DeviceContextTest, implictlyAllocatedGraphCommandMemoryShouldBeDeallocated) {
    uint32_t umdBlobId = 0xdeadbeef;
    const size_t blobSize = 4 * 1024;
    const uint32_t scratchSize = 4 * 1024;
    const uint32_t metadataSize = 16 * 1024 * 1024; // 16MB
    uint8_t blobData[blobSize] = {};

    auto graphInitCmd =
        VPUGraphInitCommand::create(ctx, umdBlobId, blobData, blobSize, scratchSize, metadataSize);
    ASSERT_NE(graphInitCmd, nullptr);

    // Allocate input and output heaps
    auto inputData = ctx->createSharedMemAlloc(allocSize);
    auto outputData = ctx->createSharedMemAlloc(allocSize);

    auto graphExecCmd = VPUGraphExecuteCommand::create(
        ctx,
        umdBlobId,
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(inputData, allocSize)}},
        std::vector<std::pair<const void *, uint32_t>>{{std::make_pair(outputData, allocSize)}},
        graphInitCmd->getAssociateBufferObjects());

    // Deallocate explictly allocated memory.
    EXPECT_TRUE(ctx->freeMemAlloc(inputData));
    EXPECT_TRUE(ctx->freeMemAlloc(outputData));

    // reset() will make destructor be called.
    graphInitCmd.reset();
    graphExecCmd.reset();
}
