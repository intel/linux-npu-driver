/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2023, Intel Corporation.
 */

// clang-format off

/**
 * @brief VPU DMA public header file.
 *
 * @ingroup DMA
 * @brief DMA shared definitions
 * @{
 */

#ifndef VPU_DMA_HW_37XX_H_
#define VPU_DMA_HW_37XX_H_

/*
 * If changes are made to this file that break backwards compatibility, e.g. adding struct
 * members, please update VPU_NNRT_37XX_API_VER_MAJOR & VPU_NNRT_37XX_API_VER_MINOR in
 * vpu_nnrt_api_37xx.h. This allows the NNRuntime to detect old MappedInferences.
 */

/** Pack the API structures for now, once alignment issues are fixed this can be removed */
#pragma pack(push, 1)

#define VPU_CPU_L2CACHE_ALIGNMENT (64)

#if defined(__GNUC__) || defined(__clang__)
#define ALIGN_DMA(x) __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#define ALIGN_DMA(x) __declspec(align(x))
#else
#error "Unknown compiler; can't define ALIGN_DMA"
#endif

/*
 * Bit field for fine-grained configuration of VPU DMA descriptor
 */
struct vpu_dma_config_bits {
    uint64_t type : 2;              /* Job type(1D/2D) */
    uint64_t burst_length : 8;      /* Burst length */
    uint64_t critical : 1;          /* Critical task */
    uint64_t interrupt_en : 1;      /* Interrupt enable */
    uint64_t interrupt_trigger : 7; /* Interrupt status id when task is executed */
    uint64_t skip_nr : 7;           /* Skip descriptor */
    uint64_t
        order_forced : 1; /* Force ordering. Dispatch the current task only after the previous task has completed */
    uint64_t watermark_en : 1; /* Job watermark enable */
    uint64_t dec_en : 1;       /* Decoder enable */
    uint64_t barrier_en : 1;   /* Barrier use enable */
    uint64_t reserved : 34;    /* Reserved */
};
typedef struct vpu_dma_config_bits vpu_dma_config_bits_t;

/*
 * VPU DMA descriptor's barrier configuration structure
 */
struct vpu_dma_barrier_cfg {
    uint64_t prod_mask; /* 64-bit mask depicting which barriers are affected by task completion */
    uint64_t cons_mask; /* 64-bit mask depicting which barriers are gating the current Link Agent */
};
typedef struct vpu_dma_barrier_cfg vpu_dma_barrier_cfg_t;

/*
 * VPU DMA descriptor's 2D attributes structure
 */
struct vpu_dma_2d_attributes {
    uint32_t src_width; /* Bytes of data required from one line of source */
    int32_t src_stride; /* Length in bytes from start of one line of data, to start of next line of data */
    uint32_t dst_width; /* Bytes of data required from one line of destination */
    int32_t dst_stride; /* Length in bytes from start of one line of data, to start of next line of data */
};
typedef struct vpu_dma_2d_attributes vpu_dma_2d_attributes_t;

/*
 * VPU DMA descriptor structure.
 */
struct vpu_dma_descriptor {
    uint64_t link_address : 40; /* pointer to the next element in linked list */
    uint64_t reserved : 23;
    uint64_t watermark : 1; /* watermark to indicate that the job has completed */
    union {
        vpu_dma_config_bits_t cfg_bits;
        uint64_t full_cfg_register;
    } cfg_link;
    uint64_t src;             /* Address of the data transfer source */
    uint64_t dst;             /* Address of the data transfer destination */
    uint32_t length;          /* Job length */
    uint32_t num_planes : 8;  /* Number of planes */
    uint32_t task_id : 24;    /* Task id for the current job */
    int32_t src_plane_stride; /* Source plane stride */
    int32_t dst_plane_stride; /* Destination plane stride */
    union {
        vpu_dma_2d_attributes_t attr2d;   /* Attributes that apply for 2D jobs (i.e. striding) */
        vpu_dma_barrier_cfg_t barriers1d; /* Barrier mask configurations for 1D jobs */
    };
    vpu_dma_barrier_cfg_t barriers; /* Barrier mask configurations for 2D jobs */
    /*
     * The descriptor must be aligned to 64 byte boundary
     * This is needed for L2 cache line alignment
     */
};
typedef struct vpu_dma_descriptor ALIGN_DMA(VPU_CPU_L2CACHE_ALIGNMENT) vpu_dma_descriptor_t;

static_assert(sizeof(vpu_dma_descriptor_t) == 80, "DMA descriptor size != 80");

#pragma pack(pop)
#endif /* ifndef VPU_DMA_HW_37XX_H_ */

/** @} */

// clang-format on
