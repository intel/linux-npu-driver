/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/core/source/context/context.hpp"

#include <level_zero/ze_api.h>

namespace L0 {
ze_result_t zeCommandListCreate(ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                const ze_command_list_desc_t *desc,
                                ze_command_list_handle_t *phCommandList) {
    return L0::CommandList::create(hContext, hDevice, desc, phCommandList);
}

ze_result_t zeCommandListCreateImmediate(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         const ze_command_queue_desc_t *altdesc,
                                         ze_command_list_handle_t *phCommandList) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeCommandListDestroy(ze_command_list_handle_t hCommandList) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::CommandList::fromHandle(hCommandList)->destroy();
}

ze_result_t zeCommandListClose(ze_command_list_handle_t hCommandList) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::CommandList::fromHandle(hCommandList)->close();
}

ze_result_t zeCommandListReset(ze_command_list_handle_t hCommandList) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::CommandList::fromHandle(hCommandList)->reset();
}

ze_result_t zeCommandListAppendWriteGlobalTimestamp(ze_command_list_handle_t hCommandList,
                                                    uint64_t *dstptr,
                                                    ze_event_handle_t hSignalEvent,
                                                    uint32_t numWaitEvents,
                                                    ze_event_handle_t *phWaitEvents) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::CommandList::fromHandle(hCommandList)
        ->appendWriteGlobalTimestamp(dstptr, hSignalEvent, numWaitEvents, phWaitEvents);
}

ze_result_t zeCommandListAppendQueryKernelTimestamps(ze_command_list_handle_t hCommandList,
                                                     uint32_t numEvents,
                                                     ze_event_handle_t *phEvents,
                                                     void *dstptr,
                                                     const size_t *pOffsets,
                                                     ze_event_handle_t hSignalEvent,
                                                     uint32_t numWaitEvents,
                                                     ze_event_handle_t *phWaitEvents) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeCommandListCreate(ze_context_handle_t hContext,
                                                        ze_device_handle_t hDevice,
                                                        const ze_command_list_desc_t *desc,
                                                        ze_command_list_handle_t *phCommandList) {
    return L0::zeCommandListCreate(hContext, hDevice, desc, phCommandList);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListCreateImmediate(ze_context_handle_t hContext,
                             ze_device_handle_t hDevice,
                             const ze_command_queue_desc_t *altdesc,
                             ze_command_list_handle_t *phCommandList) {
    return L0::zeCommandListCreateImmediate(hContext, hDevice, altdesc, phCommandList);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeCommandListDestroy(ze_command_list_handle_t hCommandList) {
    return L0::zeCommandListDestroy(hCommandList);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeCommandListClose(ze_command_list_handle_t hCommandList) {
    return L0::zeCommandListClose(hCommandList);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeCommandListReset(ze_command_list_handle_t hCommandList) {
    return L0::zeCommandListReset(hCommandList);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendWriteGlobalTimestamp(ze_command_list_handle_t hCommandList,
                                        uint64_t *dstptr,
                                        ze_event_handle_t hSignalEvent,
                                        uint32_t numWaitEvents,
                                        ze_event_handle_t *phWaitEvents) {
    return L0::zeCommandListAppendWriteGlobalTimestamp(hCommandList,
                                                       dstptr,
                                                       hSignalEvent,
                                                       numWaitEvents,
                                                       phWaitEvents);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendQueryKernelTimestamps(ze_command_list_handle_t hCommandList,
                                         uint32_t numEvents,
                                         ze_event_handle_t *phEvents,
                                         void *dstptr,
                                         const size_t *pOffsets,
                                         ze_event_handle_t hSignalEvent,
                                         uint32_t numWaitEvents,
                                         ze_event_handle_t *phWaitEvents) {
    return L0::zeCommandListAppendQueryKernelTimestamps(hCommandList,
                                                        numEvents,
                                                        phEvents,
                                                        dstptr,
                                                        pOffsets,
                                                        hSignalEvent,
                                                        numWaitEvents,
                                                        phWaitEvents);
}
} // extern "C"
