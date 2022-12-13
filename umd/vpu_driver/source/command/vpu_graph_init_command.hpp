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

class VPUGraphInitCommand : public VPUCommand {
  public:
    /**
     * VPU graph init command.
     *
     * @param umdBlobId [IN]: ID that is assigned to graph in firmware.
     * @param blobSize [IN]: Size of blob data.
     * @param kernelBuffer [IN]: BufferObject of kernel heap.
     * @param scratchBuffer [IN]: BufferObject of scratch heap.
     * @param metadataBuffer [IN]: BufferObject of metadata heap.
     * @param actKernelBuffer [IN]: BufferObject of activation kernel heap.
     */
    VPUGraphInitCommand(VPUDeviceContext *ctx,
                        uint64_t umdBlobId,
                        const size_t blobSize,
                        VPUBufferObject *kernelBuffer,
                        VPUBufferObject *scratchBuffer,
                        VPUBufferObject *metadataBuffer,
                        VPUBufferObject *actKernelBuffer,
                        size_t scratchSize = 0,
                        size_t metadataSize = 0);
    ~VPUGraphInitCommand();
    size_t getCommitSize() const override;
    const uint8_t *getCommitStream() const override;
    vpu_cmd_type getCommandType() const override;

    static std::shared_ptr<VPUGraphInitCommand> create(VPUDeviceContext *ctx,
                                                       uint64_t umdBlobId,
                                                       void *blobData,
                                                       const size_t blobSize,
                                                       size_t scratchSize,
                                                       size_t metadataSize,
                                                       const void *kernelData = nullptr,
                                                       const size_t kernelDataSize = 0u);

    static const int bufferCount = 4;

  private:
    void fillDescriptor();

  private:
    VPUDeviceContext *ctx = nullptr;
    KMDCommitCommand<vpu_cmd_ov_blob_initialize_t> commitCmd;

    VPUBufferObject *kernelBuffer = nullptr;
    VPUBufferObject *scratchBuffer = nullptr;
    VPUBufferObject *metadataBuffer = nullptr;
    VPUBufferObject *actKernelBuffer = nullptr;

    size_t scratchSize = 0;
    size_t metadataSize = 0;
};

} // namespace VPU
