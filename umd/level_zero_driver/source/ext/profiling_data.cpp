/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "profiling_data.hpp"

#include "compiler.hpp"
#include "level_zero/ze_api.h"
#include "level_zero_driver/include/l0_exception.hpp"
#include "umd_common.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <algorithm>
#include <string.h>
#include <string>
#include <utility>

namespace L0 {

static thread_local std::string lastErrorMsg = {};

GraphProfilingPool::GraphProfilingPool(VPU::VPUDeviceContext *ctx,
                                       const uint32_t size,
                                       const uint32_t count,
                                       const BlobContainer *blob,
                                       std::function<void(GraphProfilingPool *)> destroyCb)
    : ctx(ctx)
    , querySize(size)
    , blob(blob)
    , queries(count)
    , destroyCb(std::move(destroyCb)) {
    size_t poolSize = queries.size() * getFwDataCacheAlign(querySize);
    poolBuffer = ctx->createInternalBufferObject(poolSize, VPU::VPUBufferObject::Type::CachedDma);
    L0_THROW_WHEN(poolBuffer == nullptr,
                  "Failed to allocate buffer object for profiling pool",
                  ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY);
    memset(poolBuffer->getBasePointer(), 0, poolSize);
}

GraphProfilingPool::~GraphProfilingPool() {
    if (poolBuffer != nullptr && !ctx->freeMemAlloc(poolBuffer)) {
        LOG_W("GraphProfilingPool memory failed to be free'd");
    }
}

GraphProfilingQuery::GraphProfilingQuery(const BlobContainer *blob,
                                         const uint32_t size,
                                         void *pData,
                                         std::function<void()> &&destroyCb)
    : size(size)
    , data(pData)
    , blob(blob)
    , destroyCb(std::move(destroyCb)) {}

ze_result_t
GraphProfilingPool::createProfilingQuery(const uint32_t index,
                                         ze_graph_profiling_query_handle_t *phProfilingQuery) {
    if (index >= queries.size()) {
        LOG_E("Index %u is greater than pool size %lu", index, queries.size());
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (phProfilingQuery == nullptr) {
        LOG_E("Invalid phProfilingQuery pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (queries[index] != nullptr) {
        LOG_E("Index %u is occupied by GraphProfilingQuery (%p)", index, queries[index].get());
        return ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE;
    }

    auto *dataPtr = poolBuffer->getBasePointer() + (index * getFwDataCacheAlign(querySize));
    queries[index] =
        std::make_unique<GraphProfilingQuery>(blob, querySize, dataPtr, [this, index]() {
            queries[index].reset();
        });
    *phProfilingQuery = queries[index].get();
    LOG(GRAPH, "GraphProfilingQuery created - %p", *phProfilingQuery);
    return ZE_RESULT_SUCCESS;
}

ze_result_t GraphProfilingQuery::getData(ze_graph_profiling_type_t profilingType,
                                         uint32_t *pSize,
                                         uint8_t *pData) {
    if (pSize == nullptr) {
        LOG_E("Invalid pSize pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (profilingType == ZE_GRAPH_PROFILING_LAYER_LEVEL ||
        profilingType == ZE_GRAPH_PROFILING_TASK_LEVEL) {
        return Compiler::getDecodedProfilingBuffer(profilingType,
                                                   *blob,
                                                   static_cast<uint8_t *>(data),
                                                   size,
                                                   pSize,
                                                   pData,
                                                   lastErrorMsg);
    }

    if (profilingType != ZE_GRAPH_PROFILING_RAW) {
        LOG_E("Invalid profiling type");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (*pSize == 0 || *pSize > size)
        *pSize = size;

    if (pData != nullptr)
        memcpy(pData, data, *pSize);

    return ZE_RESULT_SUCCESS;
}

ze_result_t GraphProfilingQuery::destroy() {
    destroyCb();
    LOG(GRAPH, "GraphProfilingQuery destroyed - %p", this);
    return ZE_RESULT_SUCCESS;
}

ze_result_t GraphProfilingQuery::getLogString(uint32_t *pSize, char *pProfilingLog) {
    if (pSize == nullptr) {
        LOG_E("Invalid pSize pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (*pSize == 0) {
        *pSize = static_cast<uint32_t>(lastErrorMsg.size() + 1);
        return ZE_RESULT_SUCCESS;
    }

    if (pProfilingLog == nullptr) {
        LOG_E("Invalid pProfilingLog pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    *pSize = std::min(*pSize, static_cast<uint32_t>(lastErrorMsg.size() + 1));
    memcpy(pProfilingLog, lastErrorMsg.c_str(), *pSize);
    return ZE_RESULT_SUCCESS;
}

ze_result_t GraphProfilingPool::destroy() {
    for (size_t i = 0; i < queries.size(); i++) {
        if (queries[i] != nullptr) {
            LOG_E("GraphProfilingQuery object (%p) at index (%lu) has not been destroyed",
                  queries[i].get(),
                  i);
            return ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE;
        }
    }

    destroyCb(this);
    LOG(GRAPH, "GraphProfilingPool destroyed - %p", this);
    return ZE_RESULT_SUCCESS;
}

}; // namespace L0
