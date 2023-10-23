/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"

#include <string>
#include <memory>
#include <uapi/drm/ivpu_accel.h>

// MOCK CAPS.
#define FAKE_TEST_DEV_NODE "dev/node/test"
#define MOCKCAP_FAKE_VALUE 0x12341234
#define MOCKCAP_FAKE_UUID_IDX3_VALUE 127
#define MOCKCAP_FAKE_STR "A mock string"

namespace VPU {

class MockVPUDevice : public VPUDevice {
  private:
    MockOsInterfaceImp &mockOSInf;

  public:
    MockVPUDevice(std::string devnode, MockOsInterfaceImp &mockOSInf);

    static std::unique_ptr<MockVPUDevice>
    createWithDefaultHardwareInfo(MockOsInterfaceImp &mockOSInf);

    MockOsInterfaceImp &mockGetOsInterface() { return mockOSInf; }
};

} // namespace VPU
