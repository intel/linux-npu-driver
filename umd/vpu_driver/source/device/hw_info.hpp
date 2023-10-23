/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/command/vpu_command.hpp"

#include <array>
#include <functional>
#include <stdint.h>
#include <string>
#include <vector>

constexpr uint16_t INTEL_PCI_VENDOR_ID = 0x8086;

namespace VPU {

using GetCopyCommand = bool(VPUDeviceContext *, const void *, void *, size_t, VPUDescriptor &);
using PrintCopyDescriptor = void(void *, vpu_cmd_header_t *);

struct VPUHwInfo {
    uint32_t deviceId = 0u;
    uint32_t supportedDeviceIds[2] = {0, 0};
    uint32_t numSupportedDevices = 0;
    int compilerPlatform = -1;
    uint32_t deviceRevision = 0u;
    uint32_t subdeviceId = 0u;
    uint32_t coreClockRate = 0u;
    uint64_t maxMemAllocSize = 0x100000000;
    uint32_t maxHardwareContexts = 1;
    uint32_t maxCommandQueuePriority = 2;
    uint32_t numThreadsPerEU = 1;
    uint32_t physicalEUSimdWidth = 32u;
    uint32_t nExecUnits = 0u;
    uint32_t numSubslicesPerSlice = 0u;
    uint32_t platformType = 0u;
    uint32_t tileFuseMask = 0u;
    /* Each set bit in tileConfig represents enabled tile */
    uint32_t tileConfig = 0u;

    char name[256] = "Intel(R) AI Boost";

    uint64_t baseLowAddres = 0;

    GetCopyCommand *getCopyCommand = nullptr;
    PrintCopyDescriptor *printCopyDescriptor = nullptr;

    bool IsDeviceId(uint32_t deviceId) const {
        for (uint32_t i = 0; i < numSupportedDevices; i++) {
            if (deviceId == supportedDeviceIds[i])
                return true;
        }
        return false;
    }
};

extern VPUHwInfo vpuHwInfo37xx;

const VPUHwInfo VPUHwInfos[] = {vpuHwInfo37xx};

} // namespace VPU
