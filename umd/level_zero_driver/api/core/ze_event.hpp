/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/event/eventpool.hpp"
#include "level_zero_driver/core/source/event/event.hpp"
#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include <level_zero/ze_api.h>

namespace L0 {
ze_result_t zeEventPoolCreate(ze_context_handle_t hContext,
                              const ze_event_pool_desc_t *desc,
                              uint32_t numDevices,
                              ze_device_handle_t *phDevices,
                              ze_event_pool_handle_t *phEventPool) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::EventPool::create(hContext, desc, numDevices, phDevices, phEventPool));
}

ze_result_t zeEventPoolDestroy(ze_event_pool_handle_t hEventPool) {
    if (hEventPool == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::EventPool::fromHandle(hEventPool)->destroy());
}

ze_result_t zeEventCreate(ze_event_pool_handle_t hEventPool,
                          const ze_event_desc_t *desc,
                          ze_event_handle_t *phEvent) {
    if (hEventPool == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::EventPool::fromHandle(hEventPool)->createEvent(desc, phEvent));
}

ze_result_t zeEventPoolGetIpcHandle(ze_event_pool_handle_t hEventPool,
                                    ze_ipc_event_pool_handle_t *phIpc) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeEventPoolOpenIpcHandle(ze_context_handle_t hContext,
                                     ze_ipc_event_pool_handle_t hIpc,
                                     ze_event_pool_handle_t *phEventPool) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeEventPoolCloseIpcHandle(ze_event_pool_handle_t hEventPool) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeEventDestroy(ze_event_handle_t hEvent) {
    if (hEvent == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Event::fromHandle(hEvent)->destroy());
}

ze_result_t zeCommandListAppendSignalEvent(ze_command_list_handle_t hCommandList,
                                           ze_event_handle_t hEvent) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::CommandList::fromHandle(hCommandList)->appendSignalEvent(hEvent));
}

ze_result_t zeCommandListAppendWaitOnEvents(ze_command_list_handle_t hCommandList,
                                            uint32_t numEvents,
                                            ze_event_handle_t *phEvents) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::CommandList::fromHandle(hCommandList)->appendWaitOnEvents(numEvents, phEvents));
}

ze_result_t zeEventHostSignal(ze_event_handle_t hEvent) {
    if (hEvent == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Event::fromHandle(hEvent)->hostSignal());
}

ze_result_t zeEventHostSynchronize(ze_event_handle_t hEvent, uint64_t timeout) {
    if (hEvent == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Event::fromHandle(hEvent)->hostSynchronize(timeout));
}

ze_result_t zeEventQueryStatus(ze_event_handle_t hEvent) {
    if (hEvent == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Event::fromHandle(hEvent)->queryStatus());
}

ze_result_t zeCommandListAppendEventReset(ze_command_list_handle_t hCommandList,
                                          ze_event_handle_t hEvent) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::CommandList::fromHandle(hCommandList)->appendEventReset(hEvent));
}

ze_result_t zeEventHostReset(ze_event_handle_t hEvent) {
    if (hEvent == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Event::fromHandle(hEvent)->reset());
}

ze_result_t zeEventQueryKernelTimestamp(ze_event_handle_t hEvent,
                                        ze_kernel_timestamp_result_t *timestampType) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeEventPoolCreate(ze_context_handle_t hContext,
                                                      const ze_event_pool_desc_t *desc,
                                                      uint32_t numDevices,
                                                      ze_device_handle_t *phDevices,
                                                      ze_event_pool_handle_t *phEventPool) {
    return L0::zeEventPoolCreate(hContext, desc, numDevices, phDevices, phEventPool);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeEventPoolDestroy(ze_event_pool_handle_t hEventPool) {
    return L0::zeEventPoolDestroy(hEventPool);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeEventCreate(ze_event_pool_handle_t hEventPool,
                                                  const ze_event_desc_t *desc,
                                                  ze_event_handle_t *phEvent) {
    return L0::zeEventCreate(hEventPool, desc, phEvent);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeEventDestroy(ze_event_handle_t hEvent) {
    return L0::zeEventDestroy(hEvent);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeEventPoolGetIpcHandle(ze_event_pool_handle_t hEventPool,
                                                            ze_ipc_event_pool_handle_t *phIpc) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeEventPoolOpenIpcHandle(ze_context_handle_t hContext,
                                                             ze_ipc_event_pool_handle_t hIpc,
                                                             ze_event_pool_handle_t *phEventPool) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeEventPoolCloseIpcHandle(ze_event_pool_handle_t hEventPool) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendSignalEvent(ze_command_list_handle_t hCommandList, ze_event_handle_t hEvent) {
    return L0::zeCommandListAppendSignalEvent(hCommandList, hEvent);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendWaitOnEvents(ze_command_list_handle_t hCommandList,
                                uint32_t numEvents,
                                ze_event_handle_t *phEvents) {
    return L0::zeCommandListAppendWaitOnEvents(hCommandList, numEvents, phEvents);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeEventHostSignal(ze_event_handle_t hEvent) {
    return L0::zeEventHostSignal(hEvent);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeEventHostSynchronize(ze_event_handle_t hEvent,
                                                           uint64_t timeout) {
    return L0::zeEventHostSynchronize(hEvent, timeout);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeEventQueryStatus(ze_event_handle_t hEvent) {
    return L0::zeEventQueryStatus(hEvent);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendEventReset(ze_command_list_handle_t hCommandList, ze_event_handle_t hEvent) {
    return L0::zeCommandListAppendEventReset(hCommandList, hEvent);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeEventHostReset(ze_event_handle_t hEvent) {
    return L0::zeEventHostReset(hEvent);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeEventQueryKernelTimestamp(ze_event_handle_t hEvent, ze_kernel_timestamp_result_t *timestampType) {
    return L0::zeEventQueryKernelTimestamp(hEvent, timestampType);
}
} // extern "C"
