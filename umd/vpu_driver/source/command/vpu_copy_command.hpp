/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstdint>
#include <stddef.h>

#include "api/vpu_jsm_job_cmd_api.h"
#include "umd_common.hpp"
#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <any>
#include <memory>
#include <vector>

namespace VPU {

class VPUCopyCommand : public VPUCommand {
  public:
    VPUCopyCommand(VPUDeviceContext *ctx,
                   const void *srcPtr,
                   void *dstPtr,
                   size_t size,
                   VPUDescriptor &descriptor);

    static std::shared_ptr<VPUCopyCommand>
    create(VPUDeviceContext *ctx, const void *srcPtr, void *dstPtr, size_t size);

    const vpu_cmd_header_t *getHeader() const {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<vpu_cmd_copy_buffer_t>(&command));
    }

    template <class T>
    static bool fillDescriptor(VPUDeviceContext *ctx,
                               const void *srcPtr,
                               void *dstPtr,
                               size_t size,
                               VPUDescriptor &descriptor) {
        if (ctx == nullptr) {
            LOG_E("Passed context as nullptr");
            return false;
        }

        descriptor.data.resize(sizeof(T), 0);

        T *copyDescPtr = reinterpret_cast<T *>(descriptor.data.data());
        copyDescPtr->src_address = ctx->getBufferVPUAddress(srcPtr);
        if (copyDescPtr->src_address == 0) {
            LOG_E("Failed to get vpu address for copy descriptor");
            return false;
        }

        copyDescPtr->dst_address = ctx->getBufferVPUAddress(dstPtr);
        if (copyDescPtr->dst_address == 0) {
            LOG_E("Failed to get vpu address for copy descriptor");
            return false;
        }

        copyDescPtr->size = safe_cast<uint32_t>(size);

        LOG(MISC,
            "Updated copy descriptor: src_address = %#lx,  dst_address  = %#lx, size = %#x",
            copyDescPtr->src_address,
            copyDescPtr->dst_address,
            copyDescPtr->size);

        return true;
    }

    template <class T>
    static void printCopyDesc(void *descBasePtr, vpu_cmd_header_t *cmd) {
        if (descBasePtr == nullptr || cmd == nullptr) {
            LOG_W("Passed descriptor or cmd header as nullptr");
            return;
        }

        T *desc = reinterpret_cast<T *>(descBasePtr);
        for (uint32_t i = 0; i < reinterpret_cast<vpu_cmd_copy_buffer_t *>(cmd)->desc_count; i++) {
            LOG(VPU_CMD,
                "Desc %u:\n\tsrc_address = %#lx\n"
                "\tdst_address = %#lx\n\tsize = %u bytes",
                i,
                desc->src_address,
                desc->dst_address,
                desc->size);
            desc++;
        }
    }
};

} // namespace VPU
