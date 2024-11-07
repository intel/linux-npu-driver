/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>

#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "umd_common.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

namespace L0 {

static VPU::VPUBufferObject::Type flagToBufferObjectType(ze_host_mem_alloc_flags_t flag) {
    switch (flag) {
    case ZE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED:
        return VPU::VPUBufferObject::Type::CachedDma;
    case ZE_HOST_MEM_ALLOC_FLAG_BIAS_UNCACHED:
        return VPU::VPUBufferObject::Type::UncachedDma;
    case ZE_HOST_MEM_ALLOC_FLAG_BIAS_WRITE_COMBINED:
        return VPU::VPUBufferObject::Type::WriteCombineDma;
    };
    return VPU::VPUBufferObject::Type::CachedDma;
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
                VPU::VPUBufferObject::Type::WriteCombineDma));
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
            VPU::VPUBufferObject::Type::WriteCombineDma));
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
ZE_DLLEXPORT ze_result_t ZE_APICALL zeGetMemProcAddrTable(ze_api_version_t version,
                                                          ze_mem_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnAllocShared = L0::zeMemAllocShared;
    pDdiTable->pfnAllocDevice = L0::zeMemAllocDevice;
    pDdiTable->pfnAllocHost = L0::zeMemAllocHost;
    pDdiTable->pfnFree = L0::zeMemFree;
    pDdiTable->pfnGetAllocProperties = L0::zeMemGetAllocProperties;
    pDdiTable->pfnGetAddressRange = L0::zeMemGetAddressRange;
    pDdiTable->pfnGetIpcHandle = L0::zeMemGetIpcHandle;
    pDdiTable->pfnOpenIpcHandle = L0::zeMemOpenIpcHandle;
    pDdiTable->pfnCloseIpcHandle = L0::zeMemCloseIpcHandle;
    return ZE_RESULT_SUCCESS;
}
}
