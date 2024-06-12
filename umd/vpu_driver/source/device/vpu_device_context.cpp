/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"

#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <chrono>
#include <memory>
#include <string.h>
#include <sys/mman.h>
#include <thread>

namespace VPU {

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

VPUBufferObject *VPUDeviceContext::importBufferObject(VPUBufferObject::Location type, int32_t fd) {
    std::unique_ptr<VPUBufferObject> bo = VPUBufferObject::importFromFd(*drvApi, type, fd);
    if (bo == nullptr) {
        LOG_E("Failed to import VPUBufferObject from file descriptor");
        return nullptr;
    }
    void *ptr = bo->getBasePointer();

    const std::lock_guard<std::mutex> lock(mtx);
    auto [it, success] = trackedBuffers.try_emplace(ptr, std::move(bo));
    if (!success) {
        LOG_E("Failed to add buffer object to trackedBuffers");
        return nullptr;
    }
    LOG(DEVICE, "Buffer object %p successfully imported and added to trackedBuffers", &it->second);
    return it->second.get();
}

VPUBufferObject *VPUDeviceContext::createBufferObject(size_t size,
                                                      VPUBufferObject::Type type,
                                                      VPUBufferObject::Location loc) {
    if (!hwInfo->dmaMemoryRangeCapability && (static_cast<uint32_t>(type) & DRM_IVPU_BO_DMA_MEM))
        type = convertDmaToShaveRange(type);

    std::unique_ptr<VPUBufferObject> bo = VPUBufferObject::create(*drvApi, loc, type, size);
    if (bo == nullptr) {
        LOG_E("Failed to create VPUBufferObject");
        return nullptr;
    }

    void *ptr = bo->getBasePointer();
    if (ptr == nullptr) {
        LOG_E("Failed to received base pointer from new VPUBufferObject");
        return nullptr;
    }

    const std::lock_guard<std::mutex> lock(mtx);
    auto [it, success] = trackedBuffers.try_emplace(ptr, std::move(bo));
    if (!success) {
        LOG_E("Failed to add buffer object to trackedBuffers");
        return nullptr;
    }

    LOG(DEVICE, "Buffer object %p successfully added to trackedBuffers", &it->second);
    return it->second.get();
}

bool VPUDeviceContext::freeMemAlloc(void *ptr) {
    if (ptr == nullptr) {
        LOG_E("Pointer is nullptr");
        return false;
    }

    auto *bo = findBuffer(ptr);
    if (bo == nullptr || bo->getBasePointer() != ptr) {
        LOG_E("Pointer is not tracked or not a based pointer is passed");
        return false;
    }

    bo->allowDeleteExternalHandle();

    return freeMemAlloc(bo);
}

bool VPUDeviceContext::freeMemAlloc(VPUBufferObject *bo) {
    if (bo == nullptr) {
        LOG_E("VPUBufferObject is nullptr");
        return false;
    }

    const std::lock_guard<std::mutex> lock(mtx);
    if (trackedBuffers.erase(bo->getBasePointer()) == 0) {
        LOG_E("Failed to remove VPUBufferObject from trackedBuffers!");
        return false;
    }

    return true;
}

VPUBufferObject *VPUDeviceContext::findBuffer(const void *ptr) const {
    if (ptr == nullptr) {
        LOG_E("ptr passed is nullptr!");
        return nullptr;
    }

    const std::lock_guard<std::mutex> lock(mtx);
    auto it = trackedBuffers.lower_bound(ptr);
    if (it == trackedBuffers.end()) {
        LOG_E("Failed to find pointer %p in device context!", ptr);
        return nullptr;
    }

    auto &bo = it->second;
    if (!bo->isInRange(ptr)) {
        LOG_E("Pointer is not within the range");
        return nullptr;
    }

    LOG(DEVICE,
        "Pointer %p was found in device context(type: %d, range: %d).",
        ptr,
        static_cast<int>(bo->getLocation()),
        static_cast<int>(bo->getType()));
    return bo.get();
}

VPUBufferObject *VPUDeviceContext::createInternalBufferObject(size_t size,
                                                              VPUBufferObject::Type range) {
    if (size == 0) {
        LOG_E("Invalid size - %lu", size);
        return nullptr;
    }

    VPUBufferObject *bo = createBufferObject(size, range, VPUBufferObject::Location::Internal);
    if (bo == nullptr) {
        LOG_E("Failed to allocate shared memory, size = %lu, type = %i",
              size,
              static_cast<int>(range));
        return nullptr;
    }

    if (!(range == VPUBufferObject::Type::UncachedShave ||
          range == VPUBufferObject::Type::UncachedFw)) {
        memset(bo->getBasePointer(), 0, bo->getAllocSize());
    }

    return bo;
}

size_t VPUDeviceContext::getPageAlignedSize(size_t reqSize) {
    size_t pageSize = drvApi->getPageSize();
    return ALIGN(reqSize, pageSize);
}

uint64_t VPUDeviceContext::getBufferVPUAddress(const void *ptr) const {
    auto bo = findBuffer(ptr);
    if (bo == nullptr)
        return 0;

    uint64_t offset =
        reinterpret_cast<uint64_t>(ptr) - reinterpret_cast<uint64_t>(bo->getBasePointer());

    LOG(DEVICE, "CPU address %p mapped to VPU address %#lx", ptr, bo->getVPUAddr() + offset);

    return bo->getVPUAddr() + offset;
}

bool VPUDeviceContext::submitCommandBuffer(const VPUCommandBuffer *cmdBuffer) {
    drm_ivpu_submit execParam = {};
    execParam.buffers_ptr = reinterpret_cast<uint64_t>(cmdBuffer->getBufferHandles().data());
    execParam.buffer_count = safe_cast<uint32_t>(cmdBuffer->getBufferHandles().size());
    execParam.engine = cmdBuffer->getEngine();
    execParam.priority = static_cast<uint32_t>(cmdBuffer->getPriority());

    LOG(DEVICE, "Submit buffer type: %s.", cmdBuffer->getName());
    LOG(DEVICE,
        "Submit params -> engine: %u, flags: %u, offset: %u, count: %u, ptr: %#llx, prior: %u",
        execParam.engine,
        execParam.flags,
        execParam.commands_offset,
        execParam.buffer_count,
        execParam.buffers_ptr,
        execParam.priority);

    constexpr auto pollTime = std::chrono::seconds(2);
    const auto timeoutPoint = std::chrono::steady_clock::now() + pollTime;
    while (drvApi->submitCommandBuffer(&execParam) < 0) {
        /*
         * SUBMIT ioctl returns EBUSY if command queue is full. Driver should wait till firmware
         * completes a job and make a space for new job in queue. Polling time is set to 2 seconds
         * to match with TDR timeout.
         */
        if (errno != EBUSY) {
            LOG_E("Failed to submit %s command buffer: %p", cmdBuffer->getName(), cmdBuffer);
            return false;
        }

        if (std::chrono::steady_clock::now() > timeoutPoint) {
            LOG_E("Timed out waiting for driver to submit a job");
            return false;
        }

        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    return true;
}

bool VPUDeviceContext::submitJob(const VPUJob *job) {
    if (job == nullptr) {
        LOG_W("Invalid argument - job is nullptr");
        return false;
    }

    if (job->getCommandBuffers().size() == 0) {
        LOG_E("Invalid argument - no command buffer in job");
        return false;
    }

    for (const auto &cmdBuffer : job->getCommandBuffers()) {
        if (!submitCommandBuffer(cmdBuffer.get())) {
            LOG_E("Failed to submit job using cmdBuffer: %p", cmdBuffer.get());
            return false;
        }
    }

    LOG(DEVICE, "Buffer execution successfully triggered");
    return true;
}

bool VPUDeviceContext::getCopyCommandDescriptor(const void *src,
                                                void *dst,
                                                size_t size,
                                                VPUDescriptor &desc) {
    if (hwInfo->getCopyCommand == nullptr) {
        LOG_E("Failed to get copy descriptor");
        return false;
    }

    return hwInfo->getCopyCommand(this, src, dst, size, desc);
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
