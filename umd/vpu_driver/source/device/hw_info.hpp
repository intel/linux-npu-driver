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

    bool isIntegrated = true;
    bool isSubdevice = false;
    bool isSupportEcc = false;
    bool isSupportOnDemandPaging = false;

    char name[256] = "Intel VPU";

    uint64_t baseLowAddres = 0;

    GetCopyCommand *getCopyCommand = nullptr;
    PrintCopyDescriptor *printCopyDescriptor = nullptr;
};

#ifndef VPU_NEXT_GEN_HW
#define VPU_NEXT_GEN_HW
#else
extern VPUHwInfo VPU_NEXT_GEN_HW;
#endif

extern VPUHwInfo mtlHwInfo;
const VPUHwInfo VPUHwInfos[] = {mtlHwInfo, VPU_NEXT_GEN_HW};

} // namespace VPU
