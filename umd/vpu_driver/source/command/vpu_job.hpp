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

    uint64_t getStatus() const;

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

    /**
     * @brief Append a command to command list
     *
     * @param cmd [in]: Command that is added to command list
     * @return true on succesfull appending
     */
    bool appendCommand(std::shared_ptr<VPUCommand> cmd);

    inline size_t getNumCommands() const { return commands.size(); }

    const std::vector<std::shared_ptr<VPUCommand>> &getCommands() const { return commands; }

    /* Job is closed, no more append commands is allowed. Job is ready for submission */
    bool isClosed() const { return closed; }

  private:
    /**
     * @brief Segregate commands into command buffers based on following rules:
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
     * @param begin [in]: Iterator of command list where algorithm should start.
     * @return last element in command list for specific target. Target can be only COMPUTE or COPY
     */
    std::pair<std::vector<std::shared_ptr<VPUCommand>>::iterator, VPUCommandBuffer::Target>
    scheduleCommands(std::vector<std::shared_ptr<VPUCommand>>::iterator begin);

    /**
     * Create VPUCommandBuffer with user VPUCommands and designed for specific VPU engine
     * @param cmds[in]: Commands vector to be attached to the buffer
     * @param targetEngine[in]: Target of VPUCommandBuffer
     * @return true when successfully added, false otherwise.
     */
    bool createCommandBuffer(const std::vector<std::shared_ptr<VPUCommand>> &cmds,
                             VPUCommandBuffer::Target cmdtype,
                             VPUEventCommand::KMDEventDataType **lastEvent);

    VPUDeviceContext *ctx = nullptr;
    bool isCopyOnly = false;

    /* Collection of VPUCommandBuffer that later will be pushed for submission */
    std::vector<std::unique_ptr<VPUCommandBuffer>> cmdBuffers;

    /* Commands collection */
    std::vector<std::shared_ptr<VPUCommand>> commands;

    bool closed = false;
};

} // namespace VPU
