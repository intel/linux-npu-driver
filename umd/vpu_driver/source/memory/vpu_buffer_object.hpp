/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>

#include <uapi/drm/ivpu_accel.h>
#include <api/vpu_jsm_job_cmd_api.h>

enum CopyDirection : uint16_t {
    COPY_LOCAL_TO_LOCAL = VPU_CMD_COPY_LOCAL_TO_LOCAL,
    COPY_SYSTEM_TO_SYSTEM = VPU_CMD_COPY_SYSTEM_TO_SYSTEM,
    COPY_INVALID = 0x1111
};

namespace VPU {

class VPUBufferObject {
  public:
    enum class Type {
        CachedLow = DRM_IVPU_BO_CACHED | DRM_IVPU_BO_MAPPABLE,
        CachedHigh = DRM_IVPU_BO_CACHED | DRM_IVPU_BO_MAPPABLE | DRM_IVPU_BO_HIGH_MEM,
        UncachedLow = DRM_IVPU_BO_UNCACHED,
        UncachedHigh = DRM_IVPU_BO_UNCACHED | DRM_IVPU_BO_HIGH_MEM,
        WriteCombineLow = DRM_IVPU_BO_WC | DRM_IVPU_BO_MAPPABLE,
        WriteCombineHigh = DRM_IVPU_BO_WC | DRM_IVPU_BO_MAPPABLE | DRM_IVPU_BO_HIGH_MEM,
    };

    enum class Location { Internal, Host, Device, Shared };

    static std::unique_ptr<VPUBufferObject>
    create(const VPUDriverApi &drvApi, Location type, Type range, size_t size);

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

  private:
    static bool isUncached(Type t) { return t == Type::UncachedLow || t == Type::UncachedHigh; }

    bool isUncached() const { return isUncached(type); }

    const VPUDriverApi &drvApi;
    Location location;
    Type type;
    uint8_t *basePtr;
    size_t allocSize;

    uint64_t vpuAddr;
    uint32_t handle;
};

} // namespace VPU
