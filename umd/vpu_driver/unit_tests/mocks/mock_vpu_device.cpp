/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/device/devicelist.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include <limits>
#include <memory>
#include <string>
#include <cassert>

namespace VPU {

MockVPUDevice::MockVPUDevice(std::string devnode, MockOsInterfaceImp &mockOSInf)
    : VPUDevice(devnode, mockOSInf)
    , mockOSInf(mockOSInf) {}

std::unique_ptr<MockVPUDevice>
MockVPUDevice::createWithDefaultHardwareInfo(MockOsInterfaceImp &mockOSInf) {
    auto device = std::make_unique<MockVPUDevice>(FAKE_TEST_DEV_NODE, mockOSInf);
    device->init();
    return device;
};

} // namespace VPU
