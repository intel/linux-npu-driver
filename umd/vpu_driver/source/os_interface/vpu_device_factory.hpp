/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"

#include <memory>

namespace VPU {

class DeviceFactory {
  public:
    static std::vector<std::unique_ptr<VPUDevice>> createDevices(OsInterface *osi);
};

} // namespace VPU
