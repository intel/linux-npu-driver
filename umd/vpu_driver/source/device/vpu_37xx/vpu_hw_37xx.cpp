/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/device/vpu_37xx/vpu_hw_37xx.hpp"

#include <cstddef>
#include <cstdint> // IWYU pragma: keep

#include "api/vpu_jsm_job_cmd_api.h"
#include "api/vpu_nnrt_api_37xx.h"
#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/device/hw_info.hpp"

namespace VPU {
struct VPUDescriptor;

static bool
getCopyCommandDescriptor37xx(uint64_t srcAddr, uint64_t dstAddr, size_t size, VPUDescriptor &desc) {
    return VPUCopyCommand::fillDescriptor<vpu_cmd_copy_descriptor_37xx_t>(srcAddr,
                                                                          dstAddr,
                                                                          size,
                                                                          desc);
}

static void printCopyDescriptor37xx(void *desc, vpu_cmd_header_t *cmd) {
    VPUCopyCommand::printCopyDesc<vpu_cmd_copy_descriptor_37xx_t>(desc, cmd);
}

struct VPUHwInfo vpuHwInfo37xx = {.platformName = "37xx",
                                  .npuArch = NPU37XX,
                                  .physicalEUSimdWidth = 4096,
                                  .nExecUnits = 4096,
                                  .tileFuseMask = 0x3,
                                  .extraDmaDescriptorSize = 16,
                                  .fwMappedInferenceVersion = VPU_NNRT_37XX_API_VER,
                                  .fwTimestampType = VPU_TIME_RAW,
                                  .getCopyCommand = &getCopyCommandDescriptor37xx,
                                  .printCopyDescriptor = &printCopyDescriptor37xx,
                                  .fwMappedInferenceIndex = VPU_NNRT_37XX_API_VER_INDEX,
                                  .fwJsmCmdApiVerIndex = VPU_JSM_JOB_CMD_API_VER_INDEX};

VPUHwInfo getHwInfo37xx() {
    return vpuHwInfo37xx;
}
} // namespace VPU
