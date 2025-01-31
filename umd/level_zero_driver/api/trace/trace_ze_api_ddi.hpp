/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Generated header based on ze_api.h@version v1.11-r1.11.4

#pragma once

#include "trace_ze_api.hpp"

#include <iostream>
#include <level_zero/ze_ddi.h>
#include <sstream>

inline std::string
_trace_zeGetRTASBuilderExpProcAddrTable(ze_api_version_t version,
                                        ze_rtas_builder_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetRTASBuilderExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreateExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreateExp);
        ss << ", pfnGetBuildPropertiesExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetBuildPropertiesExp);
        ss << ", pfnBuildExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnBuildExp);
        ss << ", pfnDestroyExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroyExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetRTASBuilderExpProcAddrTable(ze_api_version_t version,
                                                   ze_rtas_builder_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetRTASBuilderExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetRTASBuilderExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetRTASBuilderExpProcAddrTable(ze_result_t ret,
                                                   ze_api_version_t version,
                                                   ze_rtas_builder_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetRTASBuilderExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGetRTASParallelOperationExpProcAddrTable(
    ze_api_version_t version,
    ze_rtas_parallel_operation_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetRTASParallelOperationExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreateExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreateExp);
        ss << ", pfnGetPropertiesExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetPropertiesExp);
        ss << ", pfnJoinExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnJoinExp);
        ss << ", pfnDestroyExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroyExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetRTASParallelOperationExpProcAddrTable(
    ze_api_version_t version,
    ze_rtas_parallel_operation_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetRTASParallelOperationExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetRTASParallelOperationExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetRTASParallelOperationExpProcAddrTable(
    ze_result_t ret,
    ze_api_version_t version,
    ze_rtas_parallel_operation_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetRTASParallelOperationExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGetGlobalProcAddrTable(ze_api_version_t version,
                                                   ze_global_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetGlobalProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnInit: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnInit);
        ss << ", pfnInitDrivers: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnInitDrivers);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetGlobalProcAddrTable(ze_api_version_t version,
                                           ze_global_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetGlobalProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetGlobalProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetGlobalProcAddrTable(ze_result_t ret,
                                           ze_api_version_t version,
                                           ze_global_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetGlobalProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetDriverProcAddrTable(ze_api_version_t version,
                                                   ze_driver_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetDriverProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGet: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGet);
        ss << ", pfnGetApiVersion: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetApiVersion);
        ss << ", pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetIpcProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetIpcProperties);
        ss << ", pfnGetExtensionProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetExtensionProperties);
        ss << ", pfnGetExtensionFunctionAddress: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetExtensionFunctionAddress);
        ss << ", pfnGetLastErrorDescription: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetLastErrorDescription);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetDriverProcAddrTable(ze_api_version_t version,
                                           ze_driver_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetDriverProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetDriverProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetDriverProcAddrTable(ze_result_t ret,
                                           ze_api_version_t version,
                                           ze_driver_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetDriverProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetDriverExpProcAddrTable(ze_api_version_t version,
                                                      ze_driver_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetDriverExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnRTASFormatCompatibilityCheckExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnRTASFormatCompatibilityCheckExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetDriverExpProcAddrTable(ze_api_version_t version,
                                              ze_driver_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetDriverExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetDriverExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetDriverExpProcAddrTable(ze_result_t ret,
                                              ze_api_version_t version,
                                              ze_driver_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetDriverExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGetDeviceProcAddrTable(ze_api_version_t version,
                                                   ze_device_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetDeviceProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGet: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGet);
        ss << ", pfnGetSubDevices: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetSubDevices);
        ss << ", pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetComputeProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetComputeProperties);
        ss << ", pfnGetModuleProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetModuleProperties);
        ss << ", pfnGetCommandQueueGroupProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetCommandQueueGroupProperties);
        ss << ", pfnGetMemoryProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetMemoryProperties);
        ss << ", pfnGetMemoryAccessProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetMemoryAccessProperties);
        ss << ", pfnGetCacheProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetCacheProperties);
        ss << ", pfnGetImageProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetImageProperties);
        ss << ", pfnGetExternalMemoryProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetExternalMemoryProperties);
        ss << ", pfnGetP2PProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetP2PProperties);
        ss << ", pfnCanAccessPeer: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCanAccessPeer);
        ss << ", pfnGetStatus: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetStatus);
        ss << ", pfnGetGlobalTimestamps: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetGlobalTimestamps);
        ss << ", pfnReserveCacheExt: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnReserveCacheExt);
        ss << ", pfnSetCacheAdviceExt: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetCacheAdviceExt);
        ss << ", pfnPciGetPropertiesExt: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnPciGetPropertiesExt);
        ss << ", pfnGetRootDevice: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetRootDevice);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetDeviceProcAddrTable(ze_api_version_t version,
                                           ze_device_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetDeviceProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetDeviceProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetDeviceProcAddrTable(ze_result_t ret,
                                           ze_api_version_t version,
                                           ze_device_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetDeviceProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetDeviceExpProcAddrTable(ze_api_version_t version,
                                                      ze_device_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetDeviceExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetFabricVertexExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetFabricVertexExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetDeviceExpProcAddrTable(ze_api_version_t version,
                                              ze_device_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetDeviceExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetDeviceExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetDeviceExpProcAddrTable(ze_result_t ret,
                                              ze_api_version_t version,
                                              ze_device_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetDeviceExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGetContextProcAddrTable(ze_api_version_t version,
                                                    ze_context_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetContextProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreate);
        ss << ", pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << ", pfnGetStatus: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetStatus);
        ss << ", pfnSystemBarrier: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSystemBarrier);
        ss << ", pfnMakeMemoryResident: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnMakeMemoryResident);
        ss << ", pfnEvictMemory: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnEvictMemory);
        ss << ", pfnMakeImageResident: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnMakeImageResident);
        ss << ", pfnEvictImage: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnEvictImage);
        ss << ", pfnCreateEx: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreateEx);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetContextProcAddrTable(ze_api_version_t version,
                                            ze_context_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetContextProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetContextProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetContextProcAddrTable(ze_result_t ret,
                                            ze_api_version_t version,
                                            ze_context_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetContextProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetCommandQueueProcAddrTable(ze_api_version_t version,
                                                         ze_command_queue_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetCommandQueueProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreate);
        ss << ", pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << ", pfnExecuteCommandLists: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnExecuteCommandLists);
        ss << ", pfnSynchronize: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSynchronize);
        ss << ", pfnGetOrdinal: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetOrdinal);
        ss << ", pfnGetIndex: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetIndex);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetCommandQueueProcAddrTable(ze_api_version_t version,
                                                 ze_command_queue_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetCommandQueueProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetCommandQueueProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetCommandQueueProcAddrTable(ze_result_t ret,
                                                 ze_api_version_t version,
                                                 ze_command_queue_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetCommandQueueProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGetCommandListProcAddrTable(ze_api_version_t version,
                                                        ze_command_list_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetCommandListProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreate);
        ss << ", pfnCreateImmediate: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreateImmediate);
        ss << ", pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << ", pfnClose: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnClose);
        ss << ", pfnReset: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnReset);
        ss << ", pfnAppendWriteGlobalTimestamp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendWriteGlobalTimestamp);
        ss << ", pfnAppendBarrier: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendBarrier);
        ss << ", pfnAppendMemoryRangesBarrier: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendMemoryRangesBarrier);
        ss << ", pfnAppendMemoryCopy: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendMemoryCopy);
        ss << ", pfnAppendMemoryFill: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendMemoryFill);
        ss << ", pfnAppendMemoryCopyRegion: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendMemoryCopyRegion);
        ss << ", pfnAppendMemoryCopyFromContext: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendMemoryCopyFromContext);
        ss << ", pfnAppendImageCopy: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendImageCopy);
        ss << ", pfnAppendImageCopyRegion: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendImageCopyRegion);
        ss << ", pfnAppendImageCopyToMemory: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendImageCopyToMemory);
        ss << ", pfnAppendImageCopyFromMemory: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendImageCopyFromMemory);
        ss << ", pfnAppendMemoryPrefetch: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendMemoryPrefetch);
        ss << ", pfnAppendMemAdvise: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendMemAdvise);
        ss << ", pfnAppendSignalEvent: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendSignalEvent);
        ss << ", pfnAppendWaitOnEvents: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendWaitOnEvents);
        ss << ", pfnAppendEventReset: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendEventReset);
        ss << ", pfnAppendQueryKernelTimestamps: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendQueryKernelTimestamps);
        ss << ", pfnAppendLaunchKernel: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendLaunchKernel);
        ss << ", pfnAppendLaunchCooperativeKernel: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendLaunchCooperativeKernel);
        ss << ", pfnAppendLaunchKernelIndirect: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendLaunchKernelIndirect);
        ss << ", pfnAppendLaunchMultipleKernelsIndirect: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendLaunchMultipleKernelsIndirect);
        ss << ", pfnAppendImageCopyToMemoryExt: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendImageCopyToMemoryExt);
        ss << ", pfnAppendImageCopyFromMemoryExt: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnAppendImageCopyFromMemoryExt);
        ss << ", pfnHostSynchronize: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnHostSynchronize);
        ss << ", pfnGetDeviceHandle: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetDeviceHandle);
        ss << ", pfnGetContextHandle: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetContextHandle);
        ss << ", pfnGetOrdinal: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetOrdinal);
        ss << ", pfnImmediateGetIndex: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnImmediateGetIndex);
        ss << ", pfnIsImmediate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnIsImmediate);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetCommandListProcAddrTable(ze_api_version_t version,
                                                ze_command_list_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetCommandListProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetCommandListProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetCommandListProcAddrTable(ze_result_t ret,
                                                ze_api_version_t version,
                                                ze_command_list_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetCommandListProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGetCommandListExpProcAddrTable(ze_api_version_t version,
                                        ze_command_list_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetCommandListExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreateCloneExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreateCloneExp);
        ss << ", pfnImmediateAppendCommandListsExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnImmediateAppendCommandListsExp);
        ss << ", pfnGetNextCommandIdExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetNextCommandIdExp);
        ss << ", pfnUpdateMutableCommandsExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnUpdateMutableCommandsExp);
        ss << ", pfnUpdateMutableCommandSignalEventExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnUpdateMutableCommandSignalEventExp);
        ss << ", pfnUpdateMutableCommandWaitEventsExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnUpdateMutableCommandWaitEventsExp);
        ss << ", pfnGetNextCommandIdWithKernelsExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetNextCommandIdWithKernelsExp);
        ss << ", pfnUpdateMutableCommandKernelsExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnUpdateMutableCommandKernelsExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetCommandListExpProcAddrTable(ze_api_version_t version,
                                                   ze_command_list_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetCommandListExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetCommandListExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetCommandListExpProcAddrTable(ze_result_t ret,
                                                   ze_api_version_t version,
                                                   ze_command_list_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetCommandListExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGetImageProcAddrTable(ze_api_version_t version,
                                                  ze_image_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetImageProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnCreate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreate);
        ss << ", pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << ", pfnGetAllocPropertiesExt: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetAllocPropertiesExt);
        ss << ", pfnViewCreateExt: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnViewCreateExt);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetImageProcAddrTable(ze_api_version_t version,
                                          ze_image_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetImageProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetImageProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetImageProcAddrTable(ze_result_t ret,
                                          ze_api_version_t version,
                                          ze_image_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetImageProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetImageExpProcAddrTable(ze_api_version_t version,
                                                     ze_image_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetImageExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetMemoryPropertiesExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetMemoryPropertiesExp);
        ss << ", pfnViewCreateExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnViewCreateExp);
        ss << ", pfnGetDeviceOffsetExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetDeviceOffsetExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetImageExpProcAddrTable(ze_api_version_t version,
                                             ze_image_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetImageExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetImageExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetImageExpProcAddrTable(ze_result_t ret,
                                             ze_api_version_t version,
                                             ze_image_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetImageExpProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetMemProcAddrTable(ze_api_version_t version,
                                                ze_mem_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetMemProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnAllocShared: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnAllocShared);
        ss << ", pfnAllocDevice: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnAllocDevice);
        ss << ", pfnAllocHost: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnAllocHost);
        ss << ", pfnFree: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnFree);
        ss << ", pfnGetAllocProperties: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetAllocProperties);
        ss << ", pfnGetAddressRange: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetAddressRange);
        ss << ", pfnGetIpcHandle: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetIpcHandle);
        ss << ", pfnOpenIpcHandle: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnOpenIpcHandle);
        ss << ", pfnCloseIpcHandle: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCloseIpcHandle);
        ss << ", pfnFreeExt: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnFreeExt);
        ss << ", pfnPutIpcHandle: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnPutIpcHandle);
        ss << ", pfnGetPitchFor2dImage: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetPitchFor2dImage);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetMemProcAddrTable(ze_api_version_t version, ze_mem_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetMemProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetMemProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetMemProcAddrTable(ze_result_t ret,
                                        ze_api_version_t version,
                                        ze_mem_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetMemProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetMemExpProcAddrTable(ze_api_version_t version,
                                                   ze_mem_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetMemExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetIpcHandleFromFileDescriptorExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetIpcHandleFromFileDescriptorExp);
        ss << ", pfnGetFileDescriptorFromIpcHandleExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetFileDescriptorFromIpcHandleExp);
        ss << ", pfnSetAtomicAccessAttributeExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetAtomicAccessAttributeExp);
        ss << ", pfnGetAtomicAccessAttributeExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetAtomicAccessAttributeExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetMemExpProcAddrTable(ze_api_version_t version,
                                           ze_mem_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetMemExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetMemExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetMemExpProcAddrTable(ze_result_t ret,
                                           ze_api_version_t version,
                                           ze_mem_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetMemExpProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetFenceProcAddrTable(ze_api_version_t version,
                                                  ze_fence_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetFenceProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreate);
        ss << ", pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << ", pfnHostSynchronize: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnHostSynchronize);
        ss << ", pfnQueryStatus: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnQueryStatus);
        ss << ", pfnReset: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnReset);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetFenceProcAddrTable(ze_api_version_t version,
                                          ze_fence_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetFenceProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetFenceProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetFenceProcAddrTable(ze_result_t ret,
                                          ze_api_version_t version,
                                          ze_fence_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetFenceProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetEventPoolProcAddrTable(ze_api_version_t version,
                                                      ze_event_pool_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetEventPoolProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreate);
        ss << ", pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << ", pfnGetIpcHandle: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetIpcHandle);
        ss << ", pfnOpenIpcHandle: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnOpenIpcHandle);
        ss << ", pfnCloseIpcHandle: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCloseIpcHandle);
        ss << ", pfnPutIpcHandle: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnPutIpcHandle);
        ss << ", pfnGetContextHandle: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetContextHandle);
        ss << ", pfnGetFlags: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetFlags);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetEventPoolProcAddrTable(ze_api_version_t version,
                                              ze_event_pool_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetEventPoolProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetEventPoolProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetEventPoolProcAddrTable(ze_result_t ret,
                                              ze_api_version_t version,
                                              ze_event_pool_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetEventPoolProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGetEventProcAddrTable(ze_api_version_t version,
                                                  ze_event_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetEventProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreate);
        ss << ", pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << ", pfnHostSignal: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnHostSignal);
        ss << ", pfnHostSynchronize: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnHostSynchronize);
        ss << ", pfnQueryStatus: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnQueryStatus);
        ss << ", pfnHostReset: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnHostReset);
        ss << ", pfnQueryKernelTimestamp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnQueryKernelTimestamp);
        ss << ", pfnQueryKernelTimestampsExt: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnQueryKernelTimestampsExt);
        ss << ", pfnGetEventPool: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetEventPool);
        ss << ", pfnGetSignalScope: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetSignalScope);
        ss << ", pfnGetWaitScope: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetWaitScope);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetEventProcAddrTable(ze_api_version_t version,
                                          ze_event_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetEventProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetEventProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetEventProcAddrTable(ze_result_t ret,
                                          ze_api_version_t version,
                                          ze_event_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetEventProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetEventExpProcAddrTable(ze_api_version_t version,
                                                     ze_event_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetEventExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnQueryTimestampsExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnQueryTimestampsExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetEventExpProcAddrTable(ze_api_version_t version,
                                             ze_event_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetEventExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetEventExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetEventExpProcAddrTable(ze_result_t ret,
                                             ze_api_version_t version,
                                             ze_event_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetEventExpProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetModuleProcAddrTable(ze_api_version_t version,
                                                   ze_module_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetModuleProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreate);
        ss << ", pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << ", pfnDynamicLink: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDynamicLink);
        ss << ", pfnGetNativeBinary: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetNativeBinary);
        ss << ", pfnGetGlobalPointer: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetGlobalPointer);
        ss << ", pfnGetKernelNames: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetKernelNames);
        ss << ", pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetFunctionPointer: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetFunctionPointer);
        ss << ", pfnInspectLinkageExt: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnInspectLinkageExt);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetModuleProcAddrTable(ze_api_version_t version,
                                           ze_module_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetModuleProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetModuleProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetModuleProcAddrTable(ze_result_t ret,
                                           ze_api_version_t version,
                                           ze_module_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetModuleProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string
_trace_zeGetModuleBuildLogProcAddrTable(ze_api_version_t version,
                                        ze_module_build_log_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetModuleBuildLogProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << ", pfnGetString: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetString);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetModuleBuildLogProcAddrTable(ze_api_version_t version,
                                                   ze_module_build_log_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetModuleBuildLogProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetModuleBuildLogProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetModuleBuildLogProcAddrTable(ze_result_t ret,
                                                   ze_api_version_t version,
                                                   ze_module_build_log_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetModuleBuildLogProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGetKernelProcAddrTable(ze_api_version_t version,
                                                   ze_kernel_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetKernelProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnCreate: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnCreate);
        ss << ", pfnDestroy: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnDestroy);
        ss << ", pfnSetCacheConfig: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetCacheConfig);
        ss << ", pfnSetGroupSize: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetGroupSize);
        ss << ", pfnSuggestGroupSize: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSuggestGroupSize);
        ss << ", pfnSuggestMaxCooperativeGroupCount: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSuggestMaxCooperativeGroupCount);
        ss << ", pfnSetArgumentValue: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetArgumentValue);
        ss << ", pfnSetIndirectAccess: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetIndirectAccess);
        ss << ", pfnGetIndirectAccess: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetIndirectAccess);
        ss << ", pfnGetSourceAttributes: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetSourceAttributes);
        ss << ", pfnGetProperties: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetProperties);
        ss << ", pfnGetName: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetName);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetKernelProcAddrTable(ze_api_version_t version,
                                           ze_kernel_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetKernelProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetKernelProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetKernelProcAddrTable(ze_result_t ret,
                                           ze_api_version_t version,
                                           ze_kernel_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetKernelProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetKernelExpProcAddrTable(ze_api_version_t version,
                                                      ze_kernel_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetKernelExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnSetGlobalOffsetExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetGlobalOffsetExp);
        ss << ", pfnSchedulingHintExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSchedulingHintExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetKernelExpProcAddrTable(ze_api_version_t version,
                                              ze_kernel_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetKernelExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetKernelExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetKernelExpProcAddrTable(ze_result_t ret,
                                              ze_api_version_t version,
                                              ze_kernel_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetKernelExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGetSamplerProcAddrTable(ze_api_version_t version,
                                                    ze_sampler_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetSamplerProcAddrTable(";
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
inline void trace_zeGetSamplerProcAddrTable(ze_api_version_t version,
                                            ze_sampler_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetSamplerProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetSamplerProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetSamplerProcAddrTable(ze_result_t ret,
                                            ze_api_version_t version,
                                            ze_sampler_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetSamplerProcAddrTable(version, pDdiTable) + trace_ze_result_t(ret);
}

inline std::string _trace_zeGetPhysicalMemProcAddrTable(ze_api_version_t version,
                                                        ze_physical_mem_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetPhysicalMemProcAddrTable(";
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
inline void trace_zeGetPhysicalMemProcAddrTable(ze_api_version_t version,
                                                ze_physical_mem_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetPhysicalMemProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetPhysicalMemProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetPhysicalMemProcAddrTable(ze_result_t ret,
                                                ze_api_version_t version,
                                                ze_physical_mem_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetPhysicalMemProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeGetVirtualMemProcAddrTable(ze_api_version_t version,
                                                       ze_virtual_mem_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetVirtualMemProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnReserve: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnReserve);
        ss << ", pfnFree: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnFree);
        ss << ", pfnQueryPageSize: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnQueryPageSize);
        ss << ", pfnMap: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnMap);
        ss << ", pfnUnmap: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnUnmap);
        ss << ", pfnSetAccessAttribute: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnSetAccessAttribute);
        ss << ", pfnGetAccessAttribute: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetAccessAttribute);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetVirtualMemProcAddrTable(ze_api_version_t version,
                                               ze_virtual_mem_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetVirtualMemProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetVirtualMemProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetVirtualMemProcAddrTable(ze_result_t ret,
                                               ze_api_version_t version,
                                               ze_virtual_mem_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetVirtualMemProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGetFabricVertexExpProcAddrTable(ze_api_version_t version,
                                         ze_fabric_vertex_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetFabricVertexExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetExp);
        ss << ", pfnGetSubVerticesExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetSubVerticesExp);
        ss << ", pfnGetPropertiesExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetPropertiesExp);
        ss << ", pfnGetDeviceExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetDeviceExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetFabricVertexExpProcAddrTable(ze_api_version_t version,
                                                    ze_fabric_vertex_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetFabricVertexExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetFabricVertexExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetFabricVertexExpProcAddrTable(ze_result_t ret,
                                                    ze_api_version_t version,
                                                    ze_fabric_vertex_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetFabricVertexExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeGetFabricEdgeExpProcAddrTable(ze_api_version_t version,
                                       ze_fabric_edge_exp_dditable_t *pDdiTable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_DDI] zeGetFabricEdgeExpProcAddrTable(";
    ss << "version: " << version;
    if (pDdiTable == nullptr) {
        ss << ", pDdiTable: nullptr";
    } else {
        ss << ", pDdiTable {";
        ss << "pfnGetExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetExp);
        ss << ", pfnGetVerticesExp: " << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetVerticesExp);
        ss << ", pfnGetPropertiesExp: "
           << reinterpret_cast<uintptr_t>(pDdiTable->pfnGetPropertiesExp);
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeGetFabricEdgeExpProcAddrTable(ze_api_version_t version,
                                                  ze_fabric_edge_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_BEGIN("API", "zeGetFabricEdgeExpProcAddrTable");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetFabricEdgeExpProcAddrTable(version, pDdiTable) + "..\n";
}
inline void trace_zeGetFabricEdgeExpProcAddrTable(ze_result_t ret,
                                                  ze_api_version_t version,
                                                  ze_fabric_edge_exp_dditable_t *pDdiTable) {
    TRACE_EVENT_END("API");
    if (IS_API_DDI_TRACE())
        std::cerr << _trace_zeGetFabricEdgeExpProcAddrTable(version, pDdiTable) +
                         trace_ze_result_t(ret);
}
