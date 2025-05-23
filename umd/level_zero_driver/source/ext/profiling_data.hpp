/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstdint>

#include <functional>
#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_profiling_ext.h>
#include <memory>
#include <vector>

namespace VPU {
class VPUBufferObject;
} // namespace VPU

struct _ze_graph_profiling_query_handle_t {};
struct _ze_graph_profiling_pool_handle_t {};

namespace L0 {

class BlobContainer;

struct GraphProfilingQuery : _ze_graph_profiling_query_handle_t {
  public:
    GraphProfilingQuery(const BlobContainer *blob,
                        const uint32_t size,
                        void *queryPtrInput,
                        std::shared_ptr<VPU::VPUBufferObject> profilingMemoryBo,
                        std::function<void()> &&destroyCb);

    ze_result_t destroy();
    ze_result_t getData(ze_graph_profiling_type_t profilingType, uint32_t *pSize, uint8_t *pData);
    ze_result_t getLogString(uint32_t *pSize, char *pProfilingLog);

    inline ze_graph_profiling_query_handle_t toHandle() { return this; }
    static GraphProfilingQuery *fromHandle(ze_graph_profiling_query_handle_t handle) {
        return static_cast<GraphProfilingQuery *>(handle);
    }

    inline uint8_t *getQueryPtr() const { return static_cast<uint8_t *>(data); }
    inline std::shared_ptr<VPU::VPUBufferObject> getBo() { return profilingBo; }
    inline uint32_t getSize() { return size; }

  private:
    uint32_t size = 0u;
    void *data = nullptr;
    const BlobContainer *blob;
    std::shared_ptr<VPU::VPUBufferObject> profilingBo;
    std::function<void()> destroyCb;
};

struct GraphProfilingPool : _ze_graph_profiling_pool_handle_t {
  public:
    GraphProfilingPool(const uint32_t size,
                       const uint32_t count,
                       const BlobContainer *blob,
                       std::shared_ptr<VPU::VPUBufferObject> profilingMemory,
                       std::function<void(GraphProfilingPool *)> destroyCb);

    ze_result_t destroy();
    ze_result_t createProfilingQuery(const uint32_t index,
                                     ze_graph_profiling_query_handle_t *phProfilingQuery);

    inline ze_graph_profiling_pool_handle_t toHandle() { return this; }
    static GraphProfilingPool *fromHandle(ze_graph_profiling_pool_handle_t handle) {
        return static_cast<GraphProfilingPool *>(handle);
    }

  private:
    uint32_t querySize = 0u;
    std::shared_ptr<VPU::VPUBufferObject> poolBuffer = nullptr;
    const BlobContainer *blob;

    std::vector<std::unique_ptr<GraphProfilingQuery>> queries;
    std::function<void(GraphProfilingPool *)> destroyCb;
};

}; // namespace L0
