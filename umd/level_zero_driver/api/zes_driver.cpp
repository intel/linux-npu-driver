/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "level_zero_driver/api/trace/trace_zes_api.hpp"
#include "level_zero_driver/api/trace/trace_zes_api_ddi.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/driver.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/zes_api.h>
#include <level_zero/zes_ddi.h>

namespace L0 {
ze_result_t zesInit(zes_init_flags_t flags) {
    trace_zesInit(flags);
    ze_result_t ret;

    L0_HANDLE_EXCEPTION(ret, L0::init(flags));

    trace_zesInit(ret, flags);
    return ret;
}

ze_result_t zesDriverGet(uint32_t *pCount, zes_driver_handle_t *phDrivers) {
    trace_zesDriverGet(pCount, phDrivers);
    ze_result_t ret;

    if (pCount == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret,
                        L0::driverHandleGet(pCount, static_cast<ze_driver_handle_t *>(phDrivers)));

exit:
    trace_zesDriverGet(ret, pCount, phDrivers);
    return ret;
}

} // namespace L0

extern "C" {
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetGlobalProcAddrTable(
    ze_api_version_t version,        ///< [in] API version requested
    zes_global_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetGlobalProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    ret = ZE_RESULT_SUCCESS;
    pDdiTable->pfnInit = L0::zesInit;

exit:
    trace_zesGetGlobalProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Driver table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetDriverProcAddrTable(
    ze_api_version_t version,        ///< [in] API version requested
    zes_driver_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetDriverProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    ret = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGet = L0::zesDriverGet;

    pDdiTable->pfnEventListen = nullptr;

    pDdiTable->pfnEventListenEx = nullptr;

exit:
    trace_zesGetDriverProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
