/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include <level_zero/ze_api.h>

namespace L0 {

static VPU::VPUBufferObject::Type flagToBufferObjectType(ze_host_mem_alloc_flags_t flag) {
    // TODO: Fallback to shave range to fix incorrect address in Dma tasks for kernels (EISW-108894)
    switch (flag) {
    case ZE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED:
        return VPU::VPUBufferObject::Type::CachedShave;
    case ZE_HOST_MEM_ALLOC_FLAG_BIAS_UNCACHED:
        return VPU::VPUBufferObject::Type::UncachedShave;
    case ZE_HOST_MEM_ALLOC_FLAG_BIAS_WRITE_COMBINED:
        return VPU::VPUBufferObject::Type::WriteCombineShave;
    };
    return VPU::VPUBufferObject::Type::CachedShave;
}

ze_result_t zeMemAllocShared(ze_context_handle_t hContext,
                             const ze_device_mem_alloc_desc_t *deviceDesc,
                             const ze_host_mem_alloc_desc_t *hostDesc,
                             size_t size,
                             size_t alignment,
                             ze_device_handle_t hDevice,
                             void **pptr) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    if (deviceDesc == nullptr || hostDesc == nullptr ||
        (deviceDesc->pNext && !checkPtrAlignment<ze_structure_type_t *>(deviceDesc->pNext))) {
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    ze_structure_type_t extendedAllocType =
        deviceDesc->pNext ? *reinterpret_cast<const ze_structure_type_t *>(deviceDesc->pNext)
                          : ZE_STRUCTURE_TYPE_FORCE_UINT32;

    /* For alloc exportable buffer single ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF flag is supported,
     * combination  flags not allowed
     */
    switch (extendedAllocType) {
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_DESC: {
        const ze_external_memory_export_desc_t *pExtMemDesc =
            reinterpret_cast<const ze_external_memory_export_desc_t *>(deviceDesc->pNext);

        if (pExtMemDesc->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF) {
            L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)->allocMemory(
                size,
                alignment,
                pptr,
                VPU::VPUBufferObject::Location::ExternalShared,
                flagToBufferObjectType(hostDesc->flags)));
        }
        return ZE_RESULT_ERROR_INVALID_ENUMERATION;
    }
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_FD: {
        const ze_external_memory_import_fd_t *pImportMemDesc =
            reinterpret_cast<const ze_external_memory_import_fd_t *>(deviceDesc->pNext);
        if (pImportMemDesc->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF)
            L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)->importMemory(
                VPU::VPUBufferObject::Location::ExternalShared,
                pImportMemDesc->fd,
                pptr));
        return ZE_RESULT_ERROR_INVALID_ENUMERATION;
    }
    default:
        L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)->allocMemory(
            size,
            alignment,
            pptr,
            VPU::VPUBufferObject::Location::Shared,
            flagToBufferObjectType(hostDesc->flags)));
    }
}

ze_result_t zeMemAllocDevice(ze_context_handle_t hContext,
                             const ze_device_mem_alloc_desc_t *deviceDesc,
                             size_t size,
                             size_t alignment,
                             ze_device_handle_t hDevice,
                             void **pptr) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    if (deviceDesc == nullptr ||
        (deviceDesc->pNext && !checkPtrAlignment<ze_structure_type_t *>(deviceDesc->pNext))) {
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    ze_structure_type_t extendedAllocType =
        deviceDesc->pNext ? *reinterpret_cast<const ze_structure_type_t *>(deviceDesc->pNext)
                          : ZE_STRUCTURE_TYPE_FORCE_UINT32;

    /* For alloc exportable buffer single ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF flag is supported,
     * combination  flags not allowed
     */
    switch (extendedAllocType) {
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_DESC: {
        const ze_external_memory_export_desc_t *pExtMemDesc =
            reinterpret_cast<const ze_external_memory_export_desc_t *>(deviceDesc->pNext);

        if (pExtMemDesc->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF) {
            L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)->allocMemory(
                size,
                alignment,
                pptr,
                VPU::VPUBufferObject::Location::ExternalDevice,
                VPU::VPUBufferObject::Type::WriteCombineShave));
        }
        return ZE_RESULT_ERROR_INVALID_ENUMERATION;
    }
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_FD: {
        const ze_external_memory_import_fd_t *pImportMemDesc =
            reinterpret_cast<const ze_external_memory_import_fd_t *>(deviceDesc->pNext);
        if (pImportMemDesc->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF) {
            L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)->importMemory(
                VPU::VPUBufferObject::Location::ExternalDevice,
                pImportMemDesc->fd,
                pptr));
        }
        return ZE_RESULT_ERROR_INVALID_ENUMERATION;
    }
    default:
        L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)->allocMemory(
            size,
            alignment,
            pptr,
            VPU::VPUBufferObject::Location::Device,
            VPU::VPUBufferObject::Type::WriteCombineShave));
    }
}

ze_result_t zeMemAllocHost(ze_context_handle_t hContext,
                           const ze_host_mem_alloc_desc_t *hostDesc,
                           size_t size,
                           size_t alignment,
                           void **pptr) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    if (hostDesc == nullptr ||
        (hostDesc->pNext && !checkPtrAlignment<ze_structure_type_t *>(hostDesc->pNext))) {
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    ze_structure_type_t extendedAllocType =
        hostDesc->pNext ? *reinterpret_cast<const ze_structure_type_t *>(hostDesc->pNext)
                        : ZE_STRUCTURE_TYPE_FORCE_UINT32;

    /* For alloc exportable buffer single ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF flag is supported,
     * combination  flags not allowed
     */
    switch (extendedAllocType) {
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_DESC: {
        const ze_external_memory_export_desc_t *pExtMemDesc =
            reinterpret_cast<const ze_external_memory_export_desc_t *>(hostDesc->pNext);

        if (pExtMemDesc->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF) {
            L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)->allocMemory(
                size,
                alignment,
                pptr,
                VPU::VPUBufferObject::Location::ExternalHost,
                flagToBufferObjectType(hostDesc->flags)));
        }
        return ZE_RESULT_ERROR_INVALID_ENUMERATION;
    }
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_FD: {
        const ze_external_memory_import_fd_t *pImportMemDesc =
            reinterpret_cast<const ze_external_memory_import_fd_t *>(hostDesc->pNext);
        if (pImportMemDesc->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF)
            L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)->importMemory(
                VPU::VPUBufferObject::Location::ExternalHost,
                pImportMemDesc->fd,
                pptr));
        return ZE_RESULT_ERROR_INVALID_ENUMERATION;
    }
    default:
        L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)->allocMemory(
            size,
            alignment,
            pptr,
            VPU::VPUBufferObject::Location::Host,
            flagToBufferObjectType(hostDesc->flags)));
    }
}

ze_result_t zeMemFree(ze_context_handle_t hContext, void *ptr) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)->freeMem(ptr));
}

ze_result_t zeMemGetAllocProperties(ze_context_handle_t hContext,
                                    const void *ptr,
                                    ze_memory_allocation_properties_t *pMemAllocProperties,
                                    ze_device_handle_t *phDevice) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Context::fromHandle(hContext)
                                       ->getMemAllocProperties(ptr, pMemAllocProperties, phDevice));
}

ze_result_t
zeMemGetAddressRange(ze_context_handle_t hContext, const void *ptr, void **pBase, size_t *pSize) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Context::fromHandle(hContext)->getMemAddressRange(ptr, pBase, pSize));
}

ze_result_t
zeMemGetIpcHandle(ze_context_handle_t hContext, const void *ptr, ze_ipc_mem_handle_t *pIpcHandle) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeMemOpenIpcHandle(ze_context_handle_t hContext,
                               ze_device_handle_t hDevice,
                               ze_ipc_mem_handle_t handle,
                               ze_ipc_memory_flags_t flags,
                               void **pptr) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeMemCloseIpcHandle(ze_context_handle_t hContext, const void *ptr) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeMemAllocShared(ze_context_handle_t hContext,
                                                     const ze_device_mem_alloc_desc_t *deviceDesc,
                                                     const ze_host_mem_alloc_desc_t *hostDesc,
                                                     size_t size,
                                                     size_t alignment,
                                                     ze_device_handle_t hDevice,
                                                     void **pptr) {
    return L0::zeMemAllocShared(hContext, deviceDesc, hostDesc, size, alignment, hDevice, pptr);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeMemAllocDevice(ze_context_handle_t hContext,
                                                     const ze_device_mem_alloc_desc_t *deviceDesc,
                                                     size_t size,
                                                     size_t alignment,
                                                     ze_device_handle_t hDevice,
                                                     void **pptr) {
    return L0::zeMemAllocDevice(hContext, deviceDesc, size, alignment, hDevice, pptr);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeMemAllocHost(ze_context_handle_t hContext,
                                                   const ze_host_mem_alloc_desc_t *hostDesc,
                                                   size_t size,
                                                   size_t alignment,
                                                   void **pptr) {
    return L0::zeMemAllocHost(hContext, hostDesc, size, alignment, pptr);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeMemFree(ze_context_handle_t hContext, void *ptr) {
    return L0::zeMemFree(hContext, ptr);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeMemGetAllocProperties(ze_context_handle_t hContext,
                        const void *ptr,
                        ze_memory_allocation_properties_t *pMemAllocProperties,
                        ze_device_handle_t *phDevice) {
    return L0::zeMemGetAllocProperties(hContext, ptr, pMemAllocProperties, phDevice);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeMemGetAddressRange(ze_context_handle_t hContext,
                                                         const void *ptr,
                                                         void **pBase,
                                                         size_t *pSize) {
    return L0::zeMemGetAddressRange(hContext, ptr, pBase, pSize);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeMemGetIpcHandle(ze_context_handle_t hContext,
                                                      const void *ptr,
                                                      ze_ipc_mem_handle_t *pIpcHandle) {
    return L0::zeMemGetIpcHandle(hContext, ptr, pIpcHandle);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeMemOpenIpcHandle(ze_context_handle_t hContext,
                                                       ze_device_handle_t hDevice,
                                                       ze_ipc_mem_handle_t handle,
                                                       ze_ipc_memory_flags_t flags,
                                                       void **pptr) {
    return L0::zeMemOpenIpcHandle(hContext, hDevice, handle, flags, pptr);
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeMemCloseIpcHandle(ze_context_handle_t hContext,
                                                        const void *ptr) {
    return L0::zeMemCloseIpcHandle(hContext, ptr);
}
} // extern "C"
