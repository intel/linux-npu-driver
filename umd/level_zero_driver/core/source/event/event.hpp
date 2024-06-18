/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/tools/source/metrics/metric_streamer.hpp"

#include <level_zero/ze_api.h>
#include <memory>
#include <vector>

struct _ze_event_handle_t {};

namespace L0 {

struct Event : _ze_event_handle_t, IContextObject {
  public:
    Event(VPU::VPUDeviceContext *ctx,
          VPU::VPUEventCommand::KMDEventDataType *ptr,
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
    uint64_t eventVpuAddr = 0;
    std::function<void()> destroyCb;
    std::vector<std::weak_ptr<VPU::VPUJob>> associatedJobs;
    size_t msExpectedDataSize = 0;
    uint64_t msGroupMask = 0ULL;
};

} // namespace L0
