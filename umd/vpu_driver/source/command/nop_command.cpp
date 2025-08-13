/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/nop_command.hpp"

#include "umd_common.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace VPU {

VPUNopCommand::VPUNopCommand(const VPUHwInfo &hwInfo, size_t size) {
    vpu_cmd_nop_t cmd = {};

    /* The NOP command available from 4.10 api version, for earlier api BARRIER is used */
    static_assert(sizeof(vpu_cmd_nop_t) == sizeof(vpu_cmd_barrier_t));
    cmd.header.type = isJsmCmdApiGreaterThan(hwInfo, 4, 9) ? VPU_CMD_NOP : VPU_CMD_BARRIER;
    cmd.header.size = safe_cast<uint16_t>(size);
    command.emplace<vpu_cmd_nop_t>(cmd);
}

std::shared_ptr<VPUNopCommand> VPUNopCommand::create(const VPUHwInfo &hwInfo, size_t size) {
    if (size < sizeof(vpu_cmd_nop_t) || size > UINT16_MAX) {
        LOG_E("Unsupported NOP command size: %ld", size);
        return nullptr;
    }
    return std::make_shared<VPUNopCommand>(hwInfo, size);
}
} // namespace VPU