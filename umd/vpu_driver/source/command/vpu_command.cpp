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
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <algorithm>
#include <memory>
#include <sys/types.h>
#include <vector>

namespace VPU {

VPUCommand::VPUCommand(ScheduleType schType)
    : sType(schType) {}

bool VPUCommand::copyDescriptor(void **desc, std::shared_ptr<VPUBufferObject> bo) {
    if (!descriptor.has_value())
        return true;

    std::copy(descriptor->data.begin(),
              descriptor->data.end(),
              *reinterpret_cast<uint8_t **>(desc));

    *descriptor->commandOffset = bo->getVPUAddr(*desc);
    *reinterpret_cast<uint8_t **>(desc) += getFwDataCacheAlign(descriptor->data.size());

    return true;
}

bool VPUCommand::appendAssociateBufferObject(VPUDeviceContext *ctx, const void *assocPtr) {
    auto bo = ctx->findBufferObject(assocPtr);
    if (bo == nullptr) {
        LOG_E("Failed to find a pointer %p associated with VPUCommand", assocPtr);
        return false;
    }

    appendAssociateBufferObject(std::move(bo));
    return true;
}

void VPUCommand::appendAssociateBufferObject(
    const std::vector<std::shared_ptr<VPUBufferObject>> &bufferObjectsInput) {
    for (auto &bo : bufferObjectsInput) {
        appendAssociateBufferObject(bo);
    }
}

void VPUCommand::appendAssociateBufferObject(VPUDeviceContext *ctx, VPUBufferObject *bo) {
    auto boAsSharedPtr = ctx->findBufferObject(bo->getBasePointer());
    appendAssociateBufferObject(std::move(boAsSharedPtr));
}

void VPUCommand::appendAssociateBufferObject(const std::shared_ptr<VPUBufferObject> bo) {
    auto it = std::find_if(bufferObjects.begin(), bufferObjects.end(), [&](auto &b) -> bool {
        return b.get() == bo.get();
    });
    if (it == bufferObjects.end()) {
        bufferObjects.emplace_back(bo);
    }
}

void VPUCommand::eraseAssociatedBufferObjects(size_t pos) {
    bufferObjects.erase(bufferObjects.begin() + static_cast<ssize_t>(pos), bufferObjects.end());
}

} // namespace VPU
