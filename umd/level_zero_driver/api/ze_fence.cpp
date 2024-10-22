/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "level_zero_driver/core/source/cmdqueue/cmdqueue.hpp"
#include "level_zero_driver/core/source/fence/fence.hpp"
#include "level_zero_driver/include/l0_exception.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

namespace L0 {
ze_result_t zeFenceCreate(ze_command_queue_handle_t hCommandQueue,
                          const ze_fence_desc_t *desc,
                          ze_fence_handle_t *phFence) {
    if (hCommandQueue == nullptr)
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    L0_HANDLE_EXCEPTION_AND_RETURN(
        CommandQueue::fromHandle(hCommandQueue)->createFence(desc, phFence));
}

ze_result_t zeFenceDestroy(ze_fence_handle_t hFence) {
    if (hFence == nullptr)
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    L0_HANDLE_EXCEPTION_AND_RETURN(Fence::fromHandle(hFence)->destroy());
}

ze_result_t zeFenceHostSynchronize(ze_fence_handle_t hFence, uint64_t timeout) {
    if (hFence == nullptr)
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    L0_HANDLE_EXCEPTION_AND_RETURN(Fence::fromHandle(hFence)->synchronize(timeout));
}

ze_result_t zeFenceQueryStatus(ze_fence_handle_t hFence) {
    if (hFence == nullptr)
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    L0_HANDLE_EXCEPTION_AND_RETURN(Fence::fromHandle(hFence)->queryStatus());
}

ze_result_t zeFenceReset(ze_fence_handle_t hFence) {
    if (hFence == nullptr)
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    L0_HANDLE_EXCEPTION_AND_RETURN(Fence::fromHandle(hFence)->reset());
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeGetFenceProcAddrTable(ze_api_version_t version,
                                                            ze_fence_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnCreate = L0::zeFenceCreate;
    pDdiTable->pfnDestroy = L0::zeFenceDestroy;
    pDdiTable->pfnHostSynchronize = L0::zeFenceHostSynchronize;
    pDdiTable->pfnQueryStatus = L0::zeFenceQueryStatus;
    pDdiTable->pfnReset = L0::zeFenceReset;
    return ZE_RESULT_SUCCESS;
}
}
