/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2023, Intel Corporation.
 */

/**
 * With Workload Management (WLM) the enqueuing of tasks (DPU, Shave, DMA) into
 * FIFOs is performed by management tasks (DMA tasks that write to FIFOs) which are part
 * of the Managed Mapped Inference DAG produced by the compiler.
 *
 * The DMA tasks that feed task descriptors (DPU, Shave) from DDR to CMX are referred
 * to as workload propagation tasks and are also part of the Managed Mapped Inference DAG.
 *
 * The initial version of WLM, called partial WLM, only includes workload propagation tasks,
 * not management tasks. The firmware is still responsible for enqueuing the tasks (DPU, Shave,
 * DMA).
 *
 * When a network is compiled with WLM the firmware will use the VpuManagedMappedInference
 * defined in this header rather than VpuMappedInference.
 * VpuManagedMappedInference is designed to support both fully and partially managed
 * inferences. The management mode is unambiguous and is detected by the managed player.
 */
#ifndef VPU_NNRT_WLM_H
#define VPU_NNRT_WLM_H

#include "vpu_nnrt_common.h"

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

/**
 * VpuWorkItem
 * Holds a pointer to a task descriptor (in wi_desc_ptr) that can be enqueued to the hardware.
 * If the VpuTaskType is DPU or DMA the wi_desc_ptr can point to a linked list of tasks.
 */
struct VPU_ALIGNED_STRUCT(8) VpuWorkItem {
    enum VpuTaskType : uint8_t { DPU = 0, DMA, SHV, MEDIA, UNKNOWN = 255 };
    enum VpuDMADirection : uint8_t { FROM_DDR = 0, FROM_CMX = 1 };

    /**
     * The work item descriptor reference to be enqueued into an element's work FIFO
     * 64b pointer can be in DDR for DMA descriptors
     * lower 32b is used for CMX addresses for DPU and SHAVE work items
     *
     * Constraint:
     * If the underlying VpuManagedMappedInference represents a partially managed inference
     * - and wi_desc_ptr represents a DMA descriptor
     * - and the DMA descriptor represents a descriptor linked list with > 1 DMAs
     *
     * Then:
     * - only the first DMA in the list may consume barriers
     * - only the last DMA in the list may produce barriers
     */
    uint64_t wi_desc_ptr;

    /**
     * ---------------------------------------------------------------
     * | VpuTaskType | unit               | sub_unit                 |
     * |-------------------------------------------------------------|
     * | DPU         | tile number        | n/a                      |
     * | DMA         | engine/CTRG number | 0: from DDR, 1: from CMX |
     * | SHV         | tile number        | n/a                      |
     * ---------------------------------------------------------------
     */
    VpuTaskType type;
    uint8_t unit;
    uint8_t sub_unit;
    uint8_t pad0_[1];

    /**
     * next_workitem_idx permits a linked list of work items to be marked ready
     * for enqueue, VpuTaskBarrierMap::work_item_idx points to the head.
     * The end of the list is denoted by 0.
     *
     * This is optional, if the first workitem has next_workitem_idx = 0 (no linked list)
     * then enqueue_count consecutive workitems from the work_items array
     * should be marked ready for enqueue.
     */
    uint32_t next_workitem_idx;
    uint8_t pad1_[48];
};

static_assert(sizeof(VpuWorkItem) == 64, "VpuWorkItem size != 64");
static_assert(offsetof(VpuWorkItem, next_workitem_idx) % 4 == 0, "Alignment error");

/**
 * VpuTaskInfo contains additional information about the work items which
 * is useful when statically parsing the VpuManagedMappedInference.
 *
 * This info is not used by the firmware during normal execution.
 */
struct VPU_ALIGNED_STRUCT(8) VpuTaskInfo {
    /**
     * A duplicate of VpuWorkItem members needed to account for tasks that are not in the
     * `VpuManagedMappedInference::work_items` array
     */
    uint64_t wi_desc_ptr;
    VpuWorkItem::VpuTaskType type;
    uint8_t unit;
    uint8_t sub_unit;
    uint8_t pad_0[1];

    /**
     * If this VpuTaskInfo represents the head of a descriptor linked list then
     * linked_list_nodes counts the length of that linked list, 0 otherwise.
     *
     * These values are to be iterated into th `VpuManagedMappedInferenceInfo::tasks_ref_info`
     * array starting at this VpuTaskInfo's offset
     */
    uint32_t linked_list_nodes;

    /**
     * Since most VpuWorkItem::wi_desc_ptr are temporary CMX addresses populated by
     * the schedule, descr_ref_offset serves as a 32b offset into the DDR
     * backing store that contains the descriptors for that work item type.
     * e.g. if VpuWorkItem type == DPU then descr_ref_offset is added to
     * the variant base address VpuManagedMappedInference::ref_info_base_vars
     */
    uint32_t descr_ref_offset;

    /**
     * For a given DPU ref offset into `ref_info_base_vars`, this array maps that variant ref to its
     * associate invariant ref in `ref_info_base_invars`. Similalry for SHAVE invocations and ranges.
     */
    uint32_t parent_descr_ref_offset;

    /**
     * This index offset traces this VpuTaskInfo back to the runtime
     * VpuManagedMappedInference::task_configs task that enqueued it.
     */
    uint32_t enqueueing_task_config;

    /**
     * This index offset into VpuManagedMappedInference::work_items to link this VpuTaskInfo to the runtime VpuWorkItem
     * that enqueued it. This is useful in case this VpuTaskInfo an element on a linked list.
     */
    uint32_t work_item_ref;
};

static_assert(sizeof(VpuTaskInfo) == 32, "VpuTaskInfo size != 32");

/**
 * A BarrierReferenceMap is used to describe a virtual barrier's physical mapping and configuration
 * as well as all edges of it's node in the execution DAG
 *
 * This info is not used by the firmware during normal execution.
 */
struct VPU_ALIGNED_STRUCT(4) BarrierReferenceMap {
    /**
     * The physical barrier mapped to this virtual barrier
     */
    uint16_t physical_barrier;

    /**
     * Producer count programmed to `physical_barrier`
     */
    uint16_t producer_count;

    /**
     * Conumer count programmed to `physical_barrier`
     */
    uint16_t consumer_count;
    uint8_t pad_0[2];

    /**
     * Used to reference into `VpuManagedMappedInferenceInfo::barrier_producer_ref_offsets`.
     * The index starting at `producers_ref_offset` to `producers_ref_offset + producer_count`
     * index all the `VpuTaskInfo`s that produce this virtual barrier.
     */
    uint32_t producers_ref_offset;
    /**
     * Used to reference into `VpuManagedMappedInferenceInfo::barrier_consumer_ref_offsets`.
     * The index starting at `consumers_ref_offset` to `consumers_ref_offset + consumer_count`
     * index all the `VpuTaskInfo`s that consume this virtual barrier.
     */
    uint32_t consumers_ref_offset;
};

static_assert(sizeof(BarrierReferenceMap) == 16, "BarrierReferenceMap size != 16");

struct VPU_ALIGNED_STRUCT(8) VpuManagedMappedInferenceInfo {
    /**
     * This is the contiguous metadata memory block for all tasks within VpuManagedMappedInference
     */
    VpuTaskReference<VpuTaskInfo> tasks_ref_info;

    /*
     * Mapping data for all virtual barriers of the VpuManagedMappedInference indexed by virtual barrier id
     */
    VpuTaskReference<BarrierReferenceMap> vb_mapping;

    /*
     * A backing store where the elements are indexes into `tasks_ref_info`.
     * Used by `BarrierReferenceMap::producers_ref_offset` and `BarrierReferenceMap::producer_count`
     * to get the list of tasks that produce a specific virtual barrier.
     *
     * Example:
     *      // Print all tasks that pruduce into a VB, then use that ref info
     *      auto& bm = mmii.vb_mapping[currVB];
     *      for(auto i = bm.producers_ref_offset; i < bm.producers_ref_offset + bm.producer_count; i++){
     *          auto refInfoIndex = mmii.barrier_producer_ref_offsets[i];
     *          printf("Virtual barrier %u is produced by tasks_ref_info at %u", currVB, refInfoIndex);
     *          auto& wiRefInfo = mmii.tasks_ref_info[refInfoIndex];
     *          ...
     *      }
     */
    VpuTaskReference<uint32_t> barrier_producer_ref_offsets;
    /*
     * A backing store where the elements are indexes into `tasks_ref_info`.
     * Used by `BarrierReferenceMap::consumers_ref_offset` and `BarrierReferenceMap::consumer_count`
     * to get the list of tasks that consume a specific virtual barrier.
     */
    VpuTaskReference<uint32_t> barrier_consumer_ref_offsets;
    /*
     * Convenience members used for statically parsing the VpuManagedMappedInference.
     * These are not used by the firmware during normal execution.
     *
     * The base addresses of the task lists in DDR. work_item_reference in
     * VpuTaskInfo is used to offset from these base addresses.
     */
    uint64_t ref_info_base_vars[VPU_MAX_TILES];
    uint64_t ref_info_base_invars[VPU_MAX_TILES];
    uint64_t ref_info_base_akr[VPU_MAX_TILES];
    uint64_t ref_info_base_aki[VPU_MAX_TILES];
    uint64_t ref_info_base_dma_from_ddr[VPU_MAX_DMA_ENGINES];
    uint64_t ref_info_base_dma_from_cmx[VPU_MAX_DMA_ENGINES];
    uint64_t ref_info_base_media;
};

static_assert(sizeof(VpuManagedMappedInferenceInfo) == 392, "BarrierReferenceMap size != 392");
static_assert(offsetof(VpuManagedMappedInferenceInfo, ref_info_base_vars) % 8 == 0, "Alignment error");

/**
 * VpuTaskBarrierMap
 * For a given virtual barrier VpuTaskBarrierMap contains the barrier programming
 * information (real id, producer and consumer count) as well as one or more VpuWorkItems
 * that can be enqueued once the barrier is lifted and re-programmed.
 */
struct VPU_ALIGNED_STRUCT(4) VpuTaskBarrierMap {
    /**
     * next_same_id is an index into VpuManagedMappedInference::task_configs where the
     * VpuTaskBarrierMap shares the same real_id as this VpuTaskBarrierMap
     */
    uint32_t next_same_id;
    uint16_t producer_count;
    uint16_t consumer_count;
    /**
     * This is the "real" barrier ID vs the virtual barrier ID of the compiler.
     * It is context tile group local and _not_ the physical ID of the hardware
     */
    uint8_t real_id;
    uint8_t pad0_[3];
    /**
     * An index into VpuManagedMappedInference::work_items
     */
    uint32_t work_item_idx;
    /**
     * The number of VpuWorkItems to enqueue (starting at work_item_idx) after programming
     * this barrier.
     *
     * If VpuWorkItem::next_workitem_idx is not 0 then the workitems to be enqueued are in a linked
     * list.
     *
     * If enqueue_count is > 1 and the first workitem has VpuWorkItem::next_workitem_idx = 0
     * (no linked list) then enqueue_count consecutive workitems from the work_items array
     * should be enqueued.
     */
    uint32_t enqueue_count;
    uint32_t reserved;
};

static_assert(sizeof(VpuTaskBarrierMap) == 24, "VpuTaskBarrierMap size != 24");

/**
 * VpuNNRTConfig contains information needed to configure the NNRuntime to run the
 * inference. These are not stored directly in VpuManagedMappedInference to keep
 * VpuManagedMappedInference platform independent.
 */
struct VPU_ALIGNED_STRUCT(8) VpuNNRTConfig {
    /*
     * The Shave runtime configuration. This is not currently populated by the compiler,
     * the InferenceRuntime uses the shv_rt_configs in the VpuMappedInference instead.
     * To deprecate VpuMappedInference this shv_rt_configs will need populated and used.
     */
    VpuNNShaveRuntimeConfigs shv_rt_configs;
    /*
     * logaddr_dma_hwp_ and hwp_workpoint_cfg_addr are not currently populated by the
     * compiler, instead the values from VpuMappedInference are used.
     * To deprecate VpuMappedInference these values will need populated and used.
     */
    uint64_t logaddr_dma_hwp;
    uint64_t hwp_workpoint_cfg_addr;
};

static_assert(sizeof(VpuNNRTConfig) == 112, "VpuNNRTConfig size != 112");
static_assert(offsetof(VpuNNRTConfig, logaddr_dma_hwp) % 8 == 0, "Alignment error");

/**
 * VpuBarrierConfiguration contains the information needed to program a barrier.
 */
union VPU_ALIGNED_STRUCT(4) VpuBarrierConfiguration {
    struct {
        uint8_t producerCount;
        uint8_t producerInterruptEnabled;
        uint8_t consumerCount;
        uint8_t consumerInterruptEnabled;
    } ;
    uint32_t whole;
};
static_assert(sizeof(VpuBarrierConfiguration) == 4, "VpuBarrierConfiguration size != 4");

/**
 * VpuManagedMappedInference
 */
struct VPU_ALIGNED_STRUCT(32) VpuManagedMappedInference {
    uint32_t vpu_nnrt_api_ver;
    /**
     * The virtual barrier used to indicate inference completion when the producer count reaches zero
     * For fully managed schedules, this is the only real barrier monitored by the FW
     */
    uint32_t final_barrier;
    /**
     * The collection of work items referenced by VpuManagedMappedInference::task_configs
     * This is the contiguous metadata memory block backing all tasks within VpuManagedMappedInference
     */
    VpuTaskReference<VpuWorkItem> work_items;
    /**
     * The barrier reset and WI enqueue schedule for partially managed inferences indexed by virtual barrier ID.
     * This collection is empty if the schedule is fully managed
     */
    VpuTaskReference<VpuTaskBarrierMap> task_configs;
    /**
     * VpuTaskReferences reserved for future use.
     */
    VpuTaskReference<uint32_t> reserved0[4];
    /**
     * barriers_configuration contains the information for barrier programming that can
     * be DMAed directly to the barrier FIFO registers.
     * Layout in memory is: All VpuBarrierConfigurations for physical barrier 0, then all
     * VpuBarrierConfigurations for physical barrier 1 and so on.
     * Needed for initial barrier FIFO setup and for the barrier callback to top up
     * barrier FIFOs during the inference.
     */
    VpuTaskReference<VpuBarrierConfiguration> barriers_configuration;
    /**
     * num_of_barrier_reprogrammings is an array with an entry for each physical barrier
     * that contains the number of reprogrammings for that barrier.
     */
    VpuTaskReference<uint32_t> num_of_barrier_reprogrammings;
    /**
     * initial_barriers are virtual barrier ids that index into
     * VpuManagedMappedInference::task_configs. These barriers must be programmed
     * before starting the bootstrap workitems (see bootstrap_workitems_count).
     */
    VpuTaskReference<uint32_t> initial_barriers;
    /**
     * nnrt_config contains information needed to configure the NNRuntime to run the
     * inference.
     */
    VpuTaskReference<VpuNNRTConfig> nnrt_config;
    /**
     * The following fields are set if the inference contains tasks of that type.
     * They are bitfields representing which units(s) the task type runs on, for example:
     * dpu_used 00001111b indicates the inference contains dpu tasks for tiles 1-4.
     * actshv_used 00000000b indicates the inference does not contain ActShave tasks.
     * dma_from_ddr_used 00000001b indicates the inference contains DMAs from DDR for
     *                             the first DMA engine (CTRG) only.
     */
    uint8_t actshv_used;
    uint8_t dpu_used;
    uint8_t media_used;
    uint8_t dma_from_ddr_used;
    uint8_t dma_from_cmx_used;
    uint8_t pad0_[2];
    /*
     * VpuBarrierProgrammingMode indicates how barriers should be programmed:
     * LEGACY
     *   Runtime is responsible for all barrier programming, barriers_configuration is not populated.
     * NO_BARRIER_DMAS_SCHEDULED
     *   Compiler has not scheduled any DMAs to program barriers but has populated barriers_configuration.
     *   The runtime can create DMAs to write to the barrier FIFOs (initial and top-up).
     * INITIAL_BARRIER_DMAS_SCHEDULED
     *   Compiler has inserted one or more DMAs into the schedule to perform the initial barrier programming and
     *   has populated barriers_configuration. Runtime is responsible for topping up the barrier FIFOs as the
     *   inference progresses.
     * ALL_BARRIER_DMAS_SCHEDULED
     *   Compiler has inserted DMAs into the schedule for all barrier programming, the runtime should not
     *   program any barriers.
     */
    enum VpuBarrierProgrammingMode : uint8_t {
        LEGACY = 0,
        NO_BARRIER_DMAS_SCHEDULED,
        INITIAL_BARRIER_DMAS_SCHEDULED,
        ALL_BARRIER_DMAS_SCHEDULED,
        UNKNOWN = 255
    };
    VpuBarrierProgrammingMode barrier_programming_mode;
    /**
     * The descriptor required to make the the VpuManagedMappedInference parsable.
     * This is not used by the firmware during normal execution.
     */
    VpuTaskReference<VpuManagedMappedInferenceInfo> inference_info;
    /**
     * Stride in barrier configuration array between consecutive physical barriers.
     */
    uint32_t barrier_configuration_stride;
    /**
     * Additional information for inference processing.
     */
    union {
        uint8_t inference_feature_cfg;
        struct {
            uint8_t disable_dma_sw_fifo : 1;
            uint8_t reserved : 7; // Reserved for future use.
        } inference_feature_cfg_bf;
    } inference_feature_cfg;

    uint8_t pad1_[235];

    /*
     * bootstrap_workitems_count contains the number of work items at the beginning
     * of the VpuManagedMappedInference::work_items list that should be enqueued
     * after the initial barriers are programmed. These will normally contain DMA
     * work items that feed tasks to CMX, but can contain DPU and ActShave workitems.
     * As these complete the barriers will lift resulting in further work items being
     * enqueued by the play loop.
     */
    uint32_t bootstrap_workitems_count;
    uint8_t pad2_[4];
};

static_assert(sizeof(VpuManagedMappedInference) == 704, "VpuManagedMappedInference size != 704");
static_assert(offsetof(VpuManagedMappedInference, work_items) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuManagedMappedInference, task_configs) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuManagedMappedInference, initial_barriers) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuManagedMappedInference, nnrt_config) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuManagedMappedInference, inference_info) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuManagedMappedInference, barriers_configuration) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuManagedMappedInference, num_of_barrier_reprogrammings) % 8 == 0, "Alignment error");
static_assert(offsetof(VpuManagedMappedInference, barrier_configuration_stride) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuManagedMappedInference, bootstrap_workitems_count) % 4 == 0, "Alignment error");

#pragma pack(pop)

} // namespace nn_public

#endif
