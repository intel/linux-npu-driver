/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_fill_command.hpp"

#include <cstdint>

#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace VPU {

std::shared_ptr<VPUFillCommand>
VPUFillCommand::create(VPUDeviceContext *ctx, void *dstPtr, uint64_t size, uint32_t fill_pattern) {
    if (!ctx) {
        LOG_E("Context is nullptr in Fill command");
        return nullptr;
    }

    auto bo = ctx->findBuffer(dstPtr);
    if (bo == nullptr) {
        LOG_E("Pointer %p is not allocated within context %p", dstPtr, ctx);
        return nullptr;
    }
    if (!bo->isInRange(reinterpret_cast<uint8_t *>(dstPtr) + size - 1)) {
        LOG_E("Requested fill size %lu larger than allocated memory", size);
        return nullptr;
    }
    return std::make_shared<VPUFillCommand>(ctx, dstPtr, size, fill_pattern);
}

VPUFillCommand::VPUFillCommand(VPUDeviceContext *ctx,
                               void *dstPtr,
                               uint64_t size,
                               uint32_t fill_pattern)
    : VPUCommand() {
    vpu_cmd_memory_fill_t cmd = {};

    cmd.header.type = VPU_CMD_MEMORY_FILL;
    cmd.header.size = sizeof(vpu_cmd_memory_fill_t);
    cmd.start_address = ctx->getBufferVPUAddress(dstPtr);
    cmd.size = size;
    cmd.fill_pattern = fill_pattern;
    command.emplace<vpu_cmd_memory_fill_t>(cmd);
    appendAssociateBufferObject(ctx, dstPtr);
    LOG(VPU_CMD, "Fill Command successfully created!");
}
} // namespace VPU
