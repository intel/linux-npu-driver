/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>

#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"

#include <memory>
#include <string>
#include <utility>

namespace VPU {
class MockOsInterfaceImp;
struct VPUHwInfo;
} // namespace VPU

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

    inline std::shared_ptr<VPUBufferObject>
    createHostMemAlloc(size_t size,
                       VPUBufferObject::Type type = VPUBufferObject::Type::CachedShave) {
        auto ptr = createMemAlloc(size, type, VPUBufferObject::Location::Host);
        return ptr ? findBufferObject(ptr) : nullptr;
    };

    inline std::shared_ptr<VPUBufferObject>
    createDeviceMemAlloc(size_t size,
                         VPUBufferObject::Type type = VPUBufferObject::Type::WriteCombineFw) {
        auto ptr = createMemAlloc(size, type, VPUBufferObject::Location::Device);
        return ptr ? findBufferObject(ptr) : nullptr;
    };

    inline std::shared_ptr<VPUBufferObject>
    createSharedMemAlloc(size_t size,
                         VPUBufferObject::Type type = VPUBufferObject::Type::CachedFw) {
        auto ptr = createMemAlloc(size, type, VPUBufferObject::Location::Shared);
        return ptr ? findBufferObject(ptr) : nullptr;
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
