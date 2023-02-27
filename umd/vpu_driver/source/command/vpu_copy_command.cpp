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
    if (!ctx->getCopyCommandDescriptor(srcPtr, dstPtr, size, descriptor))
        return nullptr;

    return std::make_shared<VPUCopyCommand>(ctx, srcPtr, dstPtr, size, copyDirection, descriptor);
}

VPUCopyCommand::VPUCopyCommand(VPUDeviceContext *ctx,
                               const void *srcPtr,
                               void *dstPtr,
                               size_t size,
                               CopyDirection direction,
                               VPUDescriptor &descriptor)
    : VPUCommand(direction == COPY_LOCAL_TO_LOCAL ? EngineSupport::Compute : EngineSupport::Copy) {
    vpu_cmd_copy_buffer_t cmd = {};

    cmd.header.type = direction;
    cmd.header.size = sizeof(vpu_cmd_copy_buffer_t);
    cmd.desc_start_offset = 0u;
    cmd.desc_count = 1u;
    command.emplace<vpu_cmd_copy_buffer_t>(cmd);

    descriptor.commandOffset = &(std::any_cast<vpu_cmd_copy_buffer_t>(&command)->desc_start_offset);

    setDescriptor(std::move(descriptor));
    appendAssociateBufferObject(ctx, srcPtr);
    appendAssociateBufferObject(ctx, dstPtr);

    LOG_I("Copy Command successfully created!");
}

} // namespace VPU
