/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2026, Intel Corporation.
 */

#ifndef VPU_NNRT_API_40XX_H
#define VPU_NNRT_API_40XX_H

/**
 * @file
 * @brief header file containing the structs required for non workload
 * management (non-WLM) inferences.
 */

/**
 * @addtogroup NNRT
 * @{
 */

#include "vpu_nce_hw_40xx.h"
#include "vpu_dma_hw.h"
#include "vpu_media_hw.h"
#include "vpu_nnrt_wlm.h"
#include "vpu_nnrt_common.h"
#include "vpu_nnrt_api.h" // To be removed. Temporary w/a for loader build.

namespace nn_public {

// base resources
constexpr uint32_t VPU_MAX_TILES = 6;
constexpr uint32_t VPU_BARRIERS_PER_GROUP = 16;
constexpr uint32_t VPU_DPU_PER_TILE = 1;
constexpr uint32_t VPU_SNN_PER_TILE = VPU_DPU_PER_TILE;
constexpr uint32_t VPU_SNN_TOTAL = VPU_SNN_PER_TILE * VPU_MAX_TILES;
constexpr uint32_t VPU_AS_PER_TILE = 2;
// On NPU4-5, there is only one physical DMA engine, but it is logically split into two interfaces.
constexpr uint32_t VPU_MAX_DMA_ENGINES = 2;
constexpr uint32_t VPU_AS_TOTAL = VPU_AS_PER_TILE * VPU_MAX_TILES;

#pragma pack(push, 1)

/* Do not document the legacy non-WLM API structs. */
/** @cond */

template <typename T>
struct VPU_ALIGNED_STRUCT(8) VpuPtr {
    uint64_t ptr;

    VpuPtr()
        : ptr(0) {}

    VpuPtr<T> &operator=(T *ptr) {
        this->ptr = reinterpret_cast<uint64_t>(ptr);
        return *this;
    }

    VpuPtr<T> &operator=(uint64_t ptr) {
        this->ptr = ptr;
        return *this;
    }

    operator T *() const { return reinterpret_cast<T *>(ptr); }
    T *operator->() const { return reinterpret_cast<T *>(ptr); }
    explicit operator bool() const { return ptr; }
    explicit operator uintptr_t() const { return ptr; }
};

static_assert(sizeof(VpuPtr<void>) == 8, "VpuPtr size != 8");

typedef void(actKernelEntryFunction)(void *);

struct VPU_ALIGNED_STRUCT(4) VpuTaskSchedulingBarrierConfig {
    uint32_t start_after_;
    uint32_t clean_after_;
};

static_assert(sizeof(VpuTaskSchedulingBarrierConfig) == 8, "VpuTaskSchedulingBarrierConfig size != 8");

struct VPU_ALIGNED_STRUCT(8) VpuTaskBarrierDependency {
    uint64_t wait_mask_hi_;
    uint64_t wait_mask_lo_;
    uint64_t post_mask_hi_;
    uint64_t post_mask_lo_;
    uint8_t deprecated_[2]; // Deprecated member, do not reuse until next API major version update
    uint8_t pad_[6];
};

static_assert(sizeof(VpuTaskBarrierDependency) == 40, "VpuTaskBarrierDependency size != 40");

struct VPU_ALIGNED_STRUCT(4) VpuBarrierCountConfig {
    uint32_t next_same_id_;
    uint16_t producer_count_;
    uint16_t consumer_count_;
    uint8_t real_id_;
    uint8_t pad_[3];
};

static_assert(sizeof(VpuBarrierCountConfig) == 12, "VpuBarrierCountConfig size != 12");

struct VPU_ALIGNED_STRUCT(32) VpuDPUInvariant {
    VpuDPUInvariantRegisters registers_;
    VpuTaskBarrierDependency barriers_;
    VpuTaskSchedulingBarrierConfig barriers_sched_;
    uint8_t deprecated0_[8]; // Deprecated member, do not reuse until next API major version update
    uint16_t variant_count_;
    uint8_t cluster_;
    uint8_t deprecated1_[2]; // Deprecated member, do not reuse until next API major version update
    uint8_t pad_[3];
};

static_assert(sizeof(VpuDPUInvariant) == 352, "VpuDPUInvariant size != 352");
static_assert(offsetof(VpuDPUInvariant, barriers_) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuDPUInvariant, barriers_sched_) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuDPUInvariant, variant_count_) % 2 == 0, "Alignment error");

struct VPU_ALIGNED_STRUCT(32) VpuDPUVariant {
    VpuDPUVariantRegisters registers_;
    VpuPtr<VpuDPUInvariant> invariant_;
    uint32_t invariant_index_;
    uint8_t deprecated_[13]; // Deprecated member, do not reuse until next API major version update
    uint8_t pad_[7];
};

static_assert(sizeof(VpuDPUVariant) == 224, "VpuDPUVariant size != 224");
static_assert(offsetof(VpuDPUVariant, invariant_) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuDPUVariant, invariant_index_) % 4 == 0, "Alignment error");

struct VPU_ALIGNED_STRUCT(32) VpuDMATask {
    DmaDescriptor transaction_;
    VpuTaskSchedulingBarrierConfig barriers_sched_;
    uint8_t pad_[24];
};

static_assert(sizeof(VpuDMATask) == 224, "VpuDMATask size != 224");
static_assert(offsetof(VpuDMATask, barriers_sched_) % 4 == 0, "Alignment error");

// ActKernel structs
struct VPU_ALIGNED_STRUCT(8) VpuActKernelRange {
    VpuActWLType type;
    uint8_t use_ram_barriers;
    uint8_t pad0_[6];
    VpuPtr<actKernelEntryFunction> kernel_entry;
    VpuPtr<void> text_window_base;
    uint32_t code_size;
    uint8_t deprecated_[4]; // Deprecated member, do not reuse until next API major version update
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

struct VPU_ALIGNED_STRUCT(16) VpuMediaTask {
    union VPU_ALIGNED_STRUCT(16) {
        struct VPU_ALIGNED_STRUCT(16) {
            VpuMediaBuffDescriptor buff_desc_;
            VpuMediaROIDescriptor roi_desc_;
        } standard;
        struct VPU_ALIGNED_STRUCT(16) {
            VpuMediaBuffDescriptor buff_desc_;
            VpuMediaExtendedHeader ext_hdr_;
            VpuMediaROIDescriptor roi_desc_;
        } extended;
    };
    VpuTaskSchedulingBarrierConfig barriers_sched_;
    uint8_t pad0_[8];
};

static_assert(sizeof(VpuMediaTask) == 240, "VpuMediaTask size != 240");
static_assert(offsetof(VpuMediaTask, standard.buff_desc_) % 16 == 0, "Alignment error");
static_assert(offsetof(VpuMediaTask, extended.buff_desc_) % 16 == 0, "Alignment error");
static_assert(offsetof(VpuMediaTask, barriers_sched_) % 4 == 0, "Alignment error");

struct VPU_ALIGNED_STRUCT(4) VpuTaskCounts {
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t dma_ddr_count;
    uint32_t dma_cmx_count;
    uint32_t dpu_invariant_count;
    uint32_t dpu_variant_count;
    uint32_t act_range_count;
    uint32_t act_invo_count;
    uint32_t media_count;
};

static_assert(sizeof(VpuTaskCounts) == 36, "VpuTaskCounts size != 36");

struct VPU_ALIGNED_STRUCT(32) VpuMappedInference {
    uint32_t vpu_nnrt_api_ver;
    uint8_t pad0_[4];
    uint64_t reserved0_;
    uint64_t logaddr_dma_hwp_;
    VpuTaskCounts task_storage_counts_;
    uint32_t task_storage_size_;
    VpuTaskReference<VpuDMATask> dma_tasks_ddr_[VPU_MAX_TILES];
    VpuTaskReference<VpuDMATask> dma_tasks_cmx_[VPU_MAX_TILES];
    VpuTaskReference<VpuDPUInvariant> invariants[VPU_MAX_TILES];
    VpuTaskReference<VpuDPUVariant> variants[VPU_MAX_TILES];
    VpuTaskReference<VpuActKernelRange> act_kernel_ranges[VPU_MAX_TILES];
    VpuTaskReference<VpuActKernelInvocation> act_kernel_invocations[VPU_MAX_TILES];
    VpuTaskReference<VpuMediaTask> media_tasks;
    VpuTaskReference<VpuBarrierCountConfig> barrier_configs;
    VpuNNShaveRuntimeConfigs shv_rt_configs;
    uint64_t hwp_workpoint_cfg_addr;
    VpuTaskReference<VpuManagedMappedInference> managed_inference;
};

static_assert(sizeof(VpuMappedInference) == 1728, "VpuMappedInference size != 1728");
static_assert(offsetof(VpuMappedInference, task_storage_counts_) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, task_storage_size_) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, dma_tasks_ddr_) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, dma_tasks_cmx_) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, invariants) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, variants) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, act_kernel_ranges) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, act_kernel_invocations) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, media_tasks) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, barrier_configs) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, shv_rt_configs) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, hwp_workpoint_cfg_addr) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, managed_inference) % 8 == 0, "Alignment error");

/** @endcond */

#pragma pack(pop)

} // namespace nn_public

/**
 * close the "addtogroup NNRT" block
 * @}
 */

#endif
