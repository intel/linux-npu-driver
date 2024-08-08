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
#include <optional>
#include <uapi/drm/ivpu_accel.h>

namespace VPU {
class VPUDeviceContext;

VPUJob::VPUJob(VPUDeviceContext *ctx, bool isCopyOnly)
    : ctx(ctx)
    , isCopyOnly(isCopyOnly) {}

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
        LOG_E("Failed to close the VPUJob because of dirty state");
        return false;
    }

    LOG(VPU_JOB, "Schedule commands, number of commands %lu", commands.size());

    VPUEventCommand::KMDEventDataType *lastEvent = nullptr;
    for (auto it = commands.begin(); it != commands.end();) {
        auto [next, target] = scheduleCommands(it);

        long jump = std::distance(it, next);
        LOG(VPU_JOB, "Passing %lu commands for target %u", jump, static_cast<uint32_t>(target));

        if (safe_cast<size_t>(jump) == commands.size()) {
            if (!createCommandBuffer(commands.begin(), commands.end(), target, nullptr)) {
                LOG_E("Failed to initialize VPUCommandBuffer");
                return false;
            }
        } else {
            if (!createCommandBuffer(it, next, target, &lastEvent)) {
                LOG_E("Failed to initialize VPUCommandBuffer");
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
                                 VPUCommandBuffer::Target cmdType,
                                 VPUEventCommand::KMDEventDataType **lastEvent) {
    auto cmdBuffer = VPUCommandBuffer::allocateCommandBuffer(ctx, begin, end, cmdType, lastEvent);
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
                "%s Command Buffer (%p): execution is completed with success",
                cmdBuffer->getName(),
                cmdBuffer.get());
        } else {
            LOG_E("%s Command Buffer (%p): execution failed with status %#lx",
                  cmdBuffer->getName(),
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

    if (cmd->isComputeCommand() && isCopyOnly) {
        LOG_E("Command(%#x) is of compute type and cannot be appended to copy-only list!",
              cmd->getCommandType());
        return false;
    }

    commands.push_back(cmd);
    return true;
}

std::pair<std::vector<std::shared_ptr<VPUCommand>>::iterator, VPUCommandBuffer::Target>
VPUJob::scheduleCommands(std::vector<std::shared_ptr<VPUCommand>>::iterator begin) {
    std::optional<VPUCommandBuffer::Target> target;

    auto it = begin;
    for (; it != commands.end(); it++) {
        const auto &cmd = *it;

        if (cmd->isForwardCommand()) {
            if (target.has_value() && it + 1 != commands.end()) {
                const auto &nextCmd = *(it + 1);
                if ((nextCmd->isCopyCommand() && target != VPUCommandBuffer::Target::COPY) ||
                    (nextCmd->isComputeCommand() && target != VPUCommandBuffer::Target::COMPUTE)) {
                    break;
                }
            }
        } else if (cmd->isComputeCommand()) {
            if (target.has_value()) {
                if (target != VPUCommandBuffer::Target::COMPUTE) {
                    break;
                }
            } else {
                target = VPUCommandBuffer::Target::COMPUTE;
            }
        } else if (cmd->isCopyCommand()) {
            if (target.has_value()) {
                if (target != VPUCommandBuffer::Target::COPY) {
                    break;
                }
            } else {
                target = VPUCommandBuffer::Target::COPY;
            }
        } else if (cmd->isSynchronizeCommand()) {
            it++;
            break;
        }
    }

    if (!target.has_value()) {
        if (isCopyOnly) {
            target = VPUCommandBuffer::Target::COPY;
        } else {
            target = VPUCommandBuffer::Target::COMPUTE;
        }
    }

    return {it, *target};
}

} // namespace VPU
