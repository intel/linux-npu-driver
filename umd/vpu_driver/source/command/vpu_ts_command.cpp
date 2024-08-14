/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_ts_command.hpp"

#include <cstdint>

#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace VPU {

std::shared_ptr<VPUTimeStampCommand> VPUTimeStampCommand::create(VPUDeviceContext *ctx,
                                                                 uint64_t *dstPtr) {
    if (!ctx) {
        LOG_E("Context is nullptr in Timestamp command");
        return nullptr;
    }

    if (ctx->findBuffer(dstPtr) == nullptr) {
        LOG_E("Pointer %p is not allocated within context %p", dstPtr, ctx);
        return nullptr;
    }

    return std::make_shared<VPUTimeStampCommand>(ctx, dstPtr);
}

VPUTimeStampCommand::VPUTimeStampCommand(VPUDeviceContext *ctx, uint64_t *dstPtr)
    : VPUCommand(EngineSupport::Forward) {
    vpu_cmd_timestamp_t cmd = {};

    cmd.header.type = VPU_CMD_TIMESTAMP;
    cmd.header.size = sizeof(vpu_cmd_timestamp_t);
    cmd.timestamp_address = 0u;
    cmd.timestamp_address = ctx->getBufferVPUAddress(dstPtr);
    cmd.type = ctx->getFwTimestampType();
    command.emplace<vpu_cmd_timestamp_t>(cmd);
    appendAssociateBufferObject(ctx, dstPtr);
    LOG(VPU_CMD, "Timestamp Command successfully created!");
}

} // namespace VPU
