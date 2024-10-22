/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/include/l0_exception.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

namespace L0 {
ze_result_t
zeDeviceGet(ze_driver_handle_t hDriver, uint32_t *pCount, ze_device_handle_t *phDevices) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::DriverHandle::fromHandle(hDriver)->getDevice(pCount, phDevices));
}

ze_result_t zeDeviceGetSubDevices(ze_device_handle_t hDevice,
                                  uint32_t *pCount,
                                  ze_device_handle_t *phSubdevices) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Device::fromHandle(hDevice)->getSubDevices(pCount, phSubdevices));
}

ze_result_t zeDeviceGetProperties(ze_device_handle_t hDevice,
                                  ze_device_properties_t *pDeviceProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Device::fromHandle(hDevice)->getProperties(pDeviceProperties));
}

ze_result_t zeDeviceGetComputeProperties(ze_device_handle_t hDevice,
                                         ze_device_compute_properties_t *pComputeProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Device::fromHandle(hDevice)->getDeviceComputeProperties(pComputeProperties));
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
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Device::fromHandle(hDevice)->getMemoryProperties(pCount, pMemProperties));
}

ze_result_t zeDeviceGetExternalMemoryProperties(
    ze_device_handle_t hDevice,
    ze_device_external_memory_properties_t *pExternalMemoryProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Device::fromHandle(hDevice)->getGetExternalMemoryProperties(pExternalMemoryProperties));
}

ze_result_t
zeDeviceGetMemoryAccessProperties(ze_device_handle_t hDevice,
                                  ze_device_memory_access_properties_t *pMemAccessProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Device::fromHandle(hDevice)->getMemoryAccessProperties(pMemAccessProperties));
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
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Device::fromHandle(hDevice)->getDeviceImageProperties(pImageProperties));
}

ze_result_t zeDeviceGetP2PProperties(ze_device_handle_t hDevice,
                                     ze_device_handle_t hPeerDevice,
                                     ze_device_p2p_properties_t *pP2PProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Device::fromHandle(hDevice)->getP2PProperties(hPeerDevice, pP2PProperties));
}

ze_result_t zeDeviceCanAccessPeer(ze_device_handle_t hDevice,
                                  ze_device_handle_t hPeerDevice,
                                  ze_bool_t *value) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeDeviceGetCommandQueueGroupProperties(
    ze_device_handle_t hDevice,
    uint32_t *pCount,
    ze_command_queue_group_properties_t *pCommandQueueGroupProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Device::fromHandle(hDevice)->getCommandQueueGroupProperties(
        pCount,
        pCommandQueueGroupProperties));
}

ze_result_t zeDeviceGetStatus(ze_device_handle_t hDevice) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Device::fromHandle(hDevice)->getStatus());
}

ze_result_t zeDeviceGetGlobalTimestamps(ze_device_handle_t hDevice,
                                        uint64_t *hostTimestamp,
                                        uint64_t *deviceTimestamp) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Device::fromHandle(hDevice)->getGlobalTimestamps(hostTimestamp, deviceTimestamp);
}

ze_result_t zeDevicePciGetPropertiesExt(ze_device_handle_t hDevice,
                                        ze_pci_ext_properties_t *pPciProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Device::fromHandle(hDevice)->getPciProperties(pPciProperties));
}

} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeGetDeviceProcAddrTable(ze_api_version_t version,
                                                             ze_device_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnGet = L0::zeDeviceGet;
    pDdiTable->pfnGetCommandQueueGroupProperties = L0::zeDeviceGetCommandQueueGroupProperties;
    pDdiTable->pfnGetSubDevices = L0::zeDeviceGetSubDevices;
    pDdiTable->pfnGetProperties = L0::zeDeviceGetProperties;
    pDdiTable->pfnGetComputeProperties = L0::zeDeviceGetComputeProperties;
    pDdiTable->pfnGetModuleProperties = L0::zeDeviceGetModuleProperties;
    pDdiTable->pfnGetMemoryProperties = L0::zeDeviceGetMemoryProperties;
    pDdiTable->pfnGetMemoryAccessProperties = L0::zeDeviceGetMemoryAccessProperties;
    pDdiTable->pfnGetCacheProperties = L0::zeDeviceGetCacheProperties;
    pDdiTable->pfnGetImageProperties = L0::zeDeviceGetImageProperties;
    pDdiTable->pfnGetP2PProperties = L0::zeDeviceGetP2PProperties;
    pDdiTable->pfnCanAccessPeer = L0::zeDeviceCanAccessPeer;
    pDdiTable->pfnGetStatus = L0::zeDeviceGetStatus;
    pDdiTable->pfnGetGlobalTimestamps = L0::zeDeviceGetGlobalTimestamps;
    pDdiTable->pfnGetExternalMemoryProperties = L0::zeDeviceGetExternalMemoryProperties;
    pDdiTable->pfnPciGetPropertiesExt = L0::zeDevicePciGetPropertiesExt;
    return ZE_RESULT_SUCCESS;
}
}
