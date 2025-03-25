/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_copy_command.hpp"

#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <utility>

namespace VPU {

std::shared_ptr<VPUCopyCommand> VPUCopyCommand::create(VPUDeviceContext *ctx,
                                                       const void *srcPtr,
                                                       const std::shared_ptr<VPUBufferObject> srcBo,
                                                       void *dstPtr,
                                                       std::shared_ptr<VPUBufferObject> dstBo,
                                                       size_t size) {
    if (!ctx || !srcPtr || !dstPtr || !srcBo || !dstBo) {
        LOG_E("nullptr in arguments. Copy command creation failed! ");
        return nullptr;
    }
    if (!dstBo->isInRange(dstPtr)) {
        LOG_E("Destination pointer %p outside allocated memory", dstPtr);
        return nullptr;
    }

    if (!srcBo->isInRange(srcPtr)) {
        LOG_E("Source pointer %p outside allocated memory", srcPtr);
        return nullptr;
    }

    VPUDescriptor descriptor;
    if (!ctx->getCopyCommandDescriptor(srcBo->getVPUAddr(srcPtr),
                                       dstBo->getVPUAddr(dstPtr),
                                       size,
                                       descriptor))
        return nullptr;

    return std::make_shared<VPUCopyCommand>(std::move(srcBo), std::move(dstBo), size, descriptor);
}

VPUCopyCommand::VPUCopyCommand(const std::shared_ptr<VPUBufferObject> srcBo,
                               std::shared_ptr<VPUBufferObject> dstBo,
                               size_t size,
                               VPUDescriptor &descriptor)
    : VPUCommand() {
    vpu_cmd_copy_buffer_t cmd = {};

    cmd.header.type = VPU_CMD_COPY_LOCAL_TO_LOCAL;
    cmd.header.size = sizeof(vpu_cmd_copy_buffer_t);
    cmd.desc_start_offset = 0u;
    cmd.desc_count = descriptor.numDescriptors;
    command.emplace<vpu_cmd_copy_buffer_t>(cmd);

    descriptor.commandOffset = &(std::any_cast<vpu_cmd_copy_buffer_t>(&command)->desc_start_offset);

    setDescriptor(std::move(descriptor));
    appendAssociateBufferObject(std::move(srcBo));
    appendAssociateBufferObject(std::move(dstBo));

    LOG(VPU_CMD, "Copy Command successfully created!");
}

} // namespace VPU
