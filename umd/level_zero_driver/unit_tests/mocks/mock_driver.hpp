/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/source/driver.hpp"
#include "level_zero_driver/unit_tests/mocks/mock.hpp"

namespace L0 {
namespace ult {

template <>
struct Mock<::L0::Driver> : public ::L0::Driver {};

using Driver = Mock<::L0::Driver>;

template <>
struct Mock<Driver> : public Driver {
    Mock() { Driver::setInstanceForTesting(this); }
    ~Mock() override { Driver::setInstanceForTesting(nullptr); }

    void setOsInferface(VPU::OsInterface *value) { osInfc = value; }
    void setMetrics(bool value) { envVariables.metrics = value; }
    void setAffinityMask(std::string_view value) { envVariables.affinityMask = value; }
    void setPciDeviceOrder(bool value) { envVariables.pciIdDeviceOrder = value; }
    void setSharedForceDeviceAlloc(bool value) { envVariables.sharedForceDeviceAlloc = value; }
    void initializeEnvVariables() { Driver::initializeEnvVariables(); }
    void initializeLogging() { Driver::initializeLogging(); }
};

} // namespace ult
} // namespace L0
