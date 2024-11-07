/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/ext/source/graph/blob_container.hpp"
#include "level_zero_driver/include/l0_handler.hpp"
#include "vpu_driver/source/command/vpu_command.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_ext.h>
#include <level_zero/ze_graph_profiling_ext.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace VPU {
class VPUDeviceContext;
} // namespace VPU

struct _ze_graph_handle_t {};

namespace L0 {
class IParser;
struct Context;
struct GraphProfilingPool;

struct Graph : _ze_graph_handle_t, IContextObject {
    Graph(Context *pCtx, const ze_graph_desc_2_t *pDesc);
    ~Graph() = default;

    static ze_result_t create(const ze_context_handle_t hContext,
                              const ze_device_handle_t hDevice,
                              const ze_graph_desc_2_t *pDesc,
                              ze_graph_handle_t *phGraph);
    ze_result_t destroy();
    ze_result_t getNativeBinary(size_t *pSize, uint8_t *pGraphNativeBinary);
    ze_result_t getNativeBinary2(size_t *pSize, const uint8_t **pGraphNativeBinary);
    ze_result_t setArgumentValue(uint32_t argIndex, const void *pArgValue);
    ze_result_t getProperties(ze_graph_properties_t *pGraphProperties);
    ze_result_t getProperties2(ze_graph_properties_2_t *pGraphProperties);
    ze_result_t getArgumentProperties(uint32_t argIndex,
                                      ze_graph_argument_properties_t *pGraphArgProps);
    ze_result_t getArgumentProperties2(uint32_t argIndex,
                                       ze_graph_argument_properties_2_t *pGraphArgProps);
    ze_result_t getArgumentProperties3(uint32_t argIndex,
                                       ze_graph_argument_properties_3_t *pGraphArgProps);
    ze_result_t getArgumentMetadata(uint32_t argIndex,
                                    ze_graph_argument_metadata_t *pGraphArgMetadata);

    ze_result_t createProfilingPool(uint32_t count,
                                    ze_graph_profiling_pool_handle_t *phProfilingPool);
    static ze_result_t getProfilingDataProperties(
        ze_device_profiling_data_properties_t *pDeviceProfilingDataProperties);
    static ze_result_t
    getDeviceGraphProperties(ze_device_handle_t hDevice,
                             ze_device_graph_properties_t *pDeviceGraphProperties);
    static ze_result_t
    getDeviceGraphProperties2(ze_device_handle_t hDevice,
                              ze_device_graph_properties_2_t *pDeviceGraphProperties2);
    uint32_t getProfilingOutputSize() const { return profilingOutputSize; }

    static Graph *fromHandle(ze_graph_handle_t handle) { return static_cast<Graph *>(handle); }
    inline ze_graph_handle_t toHandle() { return this; }

    ze_result_t parserInitialize();

    std::shared_ptr<VPU::VPUCommand> allocateGraphInitCommand(VPU::VPUDeviceContext *ctx);
    std::shared_ptr<VPU::VPUCommand> allocateGraphExecuteCommand(VPU::VPUDeviceContext *ctx,
                                                                 void *profilingQueryPtr);

    static ze_result_t getLogString(uint32_t *pSize, char *pBuildLog);

  private:
    void initialize();
    void addDeviceConfigToBuildFlags();

    Context *pContext;
    VPU::VPUDeviceContext *ctx;
    ze_graph_desc_2_t desc;
    std::string buildFlags;

    std::unique_ptr<BlobContainer> blob;

    std::vector<std::pair<const void *, uint32_t>> inputArgs;
    std::vector<std::pair<const void *, uint32_t>> outputArgs;

    std::vector<ze_graph_argument_properties_3_t> argumentProperties;
    std::vector<ze_graph_argument_metadata_t> argumentMetadata;
    uint32_t profilingOutputSize = 0u;

    std::shared_ptr<IParser> parser = nullptr;
    std::unordered_map<void *, std::unique_ptr<GraphProfilingPool>> profilingPools;
};

} // namespace L0
