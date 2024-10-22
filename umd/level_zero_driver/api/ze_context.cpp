/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>

#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/include/l0_exception.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

namespace L0 {
ze_result_t zeContextCreate(ze_driver_handle_t hDriver,
                            const ze_context_desc_t *desc,
                            ze_context_handle_t *phContext) {
    if (hDriver == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::DriverHandle::fromHandle(hDriver)->createContext(desc, phContext));
}

ze_result_t zeContextDestroy(ze_context_handle_t hContext) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)->destroy());
}

ze_result_t zeContextGetStatus(ze_context_handle_t hContext) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)->getStatus());
}

ze_result_t
zeVirtualMemReserve(ze_context_handle_t hContext, const void *pStart, size_t size, void **pptr) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeVirtualMemFree(ze_context_handle_t hContext, const void *ptr, size_t size) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeVirtualMemQueryPageSize(ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      size_t size,
                                      size_t *pagesize) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zePhysicalMemCreate(ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                ze_physical_mem_desc_t *desc,
                                ze_physical_mem_handle_t *phPhysicalMemory) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zePhysicalMemDestroy(ze_context_handle_t hContext,
                                 ze_physical_mem_handle_t hPhysicalMemory) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeVirtualMemMap(ze_context_handle_t hContext,
                            const void *ptr,
                            size_t size,
                            ze_physical_mem_handle_t hPhysicalMemory,
                            size_t offset,
                            ze_memory_access_attribute_t access) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeVirtualMemUnmap(ze_context_handle_t hContext, const void *ptr, size_t size) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeVirtualMemSetAccessAttribute(ze_context_handle_t hContext,
                                           const void *ptr,
                                           size_t size,
                                           ze_memory_access_attribute_t access) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeVirtualMemGetAccessAttribute(ze_context_handle_t hContext,
                                           const void *ptr,
                                           size_t size,
                                           ze_memory_access_attribute_t *access,
                                           size_t *outSize) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeContextSystemBarrier(ze_context_handle_t hContext, ze_device_handle_t hDevice) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeContextMakeMemoryResident(ze_context_handle_t hContext,
                                        ze_device_handle_t hDevice,
                                        void *ptr,
                                        size_t size) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeContextEvictMemory(ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 void *ptr,
                                 size_t size) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeContextMakeImageResident(ze_context_handle_t hContext,
                                       ze_device_handle_t hDevice,
                                       ze_image_handle_t hImage) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeContextEvictImage(ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                ze_image_handle_t hImage) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}
} // namespace L0

extern "C" {
ZE_DLLEXPORT ze_result_t ZE_APICALL zeGetContextProcAddrTable(ze_api_version_t version,
                                                              ze_context_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnCreate = L0::zeContextCreate;
    pDdiTable->pfnDestroy = L0::zeContextDestroy;
    pDdiTable->pfnGetStatus = L0::zeContextGetStatus;
    pDdiTable->pfnSystemBarrier = L0::zeContextSystemBarrier;
    pDdiTable->pfnMakeMemoryResident = L0::zeContextMakeMemoryResident;
    pDdiTable->pfnEvictMemory = L0::zeContextEvictMemory;
    pDdiTable->pfnMakeImageResident = L0::zeContextMakeImageResident;
    pDdiTable->pfnEvictImage = L0::zeContextEvictImage;
    return ZE_RESULT_SUCCESS;
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zeGetPhysicalMemProcAddrTable(ze_api_version_t version, ze_physical_mem_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnCreate = L0::zePhysicalMemCreate;
    pDdiTable->pfnDestroy = L0::zePhysicalMemDestroy;
    return ZE_RESULT_SUCCESS;
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zeGetVirtualMemProcAddrTable(ze_api_version_t version, ze_virtual_mem_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnReserve = L0::zeVirtualMemReserve;
    pDdiTable->pfnFree = L0::zeVirtualMemFree;
    pDdiTable->pfnQueryPageSize = L0::zeVirtualMemQueryPageSize;
    pDdiTable->pfnMap = L0::zeVirtualMemMap;
    pDdiTable->pfnUnmap = L0::zeVirtualMemUnmap;
    pDdiTable->pfnSetAccessAttribute = L0::zeVirtualMemSetAccessAttribute;
    pDdiTable->pfnGetAccessAttribute = L0::zeVirtualMemGetAccessAttribute;
    return ZE_RESULT_SUCCESS;
}
}
