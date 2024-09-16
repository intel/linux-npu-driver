/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_copy_command.hpp"

#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <utility>

namespace VPU {

std::shared_ptr<VPUCopyCommand>
VPUCopyCommand::create(VPUDeviceContext *ctx, const void *srcPtr, void *dstPtr, size_t size) {
    if (ctx == nullptr) {
        LOG_E("Invalid device context instance has returned. Copy command constructor failed! ");
        return nullptr;
    }

    VPUDescriptor descriptor;
    if (!ctx->getCopyCommandDescriptor(srcPtr, dstPtr, size, descriptor))
        return nullptr;

    return std::make_shared<VPUCopyCommand>(ctx, srcPtr, dstPtr, size, descriptor);
}

VPUCopyCommand::VPUCopyCommand(VPUDeviceContext *ctx,
                               const void *srcPtr,
                               void *dstPtr,
                               size_t size,
                               VPUDescriptor &descriptor)
    : VPUCommand() {
    vpu_cmd_copy_buffer_t cmd = {};

    cmd.header.type = VPU_CMD_COPY_LOCAL_TO_LOCAL;
    cmd.header.size = sizeof(vpu_cmd_copy_buffer_t);
    cmd.desc_start_offset = 0u;
    cmd.desc_count = 1u;
    command.emplace<vpu_cmd_copy_buffer_t>(cmd);

    descriptor.commandOffset = &(std::any_cast<vpu_cmd_copy_buffer_t>(&command)->desc_start_offset);

    setDescriptor(std::move(descriptor));
    appendAssociateBufferObject(ctx, srcPtr);
    appendAssociateBufferObject(ctx, dstPtr);

    LOG(VPU_CMD, "Copy Command successfully created!");
}

} // namespace VPU
