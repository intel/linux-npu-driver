/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "vpu_driver/source/os_interface/vpu_device_factory.hpp"
#include "vpu_driver/unit_tests/mocks/gmock_os_interface_imp.hpp"

#include <memory>
#include <vector>

using namespace VPU;

TEST(DeviceFactoryTest, devicesDiscoveredWithUDevFuncs) {
    // Gmock os interface imp.
    GMockOsInterfaceImp gmockInfc;

    // Set expected returns.
    EXPECT_CALL(gmockInfc, osiOpen).Times(64);

    EXPECT_CALL(gmockInfc, osiIoctl).Times(64);

    // Devices vector will be returned upon number of discovered devices.
    auto devVector = DeviceFactory::createDevices(&gmockInfc, true);
    EXPECT_EQ(0u, devVector.size());
}
