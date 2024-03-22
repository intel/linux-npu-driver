/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/event/eventpool.hpp"
#include "level_zero_driver/core/source/event/event.hpp"
#include "level_zero_driver/core/source/context/context.hpp"

#include "level_zero_driver/include/l0_exception.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"

#include <level_zero/ze_api.h>

namespace L0 {

ze_result_t EventPool::create(ze_context_handle_t hContext,
                              const ze_event_pool_desc_t *desc,
                              uint32_t numDevices,
                              ze_device_handle_t *phDevices,
                              ze_event_pool_handle_t *phEventPool) {
    if (hContext == nullptr) {
        LOG_E("Invalid hContext pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (desc == nullptr) {
        LOG_E("Invalid desc pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (numDevices > 0 && phDevices == nullptr) {
        LOG_E("Invalid number of devices");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }
    if (phEventPool == nullptr) {
        LOG_E("Invalid phEventPool pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (desc->count == 0) {
        LOG_E("Invalid descriptor count");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    try {
        Context *pContext = Context::fromHandle(hContext);
        auto eventPool = std::make_unique<EventPool>(pContext, desc);

        *phEventPool = eventPool.get();
        pContext->appendObject(std::move(eventPool));

        LOG_I("EventPool created - %p", *phEventPool);
    } catch (const DriverError &err) {
        return err.result();
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t EventPool::destroy() {
    pContext->removeObject(this);
    LOG_I("EventPool destroyed - %p", this);
    return ZE_RESULT_SUCCESS;
}

EventPool::EventPool(Context *pContext, const ze_event_pool_desc_t *desc)
    : pContext(pContext)
    , ctx(pContext->getDeviceContext())
    , pEventPool(nullptr)
    , events(desc->count) {
    pEventPool =
        ctx->createInternalBufferObject(sizeof(VPU::VPUEventCommand::JsmEventData) * events.size(),
                                        VPU::VPUBufferObject::Type::CachedFw);
    L0_THROW_WHEN(pEventPool == nullptr,
                  "Failed to allocate buffer object for event pool",
                  ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY);
}

EventPool::~EventPool() {
    if (pEventPool != nullptr) {
        ctx->freeMemAlloc(pEventPool);
    }
}

VPU::VPUEventCommand::KMDEventDataType *EventPool::getEventCpuAddress(uint32_t index) {
    auto eventPtr =
        reinterpret_cast<VPU::VPUEventCommand::JsmEventData *>(pEventPool->getBasePointer());
    return reinterpret_cast<VPU::VPUEventCommand::KMDEventDataType *>(eventPtr + index);
}

ze_result_t EventPool::createEvent(const ze_event_desc_t *desc, ze_event_handle_t *phEvent) {
    if (desc == nullptr) {
        LOG_E("Invalid desc pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (phEvent == nullptr) {
        LOG_E("Invalid phEvent pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    uint32_t index = desc->index;
    if (index >= events.size()) {
        LOG_E("Index is out of bound");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }
    if (events[index] != nullptr) {
        LOG_E("Event from index %u is already in use", index);
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto *eventPtr = getEventCpuAddress(index);
        uint64_t vpuAddr = ctx->getBufferVPUAddress(eventPtr);
        L0_THROW_WHEN(vpuAddr == 0,
                      "Failed to get VPU address from cpu pointer",
                      ZE_RESULT_ERROR_UNKNOWN);

        events[index] = std::make_unique<Event>(ctx, eventPtr, vpuAddr, [this, index]() {
            events[index].reset();
        });
        *phEvent = events[index].get();

        LOG_I("Event created - %p", *phEvent);
    } catch (const DriverError &err) {
        return err.result();
    }

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
