/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/tools/source/metrics/metric_query.hpp"

#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/tools/source/metrics/metric.hpp"
#include "umd_common.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <string.h>
#include <utility>

namespace L0 {

/*
 * Firmware requires to pass an array of addresses to where the data from metric (counter) should
 * be copied. To fullfill this requirement driver use following structure for MetricQuery commands:
 *
 * struct {
 *   uint64_t addressTable[metricCount]; // points to data fields
 *   uint64_t data[metricCount];
 * }
 */
static size_t getMetricQueryAddrTableOffset(const size_t index, MetricGroup &group) {
    uint64_t metricAddressTableSize =
        getFwDataCacheAlign(sizeof(uint64_t) * group.getNumberOfMetricGroups());
    return index * getFwDataCacheAlign(metricAddressTableSize + group.getAllocationSize());
}

static size_t getMetricQueryDataOffset(const size_t index, MetricGroup &group) {
    uint64_t metricAddressTableSize =
        getFwDataCacheAlign(sizeof(uint64_t) * group.getNumberOfMetricGroups());
    return getMetricQueryAddrTableOffset(index, group) + metricAddressTableSize;
}

MetricQueryPool::MetricQueryPool(Context *pContext,
                                 MetricGroup *metricGroupInput,
                                 const size_t poolSize)
    : pContext(pContext)
    , ctx(pContext->getDeviceContext())
    , metricGroup(metricGroupInput)
    , metricQueries(poolSize) {
    size_t bufferSize = getMetricQueryAddrTableOffset(poolSize, *metricGroup);
    pQueryPoolBuffer =
        ctx->createInternalBufferObject(bufferSize, VPU::VPUBufferObject::Type::CachedFw);
    L0_THROW_WHEN(pQueryPoolBuffer == nullptr,
                  "Failed to allocate buffer object for metric query pool",
                  ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY);
}

MetricQueryPool::~MetricQueryPool() {
    if (pQueryPoolBuffer != nullptr && !ctx->freeMemAlloc(pQueryPoolBuffer)) {
        LOG_W("MetricQueryPool memory failed to be free'd");
    }
}

MetricQuery::MetricQuery(MetricGroup &metricGroupInput,
                         uint64_t *addressTablePtr,
                         uint64_t *dataPtr,
                         std::function<void()> &&destroyCb)
    : metricGroup(metricGroupInput)
    , addrTablePtr(addressTablePtr)
    , dataPtr(dataPtr)
    , destroyCb(std::move(destroyCb)) {
    metricGroupMask = 0x1 << metricGroup.getGroupIndex();
    LOG(METRIC,
        "MetricQuery -> group mask: %#x, cpu address table: %p, group index: %u, cpu data address: "
        "%p, vpu data address: %#lx",
        metricGroupMask,
        addrTablePtr,
        metricGroup.getGroupIndex(),
        dataPtr,
        addrTablePtr[metricGroup.getGroupIndex()]);
}

ze_result_t MetricQueryPool::createMetricQuery(uint32_t index,
                                               zet_metric_query_handle_t *phMetricQuery) {
    if (phMetricQuery == nullptr) {
        LOG_E("MetricQuery handle is NULL");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (!metricGroup->isActivated()) {
        LOG_E("MetricGroup (%p) is not activated! Please activate metric group first", metricGroup);
        return ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE;
    }

    if (index >= metricQueries.size()) {
        LOG_E("Index (%u) passed in is incorrect. Pool size (%lu)", index, metricQueries.size());
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (metricQueries[index] != nullptr) {
        LOG_E("Index (%u) is occupied by MetricQuery (%p)", index, metricQueries[index].get());
        return ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE;
    }

    uint64_t *addressTablePtr = reinterpret_cast<uint64_t *>(
        pQueryPoolBuffer->getBasePointer() + getMetricQueryAddrTableOffset(index, *metricGroup));
    uint64_t *dataPtr = reinterpret_cast<uint64_t *>(pQueryPoolBuffer->getBasePointer() +
                                                     getMetricQueryDataOffset(index, *metricGroup));
    addressTablePtr[metricGroup->getGroupIndex()] = ctx->getBufferVPUAddress(dataPtr);

    metricQueries[index] =
        std::make_unique<MetricQuery>(*metricGroup, addressTablePtr, dataPtr, [this, index]() {
            metricQueries[index].reset();
        });
    *phMetricQuery = metricQueries[index].get();
    LOG(METRIC, "MetricQuery created - %p", *phMetricQuery);

    return ZE_RESULT_SUCCESS;
}

ze_result_t MetricQuery::destroy() {
    destroyCb();

    LOG(METRIC, "MetricQuery destroyed - %p", this);
    return ZE_RESULT_SUCCESS;
}

ze_result_t MetricQuery::getData(size_t *pRawDataSize, uint8_t *pRawData) {
    if (pRawDataSize == nullptr) {
        LOG_E("Invalid pRawDataSize pointer");
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
        memcpy(pRawData, dataPtr, *pRawDataSize);
    } else {
        LOG_W("Input raw data pointer is NULL");
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t MetricQuery::reset() {
    size_t dataSize = metricGroup.getAllocationSize();
    memset(dataPtr, 0, dataSize);

    LOG(METRIC, "MetricQuery has been reset successfully");

    return ZE_RESULT_SUCCESS;
}

ze_result_t MetricQueryPool::destroy() {
    for (size_t i = 0; i < metricQueries.size(); i++) {
        if (metricQueries[i] != nullptr) {
            LOG_E("MetricQuery object (%p) at index (%lu) has not been destroyed",
                  metricQueries[i].get(),
                  i);
            return ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE;
        }
    }

    pContext->removeObject(this);
    LOG(METRIC, "MetricQueryPool destroyed - %p", this);
    return ZE_RESULT_SUCCESS;
}

} // namespace L0
