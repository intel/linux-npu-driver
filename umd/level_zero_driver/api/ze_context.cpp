/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>

#include "level_zero_driver/api/trace/trace_ze_api.hpp"
#include "level_zero_driver/api/trace/trace_ze_api_ddi.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/context.hpp"
#include "level_zero_driver/source/device.hpp"
#include "level_zero_driver/source/driver_handle.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

namespace L0 {
ze_result_t zeContextCreate(ze_driver_handle_t hDriver,
                            const ze_context_desc_t *desc,
                            ze_context_handle_t *phContext) {
    trace_zeContextCreate(hDriver, desc, phContext);
    ze_result_t ret;

    if (hDriver == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::DriverHandle::fromHandle(hDriver)->createContext(desc, phContext));

exit:
    trace_zeContextCreate(ret, hDriver, desc, phContext);
    return ret;
}

ze_result_t zeContextDestroy(ze_context_handle_t hContext) {
    trace_zeContextDestroy(hContext);
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::Context::fromHandle(hContext)->destroy());

exit:
    trace_zeContextDestroy(ret, hContext);
    return ret;
}

ze_result_t zeContextGetStatus(ze_context_handle_t hContext) {
    trace_zeContextGetStatus(hContext);
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::Context::fromHandle(hContext)->getStatus());

exit:
    trace_zeContextGetStatus(ret, hContext);
    return ret;
}

ze_result_t
zeVirtualMemReserve(ze_context_handle_t hContext, const void *pStart, size_t size, void **pptr) {
    trace_zeVirtualMemReserve(hContext, pStart, size, pptr);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeVirtualMemReserve(ret, hContext, pStart, size, pptr);
    return ret;
}

ze_result_t zeVirtualMemFree(ze_context_handle_t hContext, const void *ptr, size_t size) {
    trace_zeVirtualMemFree(hContext, ptr, size);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeVirtualMemFree(ret, hContext, ptr, size);
    return ret;
}

ze_result_t zeVirtualMemQueryPageSize(ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      size_t size,
                                      size_t *pagesize) {
    trace_zeVirtualMemQueryPageSize(hContext, hDevice, size, pagesize);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeVirtualMemQueryPageSize(ret, hContext, hDevice, size, pagesize);
    return ret;
}

ze_result_t zePhysicalMemCreate(ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                ze_physical_mem_desc_t *desc,
                                ze_physical_mem_handle_t *phPhysicalMemory) {
    trace_zePhysicalMemCreate(hContext, hDevice, desc, phPhysicalMemory);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zePhysicalMemCreate(ret, hContext, hDevice, desc, phPhysicalMemory);
    return ret;
}

ze_result_t zePhysicalMemDestroy(ze_context_handle_t hContext,
                                 ze_physical_mem_handle_t hPhysicalMemory) {
    trace_zePhysicalMemDestroy(hContext, hPhysicalMemory);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zePhysicalMemDestroy(ret, hContext, hPhysicalMemory);
    return ret;
}

ze_result_t zeVirtualMemMap(ze_context_handle_t hContext,
                            const void *ptr,
                            size_t size,
                            ze_physical_mem_handle_t hPhysicalMemory,
                            size_t offset,
                            ze_memory_access_attribute_t access) {
    trace_zeVirtualMemMap(hContext, ptr, size, hPhysicalMemory, offset, access);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeVirtualMemMap(ret, hContext, ptr, size, hPhysicalMemory, offset, access);
    return ret;
}

ze_result_t zeVirtualMemUnmap(ze_context_handle_t hContext, const void *ptr, size_t size) {
    trace_zeVirtualMemUnmap(hContext, ptr, size);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeVirtualMemUnmap(ret, hContext, ptr, size);
    return ret;
}

ze_result_t zeVirtualMemSetAccessAttribute(ze_context_handle_t hContext,
                                           const void *ptr,
                                           size_t size,
                                           ze_memory_access_attribute_t access) {
    trace_zeVirtualMemSetAccessAttribute(hContext, ptr, size, access);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeVirtualMemSetAccessAttribute(ret, hContext, ptr, size, access);
    return ret;
}

ze_result_t zeVirtualMemGetAccessAttribute(ze_context_handle_t hContext,
                                           const void *ptr,
                                           size_t size,
                                           ze_memory_access_attribute_t *access,
                                           size_t *outSize) {
    trace_zeVirtualMemGetAccessAttribute(hContext, ptr, size, access, outSize);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeVirtualMemGetAccessAttribute(ret, hContext, ptr, size, access, outSize);
    return ret;
}

ze_result_t zeContextSystemBarrier(ze_context_handle_t hContext, ze_device_handle_t hDevice) {
    trace_zeContextSystemBarrier(hContext, hDevice);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeContextSystemBarrier(ret, hContext, hDevice);
    return ret;
}

ze_result_t zeContextMakeMemoryResident(ze_context_handle_t hContext,
                                        ze_device_handle_t hDevice,
                                        void *ptr,
                                        size_t size) {
    trace_zeContextMakeMemoryResident(hContext, hDevice, ptr, size);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeContextMakeMemoryResident(ret, hContext, hDevice, ptr, size);
    return ret;
}

ze_result_t zeContextEvictMemory(ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 void *ptr,
                                 size_t size) {
    trace_zeContextEvictMemory(hContext, hDevice, ptr, size);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeContextEvictMemory(ret, hContext, hDevice, ptr, size);
    return ret;
}

ze_result_t zeContextMakeImageResident(ze_context_handle_t hContext,
                                       ze_device_handle_t hDevice,
                                       ze_image_handle_t hImage) {
    trace_zeContextMakeImageResident(hContext, hDevice, hImage);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeContextMakeImageResident(ret, hContext, hDevice, hImage);
    return ret;
}

ze_result_t zeContextEvictImage(ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                ze_image_handle_t hImage) {
    trace_zeContextEvictImage(hContext, hDevice, hImage);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeContextEvictImage(ret, hContext, hDevice, hImage);
    return ret;
}
} // namespace L0

extern "C" {
ZE_DLLEXPORT ze_result_t ZE_APICALL zeGetContextProcAddrTable(ze_api_version_t version,
                                                              ze_context_dditable_t *pDdiTable) {
    trace_zeGetContextProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnCreate = L0::zeContextCreate;
    pDdiTable->pfnDestroy = L0::zeContextDestroy;
    pDdiTable->pfnGetStatus = L0::zeContextGetStatus;
    pDdiTable->pfnSystemBarrier = L0::zeContextSystemBarrier;
    pDdiTable->pfnMakeMemoryResident = L0::zeContextMakeMemoryResident;
    pDdiTable->pfnEvictMemory = L0::zeContextEvictMemory;
    pDdiTable->pfnMakeImageResident = L0::zeContextMakeImageResident;
    pDdiTable->pfnEvictImage = L0::zeContextEvictImage;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetContextProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zeGetPhysicalMemProcAddrTable(ze_api_version_t version, ze_physical_mem_dditable_t *pDdiTable) {
    trace_zeGetPhysicalMemProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnCreate = L0::zePhysicalMemCreate;
    pDdiTable->pfnDestroy = L0::zePhysicalMemDestroy;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetPhysicalMemProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zeGetVirtualMemProcAddrTable(ze_api_version_t version, ze_virtual_mem_dditable_t *pDdiTable) {
    trace_zeGetVirtualMemProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnReserve = L0::zeVirtualMemReserve;
    pDdiTable->pfnFree = L0::zeVirtualMemFree;
    pDdiTable->pfnQueryPageSize = L0::zeVirtualMemQueryPageSize;
    pDdiTable->pfnMap = L0::zeVirtualMemMap;
    pDdiTable->pfnUnmap = L0::zeVirtualMemUnmap;
    pDdiTable->pfnSetAccessAttribute = L0::zeVirtualMemSetAccessAttribute;
    pDdiTable->pfnGetAccessAttribute = L0::zeVirtualMemGetAccessAttribute;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetVirtualMemProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
