/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2026, Intel Corporation.
 */

/**
 * @file
 * @brief The loader API. Defines inference entrypoint and resource requirements.
 */

#ifndef VPU_NNRT_API_H
#define VPU_NNRT_API_H

#include "vpu_nnrt_api_ver.h"
#include "vpu_nnrt_common.h"
#include "vpu_pwrmgr_api.h"

namespace nn_public {

#pragma pack(push, 1)

/**
 * @brief Contains the resource requirements for the inference.
 */
struct VPU_ALIGNED_STRUCT(4) VpuResourceRequirements {
    /**
     * @brief Amount of CMX memory required per tile.
     */
    uint32_t nn_slice_length_;
    uint8_t deprecated_[6]; // Deprecated member, do not reuse until next API major version update
    /**
     * @brief Number of tiles.
     */
    uint8_t nn_slice_count_;
    /**
     * @brief Unused.
     */
    uint8_t nn_barriers_;
};

static_assert(sizeof(VpuResourceRequirements) == 12, "VpuResourceRequirements size != 12");

/**
 * @brief The struct passed to the firmware to run the inference.
 */
struct VPU_ALIGNED_STRUCT(64) VpuHostParsedInference {
    uint64_t reserved_;
    VpuResourceRequirements resource_requirements_;

    /**
     * @brief Determines whether mapped_ field points to the VpuMappedInference or VpuManagedMappedInference.
     */
    enum VpuMmiAccessMode : uint8_t {
        INDIRECT = 0, /**< The managed inference is accessed indirectly via VpuMappedInference::managed_inference */
        DIRECT,       /**< The managed inference is accessed directly via mapped_ field */
        UNKNOWN = 255
    };
    VpuMmiAccessMode mmi_access_;
    uint8_t pad_[3];

    struct VpuPerformanceMetrics performance_metrics_;
    /**
     * @brief Points to VpuMappedInference or VpuManagedMappedInference
     * Type is discriminated by mmi_access_ field
     */
    VpuTaskReference<void> mapped_;
};

static_assert(sizeof(VpuHostParsedInference) == 384, "VpuHostParsedInference size != 384");
static_assert(offsetof(VpuHostParsedInference, resource_requirements_) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuHostParsedInference, performance_metrics_) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuHostParsedInference, mapped_) % 8 == 0, "Alignment error");

#pragma pack(pop)

} // namespace nn_public

#endif /* VPU_NNRT_API_H */
