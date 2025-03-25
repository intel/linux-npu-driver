/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "vpu_driver/source/command/vpu_command.hpp"

#include <any>
#include <api/vpu_jsm_job_cmd_api.h>
#include <memory>
#include <utility>
#include <vector>

namespace elf {
class HostParsedInference;
}

namespace L0 {
class ElfParser;
}

namespace VPU {

class VPUBufferObject;
class VPUCommandBuffer;

class VPUInferenceExecute : public VPUCommand {
  public:
    VPUInferenceExecute(std::shared_ptr<L0::ElfParser> &parser,
                        std::shared_ptr<elf::HostParsedInference> &hpi,
                        const std::vector<std::pair<const void *, uint32_t>> &inputs,
                        const std::vector<std::pair<const void *, uint32_t>> &outputs,
                        const std::pair<void *, uint32_t> &profiling,
                        uint64_t inferenceId,
                        std::vector<std::shared_ptr<VPUBufferObject>> &bos,
                        size_t argBosPosition);
    ~VPUInferenceExecute() = default;

    VPUInferenceExecute(VPUInferenceExecute const &) = delete;
    VPUInferenceExecute &operator=(VPUInferenceExecute const &) = delete;

    static std::shared_ptr<VPUInferenceExecute>
    create(std::shared_ptr<L0::ElfParser> parser,
           std::shared_ptr<elf::HostParsedInference> &hpi,
           const std::vector<std::pair<const void *, uint32_t>> &inputs,
           const std::vector<std::pair<const void *, uint32_t>> &outputs,
           const std::pair<void *, uint32_t> &profiling,
           uint64_t inferenceId,
           std::vector<std::shared_ptr<VPUBufferObject>> &bos);

    const vpu_cmd_header_t *getHeader() const override {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<vpu_cmd_inference_execute_t>(&command));
    }

    bool setUpdates(const ArgumentUpdatesMap &updatesMap) override;
    bool update(VPUCommandBuffer *commandBuffer) override;

  private:
    std::shared_ptr<L0::ElfParser> parser;
    std::shared_ptr<elf::HostParsedInference> hpi;
    std::vector<std::pair<const void *, uint32_t>> inputs;
    std::vector<std::pair<const void *, uint32_t>> outputs;
    std::pair<void *, uint32_t> profiling;

    std::vector<uint32_t> argHandles;
    const size_t argBoPosition = 0;
};

} // namespace VPU
