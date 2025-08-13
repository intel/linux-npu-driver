/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include <cstdint>

#include "api/vpu_jsm_job_cmd_api.h"
#include "vpu_driver/source/command/command.hpp"

#include <any>
#include <memory>

namespace VPU {
class VPUBufferObject;

class VPUFillCommand : public VPUCommand {
  public:
    VPUFillCommand(void *dstPtr,
                   std::shared_ptr<VPUBufferObject> dstBo,
                   uint64_t size,
                   uint32_t fill_pattern);

    static std::shared_ptr<VPUFillCommand> create(void *dstPtr,
                                                  std::shared_ptr<VPUBufferObject> dstBo,
                                                  uint64_t size,
                                                  uint32_t fill_pattern);
    const vpu_cmd_header_t *getHeader() const {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<vpu_cmd_memory_fill_t>(&command));
    }
};

} // namespace VPU
