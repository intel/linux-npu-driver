/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>

#include "event.hpp"
#include "level_zero_driver/include/l0_handler.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"

#include <level_zero/ze_api.h>
#include <memory>
#include <vector>

namespace L0 {
struct Context;
} // namespace L0
namespace VPU {
class VPUBufferObject;
class VPUDeviceContext;
} // namespace VPU

struct _ze_event_pool_handle_t {};

namespace L0 {

struct EventPool : _ze_event_pool_handle_t, IContextObject {
    EventPool(Context *pContext, const ze_event_pool_desc_t *desc);

    inline ze_event_pool_handle_t toHandle() { return this; }
    static EventPool *fromHandle(ze_event_pool_handle_t handle) {
        return static_cast<EventPool *>(handle);
    }

    static ze_result_t create(ze_context_handle_t hContext,
                              const ze_event_pool_desc_t *desc,
                              uint32_t numDevices,
                              ze_device_handle_t *phDevices,
                              ze_event_pool_handle_t *phEventPool);
    static EventPool *create(Context *pContext, const ze_event_pool_desc_t *desc);
    ze_result_t destroy();
    ze_result_t createEvent(const ze_event_desc_t *desc, ze_event_handle_t *phEvent);

    VPU::VPUEventCommand::KMDEventDataType *getEventCpuAddress(uint32_t index);
    const std::shared_ptr<VPU::VPUBufferObject> &getEventBase() { return pEventPool; }
    bool freeEvent(uint32_t index);

  private:
    Context *pContext = nullptr;
    VPU::VPUDeviceContext *ctx = nullptr;
    std::shared_ptr<VPU::VPUBufferObject> pEventPool = nullptr;
    std::vector<std::unique_ptr<Event>> events;
};

} // namespace L0
