/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/command/kmd_commit_command.hpp"

#include <memory>

#include <cstdint>

namespace VPU {

class VPUTimeStampCommand : public VPUCommand {
  public:
    VPUTimeStampCommand(VPUDeviceContext *ctx, uint64_t *dstPtr);
    size_t getCommitSize() const override;
    const uint8_t *getCommitStream() const override;
    vpu_cmd_type getCommandType() const override;

    static std::shared_ptr<VPUTimeStampCommand> create(VPUDeviceContext *ctx, uint64_t *dstPtr);

  private:
    KMDCommitCommand<vpu_cmd_timestamp_t> commitCmd;
};

} // namespace VPU
