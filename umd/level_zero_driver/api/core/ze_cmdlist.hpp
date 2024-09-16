/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/core/source/cmdlist/immediate_cmdlist.hpp"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/include/l0_exception.hpp"

#include <level_zero/ze_api.h>

namespace L0 {
ze_result_t zeCommandListCreate(ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                const ze_command_list_desc_t *desc,
                                ze_command_list_handle_t *phCommandList) {
    L0_HANDLE_EXCEPTION_AND_RETURN(CommandList::create(hContext, hDevice, desc, phCommandList));
}

ze_result_t zeCommandListCreateImmediate(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         const ze_command_queue_desc_t *altdesc,
                                         ze_command_list_handle_t *phCommandList) {
    L0_HANDLE_EXCEPTION_AND_RETURN(
        ImmediateCommandList::create(hContext, hDevice, altdesc, phCommandList));
}

ze_result_t zeCommandListImmediateGetIndex(ze_command_list_handle_t hCommandListImmediate,
                                           uint32_t *pIndex) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeCommandListIsImmediate(ze_command_list_handle_t hCommandList,
                                     ze_bool_t *pIsImmediate) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        CommandList::fromHandle(hCommandList)->isImmediate(pIsImmediate));
}

ze_result_t zeCommandListHostSynchronize(ze_command_list_handle_t hCommandList, uint64_t timeout) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        ImmediateCommandList::fromHandle(hCommandList)->hostSynchronize(timeout));
}

ze_result_t zeCommandListDestroy(ze_command_list_handle_t hCommandList) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(CommandList::fromHandle(hCommandList)->destroy());
}

ze_result_t zeCommandListClose(ze_command_list_handle_t hCommandList) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(CommandList::fromHandle(hCommandList)->close());
}

ze_result_t zeCommandListReset(ze_command_list_handle_t hCommandList) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(CommandList::fromHandle(hCommandList)->reset());
}

ze_result_t zeCommandListAppendWriteGlobalTimestamp(ze_command_list_handle_t hCommandList,
                                                    uint64_t *dstptr,
                                                    ze_event_handle_t hSignalEvent,
                                                    uint32_t numWaitEvents,
                                                    ze_event_handle_t *phWaitEvents) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        CommandList::fromHandle(hCommandList)
            ->appendWriteGlobalTimestamp(dstptr, hSignalEvent, numWaitEvents, phWaitEvents));
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

ze_result_t zeCommandListGetNextCommandIdExp(ze_command_list_handle_t hCommandList,
                                             const ze_mutable_command_id_exp_desc_t *desc,
                                             uint64_t *pCommandId) {
    L0_HANDLE_EXCEPTION_AND_RETURN(
        CommandList::fromHandle(hCommandList)->getNextCommandId(desc, pCommandId));
}

ze_result_t zeCommandListUpdateMutableCommandsExp(ze_command_list_handle_t hCommandList,
                                                  const ze_mutable_commands_exp_desc_t *desc) {
    L0_HANDLE_EXCEPTION_AND_RETURN(
        CommandList::fromHandle(hCommandList)->updateMutableCommands(desc));
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

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListImmediateGetIndex(ze_command_list_handle_t hCommandListImmediate, uint32_t *pIndex) {
    return L0::zeCommandListImmediateGetIndex(hCommandListImmediate, pIndex);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeCommandListIsImmediate(ze_command_list_handle_t hCommandList,
                                                             ze_bool_t *pIsImmediate) {
    return L0::zeCommandListIsImmediate(hCommandList, pIsImmediate);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListHostSynchronize(ze_command_list_handle_t hCommandList, uint64_t timeout) {
    return L0::zeCommandListHostSynchronize(hCommandList, timeout);
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

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListGetNextCommandIdExp(ze_command_list_handle_t hCommandList,
                                 const ze_mutable_command_id_exp_desc_t *desc,
                                 uint64_t *pCommandId) {
    return L0::zeCommandListGetNextCommandIdExp(hCommandList, desc, pCommandId);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListUpdateMutableCommandsExp(ze_command_list_handle_t hCommandList,
                                      const ze_mutable_commands_exp_desc_t *desc) {
    return L0::zeCommandListUpdateMutableCommandsExp(hCommandList, desc);
}

} // extern "C"
