/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/command/vpu_command.hpp"

#include <uapi/drm/ivpu_accel.h>
#include <memory>

namespace VPU {

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

    static std::shared_ptr<VPUEventCommand> create(VPUDeviceContext *ctx,
                                                   const EngineSupport engType,
                                                   const vpu_cmd_type cmdType,
                                                   KMDEventDataType *eventHeapPtr,
                                                   const KMDEventDataType eventState);

    VPUEventCommand(VPUDeviceContext *ctx,
                    const EngineSupport engType,
                    const vpu_cmd_type cmdType,
                    KMDEventDataType *eventHeapPtr,
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
    static std::shared_ptr<VPUEventCommand> create(VPUDeviceContext *ctx,
                                                   KMDEventDataType *eventHeapPtr) {
        return VPUEventCommand::create(ctx,
                                       EngineSupport::Backward,
                                       VPU_CMD_FENCE_SIGNAL,
                                       eventHeapPtr,
                                       VPUEventCommand::STATE_DEVICE_RESET);
    }
};

class VPUEventSignalCommand : public VPUEventCommand {
  public:
    static std::shared_ptr<VPUEventCommand> create(VPUDeviceContext *ctx,
                                                   KMDEventDataType *eventHeapPtr) {
        return VPUEventCommand::create(ctx,
                                       EngineSupport::Synchronize,
                                       VPU_CMD_FENCE_SIGNAL,
                                       eventHeapPtr,
                                       VPUEventCommand::STATE_DEVICE_SIGNAL);
    }
};

class VPUEventWaitCommand : public VPUEventCommand {
  public:
    static std::shared_ptr<VPUEventCommand> create(VPUDeviceContext *ctx,
                                                   KMDEventDataType *eventHeapPtr) {
        return VPUEventCommand::create(ctx,
                                       EngineSupport::Forward,
                                       VPU_CMD_FENCE_WAIT,
                                       eventHeapPtr,
                                       VPUEventCommand::STATE_WAIT);
    }
};

} // namespace VPU
