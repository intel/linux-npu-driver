/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/include/umd_common.hpp"
#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <map>

namespace VPU {

VPUCommand::VPUCommand(EngineSupport engine)
    : engineSupport(engine) {}

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

    *descriptor->commandOffset =
        safe_cast<uint32_t>(ctx->getBufferVPUAddress(*desc) - ctx->getVPULowBaseAddress());
    *reinterpret_cast<uint8_t **>(desc) += getFwDataCacheAlign(descriptor->data.size());

    return true;
}

void VPUCommand::updateResourceDescriptorTable(void **desc,
                                               vpu_desc_table_entry_type type,
                                               const uint64_t address,
                                               const size_t size) {
    auto *descTable = static_cast<vpu_cmd_resource_descriptor_table_t *>(*desc);
    descTable->type = type;
    descTable->desc_count = 1;

    auto *descEntry = reinterpret_cast<vpu_cmd_resource_descriptor_t *>(descTable + 1);
    descEntry->address = address;
    descEntry->width = safe_cast<uint32_t>(size);
    descEntry++;
    *desc = descEntry;
}

void VPUCommand::updateResourceDescriptorTable(void **desc,
                                               vpu_desc_table_entry_type type,
                                               const VPUBufferObject *bo) {
    auto *descTable = static_cast<vpu_cmd_resource_descriptor_table_t *>(*desc);
    descTable->type = type;
    descTable->desc_count = 1;

    auto *descEntry = reinterpret_cast<vpu_cmd_resource_descriptor_t *>(descTable + 1);
    descEntry->address = bo->getVPUAddr();
    descEntry->width = safe_cast<uint32_t>(bo->getAllocSize());
    descEntry++;
    *desc = descEntry;
}

void VPUCommand::updateResourceDescriptorTable(void **desc,
                                               vpu_desc_table_entry_type type,
                                               const std::vector<uint64_t> &addrs,
                                               const std::vector<uint32_t> &size) {
    auto *descTable = static_cast<vpu_cmd_resource_descriptor_table_t *>(*desc);
    descTable->type = type;
    descTable->desc_count = safe_cast<uint16_t>(addrs.size());

    auto *descEntry = reinterpret_cast<vpu_cmd_resource_descriptor_t *>(descTable + 1);
    for (size_t i = 0; i < addrs.size(); i++) {
        descEntry->address = addrs[i];
        descEntry->width = size[i];
        descEntry++;
    }
    *desc = descEntry;
}

void VPUCommand::updateResourceDescriptorTable(void **desc,
                                               vpu_desc_table_entry_type type,
                                               const std::vector<uint64_t> &addrs,
                                               size_t size) {
    auto *descTable = static_cast<vpu_cmd_resource_descriptor_table_t *>(*desc);
    descTable->type = type;
    descTable->desc_count = safe_cast<uint16_t>(addrs.size());

    auto *descEntry = reinterpret_cast<vpu_cmd_resource_descriptor_t *>(descTable + 1);
    for (const auto &addr : addrs) {
        descEntry->address = addr;
        descEntry->width = safe_cast<uint32_t>(size);
        descEntry++;
    }

    *desc = descEntry;
}

bool VPUCommand::appendAssociateBufferObject(VPUDeviceContext *ctx, const void *assocPtr) {
    VPUBufferObject *bo = ctx->findBuffer(assocPtr);
    if (bo == nullptr) {
        LOG_E("Failed to find pointer %p", assocPtr);
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

} // namespace VPU
