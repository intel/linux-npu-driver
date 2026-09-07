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
#include "level_zero_driver/source/cmdqueue.hpp"
#include "level_zero_driver/source/context.hpp"
#include "level_zero_driver/source/fence.hpp"

#include <ze_api.h>
#include <ze_ddi.h>

namespace L0 {
ze_result_t zeCommandQueueCreate(ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 const ze_command_queue_desc_t *desc,
                                 ze_command_queue_handle_t *phCommandQueue) {
    trace_zeCommandQueueCreate(hContext, hDevice, desc, phCommandQueue);
    ze_result_t ret;

    L0_HANDLE_EXCEPTION(ret, L0::CommandQueue::create(hContext, hDevice, desc, phCommandQueue));

    trace_zeCommandQueueCreate(ret, hContext, hDevice, desc, phCommandQueue);
    return ret;
}

ze_result_t zeCommandQueueDestroy(ze_command_queue_handle_t hCommandQueue) {
    trace_zeCommandQueueDestroy(hCommandQueue);
    ze_result_t ret;

    if (hCommandQueue == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::CommandQueue::fromHandle(hCommandQueue)->destroy());

exit:
    trace_zeCommandQueueDestroy(ret, hCommandQueue);
    return ret;
}

ze_result_t zeCommandQueueExecuteCommandLists(ze_command_queue_handle_t hCommandQueue,
                                              uint32_t numCommandLists,
                                              ze_command_list_handle_t *phCommandLists,
                                              ze_fence_handle_t hFence) {
    trace_zeCommandQueueExecuteCommandLists(hCommandQueue, numCommandLists, phCommandLists, hFence);
    ze_result_t ret;

    if (hCommandQueue == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        L0::CommandQueue::fromHandle(hCommandQueue)
                            ->executeCommandLists(numCommandLists, phCommandLists, hFence));

exit:
    trace_zeCommandQueueExecuteCommandLists(ret,
                                            hCommandQueue,
                                            numCommandLists,
                                            phCommandLists,
                                            hFence);
    return ret;
}

ze_result_t zeCommandQueueSynchronize(ze_command_queue_handle_t hCommandQueue, uint64_t timeout) {
    trace_zeCommandQueueSynchronize(hCommandQueue, timeout);
    ze_result_t ret;

    if (hCommandQueue == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::CommandQueue::fromHandle(hCommandQueue)->synchronize(timeout));

exit:
    trace_zeCommandQueueSynchronize(ret, hCommandQueue, timeout);
    return ret;
}

ze_result_t zeCommandQueueGetOrdinal(ze_command_queue_handle_t hCommandQueue, uint32_t *pOrdinal) {
    trace_zeCommandQueueGetOrdinal(hCommandQueue, pOrdinal);
    ze_result_t ret;

    if (hCommandQueue == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::CommandQueue::fromHandle(hCommandQueue)->getOrdinal(pOrdinal));

exit:
    trace_zeCommandQueueGetOrdinal(ret, hCommandQueue, pOrdinal);
    return ret;
}

ze_result_t zeCommandQueueGetIndex(ze_command_queue_handle_t hCommandQueue, uint32_t *pIndex) {
    trace_zeCommandQueueGetIndex(hCommandQueue, pIndex);
    ze_result_t ret;

    if (hCommandQueue == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::CommandQueue::fromHandle(hCommandQueue)->getIndex(pIndex));

exit:
    trace_zeCommandQueueGetIndex(ret, hCommandQueue, pIndex);
    return ret;
}

ze_result_t zeCommandQueueGetFlags(ze_command_queue_handle_t hCommandQueue,
                                   ze_command_queue_flags_t *pFlags) {
    trace_zeCommandQueueGetFlags(hCommandQueue, pFlags);
    ze_result_t ret;

    if (hCommandQueue == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::CommandQueue::fromHandle(hCommandQueue)->getFlags(pFlags));

exit:
    trace_zeCommandQueueGetFlags(ret, hCommandQueue, pFlags);
    return ret;
}

ze_result_t zeCommandQueueGetMode(ze_command_queue_handle_t hCommandQueue,
                                  ze_command_queue_mode_t *pMode) {
    trace_zeCommandQueueGetMode(hCommandQueue, pMode);
    ze_result_t ret;

    if (hCommandQueue == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::CommandQueue::fromHandle(hCommandQueue)->getMode(pMode));

exit:
    trace_zeCommandQueueGetMode(ret, hCommandQueue, pMode);
    return ret;
}

ze_result_t zeCommandQueueGetPriority(ze_command_queue_handle_t hCommandQueue,
                                      ze_command_queue_priority_t *pPriority) {
    trace_zeCommandQueueGetPriority(hCommandQueue, pPriority);
    ze_result_t ret;

    if (hCommandQueue == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::CommandQueue::fromHandle(hCommandQueue)->getPriority(pPriority));

exit:
    trace_zeCommandQueueGetPriority(ret, hCommandQueue, pPriority);
    return ret;
}

ze_command_queue_dditable_t zeCommandQueueDdiTable = {.pfnCreate = zeCommandQueueCreate,
                                                      .pfnDestroy = zeCommandQueueDestroy,
                                                      .pfnExecuteCommandLists =
                                                          zeCommandQueueExecuteCommandLists,
                                                      .pfnSynchronize = zeCommandQueueSynchronize,
                                                      .pfnGetOrdinal = zeCommandQueueGetOrdinal,
                                                      .pfnGetIndex = zeCommandQueueGetIndex,
                                                      .pfnGetFlags = zeCommandQueueGetFlags,
                                                      .pfnGetMode = zeCommandQueueGetMode,
                                                      .pfnGetPriority = zeCommandQueueGetPriority};
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetCommandQueueProcAddrTable(ze_api_version_t version, ze_command_queue_dditable_t *pDdiTable) {
    trace_zeGetCommandQueueProcAddrTable(version, pDdiTable);
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
        pDdiTable->pfnCreate = L0::zeCommandQueueCreate;
        pDdiTable->pfnDestroy = L0::zeCommandQueueDestroy;
        pDdiTable->pfnExecuteCommandLists = L0::zeCommandQueueExecuteCommandLists;
        pDdiTable->pfnSynchronize = L0::zeCommandQueueSynchronize;
        pDdiTable->pfnGetOrdinal = L0::zeCommandQueueGetOrdinal;
        pDdiTable->pfnGetIndex = L0::zeCommandQueueGetIndex;
        pDdiTable->pfnGetFlags = L0::zeCommandQueueGetFlags;
        pDdiTable->pfnGetMode = L0::zeCommandQueueGetMode;
        pDdiTable->pfnGetPriority = L0::zeCommandQueueGetPriority;
    }

    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetCommandQueueProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
