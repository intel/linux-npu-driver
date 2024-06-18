/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/ze_api.h"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/context/context.hpp"

#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace L0 {

ze_result_t Context::checkMemInputs(size_t size, size_t alignment, void **ptr) {
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
    ze_result_t ret = checkMemInputs(size, alignment, ptr);
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
    VPU::VPUBufferObject *bo = ctx->importBufferObject(type, fd);
    if (bo == nullptr) {
        LOG_E("Failed to import buffer");
        return ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
    }
    *ptr = bo->getBasePointer();
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
        LOG_E("Given pointer is invalid to VPU");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto bo = ctx->findBuffer(ptr);
    if (bo == nullptr) {
        LOG_W("Given pointer is invalid to VPU");
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
        pMemAllocProperties->type = ZE_MEMORY_TYPE_HOST;
        break;
    case VPU::VPUBufferObject::Location::Shared:
    case VPU::VPUBufferObject::Location::ExternalShared:
        pMemAllocProperties->type = ZE_MEMORY_TYPE_SHARED;
        break;
    default:
        pMemAllocProperties->type = ZE_MEMORY_TYPE_UNKNOWN;
    }

    pMemAllocProperties->id = 0u; // No specific ID for allocated memory, set as 0
    pMemAllocProperties->pageSize = bo->getAllocSize();

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
        LOG_E("Given pointer is invalid to VPU");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto bo = ctx->findBuffer(ptr);
    if (bo == nullptr) {
        LOG_W("Given pointer is invalid to VPU");
        return ZE_RESULT_ERROR_NOT_AVAILABLE;
    }

    if (basePtr != nullptr) {
        *basePtr = bo->getBasePointer();
    } else {
        LOG_W("Input base address pointer is NULL");
    }

    if (pSize != nullptr) {
        *pSize = bo->getAllocSize();
    } else {
        LOG_W("Input size pointer is NULL");
    }

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
