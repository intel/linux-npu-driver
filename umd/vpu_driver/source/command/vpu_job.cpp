/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"
#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <assert.h>

namespace VPU {

VPUJob::VPUJob(VPUDeviceContext *ctx, bool isCopyOnly)
    : ctx(ctx)
    , isCopyOnly(isCopyOnly) {}

VPUJob::~VPUJob() {
    LOG_V("Destroying VPUJob - %p", this);

    if (ctx && descriptor && !ctx->freeMemAlloc(descriptor)) {
        LOG_E("Failed to free event sync pointer");
    }
}

bool VPUJob::updateInternalEventBuffer(std::shared_ptr<VPUCommand> cmd) {
    auto cmdType = cmd->getCommandType();

    if (cmdType == VPU_CMD_FENCE_SIGNAL || cmdType == VPU_CMD_FENCE_WAIT) {
        auto eventCmd = reinterpret_cast<VPU::VPUEventCommand *>(cmd.get());
        if (eventCmd->getInternalEventIndex() != 0 &&
            !eventCmd->updateInternalEventOffsets(
                ctx,
                eventCmd->getInternalEventIndex() & 0x01 ? eventBasePtr : eventBasePtr + 1)) {
            LOG_E("Failed to update internal events offsets");
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
        LOG_W("VPUJob is closed");
        return true;
    }

    if (descriptor || cmdBuffers.size()) {
        LOG_E("Failed to close the VPUJob because of dirty state");
        return false;
    }

    flushCommands();

    size_t descriptorSize = 0;
    for (const auto &cmd : nnCmds)
        descriptorSize += getFwDataCacheAlign(cmd->getDescriptorSize());
    for (const auto &cmd : cpCmds)
        descriptorSize += getFwDataCacheAlign(cmd->getDescriptorSize());
    if (descriptorSize)
        descriptorSize += eventPoolSize;

    if (descriptorSize > 0) {
        descriptor =
            ctx->createInternalBufferObject(descriptorSize, VPUBufferObject::Type::CachedLow);
        if (descriptor == nullptr) {
            LOG_E("Failed to allocate descriptor buffer");
            return false;
        }
        descriptorPtr = descriptor->getBasePointer();

        eventBasePtr = reinterpret_cast<decltype(eventBasePtr)>(
            descriptor->getBasePointer() + descriptor->getAllocSize() - eventPoolSize);
        for (const auto &cmd : nnCmds)
            if (!updateInternalEventBuffer(cmd))
                return false;
        for (const auto &cmd : cpCmds)
            if (!updateInternalEventBuffer(cmd))
                return false;
    }

    if (!createCommandBuffer(nnCmds, VPUCommandBuffer::Target::COMPUTE)) {
        LOG_E("Failed to initialize COMPUTE VPUCommandBuffer");
        return false;
    }

    if (!createCommandBuffer(cpCmds, VPUCommandBuffer::Target::COPY)) {
        LOG_E("Failed to initialize COPY VPUCommandBuffer");
        return false;
    }

    closed = true;
    return true;
}

bool VPUJob::createCommandBuffer(const std::vector<std::shared_ptr<VPUCommand>> &cmds,
                                 VPUCommandBuffer::Target cmdType) {
    if (cmds.size() == 0)
        return true;

    void *descriptorEnd = nullptr;
    if (descriptor)
        descriptorEnd = descriptor->getBasePointer() + descriptor->getAllocSize();

    auto cmdBuffer =
        VPUCommandBuffer::allocateCommandBuffer(ctx, cmds, &descriptorPtr, descriptorEnd, cmdType);
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

void VPUJob::printResult() const {
    for (const auto &cmdBuffer : cmdBuffers) {
        if (cmdBuffer->isSuccess()) {
            LOG_V("%s Command Buffer (%p): execution is completed with success",
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

bool VPUJob::appendInternalEvents(std::vector<std::shared_ptr<VPUCommand>> &fromCmds,
                                  std::vector<std::shared_ptr<VPUCommand>> &toCmds) {
    if (ctx == nullptr) {
        LOG_E("VPUDeviceContext is nullptr");
        return false;
    }

    auto signalCmd = VPUEventSignalCommand::create(intEventIndex);
    if (signalCmd == nullptr) {
        LOG_E("Failed to initialize signal event Command.");
        return false;
    }
    fromCmds.push_back(signalCmd);

    auto waitCmd = VPUEventWaitCommand::create(intEventIndex);
    if (waitCmd == nullptr) {
        LOG_E("Failed to initialize wait event Command.");
        return false;
    }
    toCmds.push_back(waitCmd);

    auto resetCmd = VPUEventResetCommand::create(intEventIndex);
    if (resetCmd == nullptr) {
        LOG_E("Failed to initialize reset event Command.");
        return false;
    }
    toCmds.push_back(resetCmd);

    intEventIndex = (intEventIndex == 1u) ? 2u : 1u;

    return true;
}

void VPUJob::moveCommands(std::vector<std::shared_ptr<VPUCommand>> &dst,
                          std::vector<std::shared_ptr<VPUCommand>> &src) {
    std::move(src.begin(), src.end(), std::back_inserter(dst));
    src.clear();
}

void VPUJob::flushCommands() {
    if (unclassified.size()) {
        if (isCopyOnly) {
            assert(nnCmds.empty());
            moveCommands(cpCmds, unclassified);
            return;
        }

        if (prevCmds == nullptr) {
            moveCommands(nnCmds, unclassified);
        } else {
            moveCommands(*prevCmds, unclassified);
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

    unclassified.push_back(cmd);
    if (!cmd->isCommandAgnostic()) {
        if (cmd->isComputeCommand()) {
            if (isCopyOnly) {
                LOG_E("Command(%#x) is of compute type and cannot be appended to copy-only list!",
                      cmd->getCommandType());
                return false;
            }

            if (prevCmds && prevCmds != &nnCmds && !appendInternalEvents(*prevCmds, nnCmds)) {
                return false;
            }
            prevCmds = &nnCmds;
        } else {
            if (prevCmds && prevCmds != &cpCmds && !appendInternalEvents(*prevCmds, cpCmds)) {
                return false;
            }
            prevCmds = &cpCmds;
        }
        moveCommands(*prevCmds, unclassified);
    } else {
        if (cmd->isBackwardCommand() && prevCmds) {
            moveCommands(*prevCmds, unclassified);
        }
    }

    return true;
}

} // namespace VPU
