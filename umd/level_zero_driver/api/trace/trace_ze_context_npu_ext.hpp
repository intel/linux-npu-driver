/*
 * Copyright (C) 2025-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "trace_ze_api.hpp"

#include <iostream>
#include <sstream>
#include <ze_api.h>
#include <ze_context_npu_ext.h>

inline std::string
_trace_zeContextSetProperties(ze_context_handle_t hContext,
                              ze_context_properties_npu_ext_t *pContextProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeContextSetProperties(";
    ss << "hContext: " << hContext;
    if (pContextProperties == nullptr) {
        ss << ", pContextProperties: nullptr";
    } else {
        ss << ", pContextProperties {";
        ss << "stype: " << pContextProperties->stype;
        ss << ", pNext: " << pContextProperties->pNext;
        ss << ", options: " << pContextProperties->options;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeContextSetProperties(ze_context_handle_t hContext,
                                         ze_context_properties_npu_ext_t *pContextProperties) {
    TRACE_EVENT_BEGIN("API", "zeContextSetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextSetProperties(hContext, pContextProperties) + "..\n";
}
inline void trace_zeContextSetProperties(ze_result_t ret,
                                         ze_context_handle_t hContext,
                                         ze_context_properties_npu_ext_t *pContextProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextSetProperties(hContext, pContextProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeContextReleaseMemory(ze_context_handle_t hContext) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeContextReleaseMemory(";
    ss << "hContext: " << hContext;
    ss << ")";
    return ss.str();
}
inline void trace_zeContextReleaseMemory(ze_context_handle_t hContext) {
    TRACE_EVENT_BEGIN("API", "zeContextReleaseMemory");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextReleaseMemory(hContext) + "..\n";
}
inline void trace_zeContextReleaseMemory(ze_result_t ret, ze_context_handle_t hContext) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextReleaseMemory(hContext) + trace_ze_result_t(ret);
}
