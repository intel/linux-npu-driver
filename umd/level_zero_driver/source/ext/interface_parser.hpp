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
#include <memory>
#include <vpux_hpi.hpp>

namespace L0 {
struct GraphProfilingQuery;

class IParser {
  public:
    virtual ~IParser() = default;
    virtual ze_result_t parse(std::vector<ze_graph_argument_properties_3_t> &props,
                              std::vector<ze_graph_argument_metadata_t> &args,
                              uint32_t &size) = 0;
    virtual ze_result_t initialize() = 0;
    virtual std::shared_ptr<VPU::VPUCommand> allocateInitCommand(VPU::VPUDeviceContext *ctx) = 0;
    virtual std::shared_ptr<VPU::VPUBufferObject> allocateInternal(size_t size) = 0;
    virtual std::shared_ptr<VPU::VPUCommand>
    allocateExecuteCommand(const std::vector<const void *> &inputArgs,
                           const std::vector<const void *> &outputArgs,
                           GraphProfilingQuery *profilingQuery) = 0;
};

} // namespace L0
