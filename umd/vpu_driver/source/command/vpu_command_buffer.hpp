/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <linux/kernel.h>
#include <array>
#include <memory>
#include <vector>

namespace VPU {

class VPUCommandBuffer {
  public:
    enum class Target { COMPUTE = DRM_IVPU_ENGINE_COMPUTE, COPY = DRM_IVPU_ENGINE_COPY };
    enum class Priority : uint32_t {
        IDLE = DRM_IVPU_JOB_PRIORITY_IDLE,
        NORMAL = DRM_IVPU_JOB_PRIORITY_NORMAL,
        FOCUS = DRM_IVPU_JOB_PRIORITY_FOCUS,
        REALTIME = DRM_IVPU_JOB_PRIORITY_REALTIME,
    };

    static const char *targetEngineToStr(Target type) {
        switch (type) {
        case Target::COMPUTE:
            return "COMPUTE";
        case Target::COPY:
            return "COPY";
        }
        return "UNKNOWN";
    }

    VPUCommandBuffer(VPUDeviceContext *ctx, VPUBufferObject *buffer, Target type);
    ~VPUCommandBuffer();

    /**
     * Allocate and return VPUCommandBuffer
     * @param ctx[in]: Memory manager for device interaction
     * @param cmds[in]: VPUCommands that VPUCommandBuffer will use
     * @param targetEngine[in]: Submit a job to passed engine
     * @return unique_ptr<VPUCommandBuffer> for success, nullptr for any allocation failures
     */
    static std::unique_ptr<VPUCommandBuffer>
    allocateCommandBuffer(VPUDeviceContext *ctx,
                          const std::vector<std::shared_ptr<VPUCommand>> &cmds,
                          Target targetEngine,
                          VPUEventCommand::KMDEventDataType **fenceWait = nullptr);

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
     * Print the content of command buffer
     */
    void printCommandBuffer() const;

    /**
     * Return the pointer to buffer. Used only for testing
     */
    const uint8_t *getBufferPtr() const { return buffer->getBasePointer(); }

    /**
     * Return the VPU address of fence signal command
     */
    uint64_t getFenceAddr() const { return syncFenceVpuAddr; }

    void setPriority(Priority p) { priority = p; }
    Priority getPriority() const { return priority; }

  private:
    /**
     * Initialize command buffer header
     */
    bool initHeader(size_t cmdSize);

    /**
     * Add VPUCommand details to the command list
     */
    bool addCommand(VPUCommand *cmd, uint64_t &cmdOffset, uint64_t &descOffset);

    /**
     * Print the content of blob initialize or blob execute descriptor
     */
    void printDesc(vpu_cmd_resource_descriptor_table_t *table, size_t size, const char *type) const;

    /**
     * Set fence address that is used for command buffer recognition
     */
    bool setSyncFenceAddr(VPUCommand *cmd);

  public:
    /* Address CommandHeader has to be aligned to 64 bytes (FW cache line size) */
    struct CommandHeader {
        vpu_cmd_buffer_header header;
        uint64_t reserved_0[2];
        uint8_t contextSaveArea[VPU_CONTEXT_SAVE_AREA_SIZE];
        VPUEventCommand::KMDEventDataType fenceValue;
        uint64_t reserved_1[7];
        uint8_t commandList[0];
        uint8_t descriptorList[0];
    };

    /* Make sure that CommandHeader fields meet FW alignment requirement */
    static_assert(offsetof(CommandHeader, header) % 64 == 0,
                  "Buffer header is not aligned to 64 bytes");
    static_assert(offsetof(CommandHeader, contextSaveArea) % 64 == 0,
                  "Context save area address is not aligned to 64 bytes");
    static_assert(offsetof(CommandHeader, fenceValue) % 64 == 0,
                  "Fence value is not aligned to 64 bytes");
    static_assert(offsetof(CommandHeader, commandList) % 64 == 0,
                  "Command list is not aligned to 64 bytes");
    static_assert(offsetof(CommandHeader, descriptorList) % 64 == 0,
                  "Descriptor list is not aligned to 64 bytes");

  private:
    VPUDeviceContext *ctx;
    VPUBufferObject *buffer;
    Target targetEngine;
    uint32_t jobStatus;
    Priority priority;

    uint64_t syncFenceVpuAddr = 0;
    std::vector<uint32_t> bufferHandles;
};

} // namespace VPU
