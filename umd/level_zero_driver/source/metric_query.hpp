/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/include/l0_handler.hpp"
#include "metric.hpp"

#include <functional>
#include <level_zero/ze_api.h>
#include <level_zero/zet_api.h>
#include <memory>
#include <vector>

namespace L0 {
struct Context;
} // namespace L0
namespace VPU {
class VPUBufferObject;
class VPUDeviceContext;
} // namespace VPU

struct _zet_metric_query_pool_handle_t {};
struct _zet_metric_query_handle_t {};

namespace L0 {
struct MetricQuery;

struct MetricQueryPool : _zet_metric_query_pool_handle_t, IContextObject {
    MetricQueryPool(Context *pContext, MetricGroup &metricGroupInput, const size_t poolSizeInput);

    inline zet_metric_query_pool_handle_t toHandle() { return this; }
    static MetricQueryPool *fromHandle(zet_metric_query_pool_handle_t handle) {
        return static_cast<MetricQueryPool *>(handle);
    }

    ze_result_t destroy();

    ze_result_t createMetricQuery(uint32_t index, zet_metric_query_handle_t *phMetricQuery);

  private:
    Context *pContext = nullptr;
    VPU::VPUDeviceContext *ctx = nullptr;
    MetricGroup &metricGroup;
    std::vector<std::unique_ptr<MetricQuery>> metricQueries;
    std::shared_ptr<VPU::VPUBufferObject> pQueryPoolBuffer = nullptr;
};

struct MetricQuery : _zet_metric_query_handle_t {
    MetricQuery(MetricGroup &metricGroupInput,
                uint64_t *addressTablePtr,
                uint64_t *dataPtr,
                std::shared_ptr<VPU::VPUBufferObject> poolBo,
                std::function<void()> &&destroyCb);
    ~MetricQuery() = default;

    inline zet_metric_query_handle_t toHandle() { return this; }
    static MetricQuery *fromHandle(zet_metric_query_handle_t handle) {
        return static_cast<MetricQuery *>(handle);
    }

    ze_result_t destroy();

    ze_result_t getData(size_t *pRawDataSize, uint8_t *pRawData);
    ze_result_t reset();

    uint32_t getMetricGroupMask() const { return metricGroupMask; }
    uint64_t *getMetricAddrPtr() { return addrTablePtr; }
    bool isGroupActivated() const { return metricGroup.isActivated(); }
    std::shared_ptr<VPU::VPUBufferObject> getBo() { return metricPoolBo; }

  protected:
    MetricGroup &metricGroup;
    uint64_t *addrTablePtr = nullptr;
    uint64_t *dataPtr = 0u;
    std::shared_ptr<VPU::VPUBufferObject> metricPoolBo;

  private:
    uint32_t metricGroupMask = 0u;
    std::function<void()> destroyCb;
};

} // namespace L0
