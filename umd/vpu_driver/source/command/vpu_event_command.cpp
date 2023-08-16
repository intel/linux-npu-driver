/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_event_command.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include <boost/numeric/conversion/cast.hpp>

namespace VPU {

std::shared_ptr<VPUEventCommand> VPUEventCommand::create(VPUDeviceContext *ctx,
                                                         const EngineSupport engType,
                                                         const vpu_cmd_type cmdType,
                                                         KMDEventDataType *eventHeapPtr,
                                                         const KMDEventDataType eventState) {
    if (eventHeapPtr == nullptr) {
        LOG_E("Failed to initialize %s Event cmd because eventHeapPtr is nullptr",
              getEventCommandStr(cmdType, eventState));
        return nullptr;
    }

    if (ctx == nullptr) {
        LOG_E("Context is nullptr in Event command");
        return nullptr;
    }

    VPUBufferObject *eventBuffer = ctx->findBuffer(eventHeapPtr);
    if (eventBuffer == nullptr) {
        LOG_E("Event pointer %p is not allocated within context %p", eventHeapPtr, ctx);
        return nullptr;
    }

    LOG_V("%s event ptr: %p", getEventCommandStr(cmdType, eventState), eventHeapPtr);
    return std::make_shared<VPUEventCommand>(ctx, engType, cmdType, eventHeapPtr, eventState);
}

VPUEventCommand::VPUEventCommand(VPUDeviceContext *ctx,
                                 const EngineSupport engType,
                                 const vpu_cmd_type cmdType,
                                 KMDEventDataType *eventHeapPtr,
                                 const KMDEventDataType eventState)
    : VPUCommand(engType) {
    vpu_cmd_fence_t cmd = {};

    cmd.header.type = cmdType;
    cmd.header.size = sizeof(vpu_cmd_fence_t);
    cmd.offset = boost::numeric_cast<uint32_t>(ctx->getBufferVPUAddress(eventHeapPtr) -
                                               ctx->getVPULowBaseAddress());
    cmd.value = eventState;
    command.emplace<vpu_cmd_fence_t>(cmd);
    appendAssociateBufferObject(ctx, eventHeapPtr);
}

const char *VPUEventCommand::getEventCommandStr(const vpu_cmd_type cmdType,
                                                const KMDEventDataType eventState) {
    if (cmdType == VPU_CMD_FENCE_SIGNAL) {
        if (eventState == VPUEventCommand::STATE_DEVICE_RESET) {
            return "Reset";
        } else {
            return "Signal";
        }
    } else if (cmdType == VPU_CMD_FENCE_WAIT) {
        return "Wait";
    }

    return "Unknown";
}

} // namespace VPU
