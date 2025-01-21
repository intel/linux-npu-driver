/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Generated header based on ze_graph_profiling_ext.h

#pragma once

#include "trace_ze_api.hpp"

#include <iostream>
#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_profiling_ext.h>
#include <sstream>

inline std::string _trace_zeDeviceGetProfilingDataProperties(
    ze_device_handle_t hDevice,
    ze_device_profiling_data_properties_t *pDeviceProfilingDataProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeDeviceGetProfilingDataProperties(";
    ss << "hDevice: " << hDevice;
    if (pDeviceProfilingDataProperties == nullptr) {
        ss << ", pDeviceProfilingDataProperties: nullptr";
    } else {
        ss << ", pDeviceProfilingDataProperties {";
        ss << "stype: " << pDeviceProfilingDataProperties->stype;
        ss << ", pNext: " << pDeviceProfilingDataProperties->pNext;
        ss << ", extensionVersion: " << pDeviceProfilingDataProperties->extensionVersion;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetProfilingDataProperties(
    ze_device_handle_t hDevice,
    ze_device_profiling_data_properties_t *pDeviceProfilingDataProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetProfilingDataProperties");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeDeviceGetProfilingDataProperties(hDevice,
                                                               pDeviceProfilingDataProperties) +
                         "..\n";
}
inline void trace_zeDeviceGetProfilingDataProperties(
    ze_result_t ret,
    ze_device_handle_t hDevice,
    ze_device_profiling_data_properties_t *pDeviceProfilingDataProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeDeviceGetProfilingDataProperties(hDevice,
                                                               pDeviceProfilingDataProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphProfilingPoolCreate(ze_graph_handle_t hGraph,
                                  uint32_t count,
                                  ze_graph_profiling_pool_handle_t *phProfilingPool) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphProfilingPoolCreate(";
    ss << "hGraph: " << hGraph;
    ss << ", count: " << count;
    if (phProfilingPool == nullptr) {
        ss << ", phProfilingPool: nullptr";
    } else {
        ss << ", phProfilingPool: " << *phProfilingPool;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphProfilingPoolCreate(ze_graph_handle_t hGraph,
                                             uint32_t count,
                                             ze_graph_profiling_pool_handle_t *phProfilingPool) {
    TRACE_EVENT_BEGIN("API", "zeGraphProfilingPoolCreate");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphProfilingPoolCreate(hGraph, count, phProfilingPool) + "..\n";
}
inline void trace_zeGraphProfilingPoolCreate(ze_result_t ret,
                                             ze_graph_handle_t hGraph,
                                             uint32_t count,
                                             ze_graph_profiling_pool_handle_t *phProfilingPool) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphProfilingPoolCreate(hGraph, count, phProfilingPool) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphProfilingPoolDestroy(ze_graph_profiling_pool_handle_t hProfilingPool) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphProfilingPoolDestroy(";
    ss << "hProfilingPool: " << hProfilingPool;
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphProfilingPoolDestroy(ze_graph_profiling_pool_handle_t hProfilingPool) {
    TRACE_EVENT_BEGIN("API", "zeGraphProfilingPoolDestroy");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphProfilingPoolDestroy(hProfilingPool) + "..\n";
}
inline void trace_zeGraphProfilingPoolDestroy(ze_result_t ret,
                                              ze_graph_profiling_pool_handle_t hProfilingPool) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphProfilingPoolDestroy(hProfilingPool) + trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphProfilingQueryCreate(ze_graph_profiling_pool_handle_t hProfilingPool,
                                   uint32_t index,
                                   ze_graph_profiling_query_handle_t *phProfilingQuery) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphProfilingQueryCreate(";
    ss << "hProfilingPool: " << hProfilingPool;
    ss << ", index: " << index;
    if (phProfilingQuery == nullptr) {
        ss << ", phProfilingQuery: nullptr";
    } else {
        ss << ", phProfilingQuery: " << *phProfilingQuery;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphProfilingQueryCreate(ze_graph_profiling_pool_handle_t hProfilingPool,
                                              uint32_t index,
                                              ze_graph_profiling_query_handle_t *phProfilingQuery) {
    TRACE_EVENT_BEGIN("API", "zeGraphProfilingQueryCreate");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphProfilingQueryCreate(hProfilingPool, index, phProfilingQuery) +
                         "..\n";
}
inline void trace_zeGraphProfilingQueryCreate(ze_result_t ret,
                                              ze_graph_profiling_pool_handle_t hProfilingPool,
                                              uint32_t index,
                                              ze_graph_profiling_query_handle_t *phProfilingQuery) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphProfilingQueryCreate(hProfilingPool, index, phProfilingQuery) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphProfilingQueryDestroy(ze_graph_profiling_query_handle_t hProfilingQuery) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphProfilingQueryDestroy(";
    ss << "hProfilingQuery: " << hProfilingQuery;
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphProfilingQueryDestroy(ze_graph_profiling_query_handle_t hProfilingQuery) {
    TRACE_EVENT_BEGIN("API", "zeGraphProfilingQueryDestroy");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphProfilingQueryDestroy(hProfilingQuery) + "..\n";
}
inline void trace_zeGraphProfilingQueryDestroy(ze_result_t ret,
                                               ze_graph_profiling_query_handle_t hProfilingQuery) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphProfilingQueryDestroy(hProfilingQuery) + trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphProfilingQueryGetData(ze_graph_profiling_query_handle_t hProfilingQuery,
                                    ze_graph_profiling_type_t profilingType,
                                    uint32_t *pSize,
                                    uint8_t *pData) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphProfilingQueryGetData(";
    ss << "hProfilingQuery: " << hProfilingQuery;
    ss << ", profilingType: " << profilingType;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    if (pData == nullptr) {
        ss << ", pData: nullptr";
    } else {
        ss << ", pData: " << *pData;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphProfilingQueryGetData(ze_graph_profiling_query_handle_t hProfilingQuery,
                                               ze_graph_profiling_type_t profilingType,
                                               uint32_t *pSize,
                                               uint8_t *pData) {
    TRACE_EVENT_BEGIN("API", "zeGraphProfilingQueryGetData");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphProfilingQueryGetData(hProfilingQuery,
                                                         profilingType,
                                                         pSize,
                                                         pData) +
                         "..\n";
}
inline void trace_zeGraphProfilingQueryGetData(ze_result_t ret,
                                               ze_graph_profiling_query_handle_t hProfilingQuery,
                                               ze_graph_profiling_type_t profilingType,
                                               uint32_t *pSize,
                                               uint8_t *pData) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphProfilingQueryGetData(hProfilingQuery,
                                                         profilingType,
                                                         pSize,
                                                         pData) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphProfilingLogGetString(ze_graph_profiling_query_handle_t phProfilingQuery,
                                    uint32_t *pSize,
                                    char *pProfilingLog) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphProfilingLogGetString(";
    ss << "phProfilingQuery: " << phProfilingQuery;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    if (pProfilingLog == nullptr) {
        ss << ", pProfilingLog: nullptr";
    } else {
        ss << ", pProfilingLog: " << pProfilingLog;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphProfilingLogGetString(ze_graph_profiling_query_handle_t phProfilingQuery,
                                               uint32_t *pSize,
                                               char *pProfilingLog) {
    TRACE_EVENT_BEGIN("API", "zeGraphProfilingLogGetString");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphProfilingLogGetString(phProfilingQuery, pSize, pProfilingLog) +
                         "..\n";
}
inline void trace_zeGraphProfilingLogGetString(ze_result_t ret,
                                               ze_graph_profiling_query_handle_t phProfilingQuery,
                                               uint32_t *pSize,
                                               char *pProfilingLog) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphProfilingLogGetString(phProfilingQuery, pSize, pProfilingLog) +
                         trace_ze_result_t(ret);
}
