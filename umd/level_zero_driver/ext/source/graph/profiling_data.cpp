/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/ze_api.h"
#include "level_zero_driver/ext/source/graph/graph.hpp"

#include "vpu_driver/source/utilities/log.hpp"

#include <string.h>

#ifdef ENABLE_VPUX_COMPILER
#include "VPUXCompilerL0.h"
#endif

namespace L0 {

GraphProfilingPool::GraphProfilingPool(VPU::VPUDeviceContext *ctx,
                                       const uint32_t size,
                                       const uint32_t count,
                                       VPU::VPUBufferObject *profilingPoolBuffer,
                                       std::vector<uint8_t> *graphBlobRaw)
    : ctx(ctx)
    , querySize(size)
    , profilingPool(profilingPoolBuffer)
    , graphBlobRaw(graphBlobRaw) {
    queryAllocation.resize(count, nullptr);
};

GraphProfilingQuery::GraphProfilingQuery(VPU::VPUDeviceContext *ctx,
                                         GraphProfilingPool *poolInput,
                                         const uint32_t index,
                                         const uint32_t size,
                                         void *queryPtrInput)
    : pool(poolInput)
    , index(index)
    , size(size)
    , queryPtr(queryPtrInput){};

ze_result_t
GraphProfilingPool::createProfilingQuery(const uint32_t index,
                                         ze_graph_profiling_query_handle_t *phProfilingQuery) {
    if (index >= queryAllocation.size()) {
        LOG_E("Index (%u) passed in is incorrect. Pool size (%lu).", index, queryAllocation.size());
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (!phProfilingQuery) {
        LOG_E("ProfilingQuery handle is NULL.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (queryAllocation[index] != nullptr) {
        LOG_E("Pool at index (%u) has already been allocated to Query (%p)",
              index,
              queryAllocation[index]);
        return ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE;
    }

    auto *queryPtr = reinterpret_cast<void *>(profilingPool->getBasePointer() +
                                              (index * getFwDataCacheAlign(querySize)));

    auto profilingQuery = new GraphProfilingQuery(ctx, this, index, querySize, queryPtr);
    if (!profilingQuery) {
        LOG_E("ProfilingQuery creation has failed.");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    queryAllocation[index] = profilingQuery;

    *phProfilingQuery = profilingQuery->toHandle();
    return ZE_RESULT_SUCCESS;
}

void GraphProfilingPool::removeQuery(GraphProfilingQuery *profilingQuery) {
    uint32_t index = profilingQuery->getIndex();

    if (queryAllocation[index] != nullptr) {
        queryAllocation[index] = nullptr;
        LOG_I("QueryPool allocation at index %u has been reset.", index);
    } else {
        LOG_W("Query (%p) not found in QueryPool (%p)", profilingQuery, this);
    }
}

ze_result_t GraphProfilingQuery::getData(ze_graph_profiling_type_t profilingType,
                                         uint32_t *pSize,
                                         uint8_t *pData) {
    if (pSize == nullptr) {
        LOG_E("Invalid size pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (profilingType == ZE_GRAPH_PROFILING_LAYER_LEVEL ||
        profilingType == ZE_GRAPH_PROFILING_TASK_LEVEL) {
        return Compiler::getDecodedProfilingBuffer(profilingType,
                                                   pool->getGraphBlobRaw(),
                                                   static_cast<uint8_t *>(queryPtr),
                                                   size,
                                                   pSize,
                                                   pData);
    }

    if (profilingType != ZE_GRAPH_PROFILING_RAW) {
        LOG_E("Invalid profiling type.");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (*pSize == 0 || *pSize > size)
        *pSize = size;

    if (pData != nullptr)
        memcpy(pData, queryPtr, *pSize);

    return ZE_RESULT_SUCCESS;
}

ze_result_t GraphProfilingQuery::destroy() {
    pool->removeQuery(this);
    delete this;
    return ZE_RESULT_SUCCESS;
}

ze_result_t GraphProfilingPool::destroy() {
    size_t index = 0;
    for (auto &query : queryAllocation) {
        if (query != nullptr) {
            LOG_E("Query object (%p) of index (%lu) has not been deleted.", query, index++);
            return ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE;
        }
    }

    queryAllocation.clear();

    if (!ctx) {
        LOG_E("Failed to get device context.");
        return ZE_RESULT_ERROR_DEVICE_LOST;
    }

    if (profilingPool != nullptr && !ctx->freeMemAlloc(profilingPool)) {
        LOG_W("GraphProfilingPool memory failed to be free'd.");
    } else {
        LOG_I("GraphProfilingPool memory free'd.");
    }

    delete this;

    return ZE_RESULT_SUCCESS;
}

}; // namespace L0
