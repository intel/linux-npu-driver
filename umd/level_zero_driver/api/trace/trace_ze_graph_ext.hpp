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
            ss << ", pInput: " << desc->pInput;
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
        ss << ", name: " << pGraphArgumentProperties->name[0] << pGraphArgumentProperties->name[1]
           << pGraphArgumentProperties->name[2] << pGraphArgumentProperties->name[3]
           << pGraphArgumentProperties->name[4] << pGraphArgumentProperties->name[5]
           << pGraphArgumentProperties->name[6] << pGraphArgumentProperties->name[7]
           << pGraphArgumentProperties->name[8] << pGraphArgumentProperties->name[9]
           << pGraphArgumentProperties->name[10] << pGraphArgumentProperties->name[11]
           << pGraphArgumentProperties->name[12] << pGraphArgumentProperties->name[13]
           << pGraphArgumentProperties->name[14] << pGraphArgumentProperties->name[15]
           << pGraphArgumentProperties->name[16] << pGraphArgumentProperties->name[17]
           << pGraphArgumentProperties->name[18] << pGraphArgumentProperties->name[19]
           << pGraphArgumentProperties->name[20] << pGraphArgumentProperties->name[21]
           << pGraphArgumentProperties->name[22] << pGraphArgumentProperties->name[23]
           << pGraphArgumentProperties->name[24] << pGraphArgumentProperties->name[25]
           << pGraphArgumentProperties->name[26] << pGraphArgumentProperties->name[27]
           << pGraphArgumentProperties->name[28] << pGraphArgumentProperties->name[29]
           << pGraphArgumentProperties->name[30] << pGraphArgumentProperties->name[31]
           << pGraphArgumentProperties->name[32] << pGraphArgumentProperties->name[33]
           << pGraphArgumentProperties->name[34] << pGraphArgumentProperties->name[35]
           << pGraphArgumentProperties->name[36] << pGraphArgumentProperties->name[37]
           << pGraphArgumentProperties->name[38] << pGraphArgumentProperties->name[39]
           << pGraphArgumentProperties->name[40] << pGraphArgumentProperties->name[41]
           << pGraphArgumentProperties->name[42] << pGraphArgumentProperties->name[43]
           << pGraphArgumentProperties->name[44] << pGraphArgumentProperties->name[45]
           << pGraphArgumentProperties->name[46] << pGraphArgumentProperties->name[47]
           << pGraphArgumentProperties->name[48] << pGraphArgumentProperties->name[49]
           << pGraphArgumentProperties->name[50] << pGraphArgumentProperties->name[51]
           << pGraphArgumentProperties->name[52] << pGraphArgumentProperties->name[53]
           << pGraphArgumentProperties->name[54] << pGraphArgumentProperties->name[55]
           << pGraphArgumentProperties->name[56] << pGraphArgumentProperties->name[57]
           << pGraphArgumentProperties->name[58] << pGraphArgumentProperties->name[59]
           << pGraphArgumentProperties->name[60] << pGraphArgumentProperties->name[61]
           << pGraphArgumentProperties->name[62] << pGraphArgumentProperties->name[63]
           << pGraphArgumentProperties->name[64] << pGraphArgumentProperties->name[65]
           << pGraphArgumentProperties->name[66] << pGraphArgumentProperties->name[67]
           << pGraphArgumentProperties->name[68] << pGraphArgumentProperties->name[69]
           << pGraphArgumentProperties->name[70] << pGraphArgumentProperties->name[71]
           << pGraphArgumentProperties->name[72] << pGraphArgumentProperties->name[73]
           << pGraphArgumentProperties->name[74] << pGraphArgumentProperties->name[75]
           << pGraphArgumentProperties->name[76] << pGraphArgumentProperties->name[77]
           << pGraphArgumentProperties->name[78] << pGraphArgumentProperties->name[79]
           << pGraphArgumentProperties->name[80] << pGraphArgumentProperties->name[81]
           << pGraphArgumentProperties->name[82] << pGraphArgumentProperties->name[83]
           << pGraphArgumentProperties->name[84] << pGraphArgumentProperties->name[85]
           << pGraphArgumentProperties->name[86] << pGraphArgumentProperties->name[87]
           << pGraphArgumentProperties->name[88] << pGraphArgumentProperties->name[89]
           << pGraphArgumentProperties->name[90] << pGraphArgumentProperties->name[91]
           << pGraphArgumentProperties->name[92] << pGraphArgumentProperties->name[93]
           << pGraphArgumentProperties->name[94] << pGraphArgumentProperties->name[95]
           << pGraphArgumentProperties->name[96] << pGraphArgumentProperties->name[97]
           << pGraphArgumentProperties->name[98] << pGraphArgumentProperties->name[99]
           << pGraphArgumentProperties->name[100] << pGraphArgumentProperties->name[101]
           << pGraphArgumentProperties->name[102] << pGraphArgumentProperties->name[103]
           << pGraphArgumentProperties->name[104] << pGraphArgumentProperties->name[105]
           << pGraphArgumentProperties->name[106] << pGraphArgumentProperties->name[107]
           << pGraphArgumentProperties->name[108] << pGraphArgumentProperties->name[109]
           << pGraphArgumentProperties->name[110] << pGraphArgumentProperties->name[111]
           << pGraphArgumentProperties->name[112] << pGraphArgumentProperties->name[113]
           << pGraphArgumentProperties->name[114] << pGraphArgumentProperties->name[115]
           << pGraphArgumentProperties->name[116] << pGraphArgumentProperties->name[117]
           << pGraphArgumentProperties->name[118] << pGraphArgumentProperties->name[119]
           << pGraphArgumentProperties->name[120] << pGraphArgumentProperties->name[121]
           << pGraphArgumentProperties->name[122] << pGraphArgumentProperties->name[123]
           << pGraphArgumentProperties->name[124] << pGraphArgumentProperties->name[125]
           << pGraphArgumentProperties->name[126] << pGraphArgumentProperties->name[127]
           << pGraphArgumentProperties->name[128] << pGraphArgumentProperties->name[129]
           << pGraphArgumentProperties->name[130] << pGraphArgumentProperties->name[131]
           << pGraphArgumentProperties->name[132] << pGraphArgumentProperties->name[133]
           << pGraphArgumentProperties->name[134] << pGraphArgumentProperties->name[135]
           << pGraphArgumentProperties->name[136] << pGraphArgumentProperties->name[137]
           << pGraphArgumentProperties->name[138] << pGraphArgumentProperties->name[139]
           << pGraphArgumentProperties->name[140] << pGraphArgumentProperties->name[141]
           << pGraphArgumentProperties->name[142] << pGraphArgumentProperties->name[143]
           << pGraphArgumentProperties->name[144] << pGraphArgumentProperties->name[145]
           << pGraphArgumentProperties->name[146] << pGraphArgumentProperties->name[147]
           << pGraphArgumentProperties->name[148] << pGraphArgumentProperties->name[149]
           << pGraphArgumentProperties->name[150] << pGraphArgumentProperties->name[151]
           << pGraphArgumentProperties->name[152] << pGraphArgumentProperties->name[153]
           << pGraphArgumentProperties->name[154] << pGraphArgumentProperties->name[155]
           << pGraphArgumentProperties->name[156] << pGraphArgumentProperties->name[157]
           << pGraphArgumentProperties->name[158] << pGraphArgumentProperties->name[159]
           << pGraphArgumentProperties->name[160] << pGraphArgumentProperties->name[161]
           << pGraphArgumentProperties->name[162] << pGraphArgumentProperties->name[163]
           << pGraphArgumentProperties->name[164] << pGraphArgumentProperties->name[165]
           << pGraphArgumentProperties->name[166] << pGraphArgumentProperties->name[167]
           << pGraphArgumentProperties->name[168] << pGraphArgumentProperties->name[169]
           << pGraphArgumentProperties->name[170] << pGraphArgumentProperties->name[171]
           << pGraphArgumentProperties->name[172] << pGraphArgumentProperties->name[173]
           << pGraphArgumentProperties->name[174] << pGraphArgumentProperties->name[175]
           << pGraphArgumentProperties->name[176] << pGraphArgumentProperties->name[177]
           << pGraphArgumentProperties->name[178] << pGraphArgumentProperties->name[179]
           << pGraphArgumentProperties->name[180] << pGraphArgumentProperties->name[181]
           << pGraphArgumentProperties->name[182] << pGraphArgumentProperties->name[183]
           << pGraphArgumentProperties->name[184] << pGraphArgumentProperties->name[185]
           << pGraphArgumentProperties->name[186] << pGraphArgumentProperties->name[187]
           << pGraphArgumentProperties->name[188] << pGraphArgumentProperties->name[189]
           << pGraphArgumentProperties->name[190] << pGraphArgumentProperties->name[191]
           << pGraphArgumentProperties->name[192] << pGraphArgumentProperties->name[193]
           << pGraphArgumentProperties->name[194] << pGraphArgumentProperties->name[195]
           << pGraphArgumentProperties->name[196] << pGraphArgumentProperties->name[197]
           << pGraphArgumentProperties->name[198] << pGraphArgumentProperties->name[199]
           << pGraphArgumentProperties->name[200] << pGraphArgumentProperties->name[201]
           << pGraphArgumentProperties->name[202] << pGraphArgumentProperties->name[203]
           << pGraphArgumentProperties->name[204] << pGraphArgumentProperties->name[205]
           << pGraphArgumentProperties->name[206] << pGraphArgumentProperties->name[207]
           << pGraphArgumentProperties->name[208] << pGraphArgumentProperties->name[209]
           << pGraphArgumentProperties->name[210] << pGraphArgumentProperties->name[211]
           << pGraphArgumentProperties->name[212] << pGraphArgumentProperties->name[213]
           << pGraphArgumentProperties->name[214] << pGraphArgumentProperties->name[215]
           << pGraphArgumentProperties->name[216] << pGraphArgumentProperties->name[217]
           << pGraphArgumentProperties->name[218] << pGraphArgumentProperties->name[219]
           << pGraphArgumentProperties->name[220] << pGraphArgumentProperties->name[221]
           << pGraphArgumentProperties->name[222] << pGraphArgumentProperties->name[223]
           << pGraphArgumentProperties->name[224] << pGraphArgumentProperties->name[225]
           << pGraphArgumentProperties->name[226] << pGraphArgumentProperties->name[227]
           << pGraphArgumentProperties->name[228] << pGraphArgumentProperties->name[229]
           << pGraphArgumentProperties->name[230] << pGraphArgumentProperties->name[231]
           << pGraphArgumentProperties->name[232] << pGraphArgumentProperties->name[233]
           << pGraphArgumentProperties->name[234] << pGraphArgumentProperties->name[235]
           << pGraphArgumentProperties->name[236] << pGraphArgumentProperties->name[237]
           << pGraphArgumentProperties->name[238] << pGraphArgumentProperties->name[239]
           << pGraphArgumentProperties->name[240] << pGraphArgumentProperties->name[241]
           << pGraphArgumentProperties->name[242] << pGraphArgumentProperties->name[243]
           << pGraphArgumentProperties->name[244] << pGraphArgumentProperties->name[245]
           << pGraphArgumentProperties->name[246] << pGraphArgumentProperties->name[247]
           << pGraphArgumentProperties->name[248] << pGraphArgumentProperties->name[249]
           << pGraphArgumentProperties->name[250] << pGraphArgumentProperties->name[251]
           << pGraphArgumentProperties->name[252] << pGraphArgumentProperties->name[253]
           << pGraphArgumentProperties->name[254] << pGraphArgumentProperties->name[255];
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
        ss << ", name: " << pGraphArgumentProperties->name[0] << pGraphArgumentProperties->name[1]
           << pGraphArgumentProperties->name[2] << pGraphArgumentProperties->name[3]
           << pGraphArgumentProperties->name[4] << pGraphArgumentProperties->name[5]
           << pGraphArgumentProperties->name[6] << pGraphArgumentProperties->name[7]
           << pGraphArgumentProperties->name[8] << pGraphArgumentProperties->name[9]
           << pGraphArgumentProperties->name[10] << pGraphArgumentProperties->name[11]
           << pGraphArgumentProperties->name[12] << pGraphArgumentProperties->name[13]
           << pGraphArgumentProperties->name[14] << pGraphArgumentProperties->name[15]
           << pGraphArgumentProperties->name[16] << pGraphArgumentProperties->name[17]
           << pGraphArgumentProperties->name[18] << pGraphArgumentProperties->name[19]
           << pGraphArgumentProperties->name[20] << pGraphArgumentProperties->name[21]
           << pGraphArgumentProperties->name[22] << pGraphArgumentProperties->name[23]
           << pGraphArgumentProperties->name[24] << pGraphArgumentProperties->name[25]
           << pGraphArgumentProperties->name[26] << pGraphArgumentProperties->name[27]
           << pGraphArgumentProperties->name[28] << pGraphArgumentProperties->name[29]
           << pGraphArgumentProperties->name[30] << pGraphArgumentProperties->name[31]
           << pGraphArgumentProperties->name[32] << pGraphArgumentProperties->name[33]
           << pGraphArgumentProperties->name[34] << pGraphArgumentProperties->name[35]
           << pGraphArgumentProperties->name[36] << pGraphArgumentProperties->name[37]
           << pGraphArgumentProperties->name[38] << pGraphArgumentProperties->name[39]
           << pGraphArgumentProperties->name[40] << pGraphArgumentProperties->name[41]
           << pGraphArgumentProperties->name[42] << pGraphArgumentProperties->name[43]
           << pGraphArgumentProperties->name[44] << pGraphArgumentProperties->name[45]
           << pGraphArgumentProperties->name[46] << pGraphArgumentProperties->name[47]
           << pGraphArgumentProperties->name[48] << pGraphArgumentProperties->name[49]
           << pGraphArgumentProperties->name[50] << pGraphArgumentProperties->name[51]
           << pGraphArgumentProperties->name[52] << pGraphArgumentProperties->name[53]
           << pGraphArgumentProperties->name[54] << pGraphArgumentProperties->name[55]
           << pGraphArgumentProperties->name[56] << pGraphArgumentProperties->name[57]
           << pGraphArgumentProperties->name[58] << pGraphArgumentProperties->name[59]
           << pGraphArgumentProperties->name[60] << pGraphArgumentProperties->name[61]
           << pGraphArgumentProperties->name[62] << pGraphArgumentProperties->name[63]
           << pGraphArgumentProperties->name[64] << pGraphArgumentProperties->name[65]
           << pGraphArgumentProperties->name[66] << pGraphArgumentProperties->name[67]
           << pGraphArgumentProperties->name[68] << pGraphArgumentProperties->name[69]
           << pGraphArgumentProperties->name[70] << pGraphArgumentProperties->name[71]
           << pGraphArgumentProperties->name[72] << pGraphArgumentProperties->name[73]
           << pGraphArgumentProperties->name[74] << pGraphArgumentProperties->name[75]
           << pGraphArgumentProperties->name[76] << pGraphArgumentProperties->name[77]
           << pGraphArgumentProperties->name[78] << pGraphArgumentProperties->name[79]
           << pGraphArgumentProperties->name[80] << pGraphArgumentProperties->name[81]
           << pGraphArgumentProperties->name[82] << pGraphArgumentProperties->name[83]
           << pGraphArgumentProperties->name[84] << pGraphArgumentProperties->name[85]
           << pGraphArgumentProperties->name[86] << pGraphArgumentProperties->name[87]
           << pGraphArgumentProperties->name[88] << pGraphArgumentProperties->name[89]
           << pGraphArgumentProperties->name[90] << pGraphArgumentProperties->name[91]
           << pGraphArgumentProperties->name[92] << pGraphArgumentProperties->name[93]
           << pGraphArgumentProperties->name[94] << pGraphArgumentProperties->name[95]
           << pGraphArgumentProperties->name[96] << pGraphArgumentProperties->name[97]
           << pGraphArgumentProperties->name[98] << pGraphArgumentProperties->name[99]
           << pGraphArgumentProperties->name[100] << pGraphArgumentProperties->name[101]
           << pGraphArgumentProperties->name[102] << pGraphArgumentProperties->name[103]
           << pGraphArgumentProperties->name[104] << pGraphArgumentProperties->name[105]
           << pGraphArgumentProperties->name[106] << pGraphArgumentProperties->name[107]
           << pGraphArgumentProperties->name[108] << pGraphArgumentProperties->name[109]
           << pGraphArgumentProperties->name[110] << pGraphArgumentProperties->name[111]
           << pGraphArgumentProperties->name[112] << pGraphArgumentProperties->name[113]
           << pGraphArgumentProperties->name[114] << pGraphArgumentProperties->name[115]
           << pGraphArgumentProperties->name[116] << pGraphArgumentProperties->name[117]
           << pGraphArgumentProperties->name[118] << pGraphArgumentProperties->name[119]
           << pGraphArgumentProperties->name[120] << pGraphArgumentProperties->name[121]
           << pGraphArgumentProperties->name[122] << pGraphArgumentProperties->name[123]
           << pGraphArgumentProperties->name[124] << pGraphArgumentProperties->name[125]
           << pGraphArgumentProperties->name[126] << pGraphArgumentProperties->name[127]
           << pGraphArgumentProperties->name[128] << pGraphArgumentProperties->name[129]
           << pGraphArgumentProperties->name[130] << pGraphArgumentProperties->name[131]
           << pGraphArgumentProperties->name[132] << pGraphArgumentProperties->name[133]
           << pGraphArgumentProperties->name[134] << pGraphArgumentProperties->name[135]
           << pGraphArgumentProperties->name[136] << pGraphArgumentProperties->name[137]
           << pGraphArgumentProperties->name[138] << pGraphArgumentProperties->name[139]
           << pGraphArgumentProperties->name[140] << pGraphArgumentProperties->name[141]
           << pGraphArgumentProperties->name[142] << pGraphArgumentProperties->name[143]
           << pGraphArgumentProperties->name[144] << pGraphArgumentProperties->name[145]
           << pGraphArgumentProperties->name[146] << pGraphArgumentProperties->name[147]
           << pGraphArgumentProperties->name[148] << pGraphArgumentProperties->name[149]
           << pGraphArgumentProperties->name[150] << pGraphArgumentProperties->name[151]
           << pGraphArgumentProperties->name[152] << pGraphArgumentProperties->name[153]
           << pGraphArgumentProperties->name[154] << pGraphArgumentProperties->name[155]
           << pGraphArgumentProperties->name[156] << pGraphArgumentProperties->name[157]
           << pGraphArgumentProperties->name[158] << pGraphArgumentProperties->name[159]
           << pGraphArgumentProperties->name[160] << pGraphArgumentProperties->name[161]
           << pGraphArgumentProperties->name[162] << pGraphArgumentProperties->name[163]
           << pGraphArgumentProperties->name[164] << pGraphArgumentProperties->name[165]
           << pGraphArgumentProperties->name[166] << pGraphArgumentProperties->name[167]
           << pGraphArgumentProperties->name[168] << pGraphArgumentProperties->name[169]
           << pGraphArgumentProperties->name[170] << pGraphArgumentProperties->name[171]
           << pGraphArgumentProperties->name[172] << pGraphArgumentProperties->name[173]
           << pGraphArgumentProperties->name[174] << pGraphArgumentProperties->name[175]
           << pGraphArgumentProperties->name[176] << pGraphArgumentProperties->name[177]
           << pGraphArgumentProperties->name[178] << pGraphArgumentProperties->name[179]
           << pGraphArgumentProperties->name[180] << pGraphArgumentProperties->name[181]
           << pGraphArgumentProperties->name[182] << pGraphArgumentProperties->name[183]
           << pGraphArgumentProperties->name[184] << pGraphArgumentProperties->name[185]
           << pGraphArgumentProperties->name[186] << pGraphArgumentProperties->name[187]
           << pGraphArgumentProperties->name[188] << pGraphArgumentProperties->name[189]
           << pGraphArgumentProperties->name[190] << pGraphArgumentProperties->name[191]
           << pGraphArgumentProperties->name[192] << pGraphArgumentProperties->name[193]
           << pGraphArgumentProperties->name[194] << pGraphArgumentProperties->name[195]
           << pGraphArgumentProperties->name[196] << pGraphArgumentProperties->name[197]
           << pGraphArgumentProperties->name[198] << pGraphArgumentProperties->name[199]
           << pGraphArgumentProperties->name[200] << pGraphArgumentProperties->name[201]
           << pGraphArgumentProperties->name[202] << pGraphArgumentProperties->name[203]
           << pGraphArgumentProperties->name[204] << pGraphArgumentProperties->name[205]
           << pGraphArgumentProperties->name[206] << pGraphArgumentProperties->name[207]
           << pGraphArgumentProperties->name[208] << pGraphArgumentProperties->name[209]
           << pGraphArgumentProperties->name[210] << pGraphArgumentProperties->name[211]
           << pGraphArgumentProperties->name[212] << pGraphArgumentProperties->name[213]
           << pGraphArgumentProperties->name[214] << pGraphArgumentProperties->name[215]
           << pGraphArgumentProperties->name[216] << pGraphArgumentProperties->name[217]
           << pGraphArgumentProperties->name[218] << pGraphArgumentProperties->name[219]
           << pGraphArgumentProperties->name[220] << pGraphArgumentProperties->name[221]
           << pGraphArgumentProperties->name[222] << pGraphArgumentProperties->name[223]
           << pGraphArgumentProperties->name[224] << pGraphArgumentProperties->name[225]
           << pGraphArgumentProperties->name[226] << pGraphArgumentProperties->name[227]
           << pGraphArgumentProperties->name[228] << pGraphArgumentProperties->name[229]
           << pGraphArgumentProperties->name[230] << pGraphArgumentProperties->name[231]
           << pGraphArgumentProperties->name[232] << pGraphArgumentProperties->name[233]
           << pGraphArgumentProperties->name[234] << pGraphArgumentProperties->name[235]
           << pGraphArgumentProperties->name[236] << pGraphArgumentProperties->name[237]
           << pGraphArgumentProperties->name[238] << pGraphArgumentProperties->name[239]
           << pGraphArgumentProperties->name[240] << pGraphArgumentProperties->name[241]
           << pGraphArgumentProperties->name[242] << pGraphArgumentProperties->name[243]
           << pGraphArgumentProperties->name[244] << pGraphArgumentProperties->name[245]
           << pGraphArgumentProperties->name[246] << pGraphArgumentProperties->name[247]
           << pGraphArgumentProperties->name[248] << pGraphArgumentProperties->name[249]
           << pGraphArgumentProperties->name[250] << pGraphArgumentProperties->name[251]
           << pGraphArgumentProperties->name[252] << pGraphArgumentProperties->name[253]
           << pGraphArgumentProperties->name[254] << pGraphArgumentProperties->name[255];
        ss << ", type: " << pGraphArgumentProperties->type;
        ss << ", dims: " << pGraphArgumentProperties->dims[0] << " "
           << pGraphArgumentProperties->dims[1] << " " << pGraphArgumentProperties->dims[2] << " "
           << pGraphArgumentProperties->dims[3] << " " << pGraphArgumentProperties->dims[4];
        ss << ", networkPrecision: " << pGraphArgumentProperties->networkPrecision;
        ss << ", networkLayout: " << pGraphArgumentProperties->networkLayout;
        ss << ", devicePrecision: " << pGraphArgumentProperties->devicePrecision;
        ss << ", deviceLayout: " << pGraphArgumentProperties->deviceLayout;
        ss << ", quantReverseScale: " << pGraphArgumentProperties->quantReverseScale;
        ss << ", quantZeroPoint: " << pGraphArgumentProperties->quantZeroPoint;
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
        ss << ", friendly_name: " << pGraphArgumentMetadata->friendly_name[0]
           << pGraphArgumentMetadata->friendly_name[1] << pGraphArgumentMetadata->friendly_name[2]
           << pGraphArgumentMetadata->friendly_name[3] << pGraphArgumentMetadata->friendly_name[4]
           << pGraphArgumentMetadata->friendly_name[5] << pGraphArgumentMetadata->friendly_name[6]
           << pGraphArgumentMetadata->friendly_name[7] << pGraphArgumentMetadata->friendly_name[8]
           << pGraphArgumentMetadata->friendly_name[9] << pGraphArgumentMetadata->friendly_name[10]
           << pGraphArgumentMetadata->friendly_name[11] << pGraphArgumentMetadata->friendly_name[12]
           << pGraphArgumentMetadata->friendly_name[13] << pGraphArgumentMetadata->friendly_name[14]
           << pGraphArgumentMetadata->friendly_name[15] << pGraphArgumentMetadata->friendly_name[16]
           << pGraphArgumentMetadata->friendly_name[17] << pGraphArgumentMetadata->friendly_name[18]
           << pGraphArgumentMetadata->friendly_name[19] << pGraphArgumentMetadata->friendly_name[20]
           << pGraphArgumentMetadata->friendly_name[21] << pGraphArgumentMetadata->friendly_name[22]
           << pGraphArgumentMetadata->friendly_name[23] << pGraphArgumentMetadata->friendly_name[24]
           << pGraphArgumentMetadata->friendly_name[25] << pGraphArgumentMetadata->friendly_name[26]
           << pGraphArgumentMetadata->friendly_name[27] << pGraphArgumentMetadata->friendly_name[28]
           << pGraphArgumentMetadata->friendly_name[29] << pGraphArgumentMetadata->friendly_name[30]
           << pGraphArgumentMetadata->friendly_name[31] << pGraphArgumentMetadata->friendly_name[32]
           << pGraphArgumentMetadata->friendly_name[33] << pGraphArgumentMetadata->friendly_name[34]
           << pGraphArgumentMetadata->friendly_name[35] << pGraphArgumentMetadata->friendly_name[36]
           << pGraphArgumentMetadata->friendly_name[37] << pGraphArgumentMetadata->friendly_name[38]
           << pGraphArgumentMetadata->friendly_name[39] << pGraphArgumentMetadata->friendly_name[40]
           << pGraphArgumentMetadata->friendly_name[41] << pGraphArgumentMetadata->friendly_name[42]
           << pGraphArgumentMetadata->friendly_name[43] << pGraphArgumentMetadata->friendly_name[44]
           << pGraphArgumentMetadata->friendly_name[45] << pGraphArgumentMetadata->friendly_name[46]
           << pGraphArgumentMetadata->friendly_name[47] << pGraphArgumentMetadata->friendly_name[48]
           << pGraphArgumentMetadata->friendly_name[49] << pGraphArgumentMetadata->friendly_name[50]
           << pGraphArgumentMetadata->friendly_name[51] << pGraphArgumentMetadata->friendly_name[52]
           << pGraphArgumentMetadata->friendly_name[53] << pGraphArgumentMetadata->friendly_name[54]
           << pGraphArgumentMetadata->friendly_name[55] << pGraphArgumentMetadata->friendly_name[56]
           << pGraphArgumentMetadata->friendly_name[57] << pGraphArgumentMetadata->friendly_name[58]
           << pGraphArgumentMetadata->friendly_name[59] << pGraphArgumentMetadata->friendly_name[60]
           << pGraphArgumentMetadata->friendly_name[61] << pGraphArgumentMetadata->friendly_name[62]
           << pGraphArgumentMetadata->friendly_name[63] << pGraphArgumentMetadata->friendly_name[64]
           << pGraphArgumentMetadata->friendly_name[65] << pGraphArgumentMetadata->friendly_name[66]
           << pGraphArgumentMetadata->friendly_name[67] << pGraphArgumentMetadata->friendly_name[68]
           << pGraphArgumentMetadata->friendly_name[69] << pGraphArgumentMetadata->friendly_name[70]
           << pGraphArgumentMetadata->friendly_name[71] << pGraphArgumentMetadata->friendly_name[72]
           << pGraphArgumentMetadata->friendly_name[73] << pGraphArgumentMetadata->friendly_name[74]
           << pGraphArgumentMetadata->friendly_name[75] << pGraphArgumentMetadata->friendly_name[76]
           << pGraphArgumentMetadata->friendly_name[77] << pGraphArgumentMetadata->friendly_name[78]
           << pGraphArgumentMetadata->friendly_name[79] << pGraphArgumentMetadata->friendly_name[80]
           << pGraphArgumentMetadata->friendly_name[81] << pGraphArgumentMetadata->friendly_name[82]
           << pGraphArgumentMetadata->friendly_name[83] << pGraphArgumentMetadata->friendly_name[84]
           << pGraphArgumentMetadata->friendly_name[85] << pGraphArgumentMetadata->friendly_name[86]
           << pGraphArgumentMetadata->friendly_name[87] << pGraphArgumentMetadata->friendly_name[88]
           << pGraphArgumentMetadata->friendly_name[89] << pGraphArgumentMetadata->friendly_name[90]
           << pGraphArgumentMetadata->friendly_name[91] << pGraphArgumentMetadata->friendly_name[92]
           << pGraphArgumentMetadata->friendly_name[93] << pGraphArgumentMetadata->friendly_name[94]
           << pGraphArgumentMetadata->friendly_name[95] << pGraphArgumentMetadata->friendly_name[96]
           << pGraphArgumentMetadata->friendly_name[97] << pGraphArgumentMetadata->friendly_name[98]
           << pGraphArgumentMetadata->friendly_name[99]
           << pGraphArgumentMetadata->friendly_name[100]
           << pGraphArgumentMetadata->friendly_name[101]
           << pGraphArgumentMetadata->friendly_name[102]
           << pGraphArgumentMetadata->friendly_name[103]
           << pGraphArgumentMetadata->friendly_name[104]
           << pGraphArgumentMetadata->friendly_name[105]
           << pGraphArgumentMetadata->friendly_name[106]
           << pGraphArgumentMetadata->friendly_name[107]
           << pGraphArgumentMetadata->friendly_name[108]
           << pGraphArgumentMetadata->friendly_name[109]
           << pGraphArgumentMetadata->friendly_name[110]
           << pGraphArgumentMetadata->friendly_name[111]
           << pGraphArgumentMetadata->friendly_name[112]
           << pGraphArgumentMetadata->friendly_name[113]
           << pGraphArgumentMetadata->friendly_name[114]
           << pGraphArgumentMetadata->friendly_name[115]
           << pGraphArgumentMetadata->friendly_name[116]
           << pGraphArgumentMetadata->friendly_name[117]
           << pGraphArgumentMetadata->friendly_name[118]
           << pGraphArgumentMetadata->friendly_name[119]
           << pGraphArgumentMetadata->friendly_name[120]
           << pGraphArgumentMetadata->friendly_name[121]
           << pGraphArgumentMetadata->friendly_name[122]
           << pGraphArgumentMetadata->friendly_name[123]
           << pGraphArgumentMetadata->friendly_name[124]
           << pGraphArgumentMetadata->friendly_name[125]
           << pGraphArgumentMetadata->friendly_name[126]
           << pGraphArgumentMetadata->friendly_name[127]
           << pGraphArgumentMetadata->friendly_name[128]
           << pGraphArgumentMetadata->friendly_name[129]
           << pGraphArgumentMetadata->friendly_name[130]
           << pGraphArgumentMetadata->friendly_name[131]
           << pGraphArgumentMetadata->friendly_name[132]
           << pGraphArgumentMetadata->friendly_name[133]
           << pGraphArgumentMetadata->friendly_name[134]
           << pGraphArgumentMetadata->friendly_name[135]
           << pGraphArgumentMetadata->friendly_name[136]
           << pGraphArgumentMetadata->friendly_name[137]
           << pGraphArgumentMetadata->friendly_name[138]
           << pGraphArgumentMetadata->friendly_name[139]
           << pGraphArgumentMetadata->friendly_name[140]
           << pGraphArgumentMetadata->friendly_name[141]
           << pGraphArgumentMetadata->friendly_name[142]
           << pGraphArgumentMetadata->friendly_name[143]
           << pGraphArgumentMetadata->friendly_name[144]
           << pGraphArgumentMetadata->friendly_name[145]
           << pGraphArgumentMetadata->friendly_name[146]
           << pGraphArgumentMetadata->friendly_name[147]
           << pGraphArgumentMetadata->friendly_name[148]
           << pGraphArgumentMetadata->friendly_name[149]
           << pGraphArgumentMetadata->friendly_name[150]
           << pGraphArgumentMetadata->friendly_name[151]
           << pGraphArgumentMetadata->friendly_name[152]
           << pGraphArgumentMetadata->friendly_name[153]
           << pGraphArgumentMetadata->friendly_name[154]
           << pGraphArgumentMetadata->friendly_name[155]
           << pGraphArgumentMetadata->friendly_name[156]
           << pGraphArgumentMetadata->friendly_name[157]
           << pGraphArgumentMetadata->friendly_name[158]
           << pGraphArgumentMetadata->friendly_name[159]
           << pGraphArgumentMetadata->friendly_name[160]
           << pGraphArgumentMetadata->friendly_name[161]
           << pGraphArgumentMetadata->friendly_name[162]
           << pGraphArgumentMetadata->friendly_name[163]
           << pGraphArgumentMetadata->friendly_name[164]
           << pGraphArgumentMetadata->friendly_name[165]
           << pGraphArgumentMetadata->friendly_name[166]
           << pGraphArgumentMetadata->friendly_name[167]
           << pGraphArgumentMetadata->friendly_name[168]
           << pGraphArgumentMetadata->friendly_name[169]
           << pGraphArgumentMetadata->friendly_name[170]
           << pGraphArgumentMetadata->friendly_name[171]
           << pGraphArgumentMetadata->friendly_name[172]
           << pGraphArgumentMetadata->friendly_name[173]
           << pGraphArgumentMetadata->friendly_name[174]
           << pGraphArgumentMetadata->friendly_name[175]
           << pGraphArgumentMetadata->friendly_name[176]
           << pGraphArgumentMetadata->friendly_name[177]
           << pGraphArgumentMetadata->friendly_name[178]
           << pGraphArgumentMetadata->friendly_name[179]
           << pGraphArgumentMetadata->friendly_name[180]
           << pGraphArgumentMetadata->friendly_name[181]
           << pGraphArgumentMetadata->friendly_name[182]
           << pGraphArgumentMetadata->friendly_name[183]
           << pGraphArgumentMetadata->friendly_name[184]
           << pGraphArgumentMetadata->friendly_name[185]
           << pGraphArgumentMetadata->friendly_name[186]
           << pGraphArgumentMetadata->friendly_name[187]
           << pGraphArgumentMetadata->friendly_name[188]
           << pGraphArgumentMetadata->friendly_name[189]
           << pGraphArgumentMetadata->friendly_name[190]
           << pGraphArgumentMetadata->friendly_name[191]
           << pGraphArgumentMetadata->friendly_name[192]
           << pGraphArgumentMetadata->friendly_name[193]
           << pGraphArgumentMetadata->friendly_name[194]
           << pGraphArgumentMetadata->friendly_name[195]
           << pGraphArgumentMetadata->friendly_name[196]
           << pGraphArgumentMetadata->friendly_name[197]
           << pGraphArgumentMetadata->friendly_name[198]
           << pGraphArgumentMetadata->friendly_name[199]
           << pGraphArgumentMetadata->friendly_name[200]
           << pGraphArgumentMetadata->friendly_name[201]
           << pGraphArgumentMetadata->friendly_name[202]
           << pGraphArgumentMetadata->friendly_name[203]
           << pGraphArgumentMetadata->friendly_name[204]
           << pGraphArgumentMetadata->friendly_name[205]
           << pGraphArgumentMetadata->friendly_name[206]
           << pGraphArgumentMetadata->friendly_name[207]
           << pGraphArgumentMetadata->friendly_name[208]
           << pGraphArgumentMetadata->friendly_name[209]
           << pGraphArgumentMetadata->friendly_name[210]
           << pGraphArgumentMetadata->friendly_name[211]
           << pGraphArgumentMetadata->friendly_name[212]
           << pGraphArgumentMetadata->friendly_name[213]
           << pGraphArgumentMetadata->friendly_name[214]
           << pGraphArgumentMetadata->friendly_name[215]
           << pGraphArgumentMetadata->friendly_name[216]
           << pGraphArgumentMetadata->friendly_name[217]
           << pGraphArgumentMetadata->friendly_name[218]
           << pGraphArgumentMetadata->friendly_name[219]
           << pGraphArgumentMetadata->friendly_name[220]
           << pGraphArgumentMetadata->friendly_name[221]
           << pGraphArgumentMetadata->friendly_name[222]
           << pGraphArgumentMetadata->friendly_name[223]
           << pGraphArgumentMetadata->friendly_name[224]
           << pGraphArgumentMetadata->friendly_name[225]
           << pGraphArgumentMetadata->friendly_name[226]
           << pGraphArgumentMetadata->friendly_name[227]
           << pGraphArgumentMetadata->friendly_name[228]
           << pGraphArgumentMetadata->friendly_name[229]
           << pGraphArgumentMetadata->friendly_name[230]
           << pGraphArgumentMetadata->friendly_name[231]
           << pGraphArgumentMetadata->friendly_name[232]
           << pGraphArgumentMetadata->friendly_name[233]
           << pGraphArgumentMetadata->friendly_name[234]
           << pGraphArgumentMetadata->friendly_name[235]
           << pGraphArgumentMetadata->friendly_name[236]
           << pGraphArgumentMetadata->friendly_name[237]
           << pGraphArgumentMetadata->friendly_name[238]
           << pGraphArgumentMetadata->friendly_name[239]
           << pGraphArgumentMetadata->friendly_name[240]
           << pGraphArgumentMetadata->friendly_name[241]
           << pGraphArgumentMetadata->friendly_name[242]
           << pGraphArgumentMetadata->friendly_name[243]
           << pGraphArgumentMetadata->friendly_name[244]
           << pGraphArgumentMetadata->friendly_name[245]
           << pGraphArgumentMetadata->friendly_name[246]
           << pGraphArgumentMetadata->friendly_name[247]
           << pGraphArgumentMetadata->friendly_name[248]
           << pGraphArgumentMetadata->friendly_name[249]
           << pGraphArgumentMetadata->friendly_name[250]
           << pGraphArgumentMetadata->friendly_name[251]
           << pGraphArgumentMetadata->friendly_name[252]
           << pGraphArgumentMetadata->friendly_name[253]
           << pGraphArgumentMetadata->friendly_name[254]
           << pGraphArgumentMetadata->friendly_name[255];
        ss << ", data_type: " << pGraphArgumentMetadata->data_type;
        ss << ", shape: " << pGraphArgumentMetadata->shape[0] << pGraphArgumentMetadata->shape[1]
           << pGraphArgumentMetadata->shape[2] << pGraphArgumentMetadata->shape[3]
           << pGraphArgumentMetadata->shape[4] << pGraphArgumentMetadata->shape[5]
           << pGraphArgumentMetadata->shape[6] << pGraphArgumentMetadata->shape[7];
        ss << ", shape_size: " << pGraphArgumentMetadata->shape_size;
        ss << ", tensor_names_count: " << pGraphArgumentMetadata->tensor_names_count;
        ss << ", input_name: " << pGraphArgumentMetadata->input_name[0]
           << pGraphArgumentMetadata->input_name[1] << pGraphArgumentMetadata->input_name[2]
           << pGraphArgumentMetadata->input_name[3] << pGraphArgumentMetadata->input_name[4]
           << pGraphArgumentMetadata->input_name[5] << pGraphArgumentMetadata->input_name[6]
           << pGraphArgumentMetadata->input_name[7] << pGraphArgumentMetadata->input_name[8]
           << pGraphArgumentMetadata->input_name[9] << pGraphArgumentMetadata->input_name[10]
           << pGraphArgumentMetadata->input_name[11] << pGraphArgumentMetadata->input_name[12]
           << pGraphArgumentMetadata->input_name[13] << pGraphArgumentMetadata->input_name[14]
           << pGraphArgumentMetadata->input_name[15] << pGraphArgumentMetadata->input_name[16]
           << pGraphArgumentMetadata->input_name[17] << pGraphArgumentMetadata->input_name[18]
           << pGraphArgumentMetadata->input_name[19] << pGraphArgumentMetadata->input_name[20]
           << pGraphArgumentMetadata->input_name[21] << pGraphArgumentMetadata->input_name[22]
           << pGraphArgumentMetadata->input_name[23] << pGraphArgumentMetadata->input_name[24]
           << pGraphArgumentMetadata->input_name[25] << pGraphArgumentMetadata->input_name[26]
           << pGraphArgumentMetadata->input_name[27] << pGraphArgumentMetadata->input_name[28]
           << pGraphArgumentMetadata->input_name[29] << pGraphArgumentMetadata->input_name[30]
           << pGraphArgumentMetadata->input_name[31] << pGraphArgumentMetadata->input_name[32]
           << pGraphArgumentMetadata->input_name[33] << pGraphArgumentMetadata->input_name[34]
           << pGraphArgumentMetadata->input_name[35] << pGraphArgumentMetadata->input_name[36]
           << pGraphArgumentMetadata->input_name[37] << pGraphArgumentMetadata->input_name[38]
           << pGraphArgumentMetadata->input_name[39] << pGraphArgumentMetadata->input_name[40]
           << pGraphArgumentMetadata->input_name[41] << pGraphArgumentMetadata->input_name[42]
           << pGraphArgumentMetadata->input_name[43] << pGraphArgumentMetadata->input_name[44]
           << pGraphArgumentMetadata->input_name[45] << pGraphArgumentMetadata->input_name[46]
           << pGraphArgumentMetadata->input_name[47] << pGraphArgumentMetadata->input_name[48]
           << pGraphArgumentMetadata->input_name[49] << pGraphArgumentMetadata->input_name[50]
           << pGraphArgumentMetadata->input_name[51] << pGraphArgumentMetadata->input_name[52]
           << pGraphArgumentMetadata->input_name[53] << pGraphArgumentMetadata->input_name[54]
           << pGraphArgumentMetadata->input_name[55] << pGraphArgumentMetadata->input_name[56]
           << pGraphArgumentMetadata->input_name[57] << pGraphArgumentMetadata->input_name[58]
           << pGraphArgumentMetadata->input_name[59] << pGraphArgumentMetadata->input_name[60]
           << pGraphArgumentMetadata->input_name[61] << pGraphArgumentMetadata->input_name[62]
           << pGraphArgumentMetadata->input_name[63] << pGraphArgumentMetadata->input_name[64]
           << pGraphArgumentMetadata->input_name[65] << pGraphArgumentMetadata->input_name[66]
           << pGraphArgumentMetadata->input_name[67] << pGraphArgumentMetadata->input_name[68]
           << pGraphArgumentMetadata->input_name[69] << pGraphArgumentMetadata->input_name[70]
           << pGraphArgumentMetadata->input_name[71] << pGraphArgumentMetadata->input_name[72]
           << pGraphArgumentMetadata->input_name[73] << pGraphArgumentMetadata->input_name[74]
           << pGraphArgumentMetadata->input_name[75] << pGraphArgumentMetadata->input_name[76]
           << pGraphArgumentMetadata->input_name[77] << pGraphArgumentMetadata->input_name[78]
           << pGraphArgumentMetadata->input_name[79] << pGraphArgumentMetadata->input_name[80]
           << pGraphArgumentMetadata->input_name[81] << pGraphArgumentMetadata->input_name[82]
           << pGraphArgumentMetadata->input_name[83] << pGraphArgumentMetadata->input_name[84]
           << pGraphArgumentMetadata->input_name[85] << pGraphArgumentMetadata->input_name[86]
           << pGraphArgumentMetadata->input_name[87] << pGraphArgumentMetadata->input_name[88]
           << pGraphArgumentMetadata->input_name[89] << pGraphArgumentMetadata->input_name[90]
           << pGraphArgumentMetadata->input_name[91] << pGraphArgumentMetadata->input_name[92]
           << pGraphArgumentMetadata->input_name[93] << pGraphArgumentMetadata->input_name[94]
           << pGraphArgumentMetadata->input_name[95] << pGraphArgumentMetadata->input_name[96]
           << pGraphArgumentMetadata->input_name[97] << pGraphArgumentMetadata->input_name[98]
           << pGraphArgumentMetadata->input_name[99] << pGraphArgumentMetadata->input_name[100]
           << pGraphArgumentMetadata->input_name[101] << pGraphArgumentMetadata->input_name[102]
           << pGraphArgumentMetadata->input_name[103] << pGraphArgumentMetadata->input_name[104]
           << pGraphArgumentMetadata->input_name[105] << pGraphArgumentMetadata->input_name[106]
           << pGraphArgumentMetadata->input_name[107] << pGraphArgumentMetadata->input_name[108]
           << pGraphArgumentMetadata->input_name[109] << pGraphArgumentMetadata->input_name[110]
           << pGraphArgumentMetadata->input_name[111] << pGraphArgumentMetadata->input_name[112]
           << pGraphArgumentMetadata->input_name[113] << pGraphArgumentMetadata->input_name[114]
           << pGraphArgumentMetadata->input_name[115] << pGraphArgumentMetadata->input_name[116]
           << pGraphArgumentMetadata->input_name[117] << pGraphArgumentMetadata->input_name[118]
           << pGraphArgumentMetadata->input_name[119] << pGraphArgumentMetadata->input_name[120]
           << pGraphArgumentMetadata->input_name[121] << pGraphArgumentMetadata->input_name[122]
           << pGraphArgumentMetadata->input_name[123] << pGraphArgumentMetadata->input_name[124]
           << pGraphArgumentMetadata->input_name[125] << pGraphArgumentMetadata->input_name[126]
           << pGraphArgumentMetadata->input_name[127] << pGraphArgumentMetadata->input_name[128]
           << pGraphArgumentMetadata->input_name[129] << pGraphArgumentMetadata->input_name[130]
           << pGraphArgumentMetadata->input_name[131] << pGraphArgumentMetadata->input_name[132]
           << pGraphArgumentMetadata->input_name[133] << pGraphArgumentMetadata->input_name[134]
           << pGraphArgumentMetadata->input_name[135] << pGraphArgumentMetadata->input_name[136]
           << pGraphArgumentMetadata->input_name[137] << pGraphArgumentMetadata->input_name[138]
           << pGraphArgumentMetadata->input_name[139] << pGraphArgumentMetadata->input_name[140]
           << pGraphArgumentMetadata->input_name[141] << pGraphArgumentMetadata->input_name[142]
           << pGraphArgumentMetadata->input_name[143] << pGraphArgumentMetadata->input_name[144]
           << pGraphArgumentMetadata->input_name[145] << pGraphArgumentMetadata->input_name[146]
           << pGraphArgumentMetadata->input_name[147] << pGraphArgumentMetadata->input_name[148]
           << pGraphArgumentMetadata->input_name[149] << pGraphArgumentMetadata->input_name[150]
           << pGraphArgumentMetadata->input_name[151] << pGraphArgumentMetadata->input_name[152]
           << pGraphArgumentMetadata->input_name[153] << pGraphArgumentMetadata->input_name[154]
           << pGraphArgumentMetadata->input_name[155] << pGraphArgumentMetadata->input_name[156]
           << pGraphArgumentMetadata->input_name[157] << pGraphArgumentMetadata->input_name[158]
           << pGraphArgumentMetadata->input_name[159] << pGraphArgumentMetadata->input_name[160]
           << pGraphArgumentMetadata->input_name[161] << pGraphArgumentMetadata->input_name[162]
           << pGraphArgumentMetadata->input_name[163] << pGraphArgumentMetadata->input_name[164]
           << pGraphArgumentMetadata->input_name[165] << pGraphArgumentMetadata->input_name[166]
           << pGraphArgumentMetadata->input_name[167] << pGraphArgumentMetadata->input_name[168]
           << pGraphArgumentMetadata->input_name[169] << pGraphArgumentMetadata->input_name[170]
           << pGraphArgumentMetadata->input_name[171] << pGraphArgumentMetadata->input_name[172]
           << pGraphArgumentMetadata->input_name[173] << pGraphArgumentMetadata->input_name[174]
           << pGraphArgumentMetadata->input_name[175] << pGraphArgumentMetadata->input_name[176]
           << pGraphArgumentMetadata->input_name[177] << pGraphArgumentMetadata->input_name[178]
           << pGraphArgumentMetadata->input_name[179] << pGraphArgumentMetadata->input_name[180]
           << pGraphArgumentMetadata->input_name[181] << pGraphArgumentMetadata->input_name[182]
           << pGraphArgumentMetadata->input_name[183] << pGraphArgumentMetadata->input_name[184]
           << pGraphArgumentMetadata->input_name[185] << pGraphArgumentMetadata->input_name[186]
           << pGraphArgumentMetadata->input_name[187] << pGraphArgumentMetadata->input_name[188]
           << pGraphArgumentMetadata->input_name[189] << pGraphArgumentMetadata->input_name[190]
           << pGraphArgumentMetadata->input_name[191] << pGraphArgumentMetadata->input_name[192]
           << pGraphArgumentMetadata->input_name[193] << pGraphArgumentMetadata->input_name[194]
           << pGraphArgumentMetadata->input_name[195] << pGraphArgumentMetadata->input_name[196]
           << pGraphArgumentMetadata->input_name[197] << pGraphArgumentMetadata->input_name[198]
           << pGraphArgumentMetadata->input_name[199] << pGraphArgumentMetadata->input_name[200]
           << pGraphArgumentMetadata->input_name[201] << pGraphArgumentMetadata->input_name[202]
           << pGraphArgumentMetadata->input_name[203] << pGraphArgumentMetadata->input_name[204]
           << pGraphArgumentMetadata->input_name[205] << pGraphArgumentMetadata->input_name[206]
           << pGraphArgumentMetadata->input_name[207] << pGraphArgumentMetadata->input_name[208]
           << pGraphArgumentMetadata->input_name[209] << pGraphArgumentMetadata->input_name[210]
           << pGraphArgumentMetadata->input_name[211] << pGraphArgumentMetadata->input_name[212]
           << pGraphArgumentMetadata->input_name[213] << pGraphArgumentMetadata->input_name[214]
           << pGraphArgumentMetadata->input_name[215] << pGraphArgumentMetadata->input_name[216]
           << pGraphArgumentMetadata->input_name[217] << pGraphArgumentMetadata->input_name[218]
           << pGraphArgumentMetadata->input_name[219] << pGraphArgumentMetadata->input_name[220]
           << pGraphArgumentMetadata->input_name[221] << pGraphArgumentMetadata->input_name[222]
           << pGraphArgumentMetadata->input_name[223] << pGraphArgumentMetadata->input_name[224]
           << pGraphArgumentMetadata->input_name[225] << pGraphArgumentMetadata->input_name[226]
           << pGraphArgumentMetadata->input_name[227] << pGraphArgumentMetadata->input_name[228]
           << pGraphArgumentMetadata->input_name[229] << pGraphArgumentMetadata->input_name[230]
           << pGraphArgumentMetadata->input_name[231] << pGraphArgumentMetadata->input_name[232]
           << pGraphArgumentMetadata->input_name[233] << pGraphArgumentMetadata->input_name[234]
           << pGraphArgumentMetadata->input_name[235] << pGraphArgumentMetadata->input_name[236]
           << pGraphArgumentMetadata->input_name[237] << pGraphArgumentMetadata->input_name[238]
           << pGraphArgumentMetadata->input_name[239] << pGraphArgumentMetadata->input_name[240]
           << pGraphArgumentMetadata->input_name[241] << pGraphArgumentMetadata->input_name[242]
           << pGraphArgumentMetadata->input_name[243] << pGraphArgumentMetadata->input_name[244]
           << pGraphArgumentMetadata->input_name[245] << pGraphArgumentMetadata->input_name[246]
           << pGraphArgumentMetadata->input_name[247] << pGraphArgumentMetadata->input_name[248]
           << pGraphArgumentMetadata->input_name[249] << pGraphArgumentMetadata->input_name[250]
           << pGraphArgumentMetadata->input_name[251] << pGraphArgumentMetadata->input_name[252]
           << pGraphArgumentMetadata->input_name[253] << pGraphArgumentMetadata->input_name[254]
           << pGraphArgumentMetadata->input_name[255];
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
        ss << ", name: " << pGraphArgumentProperties->name[0] << pGraphArgumentProperties->name[1]
           << pGraphArgumentProperties->name[2] << pGraphArgumentProperties->name[3]
           << pGraphArgumentProperties->name[4] << pGraphArgumentProperties->name[5]
           << pGraphArgumentProperties->name[6] << pGraphArgumentProperties->name[7]
           << pGraphArgumentProperties->name[8] << pGraphArgumentProperties->name[9]
           << pGraphArgumentProperties->name[10] << pGraphArgumentProperties->name[11]
           << pGraphArgumentProperties->name[12] << pGraphArgumentProperties->name[13]
           << pGraphArgumentProperties->name[14] << pGraphArgumentProperties->name[15]
           << pGraphArgumentProperties->name[16] << pGraphArgumentProperties->name[17]
           << pGraphArgumentProperties->name[18] << pGraphArgumentProperties->name[19]
           << pGraphArgumentProperties->name[20] << pGraphArgumentProperties->name[21]
           << pGraphArgumentProperties->name[22] << pGraphArgumentProperties->name[23]
           << pGraphArgumentProperties->name[24] << pGraphArgumentProperties->name[25]
           << pGraphArgumentProperties->name[26] << pGraphArgumentProperties->name[27]
           << pGraphArgumentProperties->name[28] << pGraphArgumentProperties->name[29]
           << pGraphArgumentProperties->name[30] << pGraphArgumentProperties->name[31]
           << pGraphArgumentProperties->name[32] << pGraphArgumentProperties->name[33]
           << pGraphArgumentProperties->name[34] << pGraphArgumentProperties->name[35]
           << pGraphArgumentProperties->name[36] << pGraphArgumentProperties->name[37]
           << pGraphArgumentProperties->name[38] << pGraphArgumentProperties->name[39]
           << pGraphArgumentProperties->name[40] << pGraphArgumentProperties->name[41]
           << pGraphArgumentProperties->name[42] << pGraphArgumentProperties->name[43]
           << pGraphArgumentProperties->name[44] << pGraphArgumentProperties->name[45]
           << pGraphArgumentProperties->name[46] << pGraphArgumentProperties->name[47]
           << pGraphArgumentProperties->name[48] << pGraphArgumentProperties->name[49]
           << pGraphArgumentProperties->name[50] << pGraphArgumentProperties->name[51]
           << pGraphArgumentProperties->name[52] << pGraphArgumentProperties->name[53]
           << pGraphArgumentProperties->name[54] << pGraphArgumentProperties->name[55]
           << pGraphArgumentProperties->name[56] << pGraphArgumentProperties->name[57]
           << pGraphArgumentProperties->name[58] << pGraphArgumentProperties->name[59]
           << pGraphArgumentProperties->name[60] << pGraphArgumentProperties->name[61]
           << pGraphArgumentProperties->name[62] << pGraphArgumentProperties->name[63]
           << pGraphArgumentProperties->name[64] << pGraphArgumentProperties->name[65]
           << pGraphArgumentProperties->name[66] << pGraphArgumentProperties->name[67]
           << pGraphArgumentProperties->name[68] << pGraphArgumentProperties->name[69]
           << pGraphArgumentProperties->name[70] << pGraphArgumentProperties->name[71]
           << pGraphArgumentProperties->name[72] << pGraphArgumentProperties->name[73]
           << pGraphArgumentProperties->name[74] << pGraphArgumentProperties->name[75]
           << pGraphArgumentProperties->name[76] << pGraphArgumentProperties->name[77]
           << pGraphArgumentProperties->name[78] << pGraphArgumentProperties->name[79]
           << pGraphArgumentProperties->name[80] << pGraphArgumentProperties->name[81]
           << pGraphArgumentProperties->name[82] << pGraphArgumentProperties->name[83]
           << pGraphArgumentProperties->name[84] << pGraphArgumentProperties->name[85]
           << pGraphArgumentProperties->name[86] << pGraphArgumentProperties->name[87]
           << pGraphArgumentProperties->name[88] << pGraphArgumentProperties->name[89]
           << pGraphArgumentProperties->name[90] << pGraphArgumentProperties->name[91]
           << pGraphArgumentProperties->name[92] << pGraphArgumentProperties->name[93]
           << pGraphArgumentProperties->name[94] << pGraphArgumentProperties->name[95]
           << pGraphArgumentProperties->name[96] << pGraphArgumentProperties->name[97]
           << pGraphArgumentProperties->name[98] << pGraphArgumentProperties->name[99]
           << pGraphArgumentProperties->name[100] << pGraphArgumentProperties->name[101]
           << pGraphArgumentProperties->name[102] << pGraphArgumentProperties->name[103]
           << pGraphArgumentProperties->name[104] << pGraphArgumentProperties->name[105]
           << pGraphArgumentProperties->name[106] << pGraphArgumentProperties->name[107]
           << pGraphArgumentProperties->name[108] << pGraphArgumentProperties->name[109]
           << pGraphArgumentProperties->name[110] << pGraphArgumentProperties->name[111]
           << pGraphArgumentProperties->name[112] << pGraphArgumentProperties->name[113]
           << pGraphArgumentProperties->name[114] << pGraphArgumentProperties->name[115]
           << pGraphArgumentProperties->name[116] << pGraphArgumentProperties->name[117]
           << pGraphArgumentProperties->name[118] << pGraphArgumentProperties->name[119]
           << pGraphArgumentProperties->name[120] << pGraphArgumentProperties->name[121]
           << pGraphArgumentProperties->name[122] << pGraphArgumentProperties->name[123]
           << pGraphArgumentProperties->name[124] << pGraphArgumentProperties->name[125]
           << pGraphArgumentProperties->name[126] << pGraphArgumentProperties->name[127]
           << pGraphArgumentProperties->name[128] << pGraphArgumentProperties->name[129]
           << pGraphArgumentProperties->name[130] << pGraphArgumentProperties->name[131]
           << pGraphArgumentProperties->name[132] << pGraphArgumentProperties->name[133]
           << pGraphArgumentProperties->name[134] << pGraphArgumentProperties->name[135]
           << pGraphArgumentProperties->name[136] << pGraphArgumentProperties->name[137]
           << pGraphArgumentProperties->name[138] << pGraphArgumentProperties->name[139]
           << pGraphArgumentProperties->name[140] << pGraphArgumentProperties->name[141]
           << pGraphArgumentProperties->name[142] << pGraphArgumentProperties->name[143]
           << pGraphArgumentProperties->name[144] << pGraphArgumentProperties->name[145]
           << pGraphArgumentProperties->name[146] << pGraphArgumentProperties->name[147]
           << pGraphArgumentProperties->name[148] << pGraphArgumentProperties->name[149]
           << pGraphArgumentProperties->name[150] << pGraphArgumentProperties->name[151]
           << pGraphArgumentProperties->name[152] << pGraphArgumentProperties->name[153]
           << pGraphArgumentProperties->name[154] << pGraphArgumentProperties->name[155]
           << pGraphArgumentProperties->name[156] << pGraphArgumentProperties->name[157]
           << pGraphArgumentProperties->name[158] << pGraphArgumentProperties->name[159]
           << pGraphArgumentProperties->name[160] << pGraphArgumentProperties->name[161]
           << pGraphArgumentProperties->name[162] << pGraphArgumentProperties->name[163]
           << pGraphArgumentProperties->name[164] << pGraphArgumentProperties->name[165]
           << pGraphArgumentProperties->name[166] << pGraphArgumentProperties->name[167]
           << pGraphArgumentProperties->name[168] << pGraphArgumentProperties->name[169]
           << pGraphArgumentProperties->name[170] << pGraphArgumentProperties->name[171]
           << pGraphArgumentProperties->name[172] << pGraphArgumentProperties->name[173]
           << pGraphArgumentProperties->name[174] << pGraphArgumentProperties->name[175]
           << pGraphArgumentProperties->name[176] << pGraphArgumentProperties->name[177]
           << pGraphArgumentProperties->name[178] << pGraphArgumentProperties->name[179]
           << pGraphArgumentProperties->name[180] << pGraphArgumentProperties->name[181]
           << pGraphArgumentProperties->name[182] << pGraphArgumentProperties->name[183]
           << pGraphArgumentProperties->name[184] << pGraphArgumentProperties->name[185]
           << pGraphArgumentProperties->name[186] << pGraphArgumentProperties->name[187]
           << pGraphArgumentProperties->name[188] << pGraphArgumentProperties->name[189]
           << pGraphArgumentProperties->name[190] << pGraphArgumentProperties->name[191]
           << pGraphArgumentProperties->name[192] << pGraphArgumentProperties->name[193]
           << pGraphArgumentProperties->name[194] << pGraphArgumentProperties->name[195]
           << pGraphArgumentProperties->name[196] << pGraphArgumentProperties->name[197]
           << pGraphArgumentProperties->name[198] << pGraphArgumentProperties->name[199]
           << pGraphArgumentProperties->name[200] << pGraphArgumentProperties->name[201]
           << pGraphArgumentProperties->name[202] << pGraphArgumentProperties->name[203]
           << pGraphArgumentProperties->name[204] << pGraphArgumentProperties->name[205]
           << pGraphArgumentProperties->name[206] << pGraphArgumentProperties->name[207]
           << pGraphArgumentProperties->name[208] << pGraphArgumentProperties->name[209]
           << pGraphArgumentProperties->name[210] << pGraphArgumentProperties->name[211]
           << pGraphArgumentProperties->name[212] << pGraphArgumentProperties->name[213]
           << pGraphArgumentProperties->name[214] << pGraphArgumentProperties->name[215]
           << pGraphArgumentProperties->name[216] << pGraphArgumentProperties->name[217]
           << pGraphArgumentProperties->name[218] << pGraphArgumentProperties->name[219]
           << pGraphArgumentProperties->name[220] << pGraphArgumentProperties->name[221]
           << pGraphArgumentProperties->name[222] << pGraphArgumentProperties->name[223]
           << pGraphArgumentProperties->name[224] << pGraphArgumentProperties->name[225]
           << pGraphArgumentProperties->name[226] << pGraphArgumentProperties->name[227]
           << pGraphArgumentProperties->name[228] << pGraphArgumentProperties->name[229]
           << pGraphArgumentProperties->name[230] << pGraphArgumentProperties->name[231]
           << pGraphArgumentProperties->name[232] << pGraphArgumentProperties->name[233]
           << pGraphArgumentProperties->name[234] << pGraphArgumentProperties->name[235]
           << pGraphArgumentProperties->name[236] << pGraphArgumentProperties->name[237]
           << pGraphArgumentProperties->name[238] << pGraphArgumentProperties->name[239]
           << pGraphArgumentProperties->name[240] << pGraphArgumentProperties->name[241]
           << pGraphArgumentProperties->name[242] << pGraphArgumentProperties->name[243]
           << pGraphArgumentProperties->name[244] << pGraphArgumentProperties->name[245]
           << pGraphArgumentProperties->name[246] << pGraphArgumentProperties->name[247]
           << pGraphArgumentProperties->name[248] << pGraphArgumentProperties->name[249]
           << pGraphArgumentProperties->name[250] << pGraphArgumentProperties->name[251]
           << pGraphArgumentProperties->name[252] << pGraphArgumentProperties->name[253]
           << pGraphArgumentProperties->name[254] << pGraphArgumentProperties->name[255];
        ss << ", type: " << pGraphArgumentProperties->type;
        ss << ", dims: " << pGraphArgumentProperties->dims[0] << " "
           << pGraphArgumentProperties->dims[1] << " " << pGraphArgumentProperties->dims[2] << " "
           << pGraphArgumentProperties->dims[3] << " " << pGraphArgumentProperties->dims[4];
        ss << ", networkPrecision: " << pGraphArgumentProperties->networkPrecision;
        ss << ", networkLayout: " << pGraphArgumentProperties->networkLayout;
        ss << ", devicePrecision: " << pGraphArgumentProperties->devicePrecision;
        ss << ", deviceLayout: " << pGraphArgumentProperties->deviceLayout;
        ss << ", quantReverseScale: " << pGraphArgumentProperties->quantReverseScale;
        ss << ", quantZeroPoint: " << pGraphArgumentProperties->quantZeroPoint;
        ss << ", dims_count: " << pGraphArgumentProperties->dims_count;
        ss << ", debug_friendly_name: " << pGraphArgumentProperties->debug_friendly_name[0]
           << pGraphArgumentProperties->debug_friendly_name[1]
           << pGraphArgumentProperties->debug_friendly_name[2]
           << pGraphArgumentProperties->debug_friendly_name[3]
           << pGraphArgumentProperties->debug_friendly_name[4]
           << pGraphArgumentProperties->debug_friendly_name[5]
           << pGraphArgumentProperties->debug_friendly_name[6]
           << pGraphArgumentProperties->debug_friendly_name[7]
           << pGraphArgumentProperties->debug_friendly_name[8]
           << pGraphArgumentProperties->debug_friendly_name[9]
           << pGraphArgumentProperties->debug_friendly_name[10]
           << pGraphArgumentProperties->debug_friendly_name[11]
           << pGraphArgumentProperties->debug_friendly_name[12]
           << pGraphArgumentProperties->debug_friendly_name[13]
           << pGraphArgumentProperties->debug_friendly_name[14]
           << pGraphArgumentProperties->debug_friendly_name[15]
           << pGraphArgumentProperties->debug_friendly_name[16]
           << pGraphArgumentProperties->debug_friendly_name[17]
           << pGraphArgumentProperties->debug_friendly_name[18]
           << pGraphArgumentProperties->debug_friendly_name[19]
           << pGraphArgumentProperties->debug_friendly_name[20]
           << pGraphArgumentProperties->debug_friendly_name[21]
           << pGraphArgumentProperties->debug_friendly_name[22]
           << pGraphArgumentProperties->debug_friendly_name[23]
           << pGraphArgumentProperties->debug_friendly_name[24]
           << pGraphArgumentProperties->debug_friendly_name[25]
           << pGraphArgumentProperties->debug_friendly_name[26]
           << pGraphArgumentProperties->debug_friendly_name[27]
           << pGraphArgumentProperties->debug_friendly_name[28]
           << pGraphArgumentProperties->debug_friendly_name[29]
           << pGraphArgumentProperties->debug_friendly_name[30]
           << pGraphArgumentProperties->debug_friendly_name[31]
           << pGraphArgumentProperties->debug_friendly_name[32]
           << pGraphArgumentProperties->debug_friendly_name[33]
           << pGraphArgumentProperties->debug_friendly_name[34]
           << pGraphArgumentProperties->debug_friendly_name[35]
           << pGraphArgumentProperties->debug_friendly_name[36]
           << pGraphArgumentProperties->debug_friendly_name[37]
           << pGraphArgumentProperties->debug_friendly_name[38]
           << pGraphArgumentProperties->debug_friendly_name[39]
           << pGraphArgumentProperties->debug_friendly_name[40]
           << pGraphArgumentProperties->debug_friendly_name[41]
           << pGraphArgumentProperties->debug_friendly_name[42]
           << pGraphArgumentProperties->debug_friendly_name[43]
           << pGraphArgumentProperties->debug_friendly_name[44]
           << pGraphArgumentProperties->debug_friendly_name[45]
           << pGraphArgumentProperties->debug_friendly_name[46]
           << pGraphArgumentProperties->debug_friendly_name[47]
           << pGraphArgumentProperties->debug_friendly_name[48]
           << pGraphArgumentProperties->debug_friendly_name[49]
           << pGraphArgumentProperties->debug_friendly_name[50]
           << pGraphArgumentProperties->debug_friendly_name[51]
           << pGraphArgumentProperties->debug_friendly_name[52]
           << pGraphArgumentProperties->debug_friendly_name[53]
           << pGraphArgumentProperties->debug_friendly_name[54]
           << pGraphArgumentProperties->debug_friendly_name[55]
           << pGraphArgumentProperties->debug_friendly_name[56]
           << pGraphArgumentProperties->debug_friendly_name[57]
           << pGraphArgumentProperties->debug_friendly_name[58]
           << pGraphArgumentProperties->debug_friendly_name[59]
           << pGraphArgumentProperties->debug_friendly_name[60]
           << pGraphArgumentProperties->debug_friendly_name[61]
           << pGraphArgumentProperties->debug_friendly_name[62]
           << pGraphArgumentProperties->debug_friendly_name[63]
           << pGraphArgumentProperties->debug_friendly_name[64]
           << pGraphArgumentProperties->debug_friendly_name[65]
           << pGraphArgumentProperties->debug_friendly_name[66]
           << pGraphArgumentProperties->debug_friendly_name[67]
           << pGraphArgumentProperties->debug_friendly_name[68]
           << pGraphArgumentProperties->debug_friendly_name[69]
           << pGraphArgumentProperties->debug_friendly_name[70]
           << pGraphArgumentProperties->debug_friendly_name[71]
           << pGraphArgumentProperties->debug_friendly_name[72]
           << pGraphArgumentProperties->debug_friendly_name[73]
           << pGraphArgumentProperties->debug_friendly_name[74]
           << pGraphArgumentProperties->debug_friendly_name[75]
           << pGraphArgumentProperties->debug_friendly_name[76]
           << pGraphArgumentProperties->debug_friendly_name[77]
           << pGraphArgumentProperties->debug_friendly_name[78]
           << pGraphArgumentProperties->debug_friendly_name[79]
           << pGraphArgumentProperties->debug_friendly_name[80]
           << pGraphArgumentProperties->debug_friendly_name[81]
           << pGraphArgumentProperties->debug_friendly_name[82]
           << pGraphArgumentProperties->debug_friendly_name[83]
           << pGraphArgumentProperties->debug_friendly_name[84]
           << pGraphArgumentProperties->debug_friendly_name[85]
           << pGraphArgumentProperties->debug_friendly_name[86]
           << pGraphArgumentProperties->debug_friendly_name[87]
           << pGraphArgumentProperties->debug_friendly_name[88]
           << pGraphArgumentProperties->debug_friendly_name[89]
           << pGraphArgumentProperties->debug_friendly_name[90]
           << pGraphArgumentProperties->debug_friendly_name[91]
           << pGraphArgumentProperties->debug_friendly_name[92]
           << pGraphArgumentProperties->debug_friendly_name[93]
           << pGraphArgumentProperties->debug_friendly_name[94]
           << pGraphArgumentProperties->debug_friendly_name[95]
           << pGraphArgumentProperties->debug_friendly_name[96]
           << pGraphArgumentProperties->debug_friendly_name[97]
           << pGraphArgumentProperties->debug_friendly_name[98]
           << pGraphArgumentProperties->debug_friendly_name[99]
           << pGraphArgumentProperties->debug_friendly_name[100]
           << pGraphArgumentProperties->debug_friendly_name[101]
           << pGraphArgumentProperties->debug_friendly_name[102]
           << pGraphArgumentProperties->debug_friendly_name[103]
           << pGraphArgumentProperties->debug_friendly_name[104]
           << pGraphArgumentProperties->debug_friendly_name[105]
           << pGraphArgumentProperties->debug_friendly_name[106]
           << pGraphArgumentProperties->debug_friendly_name[107]
           << pGraphArgumentProperties->debug_friendly_name[108]
           << pGraphArgumentProperties->debug_friendly_name[109]
           << pGraphArgumentProperties->debug_friendly_name[110]
           << pGraphArgumentProperties->debug_friendly_name[111]
           << pGraphArgumentProperties->debug_friendly_name[112]
           << pGraphArgumentProperties->debug_friendly_name[113]
           << pGraphArgumentProperties->debug_friendly_name[114]
           << pGraphArgumentProperties->debug_friendly_name[115]
           << pGraphArgumentProperties->debug_friendly_name[116]
           << pGraphArgumentProperties->debug_friendly_name[117]
           << pGraphArgumentProperties->debug_friendly_name[118]
           << pGraphArgumentProperties->debug_friendly_name[119]
           << pGraphArgumentProperties->debug_friendly_name[120]
           << pGraphArgumentProperties->debug_friendly_name[121]
           << pGraphArgumentProperties->debug_friendly_name[122]
           << pGraphArgumentProperties->debug_friendly_name[123]
           << pGraphArgumentProperties->debug_friendly_name[124]
           << pGraphArgumentProperties->debug_friendly_name[125]
           << pGraphArgumentProperties->debug_friendly_name[126]
           << pGraphArgumentProperties->debug_friendly_name[127]
           << pGraphArgumentProperties->debug_friendly_name[128]
           << pGraphArgumentProperties->debug_friendly_name[129]
           << pGraphArgumentProperties->debug_friendly_name[130]
           << pGraphArgumentProperties->debug_friendly_name[131]
           << pGraphArgumentProperties->debug_friendly_name[132]
           << pGraphArgumentProperties->debug_friendly_name[133]
           << pGraphArgumentProperties->debug_friendly_name[134]
           << pGraphArgumentProperties->debug_friendly_name[135]
           << pGraphArgumentProperties->debug_friendly_name[136]
           << pGraphArgumentProperties->debug_friendly_name[137]
           << pGraphArgumentProperties->debug_friendly_name[138]
           << pGraphArgumentProperties->debug_friendly_name[139]
           << pGraphArgumentProperties->debug_friendly_name[140]
           << pGraphArgumentProperties->debug_friendly_name[141]
           << pGraphArgumentProperties->debug_friendly_name[142]
           << pGraphArgumentProperties->debug_friendly_name[143]
           << pGraphArgumentProperties->debug_friendly_name[144]
           << pGraphArgumentProperties->debug_friendly_name[145]
           << pGraphArgumentProperties->debug_friendly_name[146]
           << pGraphArgumentProperties->debug_friendly_name[147]
           << pGraphArgumentProperties->debug_friendly_name[148]
           << pGraphArgumentProperties->debug_friendly_name[149]
           << pGraphArgumentProperties->debug_friendly_name[150]
           << pGraphArgumentProperties->debug_friendly_name[151]
           << pGraphArgumentProperties->debug_friendly_name[152]
           << pGraphArgumentProperties->debug_friendly_name[153]
           << pGraphArgumentProperties->debug_friendly_name[154]
           << pGraphArgumentProperties->debug_friendly_name[155]
           << pGraphArgumentProperties->debug_friendly_name[156]
           << pGraphArgumentProperties->debug_friendly_name[157]
           << pGraphArgumentProperties->debug_friendly_name[158]
           << pGraphArgumentProperties->debug_friendly_name[159]
           << pGraphArgumentProperties->debug_friendly_name[160]
           << pGraphArgumentProperties->debug_friendly_name[161]
           << pGraphArgumentProperties->debug_friendly_name[162]
           << pGraphArgumentProperties->debug_friendly_name[163]
           << pGraphArgumentProperties->debug_friendly_name[164]
           << pGraphArgumentProperties->debug_friendly_name[165]
           << pGraphArgumentProperties->debug_friendly_name[166]
           << pGraphArgumentProperties->debug_friendly_name[167]
           << pGraphArgumentProperties->debug_friendly_name[168]
           << pGraphArgumentProperties->debug_friendly_name[169]
           << pGraphArgumentProperties->debug_friendly_name[170]
           << pGraphArgumentProperties->debug_friendly_name[171]
           << pGraphArgumentProperties->debug_friendly_name[172]
           << pGraphArgumentProperties->debug_friendly_name[173]
           << pGraphArgumentProperties->debug_friendly_name[174]
           << pGraphArgumentProperties->debug_friendly_name[175]
           << pGraphArgumentProperties->debug_friendly_name[176]
           << pGraphArgumentProperties->debug_friendly_name[177]
           << pGraphArgumentProperties->debug_friendly_name[178]
           << pGraphArgumentProperties->debug_friendly_name[179]
           << pGraphArgumentProperties->debug_friendly_name[180]
           << pGraphArgumentProperties->debug_friendly_name[181]
           << pGraphArgumentProperties->debug_friendly_name[182]
           << pGraphArgumentProperties->debug_friendly_name[183]
           << pGraphArgumentProperties->debug_friendly_name[184]
           << pGraphArgumentProperties->debug_friendly_name[185]
           << pGraphArgumentProperties->debug_friendly_name[186]
           << pGraphArgumentProperties->debug_friendly_name[187]
           << pGraphArgumentProperties->debug_friendly_name[188]
           << pGraphArgumentProperties->debug_friendly_name[189]
           << pGraphArgumentProperties->debug_friendly_name[190]
           << pGraphArgumentProperties->debug_friendly_name[191]
           << pGraphArgumentProperties->debug_friendly_name[192]
           << pGraphArgumentProperties->debug_friendly_name[193]
           << pGraphArgumentProperties->debug_friendly_name[194]
           << pGraphArgumentProperties->debug_friendly_name[195]
           << pGraphArgumentProperties->debug_friendly_name[196]
           << pGraphArgumentProperties->debug_friendly_name[197]
           << pGraphArgumentProperties->debug_friendly_name[198]
           << pGraphArgumentProperties->debug_friendly_name[199]
           << pGraphArgumentProperties->debug_friendly_name[200]
           << pGraphArgumentProperties->debug_friendly_name[201]
           << pGraphArgumentProperties->debug_friendly_name[202]
           << pGraphArgumentProperties->debug_friendly_name[203]
           << pGraphArgumentProperties->debug_friendly_name[204]
           << pGraphArgumentProperties->debug_friendly_name[205]
           << pGraphArgumentProperties->debug_friendly_name[206]
           << pGraphArgumentProperties->debug_friendly_name[207]
           << pGraphArgumentProperties->debug_friendly_name[208]
           << pGraphArgumentProperties->debug_friendly_name[209]
           << pGraphArgumentProperties->debug_friendly_name[210]
           << pGraphArgumentProperties->debug_friendly_name[211]
           << pGraphArgumentProperties->debug_friendly_name[212]
           << pGraphArgumentProperties->debug_friendly_name[213]
           << pGraphArgumentProperties->debug_friendly_name[214]
           << pGraphArgumentProperties->debug_friendly_name[215]
           << pGraphArgumentProperties->debug_friendly_name[216]
           << pGraphArgumentProperties->debug_friendly_name[217]
           << pGraphArgumentProperties->debug_friendly_name[218]
           << pGraphArgumentProperties->debug_friendly_name[219]
           << pGraphArgumentProperties->debug_friendly_name[220]
           << pGraphArgumentProperties->debug_friendly_name[221]
           << pGraphArgumentProperties->debug_friendly_name[222]
           << pGraphArgumentProperties->debug_friendly_name[223]
           << pGraphArgumentProperties->debug_friendly_name[224]
           << pGraphArgumentProperties->debug_friendly_name[225]
           << pGraphArgumentProperties->debug_friendly_name[226]
           << pGraphArgumentProperties->debug_friendly_name[227]
           << pGraphArgumentProperties->debug_friendly_name[228]
           << pGraphArgumentProperties->debug_friendly_name[229]
           << pGraphArgumentProperties->debug_friendly_name[230]
           << pGraphArgumentProperties->debug_friendly_name[231]
           << pGraphArgumentProperties->debug_friendly_name[232]
           << pGraphArgumentProperties->debug_friendly_name[233]
           << pGraphArgumentProperties->debug_friendly_name[234]
           << pGraphArgumentProperties->debug_friendly_name[235]
           << pGraphArgumentProperties->debug_friendly_name[236]
           << pGraphArgumentProperties->debug_friendly_name[237]
           << pGraphArgumentProperties->debug_friendly_name[238]
           << pGraphArgumentProperties->debug_friendly_name[239]
           << pGraphArgumentProperties->debug_friendly_name[240]
           << pGraphArgumentProperties->debug_friendly_name[241]
           << pGraphArgumentProperties->debug_friendly_name[242]
           << pGraphArgumentProperties->debug_friendly_name[243]
           << pGraphArgumentProperties->debug_friendly_name[244]
           << pGraphArgumentProperties->debug_friendly_name[245]
           << pGraphArgumentProperties->debug_friendly_name[246]
           << pGraphArgumentProperties->debug_friendly_name[247]
           << pGraphArgumentProperties->debug_friendly_name[248]
           << pGraphArgumentProperties->debug_friendly_name[249]
           << pGraphArgumentProperties->debug_friendly_name[250]
           << pGraphArgumentProperties->debug_friendly_name[251]
           << pGraphArgumentProperties->debug_friendly_name[252]
           << pGraphArgumentProperties->debug_friendly_name[253]
           << pGraphArgumentProperties->debug_friendly_name[254]
           << pGraphArgumentProperties->debug_friendly_name[255];
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
            ss << ", pInput: " << *desc->pInput;
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
            ss << ", pInput: " << *desc->pInput;
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
            ss << ", pInput: " << *desc->pInput;
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
