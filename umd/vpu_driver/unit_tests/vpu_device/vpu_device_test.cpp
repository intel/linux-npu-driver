/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/device/device_info.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/command/vpu_ts_command.hpp"

#include "gtest/gtest.h"

#include <memory>
#include <sys/mman.h>

using namespace VPU;

struct VPUDeviceTest : public ::testing::Test {
    void SetUp() {}

    void TearDown() { ASSERT_EQ(ctx->getBuffersCount(), 0u); }

    MockOsInterfaceImp osInfc;
    std::unique_ptr<MockVPUDevice> vpuDevice = MockVPUDevice::createWithDefaultHardwareInfo(osInfc);
    std::shared_ptr<VPUDeviceContext> ctx = vpuDevice->createDeviceContext();
};

TEST_F(VPUDeviceTest, jobSubmissionTriggersIoctls) {
    osInfc.callCntIoctl = 0;

    EXPECT_FALSE(ctx->submitJob(nullptr));

    uint64_t *tsDest = static_cast<uint64_t *>(ctx->createSharedMemAlloc(4096));
    ASSERT_NE(nullptr, tsDest);

    auto job = std::make_unique<VPUJob>(ctx.get(), false);
    EXPECT_TRUE(job->appendCommand(VPUTimeStampCommand::create(ctx.get(), tsDest)));
    EXPECT_TRUE(job->closeCommands());

    // Without pinning this test won't pass
    EXPECT_TRUE(ctx->submitJob(job.get()));
    // Ioctls:
    // * createSharedMemAlloc calls BO_CREATE and BO_INFO
    // * allocateJob calls BO_CREATE and BO_INFO
    // * submitJob calls SUBMIT
    EXPECT_EQ(5u, osInfc.callCntIoctl);

    EXPECT_TRUE(ctx->freeMemAlloc(static_cast<void *>(tsDest)));
}

TEST_F(VPUDeviceTest, allocateMemory) {
    void *memPtr = nullptr;
    size_t size = 10;
    const auto &drvApi = ctx->getDriverApi();

    // Alloc failed case.
    osInfc.mockFailNextAlloc();
    memPtr = drvApi.alloc(size);
    EXPECT_FALSE(memPtr);

    // Alloc successful.
    memPtr = ctx->createHostMemAlloc(size, VPUBufferObject::Type::CachedLow);
    EXPECT_NE(nullptr, memPtr);

    bool unmapRes = ctx->freeMemAlloc(memPtr);
    EXPECT_TRUE(unmapRes);

    // Attempting to free invalid memory space.
    EXPECT_FALSE(ctx->freeMemAlloc((void *)nullptr));
}

TEST_F(VPUDeviceTest, givenCallIsConnectedReportsDeviceConnectionStatus) {
    // Device disconnected.
    osInfc.deviceConnected = false;
    EXPECT_FALSE(vpuDevice->isConnected());

    // Device connected.
    osInfc.deviceConnected = true;
    EXPECT_TRUE(vpuDevice->isConnected());
}
