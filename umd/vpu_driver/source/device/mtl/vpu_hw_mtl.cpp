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

static bool getCopyCommandDescriptorMTL(VPUDeviceContext *ctx,
                                        const void *src,
                                        void *dst,
                                        size_t size,
                                        VPUDescriptor &desc) {
    return VPUCopyCommand::fillDescriptor<vpu_cmd_copy_descriptor_mtl_t>(ctx, src, dst, size, desc);
}

static void printCopyDescriptorMTL(void *desc, vpu_cmd_header_t *cmd) {
    VPUCopyCommand::printCopyDesc<vpu_cmd_copy_descriptor_mtl_t>(desc, cmd);
}

struct VPUHwInfo mtlHwInfo = {.deviceId = 0x7D1D,
                              .nExecUnits = 4096,
                              .numSubslicesPerSlice = 2,
                              .getCopyCommand = &getCopyCommandDescriptorMTL,
                              .printCopyDescriptor = printCopyDescriptorMTL};

} // namespace VPU
