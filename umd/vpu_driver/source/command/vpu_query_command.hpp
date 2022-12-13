/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/command/kmd_commit_command.hpp"

#include <memory>

namespace VPU {

class VPUQueryCommand : public VPUCommand {
  public:
    size_t getCommitSize() const override;
    const uint8_t *getCommitStream() const override;
    vpu_cmd_type getCommandType() const override;

    static uint64_t getMetricDataAddress(VPUDeviceContext *ctx, void *dataAddress);

  protected:
    VPUQueryCommand(VPUDeviceContext *ctx,
                    EngineSupport engType,
                    vpu_cmd_type cmdType,
                    uint32_t groupMask,
                    void *dataAddress,
                    uint64_t metricDataAddress);

  private:
    KMDCommitCommand<vpu_cmd_metric_query_t> commitCmd;

    static const char *getQueryCommandStr(const vpu_cmd_type cmdType);
};

class VPUQueryBeginCommand : public VPUQueryCommand {
  public:
    VPUQueryBeginCommand(VPUDeviceContext *ctx,
                         uint32_t groupMask,
                         void *dataAddress,
                         uint64_t metricDataAddress)
        : VPUQueryCommand(ctx,
                          EngineSupport::Forward,
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
                          EngineSupport::Backward,
                          VPU_CMD_METRIC_QUERY_END,
                          groupMask,
                          dataAddress,
                          metricDataAddress){};

    static std::shared_ptr<VPUQueryEndCommand>
    create(VPUDeviceContext *ctx, uint32_t groupMask, void *dataAddress);
};

} // namespace VPU
