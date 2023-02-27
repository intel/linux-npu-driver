/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/driver/driver.hpp"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/api/ext/ze_graph.hpp"

#include "driver_version_l0.h"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <level_zero/ze_ddi.h>
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

    uint32_t versionMajor = boost::numeric_cast<uint32_t>(L0_PROJECT_VERSION_MAJOR);
    uint32_t versionMinor = boost::numeric_cast<uint32_t>(L0_PROJECT_VERSION_MINOR);
    uint32_t versionBuild = boost::numeric_cast<uint32_t>(VPU_VERSION_BUILD);

    properties->uuid = {0x01,
                        0x7d,
                        0xe9,
                        0x31,
                        0x6b,
                        0x4d,
                        0x4f,
                        0xd4,
                        0xaa,
                        0x9b,
                        0x5b,
                        0xed,
                        0x77,
                        0xfc,
                        0x8e,
                        0x89};
    properties->driverVersion = ((versionMajor << 24) & 0xFF000000) |
                                ((versionMinor << 16) & 0x00FF0000) | (versionBuild & 0x0000FFFF);

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
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t
DriverHandle::getMemAllocProperties(const void *ptr,
                                    ze_memory_allocation_properties_t *pMemAllocProperties,
                                    ze_device_handle_t *phDevice) {
    pMemAllocProperties->type = ZE_MEMORY_TYPE_UNKNOWN;
    return ZE_RESULT_SUCCESS;
}

DriverHandle::~DriverHandle() {
    for (auto &device : devices)
        delete device;
}

ze_result_t DriverHandle::initialize(std::vector<std::unique_ptr<VPU::VPUDevice>> vpuDevices) {
    for (auto &vpuDevice : vpuDevices)
        devices.emplace_back(Device::create(this, vpuDevice.release()));

    if (devices.size() == 0) {
        LOG_W("No VPU devices found.");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    numDevices = boost::numeric_cast<uint32_t>(devices.size());
    LOG_I("Update numDevices with '%d'.", numDevices);

    return ZE_RESULT_SUCCESS;
}

DriverHandle *DriverHandle::create(std::vector<std::unique_ptr<VPU::VPUDevice>> devices) {
    auto *driverHandle = new DriverHandle;
    if (driverHandle == nullptr) {
        LOG_E("New DriverHandle allocation failed!");
        return nullptr;
    }

    ze_result_t res = driverHandle->initialize(std::move(devices));
    if (res != ZE_RESULT_SUCCESS) {
        delete driverHandle;
        return nullptr;
    }

    return driverHandle;
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
        phDevices[i] = devices[i];
    }

    return ZE_RESULT_SUCCESS;
}

// Retrieves primary device associated with DriverHandle.
// Current consideration is that first device allocated
// shall be utilised as primary device.
Device *DriverHandle::getPrimaryDevice() {
    if (devices[0] != nullptr)
        return devices[0];

    return nullptr;
}

ze_result_t DriverHandle::getExtensionFunctionAddress(const char *name, void **ppFunctionAddress) {
    if (name == nullptr || ppFunctionAddress == nullptr) {
        LOG_E("Invalid name or ppFunctionAddress pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (strncmp(name, ZE_GRAPH_EXT_NAME, strlen(ZE_GRAPH_EXT_NAME)) == 0) {
        static ze_graph_dditable_ext_t table;
        table.pfnCreate = L0::zeGraphCreate;
        table.pfnDestroy = L0::zeGraphDestroy;
        table.pfnGetNativeBinary = L0::zeGraphGetNativeBinary;
        table.pfnGetProperties = L0::zeGraphGetProperties;
        table.pfnGetArgumentProperties = L0::zeGraphGetArgumentProperties;
        table.pfnSetArgumentValue = L0::zeGraphSetArgumentValue;
        table.pfnAppendGraphInitialize = L0::zeAppendGraphInitialize;
        table.pfnAppendGraphExecute = L0::zeAppendGraphExecute;
        table.pfnDeviceGetGraphProperties = L0::zeDeviceGetGraphProperties;
        *ppFunctionAddress = reinterpret_cast<void *>(&table);
    } else if (strncmp(name, ZE_PROFILING_DATA_EXT_NAME, strlen(ZE_PROFILING_DATA_EXT_NAME)) == 0) {
        static ze_graph_profiling_dditable_ext_t table;
        table.pfnProfilingPoolCreate = L0::zeGraphProfilingPoolCreate;
        table.pfnProfilingPoolDestroy = L0::zeGraphProfilingPoolDestroy;
        table.pfnProfilingQueryCreate = L0::zeGraphProfilingQueryCreate;
        table.pfnProfilingQueryDestroy = L0::zeGraphProfilingQueryDestroy;
        table.pfnProfilingQueryGetData = L0::zeGraphProfilingQueryGetData;
        table.pfnDeviceGetProfilingDataProperties = L0::zeDeviceGetProfilingDataProperties;
        *ppFunctionAddress = reinterpret_cast<void *>(&table);
    } else {
        LOG_E("The name of extension is unknown: %s", name);
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    LOG_I("Return DDI table for extension: %s", name);
    return ZE_RESULT_SUCCESS;
}

} // namespace L0
