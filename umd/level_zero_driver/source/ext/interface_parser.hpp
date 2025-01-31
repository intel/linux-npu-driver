/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/device/vpu_device_context.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_ext.h>
#include <vpux_hpi.hpp>

#include <memory>

namespace L0 {

class IParser {
  public:
    virtual ~IParser() = default;
    virtual ze_result_t parse(std::vector<ze_graph_argument_properties_3_t> &props,
                              std::vector<ze_graph_argument_metadata_t> &args,
                              uint32_t &size) = 0;
    virtual ze_result_t initialize() = 0;
    virtual std::shared_ptr<VPU::VPUCommand> allocateInitCommand(VPU::VPUDeviceContext *ctx) = 0;
    virtual std::shared_ptr<VPU::VPUCommand>
    allocateExecuteCommand(VPU::VPUDeviceContext *ctx,
                           const std::vector<std::pair<const void *, uint32_t>> &inputArgs,
                           const std::vector<std::pair<const void *, uint32_t>> &outputArgs,
                           const std::pair<void *, uint32_t> &profilingPtr) = 0;
};

} // namespace L0
