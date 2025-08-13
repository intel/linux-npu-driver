/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/fill_command.hpp"

#include <cstdint>

#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <utility>

namespace VPU {

std::shared_ptr<VPUFillCommand> VPUFillCommand::create(void *dstPtr,
                                                       std::shared_ptr<VPUBufferObject> dstBo,
                                                       uint64_t size,
                                                       uint32_t fill_pattern) {
    if (!dstBo->isInRange(reinterpret_cast<uint8_t *>(dstPtr))) {
        LOG_E("Pointer %p is not allocated within buffer", dstPtr);
        return nullptr;
    }

    if (!dstBo->isInRange(reinterpret_cast<uint8_t *>(dstPtr) + size - 1)) {
        LOG_E("Requested fill size %lu larger than allocated memory", size);
        return nullptr;
    }

    return std::make_shared<VPUFillCommand>(dstPtr, std::move(dstBo), size, fill_pattern);
}

VPUFillCommand::VPUFillCommand(void *dstPtr,
                               std::shared_ptr<VPUBufferObject> dstBo,
                               uint64_t size,
                               uint32_t fill_pattern)

    : VPUCommand() {
    vpu_cmd_memory_fill_t cmd = {};

    cmd.header.type = VPU_CMD_MEMORY_FILL;
    cmd.header.size = sizeof(vpu_cmd_memory_fill_t);
    cmd.start_address = dstBo->getVPUAddr(dstPtr);
    cmd.size = size;
    cmd.fill_pattern = fill_pattern;
    command.emplace<vpu_cmd_memory_fill_t>(cmd);
    appendAssociateBufferObject(std::move(dstBo));
    LOG(VPU_CMD, "Fill Command successfully created!");
}
} // namespace VPU
