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

#include <cstdint>

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
    : VPUCommand(EngineSupport::Backward) {
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

    LOG_I("Copy Command successfully created!");
}

bool VPUCopyCommand::changeCopyCommandType(uint32_t engine_id) {
    vpu_cmd_header_t *hdr = nullptr;

    hdr = reinterpret_cast<vpu_cmd_header_t *>(std::any_cast<vpu_cmd_copy_buffer_t>(&command));
    if (hdr == nullptr)
        return false;

    if (engine_id == DRM_IVPU_ENGINE_COMPUTE) {
        hdr->type = VPU_CMD_COPY_LOCAL_TO_LOCAL;
    } else if (engine_id == DRM_IVPU_ENGINE_COPY) {
        hdr->type = VPU_CMD_COPY_SYSTEM_TO_SYSTEM;
    } else {
        LOG_I("Unsupported engine conversion for copy command.");
        return false;
    }
    return true;
}

} // namespace VPU
