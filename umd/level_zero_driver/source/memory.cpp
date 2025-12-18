/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "context.hpp"
#include "level_zero/ze_api.h"
#include "umd_common.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <memory>

namespace L0 {

ze_result_t Context::checkMemInputs(VPU::VPUBufferObject::Location location,
                                    size_t size,
                                    size_t alignment,
                                    void **ptr) {
    switch (location) {
    case VPU::VPUBufferObject::Location::ExternalHost:
    case VPU::VPUBufferObject::Location::ExternalDevice:
    case VPU::VPUBufferObject::Location::ExternalShared:
        if (!getDeviceContext()->getDeviceCapabilities().primeBuffersCapability)
            return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
        break;
    default:
        break;
    }

    if (ptr == nullptr) {
        LOG_E("Invalid pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (size == 0) {
        LOG_E("Invalid size value");
        return ZE_RESULT_ERROR_UNSUPPORTED_SIZE;
    }

    // Checking that alignment is to power 2
    // Note: Alignment value not used in VPU implementation
    if ((alignment & (alignment - 1)) != 0)
        return ZE_RESULT_ERROR_UNSUPPORTED_ALIGNMENT;

    if (this->driverHandle == nullptr)
        return ZE_RESULT_ERROR_UNINITIALIZED;

    return ZE_RESULT_SUCCESS;
}

ze_result_t Context::allocMemory(size_t size,
                                 size_t alignment,
                                 void **ptr,
                                 VPU::VPUBufferObject::Location location,
                                 VPU::VPUBufferObject::Type type) {
    ze_result_t ret = checkMemInputs(location, size, alignment, ptr);
    if (ret != ZE_RESULT_SUCCESS)
        return ret;

    *ptr = ctx->createMemAlloc(size, type, location);

    if (*ptr == nullptr) {
        LOG_E("Failed to allocate device memory");
        return ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t Context::importMemory(VPU::VPUBufferObject::Location type, int32_t fd, void **ptr) {
    if (!getDeviceContext()->getDeviceCapabilities().primeBuffersCapability)
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    auto bo = ctx->importBufferObject(type, fd);
    if (bo == nullptr) {
        LOG_E("Failed to import buffer");
        return ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
    }
    *ptr = bo->getBasePointer();
    return ZE_RESULT_SUCCESS;
}

ze_result_t
Context::importUserPtr(void *userPtr, size_t size, ze_host_mem_alloc_flags_t flags, void **ptr) {
    if (userPtr == nullptr || ptr == nullptr) {
        LOG_E("Passed nullptr as argument");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (size == 0) {
        LOG_E("Invalid size value");
        return ZE_RESULT_ERROR_UNSUPPORTED_SIZE;
    }
    if (!getDeviceContext()->getDeviceCapabilities().userPtrCapability) {
        LOG_E("Standard allocation import is not supported");
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
    }

    auto bo = ctx->findBufferObject(userPtr);
    if (bo != nullptr) {
        LOG_E("Pointer %p has already been imported", userPtr);
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    bool isReadOnly = (flags & ZE_HOST_MEM_ALLOC_FLAG_BIAS_WRITE_COMBINED) != 0;
    bo = ctx->createTrackedBufferObjectFromUserPtr(userPtr, size, isReadOnly);
    if (bo == nullptr) {
        LOG_E("Failed to create buffer object from user pointer");
        return ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    *ptr = userPtr;
    return ZE_RESULT_SUCCESS;
}

ze_result_t Context::freeMem(void *ptr) {
    if (!ctx->freeMemAlloc(ptr))
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    return ZE_RESULT_SUCCESS;
}

ze_result_t Context::getMemAllocProperties(const void *ptr,
                                           ze_memory_allocation_properties_t *pMemAllocProperties,
                                           ze_device_handle_t *phDevice) {
    if (ptr == nullptr || pMemAllocProperties == nullptr) {
        LOG_E("Passed nullptr as argument");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto bo = ctx->findBufferObject(ptr);
    if (bo == nullptr) {
        LOG(CONTEXT, "Pointer %p has not been allocated by Context %p", ptr, this);
        return ZE_RESULT_ERROR_NOT_AVAILABLE;
    }

    pMemAllocProperties->stype = ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES;

    switch (bo->getLocation()) {
    case VPU::VPUBufferObject::Location::Device:
    case VPU::VPUBufferObject::Location::ExternalDevice:
        pMemAllocProperties->type = ZE_MEMORY_TYPE_DEVICE;
        break;
    case VPU::VPUBufferObject::Location::Host:
    case VPU::VPUBufferObject::Location::ExternalHost:
    case VPU::VPUBufferObject::Location::UserPtr:
        pMemAllocProperties->type = ZE_MEMORY_TYPE_HOST;
        break;
    case VPU::VPUBufferObject::Location::Shared:
    case VPU::VPUBufferObject::Location::ExternalShared:
        pMemAllocProperties->type = ZE_MEMORY_TYPE_SHARED;
        break;
    default:
        pMemAllocProperties->type = ZE_MEMORY_TYPE_UNKNOWN;
    }

    pMemAllocProperties->id = bo->getId();
    pMemAllocProperties->pageSize = ctx->getDriverApi().getPageSize();

    if (pMemAllocProperties->pNext &&
        checkPtrAlignment<ze_external_memory_export_fd_t *>(pMemAllocProperties->pNext)) {
        ze_external_memory_export_fd_t *pExtAllocProps =
            reinterpret_cast<ze_external_memory_export_fd_t *>(pMemAllocProperties->pNext);

        if (pExtAllocProps->stype == ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_EXPORT_FD &&
            pExtAllocProps->flags == ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF) {
            if (!bo->exportToFd(pExtAllocProps->fd)) {
                return ZE_RESULT_ERROR_NOT_AVAILABLE;
            }
        }
    }
    return ZE_RESULT_SUCCESS;
}

ze_result_t Context::getMemAddressRange(const void *ptr, void **basePtr, size_t *pSize) {
    if (ptr == nullptr) {
        LOG_E("Passed nullptr as argument");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto bo = ctx->findBufferObject(ptr);
    if (bo == nullptr) {
        LOG(CONTEXT, "Pointer %p has not been allocated by Context %p", ptr, this);
        return ZE_RESULT_ERROR_NOT_AVAILABLE;
    }

    if (basePtr != nullptr) {
        *basePtr = reinterpret_cast<void *>(bo->getVPUAddr());
    }

    if (pSize != nullptr) {
        *pSize = bo->getAllocSize();
    }

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
