/*
 * Copyright (C) 2022-2024 Intel Corporation
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

#include <bitset>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <utility>

namespace VPU {
class VPUCommandBuffer;
class VPUJob;
struct VPUDescriptor;

class VPUDeviceContext {
  public:
    VPUDeviceContext(std::unique_ptr<VPUDriverApi> drvApi, VPUHwInfo *info);
    virtual ~VPUDeviceContext() = default;

    VPUDeviceContext(VPUDeviceContext const &) = delete;
    VPUDeviceContext &operator=(VPUDeviceContext const &) = delete;

    inline void *
    createMemAlloc(size_t size, VPUBufferObject::Type type, VPUBufferObject::Location loc) {
        VPUBufferObject *bo = createBufferObject(size, type, loc);
        if (bo == nullptr)
            return nullptr;
        MemoryStatistics::get().snapshot();
        return bo->getBasePointer();
    }

    /**
       Free memory within tracking structure and unmap in memory.
       @return true when the pointer is free'd in memory, false otherwise.
     */
    bool freeMemAlloc(VPUBufferObject *bo);

    /**
       Free memory within tracking structure and unmap in memory.
       @return true when the pointer is free'd in memory, false otherwise.
     */
    bool freeMemAlloc(void *ptr);

    /**
       Find a buffer object and return.
       @param ptr[in]: A pointer to find object.
       @return : Found VPUBufferObject, nullptr otherwise.
     */
    VPUBufferObject *findBuffer(const void *ptr) const;

    /**
     * Returns page aligned size.
     */
    size_t getPageAlignedSize(size_t size);

    /**
     * Returns a VPU device address assigned to BufferObject
     */
    uint64_t getBufferVPUAddress(const void *ptr) const;

    /**
     * @brief Submit given command buffer to KMD.
     *
     * @param job    VPUJob that contains command buffers for execution
     * @return true if job submitted successfully
     */
    bool submitJob(const VPUJob *job);

    /**
       Allocates VPUBufferObject for internal usage of driver.
       @param size[in]: Size of the buffer.
       @param type[in]: Type of buffer range.
       @return pointer to buffer object
     */
    VPUBufferObject *createInternalBufferObject(size_t size, VPUBufferObject::Type type);
    VPUBufferObject *importBufferObject(VPUBufferObject::Location type, int32_t fd);
    int getFd() const { return drvApi->getFd(); }

    /**
     * Return assigned VPUDriverApi
     */
    const VPUDriverApi &getDriverApi() const { return *drvApi; }
    const VPUHwInfo &getDeviceCapabilities() const { return *hwInfo; }
    /**
     * Return value of VPU Device ID
     */
    inline uint32_t getPciDevId() const { return hwInfo->deviceId; }

    /**
     * Return platform for compilation
     */
    inline int getCompilerPlatform() const { return hwInfo->compilerPlatform; }

    uint32_t getDeviceRevision() const { return hwInfo->deviceRevision; }

    uint32_t getNumSlices() const {
        return static_cast<uint32_t>(std::bitset<32>(hwInfo->tileConfig).count());
    }

    /**
     * Return the lowest VPU address from VPU low range that is accessible by firmware device
     */
    uint64_t getVPULowBaseAddress() const { return hwInfo->baseLowAddress; }

    uint32_t getExtraDmaDescriptorSize() const { return hwInfo->extraDmaDescriptorSize; }

    uint64_t getFwMappedInferenceVersion() const { return hwInfo->fwMappedInferenceVersion; }

    uint32_t getFwTimestampType() const { return hwInfo->fwTimestampType; }

    /**
     * Return number of currently tracking buffer objects in the structure
     */
    size_t getBuffersCount() const {
        const std::lock_guard<std::mutex> lock(mtx);
        return trackedBuffers.size();
    }

    /**
     * Return size of currently tracking buffer objects in the structure
     */
    size_t getAllocatedSize() const {
        size_t size = 0;
        for (const auto &buffer : trackedBuffers)
            size += buffer.second->getAllocSize();
        return size;
    }

    /**
     * Return inference ID from kernel driver that is unique for VPU
     */
    bool getUniqueInferenceId(uint64_t &inferenceId);

    bool getCopyCommandDescriptor(const void *src, void *dst, size_t size, VPUDescriptor &desc);
    void printCopyDescriptor(void *desc, vpu_cmd_header_t *cmd);

  private:
    /**
       Create VPUBufferObject and assign it to tracking structure
       @param size size of memory to be created.
       @param range buffer object range that will be used
       @param location memory type being identified
       @return pointer to VPUBufferObject, on failure return nullptr
     */
    VPUBufferObject *createBufferObject(size_t size,
                                        VPUBufferObject::Type range,
                                        VPUBufferObject::Location location);

    bool submitCommandBuffer(const VPUCommandBuffer *cmdBuffer);

  private:
    std::unique_ptr<VPUDriverApi> drvApi;
    VPUHwInfo *hwInfo;

    std::map<const void *, std::unique_ptr<VPUBufferObject>, std::greater<const void *>>
        trackedBuffers;
    mutable std::mutex mtx;
};

} // namespace VPU
