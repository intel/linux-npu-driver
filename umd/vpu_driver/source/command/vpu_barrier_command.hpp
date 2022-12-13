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

namespace VPU {

class VPUBarrierCommand : public VPUCommand {
  public:
    VPUBarrierCommand();

    /**
     * Returns size of the actual commit struct.
     */
    size_t getCommitSize() const override { return commitCmd.getKMDCommitSize(); }

    /**
     * Return the commiting struct data in byte stream.
     */
    const uint8_t *getCommitStream() const override { return commitCmd.getKMDCommitStream(); }

    /**
     * Return the command's type
     */
    vpu_cmd_type getCommandType() const override { return commitCmd.getKMDCommandType(); }

    static std::shared_ptr<VPUBarrierCommand> create();

  private:
    KMDCommitCommand<vpu_cmd_barrier_t> commitCmd;
};

} // namespace VPU
