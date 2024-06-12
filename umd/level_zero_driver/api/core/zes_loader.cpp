/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "ze_ddi_tables.hpp"
#include "zes_device.hpp"
#include "zes_driver.hpp"

#if defined(__cplusplus)
extern "C" {
#endif
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
/// @brief Exported function for filling application's Device table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zesGetDeviceProcAddrTable(
    ze_api_version_t version,        ///< [in] API version requested
    zes_device_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = L0::zesDeviceGetProperties;

    pDdiTable->pfnGetState = nullptr;

    pDdiTable->pfnReset = nullptr;

    pDdiTable->pfnProcessesGetState = nullptr;

    pDdiTable->pfnPciGetProperties = nullptr;

    pDdiTable->pfnPciGetState = nullptr;

    pDdiTable->pfnPciGetBars = nullptr;

    pDdiTable->pfnPciGetStats = nullptr;

    pDdiTable->pfnEnumDiagnosticTestSuites = nullptr;

    pDdiTable->pfnEnumEngineGroups = nullptr;

    pDdiTable->pfnEventRegister = nullptr;

    pDdiTable->pfnEnumFabricPorts = nullptr;

    pDdiTable->pfnEnumFans = nullptr;

    pDdiTable->pfnEnumFirmwares = nullptr;

    pDdiTable->pfnEnumFrequencyDomains = nullptr;

    pDdiTable->pfnEnumLeds = nullptr;

    pDdiTable->pfnEnumMemoryModules = nullptr;

    pDdiTable->pfnEnumPerformanceFactorDomains = nullptr;

    pDdiTable->pfnEnumPowerDomains = nullptr;

    pDdiTable->pfnEnumPsus = nullptr;

    pDdiTable->pfnEnumRasErrorSets = nullptr;

    pDdiTable->pfnEnumSchedulers = nullptr;

    pDdiTable->pfnEnumStandbyDomains = nullptr;

    pDdiTable->pfnEnumTemperatureSensors = nullptr;

    pDdiTable->pfnGet = L0::zesDeviceGet;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetTests = nullptr;

    pDdiTable->pfnRunTests = nullptr;

    return result;
}

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

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetActivity = nullptr;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetLinkType = nullptr;

    pDdiTable->pfnGetConfig = nullptr;

    pDdiTable->pfnSetConfig = nullptr;

    pDdiTable->pfnGetState = nullptr;

    pDdiTable->pfnGetThroughput = nullptr;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetConfig = nullptr;

    pDdiTable->pfnSetDefaultMode = nullptr;

    pDdiTable->pfnSetFixedSpeedMode = nullptr;

    pDdiTable->pfnSetSpeedTableMode = nullptr;

    pDdiTable->pfnGetState = nullptr;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnFlash = nullptr;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

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

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetState = nullptr;

    pDdiTable->pfnSetState = nullptr;

    pDdiTable->pfnSetColor = nullptr;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetState = nullptr;

    pDdiTable->pfnGetBandwidth = nullptr;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetConfig = nullptr;

    pDdiTable->pfnSetConfig = nullptr;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetEnergyCounter = nullptr;

    pDdiTable->pfnGetLimits = nullptr;

    pDdiTable->pfnSetLimits = nullptr;

    pDdiTable->pfnGetEnergyThreshold = nullptr;

    pDdiTable->pfnSetEnergyThreshold = nullptr;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetState = nullptr;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetConfig = nullptr;

    pDdiTable->pfnSetConfig = nullptr;

    pDdiTable->pfnGetState = nullptr;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetCurrentMode = nullptr;

    pDdiTable->pfnGetTimeoutModeProperties = nullptr;

    pDdiTable->pfnGetTimesliceModeProperties = nullptr;

    pDdiTable->pfnSetTimeoutMode = nullptr;

    pDdiTable->pfnSetTimesliceMode = nullptr;

    pDdiTable->pfnSetExclusiveMode = nullptr;

    pDdiTable->pfnSetComputeUnitDebugMode = nullptr;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetMode = nullptr;

    pDdiTable->pfnSetMode = nullptr;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProperties = nullptr;

    pDdiTable->pfnGetConfig = nullptr;

    pDdiTable->pfnSetConfig = nullptr;

    pDdiTable->pfnGetState = nullptr;

    return result;
}
#if defined(__cplusplus)
}
#endif
