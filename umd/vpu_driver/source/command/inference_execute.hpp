/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "vpu_driver/source/command/command.hpp"

#include <any>
#include <api/vpu_jsm_job_cmd_api.h>
#include <memory>
#include <vector>

namespace elf {
class HostParsedInference;
}

namespace L0 {
class ElfParser;
struct GraphProfilingQuery;
} // namespace L0

namespace VPU {

class VPUBufferObject;
class VPUCommandBuffer;

class VPUInferenceExecute : public VPUCommand {
  public:
    VPUInferenceExecute(std::shared_ptr<L0::ElfParser> &parser,
                        std::shared_ptr<elf::HostParsedInference> &hpi,
                        const std::vector<const void *> &inputs,
                        const std::vector<const void *> &outputs,
                        L0::GraphProfilingQuery *profilingQuery,
                        uint64_t inferenceId,
                        std::vector<std::shared_ptr<VPUBufferObject>> &bos,
                        std::vector<std::shared_ptr<VPUBufferObject>> &userBos);
    ~VPUInferenceExecute() = default;

    VPUInferenceExecute(VPUInferenceExecute const &) = delete;
    VPUInferenceExecute &operator=(VPUInferenceExecute const &) = delete;

    static std::shared_ptr<VPUInferenceExecute>
    create(std::shared_ptr<L0::ElfParser> parser,
           std::shared_ptr<elf::HostParsedInference> &hpi,
           const std::vector<const void *> &inputs,
           const std::vector<const void *> &outputs,
           L0::GraphProfilingQuery *profilingQuery,
           uint64_t inferenceId,
           std::vector<std::shared_ptr<VPUBufferObject>> &bos);

    const vpu_cmd_header_t *getHeader() const override {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<vpu_cmd_inference_execute_t>(&command));
    }

    bool setUpdates(const ArgumentUpdatesMap &updatesMap) override;
    bool update(VPUCommandBuffer *commandBuffer) override;

    void updateScratchBuffer(VPUCommandBuffer *cmdBuffer, std::shared_ptr<VPUBufferObject> bo);
    size_t getSharedScratchSize();

  private:
    std::shared_ptr<L0::ElfParser> parser;
    std::shared_ptr<elf::HostParsedInference> hpi;
    std::vector<const void *> inputs;
    std::vector<const void *> outputs;
    L0::GraphProfilingQuery *profilingQuery;

    /* inputs, outputs and graph profiling buffer */
    size_t userArgIndex;
    std::vector<uint32_t> userArgHandles;

    /* shared scratch buffer, on command create the value is invalid */
    uint32_t lastScratchBoHandle = UINT32_MAX;
};

} // namespace VPU
