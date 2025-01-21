/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/source/driver.hpp"
#include "level_zero_driver/unit_tests/mocks/mock.hpp"

#include <atomic>

namespace L0 {
namespace ult {

template <>
struct Mock<::L0::Driver> : public ::L0::Driver {};

using Driver = Mock<::L0::Driver>;

template <>
struct Mock<Driver> : public Driver {
    Mock() = default;
    ~Mock() override { Driver::pDriver = nullptr; }

    void reset(uint32_t numMockDrvCnt = 1, bool doInit = false) {
        initCalledCount = 0;
        bDoInit = doInit;
    }

    void driverInit(ze_init_flags_t flag) override {
        initCalledCount++;

        if (bDoInit)
            Driver::driverInit(flag);
        return;
    }

    ze_result_t getInitStatus() override {
        if (bDoInit)
            return Driver::getInitStatus();

        return ZE_RESULT_SUCCESS;
    }

    void setMetrics(bool value) { envVariables.metrics = value; }
    void setAffinityMask(std::string_view value) { envVariables.affinityMask = value; }
    void setPciDeviceOrder(bool value) { envVariables.pciIdDeviceOrder = value; }
    void setSharedForceDeviceAlloc(bool value) { envVariables.sharedForceDeviceAlloc = value; }
    void initializeEnvVariables() { Driver::initializeEnvVariables(); }
    void initializeLogging() { Driver::initializeLogging(); }

    bool bDoInit = false;
    uint32_t initCalledCount = 0;
};

} // namespace ult
} // namespace L0
