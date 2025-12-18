/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/device/vpu_50xx/vpu_hw_50xx.hpp"

#include <cstddef> // IWYU pragma: keep
#include <cstdint> // IWYU pragma: keep

#include "api/vpu_jsm_job_cmd_api.h"
#include "api/vpu_nnrt_api_40xx.h"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_40xx/vpu_hw_40xx.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace VPU {

VPUHwInfo getHwInfo50xx(uint32_t deviceId) {
    VPUHwInfo info = {.platformName = "50xx",
                      .npuArch = NPU50XX,
                      .physicalEUSimdWidth = 8192,
                      .nExecUnits = 12288,
                      .tileFuseMask = 0u,
                      .timerResolution = PERF_FREQUENCY_DEFAULT_HZ / 2,
                      .fwMappedInferenceVersion = VPU_NNRT_40XX_API_VER,
                      .fwTimestampType = VPU_TIME_SYSTIME,
                      .getCopyCommand = getHwInfo40xx().getCopyCommand,
                      .printCopyDescriptor = getHwInfo40xx().printCopyDescriptor,
                      .fwMappedInferenceIndex = VPU_NNRT_40XX_API_VER_INDEX,
                      .fwJsmCmdApiVerIndex = VPU_JSM_JOB_CMD_API_VER_INDEX};

    switch (deviceId) {
    case PCI_DEVICE_ID_PTL_P:
        info.tileFuseMask = 0x7;
        break;
    default:
        LOG_W("Unknown platform");
    }
    return info;
}
} // namespace VPU
