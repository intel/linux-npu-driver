/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "level_zero_driver/api/trace/trace_ze_api.hpp"
#include "level_zero_driver/api/trace/trace_ze_api_ddi.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/cmdqueue.hpp"
#include "level_zero_driver/source/fence.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

namespace L0 {
ze_result_t zeFenceCreate(ze_command_queue_handle_t hCommandQueue,
                          const ze_fence_desc_t *desc,
                          ze_fence_handle_t *phFence) {
    trace_zeFenceCreate(hCommandQueue, desc, phFence);
    ze_result_t ret;

    if (hCommandQueue == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, CommandQueue::fromHandle(hCommandQueue)->createFence(desc, phFence));

exit:
    trace_zeFenceCreate(ret, hCommandQueue, desc, phFence);
    return ret;
}

ze_result_t zeFenceDestroy(ze_fence_handle_t hFence) {
    trace_zeFenceDestroy(hFence);
    ze_result_t ret;

    if (hFence == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, Fence::fromHandle(hFence)->destroy());

exit:
    trace_zeFenceDestroy(ret, hFence);
    return ret;
}

ze_result_t zeFenceHostSynchronize(ze_fence_handle_t hFence, uint64_t timeout) {
    trace_zeFenceHostSynchronize(hFence, timeout);
    ze_result_t ret;

    if (hFence == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, Fence::fromHandle(hFence)->synchronize(timeout));

exit:
    trace_zeFenceHostSynchronize(ret, hFence, timeout);
    return ret;
}

ze_result_t zeFenceQueryStatus(ze_fence_handle_t hFence) {
    trace_zeFenceQueryStatus(hFence);
    ze_result_t ret;

    if (hFence == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, Fence::fromHandle(hFence)->queryStatus());

exit:
    trace_zeFenceQueryStatus(ret, hFence);
    return ret;
}

ze_result_t zeFenceReset(ze_fence_handle_t hFence) {
    trace_zeFenceReset(hFence);
    ze_result_t ret;

    if (hFence == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, Fence::fromHandle(hFence)->reset());

exit:
    trace_zeFenceReset(ret, hFence);
    return ret;
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeGetFenceProcAddrTable(ze_api_version_t version,
                                                            ze_fence_dditable_t *pDdiTable) {
    trace_zeGetFenceProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnCreate = L0::zeFenceCreate;
    pDdiTable->pfnDestroy = L0::zeFenceDestroy;
    pDdiTable->pfnHostSynchronize = L0::zeFenceHostSynchronize;
    pDdiTable->pfnQueryStatus = L0::zeFenceQueryStatus;
    pDdiTable->pfnReset = L0::zeFenceReset;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetFenceProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
