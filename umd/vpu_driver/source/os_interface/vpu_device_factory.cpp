/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/os_interface/vpu_device_factory.hpp"

#include "vpu_driver/source/utilities/log.hpp"

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace VPU {
class OsInterface;

std::vector<std::unique_ptr<VPUDevice>> DeviceFactory::createDevices(OsInterface *osi,
                                                                     bool enableMetrics) {
    std::vector<std::unique_ptr<VPUDevice>> devices;
    std::string devPath;

#ifdef ANDROID
    constexpr std::string_view devPrefix = "/dev/accel";
#else
    constexpr std::string_view devPrefix = "/dev/accel/accel";
#endif
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
