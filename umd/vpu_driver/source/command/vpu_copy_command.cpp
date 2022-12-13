/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"
#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <boost/numeric/conversion/cast.hpp>
#include <cstdint>

namespace VPU {

std::shared_ptr<VPUCopyCommand>
VPUCopyCommand::create(VPUDeviceContext *ctx, const void *srcPtr, void *dstPtr, size_t size) {
    if (ctx == nullptr) {
        LOG_E("Invalid device context instance has returned. Copy command constructor failed! ");
        return nullptr;
    }

    auto copyDirection = ctx->getCopyDirection(dstPtr, srcPtr);
    if (copyDirection == COPY_INVALID) {
        LOG_E("Wrong memory type assigned during srcptr/dstptr allocation.");
        return nullptr;
    }

    VPUDescriptor descriptor;

    switch (ctx->getPciDevId()) {
    case MTL_VPU_PCI_DEVICE_ID:
        if (!fillDescriptor<vpu_cmd_copy_descriptor_mtl_t>(ctx, srcPtr, dstPtr, size, descriptor))
            return nullptr;
        break;
    default:
        LOG_E("Unknown VPU Device ID (%d)", ctx->getPciDevId());
        return nullptr;
    }

    return std::make_shared<VPUCopyCommand>(ctx, srcPtr, dstPtr, size, copyDirection, descriptor);
}

VPUCopyCommand::VPUCopyCommand(VPUDeviceContext *ctx,
                               const void *srcPtr,
                               void *dstPtr,
                               size_t size,
                               CopyDirection direction,
                               VPUDescriptor &descriptor)
    : VPUCommand(direction == COPY_LOCAL_TO_LOCAL ? EngineSupport::Compute : EngineSupport::Copy) {
    commitCmd.cmd.header.type = direction;
    commitCmd.cmd.header.size = sizeof(commitCmd.cmd);
    commitCmd.cmd.desc_start_offset = 0u;
    commitCmd.cmd.desc_count = 1u;

    LOG_I("Descriptor offset set to %u for Copy Command!", commitCmd.cmd.desc_start_offset);
    descriptor.commandOffset = &commitCmd.cmd.desc_start_offset;
    setDescriptor(std::move(descriptor));
    appendAssociateBufferObject(ctx, srcPtr);
    appendAssociateBufferObject(ctx, dstPtr);

    LOG_I("Copy Command successfully created!");
}

size_t VPUCopyCommand::getCommitSize() const {
    return commitCmd.getKMDCommitSize();
}

const uint8_t *VPUCopyCommand::getCommitStream() const {
    return commitCmd.getKMDCommitStream();
}

vpu_cmd_type VPUCopyCommand::getCommandType() const {
    return commitCmd.getKMDCommandType();
}

template <class T>
bool VPUCopyCommand::fillDescriptor(VPUDeviceContext *ctx,
                                    const void *srcPtr,
                                    void *dstPtr,
                                    size_t size,
                                    VPUDescriptor &descriptor) {
    descriptor.data.resize(sizeof(T), 0);

    T *copyDescPtr = reinterpret_cast<T *>(descriptor.data.data());
    copyDescPtr->src_address = ctx->getBufferVPUAddress(srcPtr);
    if (copyDescPtr->src_address == 0) {
        LOG_E("Failed to get vpu address for copy descriptor");
        return false;
    }

    copyDescPtr->dst_address = ctx->getBufferVPUAddress(dstPtr);
    if (copyDescPtr->dst_address == 0) {
        LOG_E("Failed to get vpu address for copy descriptor");
        return false;
    }

    copyDescPtr->size = boost::numeric_cast<uint32_t>(size);

    LOG_I("Updated copy descriptor: src_address = %#lx,  dst_address  = %#lx, size = %#x",
          copyDescPtr->src_address,
          copyDescPtr->dst_address,
          copyDescPtr->size);

    return true;
}

template <class T>
static void printCopyDesc(void *descBasePtr, vpu_cmd_header_t *cmd) {
    T *desc = reinterpret_cast<T *>(descBasePtr);
    for (uint32_t i = 0; i < reinterpret_cast<vpu_cmd_copy_buffer_t *>(cmd)->desc_count; i++) {
        LOG_I("Desc %u:\n\tsrc_address = %#lx\n"
              "\tdst_address = %#lx\n\tsize = %u bytes",
              i,
              desc->src_address,
              desc->dst_address,
              desc->size);
        desc++;
    }
}

void VPUCopyCommand::printCopyDesc(void *descBasePtr, vpu_cmd_header_t *cmd, uint16_t pciDevId) {
    if (descBasePtr == nullptr) {
        LOG_I("Descriptor base pointer is not provided, skip printing descriptors");
        return;
    }

    switch (pciDevId) {
    case MTL_VPU_PCI_DEVICE_ID:
        VPU::printCopyDesc<vpu_cmd_copy_descriptor_mtl_t>(descBasePtr, cmd);
        break;
    default:
        LOG_E("Invalid VPU Device ID (%d)", pciDevId);
    }
}

} // namespace VPU
