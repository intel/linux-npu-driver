/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"

#include "gtest/gtest.h"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

namespace L0 {
namespace ult {

struct ContextTest : public Test<DeviceFixture> {
    ze_context_handle_t hContext = nullptr;
    ze_context_desc_t desc = {};
};

TEST_F(ContextTest, contextReturnsFailureWhenInvalidParamsPassed) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, zeContextCreate(nullptr, &desc, &hContext));

    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, zeContextDestroy(nullptr));

    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, zeContextGetStatus(nullptr));
}

TEST_F(ContextTest, contextGetStatusReturned) {
    ze_result_t res = driverHandle->createContext(&desc, &hContext);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);

    L0::Context *context = L0::Context::fromHandle(hContext);

    EXPECT_EQ(context->getStatus(), ZE_RESULT_SUCCESS);

    // Testing explicit removal of device
    ASSERT_NE(mockVpuDevice, nullptr);
    mockVpuDevice->mockGetOsInterface().deviceConnected = false;

    EXPECT_EQ(context->getStatus(), ZE_RESULT_ERROR_DEVICE_LOST);

    osInfc.deviceConnected = true;
    EXPECT_EQ(context->getStatus(), ZE_RESULT_SUCCESS);

    context->destroy();
}

TEST_F(ContextTest, whenCreatingAndDestroyingContextThenSuccessIsReturned) {
    ASSERT_EQ(ZE_RESULT_SUCCESS, driverHandle->createContext(&desc, &hContext));

    auto *context = L0::Context::fromHandle(hContext);
    EXPECT_EQ(ZE_RESULT_SUCCESS, context->destroy());
}

} // namespace ult
} // namespace L0
