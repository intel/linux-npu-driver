/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/event/eventpool.hpp"
#include "level_zero_driver/core/source/event/event.hpp"
#include "level_zero_driver/core/source/context/context.hpp"

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"

#include <level_zero/ze_api.h>

namespace L0 {

EventPool *EventPool::create(DriverHandle *driver,
                             VPU::VPUDeviceContext *ctx,
                             uint32_t numDevices,
                             ze_device_handle_t *phDevices,
                             const ze_event_pool_desc_t *desc) {
    if (driver == nullptr) {
        LOG_E("Invalid driver handle.");
        return nullptr;
    }

    if (desc->count == 0) {
        LOG_E("Invalid event pool size.");
        return nullptr;
    }

    auto eventPool = new EventPool(driver, ctx, numDevices, phDevices, desc->count, desc->flags);
    if (!eventPool) {
        LOG_E("Failed to allocate event pool.");
        return nullptr;
    }

    eventPool->pEventPool = ctx->createInternalBufferObject(
        sizeof(VPU::VPUEventCommand::JsmEventData) * eventPool->szEventCap,
        VPU::VPUBufferObject::Type::CachedLow);
    if (eventPool->pEventPool == nullptr) {
        LOG_E("Failed to allocate buffer object for event pool of size %u", eventPool->szEventCap);
        delete eventPool;
        return nullptr;
    }

    auto eventPtr = reinterpret_cast<VPU::VPUEventCommand::JsmEventData *>(
        eventPool->pEventPool->getBasePointer());
    for (auto &pool : eventPool->allocationTable) {
        pool.first = reinterpret_cast<VPU::VPUEventCommand::KMDEventDataType *>(eventPtr++);
    }

    return eventPool;
}

EventPool::EventPool(DriverHandle *driver,
                     VPU::VPUDeviceContext *ctx,
                     uint32_t numDevices,
                     ze_device_handle_t *phDevices,
                     uint32_t numEvents,
                     ze_event_pool_flags_t flags)
    : ctx(ctx)
    , pEventPool(nullptr)
    , szEventCap(numEvents)
    , szEventAllocated(0)
    , allocationTable(szEventCap, std::make_pair(nullptr, false)) {}

EventPool::~EventPool() {
    if (pEventPool != nullptr) {
        ctx->freeMemAlloc(pEventPool);
    }
}

uint32_t EventPool::getNumberOfAvailableEvents() const {
    if (szEventCap == 0) {
        LOG_E("Event pool size is invalid.");
        return 0;
    }

    uint32_t nAvailableEvents = (szEventCap - szEventAllocated);
    LOG_V("Event pool: cap %u, alloc: %u, available: %u",
          szEventCap,
          static_cast<uint32_t>(szEventAllocated),
          nAvailableEvents);
    return nAvailableEvents;
}

VPU::VPUEventCommand::KMDEventDataType *EventPool::allocateEvent(uint32_t index) {
    if (index >= getEventPoolCapability()) {
        LOG_E("Event index out of bound (%u / %u)", index, getEventPoolCapability());
        return nullptr;
    }

    if (allocationTable[index].second) {
        LOG_E("Index %u already allocated by %p.", index, allocationTable[index].first);
        return nullptr;
    }

    allocationTable[index].second = true;
    LOG_V("Event pointer %p has been allocated for index %u from event pool.",
          allocationTable[index].first,
          index);
    return allocationTable[index].first;
}

bool EventPool::freeEvent(uint32_t index) {
    uint32_t evCap = getEventPoolCapability();
    if (index >= evCap) {
        LOG_E("Index out of bound (%u / %u)", index, evCap);
        return false;
    }
    allocationTable[index].second = false;
    szEventAllocated--;
    return true;
}

ze_result_t EventPool::destroy() {
    delete this;
    return ZE_RESULT_SUCCESS;
}

ze_result_t EventPool::createEvent(const ze_event_desc_t *desc, ze_event_handle_t *phEvent) {
    if (desc == nullptr || phEvent == nullptr) {
        LOG_E("Invalid event desc or output handle.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    uint32_t eventFlagsMax = boost::numeric_cast<uint32_t>(
        ZE_EVENT_SCOPE_FLAG_SUBDEVICE | ZE_EVENT_SCOPE_FLAG_DEVICE | ZE_EVENT_SCOPE_FLAG_HOST);
    if (desc->signal > eventFlagsMax || desc->wait > eventFlagsMax) {
        LOG_E("Invalid event descriptor values (sig: %u, wait: %u)", desc->signal, desc->wait);
        return ZE_RESULT_ERROR_INVALID_ENUMERATION;
    }

    if (getNumberOfAvailableEvents() == 0) {
        LOG_E("Reached maximum total of %u events.", static_cast<uint32_t>(szEventAllocated));
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (desc->index >= szEventCap) {
        LOG_E("Index out of bounds (%u / %u)", desc->index, szEventCap);
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    auto *eventPtr = allocateEvent(desc->index);
    if (eventPtr == nullptr) {
        LOG_E("Failed to get event from event pool");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    uint64_t vpuAddr = ctx->getBufferVPUAddress(eventPtr);
    auto newEvent = new Event(this, desc->index, eventPtr, vpuAddr);
    if (newEvent == nullptr) {
        LOG_E("Failed to initialize event.");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *phEvent = newEvent;
    szEventAllocated++;
    return ZE_RESULT_SUCCESS;
}

} // namespace L0
