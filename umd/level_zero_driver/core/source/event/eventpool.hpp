/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/device/device.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"
#include <level_zero/ze_api.h>

#include <boost/safe_numerics/safe_integer.hpp>

struct _ze_event_pool_handle_t {};

namespace L0 {

struct EventPool : _ze_event_pool_handle_t {
    EventPool(DriverHandle *driver,
              VPU::VPUDeviceContext *ctx,
              uint32_t numDevices,
              ze_device_handle_t *phDevices,
              uint32_t numEvents,
              ze_event_pool_flags_t flags);
    ~EventPool();

    // Block copy constructors.
    EventPool(EventPool const &) = delete;
    EventPool &operator=(EventPool const &) = delete;

    inline ze_event_pool_handle_t toHandle() { return this; }
    static EventPool *fromHandle(ze_event_pool_handle_t handle) {
        return static_cast<EventPool *>(handle);
    }

    static EventPool *create(DriverHandle *driver,
                             VPU::VPUDeviceContext *ctx,
                             uint32_t numDevices,
                             ze_device_handle_t *phDevices,
                             const ze_event_pool_desc_t *desc);
    ze_result_t destroy();
    ze_result_t createEvent(const ze_event_desc_t *desc, ze_event_handle_t *phEvent);

    /**
     * Return number of currently allocatable number of events.
     */
    uint32_t getNumberOfAvailableEvents() const;

    /**
     * Return max capability of the event pool.
     */
    uint32_t getEventPoolCapability() const { return szEventCap; }

    /**
     * Allocate a event from the event pool.
     */
    VPU::VPUEventCommand::KMDEventDataType *allocateEvent(uint32_t index);

    /**
     * Deallocate a event memory.
     */
    bool freeEvent(uint32_t index);

  private:
    /**
     * From Context -> VPUDeviceContext.
     */
    VPU::VPUDeviceContext *ctx;

    /**
     * Event pool pointer.
     */
    VPU::VPUBufferObject *pEventPool;

    /**
     * Event pool allocation capability.
     */
    uint32_t szEventCap = 0;

    /**
     * Currently allocated event size.
     */
    boost::safe_numerics::safe<uint32_t> szEventAllocated = 0;

    /**
     * Event pool allocation table.
     */
    std::vector<std::pair<VPU::VPUEventCommand::KMDEventDataType *, bool>> allocationTable;
};

} // namespace L0
