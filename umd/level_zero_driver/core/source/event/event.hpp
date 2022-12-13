/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/event/eventpool.hpp"
#include "level_zero_driver/core/source/device/device.hpp"

#include <level_zero/ze_api.h>
#include <memory>
#include <vector>

struct _ze_event_handle_t {};

namespace L0 {

struct EventPool;

struct Event : _ze_event_handle_t {
  public:
    Event(EventPool *eventPool, uint32_t index, VPU::VPUEventCommand::KMDEventDataType *ptr);
    ~Event() = default;

    inline ze_event_handle_t toHandle() { return this; }
    static Event *fromHandle(ze_event_handle_t handle) { return static_cast<Event *>(handle); }

    static Event *create(EventPool *eventPool, const ze_event_desc_t *desc);
    ze_result_t destroy();
    ze_result_t hostSignal();
    ze_result_t hostSynchronize(uint64_t timeout);
    ze_result_t queryStatus();
    ze_result_t reset();

    inline VPU::VPUEventCommand::KMDEventDataType *getSyncPointer() const { return pSyncPointer; }

    void associateJob(std::weak_ptr<VPU::VPUJob> job) { associatedJobs.push_back(std::move(job)); }

  private:
    /**
     * @brief Change sync state.
     * @param updateTo [in] Target status to be changed.
     */
    void updateSyncState(VPU::VPUEventCommand::KMDEventDataType updateTo);

    /**
     * Event index.
     */
    uint32_t nIndex = 0;

    /**
     * Event pool pointer that the event belongs to.
     */
    EventPool *pEventPool = nullptr;

    /**
     * @brief A pointer allocated from event pool for event synchronization.
     */
    VPU::VPUEventCommand::KMDEventDataType *pSyncPointer = nullptr;

    /**
     * @brief Jobs that uses the event.
     */
    std::vector<std::weak_ptr<VPU::VPUJob>> associatedJobs;
};

} // namespace L0
