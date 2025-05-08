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
#include <utility>

namespace VPU {
class VPUBufferObject;

class VPUQueryCommand : public VPUCommand {
  protected:
    VPUQueryCommand(vpu_cmd_type cmdType,
                    uint32_t groupMask,
                    void *dataAddress,
                    std::shared_ptr<VPUBufferObject> bo,
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
    VPUQueryBeginCommand(uint32_t groupMask,
                         void *dataAddress,
                         std::shared_ptr<VPUBufferObject> bo,
                         uint64_t metricDataAddress)
        : VPUQueryCommand(VPU_CMD_METRIC_QUERY_BEGIN,
                          groupMask,
                          dataAddress,
                          std::move(bo),
                          metricDataAddress){};

    static std::shared_ptr<VPUQueryBeginCommand>
    create(uint32_t groupMask, void *dataAddress, std::shared_ptr<VPUBufferObject> dataBo);
};

class VPUQueryEndCommand : public VPUQueryCommand {
  public:
    VPUQueryEndCommand(uint32_t groupMask,
                       void *dataAddress,
                       std::shared_ptr<VPUBufferObject> bo,
                       uint64_t metricDataAddress)
        : VPUQueryCommand(VPU_CMD_METRIC_QUERY_END,
                          groupMask,
                          dataAddress,
                          std::move(bo),
                          metricDataAddress){};

    static std::shared_ptr<VPUQueryEndCommand>
    create(uint32_t groupMask, void *dataAddress, std::shared_ptr<VPUBufferObject> dataBo);
};

} // namespace VPU
