/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "driver_handle.hpp"

#include "context.hpp"
#include "device.hpp"
#include "level_zero/ze_api.h"
#include "level_zero/ze_intel_npu_uuid.h"
#include "umd_common.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <utility>
#include <vector>

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

DriverHandle::DriverHandle(std::vector<std::unique_ptr<VPU::VPUDevice>> vpuDevices) {
    for (auto &vpuDevice : vpuDevices)
        devices.push_back(std::make_unique<Device>(this, std::move(vpuDevice)));

    numDevices = safe_cast<uint32_t>(devices.size());
    LOG(DRIVER, "Update numDevices with '%d'.", numDevices);
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
