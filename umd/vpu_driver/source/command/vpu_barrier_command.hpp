/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstddef> // IWYU pragma: keep
#include <cstdint> // IWYU pragma: keep

#include "api/vpu_jsm_job_cmd_api.h"
#include "vpu_driver/source/command/vpu_command.hpp"

#include <any>
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
