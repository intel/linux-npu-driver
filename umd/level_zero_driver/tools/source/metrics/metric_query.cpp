/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/tools/source/metrics/metric.hpp"
#include "level_zero_driver/tools/source/metrics/metric_query.hpp"

#include "vpu_driver/source/utilities/log.hpp"

namespace L0 {

MetricQueryPool::MetricQueryPool(VPU::VPUDeviceContext *ctx,
                                 MetricGroup *metricGroupInput,
                                 const size_t poolSize)
    : ctx(ctx)
    , metricGroup(metricGroupInput) {
    if (ctx == nullptr) {
        LOG_E("Invalid VPU Device Context.");
        return;
    }

    if (poolSize == 0) {
        LOG_E("Invalid metric query pool size.");
        return;
    }

    if (pQueryPool != nullptr) {
        LOG_E("MetricQuery pool has already been allocated. Possible memory leak.");
        return;
    }

    if (metricGroup == nullptr) {
        LOG_E("MetricGroup passed in is NULL.");
        return;
    }

    size_t metricBufferSize = metricGroup->getAllocationSize();
    LOG_I("Query Data buffer size: %lu", metricBufferSize);

    size_t numberOfGroups = metricGroup->getNumberOfMetricGroups();
    addressTableSize = getFwDataCacheAlign(sizeof(uint64_t) * numberOfGroups);

    size_t bufferSize = poolSize * getFwDataCacheAlign(addressTableSize + metricBufferSize);
    auto queryPoolBO =
        ctx->createInternalBufferObject(bufferSize, VPU::VPUBufferObject::Type::CachedLow);
    if (queryPoolBO == nullptr) {
        LOG_E("Failed to allocate buffer object for query pool");
        return;
    }

    pQueryPool = queryPoolBO->getBasePointer();
    LOG_I("MetricQueryPool pointer: %p", pQueryPool);

    queryAllocation.resize(poolSize, nullptr);

    // Mark successfully initialized.
    initialized = true;
}

MetricQuery::MetricQuery(MetricGroup &metricGroupInput,
                         VPU::VPUDeviceContext *ctx,
                         MetricQueryPool *poolInput,
                         uint32_t indexInput,
                         uint64_t *queryPtrInput)
    : metricGroup(metricGroupInput)
    , pool(poolInput)
    , index(indexInput) {
    uint32_t groupBit = metricGroup.getGroupIndex();
    metricGroupMask = 0x1 << groupBit;
    LOG_I("Metric Group mask for MetricQuery: %x", metricGroupMask);

    if (pool == nullptr || queryPtrInput == nullptr) {
        LOG_E("QueryPool/QueryArrayPtr passed in is NULL!");
        return;
    }

    metricQueryPtr = reinterpret_cast<uint64_t *>(queryPtrInput);

    dataAddress = reinterpret_cast<uint64_t>(metricQueryPtr) + pool->getAddressTableSize();

    metricQueryPtr[groupBit] = ctx->getBufferVPUAddress(reinterpret_cast<uint64_t *>(dataAddress));

    LOG_I("Data pointer %p, CPU address %p to VPU address table for metric groups, metric data VPU "
          "address %lu stored at position %u",
          reinterpret_cast<uint64_t *>(dataAddress),
          metricQueryPtr,
          metricQueryPtr[groupBit],
          groupBit);

    // Mark successfully initialized.
    initialized = true;
}

ze_result_t MetricQueryPool::createMetricQuery(uint32_t index,
                                               zet_metric_query_handle_t *phMetricQuery) {
    if (phMetricQuery == nullptr) {
        LOG_E("MetricQuery handle is NULL.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (!metricGroup->isActivated()) {
        LOG_E("MetricGroup (%p) is not activated! Please activate metric group before Query "
              "creation.",
              metricGroup);
        return ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE;
    }

    if (index >= queryAllocation.size()) {
        LOG_E("Index (%u) passed in is incorrect. Pool size (%lu).", index, queryAllocation.size());
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (queryAllocation[index] != nullptr) {
        LOG_E("Pool at index (%u) has already been allocated to Query (%p)",
              index,
              queryAllocation[index]);
        return ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE;
    }

    uint64_t *queryPtr = reinterpret_cast<uint64_t *>(
        reinterpret_cast<uint64_t>(pQueryPool) +
        (index * getFwDataCacheAlign(addressTableSize + metricGroup->getAllocationSize())));

    MetricQuery *metricQuery = new MetricQuery(*metricGroup, ctx, this, index, queryPtr);
    if (metricQuery == nullptr) {
        LOG_E("MetricQuery creation has failed.");
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;
    } else if (!metricQuery->isInitialized()) {
        LOG_E("MetricQuery has not been initialized correctly.");
        metricQuery->destroy();
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;
    }

    queryAllocation[index] = metricQuery;

    *phMetricQuery = metricQuery->toHandle();

    return ZE_RESULT_SUCCESS;
}

void MetricQueryPool::removeQuery(MetricQuery *metricQuery) {
    uint32_t index = metricQuery->getIndex();

    if (queryAllocation[index] != nullptr) {
        queryAllocation[index] = nullptr;
        LOG_I("QueryPool allocation at index %u has been reset.", index);
    } else {
        LOG_W("Query (%p) not found in QueryPool (%p)", metricQuery, this);
    }
}

ze_result_t MetricQuery::destroy() {
    pool->removeQuery(this);
    this->reset();
    delete this;

    return ZE_RESULT_SUCCESS;
}

ze_result_t MetricQuery::getData(size_t *pRawDataSize, uint8_t *pRawData) {
    if (pRawDataSize == nullptr) {
        LOG_E("Invalid pRawDataSize pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    size_t dataSize = metricGroup.getAllocationSize();

    if (*pRawDataSize == 0) {
        *pRawDataSize = dataSize;
        return ZE_RESULT_SUCCESS;
    } else if (*pRawDataSize > dataSize) {
        *pRawDataSize = dataSize;
    }

    if (pRawData != nullptr) {
        if (dataSize > *pRawDataSize) {
            LOG_E("Failed to copy data. dataSize exceeds *pRawDataSize");
            return ZE_RESULT_ERROR_UNKNOWN;
        }
        memcpy(pRawData, reinterpret_cast<uint64_t *>(dataAddress), *pRawDataSize);
    } else {
        LOG_W("Input raw data pointer is NULL.");
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t MetricQuery::reset() {
    if (metricQueryPtr == nullptr) {
        LOG_E("Invalid pointer. metricQueryPtr: %p.", metricQueryPtr);
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    size_t dataSize = metricGroup.getAllocationSize();
    memset(reinterpret_cast<uint64_t *>(dataAddress), 0, dataSize);

    LOG_I("MetricQuery has been reset successfully.");

    return ZE_RESULT_SUCCESS;
}

ze_result_t MetricQueryPool::destroy() {
    size_t index = 0;
    for (auto &query : queryAllocation) {
        if (query != nullptr) {
            LOG_E("Query object (%p) of index (%lu) has not been deleted.", query, index++);
            return ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE;
        }
    }

    queryAllocation.clear();
    if (pQueryPool != nullptr && !ctx->freeMemAlloc(pQueryPool)) {
        LOG_W("MetricQueryPool memory failed to be free'd.");
    } else {
        LOG_I("MetricQueryPool memory free'd.");
    }

    delete this;

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
