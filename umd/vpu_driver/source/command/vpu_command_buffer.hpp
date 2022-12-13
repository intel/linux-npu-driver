/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <linux/kernel.h>
#include <array>
#include <memory>
#include <vector>

#include <boost/safe_numerics/safe_integer.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace VPU {

class VPUCommandBuffer {
  public:
    enum class Target { COMPUTE = DRM_IVPU_ENGINE_COMPUTE, COPY = DRM_IVPU_ENGINE_COPY };

    static const char *targetEngineToStr(Target type) {
        switch (type) {
        case Target::COMPUTE:
            return "COMPUTE";
        case Target::COPY:
            return "COPY";
        }
        return "UNKNOWN";
    }

    VPUCommandBuffer(VPUDeviceContext *ctx, VPUBufferObject *buffer, size_t cmdSize, Target type);
    ~VPUCommandBuffer();

    /**
     * Allocate and return VPUCommandBuffer
     * @param ctx[in]: Memory manager for device interaction
     * @param cmds[in]: VPUCommands that VPUCommandBuffer will use
     * @param descPtr[in|out]: Pointer at the tail of descriptor table
     * @param descEnd[in]: The end of descriptor table for validation
     * @param targetEngine[in]: Submit a job to passed engine
     * @return unique_ptr<VPUCommandBuffer> for success, nullptr for any allocation failures
     */
    static std::unique_ptr<VPUCommandBuffer>
    allocateCommandBuffer(VPUDeviceContext *ctx,
                          const std::vector<std::shared_ptr<VPUCommand>> &cmds,
                          void **descPtr,
                          void *descEnd,
                          Target targetEngine);

    /**
     * Return true if job is finished
     */
    bool waitForCompletion(int64_t timeout_abs_ns);

    /**
     * Return true if job result is success
     */
    bool isSuccess() const { return jobStatus == DRM_IVPU_JOB_STATUS_SUCCESS; }

    uint64_t getResult() const { return jobStatus; }

    /**
     * Return the vector of stored buffer handles
     */
    const std::vector<uint32_t> &getBufferHandles() const { return bufferHandles; };

    /**
     * Return the type of engine for what buffer is targeted
     */
    uint32_t getEngine() const { return static_cast<uint32_t>(targetEngine); }

    /**
     * Return the name of target engine
     */
    const char *getName() const { return targetEngineToStr(targetEngine); }

    /**
     * Print the content of blob initialize or blob execute descriptor
     */
    void printDesc(vpu_cmd_resource_descriptor_table_t *table,
                   boost::safe_numerics::safe<size_t> size,
                   const char *type) const;
    /**
     * Print the content of command buffer
     */
    void printCommandBuffer(void *descPtr) const;

    /**
     * Return the pointer to buffer. Used only for testing
     */
    const uint8_t *getBufferPtr() const { return buffer->getBasePointer(); }

  private:
    /**
     * Initialize command buffer header
     */
    bool initHeader();

    bool addCommand(VPUCommand *cmd, uint64_t &cmdOffset, void **desc, void *descEnd);
    bool addUniqueBufferHandler(const void *ptr);
    bool copyCommandStructures(const VPUCommand *cmd, uint64_t offset);

  public:
    /* Address CommandHeader has to be aligned to 64 bytes (FW cache line size) */
    struct CommandHeader {
        vpu_cmd_buffer_header header;
        uint64_t reserved[2];
        uint8_t contextSaveArea[VPU_CONTEXT_SAVE_AREA_SIZE];
        uint8_t cmds[0];
    };

    /* Make sure that CommandHeader fields meet FW alignment requirement */
    static_assert(offsetof(CommandHeader, header) % 64 == 0,
                  "Buffer header is not aligned to 64 bytes");
    static_assert(offsetof(CommandHeader, contextSaveArea) % 64 == 0,
                  "Context save area address is not aligned to 64 bytes");
    static_assert(offsetof(CommandHeader, cmds) % 64 == 0,
                  "Command list is not aligned to 64 bytes");

  private:
    VPUDeviceContext *ctx;
    VPUBufferObject *buffer;
    size_t cmdSize;
    Target targetEngine;
    uint32_t jobStatus;

    std::vector<uint32_t> bufferHandles;
};

} // namespace VPU
