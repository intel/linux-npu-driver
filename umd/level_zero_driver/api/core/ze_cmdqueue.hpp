/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/cmdqueue/cmdqueue.hpp"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include <level_zero/ze_api.h>

namespace L0 {
ze_result_t zeCommandQueueCreate(ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 const ze_command_queue_desc_t *desc,
                                 ze_command_queue_handle_t *phCommandQueue) {
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::CommandQueue::create(hContext, hDevice, desc, phCommandQueue));
}

ze_result_t zeCommandQueueDestroy(ze_command_queue_handle_t hCommandQueue) {
    if (hCommandQueue == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::CommandQueue::fromHandle(hCommandQueue)->destroy());
}

ze_result_t zeCommandQueueExecuteCommandLists(ze_command_queue_handle_t hCommandQueue,
                                              uint32_t numCommandLists,
                                              ze_command_list_handle_t *phCommandLists,
                                              ze_fence_handle_t hFence) {
    if (hCommandQueue == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::CommandQueue::fromHandle(hCommandQueue)
            ->executeCommandLists(numCommandLists, phCommandLists, hFence));
}

ze_result_t zeCommandQueueSynchronize(ze_command_queue_handle_t hCommandQueue, uint64_t timeout) {
    if (hCommandQueue == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::CommandQueue::fromHandle(hCommandQueue)->synchronize(timeout));
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandQueueCreate(ze_context_handle_t hContext,
                     ze_device_handle_t hDevice,
                     const ze_command_queue_desc_t *desc,
                     ze_command_queue_handle_t *phCommandQueue) {
    return L0::zeCommandQueueCreate(hContext, hDevice, desc, phCommandQueue);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeCommandQueueDestroy(ze_command_queue_handle_t hCommandQueue) {
    return L0::zeCommandQueueDestroy(hCommandQueue);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandQueueExecuteCommandLists(ze_command_queue_handle_t hCommandQueue,
                                  uint32_t numCommandLists,
                                  ze_command_list_handle_t *phCommandLists,
                                  ze_fence_handle_t hFence) {
    return L0::zeCommandQueueExecuteCommandLists(hCommandQueue,
                                                 numCommandLists,
                                                 phCommandLists,
                                                 hFence);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandQueueSynchronize(ze_command_queue_handle_t hCommandQueue, uint64_t timeout) {
    return L0::zeCommandQueueSynchronize(hCommandQueue, timeout);
}
} // extern "C"
