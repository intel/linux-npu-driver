/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_command.hpp"

#include <cstddef>
#include <cstdint>

#include "vpu_driver/include/umd_common.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <algorithm>
#include <sys/types.h>
#include <vector>

namespace VPU {

VPUCommand::VPUCommand(ScheduleType schType)
    : sType(schType) {}

bool VPUCommand::copyDescriptor(VPUDeviceContext *ctx, void **desc) {
    if (ctx == nullptr) {
        LOG_E("Invalid context");
        return false;
    }

    if (!descriptor.has_value())
        return true;

    std::copy(descriptor->data.begin(),
              descriptor->data.end(),
              *reinterpret_cast<uint8_t **>(desc));

    *descriptor->commandOffset = safe_cast<uint64_t>(ctx->getBufferVPUAddress(*desc));
    *reinterpret_cast<uint8_t **>(desc) += getFwDataCacheAlign(descriptor->data.size());

    return true;
}

bool VPUCommand::appendAssociateBufferObject(VPUDeviceContext *ctx, const void *assocPtr) {
    VPUBufferObject *bo = ctx->findBuffer(assocPtr);
    if (bo == nullptr) {
        LOG_E("Failed to find a pointer %p associated with VPUCommand", assocPtr);
        return false;
    }

    appendAssociateBufferObject(bo);
    return true;
}

void VPUCommand::appendAssociateBufferObject(
    const std::vector<VPUBufferObject *> &bufferObjectsInput) {
    for (auto &bo : bufferObjectsInput) {
        appendAssociateBufferObject(bo);
    }
}

void VPUCommand::appendAssociateBufferObject(VPUBufferObject *bo) {
    auto it = std::find(bufferObjects.begin(), bufferObjects.end(), bo);
    if (it == bufferObjects.end()) {
        bufferObjects.emplace_back(bo);
    }
}

void VPUCommand::eraseAssociatedBufferObjects(size_t pos) {
    bufferObjects.erase(bufferObjects.begin() + static_cast<ssize_t>(pos), bufferObjects.end());
}

} // namespace VPU
