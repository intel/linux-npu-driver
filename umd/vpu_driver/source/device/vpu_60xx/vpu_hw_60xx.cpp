/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/device/vpu_60xx/vpu_hw_60xx.hpp"

#include <cstddef> // IWYU pragma: keep
#include <cstdint> // IWYU pragma: keep

#include "api/vpu_jsm_api.h"
#include "api/vpu_jsm_job_cmd_api.h"
#include "api/vpu_nnrt_api_ver.h"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_40xx/vpu_hw_40xx.hpp"

namespace VPU {

VPUHwInfo getHwInfo60xx() {
    VPUHwInfo info = {.platformName = "60xx",
                      .npuArch = NPU60XX,
                      .physicalEUSimdWidth = 8192,
                      .nExecUnits = 12288,
                      .tileFuseMask = 0xf,
                      .timerResolution = PERF_FREQUENCY_DEFAULT_HZ / 2,
                      .fwMappedInferenceVersion = VPU_NNRT_40XX_API_VER,
                      .fwTimestampType = VPU_TIME_SYSTIME,
                      .getCopyCommand = getHwInfo40xx().getCopyCommand,
                      .printCopyDescriptor = getHwInfo40xx().printCopyDescriptor,
                      .fwMappedInferenceIndex = VPU_NNRT_API_VER_INDEX,
                      .fwJsmCmdApiVerIndex = VPU_JSM_JOB_CMD_API_VER_INDEX,
                      .fwJsmApiVerIndex = VPU_JSM_API_VER_INDEX};
    return info;
}
} // namespace VPU
