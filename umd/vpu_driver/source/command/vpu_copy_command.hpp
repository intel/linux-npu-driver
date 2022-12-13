/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/command/kmd_commit_command.hpp"

#include <cstdint>
#include <memory>

namespace VPU {

class VPUDeviceContext;

class VPUCopyCommand : public VPUCommand {
  public:
    VPUCopyCommand(VPUDeviceContext *ctx,
                   const void *srcPtr,
                   void *dstPtr,
                   size_t size,
                   CopyDirection direction,
                   VPUDescriptor &descriptor);
    size_t getCommitSize() const override;
    const uint8_t *getCommitStream() const override;
    vpu_cmd_type getCommandType() const override;

    static std::shared_ptr<VPUCopyCommand>
    create(VPUDeviceContext *ctx, const void *srcPtr, void *dstPtr, size_t size);

    template <class T>
    static bool fillDescriptor(VPUDeviceContext *ctx,
                               const void *srcPtr,
                               void *dstPtr,
                               size_t size,
                               VPUDescriptor &descriptor);
    /**
     * Print the content of copy command descriptor
     */
    static void printCopyDesc(void *descBasePtr, vpu_cmd_header_t *cmd, uint16_t pciDevId);

  private:
    KMDCommitCommand<vpu_cmd_copy_buffer_t> commitCmd;
};

} // namespace VPU
