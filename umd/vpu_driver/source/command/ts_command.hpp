/*
 * Copyright (C) 2022-2024 Intel Corporation
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

class VPUTimeStampCommand : public VPUCommand {
  public:
    VPUTimeStampCommand(uint32_t type, uint64_t dstVPUAddr, std::shared_ptr<VPUBufferObject> dstBo);

    static std::shared_ptr<VPUTimeStampCommand>
    create(uint64_t *dstPtr, std::shared_ptr<VPUBufferObject> dstBo, uint32_t type = 0);

    const vpu_cmd_header_t *getHeader() const {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<vpu_cmd_timestamp_t>(&command));
    }
};

} // namespace VPU
