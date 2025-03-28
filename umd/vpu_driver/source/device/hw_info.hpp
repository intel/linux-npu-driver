/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>

#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/device/vpu_37xx/vpu_hw_37xx.hpp"
#include "vpu_driver/source/device/vpu_40xx/vpu_hw_40xx.hpp"

#include <array>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

constexpr uint16_t INTEL_PCI_VENDOR_ID = 0x8086;
constexpr uint64_t PERF_FREQUENCY_DEFAULT_HZ = 38'400'000;

namespace VPU {

enum NPUArch { NPUUNKNOWN = 0, NPU37XX, NPU40XX };

using GetCopyCommand = bool(uint64_t, uint64_t, size_t, VPUDescriptor &);
using PrintCopyDescriptor = void(void *, vpu_cmd_header_t *);

struct VPUHwInfo {
    uint32_t deviceId = 0u;
    char platformName[32];
    NPUArch npuArch = NPUUNKNOWN;
    uint16_t deviceRevision = UINT16_MAX;
    uint32_t subdeviceId = 0u;
    uint32_t coreClockRate = 0u;
    uint32_t maxHardwareContexts = 1;
    uint32_t maxCommandQueuePriority = 2;
    uint32_t physicalEUSimdWidth = 0u;
    uint32_t nExecUnits = 0u;
    uint32_t platformType = 0u;
    uint32_t tileFuseMask = 0u;
    /* Each set bit in tileConfig represents enabled tile */
    uint32_t tileConfig = 0u;
    uint64_t timerResolution = PERF_FREQUENCY_DEFAULT_HZ;

    char name[256] = "Intel(R) AI Boost";

    uint32_t extraDmaDescriptorSize = 0;
    uint64_t fwMappedInferenceVersion = 0;
    uint32_t fwTimestampType = 0;

    bool metricStreamerCapability = false;
    bool dmaMemoryRangeCapability = false;
    bool primeBuffersCapability = false;
    bool cmdQueueCreationCapability = false;

    GetCopyCommand *getCopyCommand = nullptr;
    PrintCopyDescriptor *printCopyDescriptor = nullptr;

    uint32_t fwMappedInferenceIndex = UINT32_MAX;
    uint32_t fwJsmCmdApiVerIndex = UINT32_MAX;
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
