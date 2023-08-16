/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <boost/safe_numerics/safe_integer.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <string.h>

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
    , handle(handle) {}

VPUBufferObject::~VPUBufferObject() {
    if (drvApi.unmap(basePtr, allocSize) != 0) {
        LOG_E("Failed to unmap handle %d", handle);
    }

    if (drvApi.closeBuffer(handle) != 0) {
        LOG_E("Failed to close handle %d", handle);
    }
}

std::unique_ptr<VPUBufferObject>
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

    ptr = drvApi.mmap(size, offset);
    if (ptr == nullptr) {
        LOG_E("Failed to mmap the created buffer");
        drvApi.closeBuffer(handle);
        return nullptr;
    }

    return std::make_unique<VPUBufferObject>(drvApi, type, range, ptr, size, handle, vpuAddr);
}

bool VPUBufferObject::copyToBuffer(const void *data, size_t size, uint64_t offset) {
    if (offset > allocSize) {
        LOG_E("Invalid offset value");
        return false;
    }

    uint8_t *dstPtr = basePtr + offset;
    size_t dstMax = allocSize - offset;

    if (data == nullptr || size == 0 || dstMax == 0 || size > dstMax) {
        LOG_E("Invalid arguments. data(%p) size(%ld) dstMax(%ld)", data, size, dstMax);
        return false;
    }

    memcpy(dstPtr, data, size);
    return true;
}

bool VPUBufferObject::fillBuffer(const void *pattern, size_t patternSize) {
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

} // namespace VPU
