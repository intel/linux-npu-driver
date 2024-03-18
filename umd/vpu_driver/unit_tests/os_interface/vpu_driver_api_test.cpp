/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"

#include <memory>
#include <gtest/gtest.h>
#include <stdexcept>
#include <uapi/drm/ivpu_accel.h>

#define FAKE_TEST_DEV_NODE "dev/node/fake"

using namespace VPU;

struct VPUDriverApiTest : public ::testing::Test {
    MockOsInterfaceImp mockOsInfc;
};

TEST_F(VPUDriverApiTest, successInitialization) {
    auto driverApi = VPUDriverApi::openDriverApi(FAKE_TEST_DEV_NODE, mockOsInfc);
    ASSERT_NE(driverApi, nullptr);
    EXPECT_TRUE(driverApi->isVpuDevice());
}

TEST_F(VPUDriverApiTest, failInitializationOnWrongIoctlVersion) {
    mockOsInfc.kmdApiVersionMajor = 0x7FFFFFFF;
    mockOsInfc.kmdApiVersionMinor = 0x7FFFFFFF;

    auto driverApi = VPUDriverApi::openDriverApi(FAKE_TEST_DEV_NODE, mockOsInfc);
    ASSERT_NE(driverApi, nullptr);
    EXPECT_FALSE(driverApi->isVpuDevice());
}

TEST_F(VPUDriverApiTest, failInitializationOnWrongDeviceName) {
    mockOsInfc.kmdApiDeviceName = "anything-but-intelVpu";

    auto driverApi = VPUDriverApi::openDriverApi(FAKE_TEST_DEV_NODE, mockOsInfc);
    ASSERT_NE(driverApi, nullptr);
    EXPECT_FALSE(driverApi->isVpuDevice());
}

TEST_F(VPUDriverApiTest, failInitializationOnInvalidFileDescriptors) {
    mockOsInfc.openSuccessful = false;

    auto driverApi = VPUDriverApi::openDriverApi(FAKE_TEST_DEV_NODE, mockOsInfc);
    ASSERT_EQ(driverApi, nullptr);
}

struct VPUDriverApiIoctlTest : public VPUDriverApiTest {
    void SetUp() override { ASSERT_NE(driverApi, nullptr); }

    std::unique_ptr<VPUDriverApi> driverApi =
        VPUDriverApi::openDriverApi(FAKE_TEST_DEV_NODE, mockOsInfc);
};

TEST_F(VPUDriverApiIoctlTest, getDeviceParamIoctl) {
    uint64_t deviceId = 0;
    EXPECT_NO_THROW(deviceId = driverApi->getDeviceParam(DRM_IVPU_PARAM_DEVICE_ID));
    EXPECT_EQ(deviceId, 0x7d1d);

    EXPECT_EQ(1u, mockOsInfc.callCntIoctl);
    EXPECT_EQ(DRM_IOCTL_IVPU_GET_PARAM, mockOsInfc.ioctlLastCommand);
}

TEST_F(VPUDriverApiIoctlTest, submitCommandBufferTest) {
    struct drm_ivpu_submit params = {};
    EXPECT_EQ(0, driverApi->submitCommandBuffer(&params));

    EXPECT_EQ(1u, mockOsInfc.callCntIoctl);
    EXPECT_EQ(DRM_IOCTL_IVPU_SUBMIT, mockOsInfc.ioctlLastCommand);
}

TEST_F(VPUDriverApiIoctlTest, ioctlError) {
    mockOsInfc.kmdIoctlRetCode = EINVAL;

    struct drm_ivpu_submit exec = {};
    EXPECT_EQ(-1, driverApi->submitCommandBuffer(&exec));

    EXPECT_THROW(driverApi->getDeviceParam(DRM_IVPU_PARAM_DEVICE_ID), std::runtime_error);

    struct drm_ivpu_bo_wait args = {};
    EXPECT_EQ(-1, driverApi->wait(&args));
}
