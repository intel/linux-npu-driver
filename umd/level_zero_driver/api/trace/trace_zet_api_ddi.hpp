/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Generated header based on zet_ddi.h@version v1.11-r1.11.4

#pragma once

#include "trace_ze_api.hpp"

#include <iostream>
#include <level_zero/zet_ddi.h>
#include <sstream>

inline std::string
_trace_zetGetMetricProgrammableExpProcAddrTable(ze_api_version_t version,
                                                zet_metric_programmable_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetMetricProgrammableExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetExp);
        ss << ", pfnGetPropertiesExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetPropertiesExp);
        ss << ", pfnGetParamInfoExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetParamInfoExp);
        ss << ", pfnGetParamValueInfoExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetParamValueInfoExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zetGetMetricProgrammableExpProcAddrTable(ze_api_version_t version,
                                               zet_metric_programmable_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetMetricProgrammableExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricProgrammableExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void
trace_zetGetMetricProgrammableExpProcAddrTable(ze_result_t ret,
                                               ze_api_version_t version,
                                               zet_metric_programmable_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricProgrammableExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetGetMetricTracerExpProcAddrTable(ze_api_version_t version,
                                          zet_metric_tracer_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetMetricTracerExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreateExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreateExp);
        ss << ", pfnDestroyExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroyExp);
        ss << ", pfnEnableExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnEnableExp);
        ss << ", pfnDisableExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDisableExp);
        ss << ", pfnReadDataExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnReadDataExp);
        ss << ", pfnDecodeExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDecodeExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetMetricTracerExpProcAddrTable(ze_api_version_t version,
                                                     zet_metric_tracer_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetMetricTracerExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricTracerExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetMetricTracerExpProcAddrTable(ze_result_t ret,
                                                     ze_api_version_t version,
                                                     zet_metric_tracer_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricTracerExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetGetMetricDecoderExpProcAddrTable(ze_api_version_t version,
                                           zet_metric_decoder_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetMetricDecoderExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreateExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreateExp);
        ss << ", pfnDestroyExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroyExp);
        ss << ", pfnGetDecodableMetricsExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetDecodableMetricsExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zetGetMetricDecoderExpProcAddrTable(ze_api_version_t version,
                                          zet_metric_decoder_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetMetricDecoderExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricDecoderExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void
trace_zetGetMetricDecoderExpProcAddrTable(ze_result_t ret,
                                          ze_api_version_t version,
                                          zet_metric_decoder_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricDecoderExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetGetDeviceProcAddrTable(ze_api_version_t version,
                                                    zet_device_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetDeviceProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetDebugProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetDebugProperties);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetDeviceProcAddrTable(ze_api_version_t version,
                                            zet_device_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetDeviceProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetDeviceProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetDeviceProcAddrTable(ze_result_t ret,
                                            ze_api_version_t version,
                                            zet_device_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetDeviceProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zetGetDeviceExpProcAddrTable(ze_api_version_t version,
                                                       zet_device_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetDeviceExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetConcurrentMetricGroupsExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetConcurrentMetricGroupsExp);
        ss << ", pfnCreateMetricGroupsFromMetricsExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreateMetricGroupsFromMetricsExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetDeviceExpProcAddrTable(ze_api_version_t version,
                                               zet_device_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetDeviceExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetDeviceExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetDeviceExpProcAddrTable(ze_result_t ret,
                                               ze_api_version_t version,
                                               zet_device_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetDeviceExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetGetContextProcAddrTable(ze_api_version_t version,
                                                     zet_context_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetContextProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnActivateMetricGroups: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnActivateMetricGroups);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetContextProcAddrTable(ze_api_version_t version,
                                             zet_context_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetContextProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetContextProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetContextProcAddrTable(ze_result_t ret,
                                             ze_api_version_t version,
                                             zet_context_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetContextProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string
_trace_zetGetCommandListExpProcAddrTable(ze_api_version_t version,
                                         zet_command_list_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetCommandListExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnAppendMarkerExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendMarkerExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetCommandListExpProcAddrTable(ze_api_version_t version,
                                                    zet_command_list_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetCommandListExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetCommandListExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetCommandListExpProcAddrTable(ze_result_t ret,
                                                    ze_api_version_t version,
                                                    zet_command_list_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetCommandListExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetGetCommandListProcAddrTable(ze_api_version_t version,
                                                         zet_command_list_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetCommandListProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnAppendMetricStreamerMarker: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendMetricStreamerMarker);
        ss << ", pfnAppendMetricQueryBegin: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendMetricQueryBegin);
        ss << ", pfnAppendMetricQueryEnd: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendMetricQueryEnd);
        ss << ", pfnAppendMetricMemoryBarrier: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendMetricMemoryBarrier);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetCommandListProcAddrTable(ze_api_version_t version,
                                                 zet_command_list_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetCommandListProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetCommandListProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetCommandListProcAddrTable(ze_result_t ret,
                                                 ze_api_version_t version,
                                                 zet_command_list_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetCommandListProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetGetModuleProcAddrTable(ze_api_version_t version,
                                                    zet_module_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetModuleProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetDebugInfo: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetDebugInfo);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetModuleProcAddrTable(ze_api_version_t version,
                                            zet_module_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetModuleProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetModuleProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetModuleProcAddrTable(ze_result_t ret,
                                            ze_api_version_t version,
                                            zet_module_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetModuleProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zetGetKernelProcAddrTable(ze_api_version_t version,
                                                    zet_kernel_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetKernelProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProfileInfo: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProfileInfo);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetKernelProcAddrTable(ze_api_version_t version,
                                            zet_kernel_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetKernelProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetKernelProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetKernelProcAddrTable(ze_result_t ret,
                                            ze_api_version_t version,
                                            zet_kernel_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetKernelProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zetGetMetricProcAddrTable(ze_api_version_t version,
                                                    zet_metric_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetMetricProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGet: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGet);
        ss << ", pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetMetricProcAddrTable(ze_api_version_t version,
                                            zet_metric_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetMetricProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetMetricProcAddrTable(ze_result_t ret,
                                            ze_api_version_t version,
                                            zet_metric_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zetGetMetricExpProcAddrTable(ze_api_version_t version,
                                                       zet_metric_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetMetricExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreateFromProgrammableExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreateFromProgrammableExp);
        ss << ", pfnDestroyExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroyExp);
        ss << ", pfnCreateFromProgrammableExp2: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreateFromProgrammableExp2);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetMetricExpProcAddrTable(ze_api_version_t version,
                                               zet_metric_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetMetricExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetMetricExpProcAddrTable(ze_result_t ret,
                                               ze_api_version_t version,
                                               zet_metric_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetGetMetricGroupProcAddrTable(ze_api_version_t version,
                                                         zet_metric_group_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetMetricGroupProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGet: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGet);
        ss << ", pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnCalculateMetricValues: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnCalculateMetricValues);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetMetricGroupProcAddrTable(ze_api_version_t version,
                                                 zet_metric_group_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetMetricGroupProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricGroupProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetMetricGroupProcAddrTable(ze_result_t ret,
                                                 ze_api_version_t version,
                                                 zet_metric_group_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricGroupProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetGetMetricGroupExpProcAddrTable(ze_api_version_t version,
                                         zet_metric_group_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetMetricGroupExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCalculateMultipleMetricValuesExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnCalculateMultipleMetricValuesExp);
        ss << ", pfnGetGlobalTimestampsExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetGlobalTimestampsExp);
        ss << ", pfnGetExportDataExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetExportDataExp);
        ss << ", pfnCalculateMetricExportDataExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnCalculateMetricExportDataExp);
        ss << ", pfnCreateExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreateExp);
        ss << ", pfnAddMetricExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnAddMetricExp);
        ss << ", pfnRemoveMetricExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnRemoveMetricExp);
        ss << ", pfnCloseExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCloseExp);
        ss << ", pfnDestroyExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroyExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetMetricGroupExpProcAddrTable(ze_api_version_t version,
                                                    zet_metric_group_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetMetricGroupExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricGroupExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetMetricGroupExpProcAddrTable(ze_result_t ret,
                                                    ze_api_version_t version,
                                                    zet_metric_group_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricGroupExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetGetMetricStreamerProcAddrTable(ze_api_version_t version,
                                         zet_metric_streamer_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetMetricStreamerProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnOpen: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnOpen);
        ss << ", pfnClose: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnClose);
        ss << ", pfnReadData: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnReadData);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetMetricStreamerProcAddrTable(ze_api_version_t version,
                                                    zet_metric_streamer_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetMetricStreamerProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricStreamerProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetMetricStreamerProcAddrTable(ze_result_t ret,
                                                    ze_api_version_t version,
                                                    zet_metric_streamer_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricStreamerProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetGetMetricQueryPoolProcAddrTable(ze_api_version_t version,
                                          zet_metric_query_pool_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetMetricQueryPoolProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreate);
        ss << ", pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetMetricQueryPoolProcAddrTable(ze_api_version_t version,
                                                     zet_metric_query_pool_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetMetricQueryPoolProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricQueryPoolProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetMetricQueryPoolProcAddrTable(ze_result_t ret,
                                                     ze_api_version_t version,
                                                     zet_metric_query_pool_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricQueryPoolProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetGetMetricQueryProcAddrTable(ze_api_version_t version,
                                                         zet_metric_query_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetMetricQueryProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreate);
        ss << ", pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << ", pfnReset: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnReset);
        ss << ", pfnGetData: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetData);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetMetricQueryProcAddrTable(ze_api_version_t version,
                                                 zet_metric_query_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetMetricQueryProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricQueryProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetMetricQueryProcAddrTable(ze_result_t ret,
                                                 ze_api_version_t version,
                                                 zet_metric_query_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetMetricQueryProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetGetTracerExpProcAddrTable(ze_api_version_t version,
                                                       zet_tracer_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetTracerExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreate);
        ss << ", pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << ", pfnSetPrologues: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetPrologues);
        ss << ", pfnSetEpilogues: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetEpilogues);
        ss << ", pfnSetEnabled: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetEnabled);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetTracerExpProcAddrTable(ze_api_version_t version,
                                               zet_tracer_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetTracerExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetTracerExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetTracerExpProcAddrTable(ze_result_t ret,
                                               ze_api_version_t version,
                                               zet_tracer_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetTracerExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetGetDebugProcAddrTable(ze_api_version_t version,
                                                   zet_debug_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zetGetDebugProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnAttach: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnAttach);
        ss << ", pfnDetach: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDetach);
        ss << ", pfnReadEvent: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnReadEvent);
        ss << ", pfnAcknowledgeEvent: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAcknowledgeEvent);
        ss << ", pfnInterrupt: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnInterrupt);
        ss << ", pfnResume: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnResume);
        ss << ", pfnReadMemory: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnReadMemory);
        ss << ", pfnWriteMemory: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnWriteMemory);
        ss << ", pfnGetRegisterSetProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetRegisterSetProperties);
        ss << ", pfnReadRegisters: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnReadRegisters);
        ss << ", pfnWriteRegisters: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnWriteRegisters);
        ss << ", pfnGetThreadRegisterSetProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetThreadRegisterSetProperties);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetGetDebugProcAddrTable(ze_api_version_t version,
                                           zet_debug_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zetGetDebugProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetDebugProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zetGetDebugProcAddrTable(ze_result_t ret,
                                           ze_api_version_t version,
                                           zet_debug_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zetGetDebugProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}
