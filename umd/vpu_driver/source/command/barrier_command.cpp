/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/barrier_command.hpp"

namespace VPU {

VPUBarrierCommand::VPUBarrierCommand()
    : VPUCommand() {
    vpu_cmd_barrier_t cmd = {};

    cmd.header.type = VPU_CMD_BARRIER;
    cmd.header.size = sizeof(vpu_cmd_barrier_t);
    command.emplace<vpu_cmd_barrier_t>(cmd);
};

std::shared_ptr<VPUBarrierCommand> VPUBarrierCommand::create() {
    return std::make_shared<VPUBarrierCommand>();
}

} // namespace VPU
