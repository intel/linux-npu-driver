/*
 * Copyright (C) 2022-2024 Intel Corporation
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

#include <level_zero/ze_api.h>
#include <memory>
#include <utility>
#include <vector>

namespace L0 {
namespace ult {

struct DeviceFixture {
    virtual void SetUp() {
        driver.setMetrics(enableMetrics);
        ASSERT_EQ(ZE_RESULT_SUCCESS, L0::init(0));

        driver.diskCache = std::make_unique<DiskCache>(osInfc);

        auto vpuDevice = VPU::MockVPUDevice::createWithDefaultHardwareInfo(osInfc);
        mockVpuDevice = vpuDevice.get();
        std::vector<std::unique_ptr<VPU::VPUDevice>> devices;
        devices.push_back(std::move(vpuDevice));

        driverHandle = std::make_unique<L0::DriverHandle>(std::move(devices));
        device = driverHandle->devices[0].get();
    }

    virtual void TearDown() {}

    Mock<Driver> driver;
    std::unique_ptr<L0::DriverHandle> driverHandle;
    L0::Device *device = nullptr;

    VPU::MockVPUDevice *mockVpuDevice = nullptr;
    VPU::MockOsInterfaceImp osInfc;
    bool enableMetrics = true;
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

struct DeviceFixtureWithoutEnvVariables : DeviceFixture {
    void SetUp() override {
        enableMetrics = false;
        DeviceFixture::SetUp();
    }

    void TearDown() override { enableMetrics = true; }
};

struct ContextFixture : DeviceFixture {
    void SetUp() override {
        DeviceFixture::SetUp();

        ze_context_handle_t hContext = {};
        ze_context_desc_t desc;
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
    L0::Context *context = nullptr;
    VPU::VPUDeviceContext *ctx = nullptr;
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

    ze_command_list_handle_t createCommandList() {
        ze_command_list_handle_t hCommandList = nullptr;
        ze_command_list_desc_t desc = {};

        ze_result_t result = L0::CommandList::create(context, device, &desc, &hCommandList);
        EXPECT_EQ(ZE_RESULT_SUCCESS, result);
        return hCommandList;
    }

    ze_command_queue_handle_t createCommandQueue() {
        ze_command_queue_handle_t hCommandQueue = nullptr;
        ze_command_queue_desc_t desc = {};

        ze_result_t result = L0::CommandQueue::create(context, device, &desc, &hCommandQueue);
        EXPECT_EQ(ZE_RESULT_SUCCESS, result);
        return hCommandQueue;
    }

    uint32_t queGrpCnt = 0u;
    ze_command_queue_group_properties_t *cmdQueGrpProps = nullptr;
};

} // namespace ult
} // namespace L0
