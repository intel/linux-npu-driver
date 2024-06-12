/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include "api/vpu_nnrt_api_40xx.h"
#include "vpux_driver_compiler.h"

#include <vector>
#include <string>

namespace VPU {

static bool getCopyCommandDescriptor40xx(VPUDeviceContext *ctx,
                                         const void *src,
                                         void *dst,
                                         size_t size,
                                         VPUDescriptor &desc) {
    return VPUCopyCommand::fillDescriptor<vpu_cmd_copy_descriptor_40xx_t>(ctx,
                                                                          src,
                                                                          dst,
                                                                          size,
                                                                          desc);
}

static void printCopyDescriptor40xx(void *desc, vpu_cmd_header_t *cmd) {
    VPUCopyCommand::printCopyDesc<vpu_cmd_copy_descriptor_40xx_t>(desc, cmd);
}

struct VPUHwInfo vpuHwInfo40xx = {.compilerPlatform = VCL_PLATFORM_VPU4000,
                                  .platformName = "40xx",
                                  .physicalEUSimdWidth = 4096,
                                  .nExecUnits = 12288,
                                  .numSubslicesPerSlice = 6,
                                  .tileFuseMask = 0x3F,
                                  .fwMappedInferenceIndex = VPU_NNRT_40XX_API_VER_INDEX,
                                  .fwMappedInferenceVersion = VPU_NNRT_40XX_API_VER,
                                  .fwTimestampType = VPU_TIME_SYSTIME,
                                  .getCopyCommand = &getCopyCommandDescriptor40xx,
                                  .printCopyDescriptor = &printCopyDescriptor40xx};

VPUHwInfo getHwInfo40xx() {
    return vpuHwInfo40xx;
}

} // namespace VPU
