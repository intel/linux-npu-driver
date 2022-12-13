/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_barrier_command.hpp"

namespace VPU {

std::shared_ptr<VPUBarrierCommand> VPUBarrierCommand::create() {
    return std::make_shared<VPUBarrierCommand>();
}

VPUBarrierCommand::VPUBarrierCommand() {
    commitCmd.cmd.header.type = VPU_CMD_BARRIER;
    commitCmd.cmd.header.size = sizeof(commitCmd.cmd);
}

} // namespace VPU
