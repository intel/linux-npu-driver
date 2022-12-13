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
#include <uapi/drm/ivpu_drm.h>

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
    struct drm_ivpu_param param = {};
    param.param = DRM_IVPU_PARAM_DEVICE_ID;
    EXPECT_EQ(0, driverApi->getDeviceParam(&param));

    EXPECT_EQ(1u, mockOsInfc.callCntIoctl);
    EXPECT_EQ(DRM_IOCTL_IVPU_GET_PARAM, mockOsInfc.ioctlLastCommand);
}

TEST_F(VPUDriverApiIoctlTest, submitCommandBufferTest) {
    struct drm_ivpu_submit params = {};
    EXPECT_EQ(0, driverApi->submitCommandBuffer(&params));

    EXPECT_EQ(1u, mockOsInfc.callCntIoctl);
    EXPECT_EQ(DRM_IOCTL_IVPU_SUBMIT, mockOsInfc.ioctlLastCommand);
}

TEST_F(VPUDriverApiIoctlTest, memAllocTest) {
    struct drm_ivpu_bo_userptr params = {};
    EXPECT_EQ(0, driverApi->createBufferFromUserPointer(&params));

    EXPECT_EQ(1u, mockOsInfc.callCntIoctl);
    EXPECT_EQ(DRM_IOCTL_IVPU_BO_USERPTR, mockOsInfc.ioctlLastCommand);
}

TEST_F(VPUDriverApiIoctlTest, ioctlError) {
    mockOsInfc.kmdIoctlRetCode = EINVAL;

    struct drm_ivpu_submit exec = {};
    EXPECT_EQ(-1, driverApi->submitCommandBuffer(&exec));

    struct drm_ivpu_param param = {};
    param.param = DRM_IVPU_PARAM_DEVICE_ID;
    EXPECT_EQ(-1, driverApi->getDeviceParam(&param));

    struct drm_ivpu_bo_wait args = {};
    EXPECT_EQ(-1, driverApi->wait(&args));

    mockOsInfc.kmdIoctlRetCode = ENOMEM;
    struct drm_ivpu_bo_userptr pin = {};
    EXPECT_EQ(-1, driverApi->createBufferFromUserPointer(&pin));
}
