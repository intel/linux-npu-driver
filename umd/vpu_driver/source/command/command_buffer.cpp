/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

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
#include <errno.h>
#include <limits>
#include <string.h>
#include <utility>

namespace VPU {

VPUCommandBuffer::VPUCommandBuffer(VPUDeviceContext *ctx,
                                   std::shared_ptr<VPUBufferObject> bufferIn,
                                   const std::vector<std::shared_ptr<VPUCommand>>::iterator &begin,
                                   const std::vector<std::shared_ptr<VPUCommand>>::iterator &end)
    : ctx(ctx)
    , buffer(std::move(bufferIn))
    , jobStatus(std::numeric_limits<uint32_t>::max())
    , commandsBegin(begin)
    , commandsEnd(end) {
    bufferHandles.emplace_back(buffer->getHandle());
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

        if (!cmdBuffer->addCommand(cmd.get(), cmdOffset, descOffset)) {
            LOG_E("Failed to append command to buffer");
            return nullptr;
        }
    }

    auto nopCmd = VPUNopCommand::create(ctx->getDeviceCapabilities(), sizeof(vpu_cmd_fence_t));
    if (!cmdBuffer->addCommand(nopCmd.get(), cmdOffset, descOffset)) {
        LOG_E("Failed to append NOP command to the command buffer");
        return nullptr;
    }

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
    return true;
}

void VPUCommandBuffer::addUniqueBoHandle(uint32_t handle) {
    auto it = std::find(bufferHandles.begin(), bufferHandles.end(), handle);
    if (it == bufferHandles.end())
        bufferHandles.emplace_back(handle);
}

bool VPUCommandBuffer::addCommand(VPUCommand *cmd, size_t &cmdOffset, size_t &descOffset) {
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
        auto &inference = dynamic_cast<VPUInferenceExecute &>(*cmd);
        inferenceScratchSize = std::max(inferenceScratchSize, inference.getSharedScratchSize());
    }

    return true;
}

bool VPUCommandBuffer::addWaitAtHead(std::shared_ptr<VPUBufferObject> waitBo, bool resetFence) {
    vpu_cmd_buffer_header_t *cmdHeader = reinterpret_cast<vpu_cmd_buffer_header_t *>(
        buffer->getBasePointer() + offsetof(CommandHeader, header));

    if (!waitBo) {
        cmdHeader->cmd_offset =
            offsetof(CommandHeader, commandList) - offsetof(CommandHeader, header);
        waitBoGuard = nullptr;
        return true;
    }

    size_t cmdOffset = offsetof(CommandHeader, internalSync);
    size_t descOffset = 0;
    VPUEventCommand::KMDEventDataType *waitFence =
        &reinterpret_cast<CommandHeader *>(waitBo->getBasePointer())->fenceValue;

    auto waitCmd = VPUEventWaitCommand::create(waitFence, waitBo);
    if (waitCmd == nullptr) {
        LOG_E("Failed to initialize synchronization wait command");
        return false;
    }
    if (!addCommand(waitCmd.get(), cmdOffset, descOffset)) {
        LOG_E("Failed to append synchronization wait command to buffer");
        return false;
    }

    if (resetFence) {
        auto resetFenceCmd = VPUEventResetCommand::create(waitFence, waitBo);

        if (!addCommand(resetFenceCmd.get(), cmdOffset, descOffset)) {
            LOG_E("Failed to append synchronization reset command to buffer");
            return false;
        }
    } else {
        auto nopCmd = VPUNopCommand::create(ctx->getDeviceCapabilities(), sizeof(vpu_cmd_fence_t));
        if (!addCommand(nopCmd.get(), cmdOffset, descOffset)) {
            LOG_E("Failed to append NOP command at the end of buffer");
            return false;
        }
    }
    waitBoGuard = std::move(waitBo);

    cmdHeader->cmd_offset = offsetof(CommandHeader, internalSync) - offsetof(CommandHeader, header);
    return true;
}

bool VPUCommandBuffer::addSelfSignalAtTail() {
    auto commandHeader = reinterpret_cast<CommandHeader *>(buffer->getBasePointer());
    size_t descOffset = 0;
    size_t cmdOffset = offsetof(CommandHeader, header) + commandHeader->header.cmd_buffer_size -
                       sizeof(vpu_cmd_fence_t);

    auto signalCmd = VPUEventSignalCommand::create(&commandHeader->fenceValue, buffer);

    if (!addCommand(signalCmd.get(), cmdOffset, descOffset)) {
        LOG_E("Failed to append synchronization signal command to buffer");
        return false;
    }
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

bool VPUCommandBuffer::waitForCompletion(int64_t timeout_abs_ns) {
    drm_ivpu_bo_wait args = {};
    args.handle = buffer->getHandle();
    args.timeout_ns = timeout_abs_ns;
    args.job_status = std::numeric_limits<uint32_t>::max();

    int ret = ctx->getDriverApi().wait(&args);
    LOG(VPU_CMD, "Wait completed: ret = %d, errno = %d, commandBuffer: %p", ret, errno, this);
    if (ret != 0)
        return false;

    jobStatus = args.job_status;
    inferenceScratchBuffer.reset();

    if (preemptionBuffer && preemptionBufferIndex.has_value()) {
        bufferHandles.erase(bufferHandles.begin() + preemptionBufferIndex.value());
        preemptionBufferIndex.reset();
        preemptionBuffer.reset();
    }
    return true;
}

void VPUCommandBuffer::resetFenceValue() {
    if (buffer == nullptr)
        return;
    CommandHeader *cmdHeader = reinterpret_cast<CommandHeader *>(buffer->getBasePointer());
    if (cmdHeader)
        cmdHeader->fenceValue = VPUEventCommand::State::STATE_HOST_RESET;
}

void VPUCommandBuffer::printCommandBuffer(const char *description) const {
    if (getLogLevel() < LogLevel::INFO)
        return;

    uint8_t *bufferPtr = buffer->getBasePointer();
    vpu_cmd_buffer_header_t *cmdHeader =
        reinterpret_cast<vpu_cmd_buffer_header_t *>(bufferPtr + offsetof(CommandHeader, header));
    LOG(VPU_CMD,
        "Start command buffer printing(%s):\n"
        "\t&vpu_cmd_buffer_header_t: %p, (vpu: %#lx)\n"
        "\tcmd_buffer_size: %#x, cmd_offset: %#x, context_save_area_address: %#lx\n",
        description ? description : "",
        cmdHeader,
        buffer->getVPUAddr(cmdHeader),
        cmdHeader->cmd_buffer_size,
        cmdHeader->cmd_offset,
        cmdHeader->context_save_area_address);

    uint32_t cmdOffset = cmdHeader->cmd_offset;
    int i = 0;
    while (cmdOffset < cmdHeader->cmd_buffer_size) {
        vpu_cmd_header_t *cmd = reinterpret_cast<vpu_cmd_header_t *>(
            bufferPtr + offsetof(CommandHeader, header) + cmdOffset);

        if (cmd->size <= 0 || cmd->size >= cmdHeader->cmd_buffer_size) {
            LOG_E("Invalid command size: %u, stop command buffer printing", cmd->size);
            return;
        }

        switch (cmd->type) {
        case VPU_CMD_TIMESTAMP:
            LOG(VPU_CMD,
                "Command %i: Timestamp (size: %u bytes)\n"
                "\ttimestamp_address = %#lx",
                i,
                cmd->size,
                reinterpret_cast<vpu_cmd_timestamp_t *>(cmd)->timestamp_address);
            break;
        case VPU_CMD_FENCE_WAIT:
            LOG(VPU_CMD,
                "Command %i: Fence Wait (size: %u bytes)\n"
                "\toffset = %#lx, value = %#lx",
                i,
                cmd->size,
                reinterpret_cast<vpu_cmd_fence_t *>(cmd)->offset,
                reinterpret_cast<vpu_cmd_fence_t *>(cmd)->value);
            break;
        case VPU_CMD_FENCE_SIGNAL:
            LOG(VPU_CMD,
                "Command %i: Fence Signal (size: %u bytes)\n"
                "\toffset = %#lx, value = %#lx",
                i,
                cmd->size,
                reinterpret_cast<vpu_cmd_fence_t *>(cmd)->offset,
                reinterpret_cast<vpu_cmd_fence_t *>(cmd)->value);
            break;
        case VPU_CMD_BARRIER:
            LOG(VPU_CMD, "Command %i: Barrier (size: %u bytes)", i, cmd->size);
            break;
        case VPU_CMD_METRIC_QUERY_BEGIN:
            LOG(VPU_CMD,
                "Command %i: Metric Query Begin (size: %u bytes)\n"
                "\tmetric_group_type = %u, metric_data_address = %#lx",
                i,
                cmd->size,
                reinterpret_cast<vpu_cmd_metric_query_t *>(cmd)->metric_group_type,
                reinterpret_cast<vpu_cmd_metric_query_t *>(cmd)->metric_data_address);
            break;
        case VPU_CMD_METRIC_QUERY_END:
            LOG(VPU_CMD,
                "Command %i: Metric Query End (size: %u bytes)\n"
                "\tmetric_group_type = %u, metric_data_address = %#lx",
                i,
                cmd->size,
                reinterpret_cast<vpu_cmd_metric_query_t *>(cmd)->metric_group_type,
                reinterpret_cast<vpu_cmd_metric_query_t *>(cmd)->metric_data_address);
            break;
        case VPU_CMD_COPY_LOCAL_TO_LOCAL:
            LOG(VPU_CMD,
                "Command %i: Copy Local to Local (size: %u bytes)\n"
                "\tdesc_start_offset = %#lx, desc_count = %u",
                i,
                cmd->size,
                reinterpret_cast<vpu_cmd_copy_buffer_t *>(cmd)->desc_start_offset,
                reinterpret_cast<vpu_cmd_copy_buffer_t *>(cmd)->desc_count);
            ctx->printCopyDescriptor(
                bufferPtr + (reinterpret_cast<vpu_cmd_copy_buffer_t *>(cmd)->desc_start_offset -
                             buffer->getVPUAddr()),
                cmd);
            break;
        case VPU_CMD_INFERENCE_EXECUTE:
            LOG(VPU_CMD,
                "Command %i: Inference Execute (size %u bytes)\n"
                "\tinference_id = %lu, host_mapped_inference.address = %#lx, "
                "host_mapped_inference.size = %#x",
                i,
                cmd->size,
                reinterpret_cast<vpu_cmd_inference_execute_t *>(cmd)->inference_id,
                reinterpret_cast<vpu_cmd_inference_execute_t *>(cmd)->host_mapped_inference.address,
                reinterpret_cast<vpu_cmd_inference_execute_t *>(cmd)->host_mapped_inference.width);
            break;
        case VPU_CMD_MEMORY_FILL:
            LOG(VPU_CMD,
                "Command %i: Memory fill (size: %u bytes) pattern = 0x%x addr=0x%lx",
                i,
                cmd->size,
                reinterpret_cast<vpu_cmd_memory_fill_t *>(cmd)->fill_pattern,
                reinterpret_cast<vpu_cmd_memory_fill_t *>(cmd)->start_address);
            break;
        case VPU_CMD_NOP:
            LOG(VPU_CMD, "Command %i: NOP (size: %u bytes)", i, cmd->size);
            break;
        default:
            LOG_E("Unknown command, stop command buffer printing");
            return;
        }
        cmdOffset += cmd->size;
        i++;
    }
    LOG(VPU_CMD, "Stop command buffer printing");
}

bool VPUCommandBuffer::replaceBufferHandles(const std::vector<uint32_t> &oldHandles,
                                            const std::vector<uint32_t> &newHandles) {
    if (oldHandles.size() != newHandles.size()) {
        LOG_E("Incorrect number of handles passed to replaceBufferHandles");
        return false;
    }

    bufferHandles.erase(std::remove_if(bufferHandles.begin(),
                                       bufferHandles.end(),
                                       [&oldHandles](auto x) {
                                           return std::find(oldHandles.begin(),
                                                            oldHandles.end(),
                                                            x) != oldHandles.end();
                                       }),
                        bufferHandles.end());
    for (auto &handle : newHandles) {
        addUniqueBoHandle(handle);
    }
    return true;
}

bool VPUCommandBuffer::updateCommands() {
    if (inferenceScratchSize) {
        inferenceScratchBuffer = ctx->scratchCacheAcquire(inferenceScratchSize);
        if (inferenceScratchBuffer == nullptr) {
            LOG_E("Failed to allocate scratch buffer");
            return false;
        }
    }

    for (auto it = commandsBegin; it != commandsEnd; it++) {
        std::shared_ptr<VPUCommand> &cmd = *it;
        if (cmd->getCommandType() == VPU_CMD_INFERENCE_EXECUTE) {
            auto &inference = dynamic_cast<VPUInferenceExecute &>(*cmd);
            inference.updateScratchBuffer(this, inferenceScratchBuffer);
        }

        if (!cmd->update(this)) {
            return false;
        }
    }
    return true;
}

void VPUCommandBuffer::addPreemptionBuffer(std::shared_ptr<VPUBufferObject> bo) {
    if (preemptionBuffer) {
        LOG_E("Preemption buffer is already set, cannot add another one");
        return;
    }
    if (bo == nullptr) {
        LOG_E("Preemption buffer is nullptr");
        return;
    }

    preemptionBuffer = std::move(bo);
    preemptionBufferIndex = static_cast<uint32_t>(bufferHandles.size());
    bufferHandles.emplace_back(preemptionBuffer->getHandle());
}

} // namespace VPU
