/*
 * Copyright (C) 2022 Intel Corporation
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

std::vector<std::unique_ptr<VPUDevice>> DeviceFactory::createDevices(OsInterface *osi) {
    std::vector<std::unique_ptr<VPUDevice>> devices;
    std::string devPrefix;
    std::string devPath;
    int maxMinor;
    int minMinor;

    if (std::filesystem::exists("/sys/class/accel")) {
        devPrefix = "/dev/accel/accel";
        minMinor = 0;
    } else {
        devPrefix = "/dev/dri/renderD";
        minMinor = 128;
    }
    maxMinor = minMinor + 63;

    for (int minor = minMinor; minor <= maxMinor; minor++) {
        devPath = devPrefix + std::to_string(minor);
        if (!osi->fileExists(devPath)) {
            continue;
        }
        auto device = std::make_unique<VPUDevice>(devPath, *osi);
        if (!device->init()) {
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
