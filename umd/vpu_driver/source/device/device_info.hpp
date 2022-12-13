/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <boost/numeric/conversion/cast.hpp>

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

struct DeviceInfo {
    // Note: Will be set by UMD.
    bool initialized = false;

    ////////////////////////////////////////
    // HW specific information
    ////////////////////////////////////////

    // Device ID from PCI configuration.
    uint32_t deviceId = 0u;

    // Device revision from PCI configuration.
    uint32_t deviceRevision = 0u;

    // Sub device ID.
    uint32_t subdeviceId = 0u;

    // Core clock rate.
    uint32_t coreClockRate = 0u;

    // Max mem alloc size.
    uint64_t maxMemAllocSize = 0u;

    // Max hardware contexts.
    uint32_t maxHardwareContexts = 0u;

    // Max command queue priority.
    uint32_t maxCommandQueuePriority = 0u;

    // Number of threads per EU.
    uint32_t numThreadsPerEU = 0u;

    // Physical EU SIMD width.
    uint32_t physicalEUSimdWidth = 0u;

    // Number of EUs
    uint32_t nExecUnits = 0u;

    // Number of sub-slices per slice.
    uint32_t numSubslicesPerSlice = 0u;

    // Platoform Type
    uint32_t platformType = 0u;

    // [Device property flags]
    // Device is integrated with the host.
    bool isIntegrated = false;
    // Device handle used for query represents a sub device
    bool isSubdevice = false;
    // Device supports error correction memory access.
    bool isSupportEcc = false;
    // Device supports on-demand page faulting.
    bool isSupportOnDemandPaging = false;

    ////////////////////////////////////////
    // Device metric information
    ////////////////////////////////////////

    // Metric group and metric properties information.
    std::vector<GroupInfo> groupsInfo = {};

    // Device name.
    char name[256] = "Intel VPU";
};

} // namespace VPU
