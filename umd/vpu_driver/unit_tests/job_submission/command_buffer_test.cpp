/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "vpu_driver/source/command/command_buffer.hpp"
#include "vpu_driver/source/command/copy_command.hpp"
#include "vpu_driver/source/command/ts_command.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include <algorithm>
#include <memory>
#include <ostream>
#include <unordered_set>
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
    EXPECT_EQ(nullptr, VPUCommandBuffer::allocateCommandBuffer(ctx, cmds.begin(), cmds.end()));
}

TEST_F(VPUCommandBufferTest, allocateCommandBufferWithTimestampCommand) {
    auto tsHeap = ctx->createSharedMemAlloc(sizeof(uint64_t));

    std::vector<std::shared_ptr<VPUCommand>> cmds;
    cmds.emplace_back(
        VPUTimeStampCommand::create(reinterpret_cast<uint64_t *>(tsHeap->getBasePointer()),
                                    tsHeap));
    ASSERT_NE(cmds.back(), nullptr);

    EXPECT_NE(nullptr, VPUCommandBuffer::allocateCommandBuffer(ctx, cmds.begin(), cmds.end()));
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
    EXPECT_EQ(nullptr, VPUCommandBuffer::allocateCommandBuffer(ctx, cmds.begin(), cmds.end()));
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

    EXPECT_NE(nullptr, VPUCommandBuffer::allocateCommandBuffer(ctx, cmds.begin(), cmds.end()));
    EXPECT_TRUE(ctx->freeMemAlloc(srcBo->getBasePointer()));
    EXPECT_TRUE(ctx->freeMemAlloc(dstBo->getBasePointer()));
}

std::ostream &operator<<(std::ostream &os, const VPUCommandBuffer::BufferHandles &handles) {
    os << "{ ";
    for (auto h : handles)
        os << h << ", ";
    os << "}";
    return os;
}

TEST_F(VPUCommandBufferTest, BufferHandlesUniqueInsertInOrder) {
    VPUCommandBuffer::BufferHandles handles;
    handles.insert(1);
    handles.insert(1);
    handles.insert(3);
    handles.insert(2);
    handles.insert(3);

    EXPECT_THAT(handles, testing::ElementsAre(1, 3, 2));

    handles.erase(1);
    EXPECT_THAT(handles, testing::ElementsAre(1, 3, 2));
    handles.erase(1);
    EXPECT_THAT(handles, testing::ElementsAre(3, 2));
    handles.erase(1);
    EXPECT_THAT(handles, testing::ElementsAre(3, 2));

    handles.erase(3);
    EXPECT_THAT(handles, testing::ElementsAre(3, 2));
    handles.erase(3);
    EXPECT_THAT(handles, testing::ElementsAre(2));

    handles.erase(2);
    EXPECT_THAT(handles, testing::ElementsAre());
    handles.erase(2);
    EXPECT_THAT(handles, testing::ElementsAre());
}

TEST_F(VPUCommandBufferTest, BufferHandlesErase) {
    VPUCommandBuffer::BufferHandles handles;
    handles.insert(1);
    handles.insert(3);
    handles.insert(2);

    handles.erase(4);
    EXPECT_THAT(handles, testing::ElementsAre(1, 3, 2));
    handles.erase(1);
    EXPECT_THAT(handles, testing::ElementsAre(3, 2));
    handles.erase(1);
    EXPECT_THAT(handles, testing::ElementsAre(3, 2));
    handles.erase(2);
    EXPECT_THAT(handles, testing::ElementsAre(3));
    handles.erase(3);
    EXPECT_THAT(handles, testing::ElementsAre());
}

TEST_F(VPUCommandBufferTest, BufferHandlesInsertResidentSet) {
    VPUCommandBuffer::BufferHandles handles;
    handles.insert(1);
    handles.insert(4);
    handles.insert(3);
    handles.insert(2);

    std::unordered_set<uint32_t> residentSet = {5, 2, 3, 7, 10};
    handles.insertResidentSet(residentSet);
    EXPECT_THAT((std::vector<uint32_t>(handles.begin(), handles.begin() + 4)),
                testing::ElementsAre(1, 4, 3, 2));
    EXPECT_THAT((std::vector<uint32_t>(handles.begin() + 4, handles.end())),
                testing::UnorderedElementsAre(5, 7, 10));

    ASSERT_EQ(handles.position(2), 3);
    handles.erase(2);
    ASSERT_EQ(handles.position(2), -1);
    handles.insert(2);
    ASSERT_EQ(handles.position(2), 3);

    residentSet = {13, 12, 11, 1, 2};
    handles.insertResidentSet(residentSet);
    EXPECT_THAT((std::vector<uint32_t>(handles.begin(), handles.begin() + 4)),
                testing::ElementsAre(1, 4, 3, 2));
    EXPECT_THAT((std::vector<uint32_t>(handles.begin() + 4, handles.end())),
                testing::UnorderedElementsAre(13, 12, 11));

    // resident set duplicates some handles with the base set (handles 1 and 2)
    // but inserting that set doesn't change use count for those duplicated handles
    // so only one erase should be enough to remove handle 1 and handle 2
    handles.erase(1);
    EXPECT_THAT((std::vector<uint32_t>(handles.begin(), handles.begin() + 3)),
                testing::ElementsAre(4, 3, 2));
    EXPECT_THAT((std::vector<uint32_t>(handles.begin() + 3, handles.end())),
                testing::UnorderedElementsAre(13, 12, 11));
    handles.erase(2);
    EXPECT_THAT((std::vector<uint32_t>(handles.begin(), handles.begin() + 2)),
                testing::ElementsAre(4, 3));
    EXPECT_THAT((std::vector<uint32_t>(handles.begin() + 2, handles.end())),
                testing::UnorderedElementsAre(13, 12, 11));
}

} // namespace VPU
