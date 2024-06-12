/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/os_interface/vpu_device_factory.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"

#include <filesystem>
#include <memory>
#include <vector>
#include <string>

namespace VPU {

std::vector<std::unique_ptr<VPUDevice>> DeviceFactory::createDevices(OsInterface *osi,
                                                                     bool enableMetrics) {
    std::vector<std::unique_ptr<VPUDevice>> devices;
    std::string devPath;

    constexpr std::string_view devPrefix = "/dev/accel/accel";
    int minMinor = 0;
    int maxMinor = minMinor + 63;

    for (int minor = minMinor; minor <= maxMinor; minor++) {
        devPath = std::string(devPrefix) + std::to_string(minor);
        auto device = std::make_unique<VPUDevice>(devPath, *osi);
        if (!device->init(enableMetrics)) {
            continue;
        }
        devices.push_back(std::move(device));
    }

    if (!devices.size()) {
        LOG_E("Failed to detect any VPU device");
    }

    return devices;
}

} // namespace VPU
