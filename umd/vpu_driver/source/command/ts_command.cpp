/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/ts_command.hpp"

#include <cstdint>

#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <utility>

namespace VPU {

std::shared_ptr<VPUTimeStampCommand>
VPUTimeStampCommand::create(uint64_t *dstPtr,
                            std::shared_ptr<VPUBufferObject> dstBo,
                            uint32_t type) {
    if (!dstBo) {
        LOG_E("nullptr in Timestamp command arguments");
        return nullptr;
    }

    if (!dstBo->isInRange(reinterpret_cast<void *>(dstPtr))) {
        LOG_E("Pointer %p is not allocated within buffer", dstPtr);

        return nullptr;
    }

    return std::make_shared<VPUTimeStampCommand>(type, dstBo->getVPUAddr(dstPtr), std::move(dstBo));
}

VPUTimeStampCommand::VPUTimeStampCommand(uint32_t type,
                                         uint64_t dstVPUAddr,
                                         std::shared_ptr<VPUBufferObject> dstBo)
    : VPUCommand() {
    vpu_cmd_timestamp_t cmd = {};

    cmd.header.type = VPU_CMD_TIMESTAMP;
    cmd.header.size = sizeof(vpu_cmd_timestamp_t);
    cmd.timestamp_address = dstVPUAddr;
    cmd.type = type;
    command.emplace<vpu_cmd_timestamp_t>(cmd);
    appendAssociateBufferObject(std::move(dstBo));
    LOG(VPU_CMD, "Timestamp Command successfully created!");
}

} // namespace VPU
