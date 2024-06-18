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

class MockVPUDeviceContext : public VPUDeviceContext {
  public:
    MockVPUDeviceContext(std::unique_ptr<VPUDriverApi> drvApi, VPUHwInfo *info)
        : VPUDeviceContext(std::move(drvApi), info) {}
    MockVPUDeviceContext() = delete;
    inline void *
    createHostMemAlloc(size_t size,
                       VPUBufferObject::Type type = VPUBufferObject::Type::CachedShave) {
        return createMemAlloc(size, type, VPUBufferObject::Location::Host);
    };

    inline void *
    createDeviceMemAlloc(size_t size,
                         VPUBufferObject::Type type = VPUBufferObject::Type::WriteCombineFw) {
        return createMemAlloc(size, type, VPUBufferObject::Location::Device);
    };

    inline void *
    createSharedMemAlloc(size_t size,
                         VPUBufferObject::Type type = VPUBufferObject::Type::CachedFw) {
        return createMemAlloc(size, type, VPUBufferObject::Location::Shared);
    };
};

class MockVPUDevice : public VPUDevice {
  private:
    MockOsInterfaceImp &mockOSInf;

  public:
    MockVPUDevice(std::string devnode, MockOsInterfaceImp &mockOSInf);

    static std::unique_ptr<MockVPUDevice>
    createWithDefaultHardwareInfo(MockOsInterfaceImp &mockOSInf);

    std::unique_ptr<MockVPUDeviceContext> createMockDeviceContext();

    MockOsInterfaceImp &mockGetOsInterface() { return mockOSInf; }
};

} // namespace VPU
