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
ZE_APIEXPORT ze_result_t ZE_APICALL zesInit(zes_init_flags_t flags) {
    return L0::zesInit(flags);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zesDriverGet(uint32_t *pCount, zes_driver_handle_t *phDrivers) {
    return L0::zesDriverGet(pCount, phDrivers);
}

} // extern "C"