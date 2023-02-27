/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"
#include "gtest/gtest.h"

#include <uapi/drm/ivpu_accel.h>

using namespace VPU;

struct VPUBufferObjectTest : public ::testing::Test {
    void SetUp() {}

    void TearDown() { ASSERT_EQ(ctx->getBuffersCount(), 0u); }

    MockOsInterfaceImp osInfc;
    std::unique_ptr<MockVPUDevice> vpuDevice = MockVPUDevice::createWithDefaultHardwareInfo(osInfc);
    std::shared_ptr<VPUDeviceContext> ctx = vpuDevice->createDeviceContext();
};

TEST_F(VPUBufferObjectTest, createBufferObject) {
    EXPECT_TRUE(VPUBufferObject::create(ctx->getDriverApi(),
                                        VPUBufferObject::Location::Host,
                                        VPUBufferObject::Type::CachedLow,
                                        4096) != nullptr);
}

TEST_F(VPUBufferObjectTest, supportRangeTestForGivenArgument) {
    auto bo = VPUBufferObject::create(ctx->getDriverApi(),
                                      VPUBufferObject::Location::Host,
                                      VPUBufferObject::Type::CachedLow,
                                      4096);
    uint8_t *ptr = bo->getBasePointer();
    EXPECT_TRUE(bo->isInRange(ptr));

    ptr = ptr + 4094;
    EXPECT_TRUE(bo->isInRange(ptr));

    ptr = ptr + 4096;
    EXPECT_FALSE(bo->isInRange(ptr));
}

TEST_F(VPUBufferObjectTest, copyToBufferMethodExpectSuccess) {
    size_t size = 256;
    std::vector<uint8_t> data(size, 0xde);

    auto bo = VPUBufferObject::create(ctx->getDriverApi(),
                                      VPUBufferObject::Location::Host,
                                      VPUBufferObject::Type::CachedLow,
                                      data.size());
    EXPECT_TRUE(bo->copyToBuffer(data.data(), data.size(), 0));

    EXPECT_EQ(memcmp(data.data(), bo->getBasePointer(), bo->getAllocSize()), 0);
}

TEST_F(VPUBufferObjectTest, copyToBufferOutsideRangeExpectFailure) {
    size_t size = 256;
    std::vector<uint8_t> data(size, 0xde);

    auto bo = VPUBufferObject::create(ctx->getDriverApi(),
                                      VPUBufferObject::Location::Host,
                                      VPUBufferObject::Type::CachedLow,
                                      size);
    EXPECT_FALSE(bo->copyToBuffer(data.data(), data.size(), size / 2));
}

TEST_F(VPUBufferObjectTest, copyToBufferGreaterThenBufferSizeExpectFailure) {
    size_t size = 256;
    std::vector<uint8_t> data(size * 2, 0xde);

    auto bo = VPUBufferObject::create(ctx->getDriverApi(),
                                      VPUBufferObject::Location::Host,
                                      VPUBufferObject::Type::CachedLow,
                                      size);
    EXPECT_FALSE(bo->copyToBuffer(data.data(), data.size(), 0));
}

TEST_F(VPUBufferObjectTest, copyToBufferNullptrDataExpectFailure) {
    size_t size = 256;
    std::vector<uint8_t> memory(size);

    auto bo = VPUBufferObject::create(ctx->getDriverApi(),
                                      VPUBufferObject::Location::Host,
                                      VPUBufferObject::Type::CachedLow,
                                      size);
    EXPECT_FALSE(bo->copyToBuffer(nullptr, size, 0));
}
