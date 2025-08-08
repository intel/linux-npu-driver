/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstdint>

#include "api/vpu_jsm_job_cmd_api.h"
#include "vpu_driver/source/command/command.hpp"

#include <any>
#include <memory>
#include <utility>

namespace VPU {
class VPUBufferObject;

class VPUEventCommand : public VPUCommand {
  public:
    typedef uint64_t KMDEventDataType;
    enum State : KMDEventDataType {
        STATE_EVENT_INITIAL = 0,
        STATE_DEVICE_RESET,
        STATE_HOST_RESET,
        STATE_WAIT,
        STATE_DEVICE_SIGNAL,
        STATE_HOST_SIGNAL,
    };

    struct JsmEventData {
        uint64_t event;       /**< enum KMDEventDataType */
        uint64_t reserved[7]; /**< Unused */
    };
    static_assert(sizeof(JsmEventData) % 64 == 0, "JsmEventData is misaligned");

    static std::shared_ptr<VPUEventCommand> create(const ScheduleType sType,
                                                   const vpu_cmd_type cmdType,
                                                   KMDEventDataType *eventHeapPtr,
                                                   std::shared_ptr<VPUBufferObject> eventHeapBo,
                                                   const KMDEventDataType eventState);

    VPUEventCommand(const ScheduleType sType,
                    const vpu_cmd_type cmdType,
                    KMDEventDataType *eventHeapPtr,
                    std::shared_ptr<VPUBufferObject> eventHeapBo,
                    const KMDEventDataType eventState);

    const vpu_cmd_header_t *getHeader() const {
        return reinterpret_cast<const vpu_cmd_header_t *>(std::any_cast<vpu_cmd_fence_t>(&command));
    }

  private:
    static const char *getEventCommandStr(const vpu_cmd_type cmdType,
                                          const KMDEventDataType eventState);
};

class VPUEventResetCommand : public VPUEventCommand {
  public:
    static std::shared_ptr<VPUEventCommand> create(KMDEventDataType *eventHeapPtr,
                                                   std::shared_ptr<VPUBufferObject> eventHeapBo) {
        return VPUEventCommand::create(ScheduleType::Generic,
                                       VPU_CMD_FENCE_SIGNAL,
                                       eventHeapPtr,
                                       std::move(eventHeapBo),
                                       VPUEventCommand::STATE_DEVICE_RESET);
    }
};

class VPUEventSignalCommand : public VPUEventCommand {
  public:
    static std::shared_ptr<VPUEventCommand> create(KMDEventDataType *eventHeapPtr,
                                                   std::shared_ptr<VPUBufferObject> eventHeapBo) {
        return VPUEventCommand::create(ScheduleType::Synchronize,
                                       VPU_CMD_FENCE_SIGNAL,
                                       eventHeapPtr,
                                       std::move(eventHeapBo),
                                       VPUEventCommand::STATE_DEVICE_SIGNAL);
    }
};

class VPUEventWaitCommand : public VPUEventCommand {
  public:
    static std::shared_ptr<VPUEventCommand> create(KMDEventDataType *eventHeapPtr,
                                                   std::shared_ptr<VPUBufferObject> eventHeapBo) {
        return VPUEventCommand::create(ScheduleType::Generic,
                                       VPU_CMD_FENCE_WAIT,
                                       eventHeapPtr,
                                       std::move(eventHeapBo),
                                       VPUEventCommand::STATE_WAIT);
    }
};

} // namespace VPU
