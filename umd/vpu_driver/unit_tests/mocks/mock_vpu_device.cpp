/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"

#include <memory>
#include <stdexcept>
#include <string>

namespace VPU {

MockVPUDevice::MockVPUDevice(std::string devnode, MockOsInterfaceImp &mockOSInf)
    : VPUDevice(std::move(devnode), mockOSInf)
    , mockOSInf(mockOSInf) {}

std::unique_ptr<MockVPUDevice>
MockVPUDevice::createWithDefaultHardwareInfo(MockOsInterfaceImp &mockOSInf) {
    auto device = std::make_unique<MockVPUDevice>(FAKE_TEST_DEV_NODE, mockOSInf);
    if (!device->init(true))
        throw std::runtime_error("Failed to initialize MockVPUDevice");
    return device;
};
std::unique_ptr<MockVPUDeviceContext> MockVPUDevice::createMockDeviceContext() {
    auto drvApi = VPUDriverApi::openDriverApi(devPath, osInfc);
    if (drvApi == nullptr) {
        LOG(UTEST, "Failed to allocate VPUDriverApi");
        return nullptr;
    }

    return std::make_unique<MockVPUDeviceContext>(std::move(drvApi), &hwInfo);
}
} // namespace VPU
