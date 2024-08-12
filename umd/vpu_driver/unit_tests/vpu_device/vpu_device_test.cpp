/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "gtest/gtest.h"
#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/command/vpu_ts_command.hpp"
#include "vpu_driver/source/device/metric_info.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include <memory>
#include <string>
#include <vector>

using namespace VPU;

struct VPUDeviceTest : public ::testing::Test {
    void SetUp() {}

    void TearDown() { ASSERT_EQ(ctx->getBuffersCount(), 0u); }

    MockOsInterfaceImp osInfc;
    std::unique_ptr<MockVPUDevice> vpuDevice = MockVPUDevice::createWithDefaultHardwareInfo(osInfc);
    std::shared_ptr<MockVPUDeviceContext> ctx = vpuDevice->createMockDeviceContext();
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

TEST_F(VPUDeviceTest, givenCallIsConnectedReportsDeviceConnectionStatus) {
    osInfc.deviceConnected = false;
    EXPECT_FALSE(vpuDevice->isConnected());

    osInfc.deviceConnected = true;
    EXPECT_TRUE(vpuDevice->isConnected());
}

TEST_F(VPUDeviceTest, deviceGetMetricsInfoRetrievesExpectedResults) {
    auto metricGroupsInfo = vpuDevice->getMetricGroupsInfo();

    EXPECT_GT(metricGroupsInfo.size(), 0u);

    EXPECT_TRUE("NOC" == metricGroupsInfo[0].metricGroupName);
    EXPECT_EQ(metricGroupsInfo[0].domain, 1u);
    EXPECT_EQ(metricGroupsInfo[0].metricCount, 1u);

    EXPECT_TRUE("noc" == metricGroupsInfo[0].counterInfo[0].metricName);
    EXPECT_EQ(metricGroupsInfo[0].counterInfo[0].metricType,
              CounterInfo::MetricType::METRIC_TYPE_EVENT);
    EXPECT_EQ(metricGroupsInfo[0].counterInfo[0].valueType,
              CounterInfo::ValueType::VALUE_TYPE_UINT64);
}
