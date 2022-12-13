/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_query_command.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace VPU {

std::shared_ptr<VPUQueryBeginCommand>
VPUQueryBeginCommand::create(VPUDeviceContext *ctx, uint32_t groupMask, void *dataAddress) {
    auto metricDataAddr = getMetricDataAddress(ctx, dataAddress);
    if (metricDataAddr == 0)
        return nullptr;
    return std::make_shared<VPUQueryBeginCommand>(ctx, groupMask, dataAddress, metricDataAddr);
}

std::shared_ptr<VPUQueryEndCommand>
VPUQueryEndCommand::create(VPUDeviceContext *ctx, uint32_t groupMask, void *dataAddress) {
    auto metricDataAddr = getMetricDataAddress(ctx, dataAddress);
    if (metricDataAddr == 0)
        return nullptr;
    return std::make_shared<VPUQueryEndCommand>(ctx, groupMask, dataAddress, metricDataAddr);
}

VPUQueryCommand::VPUQueryCommand(VPUDeviceContext *ctx,
                                 EngineSupport engType,
                                 vpu_cmd_type cmdType,
                                 uint32_t groupMask,
                                 void *dataAddress,
                                 uint64_t metricDataAddress)
    : VPUCommand(engType) {
    LOG_V("Query %s command args: \n"
          "\tgroupMask: %u, dataAddress: %p, metricDataAddress %ld",
          getQueryCommandStr(cmdType),
          groupMask,
          dataAddress,
          metricDataAddress);

    commitCmd.cmd.header.type = cmdType;
    commitCmd.cmd.header.size = sizeof(commitCmd.cmd);
    commitCmd.cmd.metric_group_type = groupMask;
    commitCmd.cmd.metric_data_address = metricDataAddress;

    appendAssociateBufferObject(ctx, dataAddress);
}

size_t VPUQueryCommand::getCommitSize() const {
    return commitCmd.getKMDCommitSize();
}

const uint8_t *VPUQueryCommand::getCommitStream() const {
    return commitCmd.getKMDCommitStream();
}

vpu_cmd_type VPUQueryCommand::getCommandType() const {
    return commitCmd.getKMDCommandType();
}

const char *VPUQueryCommand::getQueryCommandStr(const vpu_cmd_type cmdType) {
    if (cmdType == VPU_CMD_METRIC_QUERY_BEGIN) {
        return "Begin";
    } else if (cmdType == VPU_CMD_METRIC_QUERY_END) {
        return "End";
    }

    return "Unknown";
}

uint64_t VPUQueryCommand::getMetricDataAddress(VPUDeviceContext *ctx, void *dataAddress) {
    if (ctx == nullptr) {
        LOG_E("Failed to get device context.");
        return 0;
    }

    if (dataAddress == nullptr) {
        LOG_E("Invalid data address pointer.");
        return 0;
    }

    uint64_t metricDataAddress = ctx->getBufferVPUAddress(dataAddress);
    if (metricDataAddress == 0u) {
        LOG_E("Data Address (%p) passed in was not found in heap resident!", dataAddress);
    }

    return metricDataAddress;
};

} // namespace VPU
