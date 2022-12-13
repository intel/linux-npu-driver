/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/device/device.hpp"
#include <level_zero/zet_api.h>

#include <memory>
#include <vector>

struct _zet_metric_group_handle_t {};
struct _zet_metric_handle_t {};

namespace L0 {

struct Device;

struct Metric : _zet_metric_handle_t {
    Metric(zet_metric_properties_t &propertiesInput);
    ~Metric() = default;

    inline zet_metric_handle_t toHandle() { return this; }
    static Metric *fromHandle(zet_metric_handle_t handle) { return static_cast<Metric *>(handle); }

    ze_result_t getProperties(zet_metric_properties_t *pProperties);
    static zet_metric_type_t getMetricType(VPU::CounterInfo::MetricType metricTypeInput);
    static zet_value_type_t getValueType(VPU::CounterInfo::ValueType valueTypeInput);

    static size_t getMetricValueSize(VPU::CounterInfo::ValueType valueTypeInput);

  private:
    zet_metric_properties_t properties;
};

struct MetricGroup : _zet_metric_group_handle_t {
    MetricGroup(zet_metric_group_properties_t &propertiesInput,
                size_t allocationSizeInput,
                std::vector<std::shared_ptr<Metric>> &metricsInput,
                uint32_t groupIndexInput);
    ~MetricGroup() = default;

    inline zet_metric_group_handle_t toHandle() { return this; }
    static MetricGroup *fromHandle(zet_metric_group_handle_t handle) {
        return static_cast<MetricGroup *>(handle);
    }

    ze_result_t getProperties(zet_metric_group_properties_t *pProperties);
    ze_result_t getMetric(uint32_t *pCount, zet_metric_handle_t *phMetrics);
    ze_result_t calculateMetricValues(zet_metric_group_calculation_type_t type,
                                      size_t rawDataSize,
                                      const uint8_t *pRawData,
                                      uint32_t *pMetricValueCount,
                                      zet_typed_value_t *pMetricValues);
    void calculateMetricValues(const uint8_t *pRawData,
                               uint32_t *pMetricValueCount,
                               zet_typed_value_t *pMetricValues);
    void calculateMaxMetricValues(const uint8_t *pRawData,
                                  uint32_t *pMetricValueCount,
                                  zet_typed_value_t *pMetricValues);

    void setActivationStatus(bool activationStatus) { activated = activationStatus; }
    bool isActivated() const { return activated; }

    size_t getAllocationSize() const { return allocationSize; }
    uint32_t getGroupIndex() const { return groupIndex; }

  private:
    bool activated = false;
    zet_metric_group_properties_t properties;
    size_t allocationSize = 0u;
    std::vector<std::shared_ptr<Metric>> metrics;
    uint32_t groupIndex;
};

struct MetricContext {
    MetricContext(Device *deviceInput)
        : device(deviceInput) {}
    ~MetricContext() = default;
    ze_result_t activateMetricGroups(uint32_t contextId,
                                     uint32_t count,
                                     zet_metric_group_handle_t *phMetricGroups);

    // Value from FW - 10 [ms]
    constexpr static uint32_t MIN_SAMPLING_RATE_NS = 10'000'000;
    uint32_t sampleSize = 0u;
    uint64_t actualBufferSize = 0u;

  protected:
    /**
       A container for keeping activated metric groups and their associated context id.
       Content: <domain number, pair<metric group, context id>>
     */
    std::vector<std::pair<uint32_t, std::pair<MetricGroup *, uint32_t>>> activatedMetricGroups;

  private:
    Device *device = nullptr;

    /**
       Deactivate all metric groups in activatedMetricGroups map.
     */
    void deactivateMetricGroups(const uint32_t contextId);
    /**
       Activate metric group and tracked in activatedMetricGroups map.
       @return true when metric group activated successfully, false otherwise
     */
    bool activateMetricGroup(const uint32_t contextId,
                             const zet_metric_group_handle_t hMetricGroup);
};

} // namespace L0
