/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/device/device_info.hpp"

#include <vector>
#include <string>

namespace VPU {

struct MTLDeviceInfo : DeviceInfo {
    MTLDeviceInfo() {
        subdeviceId = 0;
        maxMemAllocSize = 0x100000000;
        maxCommandQueuePriority = 2;
        numThreadsPerEU = 1;
        physicalEUSimdWidth = 32;
        nExecUnits = 4096;
        numSubslicesPerSlice = 2;
        isIntegrated = true;
    }
};

} // namespace VPU
