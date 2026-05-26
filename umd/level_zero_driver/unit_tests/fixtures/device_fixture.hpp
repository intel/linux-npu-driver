/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>

#include "gtest/gtest.h"
#include "level_zero_driver/source/cmdlist.hpp"
#include "level_zero_driver/source/cmdqueue.hpp"
#include "level_zero_driver/source/context.hpp"
#include "level_zero_driver/source/device.hpp"
#include "level_zero_driver/source/driver.hpp"
#include "level_zero_driver/source/driver_handle.hpp"
#include "level_zero_driver/source/ext/disk_cache.hpp"
#include "level_zero_driver/unit_tests/mocks/mock_driver.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include <dlfcn.h>
#include <filesystem>
#include <memory>
#include <optional>
#include <stdlib.h>
#include <string>
#include <utility>
#include <vector>
#include <ze_api.h>

namespace L0 {
namespace ult {

static inline std::filesystem::path getBinaryDir() {
    Dl_info info = {};
    int ret = dladdr(reinterpret_cast<void *>(getBinaryDir), &info);
    if (ret == 0) {
        return {};
    }
    return std::filesystem::path(info.dli_fname).parent_path();
}

struct DeviceFixture {
    virtual void SetUp() {
        for (auto &envVar : fixtureEnvVariables) {
            char *currentVal = getenv(envVar.name.c_str());
            if (currentVal != nullptr) {
                envVar.prevValue = std::string(currentVal);
            }

            setenv(envVar.name.c_str(), envVar.value.c_str(), 1);
        }

        // Set path to compiler library to be loaded by driver in unit tests
        auto binaryDir = getBinaryDir();
        if (!binaryDir.empty()) {
            binaryDir /= "../_deps/npu_compiler_package-src/lib";
            setenv("NPU_ALT_DEPENDENCY_PATH", binaryDir.c_str(), 0);
        }

        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::init(0));
        driver.initializeEnvVariables();
        driver.diskCache = std::make_unique<DiskCache>(osInfc);

        auto vpuDevice = VPU::MockVPUDevice::createWithDefaultHardwareInfo(osInfc);
        mockVpuDevice = vpuDevice.get();
        std::vector<std::unique_ptr<VPU::VPUDevice>> devices;
        devices.push_back(std::move(vpuDevice));

        driverHandle = std::make_unique<L0::DriverHandle>(std::move(devices));
        device = driverHandle->devices[0].get();
    }

    virtual void TearDown() {
        for (const auto &envVar : fixtureEnvVariables) {
            if (envVar.prevValue.has_value()) {
                setenv(envVar.name.c_str(), envVar.prevValue->c_str(), 1);
            } else {
                unsetenv(envVar.name.c_str());
            }
        }
    }

    void setTestEnvironmentVar(std::string name, std::string value) {
        fixtureEnvVariables.push_back({std::move(name), std::move(value), std::nullopt});
    }

    Mock<Driver> driver;
    std::unique_ptr<L0::DriverHandle> driverHandle;
    L0::Device *device = nullptr;

    VPU::MockVPUDevice *mockVpuDevice = nullptr;
    VPU::MockOsInterfaceImp osInfc;
    using TestEnvVar = struct {
        std::string name;
        std::string value;
        std::optional<std::string> prevValue;
    };
    std::vector<TestEnvVar> fixtureEnvVariables = {};
};

struct MultiDeviceFixture {
    virtual void SetUp() {
        driver.setMetrics(true);
        std::vector<std::unique_ptr<VPU::VPUDevice>> devices;
        for (uint32_t i = 0; i < numDevices; i++) {
            auto mockDevice = VPU::MockVPUDevice::createWithDefaultHardwareInfo(osInfc);
            devices.push_back(std::move(mockDevice));
        }

        driverHandle = std::make_unique<L0::DriverHandle>(std::move(devices));
    }

    virtual void TearDown() {}
    Mock<Driver> driver;

    std::unique_ptr<L0::DriverHandle> driverHandle;
    const uint32_t numDevices = 4u;
    VPU::MockOsInterfaceImp osInfc;
};

struct DeviceFixtureWithoutMetrics : public DeviceFixture {};

struct DeviceFixtureWithMetrics : public DeviceFixture {
    void SetUp() override {
        DeviceFixture::setTestEnvironmentVar("ZET_ENABLE_METRICS", "1");
        DeviceFixture::SetUp();
    }
    void TearDown() override { DeviceFixture::TearDown(); }
};

struct ContextFixture : DeviceFixture {
    void SetUp() override {
        DeviceFixture::SetUp();

        ze_context_handle_t hContext = {};
        ze_context_desc_t desc = {};
        desc.stype = ZE_STRUCTURE_TYPE_CONTEXT_DESC;
        ze_result_t res = driverHandle->createContext(&desc, &hContext);
        EXPECT_EQ(ZE_RESULT_SUCCESS, res);
        ASSERT_NE(nullptr, hContext);

        context = L0::Context::fromHandle(hContext);
        ctx = context->getDeviceContext();
    }

    void TearDown() override {
        EXPECT_EQ(ctx->getBuffersCount(), 0u);
        if (context)
            context->destroy();
        DeviceFixture::TearDown();
    }

    ze_command_list_handle_t createCommandList() {
        ze_command_list_handle_t hCommandList = nullptr;
        ze_command_list_desc_t desc = {};
        desc.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC;

        ze_result_t result = L0::CommandList::create(context, device, &desc, &hCommandList);
        EXPECT_EQ(ZE_RESULT_SUCCESS, result);
        return hCommandList;
    }

    ze_command_queue_handle_t createCommandQueue() {
        ze_command_queue_handle_t hCommandQueue = nullptr;
        ze_command_queue_desc_t desc = {};
        desc.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC;

        ze_result_t result = L0::CommandQueue::create(context, device, &desc, &hCommandQueue);
        EXPECT_EQ(ZE_RESULT_SUCCESS, result);
        return hCommandQueue;
    }

    L0::Context *context = nullptr;
    VPU::VPUDeviceContext *ctx = nullptr;
};

struct ContextFixtureWithMetrics : ContextFixture {
    void SetUp() override {
        DeviceFixture::setTestEnvironmentVar("ZET_ENABLE_METRICS", "1");
        ContextFixture::SetUp();
    }
    void TearDown() override { ContextFixture::TearDown(); }
};

struct CommandQueueFixture : ContextFixture {
    void SetUp() override {
        ContextFixture::SetUp();

        device->getCommandQueueGroupProperties(&queGrpCnt, nullptr);
        ASSERT_NE(0u, queGrpCnt);
        cmdQueGrpProps = new ze_command_queue_group_properties_t[queGrpCnt];
        ASSERT_NE(nullptr, cmdQueGrpProps);
        device->getCommandQueueGroupProperties(&queGrpCnt, cmdQueGrpProps);
    }

    void TearDown() override {
        if (cmdQueGrpProps != nullptr) {
            delete[] cmdQueGrpProps;
        }

        ContextFixture::TearDown();
    }

    uint32_t queGrpCnt = 0u;
    ze_command_queue_group_properties_t *cmdQueGrpProps = nullptr;
};

struct CommandQueueFixtureWithMetrics : CommandQueueFixture {
    void SetUp() override {
        DeviceFixture::setTestEnvironmentVar("ZET_ENABLE_METRICS", "1");
        CommandQueueFixture::SetUp();
    }
    void TearDown() override { CommandQueueFixture::TearDown(); }
};

} // namespace ult
} // namespace L0
