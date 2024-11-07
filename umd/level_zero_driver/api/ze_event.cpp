/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "level_zero_driver/core/source/event/event.hpp"
#include "level_zero_driver/core/source/event/eventpool.hpp"
#include "level_zero_driver/include/l0_exception.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

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
        EventPool::create(hContext, desc, numDevices, phDevices, phEventPool));
}

ze_result_t zeEventPoolDestroy(ze_event_pool_handle_t hEventPool) {
    if (hEventPool == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(EventPool::fromHandle(hEventPool)->destroy());
}

ze_result_t zeEventCreate(ze_event_pool_handle_t hEventPool,
                          const ze_event_desc_t *desc,
                          ze_event_handle_t *phEvent) {
    if (hEventPool == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(EventPool::fromHandle(hEventPool)->createEvent(desc, phEvent));
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
    L0_HANDLE_EXCEPTION_AND_RETURN(Event::fromHandle(hEvent)->destroy());
}

ze_result_t zeEventHostSignal(ze_event_handle_t hEvent) {
    if (hEvent == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(Event::fromHandle(hEvent)->hostSignal());
}

ze_result_t zeEventHostSynchronize(ze_event_handle_t hEvent, uint64_t timeout) {
    if (hEvent == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(Event::fromHandle(hEvent)->hostSynchronize(timeout));
}

ze_result_t zeEventQueryStatus(ze_event_handle_t hEvent) {
    if (hEvent == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(Event::fromHandle(hEvent)->queryStatus());
}

ze_result_t zeEventHostReset(ze_event_handle_t hEvent) {
    if (hEvent == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(Event::fromHandle(hEvent)->reset());
}

ze_result_t zeEventQueryKernelTimestamp(ze_event_handle_t hEvent,
                                        ze_kernel_timestamp_result_t *timestampType) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetEventPoolProcAddrTable(ze_api_version_t version, ze_event_pool_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnCreate = L0::zeEventPoolCreate;
    pDdiTable->pfnDestroy = L0::zeEventPoolDestroy;
    pDdiTable->pfnGetIpcHandle = L0::zeEventPoolGetIpcHandle;
    pDdiTable->pfnOpenIpcHandle = L0::zeEventPoolOpenIpcHandle;
    pDdiTable->pfnCloseIpcHandle = L0::zeEventPoolCloseIpcHandle;
    return ZE_RESULT_SUCCESS;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetEventProcAddrTable(ze_api_version_t version,
                                                            ze_event_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnCreate = L0::zeEventCreate;
    pDdiTable->pfnDestroy = L0::zeEventDestroy;
    pDdiTable->pfnHostSignal = L0::zeEventHostSignal;
    pDdiTable->pfnHostSynchronize = L0::zeEventHostSynchronize;
    pDdiTable->pfnQueryStatus = L0::zeEventQueryStatus;
    pDdiTable->pfnHostReset = L0::zeEventHostReset;
    pDdiTable->pfnQueryKernelTimestamp = L0::zeEventQueryKernelTimestamp;
    return ZE_RESULT_SUCCESS;
}
}
