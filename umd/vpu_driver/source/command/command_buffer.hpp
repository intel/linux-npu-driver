/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "api/vpu_jsm_job_cmd_api.h"
#include "vpu_driver/source/command/event_command.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <algorithm>
#include <deque>
#include <iosfwd>
#include <iterator>
#include <memory>
#include <string>
#include <uapi/drm/ivpu_accel.h>
#include <unordered_map>
#include <unordered_set>
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

    struct BufferHandles {
        typedef uint32_t value_type;

        void insert(uint32_t handle) {
            if (!useCount.contains(handle)) {
                handles.resize(useCount.size() + 1);
                handles[useCount.size()] = handle;
            }
            useCount[handle]++;
        }

        void insertResidentSet(const std::unordered_set<uint32_t> &residentSet) {
            handles.resize(useCount.size() + residentSet.size());
            auto it = handles.begin() + static_cast<std::ptrdiff_t>(useCount.size());
            for (auto handle : residentSet) {
                if (useCount.contains(handle))
                    continue;
                *it++ = handle;
            }
            handles.resize(static_cast<size_t>(std::distance(handles.begin(), it)));
        }

        void erase(uint32_t handle) {
            if (!useCount.contains(handle))
                return;
            if (--useCount[handle] == 0) {
                auto end = handles.begin() + static_cast<std::ptrdiff_t>(useCount.size());
                handles.erase(std::find(handles.begin(), end, handle));
                useCount.erase(handle);
            }
        }

        auto position(uint32_t handle) const {
            if (!useCount.contains(handle)) {
                return -1L;
            }
            auto end = handles.begin() + static_cast<std::ptrdiff_t>(useCount.size());
            return std::distance(handles.begin(), std::find(handles.begin(), end, handle));
        }

        const uint32_t *data() const { return handles.data(); }
        size_t size() const { return handles.size(); }
        auto begin() const { return handles.begin(); }
        auto end() const { return handles.end(); }

      private:
        std::vector<uint32_t> handles;
        std::unordered_map<uint32_t, uint32_t> useCount;
    };

    /**
     * Return the vector of stored buffer handles
     */
    const BufferHandles &getBufferHandles();

    /**
     * Print the content of command buffer
     */
    void printCommandBuffer(const char *description) const;

    /**
     * Build a JSON description of the command buffer content.
     * Reused by both terminal logging (LOG(VPU_CMD, ...)) and file dumps.
     */
    std::string commandBufferToJson(const char *description) const;

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
    void removeBufferHandle(const uint32_t handle);

    bool updateCommands();

    void resetFenceValue();

    uint32_t getCommandBufferOffset() const { return offsetof(CommandHeader, header); }
    bool addWaitAtHead(std::shared_ptr<VPUBufferObject> waitBo, bool resetFence = false);
    bool addSelfSignalAtTail();

    void addPreemptionBuffer(std::shared_ptr<VPUBufferObject> bo);
    uint32_t getPreemptionBufferIndex() const;
    void useBusyWait();

  private:
    /**
     * Initialize command buffer header
     */
    bool initHeader(size_t cmdSize);

    /**
     * Add VPUCommand details to the command list
     */
    bool addCommand(std::shared_ptr<VPUCommand> cmd, size_t &cmdOffset, size_t &descOffset);

    /**
     * Set fence address that is used for command buffer recognition
     */
    bool setSyncFenceAddr(VPUCommand *cmd);
    void addUniqueBoHandle(uint32_t handle);
    bool wait(int64_t timeout_abs_ns);
    void busyWait(int64_t timeout_abs_ns, uint32_t tscFreqMHz);

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
    uint32_t jobStatus;
    std::deque<std::shared_ptr<VPUCommand>> commands;

    uint64_t syncFenceVpuAddr = 0;
    BufferHandles bufferHandles;

    // The inference execute command may require a shared scratch buffer
    std::unordered_map<VPUBufferObject::Type, size_t> inferenceScratchSize;
    std::unordered_map<VPUBufferObject::Type, std::shared_ptr<VPUBufferObject>>
        inferenceScratchBuffers;

    std::shared_ptr<VPUBufferObject> preemptionBuffer;
    bool useBusyWaitFlag = false;
};

} // namespace VPU
