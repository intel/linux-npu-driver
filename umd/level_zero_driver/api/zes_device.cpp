/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/include/l0_exception.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/zes_api.h>
#include <level_zero/zes_ddi.h>

namespace L0 {

ze_result_t
zesDeviceGet(zes_driver_handle_t hDriver, uint32_t *pCount, zes_device_handle_t *phDevices) {
    if (hDriver == nullptr || pCount == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::DriverHandle::fromHandle(static_cast<ze_driver_handle_t>(hDriver))
            ->getDevice(pCount, static_cast<ze_device_handle_t *>(phDevices)));
}

ze_result_t zesDeviceGetProperties(zes_device_handle_t hDevice,
                                   zes_device_properties_t *pProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Device::fromHandle(static_cast<ze_device_handle_t>(hDevice))
                                       ->getProperties(pProperties));
}

ze_result_t zesDeviceEnumEngineGroups(zes_device_handle_t hDevice,
                                      uint32_t *pCount,
                                      zes_engine_handle_t *phEngine) {
    if (hDevice == nullptr || pCount == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    if (*pCount == 0) {
        *pCount = 1;
        return ZE_RESULT_SUCCESS;
    } else if (phEngine == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    *pCount = 1;
    *phEngine = reinterpret_cast<zes_engine_handle_t>(hDevice);
    return ZE_RESULT_SUCCESS;
}

} // namespace L0

extern "C" {
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

    pDdiTable->pfnEnumEngineGroups = L0::zesDeviceEnumEngineGroups;

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
}
