/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_query_command.hpp"

#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace VPU {

std::shared_ptr<VPUQueryBeginCommand>
VPUQueryBeginCommand::create(uint32_t groupMask,
                             void *dataAddress,
                             std::shared_ptr<VPUBufferObject> dataBo) {
    if (dataAddress == nullptr || !dataBo) {
        LOG_E("Failed to initialize VPUQueryBeginCommand dataAddress or dataBo are nullptr");
        return nullptr;
    }

    auto metricDataAddr = dataBo->getVPUAddr(dataAddress);
    if (metricDataAddr == 0) {
        LOG_E("dataAddress pointer %p is in invalid range", dataAddress);
        return nullptr;
    }

    return std::make_shared<VPUQueryBeginCommand>(groupMask,
                                                  dataAddress,
                                                  std::move(dataBo),
                                                  metricDataAddr);
}

std::shared_ptr<VPUQueryEndCommand>
VPUQueryEndCommand::create(uint32_t groupMask,
                           void *dataAddress,
                           std::shared_ptr<VPUBufferObject> dataBo) {
    if (dataAddress == nullptr || !dataBo) {
        LOG_E("Failed to initialize VPUQueryEndCommand dataAddress or dataBo are nullptr");
        return nullptr;
    }
    auto metricDataAddr = dataBo->getVPUAddr(dataAddress);
    if (metricDataAddr == 0) {
        LOG_E("dataAddress pointer %p is in invalid range", dataAddress);
        return nullptr;
    }
    return std::make_shared<VPUQueryEndCommand>(groupMask,
                                                dataAddress,
                                                std::move(dataBo),
                                                metricDataAddr);
}

VPUQueryCommand::VPUQueryCommand(vpu_cmd_type cmdType,
                                 uint32_t groupMask,
                                 void *dataAddress,
                                 std::shared_ptr<VPUBufferObject> bo,
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
    appendAssociateBufferObject(std::move(bo));
}

const char *VPUQueryCommand::getQueryCommandStr(const vpu_cmd_type cmdType) {
    if (cmdType == VPU_CMD_METRIC_QUERY_BEGIN) {
        return "Begin";
    } else if (cmdType == VPU_CMD_METRIC_QUERY_END) {
        return "End";
    }

    return "Unknown";
}

} // namespace VPU
