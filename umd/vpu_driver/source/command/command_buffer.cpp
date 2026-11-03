/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include <bits/chrono.h>
// IWYU pragma: no_include <emmintrin.h>

#include "vpu_driver/source/command/command_buffer.hpp"

#include "umd_common.hpp"
#include "vpu_driver/source/command/command.hpp"
#include "vpu_driver/source/command/inference_execute.hpp"
#include "vpu_driver/source/command/nop_command.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <algorithm>
#include <chrono> // IWYU pragma: keep
#include <compare>
#include <errno.h>
#include <immintrin.h>
#include <limits>
#include <ratio>
#include <string.h>
#include <uapi/drm/ivpu_accel.h>
#include <utility>

namespace VPU {

VPUCommandBuffer::VPUCommandBuffer(VPUDeviceContext *ctx,
                                   std::shared_ptr<VPUBufferObject> bufferIn,
                                   const std::vector<std::shared_ptr<VPUCommand>>::iterator &begin,
                                   const std::vector<std::shared_ptr<VPUCommand>>::iterator &end)
    : ctx(ctx)
    , buffer(std::move(bufferIn))
    , jobStatus(std::numeric_limits<uint32_t>::max())
    , commands(begin, end) {
    bufferHandles.insert(buffer->getHandle());
}

std::unique_ptr<VPUCommandBuffer> VPUCommandBuffer::allocateCommandBuffer(
    VPUDeviceContext *ctx,
    const std::vector<std::shared_ptr<VPUCommand>>::iterator &begin,
    const std::vector<std::shared_ptr<VPUCommand>>::iterator &end) {
    if (ctx == nullptr || begin == end) {
        LOG_E("VPUDeviceContext is nullptr or command list is empty");
        return nullptr;
    }

    size_t cmdSize = 0;
    size_t descriptorSize = 0;

    for (auto it = begin; it != end; it++) {
        const auto &cmd = *it;
        cmdSize += cmd->getCommitSize();
        descriptorSize += getFwDataCacheAlign(cmd->getDescriptorSize());
    }

    cmdSize += sizeof(vpu_cmd_fence_t);

    size_t cmdOffset = offsetof(CommandHeader, commandList);
    size_t descOffset = getFwDataCacheAlign(cmdOffset + cmdSize);
    size_t cmdBufferSize = descOffset + descriptorSize + ctx->getExtraDmaDescriptorSize();

    std::shared_ptr<VPUBufferObject> buffer =
        ctx->createUntrackedBufferObject(cmdBufferSize, VPUBufferObject::Type::CachedFw);
    if (buffer == nullptr) {
        LOG_E("Failed to allocate buffer object for command buffer");
        return nullptr;
    }

    auto cmdBuffer = std::make_unique<VPUCommandBuffer>(ctx, buffer, begin, end);
    if (!cmdBuffer->initHeader(cmdSize)) {
        LOG_E("Failed to initialize VPUCommandBuffer");
        return nullptr;
    }

    for (auto it = begin; it != end; it++) {
        const auto &cmd = *it;
        if (cmd->isSynchronizeCommand() && !cmdBuffer->setSyncFenceAddr(cmd.get())) {
            LOG_E("Failed to set synchronize fence vpu addresss");
            return nullptr;
        }

        if (!cmdBuffer->addCommand(cmd, cmdOffset, descOffset)) {
            LOG_E("Failed to append command to buffer");
            return nullptr;
        }
    }

    auto nopCmd = VPUNopCommand::create(ctx->getDeviceCapabilities(), sizeof(vpu_cmd_fence_t));
    if (!cmdBuffer->addCommand(nopCmd, cmdOffset, descOffset)) {
        LOG_E("Failed to append NOP command to the command buffer");
        return nullptr;
    }
    cmdBuffer->commands.push_back(std::move(nopCmd));

    if (cmdOffset != offsetof(CommandHeader, commandList) + cmdSize) {
        LOG_E("Invalid size of command list");
        return nullptr;
    }

    if (descOffset !=
        getFwDataCacheAlign(offsetof(CommandHeader, commandList) + cmdSize) + descriptorSize) {
        LOG_E("Invalid size of descriptor");
        return nullptr;
    }

    return cmdBuffer;
}

bool VPUCommandBuffer::initHeader(size_t cmdSize) {
    if (buffer == nullptr) {
        LOG_E("Invalid command buffer pointer is passed");
        return false;
    }

    vpu_cmd_buffer_header_t *bb = reinterpret_cast<vpu_cmd_buffer_header_t *>(
        buffer->getBasePointer() + offsetof(CommandHeader, header));

    // Init memory - required for backward/forward compatibility with the firmware
    memset(buffer->getBasePointer(), 0, buffer->getAllocSize());

    /* By default command offset is set to CommandHeader.commandList where user commands begins,
     * when internal synchronization is used offset is changed to CommandHeader.internalSync where
     * are synchronization commands added by driver
     */
    bb->cmd_offset = offsetof(CommandHeader, commandList) - offsetof(CommandHeader, header);
    bb->cmd_buffer_size = safe_cast<uint32_t>(bb->cmd_offset + cmdSize);

    bb->context_save_area_address = buffer->getVPUAddr() + offsetof(CommandHeader, contextSaveArea);
    bb->api_version = VPU_JSM_JOB_CMD_API_VER_MAJOR << 16 | VPU_JSM_JOB_CMD_API_VER_MINOR;
    return true;
}

void VPUCommandBuffer::addUniqueBoHandle(uint32_t handle) {
    bufferHandles.insert(handle);
}

bool VPUCommandBuffer::addCommand(std::shared_ptr<VPUCommand> cmd,
                                  size_t &cmdOffset,
                                  size_t &descOffset) {
    if (cmd == nullptr) {
        LOG_E("Command is nullptr or command is not initialized");
        return false;
    }

    LOG(VPU_CMD,
        "Attempting append a command %#x (size: %zu) to command buffer",
        cmd->getCommandType(),
        cmd->getCommitSize());

    if (descOffset && cmdOffset >= descOffset) {
        LOG_E("Command override the descriptor");
        return false;
    }

    for (const auto &bo : cmd->getAssociateBufferObjects()) {
        addUniqueBoHandle(bo->getHandle());
    }

    if (descOffset && cmd->getDescriptorSize() > 0) {
        if (!buffer->copyToBuffer(cmd->getDescriptorData(), cmd->getDescriptorSize(), descOffset)) {
            LOG_E("Failed to update offset in command");
            return false;
        }

        cmd->patchDescriptorAddress(buffer->getVPUAddr() + descOffset);
        descOffset += getFwDataCacheAlign(cmd->getDescriptorSize());
    }

    if (!buffer->copyToBuffer(cmd->getCommitStream(), cmd->getCommitSize(), cmdOffset)) {
        LOG_E("Failed to copy command structure to command buffer");
        return false;
    }

    LOG(VPU_CMD, "Command appended to command buffer: cmdOffset: %zu", cmdOffset);

    cmdOffset += cmd->getCommitSize();

    if (cmd->getCommandType() == VPU_CMD_INFERENCE_EXECUTE) {
        auto inference = dynamic_cast<VPUInferenceExecute *>(cmd.get());
        for (auto [type, size] : inference->getSharedScratchSize()) {
            inferenceScratchSize[type] = std::max(inferenceScratchSize[type], size);
        }
    }

    return true;
}

bool VPUCommandBuffer::addWaitAtHead(std::shared_ptr<VPUBufferObject> waitBo, bool resetFence) {
    vpu_cmd_buffer_header_t *cmdHeader = reinterpret_cast<vpu_cmd_buffer_header_t *>(
        buffer->getBasePointer() + offsetof(CommandHeader, header));

    const auto cmdOffsetDefault =
        offsetof(CommandHeader, commandList) - offsetof(CommandHeader, header);
    const auto cmdOffsetInSync =
        offsetof(CommandHeader, internalSync) - offsetof(CommandHeader, header);

    // Remove previously added wait and reset or nop commands
    if (cmdHeader->cmd_offset == cmdOffsetInSync) {
        if (commands.size() < 2) {
            LOG_E("Command buffer has unexpected number of commands");
            return false;
        }
        auto waitCmdIt = commands.begin();
        if (waitCmdIt->get() == nullptr ||
            waitCmdIt->get()->getCommandType() != VPU_CMD_FENCE_WAIT) {
            LOG_E("Failed to find previously added wait command in the command buffer");
            return false;
        }
        if (waitCmdIt->get()->getAssociateBufferObjects().size() != 1) {
            LOG_E("Wait command has unexpected number of associated buffer objects");
            return false;
        }

        // remove wait and reset/nop command
        for (int i = 0; i < 2; i++) {
            for (const auto &bo : commands.front()->getAssociateBufferObjects()) {
                removeBufferHandle(bo->getHandle());
            }
            commands.pop_front();
        }
        cmdHeader->cmd_offset = cmdOffsetDefault;
    }

    // Nothing to wait for
    if (!waitBo) {
        return true;
    }

    // Add wait and reset or nop commands to the head of the command buffer
    size_t cmdOffset = offsetof(CommandHeader, internalSync);
    size_t descOffset = 0;
    VPUEventCommand::KMDEventDataType *waitFence =
        &reinterpret_cast<CommandHeader *>(waitBo->getBasePointer())->fenceValue;
    auto waitCmd = VPUEventWaitCommand::create(waitFence, waitBo);
    if (!addCommand(waitCmd, cmdOffset, descOffset)) {
        LOG_E("Failed to append synchronization wait command to buffer");
        return false;
    }

    auto resetCmd = std::shared_ptr<VPUCommand>();
    if (resetFence) {
        resetCmd = VPUEventResetCommand::create(waitFence, std::move(waitBo));
    } else {
        resetCmd = VPUNopCommand::create(ctx->getDeviceCapabilities(), sizeof(vpu_cmd_fence_t));
    }
    if (!addCommand(resetCmd, cmdOffset, descOffset)) {
        LOG_E("Failed to append synchronization reset command to buffer");
        return false;
    }

    commands.push_front(std::move(resetCmd));
    commands.push_front(std::move(waitCmd));
    cmdHeader->cmd_offset = cmdOffsetInSync;
    return true;
}

bool VPUCommandBuffer::addSelfSignalAtTail() {
    if (commands.empty()) {
        LOG_E("Command buffer is empty, cannot add self signal command");
        return false;
    }
    if (commands.back()->getCommandType() == VPU_CMD_FENCE_SIGNAL) {
        LOG_W("Self signal command is already added to the command buffer");
        return true;
    }
    if (commands.back()->getCommandType() != VPU_CMD_NOP) {
        LOG_E("Self signal command can be replaced by NOP command, last command type: %#x",
              commands.back()->getCommandType());
        return false;
    }

    auto commandHeader = reinterpret_cast<CommandHeader *>(buffer->getBasePointer());
    size_t descOffset = 0;
    size_t cmdOffset = offsetof(CommandHeader, header) + commandHeader->header.cmd_buffer_size -
                       sizeof(vpu_cmd_fence_t);

    auto signalCmd = VPUEventSignalCommand::create(&commandHeader->fenceValue, buffer);
    if (!addCommand(signalCmd, cmdOffset, descOffset)) {
        LOG_E("Failed to append synchronization signal command to buffer");
        return false;
    }
    commands.back() = std::move(signalCmd);
    return true;
}

bool VPUCommandBuffer::setSyncFenceAddr(VPUCommand *cmd) {
    if (syncFenceVpuAddr != 0) {
        LOG_E("Synchronize Fence VPU Address is already set");
        return false;
    }

    if (cmd->getCommandType() != VPU_CMD_FENCE_SIGNAL) {
        LOG_E("Not supported command type for synchronize command");
        return false;
    }

    auto *fenceSignalHeader = reinterpret_cast<const vpu_cmd_fence_t *>(cmd->getCommitStream());
    syncFenceVpuAddr = fenceSignalHeader->offset;
    return true;
}

void VPUCommandBuffer::useBusyWait() {
    if (!addSelfSignalAtTail())
        return;

    resetFenceValue();
    useBusyWaitFlag = true;
}

bool VPUCommandBuffer::waitForCompletion(int64_t timeout_abs_ns) {
    if (useBusyWaitFlag)
        busyWait(timeout_abs_ns, VPUDeviceContext::getCpuTscFreqMHz());

    bool result = wait(timeout_abs_ns);
    if (!result)
        return false;

    useBusyWaitFlag = false;
    for (auto &[_, bo] : inferenceScratchBuffers) {
        bo.reset();
    }

    if (preemptionBuffer) {
        bufferHandles.erase(preemptionBuffer->getHandle());
        preemptionBuffer.reset();
    }
    return true;
}

bool VPUCommandBuffer::wait(int64_t timeout_abs_ns) {
    drm_ivpu_bo_wait args = {};
    args.handle = buffer->getHandle();
    args.timeout_ns = timeout_abs_ns;
    args.job_status = std::numeric_limits<uint32_t>::max();

    int ret = ctx->getDriverApi().wait(&args);
    LOG(VPU_CMD, "Wait completed: ret = %d, errno = %d, commandBuffer: %p", ret, errno, this);
    if (ret != 0)
        return false;

    jobStatus = args.job_status;
    return true;
}

void VPUCommandBuffer::busyWait(int64_t timeout_abs_ns, uint32_t tscFreqMHz) {
    /* Maximum period of busy waiting is 15ms then we enter normal wait */
    auto timeoutNs =
        std::min(std::chrono::steady_clock::time_point(std::chrono::nanoseconds(timeout_abs_ns)) -
                     std::chrono::steady_clock::now(),
                 std::chrono::nanoseconds(15'000'000));

    if (timeoutNs <= std::chrono::nanoseconds::zero())
        return;

    CommandHeader *cmdHeader = reinterpret_cast<CommandHeader *>(buffer->getBasePointer());
    if (cmdHeader && cmdHeader->fenceValue != VPUEventCommand::State::STATE_DEVICE_SIGNAL) {
        // Use UMONITOR/UMWAIT to do efficient busy wait
        // UMWAIT waits until the monitored address is written or the timeout expires
        // The timeout is specified in CPU clock ticks
        unsigned long long timeoutTime =
            __rdtsc() + ((static_cast<unsigned long long>(timeoutNs.count()) *
                          static_cast<unsigned long long>(tscFreqMHz)) /
                         1'000ULL);
        do {
            if (__rdtsc() >= timeoutTime)
                break;

            _umonitor(&cmdHeader->fenceValue);
            _umwait(0, timeoutTime);
            _mm_mfence(); // ensure that the read is not moved before the UMWAIT
        } while (cmdHeader->fenceValue != VPUEventCommand::State::STATE_DEVICE_SIGNAL);
    }

    return;
}

void VPUCommandBuffer::resetFenceValue() {
    if (buffer == nullptr)
        return;
    CommandHeader *cmdHeader = reinterpret_cast<CommandHeader *>(buffer->getBasePointer());
    if (cmdHeader)
        cmdHeader->fenceValue = VPUEventCommand::State::STATE_HOST_RESET;
}

const VPUCommandBuffer::BufferHandles &VPUCommandBuffer::getBufferHandles() {
    bufferHandles.insertResidentSet(ctx->getResidentSet());
    return bufferHandles;
}

bool VPUCommandBuffer::replaceBufferHandles(const std::vector<uint32_t> &oldHandles,
                                            const std::vector<uint32_t> &newHandles) {
    if (oldHandles.size() != newHandles.size()) {
        LOG_E("Incorrect number of handles passed to replaceBufferHandles");
        return false;
    }

    for (auto handle : oldHandles) {
        bufferHandles.erase(handle);
    }
    for (auto &handle : newHandles) {
        addUniqueBoHandle(handle);
    }
    return true;
}

void VPUCommandBuffer::removeBufferHandle(const uint32_t handle) {
    bufferHandles.erase(handle);
}

bool VPUCommandBuffer::updateCommands() {
    for (auto [type, size] : inferenceScratchSize) {
        if (size == 0)
            continue;
        inferenceScratchBuffers[type] = ctx->scratchCacheAcquire(size, type);
        if (inferenceScratchBuffers[type] == nullptr) {
            LOG_E("Failed to allocate scratch buffer");
            return false;
        }
    }

    for (auto it = commands.begin(); it != commands.end(); it++) {
        std::shared_ptr<VPUCommand> &cmd = *it;
        if (cmd->getCommandType() == VPU_CMD_INFERENCE_EXECUTE) {
            auto &inference = dynamic_cast<VPUInferenceExecute &>(*cmd);
            inference.updateScratchBuffer(this, inferenceScratchBuffers);
        }

        if (!cmd->update(this)) {
            return false;
        }
    }
    return true;
}

void VPUCommandBuffer::addPreemptionBuffer(std::shared_ptr<VPUBufferObject> bo) {
    if (preemptionBuffer) {
        LOG_W("Preemption buffer is already set, cannot add another one");
        return;
    }
    if (bo == nullptr) {
        LOG_E("Preemption buffer is nullptr");
        return;
    }

    preemptionBuffer = std::move(bo);
    bufferHandles.insert(preemptionBuffer->getHandle());
}

uint32_t VPUCommandBuffer::getPreemptionBufferIndex() const {
    if (!preemptionBuffer) {
        return 0;
    }
    auto dist = bufferHandles.position(preemptionBuffer->getHandle());
    return dist > 0 ? static_cast<uint32_t>(dist) : 0;
}

} // namespace VPU
