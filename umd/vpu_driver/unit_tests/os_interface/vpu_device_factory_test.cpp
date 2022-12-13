/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/os_interface/vpu_device_factory.hpp"
#include "vpu_driver/unit_tests/mocks/gmock_os_interface_imp.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace VPU;

TEST(DeviceFactoryTest, devicesDiscoveredWithUDevFuncs) {
    // Gmock os interface imp.
    GMockOsInterfaceImp gmockInfc;

    // Set expected returns.
    EXPECT_CALL(gmockInfc, osiOpen).Times(32);

    EXPECT_CALL(gmockInfc, osiIoctl).Times(32);

    EXPECT_CALL(gmockInfc, fileExists).WillRepeatedly(::testing::ReturnRoundRobin({true, false}));

    // Devices vector will be returned upon number of discovered devices.
    auto devVector = DeviceFactory::createDevices(&gmockInfc);
    EXPECT_EQ(0u, devVector.size());
}
