/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "level_zero_driver/api/ext/ze_graph.hpp"
#include "level_zero_driver/api/ext/ze_queue.hpp"
#include "level_zero_driver/core/source/driver/driver.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <array>
#include <level_zero/ze_api.h>
#include <level_zero/ze_command_queue_npu_ext.h>
#include <level_zero/ze_ddi.h>
#include <level_zero/ze_graph_ext.h>
#include <level_zero/ze_graph_profiling_ext.h>
#include <string.h>

namespace L0 {
ze_result_t zeInit(ze_init_flags_t flags) {
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::init(flags));
}

ze_result_t zeDriverGet(uint32_t *pCount, ze_driver_handle_t *phDrivers) {
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::driverHandleGet(pCount, phDrivers));
}

ze_result_t zeDriverGetProperties(ze_driver_handle_t hDriver, ze_driver_properties_t *pProperties) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::DriverHandle::fromHandle(hDriver)->getProperties(pProperties));
}

ze_result_t zeDriverGetApiVersion(ze_driver_handle_t hDriver, ze_api_version_t *version) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::DriverHandle::fromHandle(hDriver)->getApiVersion(version));
}

ze_result_t zeDriverGetIpcProperties(ze_driver_handle_t hDriver,
                                     ze_driver_ipc_properties_t *pIPCProperties) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::DriverHandle::fromHandle(hDriver)->getIPCProperties(pIPCProperties));
}

ze_result_t zeDriverGetExtensionProperties(ze_driver_handle_t hDriver,
                                           uint32_t *pCount,
                                           ze_driver_extension_properties_t *pExtensionProperties) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    std::array<ze_driver_extension_properties_t, 11> supportedExts = {{
        {ZE_GRAPH_EXT_NAME, ZE_GRAPH_EXT_VERSION_CURRENT},
        {ZE_PROFILING_DATA_EXT_NAME, ZE_PROFILING_DATA_EXT_VERSION_1_0},
        {ZE_MUTABLE_COMMAND_LIST_EXP_NAME, ZE_MUTABLE_COMMAND_LIST_EXP_VERSION_1_0},
        {ZE_COMMAND_QUEUE_NPU_EXT_NAME, ZE_COMMAND_QUEUE_NPU_EXT_VERSION_1_0},
        {"ZE_extension_graph_1_2", ZE_GRAPH_EXT_VERSION_1_2},
        {"ZE_extension_graph_1_3", ZE_GRAPH_EXT_VERSION_1_3},
        {"ZE_extension_graph_1_4", ZE_GRAPH_EXT_VERSION_1_4},
        {"ZE_extension_graph_1_5", ZE_GRAPH_EXT_VERSION_1_5},
        {"ZE_extension_graph_1_6", ZE_GRAPH_EXT_VERSION_1_6},
        {"ZE_extension_graph_1_7", ZE_GRAPH_EXT_VERSION_1_7},
        {"ZE_extension_graph_1_8", ZE_GRAPH_EXT_VERSION_1_8},
    }};

    if (pCount == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (*pCount == 0) {
        *pCount = supportedExts.size();
        return ZE_RESULT_SUCCESS;
    }

    if (*pCount > supportedExts.size())
        *pCount = supportedExts.size();

    if (pExtensionProperties == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    for (size_t i = 0; i < *pCount; i++) {
        *pExtensionProperties = supportedExts[i];
        pExtensionProperties++;
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t zeDriverGetExtensionFunctionAddress(ze_driver_handle_t hDriver,
                                                const char *name,
                                                void **ppFunctionAddress) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    if (name == nullptr || ppFunctionAddress == nullptr) {
        LOG_E("Invalid name or ppFunctionAddress pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
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
        LOG(DRIVER, "Return DDI table for extension: %s", name);
        return ZE_RESULT_SUCCESS;
    }

    if (strcmp(name, ZE_COMMAND_QUEUE_NPU_EXT_NAME) == 0) {
        static ze_command_queue_npu_dditable_ext_1_0_t table;

        table.pfnSetWorkloadType = L0::zeCommandQueueSetWorkloadType;
        *ppFunctionAddress = reinterpret_cast<void *>(&table);
        LOG(DRIVER, "Return DDI table for extension: %s", name);
        return ZE_RESULT_SUCCESS;
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

    if (strstr(name, ZE_GRAPH_EXT_NAME) != nullptr) {
        *ppFunctionAddress = reinterpret_cast<void *>(&table);
        LOG(DRIVER, "Return DDI table for extension: %s", name);
        return ZE_RESULT_SUCCESS;
    }

    LOG_E("The name of extension is unknown: %s", name);
    return ZE_RESULT_ERROR_UNKNOWN;
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeGetGlobalProcAddrTable(ze_api_version_t version,
                                                             ze_global_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnInit = L0::zeInit;
    return ZE_RESULT_SUCCESS;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetDriverProcAddrTable(ze_api_version_t version,
                                                             ze_driver_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnGet = L0::zeDriverGet;
    pDdiTable->pfnGetApiVersion = L0::zeDriverGetApiVersion;
    pDdiTable->pfnGetProperties = L0::zeDriverGetProperties;
    pDdiTable->pfnGetIpcProperties = L0::zeDriverGetIpcProperties;
    pDdiTable->pfnGetExtensionProperties = L0::zeDriverGetExtensionProperties;
    pDdiTable->pfnGetExtensionFunctionAddress = L0::zeDriverGetExtensionFunctionAddress;
    return ZE_RESULT_SUCCESS;
}
}
