/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/device/vpu_40xx/vpu_hw_40xx.hpp"

#include <cstddef>
#include <cstdint> // IWYU pragma: keep

#include "api/vpu_jsm_job_cmd_api.h"
#include "api/vpu_nnrt_api_40xx.h"
#include "vpu_driver/source/command/copy_command.hpp"
#include "vpu_driver/source/device/hw_info.hpp"

namespace VPU {
struct VPUDescriptor;

static bool
getCopyCommandDescriptor40xx(uint64_t srcAddr, uint64_t dstAddr, size_t size, VPUDescriptor &desc) {
    return VPUCopyCommand::fillDescriptor<vpu_cmd_copy_descriptor_40xx_t>(srcAddr,
                                                                          dstAddr,
                                                                          size,
                                                                          desc);
}

static void printCopyDescriptor40xx(void *desc, vpu_cmd_header_t *cmd) {
    VPUCopyCommand::printCopyDesc<vpu_cmd_copy_descriptor_40xx_t>(desc, cmd);
}

struct VPUHwInfo vpuHwInfo40xx = {.platformName = "40xx",
                                  .npuArch = NPU40XX,
                                  .physicalEUSimdWidth = 4096,
                                  .nExecUnits = 12288,
                                  .tileFuseMask = 0x3F,
                                  .timerResolution = PERF_FREQUENCY_DEFAULT_HZ / 2,
                                  .fwMappedInferenceVersion = VPU_NNRT_40XX_API_VER,
                                  .fwTimestampType = VPU_TIME_SYSTIME,
                                  .getCopyCommand = &getCopyCommandDescriptor40xx,
                                  .printCopyDescriptor = &printCopyDescriptor40xx,
                                  .fwMappedInferenceIndex = VPU_NNRT_40XX_API_VER_INDEX,
                                  .fwJsmCmdApiVerIndex = VPU_JSM_JOB_CMD_API_VER_INDEX};

VPUHwInfo getHwInfo40xx() {
    return vpuHwInfo40xx;
}

} // namespace VPU
