/*
 * Copyright (C) 2026-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/api/ext/ze_context.hpp"

#include "level_zero_driver/api/trace/trace_ze_context_npu_ext.hpp"
#include "level_zero_driver/api/zet_misc.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/context.hpp"

#include <loader/ze_loader.h>
#include <ze_api.h>

namespace L0 {
ze_result_t zeContextSetProperties(ze_context_handle_t hContext,
                                   ze_context_properties_npu_ext_t *pContextProperties) {
    trace_zeContextSetProperties(hContext, pContextProperties);
    ze_result_t ret;

    ret = translateHandle(ZEL_HANDLE_CONTEXT, hContext);
    if (ret != ZE_RESULT_SUCCESS) {
        goto exit;
    }

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    if (pContextProperties == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::Context::fromHandle(hContext)->setProperties(pContextProperties));

exit:
    trace_zeContextSetProperties(ret, hContext, pContextProperties);
    return ret;
}

ze_result_t zeContextReleaseMemory(ze_context_handle_t hContext) {
    trace_zeContextReleaseMemory(hContext);
    ze_result_t ret;

    ret = translateHandle(ZEL_HANDLE_CONTEXT, hContext);
    if (ret != ZE_RESULT_SUCCESS) {
        goto exit;
    }

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::Context::fromHandle(hContext)->releaseMemory());

exit:
    trace_zeContextReleaseMemory(ret, hContext);
    return ret;
}

} // namespace L0
