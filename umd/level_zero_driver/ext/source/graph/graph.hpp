/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "vpu_driver/source/command/vpu_graph_init_command.hpp"
#include "vpu_driver/source/command/vpu_graph_exe_command.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "level_zero_driver/ext/source/graph/compiler.hpp"
#include "level_zero_driver/ext/source/graph/profiling_data.hpp"
#include "level_zero_driver/ext/source/graph/elf_parser.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_ext.h>

#include <boost/safe_numerics/safe_integer.hpp>
#include <cstring>
#include <vector>

struct _ze_graph_handle_t {};

namespace L0 {
struct Context;

struct Graph : _ze_graph_handle_t {
    Graph(VPU::VPUDeviceContext *pCtx, const ze_graph_desc_t *pDesc);
    ~Graph() = default;

    static ze_result_t create(const ze_context_handle_t hContext,
                              const ze_device_handle_t hDevice,
                              const ze_graph_desc_t *pDesc,
                              ze_graph_handle_t *phGraph);
    ze_result_t destroy();
    ze_result_t getNativeBinary(size_t *pSize, uint8_t *pGraphNativeBinary);
    ze_result_t setArgumentValue(uint32_t argIndex, const void *pArgValue);
    ze_result_t getProperties(ze_graph_properties_t *pGraphProperties);
    ze_result_t getArgumentProperties(boost::safe_numerics::safe<uint32_t> argIndex,
                                      ze_graph_argument_properties_t *pGraphArgProps);

    ze_result_t createProfilingPool(uint32_t count,
                                    ze_graph_profiling_pool_handle_t *phProfilingPool);
    static ze_result_t getProfilingDataProperties(
        ze_device_profiling_data_properties_t *pDeviceProfilingDataProperties);
    static ze_result_t
    getDeviceGraphProperties(ze_device_graph_properties_t *pDeviceGraphProperties);
    uint32_t getProfilingOutputSize() const { return profilingOutputSize; }

    static Graph *fromHandle(ze_graph_handle_t handle) { return static_cast<Graph *>(handle); }
    inline ze_graph_handle_t toHandle() { return this; }

    std::shared_ptr<VPU::VPUCommand> allocateGraphInitCommand(VPU::VPUDeviceContext *ctx);
    std::shared_ptr<VPU::VPUCommand> allocateGraphExecuteCommand(VPU::VPUDeviceContext *ctx,
                                                                 void *profilingQueryPtr);

  private:
    ze_result_t initialize();
    ze_result_t getUserKernelData();

    VPU::VPUDeviceContext *ctx;
    ze_graph_desc_t desc;
    std::vector<uint8_t> graphBlobRaw;

    uint64_t blobId = 0u;
    uint32_t scratchSize = 0u;
    const uint32_t metadataSize = 4 * 1024 * 1024; /* 4MB */
    uint32_t profilingOutputSize = 0u;
    std::vector<std::pair<const void *, uint32_t>> inputArgs;
    std::vector<std::pair<const void *, uint32_t>> outputArgs;
    std::vector<ze_graph_argument_properties_t> argumentProperties;
    std::optional<ElfParser> elfParser = std::nullopt;

    void *pKernelData = nullptr;
    size_t kernelDataSize = 0u;

    std::vector<std::shared_ptr<VPU::VPUGraphInitCommand>> graphInitCmds;
};

} // namespace L0
