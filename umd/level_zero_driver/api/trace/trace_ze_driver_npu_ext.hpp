/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "trace_ze_api.hpp"

#include <iostream>
#include <sstream>
#include <ze_api.h>
#include <ze_driver_npu_ext.h>

inline std::string _trace_zeDriverGetExtensionExt(ze_driver_handle_t hDriver,
                                                  ze_driver_extension_npu_ext_t *pExtension) {
    std::stringstream ss;
    ss << "NPU_LOG: [API_EXT][tid:" << gettid() << "] zeDriverGetExtensionExt(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    ss << ", pExtension: " << pExtension;
    ss << ")";
    return ss.str();
}

inline void trace_zeDriverGetExtensionExt(ze_driver_handle_t hDriver,
                                          ze_driver_extension_npu_ext_t *pExtension) {
    TRACE_EVENT_BEGIN("API", "zeDriverGetExtensionExt");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeDriverGetExtensionExt(hDriver, pExtension) + "..\n";
}

inline void trace_zeDriverGetExtensionExt(ze_result_t ret,
                                          ze_driver_handle_t hDriver,
                                          ze_driver_extension_npu_ext_t *pExtension) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeDriverGetExtensionExt(hDriver, pExtension) + trace_ze_result_t(ret);
}
