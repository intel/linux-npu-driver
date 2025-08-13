/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2021-2025, Intel Corporation.
 */

#ifndef VPU_JSM_JOB_CMD_API_H
#define VPU_JSM_JOB_CMD_API_H

/**
 * @addtogroup Jsm
 * @{
 */

/**
 * @file
 * @brief JSM common job command definitions
 */

/*
 * Major version changes that break backward compatibility.
 * Major version must start from 1 and can only be incremented.
 * Minor version must be reset to 0 when Major version is incremented.
 */
#define VPU_JSM_JOB_CMD_API_VER_MAJOR 4

/*
 * Minor version changes when API backward compatibility is preserved.
 * Resets to 0 if Major version is incremented.
 */
#define VPU_JSM_JOB_CMD_API_VER_MINOR 14

/*
 * API header changed (field names, documentation, formatting) but API itself has not been changed
 */
#define VPU_JSM_JOB_CMD_API_VER_PATCH 0

/*
 * Index in the API version table
 * Must be unique for each API
 */
#define VPU_JSM_JOB_CMD_API_VER_INDEX 3

/* Macro to encode API version into uint32_t value */
#define VPU_API_VERSION(major, minor) ((major << 16) | (minor))

/*
 * Pack the API structures to enforce binary compatibility
 * Align to 8 bytes for optimal performance
 */
#pragma pack(push, 8)

/** Maximum number of descriptors in a copy command. */
#define VPU_CMD_COPY_DESC_COUNT_MAX 4096

/** Context Save Area size, in bytes. */
#define VPU_CONTEXT_SAVE_AREA_SIZE 64

/** Timestamp value size, in bytes. */
#define VPU_TIMESTAMP_SIZE 8

/** Fence value size, in bytes. */
#define VPU_FENCE_SIZE 8

/**
 * @brief List of commands supported by the VPU
 *
 * We need to document each command extensively here
 */
enum vpu_cmd_type {
    VPU_CMD_UNKNOWN = 0x0000,

    /** Currently supported commands. */
    VPU_CMD_NOP = 0x0001,
    VPU_CMD_TIMESTAMP = 0x0100,
    /**
     * @brief Fence wait command
     *
     * This command is used for host - NPU synchronization.
     *
     * When the NPU encounters this command in a job, it stops processing
     * subsequent commands in the job until the fence is signaled, i.e., a
     * specific value (specified in the command) is written to the fence
     * address.
     *
     * @see @ref vpu_cmd_fence for details on the command structure.
     */
    VPU_CMD_FENCE_WAIT = 0x0101,
    /**
     * @brief Fence signal command
     *
     * This command is used for host - NPU synchronization.
     *
     * When the NPU encounters this command in a job, it waits until all the
     * previous commands in the job have been completed and then signals the
     * fence by writing a specific value (specified in the command) to the
     * fence address.
     *
     * @see @ref vpu_cmd_fence for details on the command structure.
     */
    VPU_CMD_FENCE_SIGNAL = 0x0102,
    /**
     * @brief Barrier command
     *
     * This command is used to ensure that all previous commands in the job have
     * been completed before proceeding with the next command in the job.
     *
     * @see @ref vpu_cmd_barrier for details on the command structure.
     */
    VPU_CMD_BARRIER = 0x0103,
    VPU_CMD_METRIC_QUERY_BEGIN = 0x0104,
    VPU_CMD_METRIC_QUERY_END = 0x0105,
    /**
     * @brief Memory fill command
     *
     * This command is used to fill a memory region with a specific pattern.
     *
     * @see @ref vpu_cmd_memory_fill for details on the command structure.
     */
    VPU_CMD_MEMORY_FILL = 0x0202,
    VPU_CMD_COPY_LOCAL_TO_LOCAL = 0x0302,
    VPU_CMD_COPY = 0x0302,
    /**
     * @brief Inference Execute command
     *
     * This command is used to execute an inference on the NPU.
     *
     * @see @ref vpu_cmd_inference_execute for details on the command structure.
     */
    VPU_CMD_INFERENCE_EXECUTE = 0x0306,

    /** Deprecated commands. Do not reuse IDs */
    VPU_CMD_COPY_SYSTEM_TO_LOCAL = 0x0200,
    VPU_CMD_COPY_LOCAL_TO_SYSTEM = 0x0201,
    VPU_CMD_COPY_SYSTEM_TO_SYSTEM = 0x0203,
    VPU_CMD_DXIL_DEPRECATED = 0x0300,
    VPU_CMD_JIT_MAPPED_INFERENCE_EXECUTE_DEPRECATED = 0x0301,
    VPU_CMD_CLEAR_BUFFER_DEPRECATED = 0x0303,
    VPU_CMD_OV_BLOB_INITIALIZE_DEPRECATED = 0x0304,
    VPU_CMD_OV_BLOB_EXECUTE_DEPRECATED = 0x0305,
    VPU_CMD_DXIL_COPY_DEPRECATED = 0x0307
};

enum vpu_desc_table_entry_type {
    VPU_DESC_TABLE_ENTRY_TYPE_UNKNOWN = 0x000,

    /** Blob Initialize */
    VPU_DESC_TABLE_ENTRY_TYPE_SCRATCH = 0x100,
    VPU_DESC_TABLE_ENTRY_TYPE_METADATA = 0x101,
    VPU_DESC_TABLE_ENTRY_TYPE_WEIGHTS = 0x102,
    VPU_DESC_TABLE_ENTRY_TYPE_KERNEL_DATA = 0x103,

    /** Blob Execute */
    VPU_DESC_TABLE_ENTRY_TYPE_INPUT = 0x200,
    VPU_DESC_TABLE_ENTRY_TYPE_OUTPUT = 0x201,
    VPU_DESC_TABLE_ENTRY_TYPE_PROFILING_OUTPUT = 0x202,
};

/**
 * @brief VPU timestamp types.
 * Used to select the type of data returned by the command VPU_CMD_TIMESTAMP.
 *
 * @see vpu_cmd_timestamp
 */
enum vpu_time_type {
    /* PerfFRC raw timestamp. */
    VPU_TIME_RAW = 0,
    /* SysTime timestamp (including SysTime delta). */
    VPU_TIME_SYSTIME = 1,
    /* SysTime delta. */
    VPU_TIME_DELTA = 2
};

/**
 * @brief Resource descriptor
 */
typedef struct vpu_cmd_resource_descriptor {
    uint64_t address;    /**< Resource address */
    uint32_t width;      /**< Resource width */
    uint32_t reserved_0; /**< Reserved */
} vpu_cmd_resource_descriptor_t;

/**
 * @brief Resource View Descriptor
 * This is used by the UMD to describe both shader
 * resource and unordered access views.
 *
 * @see vpu_cmd_resource_view_descriptor_t
 */
typedef struct vpu_cmd_resource_view_descriptor {
    uint64_t address;             /**< Resource view address */
    uint64_t width;               /**< Resource view width */
    uint64_t uav_counter_address; /**< UAV counter address */
    uint64_t reserved_0[5];       /**< Unused, reserved for future */
} vpu_cmd_resource_view_descriptor_t;

/**
 * @brief Copy command descriptor on VPU 37xx
 *
 * NOTE: Due to the presence of optional fields
 * unused in copy commands context, this copy
 * descriptor is 64B in size but DMA HW will in
 * practice read 80B of data each time it fetches
 * a given descriptor. The extra 16B are discarded
 * as soon as the DMA HW understands the optional
 * fields are unused so this does not prevent
 * allocating contiguous 64B descriptors. But this
 * means that UMD must ensure there is always an
 * extra 16B of memory accessible to the DMA HW
 * immediately after the memory allocated for any
 * descriptor.
 *
 * @see VPU_CMD_COPY
 */
typedef struct vpu_cmd_copy_descriptor_37xx {
    uint64_t reserved_0[2]; /**< Unused */
    uint64_t src_address;   /**< Source virtual address, should be < 256 GB */
    uint64_t dst_address;   /**< Destination virtual address, should be < 256 GB */
    uint32_t size;          /**< Copy Size in bytes - max 16 MB */
    uint32_t reserved_1[7]; /**< Unused */
} vpu_cmd_copy_descriptor_37xx_t;

/**
 * @brief Copy command descriptor on VPU 40xx or later
 *
 * @see VPU_CMD_COPY
 */
typedef struct vpu_cmd_copy_descriptor_40xx {
    uint64_t reserved_0[3];  /**< Unused */
    uint32_t size;           /**< Copy Size in bytes */
    uint32_t reserved_1;     /**< Unused */
    uint64_t reserved_2;     /**< Unused */
    uint64_t src_address;    /**< Source virtual address */
    uint64_t dst_address;    /**< Destination address */
    uint64_t reserved_3[17]; /**< Unused */
} vpu_cmd_copy_descriptor_40xx_t;

/**
 * @brief Command buffer header
 * Defines size of all commands in the command buffer and location
 * of the heap buffers referenced in the command buffer.
 *
 * NOTE: All instances of this object must be aligned on a 64B boundary
 * to allow proper handling of VPU cache operations.
 */
typedef struct vpu_cmd_buffer_header {
    /** Size of the command buffer */
    uint32_t cmd_buffer_size;
    /** Offset to the first command in the command buffer from start of buffer header */
    uint32_t cmd_offset;
    /**
     * Version of the API header used by the host driver, usually this will be:
     * header->api_version = VPU_JSM_JOB_CMD_API_VER_MAJOR << 16 | VPU_JSM_JOB_CMD_API_VER_MINOR
     * The FW can use this value to interpret command buffer format as used by the host driver.
     */
    uint32_t api_version;
    /* Reserved for future use, must be initialized zero if not used by the driver */
    uint32_t reserved_0;
    /** Pointer to descriptor heap base address */
    uint64_t descriptor_heap_base_address;
    /**
     * Batch buffer submission timestamp taken by UMD from SoC's global system clock, in microseconds.
     * NPU can convert this value to its own fixed clock's timebase, to match other profiling timestamps.
     */
    uint64_t submission_timestamp;
    /** Pointer to fence heap base address */
    uint64_t fence_heap_base_address;
    /**
     * Pointer to VPU-managed context save area
     * NOTE: Address must be aligned on a 64B boundary to allow proper handling of
     * VPU cache operations.
     */
    uint64_t context_save_area_address;
} vpu_cmd_buffer_header_t;

/**
 * Command header, shared by all commands.
 */
typedef struct vpu_cmd_header {
    /**
     * @brief Command type.
     *
     * Possible values defined by @ref vpu_cmd_type.
     */
    uint16_t type;
    /** Size of the command in bytes, including the header */
    uint16_t size;
} vpu_cmd_header_t;

/**
 * @brief Copy command format
 *
 * @see VPU_CMD_COPY
 */
typedef struct vpu_cmd_copy_buffer {
    vpu_cmd_header_t header;
    /**< Reserved */
    uint32_t reserved_0;
    /**
     * @brief Offset in the descriptor heap where the array of copy descriptors start
     * @see vpu_cmd_copy_descriptor_37xx_t
     * @see vpu_cmd_copy_descriptor_40xx_t
     * @see vpu_cmd_buffer_header_t.descriptor_heap_base_address
     * NOTE: Resulting address (heap base plus offset) must be aligned on a 64B boundary
     * to allow proper handling of VPU cache operations.
     */
    uint64_t desc_start_offset;
    /** Number of descriptors in the desc_start_offset header */
    uint32_t desc_count;
    /**< Reserved */
    uint32_t reserved_1;
} vpu_cmd_copy_buffer_t;

/**
 * @brief Memory fill command format
 *
 * @see @ref VPU_CMD_MEMORY_FILL
 */
typedef struct vpu_cmd_memory_fill {
    /** Common command header */
    vpu_cmd_header_t header;
    /** Reserved */
    uint32_t reserved_0;
    /**
     * Start address to fill, should be in NPU DDR.
     * NOTE:
     * - (NPU 37xx) - Address must be aligned on a 64B boundary to allow proper handling of
     *   NPU cache operations.
     * - (NPU 40xx+) - With DMA implementation, there are no alignment requirements.
     */
    uint64_t start_address;
    /** Size in bytes of memory buffer to fill */
    uint64_t size;
    /** Pattern to fill */
    uint32_t fill_pattern;
    /** Reserved */
    uint32_t reserved_1;
} vpu_cmd_memory_fill_t;

/**
 * @brief Inference Execute command format
 *
 * Format of the @ref VPU_CMD_INFERENCE_EXECUTE command.
 */
typedef struct vpu_cmd_inference_execute {
    /** Common command header */
    vpu_cmd_header_t header;
    /** Reserved */
    uint32_t reserved_0;
    /** Unique identifier for the host mapped inference */
    uint64_t inference_id;
    /** Virtual address and size of the host mapped inference */
    vpu_cmd_resource_descriptor_t host_mapped_inference;
} vpu_cmd_inference_execute_t;

/**
 * @brief Format of the timestamp command
 * @see VPU_CMD_TIMESTAMP
 */
typedef struct vpu_cmd_timestamp {
    vpu_cmd_header_t header;
    /** @see enum vpu_time_type. */
    uint32_t type;
    /**
     * Timestamp address
     * NOTE: (VPU 37xx) - Address must be aligned on a 64B boundary to allow proper handling of
     * VPU cache operations.
     * (VPU 40xx) - Address must be aligned on a 8B boundary as RISC-V facilitates cache-bypass,
     * memory access.
     */
    uint64_t timestamp_address;
} vpu_cmd_timestamp_t;

/**
 * @brief Fence Wait and Fence Signal command format
 *
 * @see VPU_CMD_FENCE_WAIT
 * @see VPU_CMD_FENCE_SIGNAL
 */
typedef struct vpu_cmd_fence {
    /** Common command header */
    vpu_cmd_header_t header;
    /** Reserved */
    uint32_t reserved_0;
    /**
     * Offset from the base of the fence heap for the current fence value
     *
     * @note
     * - (VPU 37xx) - Resulting address (heap base plus offset) must be aligned
     *   on a 64B boundary to allow proper handling of NPU cache operations.
     * - (VPU 40xx) - Resulting address (heap base plus offset) must be aligned
     *   on an 8B boundary as LNL+ facilitates cache-bypass, memory access.
     *
     * @see vpu_cmd_buffer_header_t.fence_heap_base_address
     */
    uint64_t offset;
    /**
     * @brief Fence value to wait for or signal.
     *
     * - For @ref VPU_CMD_FENCE_WAIT, this is the value to wait for.
     * - For @ref VPU_CMD_FENCE_SIGNAL, this is the value to write to the fence
     *   address.
     */
    uint64_t value;
} vpu_cmd_fence_t;

/**
 * @brief Barier command structure
 *
 * @see VPU_CMD_BARRIER
 */
typedef struct vpu_cmd_barrier {
    /** Common command header */
    vpu_cmd_header_t header;
    /** Reserved */
    uint32_t reserved_0;
} vpu_cmd_barrier_t;

/**
 * @brief Metric command structure
 * @see VPU_CMD_METRIC_QUERY_BEGIN
 * @see VPU_CMD_METRIC_QUERY_END
 */
typedef struct vpu_cmd_metric_query {
    vpu_cmd_header_t header;
    /** Metric-enabling bitmask, @see bit indexes in vpu_metric_group_bit enum */
    uint32_t metric_group_type;
    /**
     * Array of pointers to buffers storing raw metric data.
     * See hostAddressArrayMemoryLayout_t in vpu_profiling_api.h
     * NOTES:
     *   - Address must be aligned on a 64B boundary to allow proper handling of
     *     VPU cache operations.
     *   - VPU uses a combination of this address and host SSID to build a globally
     *     unique query ID, in order to track concurrent metric queries. Host should
     *     thus ensure that this address is unique per host SSID and per open query.
     */
    uint64_t metric_data_address;
} vpu_cmd_metric_query_t;

#pragma pack(pop)

/**
 * @}
 */

#endif
