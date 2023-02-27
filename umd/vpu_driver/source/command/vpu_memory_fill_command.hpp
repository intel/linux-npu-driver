/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "vpu_driver/source/command/vpu_command.hpp"

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

    static std::shared_ptr<VPUMemoryFillCommand>
    create(VPUDeviceContext *ctx, void *ptr, const void *pattern, size_t patternSize, size_t size);
    const vpu_cmd_header_t *getHeader() const {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<vpu_cmd_memory_fill_t>(&command));
    }
};

} // namespace VPU
