/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <cstdint>
#include <stddef.h>

#include "api/vpu_jsm_job_cmd_api.h"
#include "gtest/gtest.h"
#include "umd_common.hpp"
#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/command/vpu_ts_command.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace VPU;

struct DeviceContextTest : public ::testing::Test {
    void SetUp() override {
        // Make sure no other tracking buffers exist in MM.
        ASSERT_EQ(ctx->getBuffersCount(), 0u);
        cmdBufferHeader.kernel_heap_base_address = 0;
        cmdBufferHeader.descriptor_heap_base_address = 0;
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
                      std::shared_ptr<VPUBufferObject> descBuffer) {
        void *descTail = nullptr;
        size_t expDescOffset = 0;
        if (descBuffer.get()) {
            descTail = descBuffer->getBasePointer();
            expDescOffset = getOffset(descBuffer->getVPUAddr(descTail),
                                      cmdBufferHeader.descriptor_heap_base_address);
        }

        for (auto &cmd : commands) {
            EXPECT_TRUE(cmd->copyDescriptor(&descTail, descBuffer));

            uint64_t offset = UINT64_MAX;
            uint64_t vpuAddr = UINT64_MAX;
            switch (cmd->getCommandType()) {
            case VPU_CMD_TIMESTAMP:
                vpuAddr = reinterpret_cast<const vpu_cmd_timestamp_t *>(cmd->getCommitStream())
                              ->timestamp_address;
                EXPECT_GT(vpuAddr, 0) << "Invalid timestamp address in VPU_CMD_TIMESTAMP";
                break;
            case VPU_CMD_COPY_LOCAL_TO_LOCAL:
                offset = reinterpret_cast<const vpu_cmd_copy_buffer_t *>(cmd->getCommitStream())
                             ->desc_start_offset;
                EXPECT_EQ(offset, expDescOffset) << "Invalid descriptor offset in VPU_CMD_COPY_*";
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
    std::unique_ptr<MockVPUDeviceContext> deviceContext = vpuDevice->createMockDeviceContext();
    MockVPUDeviceContext *ctx = deviceContext.get();
    vpu_cmd_buffer_header_t cmdBufferHeader = {};

    const uint32_t allocSize = 4 * 1024;
};

TEST_F(DeviceContextTest, createAndFreeDeviceMemoryExpectSuccess) {
    auto bo = ctx->createDeviceMemAlloc(allocSize);
    EXPECT_NE(nullptr, bo);
    EXPECT_EQ(1u, ctx->getBuffersCount());
    EXPECT_TRUE(ctx->freeMemAlloc(bo->getBasePointer()));
}

TEST_F(DeviceContextTest, createAndFreeHostMemoryExpectSuccess) {
    auto bo = ctx->createHostMemAlloc(allocSize);
    EXPECT_NE(nullptr, bo);
    EXPECT_EQ(1u, ctx->getBuffersCount());
    EXPECT_TRUE(ctx->freeMemAlloc(bo->getBasePointer()));
}

TEST_F(DeviceContextTest, freeDeviceMemoryUsingNonBasePointerExpectFail) {
    auto bo = ctx->createDeviceMemAlloc(allocSize);
    EXPECT_NE(nullptr, bo);
    EXPECT_EQ(1u, ctx->getBuffersCount());

    auto ptr_offset = reinterpret_cast<uint8_t *>(bo->getBasePointer()) + 1000;
    EXPECT_FALSE(ctx->freeMemAlloc(ptr_offset));
    ptr_offset = reinterpret_cast<uint8_t *>(bo->getBasePointer()) + (4 * 1024);
    EXPECT_FALSE(ctx->freeMemAlloc(ptr_offset));

    EXPECT_TRUE(ctx->freeMemAlloc(bo->getBasePointer()));
}

TEST_F(DeviceContextTest, createAndFreeDeviceMemoryInHighRangeExpectSuccess) {
    auto bo = ctx->createSharedMemAlloc(allocSize, VPUBufferObject::Type::CachedShave);
    EXPECT_NE(nullptr, bo);
    EXPECT_EQ(1u, ctx->getBuffersCount());
    EXPECT_TRUE(ctx->freeMemAlloc(bo->getBasePointer()));
}

TEST_F(DeviceContextTest, createAndFreeHostMemoryInHighRangeExpectSuccess) {
    auto bo = ctx->createHostMemAlloc(allocSize, VPUBufferObject::Type::CachedShave);
    EXPECT_NE(nullptr, bo);
    EXPECT_EQ(1u, ctx->getBuffersCount());
    EXPECT_TRUE(ctx->freeMemAlloc(bo->getBasePointer()));
}

TEST_F(DeviceContextTest, createHostMemoryAndGetVPUAddressExpectSuccess) {
    auto bo = ctx->createHostMemAlloc(allocSize);
    EXPECT_NE(nullptr, bo);

    auto vpuAddr = bo->getVPUAddr();
    EXPECT_NE(0u, vpuAddr);

    auto ptrOffset = reinterpret_cast<uint8_t *>(bo->getBasePointer()) + allocSize / 4;
    EXPECT_EQ(vpuAddr + allocSize / 4, bo->getVPUAddr(ptrOffset));

    EXPECT_TRUE(ctx->freeMemAlloc(bo->getBasePointer()));
}

TEST_F(DeviceContextTest, createDeviceMemoryAndGetVPUAddressExpectSuccess) {
    auto bo = ctx->createDeviceMemAlloc(allocSize);
    EXPECT_NE(nullptr, bo);

    auto vpuAddr = bo->getVPUAddr();
    EXPECT_NE(0u, vpuAddr);

    auto ptrOffset = reinterpret_cast<uint8_t *>(bo->getBasePointer()) + allocSize / 4;
    EXPECT_EQ(vpuAddr + allocSize / 4, bo->getVPUAddr(ptrOffset));

    EXPECT_TRUE(ctx->freeMemAlloc(bo->getBasePointer()));
}

TEST_F(DeviceContextTest,
       allocatingHeapAndUsingWithinHeapMemoryForTimestampCommandReturnsSuccessful) {
    // Allocate Memory of 4 * 64bit
    size_t tsAllocSize = 4 * sizeof(uint64_t);
    auto timestamp1Bo = ctx->createSharedMemAlloc(tsAllocSize);
    auto timestamp2Bo = ctx->createSharedMemAlloc(tsAllocSize);

    uint64_t *timestamp11 = reinterpret_cast<uint64_t *>(timestamp1Bo->getBasePointer());
    uint64_t *timestamp12 = timestamp11 + 1;
    uint64_t *timestamp13 = timestamp11 + 2;
    uint64_t *timestamp14 = timestamp11 + 3;

    uint64_t *timestamp21 = reinterpret_cast<uint64_t *>(timestamp2Bo->getBasePointer());
    uint64_t *timestamp22 = timestamp21 + 1;
    uint64_t *timestamp23 = timestamp21 + 2;

    // Creating and appending commands to commandlist
    std::vector<std::shared_ptr<VPUCommand>> commands;

    commands.emplace_back(VPUTimeStampCommand::create(timestamp11, timestamp1Bo));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(timestamp12, timestamp1Bo));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(timestamp13, timestamp1Bo));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(timestamp14, std::move(timestamp1Bo)));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(timestamp22, timestamp2Bo));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUTimeStampCommand::create(timestamp23, std::move(timestamp2Bo)));
    ASSERT_NE(commands.back(), nullptr);

    EXPECT_EQ(commands.size(), 6u);
    checkOffsets(commands, nullptr);

    EXPECT_TRUE(ctx->freeMemAlloc(timestamp11));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp21));
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

    commands.emplace_back(
        VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(timestamp1->getBasePointer()),
                                    timestamp1));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx,
                                                 copy1->getBasePointer(),
                                                 copy1,
                                                 dest1->getBasePointer(),
                                                 dest1,
                                                 allocSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(
        VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(timestamp2->getBasePointer()),
                                    timestamp2));
    ASSERT_NE(commands.back(), nullptr);
    EXPECT_EQ(commands.size(), 3u);

    // replicating functionality from commandqueueExecuteCommandLists
    auto descBuffer = ctx->createUntrackedBufferObject(allocSize, VPUBufferObject::Type::CachedFw);
    ASSERT_NE(descBuffer, nullptr);

    checkOffsets(commands, std::move(descBuffer));

    EXPECT_TRUE(ctx->freeMemAlloc(timestamp1->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp2->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(copy1->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(dest1->getBasePointer()));
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

    // Append commands to commandlist vector
    std::vector<std::shared_ptr<VPUCommand>> commands;

    commands.emplace_back(
        VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(timestamp1->getBasePointer()),
                                    timestamp1));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx,
                                                 copy1->getBasePointer(),
                                                 copy1,
                                                 dest1->getBasePointer(),
                                                 dest1,
                                                 allocSize));
    ASSERT_NE(commands.back(), nullptr);

    EXPECT_EQ(commands.size(), 2u);

    // replicating functionality from commandqueueExecuteCommandLists
    auto descBuffer = ctx->createUntrackedBufferObject(allocSize, VPUBufferObject::Type::CachedFw);
    ASSERT_NE(descBuffer, nullptr);

    checkOffsets(commands, std::move(descBuffer));

    EXPECT_TRUE(ctx->freeMemAlloc(timestamp1->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(copy1->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(dest1->getBasePointer()));
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

    commands.emplace_back(
        VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(timestamp1->getBasePointer()),
                                    timestamp1));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx,
                                                 copy1->getBasePointer(),
                                                 copy1,
                                                 dest1->getBasePointer(),
                                                 dest1,
                                                 allocSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(
        VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(timestamp2->getBasePointer()),
                                    timestamp2));
    ASSERT_NE(commands.back(), nullptr);

    EXPECT_EQ(commands.size(), 3u);

    // replicating functionality from commandqueueExecuteCommandLists
    auto descBuffer = ctx->createUntrackedBufferObject(allocSize, VPUBufferObject::Type::CachedFw);
    ASSERT_NE(descBuffer, nullptr);

    checkOffsets(commands, std::move(descBuffer));

    EXPECT_TRUE(ctx->freeMemAlloc(timestamp1->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp2->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(copy1->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(dest1->getBasePointer()));
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

    commands.emplace_back(
        VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(timestamp1->getBasePointer()),
                                    timestamp1));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx,
                                                 copy1->getBasePointer(),
                                                 copy1,
                                                 dest1->getBasePointer(),
                                                 dest1,
                                                 allocSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(
        VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(timestamp2->getBasePointer()),
                                    timestamp2));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx,
                                                 copy2->getBasePointer(),
                                                 copy2,
                                                 dest2->getBasePointer(),
                                                 dest2,
                                                 allocSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(VPUCopyCommand::create(ctx,
                                                 copy3->getBasePointer(),
                                                 copy3,
                                                 dest3->getBasePointer(),
                                                 dest3,
                                                 allocSize));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(
        VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(timestamp3->getBasePointer()),
                                    timestamp3));
    ASSERT_NE(commands.back(), nullptr);

    commands.emplace_back(
        VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(timestamp4->getBasePointer()),
                                    timestamp4));
    ASSERT_NE(commands.back(), nullptr);
    EXPECT_EQ(commands.size(), 7u);

    // replicating functionality from commandqueueExecuteCommandLists
    auto descBuffer = ctx->createUntrackedBufferObject(allocSize, VPUBufferObject::Type::CachedFw);
    ASSERT_NE(descBuffer, nullptr);

    checkOffsets(commands, std::move(descBuffer));

    EXPECT_TRUE(ctx->freeMemAlloc(timestamp1->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp2->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp3->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(timestamp4->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(copy1->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(copy2->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(copy3->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(dest1->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(dest2->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(dest3->getBasePointer()));
}

TEST_F(DeviceContextTest, bufferBoundaryShouldProperlyCheckedByDeviceContext) {
    auto bo = ctx->createSharedMemAlloc(allocSize);
    uint8_t *checkPtr = reinterpret_cast<uint8_t *>(bo->getBasePointer());

    // A pointer at 0 offset.
    EXPECT_NE(nullptr, ctx->findBufferObject(checkPtr));

    // A pointer within the 4KB range.
    checkPtr += 10;
    EXPECT_NE(nullptr, ctx->findBufferObject(checkPtr));

    // A pointer outside of the range.
    checkPtr = reinterpret_cast<uint8_t *>(checkPtr) + (4 * 1024) + 1;
    EXPECT_EQ(nullptr, ctx->findBufferObject(checkPtr));

    EXPECT_TRUE(ctx->freeMemAlloc(bo->getBasePointer()));
}

TEST_F(DeviceContextTest, givenPointerDeviceContextReturnsProperValidity) {
    // nullptr is none-valid.
    EXPECT_FALSE(ctx->findBufferObject(nullptr));

    // non-null yet invalid pointer.
    void *nonNullPtr = (void *)0x04;
    EXPECT_FALSE(ctx->findBufferObject(nonNullPtr));

    // Dynamically allocated memory is not available to the VPU device.
    uint8_t *dynamicAlloc = new uint8_t[100];
    EXPECT_FALSE(ctx->findBufferObject(dynamicAlloc));
    delete[] dynamicAlloc;

    // Local variables are not valid to VPU device.
    uint64_t localVar = 0;
    EXPECT_FALSE(ctx->findBufferObject(&localVar));

    // Static / Global variables are not valid to VPU device.
    static uint64_t memTestGlobalVar;
    EXPECT_FALSE(ctx->findBufferObject(&memTestGlobalVar));

    // Allocated memory is valid and visible to VPU device.
    auto allocBo = ctx->createSharedMemAlloc(allocSize);
    EXPECT_TRUE(ctx->findBufferObject(allocBo->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(allocBo->getBasePointer()));
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
    auto hostSrcBo = ctx->createHostMemAlloc(4096u);
    auto hostDestBo = ctx->createHostMemAlloc(4096u);
    ASSERT_NE(nullptr, hostSrcBo);
    ASSERT_NE(nullptr, hostDestBo);

    // Copy command will internally create a descriptor buffer
    // which will be deallocated from VPUCopyCommand's destructor.
    auto cpCmd = VPUCopyCommand::create(ctx,
                                        hostSrcBo->getBasePointer(),
                                        hostSrcBo,
                                        hostDestBo->getBasePointer(),
                                        hostDestBo,
                                        4096);
    EXPECT_NE(cpCmd, nullptr);

    EXPECT_TRUE(ctx->freeMemAlloc(hostSrcBo->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(hostDestBo->getBasePointer()));
}
