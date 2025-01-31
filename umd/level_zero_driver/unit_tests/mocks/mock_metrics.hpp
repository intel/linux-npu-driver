/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>

#include "level_zero_driver/source/device.hpp"
#include "level_zero_driver/source/metric.hpp"
#include "level_zero_driver/source/metric_query.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace L0 {
struct DriverHandle;

namespace ult {

class MockMetricDevice : public L0::Device {
  public:
    MockMetricDevice(DriverHandle *driverHandle, std::unique_ptr<VPU::VPUDevice> vpuDevice)
        : L0::Device(driverHandle, std::move(vpuDevice)) {}
};

class MockMetricContext : public L0::MetricContext {
  public:
    MockMetricContext(Device *deviceInput)
        : L0::MetricContext(deviceInput) {}
    size_t getActivatedMetricGroupsSize() const { return activatedMetricGroups.size(); }
};

class MockMetricQuery : public L0::MetricQuery {
  public:
    void injectDummyData();
};

} // namespace ult
} // namespace L0
