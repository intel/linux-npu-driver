/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "level_zero_driver/api/trace/trace_ze_api.hpp"
#include "level_zero_driver/api/trace/trace_ze_api_ddi.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/event.hpp"
#include "level_zero_driver/source/eventpool.hpp"

#include <ze_api.h>
#include <ze_ddi.h>

namespace L0 {
ze_result_t zeEventPoolCreate(ze_context_handle_t hContext,
                              const ze_event_pool_desc_t *desc,
                              uint32_t numDevices,
                              ze_device_handle_t *phDevices,
                              ze_event_pool_handle_t *phEventPool) {
    trace_zeEventPoolCreate(hContext, desc, numDevices, phDevices, phEventPool);
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, EventPool::create(hContext, desc, numDevices, phDevices, phEventPool));

exit:
    trace_zeEventPoolCreate(ret, hContext, desc, numDevices, phDevices, phEventPool);
    return ret;
}

ze_result_t zeEventPoolDestroy(ze_event_pool_handle_t hEventPool) {
    trace_zeEventPoolDestroy(hEventPool);
    ze_result_t ret;

    if (hEventPool == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, EventPool::fromHandle(hEventPool)->destroy());

exit:
    trace_zeEventPoolDestroy(ret, hEventPool);
    return ret;
}

ze_result_t zeEventCreate(ze_event_pool_handle_t hEventPool,
                          const ze_event_desc_t *desc,
                          ze_event_handle_t *phEvent) {
    trace_zeEventCreate(hEventPool, desc, phEvent);
    ze_result_t ret;

    if (hEventPool == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, EventPool::fromHandle(hEventPool)->createEvent(desc, phEvent));

exit:
    trace_zeEventCreate(ret, hEventPool, desc, phEvent);
    return ret;
}

ze_result_t zeEventPoolGetIpcHandle(ze_event_pool_handle_t hEventPool,
                                    ze_ipc_event_pool_handle_t *phIpc) {
    trace_zeEventPoolGetIpcHandle(hEventPool, phIpc);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeEventPoolGetIpcHandle(ret, hEventPool, phIpc);
    return ret;
}

ze_result_t zeEventPoolOpenIpcHandle(ze_context_handle_t hContext,
                                     ze_ipc_event_pool_handle_t hIpc,
                                     ze_event_pool_handle_t *phEventPool) {
    trace_zeEventPoolOpenIpcHandle(hContext, hIpc, phEventPool);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeEventPoolOpenIpcHandle(ret, hContext, hIpc, phEventPool);
    return ret;
}

ze_result_t zeEventPoolCloseIpcHandle(ze_event_pool_handle_t hEventPool) {
    trace_zeEventPoolCloseIpcHandle(hEventPool);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeEventPoolCloseIpcHandle(ret, hEventPool);
    return ret;
}

ze_result_t zeEventDestroy(ze_event_handle_t hEvent) {
    trace_zeEventDestroy(hEvent);
    ze_result_t ret;

    if (hEvent == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, Event::fromHandle(hEvent)->destroy());

exit:
    trace_zeEventDestroy(ret, hEvent);
    return ret;
}

ze_result_t zeEventHostSignal(ze_event_handle_t hEvent) {
    trace_zeEventHostSignal(hEvent);
    ze_result_t ret;

    if (hEvent == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, Event::fromHandle(hEvent)->hostSignal());

exit:
    trace_zeEventHostSignal(ret, hEvent);
    return ret;
}

ze_result_t zeEventHostSynchronize(ze_event_handle_t hEvent, uint64_t timeout) {
    trace_zeEventHostSynchronize(hEvent, timeout);
    ze_result_t ret;

    if (hEvent == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, Event::fromHandle(hEvent)->hostSynchronize(timeout));

exit:
    trace_zeEventHostSynchronize(ret, hEvent, timeout);
    return ret;
}

ze_result_t zeEventQueryStatus(ze_event_handle_t hEvent) {
    trace_zeEventQueryStatus(hEvent);
    ze_result_t ret;

    if (hEvent == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, Event::fromHandle(hEvent)->queryStatus());

exit:
    trace_zeEventQueryStatus(ret, hEvent);
    return ret;
}

ze_result_t zeEventHostReset(ze_event_handle_t hEvent) {
    trace_zeEventHostReset(hEvent);
    ze_result_t ret;

    if (hEvent == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, Event::fromHandle(hEvent)->reset());

exit:
    trace_zeEventHostReset(ret, hEvent);
    return ret;
}

ze_result_t zeEventQueryKernelTimestamp(ze_event_handle_t hEvent,
                                        ze_kernel_timestamp_result_t *timestampType) {
    trace_zeEventQueryKernelTimestamp(hEvent, timestampType);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeEventQueryKernelTimestamp(ret, hEvent, timestampType);
    return ret;
}

ze_result_t zeEventCounterBasedCreate(ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      const ze_event_counter_based_desc_t *desc,
                                      ze_event_handle_t *phEvent) {
    trace_zeEventCounterBasedCreate(hContext, hDevice, desc, phEvent);

    ze_result_t ret;

    L0_HANDLE_EXCEPTION(ret, CounterBasedEvent::create(hContext, desc, hDevice, phEvent));

    trace_zeEventCounterBasedCreate(ret, hContext, hDevice, desc, phEvent);
    return ret;
}

ze_result_t zeEventCounterBasedGetIpcHandle(ze_event_handle_t hEvent,
                                            ze_ipc_event_counter_based_handle_t *phIpc) {
    trace_zeEventCounterBasedGetIpcHandle(hEvent, phIpc);

    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeEventCounterBasedGetIpcHandle(ret, hEvent, phIpc);
    return ret;
}

ze_result_t zeEventCounterBasedOpenIpcHandle(ze_context_handle_t hContext,
                                             ze_ipc_event_counter_based_handle_t hIpc,
                                             ze_event_handle_t *phEvent) {
    trace_zeEventCounterBasedOpenIpcHandle(hContext, hIpc, phEvent);

    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeEventCounterBasedOpenIpcHandle(ret, hContext, hIpc, phEvent);
    return ret;
}

ze_result_t zeEventCounterBasedCloseIpcHandle(ze_event_handle_t hEvent) {
    trace_zeEventCounterBasedCloseIpcHandle(hEvent);

    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeEventCounterBasedCloseIpcHandle(ret, hEvent);
    return ret;
}

ze_result_t zeEventCounterBasedGetDeviceAddress(ze_event_handle_t hEvent,
                                                uint64_t *completionValue,
                                                uint64_t *deviceAddress) {
    trace_zeEventCounterBasedGetDeviceAddress(hEvent, completionValue, deviceAddress);
    ze_result_t ret;

    if (hEvent == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    if (deviceAddress == nullptr || completionValue == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    {
        auto *counterEvent = dynamic_cast<CounterBasedEvent *>(Event::fromHandle(hEvent));
        if (counterEvent == nullptr) {
            ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
            goto exit;
        }
        *completionValue = counterEvent->getCompletionValue();
        *deviceAddress = counterEvent->getDeviceAddress();
    }
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeEventCounterBasedGetDeviceAddress(ret, hEvent, completionValue, deviceAddress);
    return ret;
}

ze_result_t zeEventGetCounterBasedFlags(ze_event_handle_t hEvent,
                                        ze_event_counter_based_flags_t *flags) {
    trace_zeEventGetCounterBasedFlags(hEvent, flags);
    ze_result_t ret;

    if (hEvent == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    if (flags == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    {
        auto *counterEvent = dynamic_cast<CounterBasedEvent *>(Event::fromHandle(hEvent));
        if (counterEvent) {
            *flags = counterEvent->getCreationFlags();
        } else {
            *flags = 0;
        }
    }
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeEventGetCounterBasedFlags(ret, hEvent, flags);
    return ret;
}

ze_event_pool_dditable_t zeEventPoolDdiTable = {.pfnCreate = zeEventPoolCreate,
                                                .pfnDestroy = zeEventPoolDestroy,
                                                .pfnGetIpcHandle = zeEventPoolGetIpcHandle,
                                                .pfnOpenIpcHandle = zeEventPoolOpenIpcHandle,
                                                .pfnCloseIpcHandle = zeEventPoolCloseIpcHandle,
                                                .pfnPutIpcHandle = nullptr,
                                                .pfnGetContextHandle = nullptr,
                                                .pfnGetFlags = nullptr};

ze_event_dditable_t zeEventDdiTable = {
    .pfnCreate = zeEventCreate,
    .pfnDestroy = zeEventDestroy,
    .pfnHostSignal = zeEventHostSignal,
    .pfnHostSynchronize = zeEventHostSynchronize,
    .pfnQueryStatus = zeEventQueryStatus,
    .pfnHostReset = zeEventHostReset,
    .pfnQueryKernelTimestamp = zeEventQueryKernelTimestamp,
    .pfnQueryKernelTimestampsExt = nullptr,
    .pfnGetEventPool = nullptr,
    .pfnGetSignalScope = nullptr,
    .pfnGetWaitScope = nullptr,
    .pfnCounterBasedCreate = zeEventCounterBasedCreate,
    .pfnCounterBasedGetIpcHandle = zeEventCounterBasedGetIpcHandle,
    .pfnCounterBasedOpenIpcHandle = zeEventCounterBasedOpenIpcHandle,
    .pfnCounterBasedCloseIpcHandle = zeEventCounterBasedCloseIpcHandle,
    .pfnCounterBasedGetDeviceAddress = zeEventCounterBasedGetDeviceAddress,
    .pfnGetCounterBasedFlags = zeEventGetCounterBasedFlags};
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetEventPoolProcAddrTable(ze_api_version_t version, ze_event_pool_dditable_t *pDdiTable) {
    trace_zeGetEventPoolProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    if (version >= ZE_API_VERSION_1_0) {
        pDdiTable->pfnCreate = L0::zeEventPoolCreate;
        pDdiTable->pfnDestroy = L0::zeEventPoolDestroy;
        pDdiTable->pfnGetIpcHandle = L0::zeEventPoolGetIpcHandle;
        pDdiTable->pfnOpenIpcHandle = L0::zeEventPoolOpenIpcHandle;
        pDdiTable->pfnCloseIpcHandle = L0::zeEventPoolCloseIpcHandle;
    }

    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetEventPoolProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetEventProcAddrTable(ze_api_version_t version,
                                                            ze_event_dditable_t *pDdiTable) {
    trace_zeGetEventProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    if (version >= ZE_API_VERSION_1_0) {
        pDdiTable->pfnCreate = L0::zeEventCreate;
        pDdiTable->pfnDestroy = L0::zeEventDestroy;
        pDdiTable->pfnHostSignal = L0::zeEventHostSignal;
        pDdiTable->pfnHostSynchronize = L0::zeEventHostSynchronize;
        pDdiTable->pfnQueryStatus = L0::zeEventQueryStatus;
        pDdiTable->pfnHostReset = L0::zeEventHostReset;
        pDdiTable->pfnQueryKernelTimestamp = L0::zeEventQueryKernelTimestamp;
        pDdiTable->pfnCounterBasedCreate = L0::zeEventCounterBasedCreate;
        pDdiTable->pfnCounterBasedGetIpcHandle = L0::zeEventCounterBasedGetIpcHandle;
        pDdiTable->pfnCounterBasedOpenIpcHandle = L0::zeEventCounterBasedOpenIpcHandle;
        pDdiTable->pfnCounterBasedCloseIpcHandle = L0::zeEventCounterBasedCloseIpcHandle;
        pDdiTable->pfnCounterBasedGetDeviceAddress = L0::zeEventCounterBasedGetDeviceAddress;
        pDdiTable->pfnGetCounterBasedFlags = L0::zeEventGetCounterBasedFlags;
    }

    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetEventProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
