/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "driver_handle.hpp"

#include "context.hpp"
#include "device.hpp"
#include "umd_common.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <cstring>
#include <utility>
#include <vector>
#include <ze_api.h>
#include <ze_command_queue_npu_ext.h>
#include <ze_context_npu_ext.h>
#include <ze_graph_ext.h>
#include <ze_graph_profiling_ext.h>
#include <ze_intel_npu_uuid.h>

namespace L0 {

ze_result_t DriverHandle::createContext(const ze_context_desc_t *desc,
                                        ze_context_handle_t *phContext) {
    if ((nullptr == desc) || (nullptr == phContext)) {
        LOG_E("Given pointer desc or phContext is invalid");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto device = getPrimaryDevice();
    if (device == nullptr) {
        LOG_E("Failed to retrive device");
        return ZE_RESULT_ERROR_DEVICE_LOST;
    }

    auto ctx = device->getVPUDevice()->createDeviceContext();
    if (!ctx) {
        LOG_E("VPUDevice failed to create Context");
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
        LOG_E("Invalid parameter properties pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    properties->uuid = ze_intel_npu_driver_uuid;
    properties->driverVersion = DRIVER_VERSION;

    LOG(DRIVER, "Driver properties returned successfully");
    return ZE_RESULT_SUCCESS;
}

ze_result_t DriverHandle::getIPCProperties(ze_driver_ipc_properties_t *pIPCProperties) {
    if (nullptr == pIPCProperties) {
        LOG_E("Invalid parameter pIPCProperties pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    pIPCProperties->flags = ZE_IPC_PROPERTY_FLAG_MEMORY;

    return ZE_RESULT_SUCCESS;
}

ze_result_t
DriverHandle::getMemAllocProperties(const void *ptr,
                                    ze_memory_allocation_properties_t *pMemAllocProperties,
                                    ze_device_handle_t *phDevice) {
    pMemAllocProperties->type = ZE_MEMORY_TYPE_UNKNOWN;
    return ZE_RESULT_SUCCESS;
}

static ze_command_queue_npu_ext_version_t
getSupportedCmdQueueExtVersion(const std::vector<std::unique_ptr<Device>> &devices) {
    if (devices.empty())
        return ZE_COMMAND_QUEUE_NPU_EXT_VERSION_1_0;

    for (auto &device : devices) {
        auto *vpuDevice = device->getVPUDevice();
        if (vpuDevice == nullptr || !vpuDevice->getCapCmdQueueCreation())
            return ZE_COMMAND_QUEUE_NPU_EXT_VERSION_1_0;
    }

    return ZE_COMMAND_QUEUE_NPU_EXT_VERSION_1_1;
}

static bool isUserPtrSupported(const std::vector<std::unique_ptr<Device>> &devices) {
    for (const auto &device : devices) {
        auto *vpuDevice = device->getVPUDevice();
        if (vpuDevice == nullptr || !vpuDevice->getHwInfo().userPtrCapability) {
            return false;
        }
    }
    return true;
}

std::vector<ze_driver_extension_properties_t> DriverHandle::getSupportedExtensions() {
    std::vector<ze_driver_extension_properties_t> extensions;
    extensions.reserve(13);

    auto appendExtension = [&extensions](const char *name, uint32_t version) {
        ze_driver_extension_properties_t props = {};
        strncpy(props.name, name, ZE_MAX_EXTENSION_NAME - 1);
        props.name[ZE_MAX_EXTENSION_NAME - 1] = '\0';
        props.version = version;
        extensions.push_back(props);
    };

    appendExtension(ZE_GRAPH_EXT_NAME, ZE_GRAPH_EXT_VERSION_CURRENT);
    appendExtension(ZE_PROFILING_DATA_EXT_NAME, ZE_PROFILING_DATA_EXT_VERSION_1_0);
    appendExtension(ZE_MUTABLE_COMMAND_LIST_EXP_NAME, ZE_MUTABLE_COMMAND_LIST_EXP_VERSION_1_1);
    appendExtension(ZE_COMMAND_QUEUE_NPU_EXT_NAME, getSupportedCmdQueueExtVersion(devices));
    appendExtension(ZE_CONTEXT_NPU_EXT_NAME, ZE_CONTEXT_NPU_EXT_VERSION_CURRENT);

    if (isUserPtrSupported(devices))
        appendExtension(ZE_EXTERNAL_MEMORY_MAPPING_EXT_NAME,
                        ZE_EXTERNAL_MEMMAP_SYSMEM_EXT_VERSION_1_0);

    appendExtension("ZE_extension_graph_1_2", ZE_GRAPH_EXT_VERSION_1_2);
    appendExtension("ZE_extension_graph_1_3", ZE_GRAPH_EXT_VERSION_1_3);
    appendExtension("ZE_extension_graph_1_4", ZE_GRAPH_EXT_VERSION_1_4);
    appendExtension("ZE_extension_graph_1_5", ZE_GRAPH_EXT_VERSION_1_5);
    appendExtension("ZE_extension_graph_1_6", ZE_GRAPH_EXT_VERSION_1_6);
    appendExtension("ZE_extension_graph_1_7", ZE_GRAPH_EXT_VERSION_1_7);
    appendExtension("ZE_extension_graph_1_8", ZE_GRAPH_EXT_VERSION_1_8);

    return extensions;
}

DriverHandle::DriverHandle(std::vector<std::unique_ptr<VPU::VPUDevice>> vpuDevices) {
    for (auto &vpuDevice : vpuDevices)
        devices.push_back(std::make_unique<Device>(this, std::move(vpuDevice)));

    numDevices = safe_cast<uint32_t>(devices.size());
}

std::unique_ptr<DriverHandle>
DriverHandle::create(std::vector<std::unique_ptr<VPU::VPUDevice>> devices) {
    if (devices.size() == 0) {
        LOG_W("No VPU devices found");
        return nullptr;
    }

    return std::make_unique<DriverHandle>(std::move(devices));
}

ze_result_t DriverHandle::getDevice(uint32_t *pCount, ze_device_handle_t *phDevices) {
    if (nullptr == pCount) {
        LOG_E("Invalid pCount pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (*pCount == 0) {
        *pCount = numDevices;
        return ZE_RESULT_SUCCESS;
    }

    if (*pCount > numDevices)
        *pCount = numDevices;

    if (phDevices == nullptr) {
        LOG_E("Invalid phDevices pointer");
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

} // namespace L0
