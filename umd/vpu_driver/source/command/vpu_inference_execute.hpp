/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/ext/source/graph/elf_parser.hpp"
#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "umd_common.hpp"

#include <cstdint>
#include <api/vpu_jsm_job_cmd_api.h>
#include <memory>
#include <vector>

namespace VPU {

class VPUInferenceExecute : public VPUCommand {
  public:
    VPUInferenceExecute(uint64_t inferenceId,
                        uint64_t vpuAddr,
                        uint64_t size,
                        const std::vector<VPUBufferObject *> &bos) {
        vpu_cmd_inference_execute_t cmd = {};
        cmd.header.type = VPU_CMD_INFERENCE_EXECUTE;
        cmd.header.size = sizeof(vpu_cmd_inference_execute_t);
        cmd.inference_id = inferenceId;
        cmd.host_mapped_inference.address = vpuAddr;
        cmd.host_mapped_inference.width = safe_cast<uint32_t>(size);
        command.emplace<vpu_cmd_inference_execute_t>(cmd);
        appendAssociateBufferObject(bos);
    }
    ~VPUInferenceExecute() = default;

    VPUInferenceExecute(VPUInferenceExecute const &) = delete;
    VPUInferenceExecute &operator=(VPUInferenceExecute const &) = delete;

    static std::shared_ptr<VPUInferenceExecute> create(uint64_t inferenceId,
                                                       uint64_t vpuAddr,
                                                       uint64_t size,
                                                       const std::vector<VPUBufferObject *> &bos) {
        return std::make_shared<VPUInferenceExecute>(inferenceId, vpuAddr, size, bos);
    }

    const vpu_cmd_header_t *getHeader() const {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<vpu_cmd_inference_execute_t>(&command));
    }
};

} // namespace VPU
