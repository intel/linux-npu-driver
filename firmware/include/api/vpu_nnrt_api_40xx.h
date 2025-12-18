/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2025, Intel Corporation.
 */

#ifndef VPU_NNRT_API_40XX_H
#define VPU_NNRT_API_40XX_H

/**
 * @file
 * @brief header file containing the VpuHostParsedInference and the structs required for
 * non workload management (non-WLM) inferences.
 */

/**
 * @addtogroup NNRT
 * @{
 */

#include "vpu_nce_hw_40xx.h"
#include "vpu_dma_hw.h"
#include "vpu_media_hw.h"
#include "vpu_pwrmgr_api.h"
#include "vpu_nnrt_wlm.h"
#include "vpu_nnrt_common.h"

/*
 * When a change is made to vpu_nnrt_api_40xx.h that breaks backwards compatibility
 * (old blob does not work with new firmware) VPU_NNRT_40XX_API_VER_MAJOR must be incremented.
 *
 * If a change preserves backwards compatibility but breaks forwards compatibility
 * (new blob does not work with old firmware) then VPU_NNRT_40XX_API_VER_MINOR
 * should be incremented. It resets to 0 when the major version is incremented.
 *
 * If vpu_nnrt_api_40xx.h is modified (field names, documentation, formatting) but the API
 * itself is not changed, then VPU_NNRT_40XX_API_VER_PATCH should be incremented.
 *
 * When the compiler creates a MappedInference in an ELF blob
 * VpuMappedInference.vpu_nnrt_api_ver is set to the version of the NNRT API used.
 * NNRuntime checks this version at inference time to ensure it is current and
 * returns an error if the major version does not match.
 * Note: VPU_NNRT_40XX_API_VER_PATCH is not stored in the MappedInference as
 * compatibility is not affected if this changes.
 *
 * API changelog
 * -------------
 * 11.13.1:
 *   - Replaced stack frame storage in VpuNNShaveRuntimeConfigs with a union,
 *     providing both a VpuTaskReference<uint32_t> stack_frames_ref for NPU7+ and a fixed-size array for NPU4-6.
 *
 * 11.13:
 *   - Accept CMX Shave stack frames from the blob
 * 11.12:
 *   - 1KB Shave scratch region is moved to the end of CMX,
 *     default CMX Shave stacks increased to 7.5kB per Shave
 * 11.11:
 *   - Add the ManagedMappedInference directly to the VpuHostParsedInference.
 *
 * 11.10.3:
 *   - update Field name from reserved_1 to noc_clk_en in DPU Descriptor.
 *
 * 11.10:
 *   - Increase the minor version number to uniquely identify the UD24 release from earlier versions
 *
 * 11.9:
 *   - Added VpuManagedMappedInference::model_identifier to enable the compiler to assign a unique identifier
 *     to an inference.
 *
 * 11.8.1:
 *   - Added VpuActKernelInvocation::invo_index to track the ID of enqueued ActShave workloads.
 *
 * 11.8
 *   - Added support for shave sub_unit selection for work items to allow compiler to specify
 *     (if it wants to) which shave is to execute a work item.
 *
 * 11.7:
 *   - Added VpuManagedMappedInference::inference_feature_cfg to allow the passing
 *     of additional information.
 *     Added disable_dma_sw_fifo_ to allow skipping the use of DMA SW FIFO
 *     when it is not required by an inference.
 *
 * 11.6:
 *   - Added VpuWorkItem::next_workitem_idx to allow a linked list of work items to be enqueued.
 *
 * 11.5:
 *   - Added barrier configuration data (barriers_configuration, num_of_barrier_reprogrammings,
 *     barrier_programming_mode and barrier_configuration_stride in VpuManagedMappedInference)
 *     to allow runtime to efficiently fill barrier FIFOs.
 */

#define VPU_NNRT_40XX_API_VER_MAJOR 11
#define VPU_NNRT_40XX_API_VER_MINOR 13

#define VPU_NNRT_40XX_API_VER_PATCH 1
#define VPU_NNRT_40XX_API_VER ((VPU_NNRT_40XX_API_VER_MAJOR << 16) | VPU_NNRT_40XX_API_VER_MINOR)

/* Index in the API version table, same for all HW generations */
#define VPU_NNRT_40XX_API_VER_INDEX 7

/*
 * When a change is made to the Activation Shave Runtime / Mangement kernel
 * (nnActEntry.cpp), that breaks backwards compatibility (e.g. changing the
 * nnActEntry function parameters) VPU_ACT_RT_VER_MAJOR must be incremented.
 *
 * If a change preserves backwards compatibility then VPU_ACT_RT_VER_MINOR
 * should be incremented. It resets to 0 when the major version is incremented.
 *
 * Act Runtime changelog:
 * ----------------------
 *  * 1.15:
 *  - Add cleanup function to clear specific registers at the beginning of shave entry
 *
 * 1.14:
 *  - Preemption handling fixes
 *
 * 1.13:
 *  - Window 1F reset to the beginning of CMX tile
 *
 * 1.12:
 *  - Cache operation fix
 *
 * 1.11:
 *  - Improve compatibility
 *
 * 1.10:
 *   - Support for executing shave tasks directly from DDR (expects two FIFO pushes
 *     with the full 32 bit AKI address and NW_PAGE is already correct)
 *
 * 1.9.2 (NPU4 only):
 *  - Preemption handling fixes
 *
 * 1.9.1 (NPU4 only):
 *  - Window 1F reset to the beginning of CMX tile
 *
 * 1.9:
 *   - Add NVL clock gating support
 *
 * 1.8:
 *   - Support Shave Shutdown control message
 *
 */

#if !defined(CONFIG_TARGET_SOC_5000)
#define VPU_ACT_RT_VER_MAJOR 1
#define VPU_ACT_RT_VER_MINOR 9
#define VPU_ACT_RT_VER_PATCH 3
#else
#define VPU_ACT_RT_VER_MAJOR 1
#define VPU_ACT_RT_VER_MINOR 15
#define VPU_ACT_RT_VER_PATCH 0
#endif

#define VPU_ACT_RT_VER ((VPU_ACT_RT_VER_MAJOR << 16) | VPU_ACT_RT_VER_MINOR)

/*
 * IMPORTANT:
 *
 * In order to guarantee that layout of structures will be the same
 * on different compilers and platforms the following means are used:
 *
 * 1. pack(1) enabled to disallow compiler to add any padding.
 *    Padding has been added manually to align every member to its
 *    natural alignment.
 * 2. Required alignment for struct as a whole is set explicitly -
 *    structures are aligned to the biggest alignment of all its members.
 *
 * In case of changing any of the structures - make sure that the data alignment is kept:
 *
 * 1. Struct alignment should be at least the size of the widest struct member.
 * 2. Struct size should be a multiple of its alignment, use padding as necessary at the end of the struct.
 * 3. The offset of each struct member should be a multiple of it's natural alignment, e.g. the offset of a uint64_t
 *    member should be a multiple of 8.
 */

#if defined(_MSC_VER)
#define VPU_ALIGNED_STRUCT(alignment) __declspec(align(alignment))
#elif defined(__GNUC__) || defined(__clang__)
#define VPU_ALIGNED_STRUCT(alignment) __attribute__((aligned(alignment)))
#else
#error Define alignment macro
#endif

namespace nn_public {

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

/* structs after this point are used by workload management inferences and should be included
   in the documentation. */

/** @endcond */

/**
 * @brief The struct passed to the firmware to run the inference.
 */
struct VPU_ALIGNED_STRUCT(64) VpuHostParsedInference {
    uint64_t reserved_;
    VpuResourceRequirements resource_requirements_;

    /**
     * @brief Determines whether the access to the VpuManagedMappedInference is direct or indirect.
     */
    enum VpuMmiAccessMode : uint8_t {
        INDIRECT = 0, /**< The managed inference is accessed indirectly, through the managed_inference member of the
                         VpuMappedInference struct. */
        DIRECT,       /**< The managed inference is accessed directly from the managed_inference_ member of the
                         VpuHostParsedInference  struct. */
        UNKNOWN = 255
    };

    VpuMmiAccessMode mmi_access_;
    uint8_t pad_[3];
    struct VpuPerformanceMetrics performance_metrics_;
    union VPU_ALIGNED_STRUCT(8) {
        VpuTaskReference<VpuMappedInference> mapped_;
        VpuTaskReference<VpuManagedMappedInference> managed_inference_;
    };
};

static_assert(sizeof(VpuHostParsedInference) == 384, "VpuHostParsedInference size != 384");
static_assert(offsetof(VpuHostParsedInference, resource_requirements_) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuHostParsedInference, performance_metrics_) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuHostParsedInference, mapped_) % 8 == 0, "Alignment error");

#pragma pack(pop)

} // namespace nn_public

/**
 * close the "addtogroup NNRT" block
 * @}
 */

#endif
