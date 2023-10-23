/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"
#include "vpu_driver/source/command/vpu_graph_init_command.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <cstdint>
#include <memory.h>

namespace VPU {

/*
 * Blob execute allows to run inferences in parallel based on number of metadata and scratch
 * buffers. Metadata count has to be equal to scatch count. Driver counts how many parallel
 * inferences can be run based on requested size.
 */
static size_t getBufferCount(size_t size) {
    constexpr size_t MB = 1024 * 1024;
    if (size < 64 * MB)
        return 4;
    if (size < 128 * MB)
        return 2;
    return 1;
}

std::shared_ptr<VPUGraphInitCommand> VPUGraphInitCommand::create(VPUDeviceContext *ctx,
                                                                 uint64_t umdBlobId,
                                                                 void *blobData,
                                                                 const size_t blobSize,
                                                                 size_t scratchSize,
                                                                 size_t metadataSize,
                                                                 const void *kernelData,
                                                                 const size_t kernelDataSize) {
    LOG_I("Graph init command args: \n"
          "\tumdBlobId: %#lx, blobData: %p, blobSize: %zu,\n"
          "\tscratchSize: %lu, metadataSize: %lu,\n"
          "\tkernelData: %p, kernelDataSize: %lu,\n",
          umdBlobId,
          blobData,
          blobSize,
          scratchSize,
          metadataSize,
          kernelData,
          kernelDataSize);

    if (ctx == nullptr) {
        LOG_E("Failed to get device context.");
        return nullptr;
    }

    if (blobData == nullptr) {
        LOG_E("Invalid input or output pointers.");
        return nullptr;
    }

    auto kernelBuffer = ctx->createInternalBufferObject(ctx->getPageAlignedSize(blobSize),
                                                        VPUBufferObject::Type::WriteCombineLow);
    if (kernelBuffer == nullptr) {
        LOG_E("Failed to allocate kernel heap for graph data.");
        return nullptr;
    }

    if (!kernelBuffer->copyToBuffer(blobData, blobSize, 0)) {
        LOG_E("Failed to copy kernel buffer");
        return nullptr;
    }

    const size_t bufferCount = getBufferCount(metadataSize);
    auto scratchBuffer =
        ctx->createInternalBufferObject(ctx->getPageAlignedSize(scratchSize) * bufferCount,
                                        VPUBufferObject::Type::WriteCombineHigh);
    if (scratchBuffer == nullptr) {
        LOG_E("Failed to allocate memory for scratch pointer!");
        return nullptr;
    }

    auto metadataBuffer =
        ctx->createInternalBufferObject(ctx->getPageAlignedSize(metadataSize) * bufferCount,
                                        VPUBufferObject::Type::WriteCombineLow);
    if (metadataBuffer == nullptr) {
        LOG_E("Failed to allocate memory for metadata pointer!");
        return nullptr;
    }

    VPUBufferObject *actKernelBuffer = nullptr;
    if (kernelData != nullptr && kernelDataSize != 0) {
        actKernelBuffer = ctx->createInternalBufferObject(ctx->getPageAlignedSize(kernelDataSize),
                                                          VPUBufferObject::Type::WriteCombineHigh);
        if (actKernelBuffer == nullptr) {
            LOG_E("Failed to allocate kernel data pointer!");
            return nullptr;
        }

        if (!actKernelBuffer->copyToBuffer(kernelData, kernelDataSize, 0)) {
            LOG_E("Failed to copy activation kernel buffer");
            return nullptr;
        }
    }

    return std::make_shared<VPUGraphInitCommand>(ctx,
                                                 umdBlobId,
                                                 blobSize,
                                                 kernelBuffer,
                                                 scratchBuffer,
                                                 metadataBuffer,
                                                 actKernelBuffer,
                                                 scratchSize,
                                                 metadataSize);
}

VPUGraphInitCommand::VPUGraphInitCommand(VPUDeviceContext *ctx,
                                         uint64_t umdBlobId,
                                         const size_t blobSize,
                                         VPUBufferObject *kernelBuffer,
                                         VPUBufferObject *scratchBuffer,
                                         VPUBufferObject *metadataBuffer,
                                         VPUBufferObject *actKernelBuffer,
                                         size_t scratchSize,
                                         size_t metadataSize)
    : VPUCommand(EngineSupport::Compute)
    , ctx(ctx)
    , kernelBuffer(kernelBuffer)
    , scratchBuffer(scratchBuffer)
    , metadataBuffer(metadataBuffer)
    , actKernelBuffer(actKernelBuffer)
    , scratchSize(scratchSize)
    , metadataSize(metadataSize)
    , bufferCount(getBufferCount(metadataSize)) {
    vpu_cmd_ov_blob_initialize_t cmd = {};

    cmd.header.type = VPU_CMD_OV_BLOB_INITIALIZE;
    cmd.header.size = sizeof(vpu_cmd_ov_blob_initialize_t);
    cmd.kernel_size = safe_cast<uint32_t>(blobSize);
    cmd.desc_table_size =
        static_cast<uint32_t>(2 * sizeof(vpu_cmd_resource_descriptor_table_t) +
                              2 * sizeof(vpu_cmd_resource_descriptor_t) * bufferCount);
    cmd.blob_id = umdBlobId;

    if (actKernelBuffer != nullptr) {
        cmd.desc_table_size += static_cast<uint32_t>(sizeof(vpu_cmd_resource_descriptor_table_t) +
                                                     sizeof(vpu_cmd_resource_descriptor_t));
    }

    cmd.kernel_offset =
        safe_cast<uint32_t>(kernelBuffer->getVPUAddr() - ctx->getVPULowBaseAddress());
    LOG_I("Kernel offset set to %#lx for GraphInit Command!", cmd.kernel_offset);
    command.emplace<vpu_cmd_ov_blob_initialize_t>(cmd);
    appendAssociateBufferObject(ctx, kernelBuffer->getBasePointer());
    appendAssociateBufferObject(ctx, scratchBuffer->getBasePointer());
    appendAssociateBufferObject(ctx, metadataBuffer->getBasePointer());

    if (actKernelBuffer != nullptr)
        appendAssociateBufferObject(ctx, actKernelBuffer->getBasePointer());

    fillDescriptor();

    LOG_I("Graph Init Command successfully created!");
}

VPUGraphInitCommand::~VPUGraphInitCommand() {
    if (ctx == nullptr) {
        LOG_E("Failed to get device context.");
        return;
    }

    if (kernelBuffer && !ctx->freeMemAlloc(kernelBuffer)) {
        LOG_E("Failed to free kernel heap");
    }

    if (scratchBuffer && !ctx->freeMemAlloc(scratchBuffer)) {
        LOG_E("Failed to free scratch heap");
    }

    if (metadataBuffer && !ctx->freeMemAlloc(metadataBuffer)) {
        LOG_E("Failed to free metadata heap");
    }

    if (actKernelBuffer && !ctx->freeMemAlloc(actKernelBuffer)) {
        LOG_E("Failed to free kernel data heap");
    }
}

void VPUGraphInitCommand::fillDescriptor() {
    VPUDescriptor descriptor;
    auto cmd = std::any_cast<vpu_cmd_ov_blob_initialize_t>(&command);

    descriptor.commandOffset = &cmd->desc_table_offset;
    descriptor.data.resize(cmd->desc_table_size, 0);
    void *desc = descriptor.data.data();

    uint64_t startAddr = scratchBuffer->getVPUAddr();
    size_t step = ctx->getPageAlignedSize(scratchSize);
    std::vector<uint64_t> addresses = {};
    for (size_t i = 0; i < bufferCount; i++)
        addresses.push_back(startAddr + i * step);

    updateResourceDescriptorTable(&desc, VPU_DESC_TABLE_ENTRY_TYPE_SCRATCH, addresses, scratchSize);

    startAddr = metadataBuffer->getVPUAddr();
    step = ctx->getPageAlignedSize(metadataSize);
    addresses = {};
    for (size_t i = 0; i < bufferCount; i++)
        addresses.emplace_back(startAddr + i * step);

    updateResourceDescriptorTable(&desc,
                                  VPU_DESC_TABLE_ENTRY_TYPE_METADATA,
                                  addresses,
                                  metadataSize);

    if (actKernelBuffer != nullptr) {
        updateResourceDescriptorTable(&desc,
                                      VPU_DESC_TABLE_ENTRY_TYPE_KERNEL_DATA,
                                      actKernelBuffer);
    }
    setDescriptor(std::move(descriptor));
}

} // namespace VPU
