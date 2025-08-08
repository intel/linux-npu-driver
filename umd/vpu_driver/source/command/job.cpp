/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/job.hpp"

#include "vpu_driver/source/utilities/log.hpp"

#include <iterator>
#include <uapi/drm/ivpu_accel.h>
#include <utility>

namespace VPU {
class VPUBufferObject;
class VPUDeviceContext;

VPUJob::VPUJob(VPUDeviceContext *ctx)
    : ctx(ctx) {}

bool VPUJob::updateOnSubmit() {
    for (auto &cmdBuffer : cmdBuffers) {
        if (!cmdBuffer->updateCommands()) {
            return false;
        }
    }

    return true;
}

bool VPUJob::closeCommands() {
    if (ctx == nullptr) {
        LOG_E("VPUDeviceContext is nullptr");
        return false;
    }

    if (isClosed()) {
        return true;
    }

    if (cmdBuffers.size()) {
        LOG_E("Failed to close the job because of dirty state");
        return false;
    }

    LOG(VPU_JOB, "Schedule commands, number of commands %lu", commands.size());

    std::shared_ptr<VPUBufferObject> lastEventBo;
    for (auto it = commands.begin(); it != commands.end();) {
        auto next = scheduleCommands(it);

        LOG(VPU_JOB, "Passing %lu commands to command buffer", std::distance(it, next));

        if (!createCommandBuffer(it, next, lastEventBo, next == commands.end() ? true : false)) {
            LOG_E("Failed to initialize command buffer");
            return false;
        }
        it = next;
    }

    closed = true;
    return true;
}

bool VPUJob::makeInOrder(std::shared_ptr<VPUBufferObject> &waitFor) {
    if (cmdBuffers.empty())
        return false;

    if (waitFor.get() == cmdBuffers.front()->getBuffer().get()) {
        if (!cmdBuffers.front()->waitForCompletion(INT64_MAX))
            return false;
        cmdBuffers.front()->resetFenceValue();
        waitFor = nullptr;
    }

    if (!cmdBuffers.front()->addWaitAtHead(waitFor, true)) {
        LOG_E("Failed to append synchronization to the first command buffer");
        return false;
    }
    if (!cmdBuffers.back()->addSelfSignalAtTail()) {
        LOG_E("Failed to append synchronization to the last command buffer");
        return false;
    }

    waitFor = cmdBuffers.back()->getBuffer();
    hasInOrderWorkload = true;

    cmdBuffers.front()->printCommandBuffer("InOrder patching(front)");
    if (cmdBuffers.front() != cmdBuffers.back()) {
        cmdBuffers.back()->printCommandBuffer("InOrder patching(back)");
    }

    return true;
}

bool VPUJob::stripInOrder() {
    if (!isClosed()) {
        return false;
    }
    cmdBuffers.clear();
    hasInOrderWorkload = false;
    closed = false;
    return closeCommands();
}

bool VPUJob::createCommandBuffer(const std::vector<std::shared_ptr<VPUCommand>>::iterator &begin,
                                 const std::vector<std::shared_ptr<VPUCommand>>::iterator &end,
                                 std::shared_ptr<VPUBufferObject> &lastEventBo,
                                 bool last) {
    auto cmdBuffer = VPUCommandBuffer::allocateCommandBuffer(ctx, begin, end);
    if (cmdBuffer == nullptr) {
        LOG_E("Failed to allocate VPUCommandBuffer");
        return false;
    }

    if (lastEventBo) {
        cmdBuffer->addWaitAtHead(lastEventBo);
    }

    if (!last) {
        lastEventBo = cmdBuffer->getBuffer();
        cmdBuffer->addSelfSignalAtTail();
    }
    cmdBuffer->printCommandBuffer("Create");
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

    commands.push_back(std::move(cmd));
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
