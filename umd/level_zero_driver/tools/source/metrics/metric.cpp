/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/tools/source/metrics/metric.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace L0 {

Metric::Metric(zet_metric_properties_t &propertiesInput)
    : properties(propertiesInput) {}

zet_metric_type_t Metric::getMetricType(VPU::CounterInfo::MetricType metricTypeInput) {
    static constexpr std::array<std::pair<VPU::CounterInfo::MetricType, zet_metric_type_t>, 8>
        metricMap = {
            {{VPU::CounterInfo::MetricType::METRIC_TYPE_DURATION, ZET_METRIC_TYPE_DURATION},
             {VPU::CounterInfo::MetricType::METRIC_TYPE_EVENT, ZET_METRIC_TYPE_EVENT},
             {VPU::CounterInfo::MetricType::METRIC_TYPE_EVENT_WITH_RANGE,
              ZET_METRIC_TYPE_EVENT_WITH_RANGE},
             {VPU::CounterInfo::MetricType::METRIC_TYPE_THROUGHPUT, ZET_METRIC_TYPE_THROUGHPUT},
             {VPU::CounterInfo::MetricType::METRIC_TYPE_TIMESTAMP, ZET_METRIC_TYPE_TIMESTAMP},
             {VPU::CounterInfo::MetricType::METRIC_TYPE_FLAG, ZET_METRIC_TYPE_FLAG},
             {VPU::CounterInfo::MetricType::METRIC_TYPE_RATIO, ZET_METRIC_TYPE_RATIO},
             {VPU::CounterInfo::MetricType::METRIC_TYPE_RAW, ZET_METRIC_TYPE_RAW}}};

    for (const auto &metricType : metricMap)
        if (metricType.first == metricTypeInput)
            return metricType.second;

    LOG_W("Unsupported metric type provided: %d", metricTypeInput);
    return ZET_METRIC_TYPE_FORCE_UINT32;
}

zet_value_type_t Metric::getValueType(VPU::CounterInfo::ValueType valueTypeInput) {
    static constexpr std::array<std::pair<VPU::CounterInfo::ValueType, zet_value_type_t>, 5>
        valueMap = {{{VPU::CounterInfo::ValueType::VALUE_TYPE_UINT32, ZET_VALUE_TYPE_UINT32},
                     {VPU::CounterInfo::ValueType::VALUE_TYPE_UINT64, ZET_VALUE_TYPE_UINT64},
                     {VPU::CounterInfo::ValueType::VALUE_TYPE_FLOAT32, ZET_VALUE_TYPE_FLOAT32},
                     {VPU::CounterInfo::ValueType::VALUE_TYPE_FLOAT64, ZET_VALUE_TYPE_FLOAT64},
                     {VPU::CounterInfo::ValueType::VALUE_TYPE_BOOL8, ZET_VALUE_TYPE_BOOL8}}};

    for (const auto &valueType : valueMap)
        if (valueType.first == valueTypeInput)
            return valueType.second;

    LOG_W("Unsupported value type provided: %d", valueTypeInput);
    return ZET_VALUE_TYPE_FORCE_UINT32;
}

size_t Metric::getMetricValueSize(VPU::CounterInfo::ValueType valueTypeInput) {
    switch (valueTypeInput) {
    case VPU::CounterInfo::ValueType::VALUE_TYPE_UINT32:
    case VPU::CounterInfo::ValueType::VALUE_TYPE_FLOAT32:
        return sizeof(uint32_t);
    case VPU::CounterInfo::ValueType::VALUE_TYPE_UINT64:
    case VPU::CounterInfo::ValueType::VALUE_TYPE_FLOAT64:
        return sizeof(uint64_t);
    case VPU::CounterInfo::ValueType::VALUE_TYPE_BOOL8:
        return sizeof(uint8_t);
    default:
        return 0u;
    };
}

ze_result_t Metric::getProperties(zet_metric_properties_t *pProperties) {
    if (pProperties == nullptr) {
        LOG_E("Metric properties pointer passed is NULL.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    *pProperties = properties;

    LOG_I("Metric properties retrieved successfully.");
    return ZE_RESULT_SUCCESS;
}

MetricGroup::MetricGroup(zet_metric_group_properties_t &propertiesInput,
                         size_t allocationSizeInput,
                         std::vector<std::shared_ptr<Metric>> &metricsInput,
                         uint32_t groupIndexInput)
    : properties(propertiesInput)
    , allocationSize(allocationSizeInput)
    , metrics(metricsInput)
    , groupIndex(groupIndexInput) {}

ze_result_t MetricGroup::getProperties(zet_metric_group_properties_t *pProperties) {
    if (pProperties == nullptr) {
        LOG_E("MetricGroup properties pointer passed is NULL.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    *pProperties = properties;

    LOG_I("MetricGroup properties retrieved successfully.");
    return ZE_RESULT_SUCCESS;
}

ze_result_t MetricGroup::getMetric(uint32_t *pCount, zet_metric_handle_t *phMetrics) {
    if (pCount == nullptr) {
        LOG_E("Invalid pCount pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (*pCount == 0) {
        *pCount = boost::numeric_cast<uint32_t>(metrics.size());
        return ZE_RESULT_SUCCESS;
    } else if (*pCount > metrics.size()) {
        *pCount = boost::numeric_cast<uint32_t>(metrics.size());
    }

    if (phMetrics != nullptr) {
        for (uint32_t i = 0; i < *pCount; i++) {
            phMetrics[i] = metrics[i]->toHandle();
        }
    } else {
        LOG_I("Input metric handle pointer is NULL.");
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t MetricGroup::calculateMetricValues(zet_metric_group_calculation_type_t type,
                                               size_t rawDataSize,
                                               const uint8_t *pRawData,
                                               uint32_t *pMetricValueCount,
                                               zet_typed_value_t *pMetricValues) {
    if (pRawData == nullptr) {
        LOG_E("Invalid pRawData pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (pMetricValueCount == nullptr) {
        LOG_E("Invalid pMetricValueCount pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (type > ZET_METRIC_GROUP_CALCULATION_TYPE_MAX_METRIC_VALUES) {
        LOG_E("Invalid metric group calculation type.");
        return ZE_RESULT_ERROR_INVALID_ENUMERATION;
    }

    uint32_t metricSize = boost::numeric_cast<uint32_t>(metrics.size());
    if (*pMetricValueCount == 0) {
        *pMetricValueCount = metricSize;
        return ZE_RESULT_SUCCESS;
    } else if (*pMetricValueCount > metricSize) {
        *pMetricValueCount = metricSize;
    }

    if (pMetricValues != nullptr) {
        if (type == ZET_METRIC_GROUP_CALCULATION_TYPE_MAX_METRIC_VALUES) {
            if (*pMetricValueCount != metrics.size()) {
                LOG_E("Invalid pMetricValueCount.");
                return ZE_RESULT_ERROR_INVALID_SIZE;
            }
            calculateMaxMetricValues(pRawData, pMetricValueCount, pMetricValues);
        } else {
            calculateMetricValues(pRawData, pMetricValueCount, pMetricValues);
        }
    } else {
        LOG_I("Input pMetricValues pointer is NULL.");
    }

    return ZE_RESULT_SUCCESS;
}

void MetricGroup::calculateMetricValues(const uint8_t *pRawData,
                                        uint32_t *pMetricValueCount,
                                        zet_typed_value_t *pMetricValues) {
    for (uint32_t i = 0; i < *pMetricValueCount; i++) {
        zet_metric_properties_t properties;
        metrics[i]->getProperties(&properties);
        pMetricValues[i].type = properties.resultType;
        auto rawData = const_cast<uint8_t *>(pRawData);

        switch (properties.resultType) {
        case ZET_VALUE_TYPE_UINT32:
            pMetricValues[i].value.ui32 = *reinterpret_cast<uint32_t *>(rawData);
            break;
        case ZET_VALUE_TYPE_UINT64:
            pMetricValues[i].value.ui64 = *reinterpret_cast<uint64_t *>(rawData);
            break;
        case ZET_VALUE_TYPE_FLOAT32:
            pMetricValues[i].value.fp32 = *reinterpret_cast<float *>(rawData);
            break;
        case ZET_VALUE_TYPE_FLOAT64:
            pMetricValues[i].value.fp64 = *reinterpret_cast<double *>(rawData);
            break;
        case ZET_VALUE_TYPE_BOOL8:
            pMetricValues[i].value.b8 = *reinterpret_cast<uint8_t *>(rawData);
            break;
        default:
            break;
        }

        pRawData += allocationSize / metrics.size();
    }
}

void MetricGroup::calculateMaxMetricValues(const uint8_t *pRawData,
                                           uint32_t *pMetricValueCount,
                                           zet_typed_value_t *pMetricValues) {
    std::vector<zet_typed_value_t> metricValues(*pMetricValueCount);
    calculateMetricValues(pRawData, pMetricValueCount, metricValues.data());

    for (uint8_t i = 0; i < *pMetricValueCount; i++) {
        zet_metric_properties_t properties;
        metrics[i]->getProperties(&properties);
        pMetricValues[i].type = properties.resultType;

        switch (properties.resultType) {
        case ZET_VALUE_TYPE_UINT32:
            pMetricValues[i].value.ui32 =
                std::max(pMetricValues[i].value.ui32, metricValues[i].value.ui32);
            break;
        case ZET_VALUE_TYPE_UINT64:
            pMetricValues[i].value.ui64 =
                std::max(pMetricValues[i].value.ui64, metricValues[i].value.ui64);
            break;
        case ZET_VALUE_TYPE_FLOAT32:
            pMetricValues[i].value.fp32 =
                std::max(pMetricValues[i].value.fp32, metricValues[i].value.fp32);
            break;
        case ZET_VALUE_TYPE_FLOAT64:
            pMetricValues[i].value.fp64 =
                std::max(pMetricValues[i].value.fp64, metricValues[i].value.fp64);
            break;
        case ZET_VALUE_TYPE_BOOL8:
            pMetricValues[i].value.b8 =
                std::max(pMetricValues[i].value.b8, metricValues[i].value.b8);
            break;
        default:
            break;
        }
    }
}

void MetricContext::deactivateMetricGroups(const uint32_t contextId) {
    activatedMetricGroups.erase(std::remove_if(activatedMetricGroups.begin(),
                                               activatedMetricGroups.end(),
                                               [&contextId](auto &x) {
                                                   if (x.second.second == contextId) {
                                                       x.second.first->setActivationStatus(false);
                                                       return true;
                                                   }
                                                   return false;
                                               }),
                                activatedMetricGroups.end());
    LOG_I("All Metric Groups activated by context %u have been deactivated!", contextId);
}

bool MetricContext::activateMetricGroup(const uint32_t contextId,
                                        const zet_metric_group_handle_t hMetricGroup) {
    auto metricGroup = MetricGroup::fromHandle(hMetricGroup);

    // Check metric group is available on device metricGroups list
    if (!device->isMetricGroupAvailable(metricGroup)) {
        LOG_E("Metric Group input (%p) is not associated with device (%p)", metricGroup, device);
        return false;
    }

    // Check if metric group is being held (was activated) by another context
    if (metricGroup->isActivated()) {
        LOG_E("MetricGroup is currently activated by another context.");
        return false;
    }

    zet_metric_group_properties_t properties = {};
    metricGroup->getProperties(&properties);

    uint32_t domain = properties.domain;
    for (auto const &domainIt : activatedMetricGroups) {
        // Checks if domain already exists in activatedMetricGroups map
        if (domainIt.first == domain) {
            // Checks if the metric group passed in was the one already activated
            if (domainIt.second.first == metricGroup && domainIt.second.second == contextId) {
                LOG_I("Metric Group (%p) already activated beforehand.", metricGroup);
                return true;
            }
            LOG_E("Another Metric Group with the same domain is currently activated! Please select "
                  "Metric Groups from different domains.");
            return false;
        }
    }

    metricGroup->setActivationStatus(true);
    activatedMetricGroups.push_back(std::make_pair(domain, std::make_pair(metricGroup, contextId)));
    LOG_I("Metric Group (%p) from domain (%u) has been activated by context %u!",
          metricGroup,
          domain,
          contextId);
    return true;
}

ze_result_t MetricContext::activateMetricGroups(uint32_t contextId,
                                                uint32_t count,
                                                zet_metric_group_handle_t *phMetricGroups) {
    if (device == nullptr || !device->isMetricsLoaded()) {
        LOG_E("Device is uninitialized.");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    ze_result_t result = ZE_RESULT_SUCCESS;

    if (count > 0) {
        if (phMetricGroups == nullptr) {
            LOG_E("Invalid size option passed in. count: %u when phMetricGroups is NULL", count);
            return ZE_RESULT_ERROR_INVALID_SIZE;
        }

        // Deactivate all metric groups first and re-activate those passed into phMetricGroups
        deactivateMetricGroups(contextId);
        for (uint32_t i = 0; i < count; i++) {
            if (!activateMetricGroup(contextId, phMetricGroups[i])) {
                LOG_E("Invalid Metric Group (%p) was passed in.", phMetricGroups[i]);
                result = ZE_RESULT_ERROR_INVALID_ARGUMENT;
            }
        }
    } else if (phMetricGroups == nullptr) {
        deactivateMetricGroups(contextId);
    }

    return result;
}

} // namespace L0
