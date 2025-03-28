/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/api/trace/trace_zes_api_ddi.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/zes_ddi.h>

extern "C" {
///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Diagnostics table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetDiagnosticsProcAddrTable(
    ze_api_version_t version,             ///< [in] API version requested
    zes_diagnostics_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetDiagnosticsProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetTests = nullptr;

    pDdiTable->pfnRunTests = nullptr;

exit:
    trace_zesGetDiagnosticsProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's FabricPort table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetFabricPortProcAddrTable(
    ze_api_version_t version,             ///< [in] API version requested
    zes_fabric_port_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetFabricPortProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetLinkType = nullptr;

    pDdiTable->pfnGetConfig = nullptr;

    pDdiTable->pfnSetConfig = nullptr;

    pDdiTable->pfnGetState = nullptr;

    pDdiTable->pfnGetThroughput = nullptr;

exit:
    trace_zesGetFabricPortProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Fan table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetFanProcAddrTable(
    ze_api_version_t version,     ///< [in] API version requested
    zes_fan_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetFanProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetConfig = nullptr;

    pDdiTable->pfnSetDefaultMode = nullptr;

    pDdiTable->pfnSetFixedSpeedMode = nullptr;

    pDdiTable->pfnSetSpeedTableMode = nullptr;

    pDdiTable->pfnGetState = nullptr;

exit:
    trace_zesGetFanProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Firmware table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetFirmwareProcAddrTable(
    ze_api_version_t version,          ///< [in] API version requested
    zes_firmware_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetFirmwareProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnFlash = nullptr;

exit:
    trace_zesGetFirmwareProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Frequency table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetFrequencyProcAddrTable(
    ze_api_version_t version,           ///< [in] API version requested
    zes_frequency_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetFrequencyProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetAvailableClocks = nullptr;

    pDdiTable->pfnGetRange = nullptr;

    pDdiTable->pfnSetRange = nullptr;

    pDdiTable->pfnGetState = nullptr;

    pDdiTable->pfnGetThrottleTime = nullptr;

    pDdiTable->pfnOcGetCapabilities = nullptr;

    pDdiTable->pfnOcGetFrequencyTarget = nullptr;

    pDdiTable->pfnOcSetFrequencyTarget = nullptr;

    pDdiTable->pfnOcGetVoltageTarget = nullptr;

    pDdiTable->pfnOcSetVoltageTarget = nullptr;

    pDdiTable->pfnOcSetMode = nullptr;

    pDdiTable->pfnOcGetMode = nullptr;

    pDdiTable->pfnOcGetIccMax = nullptr;

    pDdiTable->pfnOcSetIccMax = nullptr;

    pDdiTable->pfnOcGetTjMax = nullptr;

    pDdiTable->pfnOcSetTjMax = nullptr;

exit:
    trace_zesGetFrequencyProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Led table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetLedProcAddrTable(
    ze_api_version_t version,     ///< [in] API version requested
    zes_led_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetLedProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetState = nullptr;

    pDdiTable->pfnSetState = nullptr;

    pDdiTable->pfnSetColor = nullptr;

exit:
    trace_zesGetLedProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Memory table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetMemoryProcAddrTable(
    ze_api_version_t version,        ///< [in] API version requested
    zes_memory_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetMemoryProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetState = nullptr;

    pDdiTable->pfnGetBandwidth = nullptr;

exit:
    trace_zesGetMemoryProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's PerformanceFactor table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetPerformanceFactorProcAddrTable(
    ze_api_version_t version, ///< [in] API version requested
    zes_performance_factor_dditable_t
        *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetPerformanceFactorProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetConfig = nullptr;

    pDdiTable->pfnSetConfig = nullptr;

exit:
    trace_zesGetPerformanceFactorProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Power table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetPowerProcAddrTable(
    ze_api_version_t version,       ///< [in] API version requested
    zes_power_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetPowerProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetEnergyCounter = nullptr;

    pDdiTable->pfnGetLimits = nullptr;

    pDdiTable->pfnSetLimits = nullptr;

    pDdiTable->pfnGetEnergyThreshold = nullptr;

    pDdiTable->pfnSetEnergyThreshold = nullptr;

exit:
    trace_zesGetPowerProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Psu table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetPsuProcAddrTable(
    ze_api_version_t version,     ///< [in] API version requested
    zes_psu_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetPsuProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetState = nullptr;

exit:
    trace_zesGetPsuProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Ras table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetRasProcAddrTable(
    ze_api_version_t version,     ///< [in] API version requested
    zes_ras_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetRasProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetConfig = nullptr;

    pDdiTable->pfnSetConfig = nullptr;

    pDdiTable->pfnGetState = nullptr;

exit:
    trace_zesGetRasProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Scheduler table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetSchedulerProcAddrTable(
    ze_api_version_t version,           ///< [in] API version requested
    zes_scheduler_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetSchedulerProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetCurrentMode = nullptr;

    pDdiTable->pfnGetTimeoutModeProperties = nullptr;

    pDdiTable->pfnGetTimesliceModeProperties = nullptr;

    pDdiTable->pfnSetTimeoutMode = nullptr;

    pDdiTable->pfnSetTimesliceMode = nullptr;

    pDdiTable->pfnSetExclusiveMode = nullptr;

    pDdiTable->pfnSetComputeUnitDebugMode = nullptr;

exit:
    trace_zesGetSchedulerProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Standby table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetStandbyProcAddrTable(
    ze_api_version_t version,         ///< [in] API version requested
    zes_standby_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetStandbyProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetMode = nullptr;

    pDdiTable->pfnSetMode = nullptr;

exit:
    trace_zesGetStandbyProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Temperature table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetTemperatureProcAddrTable(
    ze_api_version_t version,             ///< [in] API version requested
    zes_temperature_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zesGetTemperatureProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetConfig = nullptr;

    pDdiTable->pfnSetConfig = nullptr;

    pDdiTable->pfnGetState = nullptr;

exit:
    trace_zesGetTemperatureProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zesGetDeviceExpProcAddrTable(ze_api_version_t version, zes_device_exp_dditable_t *pDdiTable) {
    trace_zesGetDeviceExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnEnumEnabledVFExp = nullptr;
    pDdiTable->pfnGetSubDevicePropertiesExp = nullptr;
    pDdiTable->pfnEnumActiveVFExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zesGetDeviceExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zesGetDriverExpProcAddrTable(ze_api_version_t version, zes_driver_exp_dditable_t *pDdiTable) {
    trace_zesGetDriverExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGetDeviceByUuidExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zesGetDriverExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zesGetFirmwareExpProcAddrTable(ze_api_version_t version, zes_firmware_exp_dditable_t *pDdiTable) {
    trace_zesGetFirmwareExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGetSecurityVersionExp = nullptr;
    pDdiTable->pfnSetSecurityVersionExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zesGetFirmwareExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zesGetRasExpProcAddrTable(ze_api_version_t version,
                                                              zes_ras_exp_dditable_t *pDdiTable) {
    trace_zesGetRasExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGetStateExp = nullptr;
    pDdiTable->pfnClearStateExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zesGetRasExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zesGetVFManagementExpProcAddrTable(ze_api_version_t version,
                                   zes_vf_management_exp_dditable_t *pDdiTable) {
    trace_zesGetVFManagementExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGetVFCapabilitiesExp = nullptr;
    pDdiTable->pfnGetVFMemoryUtilizationExp2 = nullptr;
    pDdiTable->pfnGetVFEngineUtilizationExp2 = nullptr;
    pDdiTable->pfnGetVFPropertiesExp = nullptr;
    pDdiTable->pfnGetVFMemoryUtilizationExp = nullptr;
    pDdiTable->pfnGetVFEngineUtilizationExp = nullptr;
    pDdiTable->pfnSetVFTelemetryModeExp = nullptr;
    pDdiTable->pfnSetVFTelemetrySamplingIntervalExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zesGetVFManagementExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zesGetOverclockProcAddrTable(ze_api_version_t version, zes_overclock_dditable_t *pDdiTable) {
    trace_zesGetOverclockProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGetDomainProperties = nullptr;
    pDdiTable->pfnGetDomainVFProperties = nullptr;
    pDdiTable->pfnGetDomainControlProperties = nullptr;
    pDdiTable->pfnGetControlCurrentValue = nullptr;
    pDdiTable->pfnGetControlPendingValue = nullptr;
    pDdiTable->pfnSetControlUserValue = nullptr;
    pDdiTable->pfnGetControlState = nullptr;
    pDdiTable->pfnGetVFPointValues = nullptr;
    pDdiTable->pfnSetVFPointValues = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zesGetOverclockProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
