/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/command/vpu_ts_command.hpp"

#include <cstdint>

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
    : VPUCommand(EngineSupport::Backward) {
    commitCmd.cmd.header.type = VPU_CMD_TIMESTAMP;
    commitCmd.cmd.header.size = sizeof(commitCmd.cmd);
    commitCmd.cmd.timestamp_address = 0u;
    commitCmd.cmd.timestamp_address = ctx->getBufferVPUAddress(dstPtr);

    LOG_I("Timestamp Command successfully created!");
    appendAssociateBufferObject(ctx, dstPtr);
}

size_t VPUTimeStampCommand::getCommitSize() const {
    return commitCmd.getKMDCommitSize();
}

const uint8_t *VPUTimeStampCommand::getCommitStream() const {
    return commitCmd.getKMDCommitStream();
}

vpu_cmd_type VPUTimeStampCommand::getCommandType() const {
    return commitCmd.getKMDCommandType();
}

} // namespace VPU
