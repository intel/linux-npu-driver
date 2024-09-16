/*
 * Copyright (C) 2022-2024 Intel Corporation
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
                                 vpu_cmd_type cmdType,
                                 uint32_t groupMask,
                                 void *dataAddress,
                                 uint64_t metricDataAddress)
    : VPUCommand() {
    // dataAddress       - table address containing pointers to buffers storing metric data
    // metricDataAddress - VPU address of the table containing pointers to buffers storing metric
    //                     data
    LOG(VPU_CMD,
        "Query %s command args: \n"
        "\tgroupMask: %u, dataAddress: %p, metricDataAddress %ld (%#lx)",
        getQueryCommandStr(cmdType),
        groupMask,
        dataAddress,
        metricDataAddress,
        metricDataAddress);

    vpu_cmd_metric_query_t cmd = {};
    cmd.header.type = cmdType;
    cmd.header.size = sizeof(vpu_cmd_metric_query_t);
    cmd.metric_group_type = groupMask;
    cmd.metric_data_address = metricDataAddress;
    command.emplace<vpu_cmd_metric_query_t>(cmd);
    appendAssociateBufferObject(ctx, dataAddress);
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
        LOG_E("Failed to get device context");
        return 0;
    }

    if (dataAddress == nullptr) {
        LOG_E("Invalid data address pointer");
        return 0;
    }

    uint64_t metricDataAddress = ctx->getBufferVPUAddress(dataAddress);
    if (metricDataAddress == 0u) {
        LOG_E("Data Address (%p) passed in was not found in heap resident!", dataAddress);
    }

    return metricDataAddress;
};

} // namespace VPU
