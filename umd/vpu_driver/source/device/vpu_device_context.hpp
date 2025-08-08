/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "api/vpu_jsm_job_cmd_api.h"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/source/utilities/stats.hpp"

#include <algorithm>
#include <bitset>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace VPU {
struct VPUDescriptor;
class VPUDeviceContext;

class ScratchCacheFactory {
  public:
    ScratchCacheFactory() = default;

    std::shared_ptr<VPUBufferObject> acquire(VPUDeviceContext *ctx, size_t size);
    void prune(size_t size);

  private:
    std::vector<std::shared_ptr<VPUBufferObject>> scratchBuffers;
    std::mutex scratchMutex;
};

class VPUDeviceContext {
  public:
    VPUDeviceContext(std::unique_ptr<VPUDriverApi> drvApi, VPUHwInfo *info);
    virtual ~VPUDeviceContext() = default;

    VPUDeviceContext(VPUDeviceContext const &) = delete;
    VPUDeviceContext &operator=(VPUDeviceContext const &) = delete;

    inline void *
    createMemAlloc(size_t size, VPUBufferObject::Type type, VPUBufferObject::Location loc) {
        auto bo = createBufferObject(size, type, loc);
        if (bo == nullptr)
            return nullptr;
        MemoryStatistics::get().snapshot();
        return bo->getBasePointer();
    }

    /**
       Free memory within tracking structure and unmap in memory.
       @return true when the pointer is free'd in memory, false otherwise.
     */
    bool freeMemAlloc(std::shared_ptr<VPUBufferObject> bo);

    /**
       Free memory within tracking structure and unmap in memory.
       @return true when the pointer is free'd in memory, false otherwise.
     */
    bool freeMemAlloc(void *ptr);

    std::shared_ptr<VPUBufferObject> findBufferObject(const void *ptr) const;

    /**
     * Returns page aligned size.
     */
    size_t getPageAlignedSize(size_t size);

    std::shared_ptr<VPUBufferObject> createUntrackedBufferObject(size_t size,
                                                                 VPUBufferObject::Type range);
    std::shared_ptr<VPUBufferObject> importBufferObject(VPUBufferObject::Location type, int32_t fd);
    int getFd() const { return drvApi->getFd(); }

    /**
     * Return assigned VPUDriverApi
     */
    VPUDriverApi &getDriverApi() const { return *drvApi; }
    const VPUHwInfo &getDeviceCapabilities() const { return *hwInfo; }
    /**
     * Return value of VPU Device ID
     */
    inline uint32_t getPciDevId() const { return hwInfo->deviceId; }

    uint16_t getDeviceRevision() const { return hwInfo->deviceRevision; }

    uint32_t getNumSlices() const {
        return static_cast<uint32_t>(std::bitset<32>(hwInfo->tileConfig).count());
    }

    uint32_t getExtraDmaDescriptorSize() const { return hwInfo->extraDmaDescriptorSize; }

    uint64_t getFwMappedInferenceVersion() const { return hwInfo->fwMappedInferenceVersion; }

    uint32_t getFwTimestampType() const { return hwInfo->fwTimestampType; }

    /**
     * Return number of currently tracking buffer objects in the structure
     */
    size_t getBuffersCount() {
        size_t untrackedCount = 0;
        const std::lock_guard<std::mutex> lock(mtx);
        for (auto &obj : untrackedBuffers) {
            if (!obj.expired())
                untrackedCount++;
        }
        return trackedBuffers.size() + untrackedCount;
    }

    /**
     * Return size of currently tracking buffer objects in the structure
     */
    size_t getAllocatedSize() {
        size_t size = 0;

        const std::lock_guard<std::mutex> lock(mtx);
        for (auto &obj : untrackedBuffers) {
            auto bo = obj.lock();
            if (bo)
                size += bo->getAllocSize();
        }
        for (const auto &buffer : trackedBuffers)
            size += buffer.second->getAllocSize();

        return size;
    }

    /**
     * Removes expired buffer objects from untrackedBuffers vector
     */
    void removeExpiredInternalBuffers() {
        const std::lock_guard<std::mutex> lock(mtx);
        untrackedBuffers.erase(std::remove_if(untrackedBuffers.begin(),
                                              untrackedBuffers.end(),
                                              [](auto x) { return x.expired(); }),
                               untrackedBuffers.end());
    }

    /**
     * Return inference ID from kernel driver that is unique for VPU
     */
    bool getUniqueInferenceId(uint64_t &inferenceId);

    bool
    getCopyCommandDescriptor(uint64_t srcAddr, uint64_t dstAddr, size_t size, VPUDescriptor &desc);
    void printCopyDescriptor(void *desc, vpu_cmd_header_t *cmd);

    // Scratch cache management
    std::shared_ptr<VPUBufferObject> scratchCacheAcquire(size_t size) {
        return scratchCache.acquire(this, size);
    }
    void scratchCachePrune(size_t size) { return scratchCache.prune(size); }
    void scratchCachePreload(size_t size) {
        if (size == 0) {
            return;
        }
        scratchCache.prune(size - 1);
        scratchCache.acquire(this, size);
    }

  private:
    /**
       Create VPUBufferObject and assign it to tracking structure
       @param size size of memory to be created.
       @param range buffer object range that will be used
       @param location memory type being identified
       @return pointer to VPUBufferObject, on failure return nullptr
     */
    std::shared_ptr<VPUBufferObject> createBufferObject(size_t size,
                                                        VPUBufferObject::Type range,
                                                        VPUBufferObject::Location location);

    std::unique_ptr<VPUDriverApi> drvApi;
    VPUHwInfo *hwInfo;

    std::map<const void *, std::shared_ptr<VPUBufferObject>, std::greater<const void *>>
        trackedBuffers;
    std::vector<std::weak_ptr<VPUBufferObject>> untrackedBuffers;
    mutable std::mutex mtx;

    ScratchCacheFactory scratchCache;
};

} // namespace VPU
