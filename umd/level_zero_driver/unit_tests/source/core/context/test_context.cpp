/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gtest/gtest.h"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <level_zero/ze_api.h>
#include <memory>

namespace L0 {
namespace ult {

struct ContextTest : public Test<DeviceFixture> {
    ze_context_handle_t hContext = nullptr;
    ze_context_desc_t desc = {};
};

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
