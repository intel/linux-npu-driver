/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "vpu_driver/source/command/vpu_command.hpp"

#include <memory>

namespace VPU {

class VPUBarrierCommand : public VPUCommand {
  public:
    VPUBarrierCommand();

    static std::shared_ptr<VPUBarrierCommand> create();

    const vpu_cmd_header_t *getHeader() const {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<const vpu_cmd_barrier_t>(&command));
    }
};

} // namespace VPU
