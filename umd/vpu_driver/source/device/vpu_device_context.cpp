/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/device/vpu_device_context.hpp"

#include "umd_common.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <exception>
#include <memory>
#include <uapi/drm/ivpu_accel.h>

namespace VPU {
struct VPUDescriptor;

VPUDeviceContext::VPUDeviceContext(std::unique_ptr<VPUDriverApi> drvApi, VPUHwInfo *info)
    : drvApi(std::move(drvApi))
    , hwInfo(info) {
    LOG(DEVICE, "VPUDeviceContext is created");
}

VPUBufferObject::Type convertDmaToShaveRange(VPUBufferObject::Type type) {
    switch (type) {
    case VPUBufferObject::Type::WriteCombineDma:
        return VPUBufferObject::Type::WriteCombineShave;
    case VPUBufferObject::Type::UncachedDma:
        return VPUBufferObject::Type::UncachedShave;
    case VPUBufferObject::Type::CachedDma:
        return VPUBufferObject::Type::CachedShave;
    default:
        break;
    }
    return type;
}

std::shared_ptr<VPUBufferObject>
VPUDeviceContext::importBufferObject(VPUBufferObject::Location type, int32_t fd) {
    auto bo = VPUBufferObject::importFromFd(*drvApi, type, fd);
    if (bo == nullptr) {
        LOG_E("Failed to import VPUBufferObject from file descriptor");
        return nullptr;
    }
    void *ptr = bo->getBasePointer();

    const std::lock_guard<std::mutex> lock(mtx);
    auto [it, success] = trackedBuffers.emplace(ptr, std::move(bo));
    if (!success) {
        LOG_E("Failed to add buffer object to trackedBuffers");
        return nullptr;
    }
    LOG(DEVICE, "Buffer object %p successfully imported and added to trackedBuffers", &it->second);
    return it->second;
}

std::shared_ptr<VPUBufferObject>
VPUDeviceContext::createBufferObject(size_t size,
                                     VPUBufferObject::Type type,
                                     VPUBufferObject::Location loc) {
    if (!hwInfo->dmaMemoryRangeCapability && (static_cast<uint32_t>(type) & DRM_IVPU_BO_DMA_MEM))
        type = convertDmaToShaveRange(type);

    auto bo = VPUBufferObject::create(*drvApi, loc, type, size);
    if (bo == nullptr) {
        LOG_E("Failed to create VPUBufferObject");
        return nullptr;
    }

    LOG(DEVICE,
        "Create BO: %p, cpu: %p, vpu: %#lx",
        bo.get(),
        bo->getBasePointer(),
        bo->getVPUAddr());

    const std::lock_guard<std::mutex> lock(mtx);
    auto [it, success] = trackedBuffers.emplace(bo->getBasePointer(), std::move(bo));
    if (!success) {
        LOG_E("Failed to add buffer object to trackedBuffers");
        return nullptr;
    }
    return it->second;
}

bool VPUDeviceContext::freeMemAlloc(void *ptr) {
    if (ptr == nullptr) {
        LOG_E("Pointer is nullptr");
        return false;
    }

    auto bo = findBufferObject(ptr);
    if (bo == nullptr || bo->getBasePointer() != ptr) {
        LOG_E("Pointer is not tracked or not a based pointer is passed");
        return false;
    }

    bo->allowDeleteExternalHandle();

    return freeMemAlloc(std::move(bo));
}

bool VPUDeviceContext::freeMemAlloc(std::shared_ptr<VPUBufferObject> bo) {
    if (bo == nullptr) {
        LOG_E("VPUBufferObject is nullptr");
        return false;
    }

    LOG(DEVICE,
        "Free BO: %p, cpu: %p, vpu: %#lx",
        bo.get(),
        bo->getBasePointer(),
        bo->getVPUAddr());

    const std::lock_guard<std::mutex> lock(mtx);
    if (trackedBuffers.erase(bo->getBasePointer()) == 0) {
        LOG_E("Failed to remove VPUBufferObject from trackedBuffers!");
        return false;
    }

    MemoryStatistics::get().snapshot();
    return true;
}

std::shared_ptr<VPUBufferObject> VPUDeviceContext::findBufferObject(const void *ptr) const {
    if (ptr == nullptr) {
        LOG_E("ptr passed is nullptr!");
        return nullptr;
    }

    const std::lock_guard<std::mutex> lock(mtx);
    auto it = trackedBuffers.lower_bound(ptr);
    if (it == trackedBuffers.end()) {
        LOG(DEVICE, "Could not find a pointer %p in VPUDeviceContext %p", ptr, this);
        return nullptr;
    }

    auto &bo = it->second;
    if (!bo->isInRange(ptr)) {
        LOG(DEVICE, "Pointer %p is not in the allocation size in VPUDeviceContext %p", ptr, this);
        return nullptr;
    }

    return bo;
}

std::shared_ptr<VPUBufferObject>
VPUDeviceContext::createUntrackedBufferObject(size_t size, VPUBufferObject::Type range) {
    if (size == 0) {
        LOG_E("Invalid size - %lu", size);
        return nullptr;
    }

    if (!hwInfo->dmaMemoryRangeCapability && (static_cast<uint32_t>(range) & DRM_IVPU_BO_DMA_MEM))
        range = convertDmaToShaveRange(range);

    auto bo = VPUBufferObject::create(*drvApi, VPUBufferObject::Location::Internal, range, size);
    if (bo == nullptr) {
        LOG_E("Failed to allocate shared memory, size = %lu, type = %i",
              size,
              static_cast<int>(range));
        return nullptr;
    }

    const std::lock_guard<std::mutex> lock(mtx);
    untrackedBuffers.emplace_back(bo);
    return bo;
}

size_t VPUDeviceContext::getPageAlignedSize(size_t reqSize) {
    size_t pageSize = drvApi->getPageSize();
    return ALIGN(reqSize, pageSize);
}

bool VPUDeviceContext::getCopyCommandDescriptor(uint64_t srcAddr,
                                                uint64_t dstAddr,
                                                size_t size,
                                                VPUDescriptor &desc) {
    if (hwInfo->getCopyCommand == nullptr) {
        LOG_E("Failed to get copy descriptor");
        return false;
    }

    return hwInfo->getCopyCommand(srcAddr, dstAddr, size, desc);
}

void VPUDeviceContext::printCopyDescriptor(void *desc, vpu_cmd_header_t *cmd) {
    if (hwInfo->printCopyDescriptor == nullptr) {
        LOG_W("Failed to print copy descriptor");
        return;
    }

    hwInfo->printCopyDescriptor(desc, cmd);
}

bool VPUDeviceContext::getUniqueInferenceId(uint64_t &inferenceId) {
    try {
        inferenceId = drvApi->getDeviceParam(DRM_IVPU_PARAM_UNIQUE_INFERENCE_ID);
    } catch (const std::exception &err) {
        LOG_E("Failed to get unique inference id, error: %s", err.what());
        return false;
    }
    return true;
}

} // namespace VPU
