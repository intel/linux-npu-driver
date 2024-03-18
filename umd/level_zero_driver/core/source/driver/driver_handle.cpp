/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero/ze_api.h"
#include "level_zero/ze_graph_ext.h"
#include "level_zero/ze_graph_profiling_ext.h"
#include "level_zero/ze_intel_vpu_uuid.h"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/driver/driver.hpp"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/api/ext/ze_graph.hpp"

#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <level_zero/ze_ddi.h>
#include <string.h>
#include <vector>

namespace L0 {

ze_result_t DriverHandle::createContext(const ze_context_desc_t *desc,
                                        ze_context_handle_t *phContext) {
    if ((nullptr == desc) || (nullptr == phContext)) {
        LOG_E("Given pointer desc or phContext is invalid.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto device = getPrimaryDevice();
    if (device == nullptr) {
        LOG_E("Failed to retrive device.");
        return ZE_RESULT_ERROR_DEVICE_LOST;
    }

    auto ctx = device->getVPUDevice()->createDeviceContext();
    if (!ctx) {
        LOG_E("VPUDevice failed to create Context.");
        return ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    Context *context = new Context(this, std::move(ctx));
    if (nullptr == context) {
        LOG_E("Failed to create Context");
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;
    }

    *phContext = context->toHandle();

    return ZE_RESULT_SUCCESS;
}

ze_result_t DriverHandle::getApiVersion(ze_api_version_t *version) {
    if (version == nullptr)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    *version = ZE_API_VERSION_CURRENT;
    return ZE_RESULT_SUCCESS;
}

ze_result_t DriverHandle::getProperties(ze_driver_properties_t *properties) {
    if (nullptr == properties) {
        LOG_E("Invalid parameter properties pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    properties->uuid = ze_intel_vpu_driver_uuid;
    properties->driverVersion = DRIVER_VERSION;

    LOG_I("Driver properties returned successfully.");
    return ZE_RESULT_SUCCESS;
}

ze_result_t DriverHandle::getIPCProperties(ze_driver_ipc_properties_t *pIPCProperties) {
    if (nullptr == pIPCProperties) {
        LOG_E("Invalid parameter pIPCProperties pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    pIPCProperties->flags = ZE_IPC_PROPERTY_FLAG_MEMORY;

    return ZE_RESULT_SUCCESS;
}

ze_result_t
DriverHandle::getExtensionProperties(uint32_t *pCount,
                                     ze_driver_extension_properties_t *pExtensionProperties) {
    std::array<ze_driver_extension_properties_t, 7> supportedExts = {{
        {ZE_GRAPH_EXT_NAME, ZE_GRAPH_EXT_VERSION_1_0},
        {ZE_GRAPH_EXT_NAME_1_1, ZE_GRAPH_EXT_VERSION_1_1},
        {ZE_GRAPH_EXT_NAME_1_2, ZE_GRAPH_EXT_VERSION_1_2},
        {ZE_GRAPH_EXT_NAME_1_3, ZE_GRAPH_EXT_VERSION_1_3},
        {ZE_GRAPH_EXT_NAME_1_4, ZE_GRAPH_EXT_VERSION_1_4},
        {ZE_GRAPH_EXT_NAME_1_5, ZE_GRAPH_EXT_VERSION_1_5},
        {ZE_PROFILING_DATA_EXT_NAME, ZE_PROFILING_DATA_EXT_VERSION_1_0},
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

ze_result_t
DriverHandle::getMemAllocProperties(const void *ptr,
                                    ze_memory_allocation_properties_t *pMemAllocProperties,
                                    ze_device_handle_t *phDevice) {
    pMemAllocProperties->type = ZE_MEMORY_TYPE_UNKNOWN;
    return ZE_RESULT_SUCCESS;
}

DriverHandle::DriverHandle(std::vector<std::unique_ptr<VPU::VPUDevice>> vpuDevices) {
    for (auto &vpuDevice : vpuDevices)
        devices.push_back(std::make_unique<Device>(this, std::move(vpuDevice)));

    numDevices = safe_cast<uint32_t>(devices.size());
    LOG_I("Update numDevices with '%d'.", numDevices);
}

std::unique_ptr<DriverHandle>
DriverHandle::create(std::vector<std::unique_ptr<VPU::VPUDevice>> devices) {
    if (devices.size() == 0) {
        LOG_W("No VPU devices found.");
        return nullptr;
    }

    return std::make_unique<DriverHandle>(std::move(devices));
}

ze_result_t DriverHandle::getDevice(uint32_t *pCount, ze_device_handle_t *phDevices) {
    if (nullptr == pCount) {
        LOG_E("Invalid pCount pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (*pCount == 0) {
        *pCount = numDevices;
        return ZE_RESULT_SUCCESS;
    }

    if (*pCount > numDevices)
        *pCount = numDevices;

    if (phDevices == nullptr) {
        LOG_E("Invalid phDevices pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    for (uint32_t i = 0; i < *pCount; i++) {
        phDevices[i] = devices[i].get();
    }

    return ZE_RESULT_SUCCESS;
}

// Retrieves primary device associated with DriverHandle.
// Current consideration is that first device allocated
// shall be utilised as primary device.
Device *DriverHandle::getPrimaryDevice() {
    if (devices[0] != nullptr)
        return devices[0].get();

    return nullptr;
}

ze_result_t DriverHandle::getExtensionFunctionAddress(const char *name, void **ppFunctionAddress) {
    if (name == nullptr || ppFunctionAddress == nullptr) {
        LOG_E("Invalid name or ppFunctionAddress pointer.");
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
        LOG_I("Return DDI table for extension: %s", name);
        return ZE_RESULT_SUCCESS;
    }

    static ze_graph_dditable_ext_1_5_t table;
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

    if (strcmp(name, ZE_GRAPH_EXT_NAME) == 0 || strcmp(name, ZE_GRAPH_EXT_NAME_1_1) == 0 ||
        strcmp(name, ZE_GRAPH_EXT_NAME_1_2) == 0 || strcmp(name, ZE_GRAPH_EXT_NAME_1_3) == 0 ||
        strcmp(name, ZE_GRAPH_EXT_NAME_1_4) == 0 || strcmp(name, ZE_GRAPH_EXT_NAME_1_5) == 0) {
        *ppFunctionAddress = reinterpret_cast<void *>(&table);
        LOG_I("Return DDI table for extension: %s", name);
        return ZE_RESULT_SUCCESS;
    }

    LOG_E("The name of extension is unknown: %s", name);
    return ZE_RESULT_ERROR_UNKNOWN;
}

} // namespace L0
