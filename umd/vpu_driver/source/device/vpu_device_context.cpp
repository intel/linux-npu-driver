/*
 * Copyright (C) 2022 Intel Corporation
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

#include <cassert>
#include <memory>
#include <map>
#include <sys/mman.h>
#include <string.h>

namespace VPU {

VPUDeviceContext::VPUDeviceContext(std::unique_ptr<VPUDriverApi> drvApi, VPUHwInfo *info)
    : drvApi(std::move(drvApi))
    , hwInfo(info) {
    LOG_I("VPUDeviceContext is created");
}

VPUBufferObject *VPUDeviceContext::createBufferObject(const size_t size,
                                                      const VPUBufferObject::Type type,
                                                      const VPUBufferObject::Location loc) {
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

    LOG_I("Buffer object %p successfully added to trackedBuffers", &it->second);
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

    LOG_I("Pointer %p was found in device context(type: %d, range: %d).",
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

    return bo;
}

CopyDirection VPUDeviceContext::getCopyDirection(void *dstPtr, const void *srcPtr) {
    auto dstBO = findBuffer(dstPtr);
    auto srcBO = findBuffer(srcPtr);

    if (dstBO != nullptr && srcBO != nullptr) {
        auto dstLoc = dstBO->getLocation();
        auto srcLoc = srcBO->getLocation();
        LOG_I("Destination mem type: %d Source mem type: %d",
              static_cast<int>(dstLoc),
              static_cast<int>(srcLoc));

        if ((srcLoc == VPUBufferObject::Location::Device ||
             srcLoc == VPUBufferObject::Location::Shared) &&
            (dstLoc == VPUBufferObject::Location::Device ||
             dstLoc == VPUBufferObject::Location::Shared)) {
            return COPY_LOCAL_TO_LOCAL;
        }

        return COPY_SYSTEM_TO_SYSTEM;
    }

    LOG_E("Buffer objects invalid.");
    return COPY_INVALID;
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

    LOG_V("CPU address %p mapped to VPU address %#lx", ptr, bo->getVPUAddr() + offset);

    return bo->getVPUAddr() + offset;
}

bool VPUDeviceContext::submitCommandBuffer(const VPUCommandBuffer *cmdBuffer) {
    drm_ivpu_submit execParam = {};
    execParam.buffers_ptr = reinterpret_cast<uint64_t>(cmdBuffer->getBufferHandles().data());
    execParam.buffer_count = boost::numeric_cast<uint32_t>(cmdBuffer->getBufferHandles().size());
    execParam.engine = cmdBuffer->getEngine();

    LOG_I("Buffer type: %s.", cmdBuffer->getName());
    LOG_I("Exec engine: %u, flags: %u, commands_offset: %u, buffer_count: %u, buffers_ptr: %#llx",
          execParam.engine,
          execParam.flags,
          execParam.commands_offset,
          execParam.buffer_count,
          execParam.buffers_ptr);

    if (drvApi->submitCommandBuffer(&execParam) < 0) {
        LOG_E("Failed to submit %s command buffer: %p", cmdBuffer->getName(), cmdBuffer);
        return false;
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
        cmdBuffer->printCommandBuffer(job->getDescriptorPtr());
        if (!submitCommandBuffer(cmdBuffer.get())) {
            LOG_E("Failed to submit job using cmdBuffer: %p", cmdBuffer.get());
            return false;
        }
    }

    LOG_V("Buffer execution successfully triggered.");
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

} // namespace VPU
