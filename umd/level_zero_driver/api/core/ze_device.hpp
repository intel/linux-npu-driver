/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/driver/driver.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

namespace L0 {
ze_result_t
zeDeviceGet(ze_driver_handle_t hDriver, uint32_t *pCount, ze_device_handle_t *phDevices) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::DriverHandle::fromHandle(hDriver)->getDevice(pCount, phDevices);
}

ze_result_t zeDeviceGetSubDevices(ze_device_handle_t hDevice,
                                  uint32_t *pCount,
                                  ze_device_handle_t *phSubdevices) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Device::fromHandle(hDevice)->getSubDevices(pCount, phSubdevices);
}

ze_result_t zeDeviceGetProperties(ze_device_handle_t hDevice,
                                  ze_device_properties_t *pDeviceProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Device::fromHandle(hDevice)->getProperties(pDeviceProperties);
}

ze_result_t zeDeviceGetComputeProperties(ze_device_handle_t hDevice,
                                         ze_device_compute_properties_t *pComputeProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Device::fromHandle(hDevice)->getDeviceComputeProperties(pComputeProperties);
}

ze_result_t zeDeviceGetModuleProperties(ze_device_handle_t hDevice,
                                        ze_device_module_properties_t *pKernelProperties) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeDeviceGetMemoryProperties(ze_device_handle_t hDevice,
                                        uint32_t *pCount,
                                        ze_device_memory_properties_t *pMemProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Device::fromHandle(hDevice)->getMemoryProperties(pCount, pMemProperties);
}

ze_result_t
zeDeviceGetMemoryAccessProperties(ze_device_handle_t hDevice,
                                  ze_device_memory_access_properties_t *pMemAccessProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Device::fromHandle(hDevice)->getMemoryAccessProperties(pMemAccessProperties);
}

ze_result_t zeDeviceGetCacheProperties(ze_device_handle_t hDevice,
                                       uint32_t *pCount,
                                       ze_device_cache_properties_t *pCacheProperties) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeDeviceGetImageProperties(ze_device_handle_t hDevice,
                                       ze_device_image_properties_t *pImageProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Device::fromHandle(hDevice)->getDeviceImageProperties(pImageProperties);
}

ze_result_t zeDeviceGetP2PProperties(ze_device_handle_t hDevice,
                                     ze_device_handle_t hPeerDevice,
                                     ze_device_p2p_properties_t *pP2PProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Device::fromHandle(hDevice)->getP2PProperties(hPeerDevice, pP2PProperties);
}

ze_result_t zeDeviceCanAccessPeer(ze_device_handle_t hDevice,
                                  ze_device_handle_t hPeerDevice,
                                  ze_bool_t *value) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeDeviceSetLastLevelCacheConfig(ze_device_handle_t hDevice,
                                            ze_cache_config_flags_t cacheConfig) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Device::fromHandle(hDevice)->setLastLevelCacheConfig(cacheConfig);
}

ze_result_t zeDeviceGetCommandQueueGroupProperties(
    ze_device_handle_t hDevice,
    uint32_t *pCount,
    ze_command_queue_group_properties_t *pCommandQueueGroupProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Device::fromHandle(hDevice)->getCommandQueueGroupProperties(
        pCount,
        pCommandQueueGroupProperties);
}

ze_result_t zeDeviceGetExternalMemoryProperties(
    ze_device_handle_t hDevice,
    ze_device_external_memory_properties_t *pExternalMemoryProperties) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeDeviceGetStatus(ze_device_handle_t hDevice) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Device::fromHandle(hDevice)->getStatus();
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeDeviceGet(ze_driver_handle_t hDriver,
                                                uint32_t *pCount,
                                                ze_device_handle_t *phDevices) {
    return L0::zeDeviceGet(hDriver, pCount, phDevices);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeDeviceGetSubDevices(ze_device_handle_t hDevice,
                                                          uint32_t *pCount,
                                                          ze_device_handle_t *phSubdevices) {
    return L0::zeDeviceGetSubDevices(hDevice, pCount, phSubdevices);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeDeviceGetProperties(ze_device_handle_t hDevice, ze_device_properties_t *pDeviceProperties) {
    return L0::zeDeviceGetProperties(hDevice, pDeviceProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeDeviceGetComputeProperties(ze_device_handle_t hDevice,
                             ze_device_compute_properties_t *pComputeProperties) {
    return L0::zeDeviceGetComputeProperties(hDevice, pComputeProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeDeviceGetModuleProperties(ze_device_handle_t hDevice,
                            ze_device_module_properties_t *pKernelProperties) {
    return L0::zeDeviceGetModuleProperties(hDevice, pKernelProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeDeviceGetMemoryProperties(ze_device_handle_t hDevice,
                            uint32_t *pCount,
                            ze_device_memory_properties_t *pMemProperties) {
    return L0::zeDeviceGetMemoryProperties(hDevice, pCount, pMemProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeDeviceGetMemoryAccessProperties(ze_device_handle_t hDevice,
                                  ze_device_memory_access_properties_t *pMemAccessProperties) {
    return L0::zeDeviceGetMemoryAccessProperties(hDevice, pMemAccessProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeDeviceGetCacheProperties(ze_device_handle_t hDevice,
                           uint32_t *pCount,
                           ze_device_cache_properties_t *pCacheProperties) {
    return L0::zeDeviceGetCacheProperties(hDevice, pCount, pCacheProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeDeviceGetImageProperties(ze_device_handle_t hDevice,
                           ze_device_image_properties_t *pImageProperties) {
    return L0::zeDeviceGetImageProperties(hDevice, pImageProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeDeviceGetP2PProperties(ze_device_handle_t hDevice,
                         ze_device_handle_t hPeerDevice,
                         ze_device_p2p_properties_t *pP2PProperties) {
    return L0::zeDeviceGetP2PProperties(hDevice, hPeerDevice, pP2PProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeDeviceCanAccessPeer(ze_device_handle_t hDevice,
                                                          ze_device_handle_t hPeerDevice,
                                                          ze_bool_t *value) {
    return L0::zeDeviceCanAccessPeer(hDevice, hPeerDevice, value);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeDeviceSetLastLevelCacheConfig(ze_device_handle_t hDevice, ze_cache_config_flags_t cacheConfig) {
    return L0::zeDeviceSetLastLevelCacheConfig(hDevice, cacheConfig);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeDeviceGetCommandQueueGroupProperties(
    ze_device_handle_t hDevice,
    uint32_t *pCount,
    ze_command_queue_group_properties_t *pCommandQueueGroupProperties) {
    return L0::zeDeviceGetCommandQueueGroupProperties(hDevice,
                                                      pCount,
                                                      pCommandQueueGroupProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeDeviceGetExternalMemoryProperties(
    ze_device_handle_t hDevice,
    ze_device_external_memory_properties_t *pExternalMemoryProperties) {
    return L0::zeDeviceGetExternalMemoryProperties(hDevice, pExternalMemoryProperties);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeDeviceGetStatus(ze_device_handle_t hDevice) {
    return L0::zeDeviceGetStatus(hDevice);
}
} // extern "C"
