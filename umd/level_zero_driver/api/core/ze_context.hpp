/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include <level_zero/ze_api.h>

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
ZE_APIEXPORT ze_result_t ZE_APICALL zeContextCreate(ze_driver_handle_t hDriver,
                                                    const ze_context_desc_t *desc,
                                                    ze_context_handle_t *phContext) {
    return L0::zeContextCreate(hDriver, desc, phContext);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeContextDestroy(ze_context_handle_t hContext) {
    return L0::zeContextDestroy(hContext);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeContextGetStatus(ze_context_handle_t hContext) {
    return L0::zeContextGetStatus(hContext);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeVirtualMemReserve(ze_context_handle_t hContext,
                                                        const void *pStart,
                                                        size_t size,
                                                        void **pptr) {
    return L0::zeVirtualMemReserve(hContext, pStart, size, pptr);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeVirtualMemFree(ze_context_handle_t hContext,
                                                     const void *ptr,
                                                     size_t size) {
    return L0::zeVirtualMemFree(hContext, ptr, size);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeVirtualMemQueryPageSize(ze_context_handle_t hContext,
                                                              ze_device_handle_t hDevice,
                                                              size_t size,
                                                              size_t *pagesize) {
    return L0::zeVirtualMemQueryPageSize(hContext, hDevice, size, pagesize);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zePhysicalMemCreate(ze_context_handle_t hContext,
                    ze_device_handle_t hDevice,
                    ze_physical_mem_desc_t *desc,
                    ze_physical_mem_handle_t *phPhysicalMemory) {
    return L0::zePhysicalMemCreate(hContext, hDevice, desc, phPhysicalMemory);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zePhysicalMemDestroy(ze_context_handle_t hContext,
                                                         ze_physical_mem_handle_t hPhysicalMemory) {
    return L0::zePhysicalMemDestroy(hContext, hPhysicalMemory);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeVirtualMemMap(ze_context_handle_t hContext,
                                                    const void *ptr,
                                                    size_t size,
                                                    ze_physical_mem_handle_t hPhysicalMemory,
                                                    size_t offset,
                                                    ze_memory_access_attribute_t access) {
    return L0::zeVirtualMemMap(hContext, ptr, size, hPhysicalMemory, offset, access);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeVirtualMemUnmap(ze_context_handle_t hContext,
                                                      const void *ptr,
                                                      size_t size) {
    return L0::zeVirtualMemUnmap(hContext, ptr, size);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeVirtualMemSetAccessAttribute(ze_context_handle_t hContext,
                               const void *ptr,
                               size_t size,
                               ze_memory_access_attribute_t access) {
    return L0::zeVirtualMemSetAccessAttribute(hContext, ptr, size, access);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeVirtualMemGetAccessAttribute(ze_context_handle_t hContext,
                               const void *ptr,
                               size_t size,
                               ze_memory_access_attribute_t *access,
                               size_t *outSize) {
    return L0::zeVirtualMemGetAccessAttribute(hContext, ptr, size, access, outSize);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeContextSystemBarrier(ze_context_handle_t hContext,
                                                           ze_device_handle_t hDevice) {
    return L0::zeContextSystemBarrier(hContext, hDevice);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeContextMakeMemoryResident(ze_context_handle_t hContext,
                                                                ze_device_handle_t hDevice,
                                                                void *ptr,
                                                                size_t size) {
    return L0::zeContextMakeMemoryResident(hContext, hDevice, ptr, size);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeContextEvictMemory(ze_context_handle_t hContext,
                                                         ze_device_handle_t hDevice,
                                                         void *ptr,
                                                         size_t size) {
    return L0::zeContextEvictMemory(hContext, hDevice, ptr, size);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeContextMakeImageResident(ze_context_handle_t hContext,
                                                               ze_device_handle_t hDevice,
                                                               ze_image_handle_t hImage) {
    return L0::zeContextMakeImageResident(hContext, hDevice, hImage);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeContextEvictImage(ze_context_handle_t hContext,
                                                        ze_device_handle_t hDevice,
                                                        ze_image_handle_t hImage) {
    return L0::zeContextEvictImage(hContext, hDevice, hImage);
}
} // extern "C"
