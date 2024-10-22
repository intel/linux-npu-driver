/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2024, Intel Corporation.
 */

#ifndef VPU_PWRMGR_API_H
#define VPU_PWRMGR_API_H

#if defined(_MSC_VER)
#define VPU_ALIGNED_STRUCT(alignment) __declspec(align(alignment))
#elif defined(__GNUC__) || defined(__clang__)
#define VPU_ALIGNED_STRUCT(alignment) __attribute__((aligned(alignment)))
#else
#error Define alignment macro
#endif

#pragma pack(push, 1)

#define VPU_SCALABILITY_NUM_OF_FREQ 5
#define VPU_SCALABILITY_VALUES_PER_FREQ 5

struct VPU_ALIGNED_STRUCT(8) VpuPerformanceMetrics {
    uint32_t freq_base; ///< Base of frequency values used in tables (in MHz).
    uint32_t freq_step; ///< Step of frequency for each entry in tables (in MHz).
    uint32_t bw_base;   ///< Base of bandwidth values used in tables (in MB/s).
    uint32_t bw_step;   ///< Step of bandwidth values used in tables (in MB/s).

    /// Inner arrays are for different bandwidth values.
    /// Outer arrays are for different frequency values.
    uint64_t ticks[VPU_SCALABILITY_NUM_OF_FREQ][VPU_SCALABILITY_VALUES_PER_FREQ];    ///< Table of infr. execution time
    float scalability[VPU_SCALABILITY_NUM_OF_FREQ][VPU_SCALABILITY_VALUES_PER_FREQ]; ///< Table of infr. scalability

    float activity_factor; ///< Compiler estimated activity factor for the inference.
};

static_assert(sizeof(struct VpuPerformanceMetrics) == 320, "VpuPerformanceMetrics size != 320");
static_assert(offsetof(struct VpuPerformanceMetrics, ticks) % 8 == 0, "Alignment error");
static_assert(offsetof(struct VpuPerformanceMetrics, scalability) % 4 == 0, "Alignment error");
static_assert(offsetof(struct VpuPerformanceMetrics, activity_factor) % 4 == 0, "Alignment error");

#pragma pack(pop)

#endif
