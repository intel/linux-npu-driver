/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/driver/driver.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include <level_zero/ze_api.h>

namespace L0 {
ze_result_t zeInit(ze_init_flags_t flags) {
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::init(flags));
}

ze_result_t zeDriverGet(uint32_t *pCount, ze_driver_handle_t *phDrivers) {
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::driverHandleGet(pCount, phDrivers));
}

ze_result_t zeDriverGetProperties(ze_driver_handle_t hDriver, ze_driver_properties_t *pProperties) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::DriverHandle::fromHandle(hDriver)->getProperties(pProperties));
}

ze_result_t zeDriverGetApiVersion(ze_driver_handle_t hDriver, ze_api_version_t *version) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::DriverHandle::fromHandle(hDriver)->getApiVersion(version));
}

ze_result_t zeDriverGetIpcProperties(ze_driver_handle_t hDriver,
                                     ze_driver_ipc_properties_t *pIPCProperties) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::DriverHandle::fromHandle(hDriver)->getIPCProperties(pIPCProperties));
}

ze_result_t zeDriverGetExtensionProperties(ze_driver_handle_t hDriver,
                                           uint32_t *pCount,
                                           ze_driver_extension_properties_t *pExtensionProperties) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::DriverHandle::fromHandle(hDriver)->getExtensionProperties(pCount,
                                                                      pExtensionProperties));
}

ze_result_t zeDriverGetExtensionFunctionAddress(ze_driver_handle_t hDriver,
                                                const char *name,
                                                void **ppFunctionAddress) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::DriverHandle::fromHandle(hDriver)->getExtensionFunctionAddress(name,
                                                                           ppFunctionAddress));
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeInit(ze_init_flags_t flags) {
    return L0::zeInit(flags);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeDriverGet(uint32_t *pCount, ze_driver_handle_t *phDrivers) {
    return L0::zeDriverGet(pCount, phDrivers);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeDriverGetProperties(ze_driver_handle_t hDriver,
                                                          ze_driver_properties_t *pProperties) {
    return L0::zeDriverGetProperties(hDriver, pProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeDriverGetApiVersion(ze_driver_handle_t hDriver,
                                                          ze_api_version_t *version) {
    return L0::zeDriverGetApiVersion(hDriver, version);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeDriverGetIpcProperties(ze_driver_handle_t hDriver, ze_driver_ipc_properties_t *pIPCProperties) {
    return L0::zeDriverGetIpcProperties(hDriver, pIPCProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeDriverGetExtensionProperties(ze_driver_handle_t hDriver,
                               uint32_t *pCount,
                               ze_driver_extension_properties_t *pExtensionProperties) {
    return L0::zeDriverGetExtensionProperties(hDriver, pCount, pExtensionProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeDriverGetExtensionFunctionAddress(ze_driver_handle_t hDriver,
                                                                        const char *name,
                                                                        void **ppFunctionAddress) {
    return L0::zeDriverGetExtensionFunctionAddress(hDriver, name, ppFunctionAddress);
}
} // extern "C"
