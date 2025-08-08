/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/event_command.hpp"

#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace VPU {

std::shared_ptr<VPUEventCommand>
VPUEventCommand::create(const ScheduleType sType,
                        const vpu_cmd_type cmdType,
                        KMDEventDataType *eventHeapPtr,
                        std::shared_ptr<VPUBufferObject> eventHeapBo,
                        const KMDEventDataType eventState) {
    if (eventHeapPtr == nullptr || eventHeapBo == nullptr) {
        LOG_E("Failed to initialize %s Event cmd because eventHeapPtr or eventHeapBo are nullptr",
              getEventCommandStr(cmdType, eventState));
        return nullptr;
    }

    if (!eventHeapBo->isInRange(reinterpret_cast<void *>(eventHeapPtr))) {
        LOG_E("Pointer %p is not allocated within buffer", eventHeapPtr);
        return nullptr;
    }

    LOG(VPU_CMD, "%s event ptr: %p", getEventCommandStr(cmdType, eventState), eventHeapPtr);
    return std::make_shared<VPUEventCommand>(sType, cmdType, eventHeapPtr, eventHeapBo, eventState);
}

VPUEventCommand::VPUEventCommand(const ScheduleType schType,
                                 const vpu_cmd_type cmdType,
                                 KMDEventDataType *eventHeapPtr,
                                 std::shared_ptr<VPUBufferObject> eventHeapBo,
                                 const KMDEventDataType eventState)
    : VPUCommand(schType) {
    vpu_cmd_fence_t cmd = {};

    cmd.header.type = cmdType;
    cmd.header.size = sizeof(vpu_cmd_fence_t);
    cmd.value = eventState;
    cmd.offset = eventHeapBo->getVPUAddr(eventHeapPtr);
    command.emplace<vpu_cmd_fence_t>(cmd);
    appendAssociateBufferObject(std::move(eventHeapBo));
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
