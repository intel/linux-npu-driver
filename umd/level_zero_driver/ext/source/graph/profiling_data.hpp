/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

struct _ze_graph_profiling_query_handle_t {};
struct _ze_graph_profiling_pool_handle_t {};

#include <level_zero/ze_graph_ext.h>

namespace L0 {
struct Graph;
struct GraphProfilingPool;

struct GraphProfilingQuery : _ze_graph_profiling_query_handle_t {
  public:
    GraphProfilingQuery(VPU::VPUDeviceContext *ctx,
                        GraphProfilingPool *poolInput,
                        const uint32_t index,
                        const uint32_t size,
                        void *queryPtrInput);

    ze_result_t destroy();
    ze_result_t getData(ze_graph_profiling_type_t profilingType, uint32_t *pSize, uint8_t *pData);
    ze_result_t getLogString(uint32_t *pSize, char *pProfilingLog);

    inline ze_graph_profiling_query_handle_t toHandle() { return this; }
    static GraphProfilingQuery *fromHandle(ze_graph_profiling_query_handle_t handle) {
        return static_cast<GraphProfilingQuery *>(handle);
    }

    inline uint32_t getIndex() const { return index; }
    inline uint32_t getSize() const { return size; }
    inline void *getQueryPtr() const { return queryPtr; }

  private:
    GraphProfilingPool *pool = nullptr;
    uint32_t index = 0u;
    uint32_t size = 0u;
    void *queryPtr = nullptr;
};

struct GraphProfilingPool : _ze_graph_profiling_pool_handle_t {
  public:
    GraphProfilingPool(VPU::VPUDeviceContext *ctx,
                       const uint32_t size,
                       const uint32_t count,
                       VPU::VPUBufferObject *profilingPoolBuffer,
                       std::vector<uint8_t> *graphBlobRaw);
    ze_result_t destroy();

    ze_result_t createProfilingQuery(const uint32_t index,
                                     ze_graph_profiling_query_handle_t *phProfilingQuery);
    void removeQuery(GraphProfilingQuery *profilingQuery);

    std::vector<uint8_t> *getGraphBlobRaw() { return graphBlobRaw; };

    inline ze_graph_profiling_pool_handle_t toHandle() { return this; }
    static GraphProfilingPool *fromHandle(ze_graph_profiling_pool_handle_t handle) {
        return static_cast<GraphProfilingPool *>(handle);
    }

  private:
    VPU::VPUDeviceContext *ctx;
    uint32_t querySize = 0u;
    VPU::VPUBufferObject *profilingPool = nullptr;
    std::vector<uint8_t> *graphBlobRaw;

    std::vector<GraphProfilingQuery *> queryAllocation;
};

}; // namespace L0
