/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/command/vpu_command_buffer.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <memory>
#include <vector>

namespace VPU {
class VPUDeviceContext;

class VPUJob {
  public:
    VPUJob(VPUDeviceContext *ctx);

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

    VPUCommand *getCommand(size_t index) {
        if (index >= commands.size()) {
            LOG_E("Invalid command index %zu (number of avail commands %zu)",
                  index,
                  commands.size());
            return nullptr;
        }

        return commands[index].get();
    }

    bool isClosed() const { return closed; }

    void setPriority(VPUCommandBuffer::Priority p) {
        for (auto &c : cmdBuffers)
            c->setPriority(p);
    }

    void setNeedsUpdate(bool value) { needsUpdate = value; }

  private:
    std::vector<std::shared_ptr<VPUCommand>>::iterator
    scheduleCommands(std::vector<std::shared_ptr<VPUCommand>>::iterator begin);

    bool createCommandBuffer(const std::vector<std::shared_ptr<VPUCommand>>::iterator &begin,
                             const std::vector<std::shared_ptr<VPUCommand>>::iterator &end,
                             VPUEventCommand::KMDEventDataType **lastEvent);

    VPUDeviceContext *ctx = nullptr;

    std::vector<std::unique_ptr<VPUCommandBuffer>> cmdBuffers;
    std::vector<std::shared_ptr<VPUCommand>> commands;

    bool closed = false;
    bool needsUpdate = false;
};

} // namespace VPU
