/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2023, Intel Corporation.
 */

// clang-format off

#ifndef VPU_NNRT_API_37XX_H
#define VPU_NNRT_API_37XX_H

#include "vpu_nce_hw_37xx.h"
#include "vpu_dma_hw_37xx.h"

/*
 * When a change is made to vpu_nnrt_api_37xx.h that breaks backwards compatibility
 * VPU_NNRT_37XX_API_VER_MAJOR must be incremented.
 *
 * If a change preserves backwards compatibility then VPU_NNRT_37XX_API_VER_MINOR
 * should be incremented. It resets to 0 when the major version is incremented.
 *
 * If vpu_nnrt_api_37xx.h is modified (field names, documentation, formatting) but the API
 * itself is not changed, then VPU_NNRT_37XX_API_VER_PATCH should be incremented.
 *
 * When the compiler creates a VpuMappedInference in an ELF blob
 * VpuMappedInference.vpu_nnrt_api_ver is set to the version of vpu_nnrt_api used.
 * NNRuntime checks this version at inference time to ensure it is current and
 * returns an error if the major version does not match.
 * Note: VPU_NNRT_37XX_API_VER_PATCH is not stored in the VpuMappedInference as
 * compatibility is not affected if this changes.
 */
#define VPU_NNRT_37XX_API_VER_MAJOR 7
#define VPU_NNRT_37XX_API_VER_MINOR 0
#define VPU_NNRT_37XX_API_VER_PATCH 0
#define VPU_NNRT_37XX_API_VER ((VPU_NNRT_37XX_API_VER_MAJOR << 16) | VPU_NNRT_37XX_API_VER_MINOR)

// Temporarily define VPU_NNRT_API_VER until all consumers of this header are updated
// to use VPU_NNRT_37XX_API_VER
#define VPU_NNRT_API_VER VPU_NNRT_37XX_API_VER

/*
 * Index in the API version table
 */
#define VPU_NNRT_37XX_API_VER_INDEX 7

/*
 * When a change is made to the Activation Shave Runtime / Mangement kernel
 * (nnActEntry.cpp), that breaks backwards compatibility (e.g. changing the
 * nnActEntry function parameters) VPU_ACT_RT_VER_MAJOR must be incremented.
 *
 * If a change preserves backwards compatibility then VPU_ACT_RT_VER_MINOR
 * should be incremented. It resets to 0 when the major version is incremented.
 */
#define VPU_ACT_RT_VER_MAJOR 1
#define VPU_ACT_RT_VER_MINOR 4
#define VPU_ACT_RT_VER_PATCH 0
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

constexpr uint32_t VPU_SCALABILITY_NUM_OF_FREQ = 5;
constexpr uint32_t VPU_SCALABILITY_VALUES_PER_FREQ = 5;

#pragma pack(push, 1)

template <typename T>
struct VPU_ALIGNED_STRUCT(4) VpuPtr {
    uint32_t ptr;

    VpuPtr()
        : ptr(0) {}

    VpuPtr<T> &operator=(T *ptr) {
        this->ptr = reinterpret_cast<uint32_t>(ptr);
        return *this;
    }

    VpuPtr<T> &operator=(uint32_t ptr) {
        this->ptr = ptr;
        return *this;
    }

    operator T *() const { return reinterpret_cast<T *>(ptr); }
    T *operator->() const { return reinterpret_cast<T *>(ptr); }
    explicit operator bool() const { return ptr; }
    explicit operator uintptr_t() const { return ptr; }
};

static_assert(sizeof(VpuPtr<void>) == 4, "VpuPtr size != 4");

template <typename T>
struct VPU_ALIGNED_STRUCT(8) VpuTaskReference {
    // 'address' can point to memory in host user space within the PIOVA aperture.
    // In this case it needs converted to the bridge aperture to make it accessible from
    // LeonRT/LeonNN.
    //
    // Use the methods data(int64_t offset) and at(uint32_t index, int64_t offset) to
    // apply the aperture offset to convert address to the bridge aperture.
    uint64_t reserved1;
    uint64_t reserved2;
    uint64_t reserved3;

    uint64_t address;
    uint64_t count;

    T *data() { return reinterpret_cast<T *>(address); }
    const T *data() const { return reinterpret_cast<T *>(address); }

    T *data(int64_t offset) { return reinterpret_cast<T *>(address + offset); }
    const T *data(int64_t offset) const { return reinterpret_cast<T *>(address + offset); }

    uint32_t size() const { return count; }

    T &at(uint32_t index, int64_t offset = 0) { return (reinterpret_cast<T *>(address + offset))[index]; }
    const T &at(uint32_t index, int64_t offset = 0) const { return (reinterpret_cast<T *>(address + offset))[index]; }

    template <class TD>
    VpuTaskReference &operator=(TD fixedVector) {
        // If fixedVector has a bridge aperture offset remove it to store the address in the
        // PIOVA aperture as the offset can change.
        address = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(fixedVector.data())) - fixedVector.apertureOffset();
        count = static_cast<uint64_t>(fixedVector.size());
        return *this;
    }
};

static_assert(sizeof(VpuTaskReference<uint32_t>) == 40, "VpuTaskReference size != 40");

typedef void(actKernelEntryFunction)(void *);

struct VPU_ALIGNED_STRUCT(4) VpuTaskSchedulingBarrierConfig {
    uint32_t start_after_;
    uint32_t clean_after_;
};

static_assert(sizeof(VpuTaskSchedulingBarrierConfig) == 8, "VpuTaskSchedulingBarrierConfig size != 8");

struct VPU_ALIGNED_STRUCT(8) VpuTaskBarrierDependency {
    uint64_t wait_mask_;
    uint64_t post_mask_;
    uint8_t group_;
    uint8_t mask_;
    uint8_t pad_[6];
};

static_assert(sizeof(VpuTaskBarrierDependency) == 24, "VpuTaskBarrierDependency size != 24");

struct VPU_ALIGNED_STRUCT(4) VpuBarrierCountConfig {
    uint32_t next_same_id_;
    uint16_t producer_count_;
    uint16_t consumer_count_;
    uint8_t real_id_;
    uint8_t pad_[3];
};

static_assert(sizeof(VpuBarrierCountConfig) == 12, "VpuBarrierCountConfig size != 12");

struct VPU_ALIGNED_STRUCT(8) VpuDPUInvariant {
    VpuDPUInvariantRegisters registers_;
    int32_t hwp_cmx_base_offset_;
    VpuTaskBarrierDependency barriers_;
    VpuTaskSchedulingBarrierConfig barriers_sched_;
    uint16_t variant_count_;
    uint8_t cluster_;
    uint8_t is_cont_conv_;
    VpuHWPStatMode dpu_prof_mode_;
    uint8_t pad_[3];
};

static_assert(sizeof(VpuDPUInvariant) == 304, "DPUInvariant size != 304");
static_assert(offsetof(VpuDPUInvariant, hwp_cmx_base_offset_) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuDPUInvariant, barriers_) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuDPUInvariant, barriers_sched_) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuDPUInvariant, variant_count_) % 2 == 0, "Alignment error");

struct VPU_ALIGNED_STRUCT(4) VpuDPUVariant {
    VpuDPUVariantRegisters registers_;
    VpuPtr<VpuDPUInvariant> invariant_;
    uint32_t invariant_index_;
    uint32_t weight_table_offset_;
    int32_t wload_id_;
    uint8_t cluster_;
    uint8_t pad_[3];
};

static_assert(sizeof(VpuDPUVariant) == 64, "DPUVariant size != 64");
static_assert(offsetof(VpuDPUVariant, invariant_) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuDPUVariant, invariant_index_) % 4 == 0, "Alignment error");

struct VPU_ALIGNED_STRUCT(4) VpuResourceRequirements {
    uint32_t nn_slice_length_;
    uint32_t ddr_scratch_length_;
    uint8_t reserved[2]; // Reserved due to deprecated member.
    uint8_t nn_slice_count_;
    uint8_t nn_barriers_;
};

static_assert(sizeof(VpuResourceRequirements) == 12, "VpuResourceRequirements size != 12");

struct VPU_ALIGNED_STRUCT(4) VpuNNShaveRuntimeConfigs {
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t runtime_entry; // when useScheduleEmbeddedRt = true this is a windowed address
    uint32_t act_rt_window_base;
    uint32_t stack_frames[VPU_AS_TOTAL]; // UNUSED - to be removed
    uint32_t stack_size;                 // UNUSED - to be removed
    uint32_t code_window_buffer_size;
    uint32_t perf_metrics_mask;
    uint32_t runtime_version;
    uint8_t use_schedule_embedded_rt; // when useScheduleEmbeddedRt = false; FW copies ActRt to this buffer
                                      // when useScheduleEmbeddedRt = true; buffer already contains the ActRt
    VpuHWPStatMode dpu_perf_mode;
    uint8_t pad_[2];
};

static_assert(sizeof(VpuNNShaveRuntimeConfigs) == 52, "VpuNNShaveRuntimeConfigs size != 52");

// Forcing struct padding so we have same sizeof() of the structure both on x86 compilation and Sparc
// compilation.
struct VPU_ALIGNED_STRUCT(64) VpuDMATask {
    vpu_dma_descriptor_t transaction_;
    VpuTaskSchedulingBarrierConfig barriers_sched_;
    uint8_t pad_[128 - (sizeof(vpu_dma_descriptor_t) + sizeof(VpuTaskSchedulingBarrierConfig))];
};

static_assert(sizeof(VpuDMATask) == 128, "VpuDMATask size != 128");
static_assert(offsetof(VpuDMATask, barriers_sched_) % 4 == 0, "Alignment error");

struct VPU_ALIGNED_STRUCT(4) VpuActKernelRange {
    VpuActWLType type;
    uint8_t pad_[3];
    VpuPtr<actKernelEntryFunction> kernel_entry;
    VpuPtr<void> text_window_base;
    uint32_t code_size;
    uint32_t data_sec_size;
    uint32_t kernel_invo_count;
};

static_assert(sizeof(VpuActKernelRange) == 24, "VpuActKernelRange size != 24");
static_assert(offsetof(VpuActKernelRange, kernel_entry) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuActKernelRange, text_window_base) % 4 == 0, "Alignment error");

struct VPU_ALIGNED_STRUCT(8) VpuActKernelInvocation {
    VpuPtr<VpuActKernelRange> range;
    VpuPtr<void> kernel_args;
    VpuPtr<void> data_window_base;
    VpuPtr<void> perf_packet_out;
    VpuTaskBarrierDependency barriers;
    VpuTaskSchedulingBarrierConfig barriers_sched;
    // The schedule compiler can infer an index if it's needed pre/post inference
    // Update: we can/will use the index to virtualize a WI FIFO state in a preemption payload
    uint32_t invo_index;
    uint32_t invo_tile;
    uint32_t kernel_range_index;
    uint8_t pad_[4];
};

static_assert(sizeof(VpuActKernelInvocation) == 64, "VpuActKernelInvocation size != 64");
static_assert(offsetof(VpuActKernelInvocation, kernel_args) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuActKernelInvocation, data_window_base) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuActKernelInvocation, perf_packet_out) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuActKernelInvocation, barriers) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuActKernelInvocation, barriers_sched) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuActKernelInvocation, invo_index) % 4 == 0, "Alignment error");

// Plain wrapper struct for vpu_dma_descriptor_t. Defining an array of a structure where the size of the struct is not
// multiple of its alignment is undefined behavior via the C standard. Up until GCC 11 it has been silently handled. In
// vpuip gcc case it actually ingonred alignment for N+1th element and generated code with each vpu_dma_descriptor_t in
// the array 80-bytes distanced. (See
// https://gcc.gnu.org/git/?p=gcc.git;a=commit;h=50bc94898fac1bd9cc1dabf227208fb5d369c4c4) Explicit padding required to
// force size of structure to 128.
struct VPU_ALIGNED_STRUCT(64) VpuDescriptorWrapper {
    vpu_dma_descriptor_t descriptor_;
    uint8_t pad_[128 - sizeof(vpu_dma_descriptor_t)];
};

static_assert(sizeof(VpuDescriptorWrapper) == 128, "DMA descriptor wrapper size != 128");

struct VPU_ALIGNED_STRUCT(4) VpuTaskCounts {
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t dma_count;
    uint32_t dpu_invariant_count;
    uint32_t dpu_variant_count;
    uint32_t act_range_count;
    uint32_t act_invo_count;
};

static_assert(sizeof(VpuTaskCounts) == 28, "VpuTaskCounts size != 28");

struct VPU_ALIGNED_STRUCT(64) VpuMappedInference {
    uint32_t vpu_nnrt_api_ver;
    uint8_t pad0_[4];
    uint64_t reserved0_;
    VpuTaskCounts task_storage_counts_;
    uint32_t task_storage_size_;
    VpuTaskReference<VpuDMATask> dma_tasks[VPU_MAX_DMA_ENGINES];
    VpuTaskReference<VpuDPUInvariant> invariants;
    VpuTaskReference<VpuDPUVariant> variants;
    VpuTaskReference<VpuActKernelRange> act_kernel_ranges;
    VpuTaskReference<VpuActKernelInvocation> act_kernel_invocations;
    VpuTaskReference<VpuBarrierCountConfig> barrier_configs;
    VpuNNShaveRuntimeConfigs shv_rt_configs;
    uint8_t pad1_[4];
    VpuTaskReference<uint8_t> reserved1_; // reserved for workload management
    uint8_t pad2_[24];
};

static_assert(sizeof(VpuMappedInference) == 448, "VpuMappedInference size != 448");
static_assert(offsetof(VpuMappedInference, task_storage_counts_) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, task_storage_size_) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, dma_tasks) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, invariants) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, variants) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, act_kernel_ranges) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, act_kernel_invocations) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, barrier_configs) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, shv_rt_configs) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuMappedInference, reserved1_) % 8 == 0, "Alignment error");

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

static_assert(sizeof(VpuPerformanceMetrics) == 320, "VpuPerformanceMetrics size != 320");

struct VPU_ALIGNED_STRUCT(8) VpuHostParsedInference {
    uint64_t reserved;
    VpuResourceRequirements resource_requirements_;
    uint8_t pad_[4];
    VpuPerformanceMetrics performance_metrics_;
    VpuTaskReference<VpuMappedInference> mapped_;
};

static_assert(sizeof(VpuHostParsedInference) == 384, "VpuHostParsedInference size != 384");
static_assert(offsetof(VpuHostParsedInference, resource_requirements_) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuHostParsedInference, performance_metrics_) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuHostParsedInference, mapped_) % 8 == 0, "Alignment error");

#pragma pack(pop)

} // namespace nn_public

#endif

// clang-format on
