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
#include "vpu_driver/source/command/event_command.hpp"

#include <memory>
#include <optional>
#include <uapi/drm/ivpu_accel.h>
#include <vector>

namespace VPU {
class VPUBufferObject;
class VPUCommand;
class VPUDeviceContext;

class VPUCommandBuffer {
  public:
    VPUCommandBuffer(VPUDeviceContext *ctx,
                     std::shared_ptr<VPUBufferObject> buffer,
                     const std::vector<std::shared_ptr<VPUCommand>>::iterator &begin,
                     const std::vector<std::shared_ptr<VPUCommand>>::iterator &end);

    /**
     * Allocate and return VPUCommandBuffer
     * @param ctx[in]: Memory manager for device interaction
     * @param begin[in]: VPUCommands begin iterator
     * @param begin[in]: VPUCommands end iterator
     * @param fenceWait[in]: Wait fence for this Command Buffer or nullptr
     * @param fenceBo[in]: Buffer object associated with fenceWait or nullptr
     * @return unique_ptr<VPUCommandBuffer> for success, nullptr for any allocation failures
     */
    static std::unique_ptr<VPUCommandBuffer>
    allocateCommandBuffer(VPUDeviceContext *ctx,
                          const std::vector<std::shared_ptr<VPUCommand>>::iterator &begin,
                          const std::vector<std::shared_ptr<VPUCommand>>::iterator &end);

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
    const std::vector<uint32_t> &getBufferHandles() const { return bufferHandles; }

    /**
     * Print the content of command buffer
     */
    void printCommandBuffer(const char *description) const;

    /**
     * Returns the buffer object.
     */
    std::shared_ptr<VPUBufferObject> getBuffer() const { return buffer; }

    /**
     * Return the VPU address of fence signal command
     */
    uint64_t getFenceAddr() const { return syncFenceVpuAddr; }

    bool replaceBufferHandles(const std::vector<uint32_t> &oldHandles,
                              const std::vector<uint32_t> &newHandles);

    bool updateCommands();

    void resetFenceValue();

    uint32_t getCommandBufferOffset() const { return offsetof(CommandHeader, header); }
    bool addWaitAtHead(std::shared_ptr<VPUBufferObject> waitBo, bool resetFence = false);
    bool addSelfSignalAtTail();

    void addPreemptionBuffer(std::shared_ptr<VPUBufferObject> bo);
    uint32_t getPreemptionBufferIndex() const { return preemptionBufferIndex.value_or(0); }

  private:
    /**
     * Initialize command buffer header
     */
    bool initHeader(size_t cmdSize);

    /**
     * Add VPUCommand details to the command list
     */
    bool addCommand(VPUCommand *cmd, size_t &cmdOffset, size_t &descOffset);

    /**
     * Set fence address that is used for command buffer recognition
     */
    bool setSyncFenceAddr(VPUCommand *cmd);
    void addUniqueBoHandle(uint32_t handle);

  public:
    /* CommandHeader address has to be aligned to 64 bytes (FW cache line size) */
    struct CommandHeader {
        uint8_t contextSaveArea[VPU_CONTEXT_SAVE_AREA_SIZE];
        VPUEventCommand::KMDEventDataType fenceValue;
        uint64_t reserved[7];
        vpu_cmd_buffer_header header;
        vpu_cmd_fence_t internalSync[2];
        uint8_t commandList[0];
        /* Last array is descriptors that requires address alignment to 64 bytes */
    };

    /* Make sure that CommandHeader fields meet FW alignment requirement */
    static_assert(offsetof(CommandHeader, header) % 64 == 0,
                  "Buffer header is not aligned to 64 bytes");
    static_assert(offsetof(CommandHeader, contextSaveArea) % 64 == 0,
                  "Context save area address is not aligned to 64 bytes");
    static_assert(offsetof(CommandHeader, fenceValue) % 64 == 0,
                  "Fence value is not aligned to 64 bytes");

  private:
    VPUDeviceContext *ctx;
    std::shared_ptr<VPUBufferObject> buffer;
    /* waitBoGuard keeps memory of synchronization fence
     * from previous command buffer when full synchronization requested
     */
    std::shared_ptr<VPUBufferObject> waitBoGuard;
    uint32_t jobStatus;
    std::vector<std::shared_ptr<VPUCommand>>::iterator commandsBegin;
    std::vector<std::shared_ptr<VPUCommand>>::iterator commandsEnd;

    uint64_t syncFenceVpuAddr = 0;
    std::vector<uint32_t> bufferHandles;

    // The inference execute command may require a shared scratch buffer
    size_t inferenceScratchSize = 0;
    std::shared_ptr<VPUBufferObject> inferenceScratchBuffer;

    std::shared_ptr<VPUBufferObject> preemptionBuffer;
    std::optional<uint32_t> preemptionBufferIndex = std::nullopt;
};

} // namespace VPU
