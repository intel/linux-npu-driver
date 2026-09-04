/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2026, Intel Corporation.
 */

/**
 * @file
 * @brief Shave runtime structures. Shared between WLM and non-WLM APIs.
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
         * @brief NPU4-5 platforms: Direct array of stack frame pointers.
         * The stack frame pointers are stored directly in the struct, not as a reference.
         */
        uint32_t stack_frames[12];
    };
    /**
     * @brief The size of the stacks in bytes.
     */
    uint32_t stack_size;
    uint32_t deprecated1; /**< @deprecated Do not reuse. Previously `code_window_buffer_size`. */
    /**
     * @brief Bitmask of performance metrics to be collected.
     */
    uint32_t perf_metrics_mask;
    /**
     * @brief The version of the runtime embedded in this blob.
     */
    uint32_t runtime_version;
    uint8_t use_schedule_embedded_rt; /* Do not remove, used by LNL PV firmware. */
    uint8_t deprecated2;              /**< @deprecated Do not reuse. Previously `dpu_perf_mode`. */
    uint8_t pad1_[6];
};

static_assert(offsetof(VpuNNShaveRuntimeConfigs, stack_frames_ref) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuNNShaveRuntimeConfigs, stack_frames) % 8 == 0, "Alignment error");
static_assert(sizeof(VpuNNShaveRuntimeConfigs) == 96, "VpuNNShaveRuntimeConfigs size != 96");

typedef void(actKernelEntryFunction)(void *);

enum class VpuActWLType : uint8_t {
    WL_KERNEL = 0x00,
    WL_DXIL_KERNEL,
    WL_DEBUG = 0x04,
    WL_CACHE_OP_FLUSH,
    WL_CACHE_OP_INVALIDATE,
    WL_CACHE_OP_FLUSHINV,
    WL_CACHE_OP_PREFETCH,
    WL_UNKNOWN
};

struct VPU_ALIGNED_STRUCT(8) VpuActKernelRange {
    VpuActWLType type;
    uint8_t use_ram_barriers;
    uint8_t pad0_[6];
    VpuPtr<actKernelEntryFunction> kernel_entry;
    VpuPtr<void> text_window_base;
    uint32_t code_size;
    uint8_t deprecated_[4]; /* deprecated member, do not reuse until next API major version update */
    uint32_t kernel_invo_count;
    uint8_t pad1_[4];
};

static_assert(sizeof(VpuActKernelRange) == 40, "VpuActKernelRange size != 40");
static_assert(offsetof(VpuActKernelRange, kernel_entry) % 8 == 0, "Alignment error");

struct VPU_ALIGNED_STRUCT(32) VpuActKernelInvocation {
    VpuPtr<VpuActKernelRange> range;
    VpuPtr<void> kernel_args;
    VpuPtr<void> data_window_base;
    VpuPtr<void> perf_packet_out;
    VpuTaskBarrierDependency barriers;
    VpuTaskSchedulingBarrierConfig barriers_sched;
    uint32_t invo_index;
    uint32_t invo_tile;
    uint32_t kernel_range_index;
    uint32_t next_aki_wl_addr;
};

static_assert(sizeof(VpuActKernelInvocation) == 96, "VpuActKernelInvocation size != 96");
static_assert(offsetof(VpuActKernelInvocation, barriers) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuActKernelInvocation, barriers_sched) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuActKernelInvocation, invo_tile) % 4 == 0, "Alignment error");

#pragma pack(pop)

} /* namespace nn_public */

/**
 * close the "addtogroup NNRT" block
 * @}
 */

#endif /* VPU_NNRT_SHAVERT_H */
