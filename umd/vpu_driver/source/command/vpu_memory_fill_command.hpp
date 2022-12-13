/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/command/kmd_commit_command.hpp"

#include <cstdint>
#include <memory>

namespace VPU {

class VPUMemoryFillCommand : public VPUCommand {
  public:
    VPUMemoryFillCommand(VPUDeviceContext *ctx,
                         void *ptr,
                         const void *pattern,
                         size_t patternSize,
                         size_t size);
    size_t getCommitSize() const override;
    const uint8_t *getCommitStream() const override;
    vpu_cmd_type getCommandType() const override;

    static std::shared_ptr<VPUMemoryFillCommand>
    create(VPUDeviceContext *ctx, void *ptr, const void *pattern, size_t patternSize, size_t size);

  private:
    KMDCommitCommand<vpu_cmd_memory_fill_t> commitCmd;
};

} // namespace VPU
