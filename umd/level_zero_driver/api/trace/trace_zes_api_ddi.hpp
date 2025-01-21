/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Generated header based on zes_ddi.h@version v1.11-r1.11.4

#pragma once

#include "trace_ze_api.hpp"

#include <iostream>
#include <level_zero/zes_ddi.h>
#include <sstream>

inline std::string _trace_zesGetGlobalProcAddrTable(ze_api_version_t version,
                                                    zes_global_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetGlobalProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnInit: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnInit);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetGlobalProcAddrTable(ze_api_version_t version,
                                            zes_global_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetGlobalProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetGlobalProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetGlobalProcAddrTable(ze_result_t ret,
                                            ze_api_version_t version,
                                            zes_global_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetGlobalProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesGetDeviceProcAddrTable(ze_api_version_t version,
                                                    zes_device_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetDeviceProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetState);
        ss << ", pfnReset: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnReset);
        ss << ", pfnProcessesGetState: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnProcessesGetState);
        ss << ", pfnPciGetProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnPciGetProperties);
        ss << ", pfnPciGetState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnPciGetState);
        ss << ", pfnPciGetBars: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnPciGetBars);
        ss << ", pfnPciGetStats: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnPciGetStats);
        ss << ", pfnEnumDiagnosticTestSuites: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumDiagnosticTestSuites);
        ss << ", pfnEnumEngineGroups: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumEngineGroups);
        ss << ", pfnEventRegister: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnEventRegister);
        ss << ", pfnEnumFabricPorts: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumFabricPorts);
        ss << ", pfnEnumFans: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumFans);
        ss << ", pfnEnumFirmwares: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumFirmwares);
        ss << ", pfnEnumFrequencyDomains: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumFrequencyDomains);
        ss << ", pfnEnumLeds: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumLeds);
        ss << ", pfnEnumMemoryModules: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumMemoryModules);
        ss << ", pfnEnumPerformanceFactorDomains: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumPerformanceFactorDomains);
        ss << ", pfnEnumPowerDomains: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumPowerDomains);
        ss << ", pfnGetCardPowerDomain: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetCardPowerDomain);
        ss << ", pfnEnumPsus: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumPsus);
        ss << ", pfnEnumRasErrorSets: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumRasErrorSets);
        ss << ", pfnEnumSchedulers: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumSchedulers);
        ss << ", pfnEnumStandbyDomains: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumStandbyDomains);
        ss << ", pfnEnumTemperatureSensors: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumTemperatureSensors);
        ss << ", pfnEccAvailable: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnEccAvailable);
        ss << ", pfnEccConfigurable: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEccConfigurable);
        ss << ", pfnGetEccState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetEccState);
        ss << ", pfnSetEccState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetEccState);
        ss << ", pfnGet: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGet);
        ss << ", pfnSetOverclockWaiver: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetOverclockWaiver);
        ss << ", pfnGetOverclockDomains: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetOverclockDomains);
        ss << ", pfnGetOverclockControls: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetOverclockControls);
        ss << ", pfnResetOverclockSettings: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnResetOverclockSettings);
        ss << ", pfnReadOverclockState: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnReadOverclockState);
        ss << ", pfnEnumOverclockDomains: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumOverclockDomains);
        ss << ", pfnResetExt: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnResetExt);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetDeviceProcAddrTable(ze_api_version_t version,
                                            zes_device_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetDeviceProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetDeviceProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetDeviceProcAddrTable(ze_result_t ret,
                                            ze_api_version_t version,
                                            zes_device_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetDeviceProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesGetDeviceExpProcAddrTable(ze_api_version_t version,
                                                       zes_device_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetDeviceExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetSubDevicePropertiesExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetSubDevicePropertiesExp);
        ss << ", pfnEnumActiveVFExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumActiveVFExp);
        ss << ", pfnEnumEnabledVFExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnumEnabledVFExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetDeviceExpProcAddrTable(ze_api_version_t version,
                                               zes_device_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetDeviceExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetDeviceExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetDeviceExpProcAddrTable(ze_result_t ret,
                                               ze_api_version_t version,
                                               zes_device_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetDeviceExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesGetDriverProcAddrTable(ze_api_version_t version,
                                                    zes_driver_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetDriverProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnEventListen: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnEventListen);
        ss << ", pfnEventListenEx: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnEventListenEx);
        ss << ", pfnGet: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGet);
        ss << ", pfnGetExtensionProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetExtensionProperties);
        ss << ", pfnGetExtensionFunctionAddress: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetExtensionFunctionAddress);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetDriverProcAddrTable(ze_api_version_t version,
                                            zes_driver_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetDriverProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetDriverProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetDriverProcAddrTable(ze_result_t ret,
                                            ze_api_version_t version,
                                            zes_driver_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetDriverProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesGetDriverExpProcAddrTable(ze_api_version_t version,
                                                       zes_driver_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetDriverExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetDeviceByUuidExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetDeviceByUuidExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetDriverExpProcAddrTable(ze_api_version_t version,
                                               zes_driver_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetDriverExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetDriverExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetDriverExpProcAddrTable(ze_result_t ret,
                                               ze_api_version_t version,
                                               zes_driver_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetDriverExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesGetOverclockProcAddrTable(ze_api_version_t version,
                                                       zes_overclock_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetOverclockProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetDomainProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetDomainProperties);
        ss << ", pfnGetDomainVFProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetDomainVFProperties);
        ss << ", pfnGetDomainControlProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetDomainControlProperties);
        ss << ", pfnGetControlCurrentValue: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetControlCurrentValue);
        ss << ", pfnGetControlPendingValue: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetControlPendingValue);
        ss << ", pfnSetControlUserValue: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetControlUserValue);
        ss << ", pfnGetControlState: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetControlState);
        ss << ", pfnGetVFPointValues: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetVFPointValues);
        ss << ", pfnSetVFPointValues: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetVFPointValues);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetOverclockProcAddrTable(ze_api_version_t version,
                                               zes_overclock_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetOverclockProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetOverclockProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetOverclockProcAddrTable(ze_result_t ret,
                                               ze_api_version_t version,
                                               zes_overclock_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetOverclockProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesGetSchedulerProcAddrTable(ze_api_version_t version,
                                                       zes_scheduler_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetSchedulerProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetCurrentMode: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetCurrentMode);
        ss << ", pfnGetTimeoutModeProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetTimeoutModeProperties);
        ss << ", pfnGetTimesliceModeProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetTimesliceModeProperties);
        ss << ", pfnSetTimeoutMode: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetTimeoutMode);
        ss << ", pfnSetTimesliceMode: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetTimesliceMode);
        ss << ", pfnSetExclusiveMode: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetExclusiveMode);
        ss << ", pfnSetComputeUnitDebugMode: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetComputeUnitDebugMode);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetSchedulerProcAddrTable(ze_api_version_t version,
                                               zes_scheduler_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetSchedulerProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetSchedulerProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetSchedulerProcAddrTable(ze_result_t ret,
                                               ze_api_version_t version,
                                               zes_scheduler_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetSchedulerProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesGetPerformanceFactorProcAddrTable(ze_api_version_t version,
                                            zes_performance_factor_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetPerformanceFactorProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetConfig: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetConfig);
        ss << ", pfnSetConfig: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetConfig);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zesGetPerformanceFactorProcAddrTable(ze_api_version_t version,
                                           zes_performance_factor_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetPerformanceFactorProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetPerformanceFactorProcAddrTable(version, pDdiTable) + "..\n";
}
inline void
trace_zesGetPerformanceFactorProcAddrTable(ze_result_t ret,
                                           ze_api_version_t version,
                                           zes_performance_factor_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetPerformanceFactorProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesGetPowerProcAddrTable(ze_api_version_t version,
                                                   zes_power_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetPowerProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetEnergyCounter: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetEnergyCounter);
        ss << ", pfnGetLimits: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetLimits);
        ss << ", pfnSetLimits: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetLimits);
        ss << ", pfnGetEnergyThreshold: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetEnergyThreshold);
        ss << ", pfnSetEnergyThreshold: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetEnergyThreshold);
        ss << ", pfnGetLimitsExt: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetLimitsExt);
        ss << ", pfnSetLimitsExt: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetLimitsExt);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetPowerProcAddrTable(ze_api_version_t version,
                                           zes_power_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetPowerProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetPowerProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetPowerProcAddrTable(ze_result_t ret,
                                           ze_api_version_t version,
                                           zes_power_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetPowerProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesGetFrequencyProcAddrTable(ze_api_version_t version,
                                                       zes_frequency_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetFrequencyProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetAvailableClocks: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetAvailableClocks);
        ss << ", pfnGetRange: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetRange);
        ss << ", pfnSetRange: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetRange);
        ss << ", pfnGetState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetState);
        ss << ", pfnGetThrottleTime: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetThrottleTime);
        ss << ", pfnOcGetCapabilities: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnOcGetCapabilities);
        ss << ", pfnOcGetFrequencyTarget: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnOcGetFrequencyTarget);
        ss << ", pfnOcSetFrequencyTarget: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnOcSetFrequencyTarget);
        ss << ", pfnOcGetVoltageTarget: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnOcGetVoltageTarget);
        ss << ", pfnOcSetVoltageTarget: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnOcSetVoltageTarget);
        ss << ", pfnOcSetMode: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnOcSetMode);
        ss << ", pfnOcGetMode: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnOcGetMode);
        ss << ", pfnOcGetIccMax: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnOcGetIccMax);
        ss << ", pfnOcSetIccMax: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnOcSetIccMax);
        ss << ", pfnOcGetTjMax: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnOcGetTjMax);
        ss << ", pfnOcSetTjMax: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnOcSetTjMax);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetFrequencyProcAddrTable(ze_api_version_t version,
                                               zes_frequency_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetFrequencyProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetFrequencyProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetFrequencyProcAddrTable(ze_result_t ret,
                                               ze_api_version_t version,
                                               zes_frequency_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetFrequencyProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesGetEngineProcAddrTable(ze_api_version_t version,
                                                    zes_engine_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetEngineProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetActivity: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetActivity);
        ss << ", pfnGetActivityExt: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetActivityExt);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetEngineProcAddrTable(ze_api_version_t version,
                                            zes_engine_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetEngineProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetEngineProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetEngineProcAddrTable(ze_result_t ret,
                                            ze_api_version_t version,
                                            zes_engine_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetEngineProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesGetStandbyProcAddrTable(ze_api_version_t version,
                                                     zes_standby_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetStandbyProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetMode: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetMode);
        ss << ", pfnSetMode: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetMode);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetStandbyProcAddrTable(ze_api_version_t version,
                                             zes_standby_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetStandbyProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetStandbyProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetStandbyProcAddrTable(ze_result_t ret,
                                             ze_api_version_t version,
                                             zes_standby_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetStandbyProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesGetFirmwareProcAddrTable(ze_api_version_t version,
                                                      zes_firmware_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetFirmwareProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnFlash: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnFlash);
        ss << ", pfnGetFlashProgress: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetFlashProgress);
        ss << ", pfnGetConsoleLogs: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetConsoleLogs);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetFirmwareProcAddrTable(ze_api_version_t version,
                                              zes_firmware_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetFirmwareProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetFirmwareProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetFirmwareProcAddrTable(ze_result_t ret,
                                              ze_api_version_t version,
                                              zes_firmware_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetFirmwareProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesGetFirmwareExpProcAddrTable(ze_api_version_t version,
                                                         zes_firmware_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetFirmwareExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetSecurityVersionExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetSecurityVersionExp);
        ss << ", pfnSetSecurityVersionExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetSecurityVersionExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetFirmwareExpProcAddrTable(ze_api_version_t version,
                                                 zes_firmware_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetFirmwareExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetFirmwareExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetFirmwareExpProcAddrTable(ze_result_t ret,
                                                 ze_api_version_t version,
                                                 zes_firmware_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetFirmwareExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesGetMemoryProcAddrTable(ze_api_version_t version,
                                                    zes_memory_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetMemoryProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetState);
        ss << ", pfnGetBandwidth: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetBandwidth);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetMemoryProcAddrTable(ze_api_version_t version,
                                            zes_memory_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetMemoryProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetMemoryProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetMemoryProcAddrTable(ze_result_t ret,
                                            ze_api_version_t version,
                                            zes_memory_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetMemoryProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesGetFabricPortProcAddrTable(ze_api_version_t version,
                                                        zes_fabric_port_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetFabricPortProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetLinkType: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetLinkType);
        ss << ", pfnGetConfig: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetConfig);
        ss << ", pfnSetConfig: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetConfig);
        ss << ", pfnGetState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetState);
        ss << ", pfnGetThroughput: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetThroughput);
        ss << ", pfnGetFabricErrorCounters: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetFabricErrorCounters);
        ss << ", pfnGetMultiPortThroughput: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetMultiPortThroughput);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetFabricPortProcAddrTable(ze_api_version_t version,
                                                zes_fabric_port_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetFabricPortProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetFabricPortProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetFabricPortProcAddrTable(ze_result_t ret,
                                                ze_api_version_t version,
                                                zes_fabric_port_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetFabricPortProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesGetTemperatureProcAddrTable(ze_api_version_t version,
                                                         zes_temperature_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetTemperatureProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetConfig: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetConfig);
        ss << ", pfnSetConfig: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetConfig);
        ss << ", pfnGetState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetState);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetTemperatureProcAddrTable(ze_api_version_t version,
                                                 zes_temperature_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetTemperatureProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetTemperatureProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetTemperatureProcAddrTable(ze_result_t ret,
                                                 ze_api_version_t version,
                                                 zes_temperature_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetTemperatureProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesGetPsuProcAddrTable(ze_api_version_t version,
                                                 zes_psu_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetPsuProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetState);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetPsuProcAddrTable(ze_api_version_t version, zes_psu_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetPsuProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetPsuProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetPsuProcAddrTable(ze_result_t ret,
                                         ze_api_version_t version,
                                         zes_psu_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetPsuProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesGetFanProcAddrTable(ze_api_version_t version,
                                                 zes_fan_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetFanProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetConfig: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetConfig);
        ss << ", pfnSetDefaultMode: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetDefaultMode);
        ss << ", pfnSetFixedSpeedMode: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetFixedSpeedMode);
        ss << ", pfnSetSpeedTableMode: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetSpeedTableMode);
        ss << ", pfnGetState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetState);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetFanProcAddrTable(ze_api_version_t version, zes_fan_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetFanProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetFanProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetFanProcAddrTable(ze_result_t ret,
                                         ze_api_version_t version,
                                         zes_fan_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetFanProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesGetLedProcAddrTable(ze_api_version_t version,
                                                 zes_led_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetLedProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetState);
        ss << ", pfnSetState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetState);
        ss << ", pfnSetColor: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetColor);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetLedProcAddrTable(ze_api_version_t version, zes_led_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetLedProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetLedProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetLedProcAddrTable(ze_result_t ret,
                                         ze_api_version_t version,
                                         zes_led_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetLedProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesGetRasProcAddrTable(ze_api_version_t version,
                                                 zes_ras_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetRasProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetConfig: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetConfig);
        ss << ", pfnSetConfig: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetConfig);
        ss << ", pfnGetState: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetState);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetRasProcAddrTable(ze_api_version_t version, zes_ras_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetRasProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetRasProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetRasProcAddrTable(ze_result_t ret,
                                         ze_api_version_t version,
                                         zes_ras_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetRasProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesGetRasExpProcAddrTable(ze_api_version_t version,
                                                    zes_ras_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetRasExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetStateExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetStateExp);
        ss << ", pfnClearStateExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnClearStateExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetRasExpProcAddrTable(ze_api_version_t version,
                                            zes_ras_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetRasExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetRasExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetRasExpProcAddrTable(ze_result_t ret,
                                            ze_api_version_t version,
                                            zes_ras_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetRasExpProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesGetDiagnosticsProcAddrTable(ze_api_version_t version,
                                                         zes_diagnostics_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetDiagnosticsProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetTests: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetTests);
        ss << ", pfnRunTests: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnRunTests);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetDiagnosticsProcAddrTable(ze_api_version_t version,
                                                 zes_diagnostics_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetDiagnosticsProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetDiagnosticsProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetDiagnosticsProcAddrTable(ze_result_t ret,
                                                 ze_api_version_t version,
                                                 zes_diagnostics_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetDiagnosticsProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesGetVFManagementExpProcAddrTable(ze_api_version_t version,
                                          zes_vf_management_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zesGetVFManagementExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetVFPropertiesExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetVFPropertiesExp);
        ss << ", pfnGetVFMemoryUtilizationExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetVFMemoryUtilizationExp);
        ss << ", pfnGetVFEngineUtilizationExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetVFEngineUtilizationExp);
        ss << ", pfnSetVFTelemetryModeExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetVFTelemetryModeExp);
        ss << ", pfnSetVFTelemetrySamplingIntervalExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetVFTelemetrySamplingIntervalExp);
        ss << ", pfnGetVFCapabilitiesExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetVFCapabilitiesExp);
        ss << ", pfnGetVFMemoryUtilizationExp2: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetVFMemoryUtilizationExp2);
        ss << ", pfnGetVFEngineUtilizationExp2: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetVFEngineUtilizationExp2);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesGetVFManagementExpProcAddrTable(ze_api_version_t version,
                                                     zes_vf_management_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zesGetVFManagementExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetVFManagementExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zesGetVFManagementExpProcAddrTable(ze_result_t ret,
                                                     ze_api_version_t version,
                                                     zes_vf_management_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zesGetVFManagementExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}
