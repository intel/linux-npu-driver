/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "gtest/gtest.h"

#include "vpu_driver/unit_tests/test_macros/test.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/unit_tests/mocks/mock_driver.hpp"

namespace L0 {
namespace ult {

struct DeviceFixture {
    virtual void SetUp() {
        driver.setMetrics(enableMetrics);
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeInit(0));

        auto vpuDevice = VPU::MockVPUDevice::createWithDefaultHardwareInfo(osInfc);
        mockVpuDevice = vpuDevice.get();
        std::vector<std::unique_ptr<VPU::VPUDevice>> devices;
        devices.push_back(std::move(vpuDevice));

        driverHandle->initialize(std::move(devices));
        device = driverHandle->devices[0];
    }

    virtual void TearDown() {}

    Mock<Driver> driver;
    std::unique_ptr<Mock<L0::DriverHandle>> driverHandle =
        std::make_unique<Mock<L0::DriverHandle>>();
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

        driverHandle = std::make_unique<Mock<L0::DriverHandle>>();
        ze_result_t res = driverHandle->initialize(std::move(devices));
        EXPECT_EQ(ZE_RESULT_SUCCESS, res);
    }

    virtual void TearDown() {}
    Mock<Driver> driver;

    std::unique_ptr<Mock<L0::DriverHandle>> driverHandle;
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

    template <typename F>
    uint32_t getQueueOrdinal(F f) {
        // Assume cmdQueGrpProps has all properties.
        LOG_V("Number of queue groups: %u", queGrpCnt);
        for (uint32_t i = 0; i < queGrpCnt; i++) {
            if (f(cmdQueGrpProps[i].flags)) {
                LOG_V("Engine group ordinal: %u (flags: %#x)", i, cmdQueGrpProps[i].flags);
                return i;
            }
        }
        LOG_E("Failed to get matching queue group.");
        return 0xffffffff;
    }

    uint32_t getComputeQueueOrdinal() {
        return getQueueOrdinal([](uint32_t flag) {
            if ((flag & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE) &&
                (flag & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COPY)) {
                return true;
            }
            return false;
        });
    }

    uint32_t getCopyOnlyQueueOrdinal() {
        return getQueueOrdinal([](uint32_t flag) {
            if ((flag & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE) == 0 &&
                (flag & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COPY)) {
                return true;
            }
            return false;
        });
    }

    // 1 NN, 1 Copy queue groups.
    uint32_t queGrpCnt = 0u;
    ze_command_queue_group_properties_t *cmdQueGrpProps = nullptr;
};

} // namespace ult
} // namespace L0
