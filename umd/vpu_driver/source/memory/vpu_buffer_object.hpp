/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include <memory>
#include <uapi/drm/ivpu_accel.h>

namespace VPU {
class VPUDriverApi;

class VPUBufferObject {
  public:
    enum class Type {
        CachedFw = DRM_IVPU_BO_CACHED | DRM_IVPU_BO_MAPPABLE,
        CachedShave = DRM_IVPU_BO_CACHED | DRM_IVPU_BO_MAPPABLE | DRM_IVPU_BO_HIGH_MEM,
        CachedDma = DRM_IVPU_BO_CACHED | DRM_IVPU_BO_MAPPABLE | DRM_IVPU_BO_DMA_MEM,
        UncachedFw = DRM_IVPU_BO_UNCACHED,
        UncachedShave = DRM_IVPU_BO_UNCACHED | DRM_IVPU_BO_HIGH_MEM,
        UncachedDma = DRM_IVPU_BO_UNCACHED | DRM_IVPU_BO_DMA_MEM,
        WriteCombineFw = DRM_IVPU_BO_WC | DRM_IVPU_BO_MAPPABLE,
        WriteCombineShave = DRM_IVPU_BO_WC | DRM_IVPU_BO_MAPPABLE | DRM_IVPU_BO_HIGH_MEM,
        WriteCombineDma = DRM_IVPU_BO_WC | DRM_IVPU_BO_MAPPABLE | DRM_IVPU_BO_DMA_MEM,
        ImportedMemory = 0,
    };
    const uint32_t externalMemMask = 0x8000;
    enum class Location {
        Internal = 0x1,
        Host = 0x2,
        Device = 0x4,
        Shared = 0x8,
        ExternalHost = 0x8002,
        ExternalDevice = 0x8004,
        ExternalShared = 0x8008,
    };

    static std::unique_ptr<VPUBufferObject>
    create(const VPUDriverApi &drvApi, Location type, Type range, size_t size);

    /**
     * @brief Import Buffer from file descriptor
     *
     */
    static std::unique_ptr<VPUBufferObject>
    importFromFd(const VPUDriverApi &drvApi, Location type, int32_t fd);

    VPUBufferObject(const VPUDriverApi &drvApi,
                    Location memoryType,
                    Type range,
                    void *basePtr,
                    size_t allocSize,
                    uint32_t handle,
                    uint64_t vpuAddr);
    ~VPUBufferObject();

    VPUBufferObject(const VPUBufferObject &) = delete;
    VPUBufferObject(VPUBufferObject &&) = delete;
    VPUBufferObject &operator=(const VPUBufferObject &) = delete;
    VPUBufferObject &&operator=(VPUBufferObject &&) = delete;

    /**
      Returns memory usage type that object was created from (Host/Device/Shared).
     */
    Location getLocation() const { return location; }

    /**
      Returns current range.
     */
    Type getType() const { return type; }

    void allowDeleteExternalHandle() {
        switch (location) {
        case Location::ExternalHost:
            location = Location::Host;
            break;
        case Location::ExternalDevice:
            location = Location::Device;
            break;
        case Location::ExternalShared:
            location = Location::Shared;
            break;
        default:
            break;
        }
    }

    /**
       Returns memory size of the buffer object.
     */
    size_t getAllocSize() const { return allocSize; }

    /**
       Returns buffer handle.
     */
    uint32_t getHandle() const { return handle; }

    /**
      Returns buffer object's virtual address.
     */
    uint8_t *getBasePointer() const { return basePtr; }

    /**
      Returns whether the given pointer is within buffer object's range or not.
      @param ptr[IN]: Pointer to check.
      @return true if given ptr is in range from basePtr to basePtr + allocSize - 1.
     */
    bool isInRange(const void *ptr) const {
        // Byte wise pointer arithmetic.
        uint8_t *upperBound = basePtr + allocSize;
        return ptr >= basePtr && ptr < upperBound;
    }

    /**
      Returns buffer object's VPU address.
     */
    uint64_t getVPUAddr() const { return vpuAddr; }

    /**
       Copy data to the allocated buffer.
       Member variable appendOffset will be maintained internally.
       @param dataSrc[in]: Byte stream data to copy.
       @param dataSize[in]: Size of the stream in bytes.
       @return true on successful copy, false otherwise.
     */
    bool copyToBuffer(const void *data, size_t size, uint64_t offset);

    /**
       Fill allocated buffer with pattern.
       @param pattern[in]: Pattern byte stream.
       @param patternSize[in]: Size of pattern, allowed:1,2,4
       @return true on successful fill, false otherwise.
     */
    bool fillBuffer(const void *pattern, size_t patternSize);

    /**
     * @brief Export Buffer to file descriptor
     *
     */
    bool exportToFd(int32_t &fd);
    uint64_t getId() const { return id; }

  private:
    const VPUDriverApi &drvApi;
    Location location;
    Type type;
    uint8_t *basePtr;
    size_t allocSize;

    uint64_t vpuAddr;
    uint32_t handle;
    uint64_t id;
};

} // namespace VPU
