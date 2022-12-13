/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/context/context.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"

#include <level_zero/ze_api.h>

struct _ze_driver_handle_t {};

namespace L0 {

struct Device;

struct DriverHandle : _ze_driver_handle_t {
    ~DriverHandle();
    ze_result_t createContext(const ze_context_desc_t *desc, ze_context_handle_t *phContext);
    ze_result_t getDevice(uint32_t *pCount, ze_device_handle_t *phDevices);
    ze_result_t getProperties(ze_driver_properties_t *properties);
    ze_result_t getApiVersion(ze_api_version_t *version);
    ze_result_t getIPCProperties(ze_driver_ipc_properties_t *pIPCProperties);
    ze_result_t getExtensionProperties(uint32_t *pCount,
                                       ze_driver_extension_properties_t *pExtensionProperties);
    ze_result_t getExtensionFunctionAddress(const char *name, void **ppFunctionAddress);

    ze_result_t getMemAllocProperties(const void *ptr,
                                      ze_memory_allocation_properties_t *pMemAllocProperties,
                                      ze_device_handle_t *phDevice);

    static DriverHandle *fromHandle(ze_driver_handle_t handle) {
        return static_cast<DriverHandle *>(handle);
    }

    inline ze_driver_handle_t toHandle() { return this; }

    DriverHandle &operator=(const DriverHandle &) = delete;
    DriverHandle &operator=(DriverHandle &&) = delete;

    static DriverHandle *create(std::vector<std::unique_ptr<VPU::VPUDevice>> devices);

    ze_result_t initialize(std::vector<std::unique_ptr<VPU::VPUDevice>> vpuDevices);

    uint32_t numDevices = 0;
    std::vector<Device *> devices;
    Device *getPrimaryDevice();
};

} // namespace L0
