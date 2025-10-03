/*
 * Copyright (C) 2022-2025 Intel Corporation
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
        type = VPUBufferObject::convertDmaToShaveRange(type);

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
        range = VPUBufferObject::convertDmaToShaveRange(range);

    auto bo = VPUBufferObject::create(*drvApi, VPUBufferObject::Location::Internal, range, size);
    if (bo == nullptr) {
        LOG_E("Failed to allocate shared memory, size = %lu, type = %i",
              size,
              static_cast<int>(range));
        return nullptr;
    }

    const std::lock_guard<std::mutex> lock(mtx);
    untrackedBuffers.emplace_back(bo);

    MemoryStatistics::get().snapshot();
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

std::shared_ptr<VPUBufferObject> ScratchCacheFactory::acquire(VPUDeviceContext *ctx, size_t size) {
    if (size == 0) {
        return nullptr;
    }

    const std::lock_guard<std::mutex> lock(scratchMutex);
    std::shared_ptr<VPUBufferObject> hit = nullptr;
    for (auto &bo : scratchBuffers) {
        if (bo.use_count() != 1) {
            continue;
        }

        if (bo->getAllocSize() == size) {
            hit = bo;
            break;
        }

        if (bo->getAllocSize() > size) {
            if (hit && hit->getAllocSize() < bo->getAllocSize()) {
                continue;
            }

            hit = bo;
            continue;
        }
    }

    if (hit != nullptr) {
        LOG(CONTEXT,
            "Reusing scratch buffer: handle %u, size: %lu, requested size: %lu",
            hit->getHandle(),
            hit->getAllocSize(),
            size);
        return hit;
    }

    auto bo =
        ctx->createUntrackedBufferObject(size, VPUBufferObject::Type::WriteCombineDmaUnmappable);
    if (bo == nullptr) {
        LOG_E("Failed to allocate scratch buffer of size %lu", size);
        return nullptr;
    }

    scratchBuffers.emplace_back(bo);
    LOG(CONTEXT,
        "Allocated scratch buffer: handle %u, size: %lu, requested size: %lu",
        bo->getHandle(),
        bo->getAllocSize(),
        size);
    return bo;
}

void ScratchCacheFactory::prune(size_t size) {
    if (size == 0)
        return;

    const std::lock_guard<std::mutex> lock(scratchMutex);
    scratchBuffers.erase(std::remove_if(scratchBuffers.begin(),
                                        scratchBuffers.end(),
                                        [size](const std::shared_ptr<VPUBufferObject> &bo) {
                                            return bo.use_count() == 1 &&
                                                   bo->getAllocSize() <= size;
                                        }),
                         scratchBuffers.end());
}

std::shared_ptr<VPUBufferObject> PreemptionCacheFactory::acquire(VPUDeviceContext *ctx) {
    const auto size = ctx->getDeviceCapabilities().fwPreemptBufSize;
    if (size == 0) {
        LOG_E("Preemption buffer size is zero, cannot acquire preemption buffer");
        return nullptr;
    }

    const std::lock_guard<std::mutex> lock(preemptionMutex);
    for (auto &bo : preemptionBuffers) {
        if (bo.use_count() == 1) {
            LOG(CONTEXT,
                "Reusing preemption buffer: handle %u, size: %lu",
                bo->getHandle(),
                bo->getAllocSize());
            return bo;
        }
    }

    auto bo =
        ctx->createUntrackedBufferObject(size, VPUBufferObject::Type::WriteCombineFwUnmappable);
    if (!bo) {
        LOG_E("Failed to create new preemption buffer of size %lu", size);
        return nullptr;
    }

    preemptionBuffers.push_back(bo);
    LOG(CONTEXT,
        "Returning new preemption buffer: handle %u, size: %lu",
        bo->getHandle(),
        bo->getAllocSize());
    return bo;
}

void PreemptionCacheFactory::prune() {
    const std::lock_guard<std::mutex> lock(preemptionMutex);
    if (numQueues > 0)
        numQueues--;

    size_t numQueuesToRemove = preemptionBuffers.size() > numQueues
                                   ? preemptionBuffers.size() - numQueues
                                   : preemptionBuffers.size();
    preemptionBuffers.erase(
        std::remove_if(preemptionBuffers.begin(),
                       preemptionBuffers.end(),
                       [&numQueuesToRemove](const std::shared_ptr<VPUBufferObject> &bo) {
                           if (numQueuesToRemove > 0 && bo.use_count() == 1) {
                               numQueuesToRemove--;
                               return true;
                           }
                           return false;
                       }),
        preemptionBuffers.end());
    LOG(CONTEXT, "Pruned preemption buffers, remaining count: %zu", preemptionBuffers.size());
}

} // namespace VPU
