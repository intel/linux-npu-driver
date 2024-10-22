/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/include/l0_exception.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/zes_api.h>
#include <level_zero/zes_ddi.h>

namespace L0 {

ze_result_t zesEngineGetProperties(zes_engine_handle_t hEngine,
                                   zes_engine_properties_t *pProperties) {
    if (hEngine == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Device::fromHandle(reinterpret_cast<ze_device_handle_t>(hEngine))
            ->engineGetProperties(pProperties));
}

ze_result_t zesEngineGetActivity(zes_engine_handle_t hEngine, zes_engine_stats_t *pStats) {
    if (hEngine == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Device::fromHandle(reinterpret_cast<ze_device_handle_t>(hEngine))
            ->engineGetActivity(pStats));
}

} // namespace L0

extern "C" {
///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Engine table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetEngineProcAddrTable(
    ze_api_version_t version,        ///< [in] API version requested
    zes_engine_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = L0::zesEngineGetProperties;

    pDdiTable->pfnGetActivity = L0::zesEngineGetActivity;

    pDdiTable->pfnGetActivityExt = nullptr;

    return result;
}
}
