/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_37xx/vpu_hw_37xx.hpp"

#include "api/vpu_nnrt_api_37xx.h"
#include "vpux_driver_compiler.h"

#include <vector>
#include <string>

namespace VPU {

static bool getCopyCommandDescriptor37xx(VPUDeviceContext *ctx,
                                         const void *src,
                                         void *dst,
                                         size_t size,
                                         VPUDescriptor &desc) {
    return VPUCopyCommand::fillDescriptor<vpu_cmd_copy_descriptor_37xx_t>(ctx,
                                                                          src,
                                                                          dst,
                                                                          size,
                                                                          desc);
}

static void printCopyDescriptor37xx(void *desc, vpu_cmd_header_t *cmd) {
    VPUCopyCommand::printCopyDesc<vpu_cmd_copy_descriptor_37xx_t>(desc, cmd);
}

struct VPUHwInfo vpuHwInfo37xx = {.compilerPlatform = VCL_PLATFORM_VPU3720,
                                  .platformName = "37xx",
                                  .physicalEUSimdWidth = 4096,
                                  .nExecUnits = 4096,
                                  .numSubslicesPerSlice = 2,
                                  .tileFuseMask = 0x3,
                                  .extraDmaDescriptorSize = 16,
                                  .fwMappedInferenceIndex = VPU_NNRT_37XX_API_VER_INDEX,
                                  .fwMappedInferenceVersion = VPU_NNRT_37XX_API_VER,
                                  .fwTimestampType = VPU_TIME_RAW,
                                  .getCopyCommand = &getCopyCommandDescriptor37xx,
                                  .printCopyDescriptor = &printCopyDescriptor37xx};

VPUHwInfo getHwInfo37xx() {
    return vpuHwInfo37xx;
}
} // namespace VPU
