/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Generated header based on ze_graph_ext.h

#pragma once

#include "trace_ze_api.hpp"

#include <iostream>
#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_ext.h>
#include <level_zero/ze_graph_profiling_ext.h>
#include <sstream>

inline std::string
_trace_zeDeviceGetGraphProperties(ze_device_handle_t hDevice,
                                  ze_device_graph_properties_t *pDeviceGraphProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeDeviceGetGraphProperties(";
    ss << "hDevice: " << hDevice;
    if (pDeviceGraphProperties == nullptr) {
        ss << ", *pDeviceGraphProperties: nullptr";
    } else {
        ss << ", *pDeviceGraphProperties {";
        ss << "stype: " << pDeviceGraphProperties->stype;
        ss << ", pNext: " << pDeviceGraphProperties->pNext;
        ss << ", graphExtensionVersion: " << pDeviceGraphProperties->graphExtensionVersion;
        ss << ", compilerVersion {";
        ss << "major: " << pDeviceGraphProperties->compilerVersion.major;
        ss << ", minor: " << pDeviceGraphProperties->compilerVersion.minor;
        ss << "}";
        ss << ", graphFormatsSupported: " << pDeviceGraphProperties->graphFormatsSupported;
        ss << ", maxOVOpsetVersionSupported: "
           << pDeviceGraphProperties->maxOVOpsetVersionSupported;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetGraphProperties(ze_device_handle_t hDevice,
                                             ze_device_graph_properties_t *pDeviceGraphProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetGraphProperties");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeDeviceGetGraphProperties(hDevice, pDeviceGraphProperties) + "..\n";
}
inline void trace_zeDeviceGetGraphProperties(ze_result_t ret,
                                             ze_device_handle_t hDevice,
                                             ze_device_graph_properties_t *pDeviceGraphProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeDeviceGetGraphProperties(hDevice, pDeviceGraphProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphCreate(ze_context_handle_t hContext,
                                        ze_device_handle_t hDevice,
                                        const ze_graph_desc_t *desc,
                                        ze_graph_handle_t *phGraph) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphCreate(";
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", format: " << desc->format;
        ss << ", inputSize: " << desc->inputSize;
        if (desc->pInput == nullptr) {
            ss << ", pInput: nullptr";
        } else {
            ss << ", pInput: " << static_cast<const void *>(desc->pInput);
        }
        if (desc->pBuildFlags == nullptr) {
            ss << ", pBuildFlags: nullptr";
        } else {
            ss << ", pBuildFlags: " << desc->pBuildFlags;
        }
        ss << "}";
    }
    if (phGraph == nullptr) {
        ss << ", phGraph: nullptr";
    } else {
        ss << ", phGraph: " << *phGraph;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphCreate(ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                const ze_graph_desc_t *desc,
                                ze_graph_handle_t *phGraph) {
    TRACE_EVENT_BEGIN("API", "zeGraphCreate");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphCreate(hContext, hDevice, desc, phGraph) + "..\n";
}
inline void trace_zeGraphCreate(ze_result_t ret,
                                ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                const ze_graph_desc_t *desc,
                                ze_graph_handle_t *phGraph) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphCreate(hContext, hDevice, desc, phGraph) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphDestroy(ze_graph_handle_t hGraph) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphDestroy(";
    ss << "hGraph: " << hGraph;
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphDestroy(ze_graph_handle_t hGraph) {
    TRACE_EVENT_BEGIN("API", "zeGraphDestroy");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphDestroy(hGraph) + "..\n";
}
inline void trace_zeGraphDestroy(ze_result_t ret, ze_graph_handle_t hGraph) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphDestroy(hGraph) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphGetNativeBinary(ze_graph_handle_t hGraph,
                                                 size_t *pSize,
                                                 uint8_t *pGraphNativeBinary) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphGetNativeBinary(";
    ss << "hGraph: " << hGraph;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    if (pGraphNativeBinary == nullptr) {
        ss << ", pGraphNativeBinary: nullptr";
    } else {
        ss << ", pGraphNativeBinary: " << *pGraphNativeBinary;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeGraphGetNativeBinary(ze_graph_handle_t hGraph, size_t *pSize, uint8_t *pGraphNativeBinary) {
    TRACE_EVENT_BEGIN("API", "zeGraphGetNativeBinary");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetNativeBinary(hGraph, pSize, pGraphNativeBinary) + "..\n";
}
inline void trace_zeGraphGetNativeBinary(ze_result_t ret,
                                         ze_graph_handle_t hGraph,
                                         size_t *pSize,
                                         uint8_t *pGraphNativeBinary) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetNativeBinary(hGraph, pSize, pGraphNativeBinary) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphGetProperties(ze_graph_handle_t hGraph,
                                               ze_graph_properties_t *pGraphProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphGetProperties(";
    ss << "hGraph: " << hGraph;
    if (pGraphProperties == nullptr) {
        ss << ", pGraphProperties: nullptr";
    } else {
        ss << ", pGraphProperties {";
        ss << "stype: " << pGraphProperties->stype;
        ss << ", pNext: " << pGraphProperties->pNext;
        ss << ", numGraphArgs: " << pGraphProperties->numGraphArgs;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphGetProperties(ze_graph_handle_t hGraph,
                                       ze_graph_properties_t *pGraphProperties) {
    TRACE_EVENT_BEGIN("API", "zeGraphGetProperties");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetProperties(hGraph, pGraphProperties) + "..\n";
}
inline void trace_zeGraphGetProperties(ze_result_t ret,
                                       ze_graph_handle_t hGraph,
                                       ze_graph_properties_t *pGraphProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetProperties(hGraph, pGraphProperties) + trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphGetArgumentProperties(ze_graph_handle_t hGraph,
                                    uint32_t argIndex,
                                    ze_graph_argument_properties_t *pGraphArgumentProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphGetArgumentProperties(";
    ss << "hGraph: " << hGraph;
    ss << ", argIndex: " << argIndex;
    if (pGraphArgumentProperties == nullptr) {
        ss << ", pGraphArgumentProperties: nullptr";
    } else {
        ss << ", pGraphArgumentProperties {";
        ss << "stype: " << pGraphArgumentProperties->stype;
        ss << ", pNext: " << pGraphArgumentProperties->pNext;
        ss << ", name: " << pGraphArgumentProperties->name;
        ss << ", type: " << pGraphArgumentProperties->type;
        ss << ", dims: " << pGraphArgumentProperties->dims[0] << " "
           << pGraphArgumentProperties->dims[1] << " " << pGraphArgumentProperties->dims[2] << " "
           << pGraphArgumentProperties->dims[3] << " " << pGraphArgumentProperties->dims[4];
        ss << ", networkPrecision: " << pGraphArgumentProperties->networkPrecision;
        ss << ", networkLayout: " << pGraphArgumentProperties->networkLayout;
        ss << ", devicePrecision: " << pGraphArgumentProperties->devicePrecision;
        ss << ", deviceLayout: " << pGraphArgumentProperties->deviceLayout;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeGraphGetArgumentProperties(ze_graph_handle_t hGraph,
                                   uint32_t argIndex,
                                   ze_graph_argument_properties_t *pGraphArgumentProperties) {
    TRACE_EVENT_BEGIN("API", "zeGraphGetArgumentProperties");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetArgumentProperties(hGraph,
                                                         argIndex,
                                                         pGraphArgumentProperties) +
                         "..\n";
}
inline void
trace_zeGraphGetArgumentProperties(ze_result_t ret,
                                   ze_graph_handle_t hGraph,
                                   uint32_t argIndex,
                                   ze_graph_argument_properties_t *pGraphArgumentProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetArgumentProperties(hGraph,
                                                         argIndex,
                                                         pGraphArgumentProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphSetArgumentValue(ze_graph_handle_t hGraph, uint32_t argIndex, const void *pArgValue) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphSetArgumentValue(";
    ss << "hGraph: " << hGraph;
    ss << ", argIndex: " << argIndex;
    ss << ", pArgValue: " << pArgValue;
    ss << ")";
    return ss.str();
}
inline void
trace_zeGraphSetArgumentValue(ze_graph_handle_t hGraph, uint32_t argIndex, const void *pArgValue) {
    TRACE_EVENT_BEGIN("API", "zeGraphSetArgumentValue");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphSetArgumentValue(hGraph, argIndex, pArgValue) + "..\n";
}
inline void trace_zeGraphSetArgumentValue(ze_result_t ret,
                                          ze_graph_handle_t hGraph,
                                          uint32_t argIndex,
                                          const void *pArgValue) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphSetArgumentValue(hGraph, argIndex, pArgValue) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeAppendGraphInitialize(ze_command_list_handle_t hCommandList,
                                                  ze_graph_handle_t hGraph,
                                                  ze_event_handle_t hSignalEvent,
                                                  uint32_t numWaitEvents,
                                                  ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeAppendGraphInitialize(";
    ss << "hCommandList: " << hCommandList;
    ss << ", hGraph: " << hGraph;
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeAppendGraphInitialize(ze_command_list_handle_t hCommandList,
                                          ze_graph_handle_t hGraph,
                                          ze_event_handle_t hSignalEvent,
                                          uint32_t numWaitEvents,
                                          ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeAppendGraphInitialize");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeAppendGraphInitialize(hCommandList,
                                                    hGraph,
                                                    hSignalEvent,
                                                    numWaitEvents,
                                                    phWaitEvents) +
                         "..\n";
}
inline void trace_zeAppendGraphInitialize(ze_result_t ret,
                                          ze_command_list_handle_t hCommandList,
                                          ze_graph_handle_t hGraph,
                                          ze_event_handle_t hSignalEvent,
                                          uint32_t numWaitEvents,
                                          ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeAppendGraphInitialize(hCommandList,
                                                    hGraph,
                                                    hSignalEvent,
                                                    numWaitEvents,
                                                    phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeAppendGraphExecute(ze_command_list_handle_t hCommandList,
                                               ze_graph_handle_t hGraph,
                                               ze_graph_profiling_query_handle_t hProfilingQuery,
                                               ze_event_handle_t hSignalEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeAppendGraphExecute(";
    ss << "hCommandList: " << hCommandList;
    ss << ", hGraph: " << hGraph;
    ss << ", hProfilingQuery: " << hProfilingQuery;
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeAppendGraphExecute(ze_command_list_handle_t hCommandList,
                                       ze_graph_handle_t hGraph,
                                       ze_graph_profiling_query_handle_t hProfilingQuery,
                                       ze_event_handle_t hSignalEvent,
                                       uint32_t numWaitEvents,
                                       ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeAppendGraphExecute");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeAppendGraphExecute(hCommandList,
                                                 hGraph,
                                                 hProfilingQuery,
                                                 hSignalEvent,
                                                 numWaitEvents,
                                                 phWaitEvents) +
                         "..\n";
}
inline void trace_zeAppendGraphExecute(ze_result_t ret,
                                       ze_command_list_handle_t hCommandList,
                                       ze_graph_handle_t hGraph,
                                       ze_graph_profiling_query_handle_t hProfilingQuery,
                                       ze_event_handle_t hSignalEvent,
                                       uint32_t numWaitEvents,
                                       ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeAppendGraphExecute(hCommandList,
                                                 hGraph,
                                                 hProfilingQuery,
                                                 hSignalEvent,
                                                 numWaitEvents,
                                                 phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphGetArgumentProperties2(ze_graph_handle_t hGraph,
                                     uint32_t argIndex,
                                     ze_graph_argument_properties_2_t *pGraphArgumentProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphGetArgumentProperties2(";
    ss << "hGraph: " << hGraph;
    ss << ", argIndex: " << argIndex;
    if (pGraphArgumentProperties == nullptr) {
        ss << ", pGraphArgumentProperties: nullptr";
    } else {
        ss << ", pGraphArgumentProperties {";
        ss << "stype: " << pGraphArgumentProperties->stype;
        ss << ", pNext: " << pGraphArgumentProperties->pNext;
        ss << ", name: " << pGraphArgumentProperties->name;
        ss << ", type: " << pGraphArgumentProperties->type;
        ss << ", dims: " << pGraphArgumentProperties->dims[0] << " "
           << pGraphArgumentProperties->dims[1] << " " << pGraphArgumentProperties->dims[2] << " "
           << pGraphArgumentProperties->dims[3] << " " << pGraphArgumentProperties->dims[4];
        ss << ", networkPrecision: " << pGraphArgumentProperties->networkPrecision;
        ss << ", networkLayout: " << pGraphArgumentProperties->networkLayout;
        ss << ", devicePrecision: " << pGraphArgumentProperties->devicePrecision;
        ss << ", deviceLayout: " << pGraphArgumentProperties->deviceLayout;
        ss << ", quantReverseScale: " << pGraphArgumentProperties->quantReverseScale;
        ss << ", quantZeroPoint: "
           << static_cast<unsigned>(pGraphArgumentProperties->quantZeroPoint);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeGraphGetArgumentProperties2(ze_graph_handle_t hGraph,
                                    uint32_t argIndex,
                                    ze_graph_argument_properties_2_t *pGraphArgumentProperties) {
    TRACE_EVENT_BEGIN("API", "zeGraphGetArgumentProperties2");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetArgumentProperties2(hGraph,
                                                          argIndex,
                                                          pGraphArgumentProperties) +
                         "..\n";
}
inline void
trace_zeGraphGetArgumentProperties2(ze_result_t ret,
                                    ze_graph_handle_t hGraph,
                                    uint32_t argIndex,
                                    ze_graph_argument_properties_2_t *pGraphArgumentProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetArgumentProperties2(hGraph,
                                                          argIndex,
                                                          pGraphArgumentProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphGetArgumentMetadata(ze_graph_handle_t hGraph,
                                  uint32_t argIndex,
                                  ze_graph_argument_metadata_t *pGraphArgumentMetadata) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphGetArgumentMetadata(";
    ss << "hGraph: " << hGraph;
    ss << ", argIndex: " << argIndex;
    if (pGraphArgumentMetadata == nullptr) {
        ss << ", pGraphArgumentMetadata: nullptr";
    } else {
        ss << ", pGraphArgumentMetadata {";
        ss << "stype: " << pGraphArgumentMetadata->stype;
        ss << ", pNext: " << pGraphArgumentMetadata->pNext;
        ss << ", type: " << pGraphArgumentMetadata->type;
        ss << ", friendly_name: " << pGraphArgumentMetadata->friendly_name;
        ss << ", data_type: " << pGraphArgumentMetadata->data_type;
        ss << ", shape: " << pGraphArgumentMetadata->shape[0] << " "
           << pGraphArgumentMetadata->shape[1] << " " << pGraphArgumentMetadata->shape[2] << " "
           << pGraphArgumentMetadata->shape[3] << " " << pGraphArgumentMetadata->shape[4] << " "
           << pGraphArgumentMetadata->shape[5] << " " << pGraphArgumentMetadata->shape[6] << " "
           << pGraphArgumentMetadata->shape[7];
        ss << ", shape_size: " << pGraphArgumentMetadata->shape_size;
        ss << ", tensor_names_count: " << pGraphArgumentMetadata->tensor_names_count;
        ss << ", input_name: " << pGraphArgumentMetadata->input_name;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphGetArgumentMetadata(ze_graph_handle_t hGraph,
                                             uint32_t argIndex,
                                             ze_graph_argument_metadata_t *pGraphArgumentMetadata) {
    TRACE_EVENT_BEGIN("API", "zeGraphGetArgumentMetadata");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetArgumentMetadata(hGraph, argIndex, pGraphArgumentMetadata) +
                         "..\n";
}
inline void trace_zeGraphGetArgumentMetadata(ze_result_t ret,
                                             ze_graph_handle_t hGraph,
                                             uint32_t argIndex,
                                             ze_graph_argument_metadata_t *pGraphArgumentMetadata) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetArgumentMetadata(hGraph, argIndex, pGraphArgumentMetadata) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphGetArgumentProperties3(ze_graph_handle_t hGraph,
                                     uint32_t argIndex,
                                     ze_graph_argument_properties_3_t *pGraphArgumentProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphGetArgumentProperties3(";
    ss << "hGraph: " << hGraph;
    ss << ", argIndex: " << argIndex;
    if (pGraphArgumentProperties == nullptr) {
        ss << ", pGraphArgumentProperties: nullptr";
    } else {
        ss << ", pGraphArgumentProperties {";
        ss << "stype: " << pGraphArgumentProperties->stype;
        ss << ", pNext: " << pGraphArgumentProperties->pNext;
        ss << ", name: " << pGraphArgumentProperties->name;
        ss << ", type: " << pGraphArgumentProperties->type;
        ss << ", dims: " << pGraphArgumentProperties->dims[0] << " "
           << pGraphArgumentProperties->dims[1] << " " << pGraphArgumentProperties->dims[2] << " "
           << pGraphArgumentProperties->dims[3] << " " << pGraphArgumentProperties->dims[4];
        ss << ", networkPrecision: " << pGraphArgumentProperties->networkPrecision;
        ss << ", networkLayout: " << pGraphArgumentProperties->networkLayout;
        ss << ", devicePrecision: " << pGraphArgumentProperties->devicePrecision;
        ss << ", deviceLayout: " << pGraphArgumentProperties->deviceLayout;
        ss << ", quantReverseScale: " << pGraphArgumentProperties->quantReverseScale;
        ss << ", quantZeroPoint: "
           << static_cast<unsigned>(pGraphArgumentProperties->quantZeroPoint);
        ss << ", dims_count: " << pGraphArgumentProperties->dims_count;
        ss << ", debug_friendly_name: " << pGraphArgumentProperties->debug_friendly_name;
        ss << ", associated_tensor_names_count: "
           << pGraphArgumentProperties->associated_tensor_names_count;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeGraphGetArgumentProperties3(ze_graph_handle_t hGraph,
                                    uint32_t argIndex,
                                    ze_graph_argument_properties_3_t *pGraphArgumentProperties) {
    TRACE_EVENT_BEGIN("API", "zeGraphGetArgumentProperties3");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetArgumentProperties3(hGraph,
                                                          argIndex,
                                                          pGraphArgumentProperties) +
                         "..\n";
}
inline void
trace_zeGraphGetArgumentProperties3(ze_result_t ret,
                                    ze_graph_handle_t hGraph,
                                    uint32_t argIndex,
                                    ze_graph_argument_properties_3_t *pGraphArgumentProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetArgumentProperties3(hGraph,
                                                          argIndex,
                                                          pGraphArgumentProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphQueryNetworkCreate(ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 const ze_graph_desc_t *desc,
                                 ze_graph_query_network_handle_t *phGraphQueryNetwork) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphQueryNetworkCreate(";
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", format: " << desc->format;
        ss << ", inputSize: " << desc->inputSize;
        if (desc->pInput == nullptr) {
            ss << ", pInput: nullptr";
        } else {
            ss << ", pInput: " << static_cast<const void *>(desc->pInput);
        }
        if (desc->pBuildFlags == nullptr) {
            ss << ", pBuildFlags: nullptr";
        } else {
            ss << ", pBuildFlags: " << desc->pBuildFlags;
        }
        ss << "}";
    }
    if (phGraphQueryNetwork == nullptr) {
        ss << ", phGraphQueryNetwork: nullptr";
    } else {
        ss << ", phGraphQueryNetwork: " << *phGraphQueryNetwork;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphQueryNetworkCreate(ze_context_handle_t hContext,
                                            ze_device_handle_t hDevice,
                                            const ze_graph_desc_t *desc,
                                            ze_graph_query_network_handle_t *phGraphQueryNetwork) {
    TRACE_EVENT_BEGIN("API", "zeGraphQueryNetworkCreate");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphQueryNetworkCreate(hContext,
                                                      hDevice,
                                                      desc,
                                                      phGraphQueryNetwork) +
                         "..\n";
}
inline void trace_zeGraphQueryNetworkCreate(ze_result_t ret,
                                            ze_context_handle_t hContext,
                                            ze_device_handle_t hDevice,
                                            const ze_graph_desc_t *desc,
                                            ze_graph_query_network_handle_t *phGraphQueryNetwork) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphQueryNetworkCreate(hContext,
                                                      hDevice,
                                                      desc,
                                                      phGraphQueryNetwork) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphQueryNetworkDestroy(ze_graph_query_network_handle_t hGraphQueryNetwork) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphQueryNetworkDestroy(";
    ss << "hGraphQueryNetwork: " << hGraphQueryNetwork;
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphQueryNetworkDestroy(ze_graph_query_network_handle_t hGraphQueryNetwork) {
    TRACE_EVENT_BEGIN("API", "zeGraphQueryNetworkDestroy");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphQueryNetworkDestroy(hGraphQueryNetwork) + "..\n";
}
inline void trace_zeGraphQueryNetworkDestroy(ze_result_t ret,
                                             ze_graph_query_network_handle_t hGraphQueryNetwork) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphQueryNetworkDestroy(hGraphQueryNetwork) + trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphQueryNetworkGetSupportedLayers(ze_graph_query_network_handle_t hGraphQueryNetwork,
                                             size_t *pSize,
                                             char *pSupportedLayers) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphQueryNetworkGetSupportedLayers(";
    ss << "hGraphQueryNetwork: " << hGraphQueryNetwork;
    if (pSize == nullptr) {
        ss << ", *pSize: nullptr";
    } else {
        ss << ", *pSize: " << *pSize;
    }
    if (pSupportedLayers == nullptr) {
        ss << ", *pSupportedLayers: nullptr";
    } else {
        ss << ", *pSupportedLayers: " << *pSupportedLayers;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeGraphQueryNetworkGetSupportedLayers(ze_graph_query_network_handle_t hGraphQueryNetwork,
                                            size_t *pSize,
                                            char *pSupportedLayers) {
    TRACE_EVENT_BEGIN("API", "zeGraphQueryNetworkGetSupportedLayers");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphQueryNetworkGetSupportedLayers(hGraphQueryNetwork,
                                                                  pSize,
                                                                  pSupportedLayers) +
                         "..\n";
}
inline void
trace_zeGraphQueryNetworkGetSupportedLayers(ze_result_t ret,
                                            ze_graph_query_network_handle_t hGraphQueryNetwork,
                                            size_t *pSize,
                                            char *pSupportedLayers) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphQueryNetworkGetSupportedLayers(hGraphQueryNetwork,
                                                                  pSize,
                                                                  pSupportedLayers) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphBuildLogGetString(ze_graph_handle_t hGraph, uint32_t *pSize, char *pBuildLog) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphBuildLogGetString(";
    ss << "hGraph: " << hGraph;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    if (pBuildLog == nullptr) {
        ss << ", pBuildLog: nullptr";
    } else {
        ss << ", pBuildLog: " << pBuildLog;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeGraphBuildLogGetString(ze_graph_handle_t hGraph, uint32_t *pSize, char *pBuildLog) {
    TRACE_EVENT_BEGIN("API", "zeGraphBuildLogGetString");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphBuildLogGetString(hGraph, pSize, pBuildLog) + "..\n";
}
inline void trace_zeGraphBuildLogGetString(ze_result_t ret,
                                           ze_graph_handle_t hGraph,
                                           uint32_t *pSize,
                                           char *pBuildLog) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphBuildLogGetString(hGraph, pSize, pBuildLog) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphCreate2(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         const ze_graph_desc_2_t *desc,
                                         ze_graph_handle_t *phGraph) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphCreate2(";
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", format: " << desc->format;
        ss << ", inputSize: " << desc->inputSize;
        if (desc->pInput == nullptr) {
            ss << ", pInput: nullptr";
        } else {
            ss << ", pInput: " << static_cast<const void *>(desc->pInput);
        }
        if (desc->pBuildFlags == nullptr) {
            ss << ", pBuildFlags: nullptr";
        } else {
            ss << ", pBuildFlags: " << desc->pBuildFlags;
        }
        ss << ", flags: " << desc->flags;
        ss << "}";
    }
    if (phGraph == nullptr) {
        ss << ", phGraph: nullptr";
    } else {
        ss << ", phGraph: " << *phGraph;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphCreate2(ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 const ze_graph_desc_2_t *desc,
                                 ze_graph_handle_t *phGraph) {
    TRACE_EVENT_BEGIN("API", "zeGraphCreate2");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphCreate2(hContext, hDevice, desc, phGraph) + "..\n";
}
inline void trace_zeGraphCreate2(ze_result_t ret,
                                 ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 const ze_graph_desc_2_t *desc,
                                 ze_graph_handle_t *phGraph) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphCreate2(hContext, hDevice, desc, phGraph) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGraphQueryNetworkCreate2(ze_context_handle_t hContext,
                                  ze_device_handle_t hDevice,
                                  const ze_graph_desc_2_t *desc,
                                  ze_graph_query_network_handle_t *phGraphQueryNetwork) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphQueryNetworkCreate2(";
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", format: " << desc->format;
        ss << ", inputSize: " << desc->inputSize;
        if (desc->pInput == nullptr) {
            ss << ", pInput: nullptr";
        } else {
            ss << ", pInput: " << static_cast<const void *>(desc->pInput);
        }
        if (desc->pBuildFlags == nullptr) {
            ss << ", pBuildFlags: nullptr";
        } else {
            ss << ", pBuildFlags: " << desc->pBuildFlags;
        }
        ss << ", flags: " << desc->flags;
        ss << "}";
    }
    if (phGraphQueryNetwork == nullptr) {
        ss << ", phGraphQueryNetwork: nullptr";
    } else {
        ss << ", phGraphQueryNetwork: " << *phGraphQueryNetwork;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphQueryNetworkCreate2(ze_context_handle_t hContext,
                                             ze_device_handle_t hDevice,
                                             const ze_graph_desc_2_t *desc,
                                             ze_graph_query_network_handle_t *phGraphQueryNetwork) {
    TRACE_EVENT_BEGIN("API", "zeGraphQueryNetworkCreate2");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphQueryNetworkCreate2(hContext,
                                                       hDevice,
                                                       desc,
                                                       phGraphQueryNetwork) +
                         "..\n";
}
inline void trace_zeGraphQueryNetworkCreate2(ze_result_t ret,
                                             ze_context_handle_t hContext,
                                             ze_device_handle_t hDevice,
                                             const ze_graph_desc_2_t *desc,
                                             ze_graph_query_network_handle_t *phGraphQueryNetwork) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphQueryNetworkCreate2(hContext,
                                                       hDevice,
                                                       desc,
                                                       phGraphQueryNetwork) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphQueryContextMemory(ze_context_handle_t hContext,
                                                    ze_graph_memory_query_type_t type,
                                                    ze_graph_memory_query_t *query) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphQueryContextMemory(";
    ss << "hContext: " << hContext;
    ss << ", type: " << type;
    if (query == nullptr) {
        ss << ", query: nullptr";
    } else {
        ss << ", query {";
        ss << "total: " << query->total;
        ss << ", allocated: " << query->allocated;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphQueryContextMemory(ze_context_handle_t hContext,
                                            ze_graph_memory_query_type_t type,
                                            ze_graph_memory_query_t *query) {
    TRACE_EVENT_BEGIN("API", "zeGraphQueryContextMemory");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphQueryContextMemory(hContext, type, query) + "..\n";
}
inline void trace_zeGraphQueryContextMemory(ze_result_t ret,
                                            ze_context_handle_t hContext,
                                            ze_graph_memory_query_type_t type,
                                            ze_graph_memory_query_t *query) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphQueryContextMemory(hContext, type, query) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeDeviceGetGraphProperties2(ze_device_handle_t hDevice,
                                   ze_device_graph_properties_2_t *pDeviceGraphProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeDeviceGetGraphProperties2(";
    ss << "hDevice: " << hDevice;
    if (pDeviceGraphProperties == nullptr) {
        ss << ", *pDeviceGraphProperties: nullptr";
    } else {
        ss << ", *pDeviceGraphProperties {";
        ss << "stype: " << pDeviceGraphProperties->stype;
        ss << ", pNext: " << pDeviceGraphProperties->pNext;
        ss << ", graphExtensionVersion: " << pDeviceGraphProperties->graphExtensionVersion;
        ss << ", compilerVersion {";
        ss << "major: " << pDeviceGraphProperties->compilerVersion.major;
        ss << ", minor: " << pDeviceGraphProperties->compilerVersion.minor;
        ss << "}";
        ss << ", graphFormatsSupported: " << pDeviceGraphProperties->graphFormatsSupported;
        ss << ", maxOVOpsetVersionSupported: "
           << pDeviceGraphProperties->maxOVOpsetVersionSupported;
        ss << ", elfVersion {";
        ss << "major: " << pDeviceGraphProperties->elfVersion.major;
        ss << ", minor: " << pDeviceGraphProperties->elfVersion.minor;
        ss << ", patch: " << pDeviceGraphProperties->elfVersion.patch;
        ss << "}";
        ss << ", runtimeVersion {";
        ss << "major: " << pDeviceGraphProperties->runtimeVersion.major;
        ss << ", minor: " << pDeviceGraphProperties->runtimeVersion.minor;
        ss << ", patch: " << pDeviceGraphProperties->runtimeVersion.patch;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeDeviceGetGraphProperties2(ze_device_handle_t hDevice,
                                  ze_device_graph_properties_2_t *pDeviceGraphProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetGraphProperties2");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeDeviceGetGraphProperties2(hDevice, pDeviceGraphProperties) + "..\n";
}
inline void
trace_zeDeviceGetGraphProperties2(ze_result_t ret,
                                  ze_device_handle_t hDevice,
                                  ze_device_graph_properties_2_t *pDeviceGraphProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeDeviceGetGraphProperties2(hDevice, pDeviceGraphProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphGetNativeBinary2(ze_graph_handle_t hGraph,
                                                  size_t *pSize,
                                                  const uint8_t **pGraphNativeBinary) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphGetNativeBinary2(";
    ss << "hGraph: " << hGraph;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    ss << ", pGraphNativeBinary: " << pGraphNativeBinary;
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphGetNativeBinary2(ze_graph_handle_t hGraph,
                                          size_t *pSize,
                                          const uint8_t **pGraphNativeBinary) {
    TRACE_EVENT_BEGIN("API", "zeGraphGetNativeBinary2");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetNativeBinary2(hGraph, pSize, pGraphNativeBinary) + "..\n";
}
inline void trace_zeGraphGetNativeBinary2(ze_result_t ret,
                                          ze_graph_handle_t hGraph,
                                          size_t *pSize,
                                          const uint8_t **pGraphNativeBinary) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetNativeBinary2(hGraph, pSize, pGraphNativeBinary) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphGetProperties2(ze_graph_handle_t hGraph,
                                                ze_graph_properties_2_t *pGraphProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphGetProperties2(";
    ss << "hGraph: " << hGraph;
    if (pGraphProperties == nullptr) {
        ss << ", pGraphProperties: nullptr";
    } else {
        ss << ", pGraphProperties {";
        ss << "stype: " << pGraphProperties->stype;
        ss << ", pNext: " << pGraphProperties->pNext;
        ss << ", numGraphArgs: " << pGraphProperties->numGraphArgs;
        ss << ", initStageRequired: " << pGraphProperties->initStageRequired;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphGetProperties2(ze_graph_handle_t hGraph,
                                        ze_graph_properties_2_t *pGraphProperties) {
    TRACE_EVENT_BEGIN("API", "zeGraphGetProperties2");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetProperties2(hGraph, pGraphProperties) + "..\n";
}
inline void trace_zeGraphGetProperties2(ze_result_t ret,
                                        ze_graph_handle_t hGraph,
                                        ze_graph_properties_2_t *pGraphProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetProperties2(hGraph, pGraphProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphInitialize(ze_graph_handle_t hGraph) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphInitialize(";
    ss << "hGraph: " << hGraph;
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphInitialize(ze_graph_handle_t hGraph) {
    TRACE_EVENT_BEGIN("API", "zeGraphInitialize");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphInitialize(hGraph) + "..\n";
}
inline void trace_zeGraphInitialize(ze_result_t ret, ze_graph_handle_t hGraph) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphInitialize(hGraph) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphCompilerGetSupportedOptions(ze_npu_options_type_t type,
                                                             size_t *pSize,
                                                             char *pSupportedOptions) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphCompilerGetSupportedOptions(";
    ss << "type: " << type;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    if (pSupportedOptions == nullptr) {
        ss << ", pSupportedOptions: nullptr";
    } else {
        ss << ", pSupportedOptions: " << pSupportedOptions;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphCompilerGetSupportedOptions(ze_npu_options_type_t type,
                                                     size_t *pSize,
                                                     char *pSupportedOptions) {
    TRACE_EVENT_BEGIN("API", "zeGraphCompilerGetSupportedOptions");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphCompilerGetSupportedOptions(type, pSize, pSupportedOptions) +
                         "..\n";
}

inline void trace_zeGraphCompilerGetSupportedOptions(ze_result_t ret,
                                                     ze_npu_options_type_t type,
                                                     size_t *pSize,
                                                     char *pSupportedOptions) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphCompilerGetSupportedOptions(type, pSize, pSupportedOptions) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphCompilerIsOptionSupported(ze_npu_options_type_t type,
                                                           const char *pOption,
                                                           const char *pValue) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphCompilerIsOptionSupported(";
    ss << "type: " << type;
    if (pOption == nullptr) {
        ss << ", pOption: nullptr";
    } else {
        ss << ", pOption: " << pOption;
    }
    if (pValue == nullptr) {
        ss << ", pValue: nullptr";
    } else {
        ss << ", pValue: " << pValue;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphCompilerIsOptionSupported(ze_npu_options_type_t type,
                                                   const char *pOption,
                                                   const char *pValue) {
    TRACE_EVENT_BEGIN("API", "zeGraphCompilerIsOptionSupported");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphCompilerIsOptionSupported(type, pOption, pValue) + "..\n";
}

inline void trace_zeGraphCompilerIsOptionSupported(ze_result_t ret,
                                                   ze_npu_options_type_t type,
                                                   const char *pOption,
                                                   const char *pValue) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphCompilerIsOptionSupported(type, pOption, pValue) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphCreate3(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         const ze_graph_desc_2_t *desc,
                                         ze_graph_handle_t *phGraph,
                                         ze_graph_build_log_handle_t *phGraphBuildLog) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphCreate3(";
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", format: " << desc->format;
        ss << ", inputSize: " << desc->inputSize;
        if (desc->pInput == nullptr) {
            ss << ", pInput: nullptr";
        } else {
            ss << ", pInput: " << static_cast<const void *>(desc->pInput);
        }
        if (desc->pBuildFlags == nullptr) {
            ss << ", pBuildFlags: nullptr";
        } else {
            ss << ", pBuildFlags: " << desc->pBuildFlags;
        }
        ss << ", flags: " << desc->flags;
        ss << "}";
    }
    if (phGraph == nullptr) {
        ss << ", phGraph: nullptr";
    } else {
        ss << ", phGraph: " << *phGraph;
    }
    if (phGraphBuildLog == nullptr) {
        ss << ", phGraphBuildLog: nullptr";
    } else {
        ss << ", phGraphBuildLog: " << *phGraphBuildLog;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphCreate3(ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 const ze_graph_desc_2_t *desc,
                                 ze_graph_handle_t *phGraph,
                                 ze_graph_build_log_handle_t *phGraphBuildLog) {
    TRACE_EVENT_BEGIN("API", "zeGraphCreate3");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphCreate3(hContext, hDevice, desc, phGraph, phGraphBuildLog) +
                         "..\n";
}
inline void trace_zeGraphCreate3(ze_result_t ret,
                                 ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 const ze_graph_desc_2_t *desc,
                                 ze_graph_handle_t *phGraph,
                                 ze_graph_build_log_handle_t *phGraphBuildLog) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphCreate3(hContext, hDevice, desc, phGraph, phGraphBuildLog) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphBuildLogGetString2(ze_graph_build_log_handle_t hGraphBuildLog,
                                                    uint32_t *pSize,
                                                    char *pBuildLog) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphBuildLogGetString2(";
    ss << "hGraphBuildLog: " << hGraphBuildLog;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    if (pBuildLog == nullptr) {
        ss << ", pBuildLog: nullptr";
    } else {
        ss << ", pBuildLog: " << pBuildLog;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphBuildLogGetString2(ze_graph_build_log_handle_t hGraphBuildLog,
                                            uint32_t *pSize,
                                            char *pBuildLog) {
    TRACE_EVENT_BEGIN("API", "zeGraphBuildLogGetString2");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphBuildLogGetString2(hGraphBuildLog, pSize, pBuildLog) + "..\n";
}
inline void trace_zeGraphBuildLogGetString2(ze_result_t ret,
                                            ze_graph_build_log_handle_t hGraphBuildLog,
                                            uint32_t *pSize,
                                            char *pBuildLog) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphBuildLogGetString2(hGraphBuildLog, pSize, pBuildLog) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphBuildLogDestroy(ze_graph_build_log_handle_t hGraphBuildLog) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphBuildLogDestroy(";
    ss << "hGraphBuildLog: " << hGraphBuildLog;
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphBuildLogDestroy(ze_graph_build_log_handle_t hGraphBuildLog) {
    TRACE_EVENT_BEGIN("API", "zeGraphBuildLogDestroy");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphBuildLogDestroy(hGraphBuildLog) + "..\n";
}
inline void trace_zeGraphBuildLogDestroy(ze_result_t ret,
                                         ze_graph_build_log_handle_t hGraphBuildLog) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphBuildLogDestroy(hGraphBuildLog) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGraphGetProperties3(ze_graph_handle_t hGraph,
                                                ze_graph_properties_3_t *pGraphProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeGraphGetProperties3(";
    ss << "hGraph: " << hGraph;
    if (pGraphProperties == nullptr) {
        ss << ", pGraphProperties: nullptr";
    } else {
        ss << ", pGraphProperties {";
        ss << "stype: " << pGraphProperties->stype;
        ss << ", pNext: " << pGraphProperties->pNext;
        ss << ", numGraphArgs: " << pGraphProperties->numGraphArgs;
        ss << ", initStageRequired: " << pGraphProperties->initStageRequired;
        ss << ", flags: " << pGraphProperties->flags;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGraphGetProperties3(ze_graph_handle_t hGraph,
                                        ze_graph_properties_3_t *pGraphProperties) {
    TRACE_EVENT_BEGIN("API", "zeGraphGetProperties3");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetProperties3(hGraph, pGraphProperties) + "..\n";
}
inline void trace_zeGraphGetProperties3(ze_result_t ret,
                                        ze_graph_handle_t hGraph,
                                        ze_graph_properties_3_t *pGraphProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeGraphGetProperties3(hGraph, pGraphProperties) +
                         trace_ze_result_t(ret);
}
