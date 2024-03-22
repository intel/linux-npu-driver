/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2021-2023, Intel Corporation.
 */

/**
 * @brief JSM common job command definitions
 */

#ifndef VPU_JSM_JOB_CMD_API_H
#define VPU_JSM_JOB_CMD_API_H

/*
 * Major version changes that break backward compatibility.
 * Major version must start from 1 and can only be incremented.
 */
#define VPU_JSM_JOB_CMD_API_VER_MAJOR 4

/*
 * Minor version changes when API backward compatibility is preserved.
 * Resets to 0 if Major version is incremented.
 */
#define VPU_JSM_JOB_CMD_API_VER_MINOR 1

/*
 * API header changed (field names, documentation, formatting) but API itself has not been changed
 */
#define VPU_JSM_JOB_CMD_API_VER_PATCH 4

/*
 * Index in the API version table
 * Must be unique for each API
 */
#define VPU_JSM_JOB_CMD_API_VER_INDEX 3

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

/** Max number of inference variants in VPU_CMD_INFERENCE_EXECUTE command. */
#define VPU_INFERENCE_EXECUTE_ARRAY_SIZE 6U

/** Minimum size of inference execute command, in bytes. */
#define VPU_INFERENCE_EXECUTE_CMD_MIN_SIZE 32U

/**
 * @brief List of commands supported by the VPU
 *
 * We need to document each command extensively here
 */
enum vpu_cmd_type {
    VPU_CMD_UNKNOWN = 0x0000,
    VPU_CMD_NOP = 0x0001,

    /** Implemented by both Copy and Compute Engine */
    VPU_CMD_TIMESTAMP = 0x0100,
    VPU_CMD_FENCE_WAIT = 0x0101,
    VPU_CMD_FENCE_SIGNAL = 0x0102,
    VPU_CMD_BARRIER = 0x0103,
    VPU_CMD_METRIC_QUERY_BEGIN = 0x0104,
    VPU_CMD_METRIC_QUERY_END = 0x0105,

    /** Implemented by Copy Engine */
    VPU_CMD_COPY_SYSTEM_TO_LOCAL = 0x0200,
    VPU_CMD_COPY_LOCAL_TO_SYSTEM = 0x0201,
    VPU_CMD_MEMORY_FILL = 0x0202,
    VPU_CMD_COPY_SYSTEM_TO_SYSTEM = 0x0203,

    /** Implemented by Compute Engine */
    VPU_CMD_DXIL = 0x0300,
    VPU_CMD_JIT_MAPPED_INFERENCE_EXECUTE = 0x0301,
    VPU_CMD_COPY_LOCAL_TO_LOCAL = 0x0302,
    VPU_CMD_CLEAR_BUFFER = 0x0303,
    VPU_CMD_OV_BLOB_INITIALIZE = 0x0304,
    VPU_CMD_OV_BLOB_EXECUTE = 0x0305,
    VPU_CMD_INFERENCE_EXECUTE = 0x0306,
    VPU_CMD_DXIL_COPY = 0x0307
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
 * @brief Resource descriptor
 * @see vpu_cmd_resource_descriptor_table_t
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
 * @brief A header for a table of resource descriptors of
 * given type.
 *
 * The header is followed be a variable number (@see desc_count)
 * of resource descriptors.
 * @see enum vpu_desc_table_entry_type
 * @see vpu_cmd_resource_descriptor_t
 */
typedef struct vpu_cmd_resource_descriptor_table {
    uint16_t type;       /**< enum vpu_desc_table_entry_type */
    uint16_t desc_count; /**< Number of descriptors in the descriptor table entry */
    uint32_t reserved_0; /**< Unused */
} vpu_cmd_resource_descriptor_table_t;

/**
 * @brief Copy command descriptor on VPU 37xx
 * Note VPU 37xx does not have a LOCAL memory
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
 * @see VPU_CMD_COPY_SYSTEM_TO_SYSTEM
 */
typedef struct vpu_cmd_copy_descriptor_37xx {
    uint64_t reserved_0[2]; /**< Unused */
    uint64_t src_address;   /**< Source virtual address, should be < 256 GB */
    uint64_t dst_address;   /**< Destination virtual address, should be < 256 GB */
    uint32_t size;          /**< Copy Size in bytes - max 16 MB */
    uint32_t reserved_1[7]; /**< Unused */
} vpu_cmd_copy_descriptor_37xx_t;
typedef vpu_cmd_copy_descriptor_37xx_t vpu_cmd_copy_descriptor_mtl_t;

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
    /** Pointer to kernel heap base address */
    uint64_t kernel_heap_base_address;
    /** Pointer to descriptor heap base address */
    uint64_t descriptor_heap_base_address;
    /** Unused */
    uint64_t reserved_1;
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
    uint16_t type; /**< @see enum vpu_cmd_type */
    uint16_t size; /**< Size of the command in bytes, including the header */
} vpu_cmd_header_t;

/**
 * @brief Copy command format
 *
 * @see VPU_CMD_COPY_SYSTEM_TO_LOCAL
 * @see VPU_CMD_COPY_LOCAL_TO_SYSTEM
 * @see VPU_CMD_COPY_SYSTEM_TO_SYSTEM
 * @see VPU_CMD_COPY_SYSTEM_TO_SYSTEM
 */
typedef struct vpu_cmd_copy_buffer {
    vpu_cmd_header_t header;
    /**< Reserved */
    uint32_t reserved_0;
    /**
     * @brief Offset in the descriptor heap where the array of copy descriptors start
     * @see vpu_cmd_copy_descriptor_37xx_t
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
 * @brief Momory fill command format
 * @see VPU_CMD_MEMORY_FILL
 */
typedef struct vpu_cmd_memory_fill {
    vpu_cmd_header_t header;
    /** Reserved */
    uint32_t reserved_0;
    /**
     * Start address to fill, should be in VPU DDR
     * NOTE: Address must be aligned on a 64B boundary to allow proper handling of
     * VPU cache operations.
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
 * @brief DXIL workload execution command
 * @see VPU_CMD_DXIL
 */
typedef struct vpu_cmd_dxil {
    vpu_cmd_header_t header;
    uint32_t reserved_0;       /**< Reserved */
    uint64_t kernel_entry;     /**< VA to kernel entry function */
    uint64_t dispatch_data;    /**< VA to buffer containing all the kernels invocation data */
    uint32_t kernel_data_size; /**< Size of kernel data for a single kernel */
    uint32_t shave_count;      /**< Number of shaves or kernel invocations */
    uint64_t reserved_1;       /**< Reserved */
    uint64_t shave_stack[16];  /**< Array of pointers to VA where the stack for each Shave is located */
    uint32_t shave_stack_size; /**< Shave stack size */
    uint32_t reserved_2;       /**< Reserved */
} vpu_cmd_dxil_t;

/**
 * @brief Parse inference from a blob
 * @see VPU_CMD_OV_BLOB_INITIALIZE
 */
typedef struct vpu_cmd_ov_blob_initialize {
    vpu_cmd_header_t header;
    /** Size of the kernel ASM */
    uint32_t kernel_size;
    /**
     * Offset from KernelHeapBaseAddress to kernel blob
     * NOTE: Resulting address (heap base plus offset) must be aligned on a 64B boundary
     * to allow proper handling of VPU cache operations.
     */
    uint64_t kernel_offset;
    /** Size in bytes of the Init descriptor table - scratch */
    uint32_t desc_table_size;
    /** Reserved */
    uint32_t reserved_0;
    /** Offset from the base of the descriptor heap */
    uint64_t desc_table_offset;
    /** Unique Blob Id */
    uint64_t blob_id;
} vpu_cmd_ov_blob_initialize_t;

/**
 * @brief Run inference on a previously initialized blob.
 * @see VPU_CMD_OV_BLOB_EXECUTE
 */
typedef struct vpu_cmd_ov_blob_execute {
    vpu_cmd_header_t header;
    /** Size in bytes of Exec descriptor table - input, output */
    uint32_t desc_table_size;
    /**
     * Offset from the base of the descriptor heap
     * NOTE: Resulting address (heap base plus offset) must be aligned on a 64B boundary
     * to allow proper handling of VPU cache operations.
     */
    uint64_t desc_table_offset;
    /** Unique Blob id */
    uint64_t blob_id;
} vpu_cmd_ov_blob_execute_t;

typedef struct vpu_cmd_inference_entry {
    /** Virtual address and size of the host mapped inference */
    vpu_cmd_resource_descriptor_t host_mapped_inference;
    /** Reserved */
    uint64_t reserved_0;
} vpu_cmd_inference_entry_t;

/**
 * @brief Execute parsed inference
 * @see VPU_CMD_INFERENCE_EXECUTE
 */
typedef struct vpu_cmd_inference_execute {
    vpu_cmd_header_t header;
    /**
     * Number of inferences in the inference array.
     * For backward compatibility reason, VPU behaviour upon processing this command
     * is the following:
     *   - If inference_count == 0
     *     - interpret below union as host_mapped_inference object.
     *   - If inference_count != 0
     *     - interpret below union as inference array, of size defined by
     *       inference_count value.
     */
    uint32_t inference_count;
    /** Unique identifier for the host mapped inference */
    uint64_t inference_id;
    union {
        /** Singleton inference object for backward compatibility */
        vpu_cmd_resource_descriptor_t host_mapped_inference;
        /** Array of inference objects */
        vpu_cmd_inference_entry_t inferences[VPU_INFERENCE_EXECUTE_ARRAY_SIZE];
    };
} vpu_cmd_inference_execute_t;

/**
 * @brief Format of the timestamp command
 * @see VPU_CMD_TIMESTAMP
 */
typedef struct vpu_cmd_timestamp {
    vpu_cmd_header_t header;
    /** Reserved */
    uint32_t reserved_0;
    /**
     * Timestamp address
     * NOTE: (MTL) - Address must be aligned on a 64B boundary to allow proper handling of
     * VPU cache operations.
     */
    uint64_t timestamp_address;
} vpu_cmd_timestamp_t;

/**
 * @brief Fence wait/signal commands format
 * @see VPU_CMD_FENCE_WAIT
 * @see VPU_CMD_FENCE_SIGNAL
 */
typedef struct vpu_cmd_fence {
    vpu_cmd_header_t header;
    /** Reserved */
    uint32_t reserved_0;
    /**
     * Offset from the base of the fence heap for the current fence value
     * NOTE: Resulting address (heap base plus offset) must be aligned on a 64B boundary
     * to allow proper handling of VPU cache operations.
     */
    uint64_t offset;
    /** Fence value to be written */
    uint64_t value;
} vpu_cmd_fence_t;

/**
 * @brief Barier command structure
 * @see VPU_CMD_BARRIER
 */
typedef struct vpu_cmd_barrier {
    vpu_cmd_header_t header;
    uint32_t reserved_0; /**< Reserved */
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

/**
 * @brief Clear buffer command structure
 * @see VPU_CMD_CLEAR_BUFFER
 */
typedef struct vpu_cmd_clear_buffer {
    vpu_cmd_header_t header;
    uint32_t reserved_0;     /**< Reserved */
    uint64_t start_address;  /**< Start address to clear, should be in VPU DDR */
    uint64_t size;           /**< Size in bytes of memory buffer to clear */
    uint32_t clear_value[4]; /**< Clear value, 4 bytes per channel */
} vpu_cmd_clear_buffer_t;

/**
 * @brief Address patch definition used in vpu_cmd_jit_mapped_inference_execute_t
 */
typedef struct vpu_inference_address_patch {
    /** Patch offset in the host parsed inference */
    uint64_t host_parsed_inference_offset;
    /** Offset in the descriptor heap to read the base address for the resource */
    uint64_t descriptor_heap_offest;
    /** Offset within the resource to compute the final address to be patched */
    uint64_t resource_offset;
    /** Address mask for patch. Note: Use lower 4 bytes for 4 bytes address*/
    uint64_t address_mask;
    /** Size of the patch in bytes, e.g., 4 for uint32_t, 8 for uint64_t */
    uint32_t patch_size;
    /** Reserved bytes */
    uint32_t reserved_0;
} vpu_inference_address_patch_t;

/**
 * @brief JIT map and execute parsed inference
 * @see VPU_CMD_JIT_MAPPED_INFERENCE_EXECUTE
 */
typedef struct vpu_cmd_jit_mapped_inference_execute {
    vpu_cmd_header_t header;
    /** Reserved */
    uint32_t reserved_0;
    /** Unique identifier for the host parsed inference */
    uint64_t inference_id;
    /** Virtual address and size of the host parsed inference */
    vpu_cmd_resource_descriptor_t host_parsed_inference;
    /** Virtual address and size of the descriptor heap */
    vpu_cmd_resource_descriptor_t descriptor_heap;
    /** Virtual address and size of the address patch buffer
        Number of patchd can computed by
        address_patch_buffer_size / sizeof(vpu_inference_address_patch_t) */
    vpu_cmd_resource_descriptor_t address_patch_buffer;
} vpu_cmd_jit_mapped_inference_execute_t;

#pragma pack(pop)

#endif
