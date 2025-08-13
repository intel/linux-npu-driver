/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include "perfetto.h"

#include <stdint.h>

#include "level_zero_driver/api/ext/ze_graph.hpp"
#include "level_zero_driver/api/ext/ze_queue.hpp"
#include "level_zero_driver/api/prv/zex_driver.hpp"
#include "level_zero_driver/api/trace/trace_ze_api.hpp"
#include "level_zero_driver/api/trace/trace_ze_api_ddi.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/driver.hpp"
#include "level_zero_driver/source/driver_handle.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <array>
#include <dlfcn.h>
#include <level_zero/ze_api.h>
#include <level_zero/ze_command_queue_npu_ext.h>
#include <level_zero/ze_ddi.h>
#include <level_zero/ze_graph_ext.h>
#include <level_zero/ze_graph_profiling_ext.h>
#include <string.h>

#ifdef ANDROID
#define LIB_ZE_INTEL_VPU_NAME "libze_intel_vpu.so"
#else
#define LIB_ZE_INTEL_VPU_NAME "libze_intel_vpu.so.1"
#endif

namespace L0 {

ze_result_t zeInit(ze_init_flags_t flags) {
    trace_zeInit(flags);
    ze_result_t ret;

    void *handle = dlopen(LIB_ZE_INTEL_VPU_NAME, RTLD_NOLOAD | RTLD_LAZY);
    if (handle != nullptr) {
        dlclose(handle);
        LOG_E("Skip loading libze_intel_npu.so.* because libze_intel_vpu.so.* is installed");
        ret = ZE_RESULT_ERROR_UNINITIALIZED;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::init(flags));

exit:
    trace_zeInit(ret, flags);
    return ret;
}

ze_result_t zeDriverGet(uint32_t *pCount, ze_driver_handle_t *phDrivers) {
    trace_zeDriverGet(pCount, phDrivers);
    ze_result_t ret;

    L0_HANDLE_EXCEPTION(ret, L0::driverHandleGet(pCount, phDrivers));

    trace_zeDriverGet(ret, pCount, phDrivers);
    return ret;
}

ze_result_t
zeInitDrivers(uint32_t *pCount, ze_driver_handle_t *phDrivers, ze_init_driver_type_desc_t *desc) {
    trace_zeInitDrivers(pCount, phDrivers, desc);
    ze_result_t ret;

    L0_HANDLE_EXCEPTION(ret, L0::initDrivers(pCount, phDrivers, desc));

    trace_zeInitDrivers(ret, pCount, phDrivers, desc);
    return ret;
}

ze_result_t zeDriverGetProperties(ze_driver_handle_t hDriver, ze_driver_properties_t *pProperties) {
    trace_zeDriverGetProperties(hDriver, pProperties);
    ze_result_t ret;

    if (hDriver == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::DriverHandle::fromHandle(hDriver)->getProperties(pProperties));

exit:
    trace_zeDriverGetProperties(ret, hDriver, pProperties);
    return ret;
}

ze_result_t zeDriverGetApiVersion(ze_driver_handle_t hDriver, ze_api_version_t *version) {
    trace_zeDriverGetApiVersion(hDriver, version);
    ze_result_t ret;

    if (hDriver == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::DriverHandle::fromHandle(hDriver)->getApiVersion(version));

exit:
    trace_zeDriverGetApiVersion(ret, hDriver, version);
    return ret;
}

ze_result_t zeDriverGetIpcProperties(ze_driver_handle_t hDriver,
                                     ze_driver_ipc_properties_t *pIPCProperties) {
    trace_zeDriverGetIpcProperties(hDriver, pIPCProperties);
    ze_result_t ret;

    if (hDriver == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        L0::DriverHandle::fromHandle(hDriver)->getIPCProperties(pIPCProperties));

exit:
    trace_zeDriverGetIpcProperties(ret, hDriver, pIPCProperties);
    return ret;
}

ze_result_t zeDriverGetExtensionProperties(ze_driver_handle_t hDriver,
                                           uint32_t *pCount,
                                           ze_driver_extension_properties_t *pExtensionProperties) {
    trace_zeDriverGetExtensionProperties(hDriver, pCount, pExtensionProperties);
    ze_result_t ret;

    std::array<ze_driver_extension_properties_t, 11> supportedExts = {{
        {ZE_GRAPH_EXT_NAME, ZE_GRAPH_EXT_VERSION_CURRENT},
        {ZE_PROFILING_DATA_EXT_NAME, ZE_PROFILING_DATA_EXT_VERSION_1_0},
        {ZE_MUTABLE_COMMAND_LIST_EXP_NAME, ZE_MUTABLE_COMMAND_LIST_EXP_VERSION_1_1},
        {ZE_COMMAND_QUEUE_NPU_EXT_NAME, ZE_COMMAND_QUEUE_NPU_EXT_VERSION_1_0},
        {"ZE_extension_graph_1_2", ZE_GRAPH_EXT_VERSION_1_2},
        {"ZE_extension_graph_1_3", ZE_GRAPH_EXT_VERSION_1_3},
        {"ZE_extension_graph_1_4", ZE_GRAPH_EXT_VERSION_1_4},
        {"ZE_extension_graph_1_5", ZE_GRAPH_EXT_VERSION_1_5},
        {"ZE_extension_graph_1_6", ZE_GRAPH_EXT_VERSION_1_6},
        {"ZE_extension_graph_1_7", ZE_GRAPH_EXT_VERSION_1_7},
        {"ZE_extension_graph_1_8", ZE_GRAPH_EXT_VERSION_1_8},
    }};

    if (hDriver == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    if (pCount == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    for (auto &ext : supportedExts) {
        if (strcmp(ext.name, ZE_COMMAND_QUEUE_NPU_EXT_NAME) == 0) {
            auto driver = DriverHandle::fromHandle(hDriver);
            ext.version = driver->getSupportedCmdQueueExtVersion();
            break;
        }
    }

    if (*pCount == 0) {
        *pCount = supportedExts.size();
        ret = ZE_RESULT_SUCCESS;
        goto exit;
    }

    if (*pCount > supportedExts.size())
        *pCount = supportedExts.size();

    if (pExtensionProperties == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    for (size_t i = 0; i < *pCount; i++) {
        *pExtensionProperties = supportedExts[i];
        pExtensionProperties++;
    }

    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeDriverGetExtensionProperties(ret, hDriver, pCount, pExtensionProperties);
    return ret;
}

ze_result_t zeDriverGetExtensionFunctionAddress(ze_driver_handle_t hDriver,
                                                const char *name,
                                                void **ppFunctionAddress) {
    trace_zeDriverGetExtensionFunctionAddress(hDriver, name, ppFunctionAddress);
    ze_result_t ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (hDriver == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    if (name == nullptr || ppFunctionAddress == nullptr) {
        LOG_E("Invalid name or ppFunctionAddress pointer");
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (strcmp(name, ZE_PROFILING_DATA_EXT_NAME) == 0) {
        static ze_graph_profiling_dditable_ext_t table;
        table.pfnProfilingPoolCreate = L0::zeGraphProfilingPoolCreate;
        table.pfnProfilingPoolDestroy = L0::zeGraphProfilingPoolDestroy;
        table.pfnProfilingQueryCreate = L0::zeGraphProfilingQueryCreate;
        table.pfnProfilingQueryDestroy = L0::zeGraphProfilingQueryDestroy;
        table.pfnProfilingQueryGetData = L0::zeGraphProfilingQueryGetData;
        table.pfnDeviceGetProfilingDataProperties = L0::zeDeviceGetProfilingDataProperties;
        table.pfnProfilingLogGetString = L0::zeGraphProfilingLogGetString;
        *ppFunctionAddress = reinterpret_cast<void *>(&table);
        ret = ZE_RESULT_SUCCESS;
        goto exit;
    }

    if (strcmp(name, ZE_COMMAND_QUEUE_NPU_EXT_NAME) == 0) {
        static ze_command_queue_npu_dditable_ext_t table;

        table.pfnSetWorkloadType = L0::zeCommandQueueSetWorkloadType;
        *ppFunctionAddress = reinterpret_cast<void *>(&table);
        ret = ZE_RESULT_SUCCESS;
        goto exit;
    }

    static ze_graph_dditable_ext_t table;
    // version 1.0
    table.pfnCreate = L0::zeGraphCreate;
    table.pfnDestroy = L0::zeGraphDestroy;
    table.pfnGetNativeBinary = L0::zeGraphGetNativeBinary;
    table.pfnGetProperties = L0::zeGraphGetProperties;
    table.pfnGetArgumentProperties = L0::zeGraphGetArgumentProperties;
    table.pfnSetArgumentValue = L0::zeGraphSetArgumentValue;
    table.pfnAppendGraphInitialize = L0::zeAppendGraphInitialize;
    table.pfnAppendGraphExecute = L0::zeAppendGraphExecute;
    table.pfnDeviceGetGraphProperties = L0::zeDeviceGetGraphProperties;

    // version 1.1
    table.pfnGraphGetArgumentMetadata = L0::zeGraphGetArgumentMetadata;
    table.pfnGetArgumentProperties2 = L0::zeGraphGetArgumentProperties2;

    // version 1.2
    table.pfnGetArgumentProperties3 = L0::zeGraphGetArgumentProperties3;

    // version 1.3
    table.pfnQueryNetworkCreate = L0::zeGraphQueryNetworkCreate;
    table.pfnQueryNetworkDestroy = L0::zeGraphQueryNetworkDestroy;
    table.pfnQueryNetworkGetSupportedLayers = L0::zeGraphQueryNetworkGetSupportedLayers;

    // version 1.4
    table.pfnBuildLogGetString = L0::zeGraphBuildLogGetString;

    // version 1.5
    table.pfnCreate2 = L0::zeGraphCreate2;
    table.pfnQueryNetworkCreate2 = L0::zeGraphQueryNetworkCreate2;
    table.pfnQueryContextMemory = L0::zeGraphQueryContextMemory;

    // version 1.6
    table.pfnDeviceGetGraphProperties2 = L0::zeDeviceGetGraphProperties2;

    // version 1.7
    table.pfnGetNativeBinary2 = L0::zeGraphGetNativeBinary2;

    // version 1.8
    table.pfnGetProperties2 = L0::zeGraphGetProperties2;
    table.pfnGraphInitialize = L0::zeGraphInitialize;

    // version 1.11
    table.pfnCompilerGetSupportedOptions = L0::zeGraphCompilerGetSupportedOptions;
    table.pfnCompilerIsOptionSupported = L0::zeGraphCompilerIsOptionSupported;

    // version 1.12
    table.pfnCreate3 = L0::zeGraphCreate3;
    table.pfnGetProperties3 = L0::zeGraphGetProperties3;
    table.pfnBuildLogGetString2 = L0::zeGraphBuildLogGetString2;
    table.pfnBuildLogDestroy = L0::zeGraphBuildLogDestroy;

    if (strstr(name, ZE_GRAPH_EXT_NAME) != nullptr) {
        *ppFunctionAddress = reinterpret_cast<void *>(&table);
        ret = ZE_RESULT_SUCCESS;
        goto exit;
    }

#define CHECK_PRIVATE_FUNCTION(function)                             \
    {                                                                \
        if (strcmp(name, #function) == 0) {                          \
            *ppFunctionAddress = reinterpret_cast<void *>(function); \
            ret = ZE_RESULT_SUCCESS;                                 \
            goto exit;                                               \
        }                                                            \
    }

    CHECK_PRIVATE_FUNCTION(zexDiskCacheSetSize);
    CHECK_PRIVATE_FUNCTION(zexDiskCacheGetSize);
    CHECK_PRIVATE_FUNCTION(zexDiskCacheGetDirectory);

    LOG_E("Driver Function Extension with %s name does not exist", name);
exit:
    trace_zeDriverGetExtensionFunctionAddress(ret, hDriver, name, ppFunctionAddress);
    return ret;
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeGetGlobalProcAddrTable(ze_api_version_t version,
                                                             ze_global_dditable_t *pDdiTable) {
    trace_zeGetGlobalProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnInit = L0::zeInit;
    pDdiTable->pfnInitDrivers = L0::zeInitDrivers;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetGlobalProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetDriverProcAddrTable(ze_api_version_t version,
                                                             ze_driver_dditable_t *pDdiTable) {
    trace_zeGetDriverProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGet = L0::zeDriverGet;
    pDdiTable->pfnGetApiVersion = L0::zeDriverGetApiVersion;
    pDdiTable->pfnGetProperties = L0::zeDriverGetProperties;
    pDdiTable->pfnGetIpcProperties = L0::zeDriverGetIpcProperties;
    pDdiTable->pfnGetExtensionProperties = L0::zeDriverGetExtensionProperties;
    pDdiTable->pfnGetExtensionFunctionAddress = L0::zeDriverGetExtensionFunctionAddress;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetDriverProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
