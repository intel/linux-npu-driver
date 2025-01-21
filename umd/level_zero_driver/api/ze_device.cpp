/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "level_zero_driver/api/trace/trace_ze_api.hpp"
#include "level_zero_driver/api/trace/trace_ze_api_ddi.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/device.hpp"
#include "level_zero_driver/source/driver_handle.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

namespace L0 {
ze_result_t
zeDeviceGet(ze_driver_handle_t hDriver, uint32_t *pCount, ze_device_handle_t *phDevices) {
    trace_zeDeviceGet(hDriver, pCount, phDevices);
    ze_result_t ret;

    if (hDriver == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::DriverHandle::fromHandle(hDriver)->getDevice(pCount, phDevices));

exit:
    trace_zeDeviceGet(ret, hDriver, pCount, phDevices);
    return ret;
}

ze_result_t zeDeviceGetSubDevices(ze_device_handle_t hDevice,
                                  uint32_t *pCount,
                                  ze_device_handle_t *phSubdevices) {
    trace_zeDeviceGetSubDevices(hDevice, pCount, phSubdevices);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::Device::fromHandle(hDevice)->getSubDevices(pCount, phSubdevices));

exit:
    trace_zeDeviceGetSubDevices(ret, hDevice, pCount, phSubdevices);
    return ret;
}

ze_result_t zeDeviceGetProperties(ze_device_handle_t hDevice,
                                  ze_device_properties_t *pDeviceProperties) {
    trace_zeDeviceGetProperties(hDevice, pDeviceProperties);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::Device::fromHandle(hDevice)->getProperties(pDeviceProperties));

exit:
    trace_zeDeviceGetProperties(ret, hDevice, pDeviceProperties);
    return ret;
}

ze_result_t zeDeviceGetComputeProperties(ze_device_handle_t hDevice,
                                         ze_device_compute_properties_t *pComputeProperties) {
    trace_zeDeviceGetComputeProperties(hDevice, pComputeProperties);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        L0::Device::fromHandle(hDevice)->getDeviceComputeProperties(pComputeProperties));

exit:
    trace_zeDeviceGetComputeProperties(ret, hDevice, pComputeProperties);
    return ret;
}

ze_result_t zeDeviceGetModuleProperties(ze_device_handle_t hDevice,
                                        ze_device_module_properties_t *pKernelProperties) {
    trace_zeDeviceGetModuleProperties(hDevice, pKernelProperties);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeDeviceGetModuleProperties(ret, hDevice, pKernelProperties);
    return ret;
}

ze_result_t zeDeviceGetMemoryProperties(ze_device_handle_t hDevice,
                                        uint32_t *pCount,
                                        ze_device_memory_properties_t *pMemProperties) {
    trace_zeDeviceGetMemoryProperties(hDevice, pCount, pMemProperties);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        L0::Device::fromHandle(hDevice)->getMemoryProperties(pCount, pMemProperties));

exit:
    trace_zeDeviceGetMemoryProperties(ret, hDevice, pCount, pMemProperties);
    return ret;
}

ze_result_t zeDeviceGetExternalMemoryProperties(
    ze_device_handle_t hDevice,
    ze_device_external_memory_properties_t *pExternalMemoryProperties) {
    trace_zeDeviceGetExternalMemoryProperties(hDevice, pExternalMemoryProperties);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        L0::Device::fromHandle(hDevice)->getGetExternalMemoryProperties(pExternalMemoryProperties));

exit:
    trace_zeDeviceGetExternalMemoryProperties(ret, hDevice, pExternalMemoryProperties);
    return ret;
}

ze_result_t
zeDeviceGetMemoryAccessProperties(ze_device_handle_t hDevice,
                                  ze_device_memory_access_properties_t *pMemAccessProperties) {
    trace_zeDeviceGetMemoryAccessProperties(hDevice, pMemAccessProperties);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        L0::Device::fromHandle(hDevice)->getMemoryAccessProperties(pMemAccessProperties));

exit:
    trace_zeDeviceGetMemoryAccessProperties(ret, hDevice, pMemAccessProperties);
    return ret;
}

ze_result_t zeDeviceGetCacheProperties(ze_device_handle_t hDevice,
                                       uint32_t *pCount,
                                       ze_device_cache_properties_t *pCacheProperties) {
    trace_zeDeviceGetCacheProperties(hDevice, pCount, pCacheProperties);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeDeviceGetCacheProperties(ret, hDevice, pCount, pCacheProperties);
    return ret;
}

ze_result_t zeDeviceGetImageProperties(ze_device_handle_t hDevice,
                                       ze_device_image_properties_t *pImageProperties) {
    trace_zeDeviceGetImageProperties(hDevice, pImageProperties);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        L0::Device::fromHandle(hDevice)->getDeviceImageProperties(pImageProperties));

exit:
    trace_zeDeviceGetImageProperties(ret, hDevice, pImageProperties);
    return ret;
}

ze_result_t zeDeviceGetP2PProperties(ze_device_handle_t hDevice,
                                     ze_device_handle_t hPeerDevice,
                                     ze_device_p2p_properties_t *pP2PProperties) {
    trace_zeDeviceGetP2PProperties(hDevice, hPeerDevice, pP2PProperties);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        L0::Device::fromHandle(hDevice)->getP2PProperties(hPeerDevice, pP2PProperties));

exit:
    trace_zeDeviceGetP2PProperties(ret, hDevice, hPeerDevice, pP2PProperties);
    return ret;
}

ze_result_t zeDeviceCanAccessPeer(ze_device_handle_t hDevice,
                                  ze_device_handle_t hPeerDevice,
                                  ze_bool_t *value) {
    trace_zeDeviceCanAccessPeer(hDevice, hPeerDevice, value);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeDeviceCanAccessPeer(ret, hDevice, hPeerDevice, value);
    return ret;
}

ze_result_t zeDeviceGetCommandQueueGroupProperties(
    ze_device_handle_t hDevice,
    uint32_t *pCount,
    ze_command_queue_group_properties_t *pCommandQueueGroupProperties) {
    trace_zeDeviceGetCommandQueueGroupProperties(hDevice, pCount, pCommandQueueGroupProperties);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        L0::Device::fromHandle(hDevice)->getCommandQueueGroupProperties(
                            pCount,
                            pCommandQueueGroupProperties));

exit:
    trace_zeDeviceGetCommandQueueGroupProperties(ret,
                                                 hDevice,
                                                 pCount,
                                                 pCommandQueueGroupProperties);
    return ret;
}

ze_result_t zeDeviceGetStatus(ze_device_handle_t hDevice) {
    trace_zeDeviceGetStatus(hDevice);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::Device::fromHandle(hDevice)->getStatus());

exit:
    trace_zeDeviceGetStatus(ret, hDevice);
    return ret;
}

ze_result_t zeDeviceGetGlobalTimestamps(ze_device_handle_t hDevice,
                                        uint64_t *hostTimestamp,
                                        uint64_t *deviceTimestamp) {
    trace_zeDeviceGetGlobalTimestamps(hDevice, hostTimestamp, deviceTimestamp);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(
        ret,
        L0::Device::fromHandle(hDevice)->getGlobalTimestamps(hostTimestamp, deviceTimestamp));

exit:
    trace_zeDeviceGetGlobalTimestamps(ret, hDevice, hostTimestamp, deviceTimestamp);
    return ret;
}

ze_result_t zeDevicePciGetPropertiesExt(ze_device_handle_t hDevice,
                                        ze_pci_ext_properties_t *pPciProperties) {
    trace_zeDevicePciGetPropertiesExt(hDevice, pPciProperties);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::Device::fromHandle(hDevice)->getPciProperties(pPciProperties));

exit:
    trace_zeDevicePciGetPropertiesExt(ret, hDevice, pPciProperties);
    return ret;
}

} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeGetDeviceProcAddrTable(ze_api_version_t version,
                                                             ze_device_dditable_t *pDdiTable) {
    trace_zeGetDeviceProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

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
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetDeviceProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
