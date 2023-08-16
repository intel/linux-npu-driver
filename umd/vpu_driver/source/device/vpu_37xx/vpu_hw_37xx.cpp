/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/include/umd_common.hpp"
#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/device/hw_info.hpp"

#include <vector>
#include <string>

namespace VPU {

#ifdef ENABLE_VPUX_COMPILER
#include "VPUXCompilerL0.h"
static constexpr int vpuxCompilerPlatform = VCL_PLATFORM_VPU3720;
#else
static constexpr int vpuxCompilerPlatform = -1;
#endif

static bool getCopyCommandDescriptor37xx(VPUDeviceContext *ctx,
                                         const void *src,
                                         void *dst,
                                         size_t size,
                                         VPUDescriptor &desc) {
    return VPUCopyCommand::fillDescriptor<vpu_cmd_copy_descriptor_mtl_t>(ctx, src, dst, size, desc);
}

static void printCopyDescriptor37xx(void *desc, vpu_cmd_header_t *cmd) {
    VPUCopyCommand::printCopyDesc<vpu_cmd_copy_descriptor_mtl_t>(desc, cmd);
}

struct VPUHwInfo vpuHwInfo37xx = {.supportedDeviceIds = {0x7D1D, 0xAD1D},
                                  .numSupportedDevices = 2,
                                  .compilerPlatform = vpuxCompilerPlatform,
                                  .nExecUnits = 4096,
                                  .numSubslicesPerSlice = 2,
                                  .tileFuseMask = 0x3,
                                  .getCopyCommand = &getCopyCommandDescriptor37xx,
                                  .printCopyDescriptor = &printCopyDescriptor37xx};
} // namespace VPU
