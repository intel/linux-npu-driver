/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/command/vpu_command_buffer.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"

#include <memory>
#include <vector>

namespace VPU {

class VPUJob {
  public:
    VPUJob(VPUDeviceContext *ctx, bool isCopyOnly);
    ~VPUJob();

    /**
     * Finalize building the job by moving commands into appropriate VPUCommandBuffers
     * @return true if command buffers are created with success
     */
    bool closeCommands();

    /**
     * Return true if the command buffers execution is completed with success
     */
    bool isSuccess() const;

    /**
     * @brief Returns true if the job has completed by the specified time.
     * @param timeout_abs_ns[in]: Absolute timeout in nanoseconds
     */
    bool waitForCompletion(int64_t timeout_abs_ns);

    /**
     * Print job result to the terminal
     */
    void printResult() const;

    /**
     * Return collection of VPUCommandBuffers
     */
    const std::vector<std::unique_ptr<VPUCommandBuffer>> &getCommandBuffers() const {
        return cmdBuffers;
    }

    void *getDescriptorPtr() const { return descriptorPtr; }

    /**
     * @brief Append command to proper collection based on following rules:
     *  - if command is copy type it is pushed to copy command collection
     *  - if command is compute type it is pushed to compute command collection
     *  - if command is backward type it is pushed to command collection that was used in
     * previous appendCommand call
     *  - if command is forward type it is pushed to command collection that will be used in
     * next appendCommand call
     *  If there was no commands in copy or compute type then forward and backward commands are
     * pushed to collection based on isCopyOnly member. If isCopyOnly is true, then commands are
     * flushed to copy collection. If isCopyOnly is false, then commands are flushed to compute
     * collection
     *
     * @param cmd [in]: Command that is added to command list
     * @return true on succesfull appending
     */
    bool appendCommand(std::shared_ptr<VPUCommand> cmd);

    inline size_t getNumCommands() const {
        return nnCmds.size() + cpCmds.size() + unclassified.size();
    }

    const std::vector<std::shared_ptr<VPUCommand>> &getNNCommands() const { return nnCmds; }
    const std::vector<std::shared_ptr<VPUCommand>> &getCopyCommands() const { return cpCmds; }

    /* Job is closed, no more append commands is allowed. Job is ready for submission */
    bool isClosed() const { return closed; }

  private:
    /**
     * @brief Move commands from src vector to dst vector. Clean the src vector.
     *
     * @param dst [in]: Destination vector
     * @param src [in]: Source vector
     */
    static void moveCommands(std::vector<std::shared_ptr<VPUCommand>> &dst,
                             std::vector<std::shared_ptr<VPUCommand>> &src);

    /**
     * @brief Flush unclassified commands to the last used command list. If last used command list
     * is not set, then push unclassified commands to compute command collection. If isCopyOnly is
     * true, flush commands to copy collection
     */
    void flushCommands();

    /**
     * @brief Add internal event between two command collections to synchronize the command order
     * execution.
     *
     * @param fromCmds[in]: Command list from
     * @param toCmds[in]: Command list to
     * @return true on successful event appending.
     */
    bool appendInternalEvents(std::vector<std::shared_ptr<VPUCommand>> &fromCmds,
                              std::vector<std::shared_ptr<VPUCommand>> &toCmds);

    /**
     * Create VPUCommandBuffer with user VPUCommands and designed for specific VPU engine
     * @param cmds[in]: Commands vector to be attached to the buffer
     * @param targetEngine[in]: Target of VPUCommandBuffer
     * @return true when successfully added, false otherwise.
     */
    bool createCommandBuffer(const std::vector<std::shared_ptr<VPUCommand>> &cmds,
                             VPUCommandBuffer::Target cmdtype);

    /**
     * Update buffer of VPUEventCommand used as internal event
     * @param cmd[in]: Internal Event Commands to be attached to buffer based on index
     * @return true when successfully added, false otherwise.
     */
    bool updateInternalEventBuffer(std::shared_ptr<VPUCommand> cmd);

    VPUDeviceContext *ctx = nullptr;
    bool isCopyOnly = false;

    /* Descriptor buffer that is shared between command buffers */
    VPUBufferObject *descriptor = nullptr;
    /* Pointer to the descriptor memory that is populated by commands */
    void *descriptorPtr = nullptr;

    /* Collection of VPUCommandBuffer that later will be pushed for submission */
    std::vector<std::unique_ptr<VPUCommandBuffer>> cmdBuffers;

    /* Commands collection */
    std::vector<std::shared_ptr<VPUCommand>> cpCmds;
    std::vector<std::shared_ptr<VPUCommand>> nnCmds;
    std::vector<std::shared_ptr<VPUCommand>> unclassified;
    std::vector<std::shared_ptr<VPUCommand>> *prevCmds = nullptr;

    /* Memory for internal events */
    VPUEventCommand::KMDEventDataType *eventBasePtr = nullptr;
    static constexpr size_t eventPoolSize = sizeof(VPUEventCommand::KMDEventDataType) * 2;
    uint8_t intEventIndex = 1u;

    bool closed = false;
};

} // namespace VPU
