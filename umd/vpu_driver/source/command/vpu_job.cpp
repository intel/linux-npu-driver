/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_job.hpp"

#include "umd_common.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <iterator>
#include <uapi/drm/ivpu_accel.h>
#include <utility>

namespace VPU {
class VPUDeviceContext;

VPUJob::VPUJob(VPUDeviceContext *ctx)
    : ctx(ctx) {}

bool VPUJob::closeCommands() {
    if (ctx == nullptr) {
        LOG_E("VPUDeviceContext is nullptr");
        return false;
    }

    if (needsUpdate) {
        needsUpdate = false;

        for (auto &cmdBuffer : cmdBuffers) {
            if (!cmdBuffer->updateCommands()) {
                return false;
            }
        }
        return true;
    }

    if (isClosed()) {
        LOG_W("VPUJob is closed");
        return true;
    }

    if (cmdBuffers.size()) {
        LOG_E("Failed to close the job because of dirty state");
        return false;
    }

    LOG(VPU_JOB, "Schedule commands, number of commands %lu", commands.size());

    VPUEventCommand::KMDEventDataType *lastEvent = nullptr;
    for (auto it = commands.begin(); it != commands.end();) {
        auto next = scheduleCommands(it);

        long jump = std::distance(it, next);
        LOG(VPU_JOB, "Passing %lu commands to command buffer", jump);

        if (safe_cast<size_t>(jump) == commands.size()) {
            if (!createCommandBuffer(commands.begin(), commands.end(), nullptr)) {
                LOG_E("Failed to initialize command buffer");
                return false;
            }
        } else {
            if (!createCommandBuffer(it, next, &lastEvent)) {
                LOG_E("Failed to initialize command buffer");
                return false;
            }
        }

        it = next;
    }

    closed = true;
    return true;
}

bool VPUJob::createCommandBuffer(const std::vector<std::shared_ptr<VPUCommand>>::iterator &begin,
                                 const std::vector<std::shared_ptr<VPUCommand>>::iterator &end,
                                 VPUEventCommand::KMDEventDataType **lastEvent) {
    auto cmdBuffer = VPUCommandBuffer::allocateCommandBuffer(ctx, begin, end, lastEvent);
    if (cmdBuffer == nullptr) {
        LOG_E("Failed to allocate VPUCommandBuffer");
        return false;
    }

    cmdBuffers.emplace_back(std::move(cmdBuffer));
    return true;
}

bool VPUJob::waitForCompletion(int64_t timeout_abs_ns) {
    for (const auto &cmdBuffer : cmdBuffers)
        if (!cmdBuffer->waitForCompletion(timeout_abs_ns))
            return false;
    printResult();
    return true;
}

bool VPUJob::isSuccess() const {
    for (const auto &cmdBuffer : cmdBuffers)
        if (!cmdBuffer->isSuccess())
            return false;

    return true;
}

uint64_t VPUJob::getStatus() const {
    for (const auto &cmdBuffer : cmdBuffers) {
        auto status = cmdBuffer->getResult();
        if (status != DRM_IVPU_JOB_STATUS_SUCCESS)
            return status;
    }
    return DRM_IVPU_JOB_STATUS_SUCCESS;
}

void VPUJob::printResult() const {
    for (const auto &cmdBuffer : cmdBuffers) {
        if (cmdBuffer->getResult() == DRM_IVPU_JOB_STATUS_SUCCESS) {
            LOG(VPU_JOB,
                "Compute command buffer (%p): execution is completed with success",
                cmdBuffer.get());
        } else {
            LOG_E("Compute command buffer (%p): execution failed with status %#lx",
                  cmdBuffer.get(),
                  cmdBuffer->getResult());
        }
    }
}

bool VPUJob::appendCommand(std::shared_ptr<VPUCommand> cmd) {
    if (isClosed()) {
        LOG_E("Job is already closed");
        return false;
    }

    if (cmdBuffers.size() != 0) {
        LOG_E("CommandBuffers are already allocated");
        return false;
    }

    if (cmd->getCommitSize() == 0) {
        LOG(VPU_JOB, "Command is empty, skipping it");
        return true;
    }

    commands.push_back(cmd);
    return true;
}

std::vector<std::shared_ptr<VPUCommand>>::iterator
VPUJob::scheduleCommands(std::vector<std::shared_ptr<VPUCommand>>::iterator begin) {
    auto it = begin;
    for (; it != commands.end(); it++) {
        const auto &cmd = *it;

        if (cmd->isSynchronizeCommand()) {
            it++;
            break;
        }
    }

    return it;
}

} // namespace VPU
