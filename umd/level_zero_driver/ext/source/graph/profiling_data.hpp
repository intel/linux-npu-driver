/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/context/context.hpp"
#include <level_zero/ze_graph_ext.h>

struct _ze_graph_profiling_query_handle_t {};
struct _ze_graph_profiling_pool_handle_t {};

namespace L0 {

struct Graph;
struct GraphProfilingPool;

struct GraphProfilingQuery : _ze_graph_profiling_query_handle_t {
  public:
    GraphProfilingQuery(std::vector<uint8_t> *graphBlobRaw,
                        const uint32_t size,
                        void *queryPtrInput,
                        std::function<void()> &&destroyCb);

    ze_result_t destroy();
    ze_result_t getData(ze_graph_profiling_type_t profilingType, uint32_t *pSize, uint8_t *pData);
    ze_result_t getLogString(uint32_t *pSize, char *pProfilingLog);

    inline ze_graph_profiling_query_handle_t toHandle() { return this; }
    static GraphProfilingQuery *fromHandle(ze_graph_profiling_query_handle_t handle) {
        return static_cast<GraphProfilingQuery *>(handle);
    }

    inline void *getQueryPtr() const { return data; }

  private:
    uint32_t size = 0u;
    void *data = nullptr;
    std::vector<uint8_t> *graphBlobRaw;
    std::function<void()> destroyCb;
};

struct GraphProfilingPool : _ze_graph_profiling_pool_handle_t {
  public:
    GraphProfilingPool(VPU::VPUDeviceContext *ctx,
                       const uint32_t size,
                       const uint32_t count,
                       std::vector<uint8_t> *graphBlobRaw,
                       std::function<void(GraphProfilingPool *)> destroyCb);
    ~GraphProfilingPool();

    ze_result_t destroy();
    ze_result_t createProfilingQuery(const uint32_t index,
                                     ze_graph_profiling_query_handle_t *phProfilingQuery);

    inline ze_graph_profiling_pool_handle_t toHandle() { return this; }
    static GraphProfilingPool *fromHandle(ze_graph_profiling_pool_handle_t handle) {
        return static_cast<GraphProfilingPool *>(handle);
    }

  private:
    VPU::VPUDeviceContext *ctx = nullptr;
    uint32_t querySize = 0u;
    VPU::VPUBufferObject *poolBuffer = nullptr;
    std::vector<uint8_t> *graphBlobRaw;

    std::vector<std::unique_ptr<GraphProfilingQuery>> queries;
    std::function<void(GraphProfilingPool *)> destroyCb;
};

}; // namespace L0
