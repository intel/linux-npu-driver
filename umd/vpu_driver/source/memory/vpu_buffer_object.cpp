/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include "umd_common.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/stats.hpp"

#include <algorithm>
#include <atomic>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <utility>

namespace VPU {

VPUBufferObject::VPUBufferObject(const VPUDriverApi &drvApi,
                                 Location location,
                                 Type type,
                                 void *basePtr,
                                 size_t allocSize,
                                 uint32_t handle,
                                 uint64_t vpuAddr)
    : drvApi(drvApi)
    , location(location)
    , type(type)
    , basePtr(static_cast<uint8_t *>(basePtr))
    , allocSize(allocSize)
    , vpuAddr(vpuAddr)
    , handle(handle) {
    static std::atomic<uint64_t> counter = 0;
    id = ++counter;
}

VPUBufferObject::~VPUBufferObject() {
    if (basePtr != nullptr && drvApi.unmap(basePtr, allocSize) != 0) {
        LOG_E("Failed to unmap handle %d", handle);
    }

    /* The exportable buffers are managed by user space, driver never closes
     * handle without direct api call from user space.
     */
    if (static_cast<uint32_t>(location) & externalMemMask)
        return;

    if (MemoryStatistics::get().isEnabled()) {
        size_t pageSize = drvApi.getPageSize();
        MemoryStatistics::get().dec(location, ALIGN(allocSize, pageSize));
    }

    if (drvApi.closeBuffer(handle) != 0) {
        LOG_E("Failed to close handle %d", handle);
    }
}

std::shared_ptr<VPUBufferObject>
VPUBufferObject::create(const VPUDriverApi &drvApi, Location type, Type range, size_t size) {
    uint32_t handle = 0;
    uint64_t vpuAddr = 0;
    if (drvApi.createBuffer(size, static_cast<uint32_t>(range), handle, vpuAddr)) {
        LOG_E("Failed to allocate memory");
        return nullptr;
    }

    void *ptr = nullptr;
    uint64_t offset = 0;
    if (drvApi.getBufferInfo(handle, offset)) {
        LOG_E("Failed to get info about buffer");
        drvApi.closeBuffer(handle);
        return nullptr;
    }

    if (static_cast<int>(range) & DRM_IVPU_BO_MAPPABLE) {
        ptr = drvApi.mmap(size, safe_cast<off_t>(offset));
        if (ptr == nullptr) {
            LOG_E("Failed to mmap the created buffer");
            drvApi.closeBuffer(handle);
            return nullptr;
        }
    }

    if (MemoryStatistics::get().isEnabled()) {
        size_t pageSize = drvApi.getPageSize();
        MemoryStatistics::get().inc(type, ALIGN(size, pageSize));
    }

    return std::make_shared<VPUBufferObject>(drvApi,
                                             type,
                                             range,
                                             std::move(ptr),
                                             size,
                                             handle,
                                             vpuAddr);
}

std::shared_ptr<VPUBufferObject>
VPUBufferObject::importFromFd(const VPUDriverApi &drvApi, Location type, int32_t fd) {
    uint32_t handle = 0;

    int32_t err = drvApi.importBuffer(fd, 0, handle);
    if (err != 0) {
        LOG_E("Buffer import failed, system error code = %d", err);
        return nullptr;
    }
    /* Kernel doesn't track how many imports was done from single descriptor,
     * if there would be situation that user-space imports the same memory object twice
     * on the same DRM file description, the same GEM handle would be returned by both imports,
     * and user-space needs to ensure * &DRM_IOCTL_GEM_CLOSE is performed once only.
     * Here in case of fail we can't invoke close operation, this instance doesn't know how many
     * instances of handle is in use.
     */
    void *ptr = nullptr;
    uint64_t offset = 0, size = 0, vpuAddr = 0;
    uint32_t flags = 0;

    if (drvApi.getExtBufferInfo(handle, flags, vpuAddr, size, offset)) {
        LOG_E("Failed to get info about buffer");
        return nullptr;
    }
    /* Flags are ignored, range is set always to Type::ExternalMemory */
    Type range = Type::ImportedMemory;

    ptr = drvApi.mmap(size, safe_cast<off_t>(offset));
    if (ptr == nullptr) {
        LOG_E("Failed to mmap the imported buffer");
        return nullptr;
    }

    return std::make_shared<VPUBufferObject>(drvApi,
                                             type,
                                             range,
                                             std::move(ptr),
                                             size,
                                             handle,
                                             vpuAddr);
}

bool VPUBufferObject::copyToBuffer(const void *data, size_t size, size_t offset) {
    if (basePtr == nullptr) {
        LOG_E("Base pointer is null");
        return false;
    }

    if ((offset + size) > allocSize) {
        LOG_E("Copy out of buffer range");
        return false;
    }

    if (data == nullptr || size == 0) {
        LOG_E("Invalid arguments. data(%p) size(%ld)", data, size);
        return false;
    }

    uint8_t *dstPtr = basePtr + offset;
    memcpy(dstPtr, data, size);
    return true;
}

bool VPUBufferObject::fillBuffer(const void *pattern, size_t patternSize) {
    if (basePtr == nullptr) {
        LOG_E("Base pointer is null");
        return false;
    }

    if (!pattern) {
        LOG_E("Fill pattern undefined");
        return false;
    }

    switch (patternSize) {
    case sizeof(uint32_t): {
        uint32_t *start = reinterpret_cast<uint32_t *>(basePtr);
        uint32_t *end = start + (allocSize / sizeof(uint32_t));

        std::fill(start, end, *reinterpret_cast<const uint32_t *>(pattern));
        break;
    }

    case sizeof(uint16_t): {
        uint16_t *start = reinterpret_cast<uint16_t *>(basePtr);
        uint16_t *end = start + (allocSize / sizeof(uint16_t));

        std::fill(start, end, *reinterpret_cast<const uint16_t *>(pattern));
        break;
    }

    case sizeof(uint8_t):
        memset(reinterpret_cast<uint8_t *>(basePtr),
               *static_cast<const uint8_t *>(pattern),
               allocSize);
        break;
    default:
        LOG_E("Unsupported pattern size");
        return false;
    }
    return true;
}

bool VPUBufferObject::exportToFd(int32_t &fd) {
    if ((static_cast<uint32_t>(getLocation()) & VPUBufferObject::externalMemMask) !=
        VPUBufferObject::externalMemMask) {
        LOG_E("Buffer should be created as exportable");
        return false;
    }

    int32_t err = drvApi.exportBuffer(handle, O_RDWR, fd);
    if (err != 0) {
        LOG_E("Buffer export failed, system error code = %d", err);
        return false;
    }
    return true;
}

} // namespace VPU
