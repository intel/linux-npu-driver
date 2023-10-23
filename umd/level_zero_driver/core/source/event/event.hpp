/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/device/device.hpp"

#include <level_zero/ze_api.h>
#include <memory>
#include <vector>

struct _ze_event_handle_t {};

namespace L0 {

struct Event : _ze_event_handle_t, IContextObject {
  public:
    Event(VPU::VPUEventCommand::KMDEventDataType *ptr,
          uint64_t vpuAddr,
          std::function<void()> &&destroyCb);
    ~Event() = default;

    inline ze_event_handle_t toHandle() { return this; }
    static Event *fromHandle(ze_event_handle_t handle) { return static_cast<Event *>(handle); }

    ze_result_t destroy();
    ze_result_t hostSignal();
    ze_result_t hostSynchronize(uint64_t timeout);
    ze_result_t queryStatus();
    ze_result_t reset();

    inline VPU::VPUEventCommand::KMDEventDataType *getSyncPointer() const { return eventState; }

    void associateJob(std::weak_ptr<VPU::VPUJob> job) { associatedJobs.push_back(std::move(job)); }

  private:
    void setEventState(VPU::VPUEventCommand::KMDEventDataType updateTo);

    VPU::VPUEventCommand::KMDEventDataType *eventState = nullptr;
    uint64_t eventVpuAddr = 0;
    std::function<void()> destroyCb;
    std::vector<std::weak_ptr<VPU::VPUJob>> associatedJobs;
};

} // namespace L0
