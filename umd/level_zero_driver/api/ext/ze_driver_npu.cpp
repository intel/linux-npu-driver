/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/api/ext/ze_driver_npu.hpp"

#include "level_zero_driver/api/trace/trace_ze_driver_npu_ext.hpp"
#include "level_zero_driver/api/zet_misc.hpp"
#include "level_zero_driver/source/driver_handle.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <exception>
#include <loader/ze_loader.h>
#include <string.h>
#include <vector>
#include <ze_api.h>

namespace L0 {
/* Defined in ze_driver.cpp */
ze_result_t zeDriverGetExtensionFunctionAddress(ze_driver_handle_t hDriver,
                                                const char *name,
                                                void **ppFunctionAddress);

ze_result_t ZE_APICALL zeDriverGetExtensionExt(ze_driver_handle_t hDriver,
                                               ze_driver_extension_npu_ext_t *pExtension) {
    trace_zeDriverGetExtensionExt(hDriver, pExtension);
    ze_result_t ret;

    if (hDriver == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    if (pExtension == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    ret = translateHandle(ZEL_HANDLE_DRIVER, hDriver);
    if (ret != ZE_RESULT_SUCCESS) {
        goto exit;
    }

    if (pExtension->name == nullptr || pExtension->ppFunctionAddress == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }
    try {
        auto extensions = DriverHandle::fromHandle(hDriver)->getSupportedExtensions();

        for (const auto &ext : extensions) {
            if (strcmp(pExtension->name, ext.name) == 0 && pExtension->version <= ext.version) {
                ret = L0::zeDriverGetExtensionFunctionAddress(hDriver,
                                                              ext.name,
                                                              pExtension->ppFunctionAddress);
                goto exit;
            }
        }
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
    } catch (const std::exception &e) {
        LOG_E("Exception caught, msg: '%s'", e.what());
        ret = ZE_RESULT_ERROR_UNKNOWN;
    }

exit:
    trace_zeDriverGetExtensionExt(ret, hDriver, pExtension);
    return ret;
}

} // namespace L0