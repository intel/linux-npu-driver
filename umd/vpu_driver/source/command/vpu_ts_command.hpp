/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "vpu_driver/source/command/vpu_command.hpp"

#include <memory>

#include <cstdint>

namespace VPU {

class VPUTimeStampCommand : public VPUCommand {
  public:
    VPUTimeStampCommand(VPUDeviceContext *ctx, uint64_t *dstPtr);

    static std::shared_ptr<VPUTimeStampCommand> create(VPUDeviceContext *ctx, uint64_t *dstPtr);
    const vpu_cmd_header_t *getHeader() const {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<vpu_cmd_timestamp_t>(&command));
    }
};

} // namespace VPU
