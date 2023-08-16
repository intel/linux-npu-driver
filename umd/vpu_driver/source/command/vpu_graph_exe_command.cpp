/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"

#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/command/vpu_graph_exe_command.hpp"

#include <cstdint>

namespace VPU {

std::shared_ptr<VPUGraphExecuteCommand>
VPUGraphExecuteCommand::create(VPUDeviceContext *ctx,
                               uint64_t umdBlobId,
                               const std::vector<std::pair<const void *, uint32_t>> &inputBuffers,
                               const std::vector<std::pair<const void *, uint32_t>> &outputBuffers,
                               const std::vector<VPUBufferObject *> &graphInitBufferObjects,
                               size_t profilingSize,
                               void *profilingBuffer) {
    LOG_V("Graph exe command args:\n\tumdBlobId: %#lx, inputBuffer: %p, outputBuffer: %p",
          umdBlobId,
          &inputBuffers,
          &outputBuffers);

    if (ctx == nullptr) {
        LOG_E("Failed to get device context.");
        return nullptr;
    }

    std::vector<uint64_t> inputArray;
    std::vector<uint64_t> outputArray;

    inputArray.reserve(inputBuffers.size());
    outputArray.reserve(outputBuffers.size());

    if (!checkUserArgs(ctx, inputBuffers, inputArray))
        return nullptr;

    if (!checkUserArgs(ctx, outputBuffers, outputArray))
        return nullptr;

    if (profilingSize > 0) {
        if (profilingBuffer == nullptr) {
            LOG_E("Failed to get profiling buffer");
            return nullptr;
        }

        if (ctx->getBufferVPUAddress(profilingBuffer) == 0) {
            LOG_E("Failed to getHeapVPUAddress on profilingBuffer (%p)!", profilingBuffer);
            return nullptr;
        }
    }

    return std::make_shared<VPUGraphExecuteCommand>(ctx,
                                                    umdBlobId,
                                                    inputBuffers,
                                                    outputBuffers,
                                                    graphInitBufferObjects,
                                                    profilingSize,
                                                    profilingBuffer,
                                                    inputArray,
                                                    outputArray);
}

VPUGraphExecuteCommand::VPUGraphExecuteCommand(
    VPUDeviceContext *ctx,
    uint64_t umdBlobId,
    const std::vector<std::pair<const void *, uint32_t>> &inputBuffers,
    const std::vector<std::pair<const void *, uint32_t>> &outputBuffers,
    const std::vector<VPUBufferObject *> &graphInitBufferObjects,
    size_t profilingSize,
    void *profilingBuffer,
    std::vector<uint64_t> inputArray,
    std::vector<uint64_t> outputArray)
    : VPUCommand(EngineSupport::Compute)
    , ctx(ctx)
    , profilingSize(profilingSize)
    , profilingBuffer(profilingBuffer) {
    vpu_cmd_ov_blob_execute_t cmd = {};

    cmd.header.type = VPU_CMD_OV_BLOB_EXECUTE;
    cmd.header.size = sizeof(vpu_cmd_ov_blob_execute_t);
    cmd.desc_table_offset = 0;
    cmd.blob_id = umdBlobId;

    size_t totalArgs = inputBuffers.size() + outputBuffers.size();

    cmd.desc_table_size =
        boost::numeric_cast<uint32_t>(2 * sizeof(vpu_cmd_resource_descriptor_table_t) +
                                      totalArgs * sizeof(vpu_cmd_resource_descriptor_t));

    if (profilingSize > 0) {
        cmd.desc_table_size += boost::numeric_cast<uint32_t>(
            sizeof(vpu_cmd_resource_descriptor_table_t) + sizeof(vpu_cmd_resource_descriptor_t));
    }
    command.emplace<vpu_cmd_ov_blob_execute_t>(cmd);
    for (const auto &ptr : inputBuffers) {
        appendAssociateBufferObject(ctx, ptr.first);
    }

    for (const auto &ptr : outputBuffers) {
        appendAssociateBufferObject(ctx, ptr.first);
    }

    appendAssociateBufferObject(graphInitBufferObjects);

    if (profilingSize != 0 && profilingBuffer != nullptr) {
        appendAssociateBufferObject(ctx, profilingBuffer);
    }

    std::vector<uint32_t> inputBufferSize;
    std::vector<uint32_t> outputBufferSize;
    for (const auto &buffer : inputBuffers)
        inputBufferSize.push_back(buffer.second);
    for (const auto &buffer : outputBuffers)
        outputBufferSize.push_back(buffer.second);

    fillDescriptor(inputArray, outputArray, inputBufferSize, outputBufferSize);
}

void VPUGraphExecuteCommand::fillDescriptor(std::vector<uint64_t> inputArray,
                                            std::vector<uint64_t> outputArray,
                                            const std::vector<uint32_t> inputArraySize,
                                            const std::vector<uint32_t> outputArraySize) {
    VPUDescriptor descriptor;
    auto cmd = std::any_cast<vpu_cmd_ov_blob_execute_t>(&command);

    descriptor.commandOffset = &cmd->desc_table_offset;
    descriptor.data.resize(cmd->desc_table_size, 0);
    void *desc = descriptor.data.data();

    updateResourceDescriptorTable(&desc,
                                  VPU_DESC_TABLE_ENTRY_TYPE_INPUT,
                                  inputArray,
                                  inputArraySize);
    updateResourceDescriptorTable(&desc,
                                  VPU_DESC_TABLE_ENTRY_TYPE_OUTPUT,
                                  outputArray,
                                  outputArraySize);

    if (profilingBuffer) {
        auto bufferVpuAddr = ctx->getBufferVPUAddress(profilingBuffer);
        updateResourceDescriptorTable(&desc,
                                      VPU_DESC_TABLE_ENTRY_TYPE_PROFILING_OUTPUT,
                                      bufferVpuAddr,
                                      profilingSize);
    }

    setDescriptor(std::move(descriptor));
}

bool VPUGraphExecuteCommand::checkUserArgs(
    VPUDeviceContext *ctx,
    const std::vector<std::pair<const void *, uint32_t>> &userArgs,
    std::vector<uint64_t> &vpuAddr) {
    for (const auto &ptr : userArgs) {
        if (ptr.first == nullptr) {
            LOG_E("Invalid user pointer.");
            return false;
        }

        auto bo = ctx->findBuffer(ptr.first);
        if (bo == nullptr) {
            LOG_E("Couldn't find tracked buffer for user arg(%p).", ptr.first);
            return false;
        }

        uint64_t address = bo->getVPUAddr() +
                           reinterpret_cast<uint8_t *>(const_cast<void *>(ptr.first)) -
                           bo->getBasePointer();

        // Push VPU Address into input/output buffer.
        vpuAddr.emplace_back(address);
    }

    return true;
}

} // namespace VPU
