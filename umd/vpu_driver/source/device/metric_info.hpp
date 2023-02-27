/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>
#include <string>
#include <vector>

namespace VPU {
/*
FOR QUERY-BASED AND TIME-BASED PROFILING:
------------------------------------------------------------------------------------
|           Metric Group Index           |                Group mask               |
------------------------------------------------------------------------------------
| VPU_METRIC_GROUP_BIT_L1_INST_CACHE  (0)| 1 << VPU_METRIC_GROUP_BIT_L1_INST_CACHE |
| VPU_METRIC_GROUP_BIT_L1_DATA_CACHE  (1)| 1 << VPU_METRIC_GROUP_BIT_L1_DATA_CACHE |
| VPU_METRIC_GROUP_BIT_L2_CACHE       (2)| 1 << VPU_METRIC_GROUP_BIT_L2_CACHE      |
| VPU_METRIC_GROUP_BIT_DDR            (3)| 1 << VPU_METRIC_GROUP_BIT_DDR           |
| VPU_METRIC_GROUP_BIT_COUNT          (4)|                                         |
------------------------------------------------------------------------------------
[from FW repository]
*/

struct CounterInfo {
    enum MetricType {
        METRIC_TYPE_DURATION,
        METRIC_TYPE_EVENT,
        METRIC_TYPE_EVENT_WITH_RANGE,
        METRIC_TYPE_THROUGHPUT,
        METRIC_TYPE_TIMESTAMP,
        METRIC_TYPE_FLAG,
        METRIC_TYPE_RATIO,
        METRIC_TYPE_RAW,
        METRIC_TYPE_MAX
    };

    enum ValueType {
        VALUE_TYPE_UINT32,
        VALUE_TYPE_UINT64,
        VALUE_TYPE_FLOAT32,
        VALUE_TYPE_FLOAT64,
        VALUE_TYPE_BOOL8,
        VALUE_TYPE_MAX
    };

    uint32_t tier;
    MetricType metricType;
    ValueType valueType;
    std::string metricName;
    std::string component;
    std::string metricDescription;
    std::string units;
};

struct GroupInfo {
    uint32_t groupIndex;
    std::string metricGroupName;
    std::string metricGroupDescription;
    uint32_t domain;
    uint32_t metricCount;
    std::vector<CounterInfo> counterInfo;
};

} // namespace VPU
