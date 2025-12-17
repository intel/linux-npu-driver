/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/api/trace/trace_ze_api.hpp"
#include "level_zero_driver/api/trace/trace_ze_api_ddi.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/context.hpp"
#include "level_zero_driver/source/device.hpp"
#include "umd_common.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>
#include <level_zero/ze_mem_import_system_memory_ext.h>

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
    trace_zeMemAllocShared(hContext, deviceDesc, hostDesc, size, alignment, hDevice, pptr);
    ze_structure_type_t extendedAllocType = ZE_STRUCTURE_TYPE_FORCE_UINT32;
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    if (deviceDesc == nullptr || hostDesc == nullptr ||
        (deviceDesc->pNext && !checkPtrAlignment<ze_structure_type_t *>(deviceDesc->pNext))) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (deviceDesc->pNext)
        extendedAllocType = *reinterpret_cast<const ze_structure_type_t *>(deviceDesc->pNext);

    /* For alloc exportable buffer single ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF flag is supported,
     * combination  flags not allowed
     */
    switch (extendedAllocType) {
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_DESC: {
        const ze_external_memory_export_desc_t *pExtMemDesc =
            reinterpret_cast<const ze_external_memory_export_desc_t *>(deviceDesc->pNext);

        if (pExtMemDesc->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF) {
            L0_HANDLE_EXCEPTION(ret,
                                L0::Context::fromHandle(hContext)->allocMemory(
                                    size,
                                    alignment,
                                    pptr,
                                    VPU::VPUBufferObject::Location::ExternalShared,
                                    flagToBufferObjectType(hostDesc->flags)));
            goto exit;
        }
        ret = ZE_RESULT_ERROR_INVALID_ENUMERATION;
        break;
    }
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_FD: {
        const ze_external_memory_import_fd_t *pImportMemDesc =
            reinterpret_cast<const ze_external_memory_import_fd_t *>(deviceDesc->pNext);
        if (pImportMemDesc->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF) {
            L0_HANDLE_EXCEPTION(ret,
                                L0::Context::fromHandle(hContext)->importMemory(
                                    VPU::VPUBufferObject::Location::ExternalShared,
                                    pImportMemDesc->fd,
                                    pptr));
            goto exit;
        }
        ret = ZE_RESULT_ERROR_INVALID_ENUMERATION;
        break;
    }
    default:
        L0_HANDLE_EXCEPTION(ret,
                            L0::Context::fromHandle(hContext)->allocMemory(
                                size,
                                alignment,
                                pptr,
                                VPU::VPUBufferObject::Location::Shared,
                                flagToBufferObjectType(hostDesc->flags)));
        break;
    }

exit:
    trace_zeMemAllocShared(ret, hContext, deviceDesc, hostDesc, size, alignment, hDevice, pptr);
    return ret;
}

ze_result_t zeMemAllocDevice(ze_context_handle_t hContext,
                             const ze_device_mem_alloc_desc_t *deviceDesc,
                             size_t size,
                             size_t alignment,
                             ze_device_handle_t hDevice,
                             void **pptr) {
    trace_zeMemAllocDevice(hContext, deviceDesc, size, alignment, hDevice, pptr);
    ze_structure_type_t extendedAllocType = ZE_STRUCTURE_TYPE_FORCE_UINT32;
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    if (deviceDesc == nullptr ||
        (deviceDesc->pNext && !checkPtrAlignment<ze_structure_type_t *>(deviceDesc->pNext))) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (deviceDesc->pNext)
        extendedAllocType = *reinterpret_cast<const ze_structure_type_t *>(deviceDesc->pNext);

    /* For alloc exportable buffer single ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF flag is supported,
     * combination  flags not allowed
     */
    switch (extendedAllocType) {
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_DESC: {
        const ze_external_memory_export_desc_t *pExtMemDesc =
            reinterpret_cast<const ze_external_memory_export_desc_t *>(deviceDesc->pNext);

        if (pExtMemDesc->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF) {
            L0_HANDLE_EXCEPTION(ret,
                                L0::Context::fromHandle(hContext)->allocMemory(
                                    size,
                                    alignment,
                                    pptr,
                                    VPU::VPUBufferObject::Location::ExternalDevice,
                                    VPU::VPUBufferObject::Type::WriteCombineDma));
            goto exit;
        }
        ret = ZE_RESULT_ERROR_INVALID_ENUMERATION;
        break;
    }
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_FD: {
        const ze_external_memory_import_fd_t *pImportMemDesc =
            reinterpret_cast<const ze_external_memory_import_fd_t *>(deviceDesc->pNext);
        if (pImportMemDesc->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF) {
            L0_HANDLE_EXCEPTION(ret,
                                L0::Context::fromHandle(hContext)->importMemory(
                                    VPU::VPUBufferObject::Location::ExternalDevice,
                                    pImportMemDesc->fd,
                                    pptr));
            goto exit;
        }
        ret = ZE_RESULT_ERROR_INVALID_ENUMERATION;
        break;
    }
    default:
        L0_HANDLE_EXCEPTION(ret,
                            L0::Context::fromHandle(hContext)->allocMemory(
                                size,
                                alignment,
                                pptr,
                                VPU::VPUBufferObject::Location::Device,
                                VPU::VPUBufferObject::Type::WriteCombineDma));
        break;
    }

exit:
    trace_zeMemAllocDevice(ret, hContext, deviceDesc, size, alignment, hDevice, pptr);
    return ret;
}

ze_result_t zeMemAllocHost(ze_context_handle_t hContext,
                           const ze_host_mem_alloc_desc_t *hostDesc,
                           size_t size,
                           size_t alignment,
                           void **pptr) {
    trace_zeMemAllocHost(hContext, hostDesc, size, alignment, pptr);
    uint32_t extendedAllocType = ZE_STRUCTURE_TYPE_FORCE_UINT32;
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    if (hostDesc == nullptr ||
        (hostDesc->pNext && !checkPtrAlignment<ze_structure_type_t *>(hostDesc->pNext))) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (hostDesc->pNext)
        extendedAllocType = *reinterpret_cast<const ze_structure_type_t *>(hostDesc->pNext);

    /* For alloc exportable buffer single ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF flag is supported,
     * combination  flags not allowed
     */
    switch (extendedAllocType) {
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_DESC: {
        const ze_external_memory_export_desc_t *pExtMemDesc =
            reinterpret_cast<const ze_external_memory_export_desc_t *>(hostDesc->pNext);

        if (pExtMemDesc->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF) {
            L0_HANDLE_EXCEPTION(ret,
                                L0::Context::fromHandle(hContext)->allocMemory(
                                    size,
                                    alignment,
                                    pptr,
                                    VPU::VPUBufferObject::Location::ExternalHost,
                                    flagToBufferObjectType(hostDesc->flags)));
            goto exit;
        }
        ret = ZE_RESULT_ERROR_INVALID_ENUMERATION;
        break;
    }
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_FD: {
        const ze_external_memory_import_fd_t *pImportMemDesc =
            reinterpret_cast<const ze_external_memory_import_fd_t *>(hostDesc->pNext);
        if (pImportMemDesc->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF) {
            L0_HANDLE_EXCEPTION(ret,
                                L0::Context::fromHandle(hContext)->importMemory(
                                    VPU::VPUBufferObject::Location::ExternalHost,
                                    pImportMemDesc->fd,
                                    pptr));
            goto exit;
        }
        ret = ZE_RESULT_ERROR_INVALID_ENUMERATION;
        break;
    }
    case ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_SYSTEM_MEMORY: {
        const ze_external_memory_import_system_memory_t *pImportMemDesc =
            reinterpret_cast<const ze_external_memory_import_system_memory_t *>(hostDesc->pNext);
        L0_HANDLE_EXCEPTION(
            ret,
            L0::Context::fromHandle(hContext)->importUserPtr(pImportMemDesc->pSystemMemory,
                                                             pImportMemDesc->size,
                                                             hostDesc->flags,
                                                             pptr));
        break;
    }
    default:
        L0_HANDLE_EXCEPTION(ret,
                            L0::Context::fromHandle(hContext)->allocMemory(
                                size,
                                alignment,
                                pptr,
                                VPU::VPUBufferObject::Location::Host,
                                flagToBufferObjectType(hostDesc->flags)));
        break;
    }

exit:
    trace_zeMemAllocHost(ret, hContext, hostDesc, size, alignment, pptr);
    return ret;
}

ze_result_t zeMemFree(ze_context_handle_t hContext, void *ptr) {
    trace_zeMemFree(hContext, ptr);
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::Context::fromHandle(hContext)->freeMem(ptr));

exit:
    trace_zeMemFree(ret, hContext, ptr);
    return ret;
}

ze_result_t zeMemGetAllocProperties(ze_context_handle_t hContext,
                                    const void *ptr,
                                    ze_memory_allocation_properties_t *pMemAllocProperties,
                                    ze_device_handle_t *phDevice) {
    trace_zeMemGetAllocProperties(hContext, ptr, pMemAllocProperties, phDevice);
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        L0::Context::fromHandle(hContext)
                            ->getMemAllocProperties(ptr, pMemAllocProperties, phDevice));

exit:
    trace_zeMemGetAllocProperties(ret, hContext, ptr, pMemAllocProperties, phDevice);
    return ret;
}

ze_result_t
zeMemGetAddressRange(ze_context_handle_t hContext, const void *ptr, void **pBase, size_t *pSize) {
    trace_zeMemGetAddressRange(hContext, ptr, pBase, pSize);
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        L0::Context::fromHandle(hContext)->getMemAddressRange(ptr, pBase, pSize));

exit:
    trace_zeMemGetAddressRange(ret, hContext, ptr, pBase, pSize);
    return ret;
}

ze_result_t
zeMemGetIpcHandle(ze_context_handle_t hContext, const void *ptr, ze_ipc_mem_handle_t *pIpcHandle) {
    trace_zeMemGetIpcHandle(hContext, ptr, pIpcHandle);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeMemGetIpcHandle(ret, hContext, ptr, pIpcHandle);
    return ret;
}

ze_result_t zeMemOpenIpcHandle(ze_context_handle_t hContext,
                               ze_device_handle_t hDevice,
                               ze_ipc_mem_handle_t handle,
                               ze_ipc_memory_flags_t flags,
                               void **pptr) {
    trace_zeMemOpenIpcHandle(hContext, hDevice, handle, flags, pptr);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeMemOpenIpcHandle(ret, hContext, hDevice, handle, flags, pptr);
    return ret;
}

ze_result_t zeMemCloseIpcHandle(ze_context_handle_t hContext, const void *ptr) {
    trace_zeMemCloseIpcHandle(hContext, ptr);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeMemCloseIpcHandle(ret, hContext, ptr);
    return ret;
}
} // namespace L0

extern "C" {
ZE_DLLEXPORT ze_result_t ZE_APICALL zeGetMemProcAddrTable(ze_api_version_t version,
                                                          ze_mem_dditable_t *pDdiTable) {
    trace_zeGetMemProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnAllocShared = L0::zeMemAllocShared;
    pDdiTable->pfnAllocDevice = L0::zeMemAllocDevice;
    pDdiTable->pfnAllocHost = L0::zeMemAllocHost;
    pDdiTable->pfnFree = L0::zeMemFree;
    pDdiTable->pfnGetAllocProperties = L0::zeMemGetAllocProperties;
    pDdiTable->pfnGetAddressRange = L0::zeMemGetAddressRange;
    pDdiTable->pfnGetIpcHandle = L0::zeMemGetIpcHandle;
    pDdiTable->pfnOpenIpcHandle = L0::zeMemOpenIpcHandle;
    pDdiTable->pfnCloseIpcHandle = L0::zeMemCloseIpcHandle;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetMemProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
