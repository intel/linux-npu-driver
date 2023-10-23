/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <memory>
#include <mutex>
#include <map>

namespace VPU {

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
        return bo->getBasePointer();
    }

    inline void *
    createHostMemAlloc(size_t size,
                       VPUBufferObject::Type type = VPUBufferObject::Type::CachedHigh) {
        return createMemAlloc(size, type, VPUBufferObject::Location::Host);
    };

    inline void *
    createDeviceMemAlloc(size_t size,
                         VPUBufferObject::Type type = VPUBufferObject::Type::WriteCombineLow) {
        return createMemAlloc(size, type, VPUBufferObject::Location::Device);
    };

    inline void *
    createSharedMemAlloc(size_t size,
                         VPUBufferObject::Type type = VPUBufferObject::Type::CachedLow) {
        return createMemAlloc(size, type, VPUBufferObject::Location::Shared);
    };

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

    int getFd() const { return drvApi->getFd(); }

    /**
     * Return assigned VPUDriverApi
     */
    const VPUDriverApi &getDriverApi() const { return *drvApi; }

    /**
     * Return value of VPU Device ID
     */
    inline uint32_t getPciDevId() const { return hwInfo->deviceId; }

    /**
     * Return platform for compilation
     */
    inline int getCompilerPlatform() const { return hwInfo->compilerPlatform; }

    /**
     * Return the lowest VPU address from VPU low range that is accessible by firmware device
     */
    uint64_t getVPULowBaseAddress() const { return hwInfo->baseLowAddres; }

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
    VPUBufferObject *createBufferObject(const size_t size,
                                        const VPUBufferObject::Type range,
                                        const VPUBufferObject::Location location);

    bool submitCommandBuffer(const VPUCommandBuffer *cmdBuffer);

  private:
    std::unique_ptr<VPUDriverApi> drvApi;
    VPUHwInfo *hwInfo;

    std::map<const void *, std::unique_ptr<VPUBufferObject>, std::greater<const void *>>
        trackedBuffers;
    mutable std::mutex mtx;
};

} // namespace VPU
