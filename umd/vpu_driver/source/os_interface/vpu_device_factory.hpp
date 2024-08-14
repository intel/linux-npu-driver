/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/device/vpu_device.hpp"

#include <memory>
#include <vector>

namespace VPU {
class OsInterface;

class DeviceFactory {
  public:
    static std::vector<std::unique_ptr<VPUDevice>> createDevices(OsInterface *osi,
                                                                 bool enableMetrics);
};

} // namespace VPU
