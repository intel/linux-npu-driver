/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "level_zero_driver/core/source/driver/driver.hpp"
#include "level_zero_driver/include/l0_exception.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/zes_api.h>
#include <level_zero/zes_ddi.h>

namespace L0 {
ze_result_t zesInit(zes_init_flags_t flags) {
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::init(flags));
}

ze_result_t zesDriverGet(uint32_t *pCount, zes_driver_handle_t *phDrivers) {
    if (pCount == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::driverHandleGet(pCount, static_cast<ze_driver_handle_t *>(phDrivers)));
}

} // namespace L0

extern "C" {
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetGlobalProcAddrTable(
    ze_api_version_t version,        ///< [in] API version requested
    zes_global_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnInit = L0::zesInit;

    return ZE_RESULT_SUCCESS;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGet = L0::zesDriverGet;

    pDdiTable->pfnEventListen = nullptr;

    pDdiTable->pfnEventListenEx = nullptr;

    return result;
}
}
