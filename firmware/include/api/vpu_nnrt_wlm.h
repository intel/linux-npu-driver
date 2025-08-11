/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2025, Intel Corporation.
 */

/**
 * @file
 * @brief nn_public API containing structs used to pass an inference to the NN Runtime.
 *
 * With Workload Management (WLM), the enqueuing of tasks (DPU, Shave, DMA) into
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
 * When a network is compiled with WLM, the firmware will use the VpuManagedMappedInference
 * defined in this header rather than VpuMappedInference.
 * VpuManagedMappedInference is designed to support both fully and partially managed
 * inferences. The management mode is unambiguous and is detected by the managed player.
 */

#ifndef VPU_NNRT_WLM_H
#define VPU_NNRT_WLM_H

/**
 * @addtogroup NNRT
 * @{
 */

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
 * 3. The offset of each struct member should be a multiple of its natural alignment, e.g., the offset of a uint64_t
 *    member should be a multiple of 8.
 */

#include "vpu_nnrt_common.h"

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
 * @brief Holds a pointer to a task descriptor (in wi_desc_ptr) that can be enqueued to the hardware.
 */
struct VPU_ALIGNED_STRUCT(8) VpuWorkItem {
    enum VpuTaskType : uint8_t { DPU = 0, DMA, SHV, MEDIA, DPU_AUTO, UNKNOWN = 255 };
    enum VpuDMADirection : uint8_t { FROM_DDR = 0, FROM_CMX = 1 };

    /**
     * @brief The work item descriptor reference to be enqueued into an element's work FIFO.
     *
     * If the VpuTaskType is DPU, DMA or SHV the wi_desc_ptr can point to a linked list of tasks.
     *
     * 64b pointer can be in DDR for DMA descriptors.
     * Lower 32b is used for CMX addresses for DPU and SHAVE work items.
     *
     * Constraint:
     * If the underlying VpuManagedMappedInference represents a partially managed inference
     * - and wi_desc_ptr represents a DMA descriptor
     * - and the DMA descriptor represents a descriptor linked list with > 1 DMAs
     * Then:
     * - only the first DMA in the list may consume barriers
     * - only the last DMA in the list may produce barriers
     */
    uint64_t wi_desc_ptr;

    /**
     * @brief Specifies the type of the task to be enqueued, DPU, DPU_AUTO, DMA, SHV etc.
     *
     * The following table describes the meaning of unit and sub_unit for each type:
     *
     * | VpuTaskType | unit               | sub_unit                 |
     * |-------------|--------------------|--------------------------|
     * | DPU         | tile number        | n/a                      |
     * | DMA         | engine/CTRG number | 0: from DDR, 1: from CMX |
     * | SHV         | tile number        | see description below    |
     * | DPU_AUTO    | tile number        | n/a                      |
     *
     *  For Shave tasks a sub_unit of zero indicates that the compiler doesn't care
     *  which shave executes a task. A non-zero value encodes the tile-local ID of the
     *  shave to execute a task as (sub_unit - 1)
     */
    VpuTaskType type;

    /**
     * @brief The hardware unit to which the task is to be enqueued.
     *
     * @see VpuWorkItem::type for the meaning of unit for each task type.
     */
    uint8_t unit;

    /**
     * @brief The hardware sub-unit to which the task is to be enqueued.
     *
     * @see VpuWorkItem::type for the meaning of sub_unit for each task type.
     */
    uint8_t sub_unit;

    uint8_t pad0_[1];

    /**
     * @brief Next workitem in a linked list.
     *
     * Permits a linked list of work items to be marked ready for enqueue.
     *
     * VpuTaskBarrierMap::work_item_idx points to the head.
     * The end of the list is denoted by 0.
     *
     * This is optional. If the first workitem has next_workitem_idx = 0 (no linked list),
     * then enqueue_count consecutive workitems from the work_items array
     * should be marked ready for enqueue.
     */
    uint32_t next_workitem_idx;

    uint8_t pad1_[48];
};

static_assert(sizeof(VpuWorkItem) == 64, "VpuWorkItem size != 64");
static_assert(offsetof(VpuWorkItem, next_workitem_idx) % 4 == 0, "Alignment error");

/**
 * @brief Contains additional information about the work items which
 * is useful when statically parsing the VpuManagedMappedInference.
 *
 * This info is not used by the firmware during normal execution.
 */
struct VPU_ALIGNED_STRUCT(8) VpuTaskInfo {
    /**
     * A duplicate of VpuWorkItem members needed to account for tasks that are not in the
     * `VpuManagedMappedInference::work_items` array.
     */
    uint64_t wi_desc_ptr;

    /**
     * Specifies the type of the task to be enqueued, DPU, DMA, SHV etc.
     */
    VpuWorkItem::VpuTaskType type;

    /**
     * The hardware unit to which the task is to be enqueued.
     *
     * @see VpuWorkItem::type for the meaning of unit for each task type.
     */
    uint8_t unit;

    /**
     * The hardware sub-unit to which the task is to be enqueued.
     *
     * @see VpuWorkItem::type for the meaning of sub_unit for each task type.
     */
    uint8_t sub_unit;

    uint8_t pad_0[1];

    /**
     * If this VpuTaskInfo represents the head of a descriptor linked list, then
     * linked_list_nodes counts the length of that linked list, 0 otherwise.
     *
     * These values are to be iterated into the `VpuManagedMappedInferenceInfo::tasks_ref_info`
     * array starting at this VpuTaskInfo's offset.
     */
    uint32_t linked_list_nodes;

    /**
     * Since most VpuWorkItem::wi_desc_ptr are temporary CMX addresses populated by
     * the schedule, descr_ref_offset serves as a 32b offset into the DDR
     * backing store that contains the descriptors for that work item type.
     *
     * e.g., if VpuWorkItem type == DPU, then descr_ref_offset is added to
     * the variant base address VpuManagedMappedInference::ref_info_base_vars.
     */
    uint32_t descr_ref_offset;

    /**
     * For a given DPU ref offset into `ref_info_base_vars`, this array maps that variant ref to its
     * associated invariant ref in `ref_info_base_invars`. Similarly for SHAVE invocations and ranges.
     */
    uint32_t parent_descr_ref_offset;

    /**
     * This index offset traces this VpuTaskInfo back to the runtime
     * VpuManagedMappedInference::task_configs task that enqueued it.
     */
    uint32_t enqueueing_task_config;

    /**
     * This index offset into VpuManagedMappedInference::work_items links this VpuTaskInfo to the runtime
     * VpuWorkItem that enqueued it. This is useful in case this VpuTaskInfo is an element on a linked list.
     */
    uint32_t work_item_ref;
};

static_assert(sizeof(VpuTaskInfo) == 32, "VpuTaskInfo size != 32");

/**
 * @brief Describes a virtual barrier's physical mapping and configuration
 * as well as all edges of its node in the execution DAG.
 *
 * This info is not used by the firmware during normal execution.
 */
struct VPU_ALIGNED_STRUCT(4) BarrierReferenceMap {
    /**
     * The physical barrier mapped to this virtual barrier.
     */
    uint16_t physical_barrier;

    /**
     * Producer count programmed to `physical_barrier`.
     */
    uint16_t producer_count;

    /**
     * Consumer count programmed to `physical_barrier`.
     */
    uint16_t consumer_count;

    uint8_t pad_0[2];

    /**
     * Used to reference into `VpuManagedMappedInferenceInfo::barrier_producer_ref_offsets`.
     *
     * The index starting at `producers_ref_offset` to `producers_ref_offset + producer_count`
     * indexes all the `VpuTaskInfo`s that produce this virtual barrier.
     */
    uint32_t producers_ref_offset;

    /**
     * Used to reference into `VpuManagedMappedInferenceInfo::barrier_consumer_ref_offsets`.
     *
     * The index starting at `consumers_ref_offset` to `consumers_ref_offset + consumer_count`
     * indexes all the `VpuTaskInfo`s that consume this virtual barrier.
     */
    uint32_t consumers_ref_offset;
};

static_assert(sizeof(BarrierReferenceMap) == 16, "BarrierReferenceMap size != 16");

/**
 * @brief Contains additional information about the VpuManagedMappedInference.
 *
 * This info is not used by the firmware during normal execution.
 */
struct VPU_ALIGNED_STRUCT(8) VpuManagedMappedInferenceInfo {
    /**
     * This is the contiguous metadata memory block for all tasks within VpuManagedMappedInference.
     */
    VpuTaskReference<VpuTaskInfo> tasks_ref_info;

    /**
     * Mapping data for all virtual barriers of the VpuManagedMappedInference indexed by virtual barrier id.
     */
    VpuTaskReference<BarrierReferenceMap> vb_mapping;

    /**
     * A backing store where the elements are indexes into `tasks_ref_info`.
     *
     * Used by `BarrierReferenceMap::producers_ref_offset` and `BarrierReferenceMap::producer_count`
     * to get the list of tasks that produce a specific virtual barrier.
     *
     * Example:
     * @code
     * // Print all tasks that produce into a VB, then use that ref info
     * auto& bm = mmii.vb_mapping[currVB];
     * for(auto i = bm.producers_ref_offset; i < bm.producers_ref_offset + bm.producer_count; i++){
     *     auto refInfoIndex = mmii.barrier_producer_ref_offsets[i];
     *     printf("Virtual barrier %u is produced by tasks_ref_info at %u", currVB, refInfoIndex);
     *     auto& wiRefInfo = mmii.tasks_ref_info[refInfoIndex];
     *     ...
     * }
     * @endcode
     */
    VpuTaskReference<uint32_t> barrier_producer_ref_offsets;

    /**
     * A backing store where the elements are indexes into `tasks_ref_info`.
     *
     * Used by `BarrierReferenceMap::consumers_ref_offset` and `BarrierReferenceMap::consumer_count`
     * to get the list of tasks that consume a specific virtual barrier.
     */
    VpuTaskReference<uint32_t> barrier_consumer_ref_offsets;

    /**
     * Convenience members used for statically parsing the VpuManagedMappedInference.
     *
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
 * @brief Contains barrier programming information.
 *
 * Barrier programming information includes real id, producer and consumer count.
 * Also contains one or more VpuWorkItems that can be marked ready to be enqueued once the barrier is lifted and
 * re-programmed.
 */
struct VPU_ALIGNED_STRUCT(4) VpuTaskBarrierMap {
    /**
     * @brief Index into VpuManagedMappedInference::task_configs where the
     * VpuTaskBarrierMap shares the same real_id as this VpuTaskBarrierMap.
     */
    uint32_t next_same_id;

    /**
     * @brief Number of producers for this barrier.
     */
    uint16_t producer_count;

    /**
     * @brief Number of consumers for this barrier.
     */
    uint16_t consumer_count;

    /**
     * @brief This is the "real" barrier ID vs the virtual barrier ID of the compiler.
     *
     * It is context tile group local and _not_ the physical ID of the hardware.
     */
    uint8_t real_id;

    uint8_t pad0_[3];

    /**
     * @brief An index into VpuManagedMappedInference::work_items.
     */
    uint32_t work_item_idx;

    /**
     * @brief The number of VpuWorkItems to enqueue (starting at work_item_idx) after programming
     * this barrier.
     *
     * If VpuWorkItem::next_workitem_idx is not 0, then the workitems to be enqueued are in a linked
     * list.
     *
     * If enqueue_count is > 1 and the first workitem has VpuWorkItem::next_workitem_idx = 0
     * (no linked list), then enqueue_count consecutive workitems from the work_items array
     * should be enqueued.
     */
    uint32_t enqueue_count;

    uint32_t reserved;
};

static_assert(sizeof(VpuTaskBarrierMap) == 24, "VpuTaskBarrierMap size != 24");

/**
 * @brief Contains information needed to configure the NNRuntime to run the inference.
 *
 * These are not stored directly in VpuManagedMappedInference to keep VpuManagedMappedInference
 * platform independent.
 */
struct VPU_ALIGNED_STRUCT(8) VpuNNRTConfig {
    /**
     * @brief The Shave runtime configuration.
     */
    VpuNNShaveRuntimeConfigs shv_rt_configs;

    /**
     * @brief DMA hardware profiling log address.
     */
    uint64_t logaddr_dma_hwp;
    /**
     * @brief Hardware profiling workpoint config address.
     */
    uint64_t hwp_workpoint_cfg_addr;
};

static_assert(sizeof(VpuNNRTConfig) == 112, "VpuNNRTConfig size != 112");
static_assert(offsetof(VpuNNRTConfig, logaddr_dma_hwp) % 8 == 0, "Alignment error");

/**
 * @brief Contains the information needed to program a barrier.
 */
union VPU_ALIGNED_STRUCT(4) VpuBarrierConfiguration {
    struct {
        uint8_t producerCount;
        uint8_t producerInterruptEnabled;
        uint8_t consumerCount;
        uint8_t consumerInterruptEnabled;
    };
    uint32_t whole;
};
static_assert(sizeof(VpuBarrierConfiguration) == 4, "VpuBarrierConfiguration size != 4");

/**
 * @brief Contains the information needed to run a fully or partially managed inference.
 */
struct VPU_ALIGNED_STRUCT(32) VpuManagedMappedInference {
    /**
     * @brief The NNRuntime API version used in this VpuManagedMappedInference.
     */
    uint32_t vpu_nnrt_api_ver;

    /**
     * @brief The virtual barrier used to indicate inference completion when the producer count reaches zero.
     */
    uint32_t final_barrier;

    /**
     * @brief The collection of work items referenced by VpuManagedMappedInference::task_configs.
     *
     * This is the contiguous metadata memory block backing all tasks within VpuManagedMappedInference.
     * @see VpuWorkItem
     */
    VpuTaskReference<VpuWorkItem> work_items;

    /**
     * @brief The barrier reprogramming and WI enqueue schedule for partially managed inferences indexed by virtual
     * barrier ID.
     *
     * This collection can be empty if the schedule is fully managed.
     */
    VpuTaskReference<VpuTaskBarrierMap> task_configs;

    /**
     * @brief VpuTaskReferences reserved for future use.
     */
    VpuTaskReference<uint32_t> reserved0[4];

    /**
     * @brief barriers_configuration contains the information for barrier programming that can
     * be DMAed directly to the barrier FIFO registers.
     *
     * Layout in memory is: All VpuBarrierConfigurations for physical barrier 0, then all
     * VpuBarrierConfigurations for physical barrier 1 and so on.
     *
     * Needed for initial barrier FIFO setup and for the barrier callback to top up
     * barrier FIFOs during the inference.
     */
    VpuTaskReference<VpuBarrierConfiguration> barriers_configuration;

    /**
     * @brief An array with an entry for each physical barrier that contains the number of
     * reprogrammings for that barrier.
     */
    VpuTaskReference<uint32_t> num_of_barrier_reprogrammings;

    /**
     * @brief virtual barrier ids that index into VpuManagedMappedInference::task_configs.
     * These barriers must be programmed before starting the bootstrap workitems
     * (see bootstrap_workitems_count).
     */
    VpuTaskReference<uint32_t> initial_barriers;

    /**
     * @brief nnrt_config contains information needed to configure the NNRuntime to run the
     * inference.
     */
    VpuTaskReference<VpuNNRTConfig> nnrt_config;

    /**
     * @brief Bitfield indicating that ActShaves are used by the inference.
     *
     * e.g. 00000000b - no ActShaves used, 00001111b - ActShaves on 4 tiles are used.
     */
    uint8_t actshv_used;

    /**
     * @brief Bitfield indicating that DPUs are used by the inference.
     *
     * e.g. 00000000b - no DPUs used, 00001111b - DPUs on 4 tiles are used.
     */
    uint8_t dpu_used;

    /**
     * @brief Bitfield indicating that Media engines are used by the inference.
     *
     * e.g. 00000000b - no DPUs used, 00000001b - one media engine is used.
     */
    uint8_t media_used;

    /**
     * @brief Bitfield indicating that DMA engines transferring from DDR are used by
     * the inference.
     *
     * e.g. 00000011b - two DMA engines are used.
     */
    uint8_t dma_from_ddr_used;

    /**
     * @brief Bitfield indicating that DMA engines transferring from CMX are used by
     * the inference.
     *
     * e.g. 00000011b - two DMA engines are used.
     */
    uint8_t dma_from_cmx_used;

    uint8_t pad0_[2];

    /**
     * @brief Represents the programming modes for the barriers.
     */
    enum VpuBarrierProgrammingMode : uint8_t {
        LEGACY = 0,                     /**< Runtime is responsible for all barrier programming, barriers_configuration
                                             is not populated. */
        NO_BARRIER_DMAS_SCHEDULED,      /**< Compiler has not scheduled any DMAs to program barriers
                                             but has populated barriers_configuration.
                                             The runtime can create DMAs to write to the barrier FIFOs
                                             (initial and top-up). */
        INITIAL_BARRIER_DMAS_SCHEDULED, /**< Compiler has inserted one or more DMAs into the schedule to perform the
                                             initial barrier programming and has populated barriers_configuration.
                                             Runtime is responsible for topping up the barrier FIFOs as the inference
                                             progresses.*/
        ALL_BARRIER_DMAS_SCHEDULED,     /**< Compiler has inserted DMAs into the schedule for all barrier programming,
                                             the runtime should not program any barriers. */
        ALL_BARRIER_DMAS_SCHEDULED_4K,  /**< Compiler has inserted DMAs into the schedule for all barrier programming.
                                             The runtime should not program any barriers. */
        UNKNOWN = 255
    };

    /**
     * @brief Indicates how barriers should be programmed.
     * @see VpuBarrierProgrammingMode
     */
    VpuBarrierProgrammingMode barrier_programming_mode;

    /**
     * @brief The descriptor required to make the VpuManagedMappedInference parsable.
     *
     * This is not used by the firmware during normal execution.
     */
    VpuTaskReference<VpuManagedMappedInferenceInfo> inference_info;

    /**
     * @brief Stride in barrier configuration array between consecutive physical barriers.
     */
    uint32_t barrier_configuration_stride;

    /**
     * @brief Additional information indicating if the inference uses specific features.
     */
    union {
        uint8_t inference_feature_cfg;
        struct {
            uint8_t disable_dma_sw_fifo : 1; /* Disable use of DMA SW FIFO */
            uint8_t reserved : 7;            /* Reserved for future use. */
        } inference_feature_cfg_bf;
    } inference_feature_cfg;

    uint8_t pad1_[3];

    /**
     * @brief Unique identifier for the compiled model, used for debugging and troubleshooting.
     * Helps track and differentiate models during execution.
     */
    uint32_t model_identifier;

    uint8_t pad2_[228];

    /**
     * @brief bootstrap_workitems_count contains the number of work items at the beginning
     * of the VpuManagedMappedInference::work_items list that should be enqueued
     * after the initial barriers are programmed.
     *
     * These will normally contain DMA work items that feed tasks to CMX, but can contain DPU and ActShave workitems.
     * As these complete, barriers will lift resulting in further work items being
     * enqueued by the play loop.
     */
    uint32_t bootstrap_workitems_count;

    uint8_t pad3_[4];
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
static_assert(offsetof(VpuManagedMappedInference, inference_feature_cfg) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuManagedMappedInference, model_identifier) % 4 == 0, "Alignment error");
static_assert(offsetof(VpuManagedMappedInference, bootstrap_workitems_count) % 4 == 0, "Alignment error");

#pragma pack(pop)

} // namespace nn_public

/**
 * close the "addtogroup NNRT" block
 * @}
 */

#endif
