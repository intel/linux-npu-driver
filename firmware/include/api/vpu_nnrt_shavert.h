/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2026, Intel Corporation.
 */

/**
 * @file
 * @brief Shave runtime configuration structures. Shared between WLM and non-WLM APIs.
 */

#ifndef VPU_NNRT_SHAVERT_H
#define VPU_NNRT_SHAVERT_H

#include "vpu_nnrt_common.h"

/**
 * @addtogroup NNRT
 * @{
 */

namespace nn_public {

#pragma pack(push, 1)

/**
 * @brief Contains runtime configuration for the Shaves
 */
struct VPU_ALIGNED_STRUCT(8) VpuNNShaveRuntimeConfigs {
    uint64_t reserved;
    /**
     * @brief The entrypoint address.
     */
    uint64_t runtime_entry;
    /**
     * @brief The window base address.
     */
    uint64_t act_rt_window_base;
    /**
     * @brief The addresses of the stacks (one per Shave).
     * The stack frame pointers specify the memory locations (in DDR or CMX) for each Shave's stack.
     */
    union VPU_ALIGNED_STRUCT(8) {
        /**
         * @brief NPU7+ platforms: Reference to an array of stack frame pointers.
         * The 'ref' member holds the address and count of the stack frame pointers array.
         */
        struct VPU_ALIGNED_STRUCT(8) {
            VpuTaskReference<uint32_t> ref;
            uint8_t pad_[8];
        } stack_frames_ref;

        /**
         * @brief NPU4-6 platforms: Direct array of stack frame pointers.
         * The stack frame pointers are stored directly in the struct, not as a reference.
         */
        uint32_t stack_frames[12];
    };
    /**
     * @brief The size of the stacks in bytes.
     */
    uint32_t stack_size;
    /**
     * @brief Unused
     */
    uint32_t code_window_buffer_size;
    /**
     * @brief Bitmask of performance metrics to be collected.
     */
    uint32_t perf_metrics_mask;
    /**
     * @brief The version of the runtime embedded in this blob.
     */
    uint32_t runtime_version;
    /**
     * @brief Unused
     */
    uint8_t use_schedule_embedded_rt;
    /**
     * @brief Unused
     */
    uint8_t dpu_perf_mode;
    uint8_t pad1_[6];
};

static_assert(offsetof(VpuNNShaveRuntimeConfigs, stack_frames_ref) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuNNShaveRuntimeConfigs, stack_frames) % 8 == 0, "Alignment error");
static_assert(sizeof(VpuNNShaveRuntimeConfigs) == 96, "VpuNNShaveRuntimeConfigs size != 96");

#pragma pack(pop)

} // namespace nn_public

/**
 * close the "addtogroup NNRT" block
 * @}
 */

#endif /* VPU_NNRT_SHAVERT_H */
