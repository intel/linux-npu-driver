/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/command/vpu_memory_fill_command.hpp"

#include <cstdint>

namespace VPU {

std::shared_ptr<VPUMemoryFillCommand> VPUMemoryFillCommand::create(VPUDeviceContext *ctx,
                                                                   void *ptr,
                                                                   const void *pattern,
                                                                   size_t patternSize,
                                                                   size_t size) {
    if (ctx == nullptr) {
        LOG_E("Device context passed as nullptr.");
        return nullptr;
    }

    if (ctx->findBuffer(ptr) == nullptr) {
        LOG_E("Pointer %p is not allocated within context %p", ptr, ctx);
        return nullptr;
    }

    if (!(patternSize == 1u || patternSize == 2u || patternSize == 4u)) {
        LOG_E("Invalid pattern size %ld. Value should be power of 2. Max = 4.", patternSize);
        return nullptr;
    }

    return std::make_shared<VPUMemoryFillCommand>(ctx, ptr, pattern, patternSize, size);
}

VPUMemoryFillCommand::VPUMemoryFillCommand(VPUDeviceContext *ctx,
                                           void *ptr,
                                           const void *pattern,
                                           size_t patternSize,
                                           size_t size)
    : VPUCommand(EngineSupport::Copy) {
    vpu_cmd_memory_fill_t cmd = {};

    cmd.header.type = VPU_CMD_MEMORY_FILL;
    cmd.header.size = sizeof(vpu_cmd_memory_fill_t);
    cmd.start_address = ctx->getBufferVPUAddress(ptr);
    cmd.size = boost::numeric_cast<uint64_t>(size);

    if (patternSize == 1u) {
        uint8_t uintPattern = *static_cast<const uint8_t *>(pattern);
        cmd.fill_pattern = uintPattern;
        for (int i = 0; i < 3; i++) {
            cmd.fill_pattern = (cmd.fill_pattern << 8) | uintPattern;
        }
    } else if (patternSize == 2u) {
        uint16_t uintPattern = *static_cast<const uint16_t *>(pattern);
        cmd.fill_pattern = uintPattern;
        cmd.fill_pattern = (cmd.fill_pattern << 16) | uintPattern;
    } else if (patternSize == 4u) {
        uint32_t uintPattern = *static_cast<const uint32_t *>(pattern);
        cmd.fill_pattern = uintPattern;
    }
    command.emplace<vpu_cmd_memory_fill_t>(cmd);
    appendAssociateBufferObject(ctx, ptr);

    LOG_I("Memory Fill command successfully created.");
}

} // namespace VPU
