/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstdint>
#include <stddef.h>

#include "api/vpu_jsm_job_cmd_api.h"
#include "umd_common.hpp"
#include "vpu_driver/source/command/command.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <any>
#include <memory>
#include <vector>

namespace VPU {
class VPUBufferObject;
class VPUDeviceContext;

class VPUCopyCommand : public VPUCommand {
  public:
    VPUCopyCommand(const std::shared_ptr<VPUBufferObject> srcBo,
                   std::shared_ptr<VPUBufferObject> dstBo,
                   size_t size,
                   VPUDescriptor descriptor);

    static std::shared_ptr<VPUCopyCommand> create(VPUDeviceContext *ctx,
                                                  const void *srcPtr,
                                                  const std::shared_ptr<VPUBufferObject> srcBo,
                                                  void *dstPtr,
                                                  std::shared_ptr<VPUBufferObject> dstBo,
                                                  size_t size);

    const vpu_cmd_header_t *getHeader() const override {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<vpu_cmd_copy_buffer_t>(&command));
    }

    void patchDescriptorAddress(uint64_t vpuAddr) override {
        std::any_cast<vpu_cmd_copy_buffer_t>(&command)->desc_start_offset = vpuAddr;
    }

    template <class T>
    static bool
    fillDescriptor(uint64_t srcAddr, uint64_t dstAddr, size_t size, VPUDescriptor &descriptor) {
        // The some hardware limits the DMA descriptor copy size 16MB
        // because copy operation can not be interrupted
        // for efficiency reason we limit single operation to 8 MB
        static constexpr uint32_t COPY_SIZE_LIMIT = (8 << 20);

        if (srcAddr == 0 || dstAddr == 0) {
            LOG_E("Failed to get vpu address for copy descriptor");
            return false;
        }

        descriptor.numDescriptors =
            safe_cast<uint32_t>((size + COPY_SIZE_LIMIT - 1) / COPY_SIZE_LIMIT);
        descriptor.data.resize(sizeof(T) * descriptor.numDescriptors, 0);

        T *copyDescs = reinterpret_cast<T *>(descriptor.data.data());
        size_t sizeLeft = size;
        for (size_t i = 0; i < descriptor.numDescriptors; i++) {
            copyDescs[i].src_address = srcAddr;
            copyDescs[i].dst_address = dstAddr;
            if (sizeLeft < COPY_SIZE_LIMIT) {
                copyDescs[i].size = safe_cast<uint32_t>(sizeLeft);
            } else {
                copyDescs[i].size = COPY_SIZE_LIMIT;
                sizeLeft -= COPY_SIZE_LIMIT;
            }

            LOG(MISC,
                "Updated copy descriptor: src_address = %#lx,  dst_address  = %#lx, size = %#x",
                copyDescs[i].src_address,
                copyDescs[i].dst_address,
                copyDescs[i].size);

            srcAddr += COPY_SIZE_LIMIT;
            dstAddr += COPY_SIZE_LIMIT;
        }

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
