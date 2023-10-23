/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "vpu_driver/source/command/vpu_command.hpp"

#include <cstdint>
#include <memory>

namespace VPU {

class VPUGraphExecuteCommand : public VPUCommand {
  public:
    /**
     * VPU graph execute command.
     *
     * @param umdBlobId [IN]: ID used by firmware to match with initialized graph.
     * @param inputdata [IN]: Pointer to heap for inference input.
     * @param outputBuffer [IN]: Pointer to heap for inference output.
     * @param profilingOutputSize [IN]: size of profiling buffer.
     */
    VPUGraphExecuteCommand(VPUDeviceContext *ctx,
                           uint64_t umdBlobId,
                           const std::vector<std::pair<const void *, uint32_t>> &inputBuffers,
                           const std::vector<std::pair<const void *, uint32_t>> &outputBuffers,
                           const std::vector<VPUBufferObject *> &graphInitBufferObjects,
                           size_t profilingSize,
                           void *profilingBuffer,
                           std::vector<uint64_t> inputArray,
                           std::vector<uint64_t> outputArray);
    ~VPUGraphExecuteCommand() = default;
    VPUGraphExecuteCommand(VPUGraphExecuteCommand const &) = delete;
    VPUGraphExecuteCommand &operator=(VPUGraphExecuteCommand const &) = delete;

    static std::shared_ptr<VPUGraphExecuteCommand>
    create(VPUDeviceContext *ctx,
           uint64_t umdBlobId,
           const std::vector<std::pair<const void *, uint32_t>> &inputBuffers,
           const std::vector<std::pair<const void *, uint32_t>> &outputBuffers,
           const std::vector<VPUBufferObject *> &graphInitBufferObjects,
           size_t profilingSize = 0,
           void *profilingBuffer = nullptr);

    const vpu_cmd_header_t *getHeader() const {
        return reinterpret_cast<const vpu_cmd_header_t *>(
            std::any_cast<vpu_cmd_ov_blob_execute_t>(&command));
    }

  private:
    VPUDeviceContext *ctx = nullptr;
    size_t profilingSize = 0u;
    void *profilingBuffer = nullptr;

    static bool checkUserArgs(VPUDeviceContext *ctx,
                              const std::vector<std::pair<const void *, uint32_t>> &userArgs,
                              std::vector<uint64_t> &vpuAddr);
    void fillDescriptor(std::vector<uint64_t> &inputArray,
                        std::vector<uint64_t> &outputArray,
                        const std::vector<uint32_t> &inputArraySize,
                        const std::vector<uint32_t> &outputArraySize);
};

} // namespace VPU
