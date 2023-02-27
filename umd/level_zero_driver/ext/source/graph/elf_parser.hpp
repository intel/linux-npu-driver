/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero/ze_graph_ext.h"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/command/vpu_inference_execute.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <cstdint>
#include <memory>
#include <vpux_loader/vpux_loader.hpp>

namespace L0 {

class ElfParser {
  public:
    ElfParser(VPU::VPUDeviceContext *ctx,
              std::unique_ptr<elf::BufferManager> manager,
              std::unique_ptr<elf::VPUXLoader> loader);
    ~ElfParser();

    ElfParser(const ElfParser &rhs) = delete;
    ElfParser operator=(const ElfParser &rhs) = delete;
    ElfParser(ElfParser &&rhs)
        : ctx(rhs.ctx)
        , manager(std::move(rhs.manager))
        , loader(std::move(rhs.loader))
        , hostParsedInference(rhs.hostParsedInference) {
        rhs.ctx = nullptr;
        rhs.hostParsedInference = nullptr;
    }
    ElfParser &operator=(ElfParser &&rhs) {
        ctx = rhs.ctx;
        manager = std::move(rhs.manager);
        loader = std::move(rhs.loader);
        hostParsedInference = rhs.hostParsedInference;
        rhs.ctx = nullptr;
        rhs.hostParsedInference = nullptr;
        return *this;
    }

    static std::optional<ElfParser>
    getElfParser(VPU::VPUDeviceContext *ctx, uint8_t *ptr, size_t size);

    void getArgumentProperties(std::vector<ze_graph_argument_properties_t> &props) const;

    bool applyInputOutputs(const std::vector<std::pair<const void *, uint32_t>> &inputs,
                           const std::vector<std::pair<const void *, uint32_t>> &outputs);

    std::shared_ptr<VPU::VPUInferenceExecute> getCommand(uint64_t inferenceId);

  private:
    void addArtificalBarrierConfig();
    static ze_graph_argument_precision_t getTensorPrecision(elf::DType type);

    VPU::VPUDeviceContext *ctx;
    std::unique_ptr<elf::BufferManager> manager;
    std::unique_ptr<elf::VPUXLoader> loader;
    VPU::VPUBufferObject *hostParsedInference;
    std::vector<VPU::VPUBufferObject *> userBuffers;
};

} // namespace L0
