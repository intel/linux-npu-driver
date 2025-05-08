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
#include "vpu_driver/source/command/vpu_event_command.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <memory>
#include <uapi/drm/ivpu_accel.h>
#include <vector>

namespace VPU {
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
                          const std::vector<std::shared_ptr<VPUCommand>>::iterator &end,
                          VPUEventCommand::KMDEventDataType **fenceWait,
                          std::shared_ptr<VPUBufferObject> &fenceBo);

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

    bool replaceBufferHandles(std::vector<uint32_t> &oldHandles, std::vector<uint32_t> &newHandles);

    bool updateCommands();

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
    void addUniqueBoHandle(uint32_t handle);

  public:
    /* Address CommandHeader has to be aligned to 64 bytes (FW cache line size) */
    struct CommandHeader {
        vpu_cmd_buffer_header header;
        uint8_t contextSaveArea[VPU_CONTEXT_SAVE_AREA_SIZE] __attribute__((aligned(64)));
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
    std::shared_ptr<VPUBufferObject> buffer;
    uint32_t jobStatus;
    std::vector<std::shared_ptr<VPUCommand>>::iterator commandsBegin;
    std::vector<std::shared_ptr<VPUCommand>>::iterator commandsEnd;

    uint64_t syncFenceVpuAddr = 0;
    std::vector<uint32_t> bufferHandles;
};

} // namespace VPU
