/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include <stdint.h>

#include "api/vpu_jsm_job_cmd_api.h"
#include "vpu_driver/source/command/vpu_command.hpp"

#include <any>
#include <memory>

namespace VPU {
class VPUDeviceContext;

class VPUQueryCommand : public VPUCommand {
  public:
    static uint64_t getMetricDataAddress(VPUDeviceContext *ctx, void *dataAddress);

  protected:
    VPUQueryCommand(VPUDeviceContext *ctx,
                    vpu_cmd_type cmdType,
                    uint32_t groupMask,
                    void *dataAddress,
                    uint64_t metricDataAddress);
    const vpu_cmd_header_t *getHeader() const {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<vpu_cmd_metric_query_t>(&command));
    }

  private:
    static const char *getQueryCommandStr(const vpu_cmd_type cmdType);
};

class VPUQueryBeginCommand : public VPUQueryCommand {
  public:
    VPUQueryBeginCommand(VPUDeviceContext *ctx,
                         uint32_t groupMask,
                         void *dataAddress,
                         uint64_t metricDataAddress)
        : VPUQueryCommand(ctx,
                          VPU_CMD_METRIC_QUERY_BEGIN,
                          groupMask,
                          dataAddress,
                          metricDataAddress){};

    static std::shared_ptr<VPUQueryBeginCommand>
    create(VPUDeviceContext *ctx, uint32_t groupMask, void *dataAddress);
};

class VPUQueryEndCommand : public VPUQueryCommand {
  public:
    VPUQueryEndCommand(VPUDeviceContext *ctx,
                       uint32_t groupMask,
                       void *dataAddress,
                       uint64_t metricDataAddress)
        : VPUQueryCommand(ctx,
                          VPU_CMD_METRIC_QUERY_END,
                          groupMask,
                          dataAddress,
                          metricDataAddress){};

    static std::shared_ptr<VPUQueryEndCommand>
    create(VPUDeviceContext *ctx, uint32_t groupMask, void *dataAddress);
};

} // namespace VPU
