/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/device/vpu_37xx/vpu_hw_37xx.hpp"
#include "vpu_driver/source/device/vpu_40xx/vpu_hw_40xx.hpp"

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
    int compilerPlatform = -1;
    char platformName[32];
    uint32_t deviceRevision = 0u;
    uint32_t subdeviceId = 0u;
    uint32_t coreClockRate = 0u;
    uint64_t maxMemAllocSize = 0x100000000;
    uint32_t maxHardwareContexts = 1;
    uint32_t maxCommandQueuePriority = 2;
    uint32_t numThreadsPerEU = 1;
    uint32_t physicalEUSimdWidth = 0u;
    uint32_t nExecUnits = 0u;
    uint32_t numSubslicesPerSlice = 0u;
    uint32_t platformType = 0u;
    uint32_t tileFuseMask = 0u;
    /* Each set bit in tileConfig represents enabled tile */
    uint32_t tileConfig = 0u;
    /* timer resolution in cycles per second */
    uint64_t timerResolution = 38'400'000;

    char name[256] = "Intel(R) AI Boost";

    uint64_t baseLowAddress = 0;

    uint32_t extraDmaDescriptorSize = 0;
    uint32_t fwMappedInferenceIndex = 0;
    uint64_t fwMappedInferenceVersion = 0;
    uint32_t fwTimestampType = 0;

    bool metricStreamerCapability = false;
    bool dmaMemoryRangeCapability = false;

    GetCopyCommand *getCopyCommand = nullptr;
    PrintCopyDescriptor *printCopyDescriptor = nullptr;
};

inline VPUHwInfo getHwInfoByDeviceId(uint32_t deviceId) {
    switch (deviceId) {
    case PCI_DEVICE_ID_MTL:
    case PCI_DEVICE_ID_ARL:
        return getHwInfo37xx();
    case PCI_DEVICE_ID_LNL:
        return getHwInfo40xx();
    }
    throw std::runtime_error("Unrecognized PCI device ID");
}

} // namespace VPU
