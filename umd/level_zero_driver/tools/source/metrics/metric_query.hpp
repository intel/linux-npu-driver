/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "umd_common.hpp"
#include "level_zero_driver/tools/source/metrics/metric.hpp"
#include <level_zero/zet_api.h>

struct _zet_metric_query_pool_handle_t {};
struct _zet_metric_query_handle_t {};

namespace L0 {

struct MetricQueryPool;

struct MetricQuery : _zet_metric_query_handle_t {
    MetricQuery(MetricGroup &metricGroupInput,
                VPU::VPUDeviceContext *ctx,
                MetricQueryPool *poolInput,
                uint32_t indexInput,
                uint64_t *queryPtrInput);
    ~MetricQuery() = default;

    inline zet_metric_query_handle_t toHandle() { return this; }
    static MetricQuery *fromHandle(zet_metric_query_handle_t handle) {
        return static_cast<MetricQuery *>(handle);
    }

    ze_result_t destroy();
    ze_result_t getData(size_t *pRawDataSize, uint8_t *pRawData);
    ze_result_t reset();

    uint32_t getIndex() const { return index; }
    uint32_t getMetricGroupMask() const { return metricGroupMask; }

    // metricQueryPtr is a CPU address to table with VPU addresses for metric query command
    uint64_t *getMetricAddrPtr() { return metricQueryPtr; }

    bool isGroupActivated() const { return metricGroup.isActivated(); }
    bool isInitialized() const { return initialized; }

  protected:
    uint64_t dataAddress = 0u;
    uint64_t *metricQueryPtr = nullptr;
    MetricGroup &metricGroup;

  private:
    bool initialized = false;
    MetricQueryPool *pool = nullptr;
    uint32_t index = 0u;
    uint32_t metricGroupMask = 0u;
};

struct MetricQueryPool : _zet_metric_query_pool_handle_t {
    MetricQueryPool(VPU::VPUDeviceContext *ctx,
                    MetricGroup *metricGroupInput,
                    const size_t poolSizeInput);
    ~MetricQueryPool() = default;

    inline zet_metric_query_pool_handle_t toHandle() { return this; }
    static MetricQueryPool *fromHandle(zet_metric_query_pool_handle_t handle) {
        return static_cast<MetricQueryPool *>(handle);
    }

    ze_result_t destroy();
    bool isInitialized() const { return initialized; }
    size_t getAddressTableSize() const { return addressTableSize; }

    void removeQuery(MetricQuery *metricQuery);
    ze_result_t createMetricQuery(uint32_t index, zet_metric_query_handle_t *phMetricQuery);

  private:
    bool initialized = false;

    VPU::VPUDeviceContext *ctx;
    MetricGroup *metricGroup = nullptr;
    size_t addressTableSize = 0u;

    /**
     * Query allocation map <index, MetricQuery pointer>
     */
    std::vector<MetricQuery *> queryAllocation;

    void *pQueryPool = nullptr;
};

} // namespace L0
