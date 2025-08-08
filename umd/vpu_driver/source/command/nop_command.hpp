/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstddef> // IWYU pragma: keep
#include <cstdint> // IWYU pragma: keep

#include "api/vpu_jsm_job_cmd_api.h"
#include "vpu_driver/source/command/command.hpp"

#include <any>
#include <memory>

namespace VPU {
struct VPUHwInfo;

class VPUNopCommand : public VPUCommand {
  public:
    typedef struct vpu_cmd_nop {
        vpu_cmd_header_t header;
        uint32_t reserved_0;
    } vpu_cmd_nop_t;

    /* NOP command can be created with different size,
     * then one nop command can be used instead sequence of single NOPs
     */
    VPUNopCommand(const VPUHwInfo &hwInfo, size_t size);

    static std::shared_ptr<VPUNopCommand> create(const VPUHwInfo &hwInfo,
                                                 size_t size = sizeof(vpu_cmd_nop_t));

    const vpu_cmd_header_t *getHeader() const {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<const vpu_cmd_nop_t>(&command));
    }
};

} // namespace VPU
