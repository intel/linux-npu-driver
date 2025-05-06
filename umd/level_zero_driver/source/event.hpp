/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/include/l0_handler.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"

#include <functional>
#include <level_zero/ze_api.h>
#include <memory>
#include <utility>
#include <vector>

namespace VPU {
class VPUBufferObject;
class VPUDeviceContext;
class VPUJob;
} // namespace VPU

struct _ze_event_handle_t {};

namespace L0 {

struct Event : _ze_event_handle_t, IContextObject {
  public:
    Event(VPU::VPUDeviceContext *ctx,
          VPU::VPUEventCommand::KMDEventDataType *ptr,
          const std::shared_ptr<VPU::VPUBufferObject> eventBaseBo,
          uint64_t vpuAddr,
          std::function<void()> &&destroyCb);
    ~Event() = default;

    inline ze_event_handle_t toHandle() { return this; }
    static Event *fromHandle(ze_event_handle_t handle) { return static_cast<Event *>(handle); }

    ze_result_t destroy();
    ze_result_t hostSignal();
    ze_result_t hostSynchronize(uint64_t timeout);
    ze_result_t queryStatus(int64_t timeout = 0L);
    ze_result_t reset();

    inline VPU::VPUEventCommand::KMDEventDataType *getSyncPointer() const { return eventState; }
    const std::shared_ptr<VPU::VPUBufferObject> getAssociatedBo() const;

    void associateJob(std::weak_ptr<VPU::VPUJob> job) { associatedJobs.push_back(std::move(job)); }
    void setMetricTrackData(uint64_t groupMask, size_t dataSize) {
        msGroupMask = groupMask;
        msExpectedDataSize = dataSize;
    }
    void trackMetricData(int64_t timeoutNs);

  private:
    void setEventState(VPU::VPUEventCommand::KMDEventDataType updateTo);

    VPU::VPUDeviceContext *pDevCtx = nullptr;
    VPU::VPUEventCommand::KMDEventDataType *eventState = nullptr;
    const std::shared_ptr<VPU::VPUBufferObject> eventBase;
    uint64_t eventVpuAddr = 0;
    std::function<void()> destroyCb;
    std::vector<std::weak_ptr<VPU::VPUJob>> associatedJobs;
    size_t msExpectedDataSize = 0;
    uint64_t msGroupMask = 0ULL;
};

} // namespace L0
