/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/driver/driver.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

namespace L0 {

ze_result_t
zesDeviceGet(zes_driver_handle_t hDriver, uint32_t *pCount, zes_device_handle_t *phDevices) {
    if (static_cast<ze_driver_handle_t>(hDriver) == nullptr || pCount == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::DriverHandle::fromHandle(static_cast<ze_driver_handle_t>(hDriver))
            ->getDevice(pCount, static_cast<ze_device_handle_t *>(phDevices)));
}

ze_result_t zesDeviceGetProperties(zes_device_handle_t hDevice,
                                   zes_device_properties_t *pProperties) {
    if (static_cast<ze_device_handle_t>(hDevice) == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Device::fromHandle(static_cast<ze_device_handle_t>(hDevice))
                                       ->getProperties(pProperties));
}

} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zesDeviceGet(zes_driver_handle_t hDriver,
                                                 uint32_t *pCount,
                                                 zes_device_handle_t *phDevices) {
    return L0::zesDeviceGet(hDriver, pCount, phDevices);
}
ZE_APIEXPORT ze_result_t ZE_APICALL zesDeviceGetProperties(zes_device_handle_t hDevice,
                                                           zes_device_properties_t *pProperties) {
    return L0::zesDeviceGetProperties(hDevice, pProperties);
}

} // extern "C"