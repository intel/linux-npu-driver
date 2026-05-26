/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include "perfetto.h"

#include <stdint.h>

#include "level_zero_driver/api/ext/ze_context.hpp"
#include "level_zero_driver/api/ext/ze_driver_npu.hpp"
#include "level_zero_driver/api/ext/ze_graph.hpp"
#include "level_zero_driver/api/ext/ze_queue.hpp"
#include "level_zero_driver/api/prv/zex_driver.hpp"
#include "level_zero_driver/api/trace/trace_ze_api.hpp"
#include "level_zero_driver/api/trace/trace_ze_api_ddi.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/driver.hpp"
#include "level_zero_driver/source/driver_handle.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <algorithm>
#include <dlfcn.h>
#include <string.h>
#include <vector>
#include <ze_api.h>
#include <ze_command_queue_npu_ext.h>
#include <ze_context_npu_ext.h>
#include <ze_ddi.h>
#include <ze_driver_npu_ext.h>
#include <ze_graph_ext.h>
#include <ze_graph_profiling_ext.h>

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
                                           ze_driver_extension_properties_t *pExtProps) {
    trace_zeDriverGetExtensionProperties(hDriver, pCount, pExtProps);
    ze_result_t ret;
    std::vector<ze_driver_extension_properties_t> exts;

    if (hDriver == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    if (pCount == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    exts = DriverHandle::fromHandle(hDriver)->getSupportedExtensions();
    if (*pCount == 0) {
        *pCount = static_cast<uint32_t>(exts.size());
        ret = ZE_RESULT_SUCCESS;
        goto exit;
    }

    *pCount = std::min(*pCount, static_cast<uint32_t>(exts.size()));
    if (pExtProps == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    for (size_t i = 0; i < *pCount; i++) {
        pExtProps[i] = exts[i];
    }

    ret = ZE_RESULT_SUCCESS;
exit:
    trace_zeDriverGetExtensionProperties(ret, hDriver, pCount, pExtProps);
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
        static ze_graph_profiling_dditable_ext_t table = {
            .pfnProfilingPoolCreate = L0::zeGraphProfilingPoolCreate,
            .pfnProfilingPoolDestroy = L0::zeGraphProfilingPoolDestroy,
            .pfnProfilingQueryCreate = L0::zeGraphProfilingQueryCreate,
            .pfnProfilingQueryDestroy = L0::zeGraphProfilingQueryDestroy,
            .pfnProfilingQueryGetData = L0::zeGraphProfilingQueryGetData,
            .pfnDeviceGetProfilingDataProperties = L0::zeDeviceGetProfilingDataProperties,
            .pfnProfilingLogGetString = L0::zeGraphProfilingLogGetString,
        };

        *ppFunctionAddress = reinterpret_cast<void *>(&table);
        ret = ZE_RESULT_SUCCESS;
        goto exit;
    }

    if (strcmp(name, ZE_COMMAND_QUEUE_NPU_EXT_NAME) == 0) {
        static ze_command_queue_npu_dditable_ext_t table = {
            .pfnSetWorkloadType = L0::zeCommandQueueSetWorkloadType,
        };

        *ppFunctionAddress = reinterpret_cast<void *>(&table);
        ret = ZE_RESULT_SUCCESS;
        goto exit;
    }

    if (strcmp(name, ZE_CONTEXT_NPU_EXT_NAME) == 0) {
        static ze_context_npu_dditable_ext_t table = {
            .pfnSetProperties = L0::zeContextSetProperties,
            .pfnReleaseMemory = L0::zeContextReleaseMemory,
        };

        *ppFunctionAddress = reinterpret_cast<void *>(&table);
        ret = ZE_RESULT_SUCCESS;
        goto exit;
    }

    if (strcmp(name, ZE_DRIVER_NPU_EXT_NAME) == 0) {
        static ze_driver_npu_dditable_ext_t table = {
            .pfnGetExtension = L0::zeDriverGetExtensionExt,
        };

        *ppFunctionAddress = reinterpret_cast<void *>(&table);
        ret = ZE_RESULT_SUCCESS;
        goto exit;
    }

    static ze_graph_dditable_ext_t table = {
        // version 1.0
        .pfnCreate = L0::zeGraphCreate,
        .pfnDestroy = L0::zeGraphDestroy,
        .pfnGetProperties = L0::zeGraphGetProperties,
        .pfnGetArgumentProperties = L0::zeGraphGetArgumentProperties,
        .pfnSetArgumentValue = L0::zeGraphSetArgumentValue,
        .pfnAppendGraphInitialize = L0::zeAppendGraphInitialize,
        .pfnAppendGraphExecute = L0::zeAppendGraphExecute,
        .pfnGetNativeBinary = L0::zeGraphGetNativeBinary,
        .pfnDeviceGetGraphProperties = L0::zeDeviceGetGraphProperties,
        // version 1.1
        .pfnGraphGetArgumentMetadata = L0::zeGraphGetArgumentMetadata,
        .pfnGetArgumentProperties2 = L0::zeGraphGetArgumentProperties2,
        // version 1.2
        .pfnGetArgumentProperties3 = L0::zeGraphGetArgumentProperties3,
        // version 1.3
        .pfnQueryNetworkCreate = L0::zeGraphQueryNetworkCreate,
        .pfnQueryNetworkDestroy = L0::zeGraphQueryNetworkDestroy,
        .pfnQueryNetworkGetSupportedLayers = L0::zeGraphQueryNetworkGetSupportedLayers,
        // version 1.4
        .pfnBuildLogGetString = L0::zeGraphBuildLogGetString,
        // version 1.5
        .pfnCreate2 = L0::zeGraphCreate2,
        .pfnQueryNetworkCreate2 = L0::zeGraphQueryNetworkCreate2,
        .pfnQueryContextMemory = L0::zeGraphQueryContextMemory,
        // version 1.6
        .pfnDeviceGetGraphProperties2 = L0::zeDeviceGetGraphProperties2,
        // version 1.7
        .pfnGetNativeBinary2 = L0::zeGraphGetNativeBinary2,

        // version 1.8
        .pfnGetProperties2 = L0::zeGraphGetProperties2,
        .pfnGraphInitialize = L0::zeGraphInitialize,

        // version 1.11
        .pfnCompilerGetSupportedOptions = L0::zeGraphCompilerGetSupportedOptions,
        .pfnCompilerIsOptionSupported = L0::zeGraphCompilerIsOptionSupported,
        // version 1.12
        .pfnCreate3 = L0::zeGraphCreate3,
        .pfnGetProperties3 = L0::zeGraphGetProperties3,
        .pfnBuildLogGetString2 = L0::zeGraphBuildLogGetString2,
        .pfnBuildLogDestroy = L0::zeGraphBuildLogDestroy,

        // version 1.15
        .pfnSetArgumentValue2 = L0::zeGraphSetArgumentValue2,

        // version 1.16
        .pfnEvict = L0::zeGraphEvict,
    };

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
    CHECK_PRIVATE_FUNCTION(zexContextSetIdlePruningTimeout);

    LOG_E("Driver Function Extension with %s name does not exist", name);

exit:
    trace_zeDriverGetExtensionFunctionAddress(ret, hDriver, name, ppFunctionAddress);
    return ret;
}

ze_global_dditable_t zeGlobalDdiTable = {
    .pfnInit = L0::zeInit,
    .pfnInitDrivers = L0::zeInitDrivers,
};

ze_driver_dditable_t zeDriverDdiTable = {
    .pfnGet = zeDriverGet,
    .pfnGetApiVersion = zeDriverGetApiVersion,
    .pfnGetProperties = zeDriverGetProperties,
    .pfnGetIpcProperties = zeDriverGetIpcProperties,
    .pfnGetExtensionProperties = zeDriverGetExtensionProperties,
    .pfnGetExtensionFunctionAddress = zeDriverGetExtensionFunctionAddress,
    .pfnGetLastErrorDescription = nullptr,
    .pfnRTASFormatCompatibilityCheckExt = nullptr,
    .pfnGetDefaultContext = nullptr};

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

    if (version >= ZE_API_VERSION_1_0) {
        pDdiTable->pfnInit = L0::zeInit;
    }

    if (version >= ZE_API_VERSION_1_10) {
        pDdiTable->pfnInitDrivers = L0::zeInitDrivers;
    }

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

    if (version >= ZE_API_VERSION_1_0) {
        pDdiTable->pfnGet = L0::zeDriverGet;
        pDdiTable->pfnGetApiVersion = L0::zeDriverGetApiVersion;
        pDdiTable->pfnGetProperties = L0::zeDriverGetProperties;
        pDdiTable->pfnGetIpcProperties = L0::zeDriverGetIpcProperties;
        pDdiTable->pfnGetExtensionProperties = L0::zeDriverGetExtensionProperties;
    }

    if (version >= ZE_API_VERSION_1_1) {
        pDdiTable->pfnGetExtensionFunctionAddress = L0::zeDriverGetExtensionFunctionAddress;
    }

    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetDriverProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
