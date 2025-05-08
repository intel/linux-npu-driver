/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_command_buffer.hpp"

#include "umd_common.hpp"
#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <algorithm>
#include <errno.h>
#include <limits>
#include <string.h>
#include <utility>

namespace VPU {

VPUCommandBuffer::VPUCommandBuffer(VPUDeviceContext *ctx,
                                   std::shared_ptr<VPUBufferObject> buffer,
                                   const std::vector<std::shared_ptr<VPUCommand>>::iterator &begin,
                                   const std::vector<std::shared_ptr<VPUCommand>>::iterator &end)
    : ctx(ctx)
    , buffer(std::move(buffer))
    , jobStatus(std::numeric_limits<uint32_t>::max())
    , commandsBegin(begin)
    , commandsEnd(end) {
    bufferHandles.emplace_back(VPUCommandBuffer::buffer->getHandle());
}

std::unique_ptr<VPUCommandBuffer> VPUCommandBuffer::allocateCommandBuffer(
    VPUDeviceContext *ctx,
    const std::vector<std::shared_ptr<VPUCommand>>::iterator &begin,
    const std::vector<std::shared_ptr<VPUCommand>>::iterator &end,
    VPUEventCommand::KMDEventDataType **fenceWait,
    std::shared_ptr<VPUBufferObject> &fenceBo) {
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

    if (fenceWait) {
        if (*fenceWait) {
            // Add internal fence wait command
            cmdSize += sizeof(vpu_cmd_fence_t);
        }

        // Add internal fence signal command
        cmdSize += sizeof(vpu_cmd_fence_t);
    }

    size_t cmdBufferSize = sizeof(CommandHeader) + getFwDataCacheAlign(cmdSize) + descriptorSize +
                           ctx->getExtraDmaDescriptorSize();

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

    uint64_t cmdOffset = offsetof(CommandHeader, commandList);
    uint64_t descOffset = cmdOffset + getFwDataCacheAlign(cmdSize);

    if (fenceWait && *fenceWait) {
        auto waitCmd = VPUEventWaitCommand::create(*fenceWait, fenceBo);
        if (waitCmd == nullptr) {
            LOG_E("Failed to initialize wait event command");
            return nullptr;
        }

        if (!cmdBuffer->addCommand(waitCmd.get(), cmdOffset, descOffset)) {
            LOG_E("Failed to append wait command to buffer");
            return nullptr;
        }
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

    if (fenceWait) {
        VPUEventCommand::KMDEventDataType *fenceSignal = reinterpret_cast<uint64_t *>(
            buffer->getBasePointer() + offsetof(CommandHeader, fenceValue));
        auto signalCmd = VPUEventSignalCommand::create(fenceSignal, buffer);
        if (signalCmd == nullptr) {
            LOG_E("Failed to create signal event Command");
            return nullptr;
        }

        if (!cmdBuffer->addCommand(signalCmd.get(), cmdOffset, descOffset)) {
            LOG_E("Failed to append signal command to buffer");
            return nullptr;
        }

        *fenceWait = fenceSignal;
        fenceBo = std::move(buffer);
    }

    if (cmdOffset != offsetof(CommandHeader, commandList) + cmdSize) {
        LOG_E("Invalid size of command list");
        return nullptr;
    }

    if (descOffset !=
        offsetof(CommandHeader, commandList) + getFwDataCacheAlign(cmdSize) + descriptorSize) {
        LOG_E("Invalid size of descriptor");
        return nullptr;
    }

    cmdBuffer->printCommandBuffer();
    return cmdBuffer;
}

bool VPUCommandBuffer::initHeader(size_t cmdSize) {
    if (buffer == nullptr) {
        LOG_E("Invalid command buffer pointer is passed");
        return false;
    }

    vpu_cmd_buffer_header_t *bb =
        reinterpret_cast<vpu_cmd_buffer_header_t *>(buffer->getBasePointer());

    // Init memory - required for backward/forward compatibility with the firmware
    memset(buffer->getBasePointer(), 0, buffer->getAllocSize());

    bb->cmd_offset = offsetof(CommandHeader, commandList);
    bb->cmd_buffer_size = safe_cast<uint32_t>(bb->cmd_offset + cmdSize);

    bb->context_save_area_address = buffer->getVPUAddr() + offsetof(CommandHeader, contextSaveArea);
    return true;
}

void VPUCommandBuffer::addUniqueBoHandle(uint32_t handle) {
    auto it = std::find(bufferHandles.begin(), bufferHandles.end(), handle);
    if (it == bufferHandles.end())
        bufferHandles.emplace_back(handle);
}

bool VPUCommandBuffer::addCommand(VPUCommand *cmd, uint64_t &cmdOffset, uint64_t &descOffset) {
    if (cmd == nullptr) {
        LOG_E("Command is nullptr or command is not initialized");
        return false;
    }

    LOG(VPU_CMD,
        "Attempting append a command %#x (size: %zu) to command buffer",
        cmd->getCommandType(),
        cmd->getCommitSize());

    if (cmdOffset >= descOffset) {
        LOG_E("Command override the descriptor");
        return false;
    }

    for (const auto &bo : cmd->getAssociateBufferObjects()) {
        addUniqueBoHandle(bo->getHandle());
    }

    void *descPtr = buffer->getBasePointer() + descOffset;
    if (!cmd->copyDescriptor(&descPtr, buffer)) {
        LOG_E("Failed to update offset in command");
        return false;
    }

    if (!buffer->copyToBuffer(cmd->getCommitStream(), cmd->getCommitSize(), cmdOffset)) {
        LOG_E("Failed to copy command structure to command buffer");
        return false;
    }

    LOG(VPU_CMD, "Command appended to command buffer: cmdOffset: %zu", cmdOffset);

    cmdOffset += cmd->getCommitSize();
    descOffset += getFwDataCacheAlign(cmd->getDescriptorSize());

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
    return true;
}

void VPUCommandBuffer::printDesc(vpu_cmd_resource_descriptor_table_t *table,
                                 size_t size,
                                 const char *type) const {
    if (table == nullptr) {
        LOG(VPU_CMD, "Descriptor base pointer is not provided, skip printing descriptors");
        return;
    }

    LOG(VPU_CMD,
        "Blob %s command descriptor table (table pointer: %p, size: %lu):",
        type,
        table,
        size);

    for (int i = 0; size > sizeof(vpu_cmd_resource_descriptor); i++) {
        LOG(VPU_CMD,
            "Table: %i:\n\ttype = %#x\n\tdesc_count = %i",
            i,
            table->type,
            table->desc_count);
        size_t count = table->desc_count;
        table++;

        vpu_cmd_resource_descriptor *desc = reinterpret_cast<decltype(desc)>(table);
        for (size_t j = 0; j < count; j++) {
            LOG(VPU_CMD,
                "Entry %lu:\n\taddress = %#lx\n\twidth = %#x",
                j,
                desc->address,
                desc->width);
            desc++;
        }

        table = reinterpret_cast<vpu_cmd_resource_descriptor_table_t *>(desc);
        size -= count * sizeof(vpu_cmd_resource_descriptor) +
                sizeof(vpu_cmd_resource_descriptor_table_t);
    }
}

void VPUCommandBuffer::printCommandBuffer() const {
    if (getLogLevel() < LogLevel::INFO)
        return;

    uint8_t *bufferPtr = buffer->getBasePointer();
    vpu_cmd_buffer_header_t *cmdHeader = reinterpret_cast<vpu_cmd_buffer_header_t *>(bufferPtr);
    LOG(VPU_CMD,
        "Start compute command buffer printing:\n"
        "\tCommand buffer ptr cpu = %p, vpu = %#lx\n"
        "\tSize = %u bytes, commands offset %u\n"
        "\tKernel heap addr = %#lx\n"
        "\tDescriptor heap addr = %#lx\n"
        "\tFence heap addr = %#lx",
        cmdHeader,
        buffer->getVPUAddr(),
        cmdHeader->cmd_buffer_size,
        cmdHeader->cmd_offset,
        cmdHeader->kernel_heap_base_address,
        cmdHeader->descriptor_heap_base_address,
        cmdHeader->fence_heap_base_address);

    size_t cmdOffset = cmdHeader->cmd_offset;
    int i = 0;
    while (cmdOffset < cmdHeader->cmd_buffer_size) {
        vpu_cmd_header_t *cmd = reinterpret_cast<vpu_cmd_header_t *>(bufferPtr + cmdOffset);

        if (cmd->size <= 0 || cmd->size >= cmdHeader->cmd_buffer_size) {
            LOG_E("Invalid command size, stop command buffer printing");
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
                bufferPtr + reinterpret_cast<vpu_cmd_copy_buffer_t *>(cmd)->desc_start_offset +
                    cmdHeader->descriptor_heap_base_address - buffer->getVPUAddr(),
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

        default:
            LOG_E("Unknown command, stop command buffer printing");
            return;
        }
        cmdOffset += cmd->size;
        i++;
    }
    LOG(VPU_CMD, "Stop compute command buffer printing");
}

bool VPUCommandBuffer::replaceBufferHandles(std::vector<uint32_t> &oldHandles,
                                            std::vector<uint32_t> &newHandles) {
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
    for (auto it = commandsBegin; it != commandsEnd; it++) {
        auto &cmd = *it;
        if (cmd->needsUpdate()) {
            if (!cmd->update(this)) {
                return false;
            }
        }
    }
    return true;
}

} // namespace VPU
