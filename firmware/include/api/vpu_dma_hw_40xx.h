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

#ifndef VPU_DMA_HW_40XX_H_
#define VPU_DMA_HW_40XX_H_

/*
 * If changes are made to this file that break backwards compatibility, e.g. adding struct
 * members, please update VPU_NNRT_40XX_API_VER_MAJOR & VPU_NNRT_40XX_API_VER_MINOR in
 * vpu_nnrt_api_40xx.h. This allows the NNRuntime to detect old MappedInferences.
 */

// Engine ID enum
typedef enum {
    DMA_ENGINE_0 = 0, // Engine 0
    DMA_ENGINE_MAX
} DmaEngineId;

// Job status enum
typedef enum {
    DMA_JOB_NOT_INITIALIZED = 1, // Job not yet initialized
    DMA_JOB_INITIALIZED,         // Job initialized, not started
    DMA_JOB_PENDING,             // Job pending
    DMA_JOB_PREEMPTED,           // Job has been preempted
    DMA_JOB_FINISHED,            // Job finished
    DMA_JOB_ABORTED,             // Job has been aborted
    DMA_JOB_TIMEOUT,             // The waiting for the job completion has timed out
    DMA_JOB_BLOCKED,             // Job blocked on barrier dependency that wont be lifted
    DMA_JOB_DISCARDED,           // Job was discarded due to error before it was started
    DMA_JOB_NACK,                // Job refused by the DMA HW
} DmaJobStatus;

// Dma descriptor type enum (1D / MultiD)
typedef enum {
    DMA_DESCTYPE_1D = 0, // DMA descriptor type 1D
    DMA_DESCTYPE_MULTID, // DMA descriptor type > 1D
    DMA_DESCTYPE_MAX
} DmaDescType;

// Number of dimensions for the transfer
typedef enum {
    DMA_1D = 0, // 1D transfer
    DMA_2D,     // 2D transfer
    DMA_3D,     // 3D transfer
    DMA_4D,     // 4D transfer
    DMA_5D,     // 5D transfer
    DMA_6D,     // 6D transfer
    DMA_NUM_DIM_MAX
} DmaDimensions;

// Number of dimensions for the dynamic task transfer
typedef enum {
    DMA_DYN_NUM_DIM_DISABLED = 0, // 1D dynamic task transfer
    DMA_DYN_NUM_DIM_2D,           // 2D dynamic task transfer
    DMA_DYN_NUM_DIM_3D,           // 3D dynamic task transfer
    DMA_DYN_NUM_DIM_MAX
} DmaDynamicDimensions;

// Burst Length Encoding
typedef enum {
    DMA_BL_8 = 0, // 8   Data cycles, 512  Data Bytes @ 64 B / cycle
    DMA_BL_16,    // 16  Data cycles, 1024 Data Bytes @ 64 B / cycle
    DMA_BL_24,    // 24  Data cycles, 1536 Data Bytes @ 64 B / cycle
    DMA_BL_32,    // 32  Data cycles, 2048 Data Bytes @ 64 B / cycle
    DMA_BL_40,    // 40  Data cycles, 2560 Data Bytes @ 64 B / cycle
    DMA_BL_48,    // 48  Data cycles, 3072 Data Bytes @ 64 B / cycle
    DMA_BL_56,    // 56  Data cycles, 3584 Data Bytes @ 64 B / cycle
    DMA_BL_64,    // 64  Data cycles, 4096 Data Bytes @ 64 B / cycle
    DMA_BL_72,    // 72  Data cycles, 4608 Data Bytes @ 64 B / cycle
    DMA_BL_80,    // 80  Data cycles, 5120 Data Bytes @ 64 B / cycle
    DMA_BL_88,    // 88  Data cycles, 5632 Data Bytes @ 64 B / cycle
    DMA_BL_96,    // 96  Data cycles, 6144 Data Bytes @ 64 B / cycle
    DMA_BL_104,   // 104 Data cycles, 6656 Data Bytes @ 64 B / cycle
    DMA_BL_112,   // 112 Data cycles, 7168 Data Bytes @ 64 B / cycle
    DMA_BL_120,   // 120 Data cycles, 7680 Data Bytes @ 64 B / cycle
    DMA_BL_128    // 128 Data cycles, 8192 Data Bytes @ 64 B / cycle
    // No max needed. All possible values are legit
} DmaBurstLengthEncoding;

// Source/Destination List Configuration
typedef enum {
    DMA_LIST_DISABLED = 0, // Disabled
    DMA_LIST_ABS_INDEX,    // Enabled, absolute index addressing
    DMA_LIST_REL_INDEX,    // Enabled, relative index addressing
    DMA_LIST_MAX
} DmaListCfg;

// Data format Conversion Configuration
typedef enum {
    DMA_DATA_CONV_DISABLED = 0, // Disabled
    DMA_DATA_CONV_INT8_INT4,    // Enabled, INT8 -> INT4
    DMA_DATA_CONV_FP16_FP8,     // Enabled, FP16 -> FP8
    DMA_DATA_CONV_FP32_FP16,    // Enabled, FP32 -> FP16
    DMA_DATA_CONV_FP32_BF16,    // Enabled, FP32 -> BF16
    DMA_DATA_CONV_MAX
} DmaDataConversionCfg;

// Acceleration Modules Configuration
typedef enum {
    DMA_ACCEL_DISABLED = 0, // Disabled
    DMA_ACCEL_COMPRESS,     // Compression
    DMA_ACCEL_DECOMPRESS,   // Decompression
    DMA_ACCEL_WEIGHT_PROC,  // Weight processing
    // All possible values are legit. Adding MAX for the Event tracing
    DMA_ACCEL_MAX,
} DmaAccelCfg;

// Tile4 Layout Configuration
typedef enum {
    DMA_TILE4_DISABLED = 0, // Disabled
    DMA_TILE4_READ,         // Read Tile4
    DMA_TILE4_WRITE         // Write Tile4
    // No max needed. All possible values are legit
} DmaTile4Cfg;

// AXI User Bits Configuration
typedef enum {
    DMA_AUB_DISABLED = 0, // Disabled
    DMA_AUB_SRC,          // Source enabled
    DMA_AUB_DST,          // Destination enabled
    DMA_AUB_SRC_DST       // Source and Destination enabled
    // No max needed. All possible values are legit
} DmaAxiUserBitsCfg;

// DMA accelerator dtype configuration
typedef enum {
    DMA_ACC_DTYPE_INT8_UINT8 = 0, // INT8 / UINT8
    DMA_ACC_DTYPE_FP16_BF16,      // FP16 / BF16
    DMA_ACC_DTYPE_INT4_UINT4,     // INT4 / UINT4 (Illegal for Compressor/Decompressor)
    DMA_ACC_DTYPE_MAX
} DmaAccDTypeCfg;

// DMA WPREP sparse configuration
typedef enum {
    DMA_ACC_WPREP_DENSE_WITH_C_PADDING = 0,
    DMA_ACC_WPREP_SPARSE_UNPACKED,
    DMA_ACC_WPREP_DENSE_WITHOUT_C_PADDING,
    DMA_ACC_WPREP_SPARSE_PACKED
    // No max needed. All possible values are legit
} DmaWprepSparseCfg;

// Dma preemption Link Agent status enum
typedef enum {
    // The link agent has not reached the halted status yet. If the LA status is this when the timeout has been reached,
    // it probably means that it is blocked by a barrier on the current descriptor.
    DMA_PREEMPT_LA_STATUS_STILL_RUNNING = 0,
    // The link agent has been freed before the preemption. (It can be because the job ended before the request of
    // preemption entered into effect). So the LA is by all means free and can be used for pushing another job.
    DMA_PREEMPT_LA_STATUS_FREE,
    // LA is halted.
    DMA_PREEMPT_LA_STATUS_HALTED
} DmaPreemptionLAStatus;

typedef enum {
    DMA_LA_STATE_HALT,
    DMA_LA_STATE_RESUME,
    DMA_LA_STATE_ABORT,
    DMA_LA_STATE_ABORT_REMOVE,
    DMA_LA_STATE_MAX,
} DmaLACommands;

// Job modes enum
typedef enum {
    DMA_MODE_NORMAL,    // Normal mode
    DMA_MODE_REAL_TIME, // Real Time mode
    DMA_MODE_MAX,
} DmaJobMode;

#pragma pack(push, 1)

#define DMA_L2CACHE_ALIGNMENT (32) // Descriptors must be 32-byte aligned

#if defined(__GNUC__) || defined(__clang__)
#define ALIGN_DMA(x) __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#define ALIGN_DMA(x) __declspec(align(x))
#else
#error "Unknown compiler; can't define ALIGN_DMA"
#endif

// Barrier configuration struct
typedef struct {
    uint64_t prod_mask_lower; // 64-bit mask depicting which producer barriers are affected by
                              // task completion (least significant 64-bit word)
    uint64_t cons_mask_lower; // 64-bit mask depicting which consumer barriers are gating the current
                              // Link Agent (least significant 64-bit word)
    uint64_t prod_mask_upper; // 64-bit mask depicting which producer barriers are affected by
                              // task completion (most significant 64-bit word)
    uint64_t cons_mask_upper; // 64-bit mask depicting which consumer barriers are gating the current
                              // Link Agent (most significant 64-bit word)
} DmaBarrierCfg;

// DMA accelerator parameters
typedef union {
    struct {
        uint64_t dtype : 2;             // Number of dimensions enabled on descriptor
        uint64_t reserved1 : 1;         // Reserved
        uint64_t sparse : 1;            // 0: Dense, 1: Sparse.
        uint64_t bitc_en : 1;           // 0: Bypass BTC compression, 1: Enable BTC compression
        uint64_t z : 10;                // Multiple of 16, Max 8K
        uint64_t bitmap_buf_sz : 19;    // Sized to allow bitmap for 4MB activation
        uint64_t reserved2 : 3;         // Reserved
        uint64_t bitmap_base_addr : 27; // Always in CMX and does not cross tiles. Full virtual address support for
                                        // upto 4MB tile. This field must be 512 byte aligned
    } compress;
    struct {
        uint64_t dtype : 2;             // Number of dimensions enabled on descriptor
        uint64_t reserved1 : 1;         // Reserved
        uint64_t sparse : 1;            // 0: Dense, 1: Sparse.
        uint64_t bitc_en : 1;           // 0: Bypass BTC compression, 1: Enable BTC compression
        uint64_t z : 10;                // Multiple of 16, Max 8K
        uint64_t bitmap_buf_sz : 19;    // Size of the bitmap buffer in bytes
        uint64_t reserved2 : 3;         // Reserved
        uint64_t bitmap_base_addr : 27; // Always in CMX and does not cross tiles. Full virtual address support for
                                        // upto 4MB tile. This field must be 512 byte aligned
    } decompress;
    struct {
        uint64_t dtype : 2;     // Number of dimensions enabled on descriptor
        uint64_t reserved1 : 1; // Reserved
        uint64_t sparse : 2;
        uint64_t zeropoint : 8;         // Zero point for the layer, in case the layer is asymetrically quantized
        uint64_t ic : 14;               // For Conv1x1, ConvNxN, MatMul, this field indicates the the Input Channel
                                        // dimension size. For Depthwise this indicates the DC. 0 based number
        uint64_t filtersize : 7;        // Fx * Fy, support for up to 11x11 (121) filter sizes. 0 based number
        uint64_t reserved2 : 3;         // Reserved
        uint64_t bitmap_base_addr : 27; // Always in CMX and does not cross tiles. Full virtual address support for
                                        // upto 4MB tile. This field must be 512 byte aligned
    } w_prep;
    uint64_t full; // Full config word
} DmaAccInfo;

// Descriptor configuration fields struct
typedef struct {
    uint64_t num_dim : 3;           // Number of dimensions enabled on descriptor
    uint64_t int_en : 1;            // Interrupt enable
    uint64_t int_id : 8;            // Interrupt ID [0,127] for physical, [128,255] for virtual
    uint64_t src_burst_length : 4;  // Number of consecutive accesses requests
                                    // towards CMX or NoC (via the AXI interface)
                                    // for which the DMA-channel owns the bus to read
                                    // the source
    uint64_t dst_burst_length : 4;  // Number of consecutive accesses requests
                                    // towards CMX or NoC (via the AXI interface)
                                    // for which the DMA-channel owns the bus to write
                                    // the destination
    uint64_t arb_qos : 8;           // Number of arbitration runs a JOB can lose before
                                    // becoming high priority. 0 indicates a high priority
                                    // JOB directly
    uint64_t ord : 1;               // Forces JOB execution in JOB List order. Next JOB on list can only
                                    // execute once previous JOB on list has completed
    uint64_t barrier_en : 1;        // Barrier use enable
    uint64_t memset_en : 1;         // Memory Set Enable. Uses a 32 bit pattern as constant source data
                                    // for writes
    uint64_t atp_en : 1;            // Address Translation Prefetch Enable. Enables generation of TLB
                                    // prefetch requests
    uint64_t watermark_en : 1;      // Job watermark enable
    uint64_t rwf_en : 1;            // Remote Width Fetch Enable
    uint64_t rws_en : 1;            // Remote Width Store Enable
    uint64_t src_list_cfg : 2;      // Source List Configuration. JOB to read data from
                                    // addresses computed using a memory source index list
    uint64_t dst_list_cfg : 2;      // Destination  List Configuration. JOB to read data from
                                    // addresses computed using a memory source index list
    uint64_t conversion_cfg : 3;    // Data Format Conversion Configuration. CDMA to process
                                    // data during transfer
    uint64_t acceleration_cfg : 2;  // Acceleration Modules Configuration. CDMA to process
                                    // data during transfer
    uint64_t tile4_cfg : 2;         // Configuration for Tile4 Layout
    uint64_t axi_user_bits_cfg : 2; // Configuration for AXI User Bits
    uint64_t hwp_id_en : 1;         // Enable use of SW provided ID for HW profiling
    uint64_t hwp_id : 12;           // ID for HW profiling (if feature is set)
    uint64_t reserved1 : 1;
    uint64_t dynamic_task_en : 1;   // Enable Dynamic tasks
    uint64_t ptr_wr_en : 1;         // Enable Descriptor Pointer Write
} DmaConfigFields;

typedef struct ALIGN_DMA(DMA_L2CACHE_ALIGNMENT) {
    union {
        uint64_t link_addr_offsetof; // Used by the compiler to get the offset of the link_address field
        uint64_t watermark : 1;      // Watermark to indicate that the job has completed
        struct {
            uint64_t link_address : 48; // Pointer to the next element in linked list
            uint64_t rsvd1 : 15;        // Reserved
            uint64_t lra : 1;           // Link Relative Address. Base address fetched from LBA_ADDR
        };
    };
    uint32_t lba_addr; // CMX address to the location of the Base Address for Relative Addressing options
    uint8_t src_aub;   // Source AXI user bits
    uint8_t rsvd2;     // Reserved
    uint8_t dst_aub;   // Destination AXI user bits
    uint8_t rsvd3;     // Reserved
    union {
        uint64_t full;          // Full config word
        DmaConfigFields fields; // Config fields (shown above)
    } cfg;
    union {
        uint32_t remote_width_fetch; // Remote width fetch
        struct {
            uint32_t src; // Source width
            uint32_t dst; // Destination width
        } width;
    };
    union {
        DmaAccInfo acc_info; // Acceleration info word
        uint32_t mset_data;  // Memset data for painting
    };
    union {
        uint64_t src_offsetof; // Used by the compiler to get the offset of the src field
        struct {
            uint64_t src : 48;   // Address of the data transfer source (48 bits, byte-aligned)
            uint64_t rsvd4 : 15; // Reserved
            uint64_t sra : 1;    // Source Relative Address. Base address fetched from SBA_ADDR
        };
    };
    union {
        uint64_t dst_offsetof; // Used by the compiler to get the offset of the dst field
        struct {
            uint64_t dst : 48;   // Address of the data transfer destination (48 bits, byte-aligned)
            uint64_t rsvd5 : 15; // Reserved
            uint64_t dra : 1;    // Destination Relative Address. Base address fetched from DBA_ADDR
        };
    };
    uint32_t sba_addr;     // Source CMX address to the Base Address for Relative Addressing
    uint32_t dba_addr;     // Destination CMX address to the Base Address for Relative Addressing
    DmaBarrierCfg barrier; // Barrier mask configurations
    union {
        struct {
            uint32_t src; // Source list size
            uint32_t dst; // Destination List size
        } list_size;
        struct {
            uint32_t src; // Source dimension size 2D
            uint32_t dst; // Destination dimension size 2D
        } dim_size_1;
        struct {
            uint32_t src; // Source dynamic task dimension size 2D
            uint32_t dst; // Destination dynamic task dimension size 2D
        } dyn_addr_1;
    };
    uint32_t stride_src_1; // Source stride 2D
    uint32_t stride_dst_1; // Destination stride 2D
    union {
        struct {
            uint32_t src; // Source list size
            uint32_t dst; // Destination List size
        } list_addr;
        struct {
            uint32_t src; // Source dimension size 3D
            uint32_t dst; // Destination dimension size 3D
        } dim_size_2;
        struct {
            uint32_t src; // Source dynamic task dimension size 3D
            uint32_t dst; // Destination dynamic task dimension size 3D
        } dyn_addr_2;
    };
    uint32_t stride_src_2; // Source stride 3D
    union {
        uint32_t remote_width_store; // Remote width store
        uint32_t stride_dst_2;       // Destination stride 3D
    };
    uint16_t dim_size_src_3;      // Source dimension size 4D
    uint16_t dim_size_src_4;      // Source dimension size 5D
    uint16_t dim_size_dst_3;      // Destination stride 4D
    uint16_t dim_size_dst_4;      // Destination stride 5D
    uint16_t dim_size_src_5;      // Source dimension size 6D
    uint16_t src_dyn_size_cfg : 2; // Dynamic task source dimension configuration
    uint16_t rsvd6 : 14;          // Reserved
    uint16_t dim_size_dst_5;      // Destination stride 6D
    uint16_t dst_dyn_size_cfg : 2; // Dynamic task destination dimension configuration
    uint16_t rsvd7 : 14;          // Reserved
    uint32_t stride_src_3;        // Source stride 4D
    uint32_t stride_dst_3;        // Destination stride 4D
    uint32_t stride_src_4;        // Source stride 5D
    uint32_t stride_dst_4;        // Destination stride 5D
    uint32_t stride_src_5;        // Source stride 6D
    uint32_t stride_dst_5;        // Destination stride 6D
    uint16_t task_dyn_id;         // Dynamic task phase ID
    uint16_t rsvd8;               // Reserved
    uint32_t task_dyn_addr;       // Dynamic Task address
    uint64_t pad[2];              // Padding to make all descriptors 32-Byte aligned
} DmaDescriptor;

static_assert(sizeof(DmaDescriptor) == 192, "DmaDescriptor size != 192");

#pragma pack(pop)
#endif /* ifndef VPU_DMA_HW_40XX_H_ */

// clang-format on
