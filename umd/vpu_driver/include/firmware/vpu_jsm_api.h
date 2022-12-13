/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020-2022 Intel Corporation
 */

/**
 * @file
 * @brief JSM shared definitions
 *
 * @ingroup Jsm
 * @brief JSM shared definitions
 * @{
 */
#ifndef VPU_JSM_API_H
#define VPU_JSM_API_H

/*
 * Major version changes that break backward compatibility
 */
#define VPU_JSM_API_VER_MAJOR 2

/*
 * Minor version changes when API backward compatibility is preserved.
 */
#define VPU_JSM_API_VER_MINOR 10

/*
 * API header changed (field names, documentation, formatting) but API itself has not been changed
 */
#define VPU_JSM_API_VER_PATCH 1

/*
 * Index in the API version table
 */
#define VPU_JSM_API_VER_INDEX 4

/*
 * Number of Priority Bands for Hardware Scheduling
 * Bands: RealTime, Focus, Normal, Idle
 */
#define VPU_HWS_NUM_PRIORITY_BANDS 4

/* Max number of impacted contexts that can be dealt with the engine reset command */
#define VPU_MAX_ENGINE_RESET_IMPACTED_CONTEXTS 3

/** Pack the API structures for now, once alignment issues are fixed this can be removed */
#pragma pack(push, 1)

/*
 * Engine indexes.
 */
#define VPU_ENGINE_COMPUTE 0
#define VPU_ENGINE_COPY 1
#define VPU_ENGINE_NB 2

/*
 * VPU status values.
 */
#define VPU_JSM_STATUS_SUCCESS 0x0U
#define VPU_JSM_STATUS_PARSING_ERR 0x1U
#define VPU_JSM_STATUS_PROCESSING_ERR 0x2U
#define VPU_JSM_STATUS_PREEMPTED 0x3U
#define VPU_JSM_STATUS_ABORTED 0x4U
#define VPU_JSM_STATUS_USER_CTX_VIOL_ERR 0x5U
#define VPU_JSM_STATUS_GLOBAL_CTX_VIOL_ERR 0x6U
#define VPU_JSM_STATUS_MVNCI_WRONG_INPUT_FORMAT 0x7U
#define VPU_JSM_STATUS_MVNCI_UNSUPPORTED_NETWORK_ELEMENT 0x8U
#define VPU_JSM_STATUS_MVNCI_INVALID_HANDLE 0x9U
#define VPU_JSM_STATUS_MVNCI_OUT_OF_RESOURCES 0xAU
#define VPU_JSM_STATUS_MVNCI_NOT_IMPLEMENTED 0xBU
#define VPU_JSM_STATUS_MVNCI_INTERNAL_ERROR 0xCU
/* Job status returned when the job was preempted mid-inference */
#define VPU_JSM_STATUS_PREEMPTED_MID_INFERENCE 0xDU

/*
 * Host <-> VPU IPC channels.
 * ASYNC commands use a high priority channel, other messages use low-priority ones.
 */
#define VPU_IPC_CHAN_ASYNC_CMD 0
#define VPU_IPC_CHAN_GEN_CMD 10
#define VPU_IPC_CHAN_JOB_RET 11

/*
 * Job flags bit masks.
 */
#define VPU_JOB_FLAGS_NULL_SUBMISSION_MASK 0x00000001

/*
 * Sizes of the reserved areas in jobs, in bytes.
 */
#define VPU_JOB_RESERVED_BYTES 16
/*
 * Sizes of the reserved areas in job queues, in bytes.
 */
#define VPU_JOB_QUEUE_RESERVED_BYTES 52

/*
 * Max length (including trailing NULL char) of trace entity name (e.g., the
 * name of a logging destination or a loggable HW component).
 */
#define VPU_TRACE_ENTITY_NAME_MAX_LEN 32

/*
 * Max length (including trailing NULL char) of a dyndbg command.
 *
 * NOTE: 112 is used so that the size of 'struct vpu_ipc_msg' in the JSM API is
 * 128 bytes (multiple of 64 bytes, the cache line size).
 */
#define VPU_DYNDBG_CMD_MAX_LEN 112

/*
 * Job format.
 */
struct vpu_job {
    volatile uint64_t batch_buf_addr;                 /**< Address of VPU commands batch buffer */
    volatile uint32_t job_id;                         /**< Job ID */
    volatile uint32_t flags;                          /**< Flags bit field, see VPU_JOB_FLAGS_* above */
    volatile uint64_t root_page_table_addr;           /**< Address of root page table to use for this job */
    volatile uint64_t root_page_table_update_counter; /**< Page tables update events counter */
    volatile uint64_t preemption_buffer_address;      /**< Address of the preemption buffer to use for this job */
    volatile uint64_t preemption_buffer_size;         /**< Size of the preemption buffer to use for this job */
    uint8_t reserved[VPU_JOB_RESERVED_BYTES];
};
typedef struct vpu_job vpu_job_t;

/*
 * Job queue control registers.
 */
struct vpu_job_queue_header {
    volatile uint32_t engine_idx;
    uint32_t head;
    volatile uint32_t tail;
    uint8_t reserved[VPU_JOB_QUEUE_RESERVED_BYTES];
};
typedef struct vpu_job_queue_header vpu_job_queue_header_t;

/*
 * Job queue format.
 */
struct vpu_job_queue {
    struct vpu_job_queue_header header;
    struct vpu_job job[];
};
typedef struct vpu_job_queue vpu_job_queue_t;

/**
 * Logging entity types.
 *
 * This enum defines the different types of entities involved in logging.
 */
enum vpu_trace_entity_type {
    /** Logging destination (entity where logs can be stored / printed). */
    VPU_TRACE_ENTITY_TYPE_DESTINATION = 1,
    /** Loggable HW component (HW entity that can be logged). */
    VPU_TRACE_ENTITY_TYPE_HW_COMPONENT = 2,
};

/*
 * Host <-> VPU IPC messages types.
 */
enum vpu_ipc_msg_type {
    VPU_IPC_MSG_UNKNOWN = 0xFFFFFFFF,
    /* IPC Host -> Device, Async commands */
    VPU_IPC_MSG_ASYNC_CMD = 0x1100,
    VPU_IPC_MSG_ENGINE_RESET = VPU_IPC_MSG_ASYNC_CMD,
    VPU_IPC_MSG_ENGINE_PREEMPT = 0x1101,
    VPU_IPC_MSG_REGISTER_DB = 0x1102,
    VPU_IPC_MSG_UNREGISTER_DB = 0x1103,
    VPU_IPC_MSG_QUERY_ENGINE_HB = 0x1104,
    VPU_IPC_MSG_GET_POWER_LEVEL_COUNT = 0x1105,
    VPU_IPC_MSG_GET_POWER_LEVEL = 0x1106,
    VPU_IPC_MSG_SET_POWER_LEVEL = 0x1107,
    /* @deprecated */
    VPU_IPC_MSG_METRIC_STREAMER_OPEN = 0x1108,
    /* @deprecated */
    VPU_IPC_MSG_METRIC_STREAMER_CLOSE = 0x1109,
    /** Configure logging (used to modify configuration passed in boot params). */
    VPU_IPC_MSG_TRACE_SET_CONFIG = 0x110a,
    /** Return current logging configuration. */
    VPU_IPC_MSG_TRACE_GET_CONFIG = 0x110b,
    /**
     * Get masks of destinations and HW components supported by the firmware
     * (may vary between HW generations and FW compile
     * time configurations)
     */
    VPU_IPC_MSG_TRACE_GET_CAPABILITY = 0x110c,
    /** Get the name of a destination or HW component. */
    VPU_IPC_MSG_TRACE_GET_NAME = 0x110d,
    /**
     * Release resource associated with host ssid . All jobs that belong to the host_ssid
     * aborted and removed from internal scheduling queues. All doorbells assigned
     * to the host_ssid are unregistered and any internal FW resources belonging to
     * the host_ssid are released.
     */
    VPU_IPC_MSG_SSID_RELEASE = 0x110e,
    /**
     * Start collecting metric data.
     * @see vpu_jsm_metric_streamer_start
     */
    VPU_IPC_MSG_METRIC_STREAMER_START = 0x110f,
    /**
     * Stop collecting metric data. This command will return success if it is called
     * for a metric stream that has already been stopped or was never started.
     * @see vpu_jsm_metric_streamer_stop
     */
    VPU_IPC_MSG_METRIC_STREAMER_STOP = 0x1110,
    /**
     * Update current and next buffer for metric data collection. This command can
     * also be used to request information about the number of collected samples
     * and the amount of data written to the buffer.
     * @see vpu_jsm_metric_streamer_update
     */
    VPU_IPC_MSG_METRIC_STREAMER_UPDATE = 0x1111,
    /**
     * Request description of selected metric groups and metric counters within
     * each group. The VPU will write the description of groups and counters to
     * the buffer specified in the command structure.
     * @see vpu_jsm_metric_streamer_start
     */
    VPU_IPC_MSG_METRIC_STREAMER_INFO = 0x1112,
    /** Control command: Priority band setup */
    VPU_IPC_MSG_SET_PRIORITY_BAND_SETUP = 0x1113,
    /** Control command: Create command queue */
    VPU_IPC_MSG_CREATE_CMD_QUEUE = 0x1114,
    /** Control command: Destroy command queue */
    VPU_IPC_MSG_DESTROY_CMD_QUEUE = 0x1115,
    /** Control command: Set context scheduling properties */
    VPU_IPC_MSG_SET_CONTEXT_SCHED_PROPERTIES = 0x1116,
    /*
     * Register a doorbell to notify VPU of new work. The doorbell may later be
     * deallocated or reassigned to another context.
     */
    VPU_IPC_MSG_HWS_REGISTER_DB = 0x1117,
    /* IPC Host -> Device, General commands */
    VPU_IPC_MSG_GENERAL_CMD = 0x1200,
    VPU_IPC_MSG_BLOB_DEINIT = VPU_IPC_MSG_GENERAL_CMD,
    /**
     * Control dyndbg behavior by executing a dyndbg command; equivalent to
     * Linux command: `echo '<dyndbg_cmd>' > <debugfs>/dynamic_debug/control`.
     */
    VPU_IPC_MSG_DYNDBG_CONTROL = 0x1201,
    /* IPC Device -> Host, Job completion */
    VPU_IPC_MSG_JOB_DONE = 0x2100,
    /* IPC Device -> Host, Async command completion */
    VPU_IPC_MSG_ASYNC_CMD_DONE = 0x2200,
    VPU_IPC_MSG_ENGINE_RESET_DONE = VPU_IPC_MSG_ASYNC_CMD_DONE,
    VPU_IPC_MSG_ENGINE_PREEMPT_DONE = 0x2201,
    VPU_IPC_MSG_REGISTER_DB_DONE = 0x2202,
    VPU_IPC_MSG_UNREGISTER_DB_DONE = 0x2203,
    VPU_IPC_MSG_QUERY_ENGINE_HB_DONE = 0x2204,
    VPU_IPC_MSG_GET_POWER_LEVEL_COUNT_DONE = 0x2205,
    VPU_IPC_MSG_GET_POWER_LEVEL_DONE = 0x2206,
    VPU_IPC_MSG_SET_POWER_LEVEL_DONE = 0x2207,
    /* @deprecated */
    VPU_IPC_MSG_METRIC_STREAMER_OPEN_DONE = 0x2208,
    /* @deprecated */
    VPU_IPC_MSG_METRIC_STREAMER_CLOSE_DONE = 0x2209,
    /** Response to VPU_IPC_MSG_TRACE_SET_CONFIG. */
    VPU_IPC_MSG_TRACE_SET_CONFIG_RSP = 0x220a,
    /** Response to VPU_IPC_MSG_TRACE_GET_CONFIG. */
    VPU_IPC_MSG_TRACE_GET_CONFIG_RSP = 0x220b,
    /** Response to VPU_IPC_MSG_TRACE_GET_CAPABILITY. */
    VPU_IPC_MSG_TRACE_GET_CAPABILITY_RSP = 0x220c,
    /** Response to VPU_IPC_MSG_TRACE_GET_NAME. */
    VPU_IPC_MSG_TRACE_GET_NAME_RSP = 0x220d,
    /** Response to VPU_IPC_MSG_SSID_RELEASE. */
    VPU_IPC_MSG_SSID_RELEASE_DONE = 0x220e,
    /**
     * Response to VPU_IPC_MSG_METRIC_STREAMER_START.
     * VPU will return an error result if metric collection cannot be started,
     * e.g. when the specified metric mask is invalid.
     * @see vpu_jsm_metric_streamer_done
     */
    VPU_IPC_MSG_METRIC_STREAMER_START_DONE = 0x220f,
    /**
     * Response to VPU_IPC_MSG_METRIC_STREAMER_STOP.
     * Returns information about collected metric data.
     * @see vpu_jsm_metric_streamer_done
     */
    VPU_IPC_MSG_METRIC_STREAMER_STOP_DONE = 0x2210,
    /**
     * Response to VPU_IPC_MSG_METRIC_STREAMER_UPDATE.
     * Returns information about collected metric data.
     * @see vpu_jsm_metric_streamer_done
     */
    VPU_IPC_MSG_METRIC_STREAMER_UPDATE_DONE = 0x2211,
    /**
     * Response to VPU_IPC_MSG_METRIC_STREAMER_INFO.
     * Returns a description of the metric groups and metric counters.
     * @see vpu_jsm_metric_streamer_done
     */
    VPU_IPC_MSG_METRIC_STREAMER_INFO_DONE = 0x2212,
    /**
     * Asynchronous event sent from the VPU to the host either when the current
     * metric buffer is full or when the VPU has collected a multiple of
     * @notify_sample_count samples as indicated through the start command
     * (VPU_IPC_MSG_METRIC_STREAMER_START). Returns information about collected
     * metric data.
     * @see vpu_jsm_metric_streamer_done
     */
    VPU_IPC_MSG_METRIC_STREAMER_NOTIFICATION = 0x2213,
    /** Response to control command: Priority band setup */
    VPU_IPC_MSG_SET_PRIORITY_BAND_SETUP_RSP = 0x2214,
    /** Response to control command: Create command queue */
    VPU_IPC_MSG_CREATE_CMD_QUEUE_RSP = 0x2215,
    /** Response to control command: Destroy command queue */
    VPU_IPC_MSG_DESTROY_CMD_QUEUE_RSP = 0x2216,
    /** Response to control command: Set context scheduling properties */
    VPU_IPC_MSG_SET_CONTEXT_SCHED_PROPERTIES_RSP = 0x2217,
    /* IPC Device -> Host, General command completion */
    VPU_IPC_MSG_GENERAL_CMD_DONE = 0x2300,
    VPU_IPC_MSG_BLOB_DEINIT_DONE = VPU_IPC_MSG_GENERAL_CMD_DONE,
    /** Response to VPU_IPC_MSG_DYNDBG_CONTROL. */
    VPU_IPC_MSG_DYNDBG_CONTROL_RSP = 0x2301,
};
typedef enum vpu_ipc_msg_type vpu_ipc_msg_type_t;

enum vpu_ipc_msg_status { VPU_IPC_MSG_FREE, VPU_IPC_MSG_ALLOCATED };
typedef enum vpu_ipc_msg_status vpu_ipc_msg_status_t;

/*
 * Host <-> LRT IPC message payload definitions
 */
struct vpu_ipc_msg_payload_engine_reset {
    /* Engine to be reset. */
    uint32_t engine_idx;
};
typedef struct vpu_ipc_msg_payload_engine_reset vpu_ipc_msg_payload_engine_reset_t;

struct vpu_ipc_msg_payload_engine_preempt {
    /* Engine to be preempted. */
    uint32_t engine_idx;
    /* ID of the preemption request. */
    uint32_t preempt_id;
};
typedef struct vpu_ipc_msg_payload_engine_preempt vpu_ipc_msg_payload_engine_preempt_t;

/*
 * @brief Register doorbell command structure.
 * This structure supports doorbell registration for only OS scheduling.
 * @see VPU_IPC_MSG_REGISTER_DB
 */
struct vpu_ipc_msg_payload_register_db {
    /* Index of the doorbell to register. */
    uint32_t db_idx;
    /* Virtual address in Global GTT pointing to the start of job queue. */
    uint64_t jobq_base;
    /* Size of the job queue in bytes. */
    uint32_t jobq_size;
    /* Host sub-stream ID for the context assigned to the doorbell. */
    uint32_t host_ssid;
};
typedef struct vpu_ipc_msg_payload_register_db vpu_ipc_msg_payload_register_db_t;

/**
 * @brief Unregister doorbell command structure.
 * Request structure to unregister a doorbell for both HW and OS scheduling.
 * @see VPU_IPC_MSG_UNREGISTER_DB
 */
struct vpu_ipc_msg_payload_unregister_db {
    /* Index of the doorbell to unregister. */
    uint32_t db_idx;
};
typedef struct vpu_ipc_msg_payload_unregister_db vpu_ipc_msg_payload_unregister_db_t;

struct vpu_ipc_msg_payload_query_engine_hb {
    /* Engine to return heartbeat value. */
    uint32_t engine_idx;
};
typedef struct vpu_ipc_msg_payload_query_engine_hb vpu_ipc_msg_payload_query_engine_hb_t;

struct vpu_ipc_msg_payload_power_level {
    /**
     * Requested power level. The power level value is in the
     * range [0, power_level_count-1] where power_level_count
     * is the number of available power levels as returned by
     * the get power level count command. A power level of 0
     * corresponds to the maximum possible power level, while
     * power_level_count-1 corresponds to the minimum possible
     * power level. Values outside of this range are not
     * considered to be valid.
     */
    uint32_t power_level;
};
typedef struct vpu_ipc_msg_payload_set_power_level vpu_ipc_msg_payload_set_power_level_t;

struct vpu_ipc_msg_payload_ssid_release {
    /* Host sub-stream ID for the context to be released. */
    uint32_t host_ssid;
};
typedef struct vpu_ipc_msg_payload_ssid_release vpu_ipc_msg_payload_ssid_release_t;

/**
 * @brief Metric streamer start command structure.
 * This structure is also used with VPU_IPC_MSG_METRIC_STREAMER_INFO to request metric
 * groups and metric counters description from the firmware.
 * @see VPU_IPC_MSG_METRIC_STREAMER_START
 * @see VPU_IPC_MSG_METRIC_STREAMER_INFO
 */
struct vpu_jsm_metric_streamer_start {
    /**
     * Bitmask to select the desired metric groups.
     * A metric group can belong only to one metric streamer instance at a time.
     * Since each metric streamer instance has a unique set of metric groups, it
     * can also identify a metric streamer instance if more than one instance was
     * started. If the VPU device does not support multiple metric streamer instances,
     * then VPU_IPC_MSG_METRIC_STREAMER_START will return an error even if the second
     * instance has different groups to the first.
     */
    uint64_t metric_group_mask;
    /** Sampling rate in nanoseconds. */
    uint64_t sampling_rate;
    /**
     * If > 0 the VPU will send a VPU_IPC_MSG_METRIC_STREAMER_NOTIFICATION message
     * after every @notify_sample_count samples is collected or dropped by the VPU.
     * If set to UINT_MAX the VPU will only generate a notification when the metric
     * buffer is full. If set to 0 the VPU will never generate a notification.
     */
    uint32_t notify_sample_count;
    uint32_t reserved_0;
    /**
     * Address and size of the buffer where the VPU will write metric data. The
     * VPU writes all counters from enabled metric groups one after another. If
     * there is no space left to write data at the next sample period the VPU
     * will switch to the next buffer (@see next_buffer_addr) and will optionally
     * send a notification to the host driver if @notify_sample_count is non-zero.
     * If @next_buffer_addr is NULL the VPU will stop collecting metric data.
     */
    uint64_t buffer_addr;
    uint64_t buffer_size;
    /**
     * Address and size of the next buffer to write metric data to after the initial
     * buffer is full. If the address is NULL the VPU will stop collecting metric
     * data.
     */
    uint64_t next_buffer_addr;
    uint64_t next_buffer_size;
};
typedef struct vpu_jsm_metric_streamer_start vpu_jsm_metric_streamer_start_t;
static_assert(sizeof(struct vpu_jsm_metric_streamer_start) % 8 == 0, "vpu_jsm_metric_streamer_start is misaligned");

/**
 * @brief Metric streamer stop command structure.
 * @see VPU_IPC_MSG_METRIC_STREAMER_STOP
 */
struct vpu_jsm_metric_streamer_stop {
    /** Bitmask to select the desired metric groups. */
    uint64_t metric_group_mask;
};
typedef struct vpu_jsm_metric_streamer_stop vpu_jsm_metric_streamer_stop_t;
static_assert(sizeof(struct vpu_jsm_metric_streamer_stop) % 8 == 0, "vpu_jsm_metric_streamer_stop is misaligned");

/**
 * Provide VPU FW with buffers to write metric data.
 * @see VPU_IPC_MSG_METRIC_STREAMER_UPDATE
 */
struct vpu_jsm_metric_streamer_update {
    /** Metric group mask that identifies metric streamer instance. */
    uint64_t metric_group_mask;
    /**
     * Address and size of the buffer where the VPU will write metric data. If
     * the buffer address is 0 or same as the currently used buffer the VPU will
     * continue writing metric data to the current buffer. In this case the
     * buffer size is ignored and the size of the current buffer is unchanged.
     * If the address is non-zero and differs from the current buffer address the
     * VPU will immediately switch data collection to the new buffer.
     */
    uint64_t buffer_addr;
    uint64_t buffer_size;
    /**
     * Address and size of the next buffer to write metric data after the initial
     * buffer is full. If the address is NULL the VPU will stop collecting metric
     * data but will continue to record dropped samples.
     *
     * Note that there is a hazard possible if both buffer_addr and the next_buffer_addr
     * are non-zero in same update request. It is the host's responsibility to ensure
     * that both addresses make sense even if the VPU just switched to writing samples
     * from the current to the next buffer.
     */
    uint64_t next_buffer_addr;
    uint64_t next_buffer_size;
};
typedef struct vpu_jsm_metric_streamer_update vpu_jsm_metric_streamer_update_t;
static_assert(sizeof(struct vpu_jsm_metric_streamer_update) % 8 == 0, "vpu_jsm_metric_streamer_update is misaligned");

struct vpu_ipc_msg_payload_blob_deinit {
    /* 64-bit unique ID for the blob to be de-initialized. */
    uint64_t blob_id;
};
typedef struct vpu_ipc_msg_payload_blob_deinit vpu_ipc_msg_payload_blob_deinit_t;

struct vpu_ipc_msg_payload_job_done {
    /* Engine to which the job was submitted. */
    uint32_t engine_idx;
    /* Index of the doorbell to which the job was submitted */
    uint32_t db_idx;
    /* ID of the completed job */
    uint32_t job_id;
    /* Status of the completed job */
    uint32_t job_status;
    /* Host SSID */
    uint32_t host_ssid;
    /* Zero Padding */
    uint32_t reserved;
    /* Command queue id */
    uint64_t cmdq_id;
};
typedef struct vpu_ipc_msg_payload_job_done vpu_ipc_msg_payload_job_done_t;

struct vpu_jsm_engine_reset_context {
    /* Host SSID */
    uint32_t host_ssid;
    /* Zero Padding */
    uint32_t reserved;
    /* Command queue id */
    uint64_t cmdq_id;
    /* Flags: 0: cause of hang; 1: collateral damage of reset */
    uint64_t flags;
};

struct vpu_ipc_msg_payload_engine_reset_done {
    /* Engine ordinal */
    uint32_t engine_idx;
    /* Number of impacted contexts */
    uint32_t num_impacted_contexts;
    /* Array of impacted command queue ids and their flags */
    struct vpu_jsm_engine_reset_context impacted_contexts[VPU_MAX_ENGINE_RESET_IMPACTED_CONTEXTS];
};
typedef struct vpu_ipc_msg_payload_engine_reset_done vpu_ipc_msg_payload_engine_reset_done_t;

struct vpu_ipc_msg_payload_engine_preempt_done {
    /* Engine preempted. */
    uint32_t engine_idx;
    /* ID of the preemption request. */
    uint32_t preempt_id;
};
typedef struct vpu_ipc_msg_payload_engine_preempt_done vpu_ipc_msg_payload_engine_preempt_done_t;

/**
 * Response structure for register doorbell command for both OS
 * and HW scheduling.
 * @see VPU_IPC_MSG_REGISTER_DB
 * @see VPU_IPC_MSG_HWS_REGISTER_DB
 */
struct vpu_ipc_msg_payload_register_db_done {
    /* Index of the registered doorbell. */
    uint32_t db_idx;
};
typedef struct vpu_ipc_msg_payload_register_db_done vpu_ipc_msg_payload_register_db_done_t;

/**
 * Response structure for unregister doorbell command for both OS
 * and HW scheduling.
 * @see VPU_IPC_MSG_UNREGISTER_DB
 */
struct vpu_ipc_msg_payload_unregister_db_done {
    /* Index of the unregistered doorbell. */
    uint32_t db_idx;
};
typedef struct vpu_ipc_msg_payload_unregister_db_done vpu_ipc_msg_payload_unregister_db_done_t;

struct vpu_ipc_msg_payload_query_engine_hb_done {
    /* Engine returning heartbeat value. */
    uint32_t engine_idx;
    /* Heartbeat value. */
    uint64_t heartbeat;
};
typedef struct vpu_ipc_msg_payload_query_engine_hb_done vpu_ipc_msg_payload_query_engine_hb_done_t;

struct vpu_ipc_msg_payload_get_power_level_count_done {
    /**
     * Number of supported power levels. The maximum possible
     * value of power_level_count is 16 but this may vary across
     * implementations.
     */
    uint32_t power_level_count;
    /**
     * Power consumption limit for each supported power level in
     * [0-100%] range relative to power level 0.
     */
    uint8_t power_limit[16];
};
typedef struct vpu_ipc_msg_payload_get_power_level_count_done vpu_ipc_msg_payload_get_power_level_count_done_t;

struct vpu_ipc_msg_payload_blob_deinit_done {
    /* 64-bit unique ID for the blob de-initialized. */
    uint64_t blob_id;
};
typedef struct vpu_ipc_msg_payload_blob_deinit_done vpu_ipc_msg_payload_blob_deinit_done_t;

/* HWS priority band setup request / response */
struct vpu_ipc_msg_payload_hws_priority_band_setup {
    /*
     * Grace period in 100ns units when preempting another priority band for
     * this priority band
     */
    uint64_t grace_period[VPU_HWS_NUM_PRIORITY_BANDS];
    /*
     * Default quantum in 100ns units for scheduling across processes
     * within a priority band
     */
    uint64_t process_quantum[VPU_HWS_NUM_PRIORITY_BANDS];
    /*
     * Default grace period in 100ns units for processes that preempt each
     * other within a priority band
     */
    uint64_t process_grace_period[VPU_HWS_NUM_PRIORITY_BANDS];
    /*
     * For normal priority band, specifies the target VPU percentage
     * in situations when it's starved by the focus band.
     */
    uint32_t normal_band_percentage;
};

/* HWS create command queue request */
struct vpu_ipc_msg_payload_hws_create_cmdq {
    /* Process id */
    uint64_t process_id;
    /* Host SSID */
    uint32_t host_ssid;
    /* Zero Padding */
    uint32_t reserved;
    /* Command queue id */
    uint64_t cmdq_id;
    /* Command queue base */
    uint64_t cmdq_base;
    /* Command queue size */
    uint32_t cmdq_size;
};

/* HWS create command queue response */
struct vpu_ipc_msg_payload_hws_create_cmdq_rsp {
    /* Process id */
    uint64_t process_id;
    /* Host SSID */
    uint32_t host_ssid;
    /* Zero Padding */
    uint32_t reserved;
    /* Command queue id */
    uint64_t cmdq_id;
};

/* HWS destroy command queue request / response */
struct vpu_ipc_msg_payload_hws_destroy_cmdq {
    /* Host SSID */
    uint32_t host_ssid;
    /* Zero Padding */
    uint32_t reserved;
    /* Command queue id */
    uint64_t cmdq_id;
};

/* HWS set context scheduling properties request / response */
struct vpu_ipc_msg_payload_hws_set_context_sched_properties {
    /* Host SSID */
    uint32_t host_ssid;
    /* Zero Padding */
    uint32_t reserved_0;
    /* Command queue id */
    uint64_t cmdq_id;
    /* Priority band to assign to work of this context */
    uint32_t priority_band;
    /* Inside realtime band assigns a further priority */
    uint32_t realtime_priority_level;
    /* Priority relative to other contexts in the same process */
    uint32_t in_process_priority;
    /* Zero padding / Reserved */
    uint32_t reserved_1;
    /* Context quantum relative to other contexts of same priority in the same process */
    uint64_t context_quantum;
    /* Grace period when preempting context of the same priority within the same process */
    uint64_t grace_period_same_priority;
    /* Grace period when preempting context of a lower priority within the same process */
    uint64_t grace_period_lower_priority;
};

/*
 * @brief Register doorbell command structure.
 * This structure supports doorbell registration for both HW and OS scheduling.
 * Note: Queue base and size are added here so that the same structure can be used for
 * OS scheduling and HW scheduling. For OS scheduling, cmdq_id will be ignored
 * and cmdq_base and cmdq_size will be used. For HW scheduling, cmdq_base and cmdq_size will be
 * ignored and cmdq_id is used.
 * @see VPU_IPC_MSG_HWS_REGISTER_DB
 */
struct vpu_jsm_hws_register_db {
    /* Index of the doorbell to register. */
    uint32_t db_id;
    /* Host sub-stream ID for the context assigned to the doorbell. */
    uint32_t host_ssid;
    /* ID of the command queue associated with the doorbell. */
    uint64_t cmdq_id;
    /* Virtual address pointing to the start of command queue. */
    uint64_t cmdq_base;
    /* Size of the command queue in bytes. */
    uint64_t cmdq_size;
};

/**
 * Payload for VPU_IPC_MSG_TRACE_SET_CONFIG[_RSP] and
 * VPU_IPC_MSG_TRACE_GET_CONFIG_RSP messages.
 *
 * The payload is interpreted differently depending on the type of message:
 *
 * - For VPU_IPC_MSG_TRACE_SET_CONFIG, the payload specifies the desired
 *   logging configuration to be set.
 *
 * - For VPU_IPC_MSG_TRACE_SET_CONFIG_RSP, the payload reports the logging
 *   configuration that was set after a VPU_IPC_MSG_TRACE_SET_CONFIG request.
 *   The host can compare this payload with the one it sent in the
 *   VPU_IPC_MSG_TRACE_SET_CONFIG request to check whether or not the
 *   configuration was set as desired.
 *
 * - VPU_IPC_MSG_TRACE_GET_CONFIG_RSP, the payload reports the current logging
 *   configuration.
 */
struct vpu_ipc_msg_payload_trace_config {
    /**
     * Logging level (currently set or to be set); see 'mvLog_t' enum for
     * acceptable values. The specified logging level applies to all
     * destinations and HW components
     */
    uint32_t trace_level;
    /**
     * Bitmask of logging destinations (currently enabled or to be enabled);
     * bitwise OR of values defined in logging_destination enum.
     */
    uint32_t trace_destination_mask;
    /**
     * Bitmask of loggable HW components (currently enabled or to be enabled);
     * bitwise OR of values defined in loggable_hw_component enum.
     */
    uint64_t trace_hw_component_mask;
    uint64_t reserved_0; /**< Reserved for future extensions. */
};

/**
 * Payload for VPU_IPC_MSG_TRACE_GET_CAPABILITY_RSP messages.
 */
struct vpu_ipc_msg_payload_trace_capability_rsp {
    uint32_t trace_destination_mask; /**< Bitmask of supported logging destinations. */
    uint32_t reserved_0;
    uint64_t trace_hw_component_mask; /**< Bitmask of supported loggable HW components. */
    uint64_t reserved_1;              /**< Reserved for future extensions. */
};

/**
 * Payload for VPU_IPC_MSG_TRACE_GET_NAME requests.
 */
struct vpu_ipc_msg_payload_trace_get_name {
    /**
     * The type of the entity to query name for; see logging_entity_type for
     * possible values.
     */
    uint32_t entity_type;
    uint32_t reserved_0;
    /**
     * The ID of the entity to query name for; possible values depends on the
     * entity type.
     */
    uint64_t entity_id;
};

/**
 * Payload for VPU_IPC_MSG_TRACE_GET_NAME_RSP responses.
 */
struct vpu_ipc_msg_payload_trace_get_name_rsp {
    /**
     * The type of the entity whose name was queried; see logging_entity_type
     * for possible values.
     */
    uint32_t entity_type;
    uint32_t reserved_0;
    /**
     * The ID of the entity whose name was queried; possible values depends on
     * the entity type.
     */
    uint64_t entity_id;
    /** Reserved for future extensions. */
    uint64_t reserved_1;
    /** The name of the entity. */
    char entity_name[VPU_TRACE_ENTITY_NAME_MAX_LEN];
};

/**
 * Data sent from the VPU to the host in all metric streamer response messages
 * and in asynchronous notification.
 * @see VPU_IPC_MSG_METRIC_STREAMER_START_DONE
 * @see VPU_IPC_MSG_METRIC_STREAMER_STOP_DONE
 * @see VPU_IPC_MSG_METRIC_STREAMER_UPDATE_DONE
 * @see VPU_IPC_MSG_METRIC_STREAMER_INFO_DONE
 * @see VPU_IPC_MSG_METRIC_STREAMER_NOTIFICATION
 */
struct vpu_jsm_metric_streamer_done {
    /** Metric group mask that identifies metric streamer instance. */
    uint64_t metric_group_mask;
    /**
     * Size in bytes of single sample - total size of all enabled counters.
     * Some VPU implementations may align sample_size to more than 8 bytes.
     */
    uint32_t sample_size;
    uint32_t reserved_0;
    /**
     * Number of samples collected since the metric streamer was started.
     * This will be 0 if the metric streamer was not started.
     */
    uint32_t samples_collected;
    /**
     * Number of samples dropped since the metric streamer was started. This
     * is incremented every time the metric streamer is not able to write
     * collected samples because the current buffer is full and there is no
     * next buffer to switch to.
     */
    uint32_t samples_dropped;
    /** Address of the buffer that contains the latest metric data. */
    uint64_t buffer_addr;
    /**
     * Number of bytes written into the metric data buffer. In response to the
     * VPU_IPC_MSG_METRIC_STREAMER_INFO request this field contains the size of
     * all group and counter descriptors. The size is updated even if the buffer
     * in the request was NULL or too small to hold descriptors of all counters
     */
    uint64_t bytes_written;
};
typedef struct vpu_jsm_metric_streamer_done vpu_jsm_metric_streamer_done_t;
static_assert(sizeof(struct vpu_jsm_metric_streamer_done) % 8 == 0, "vpu_jsm_metric_streamer_done is misaligned");

/**
 * Metric group description placed in the metric buffer after successful completion
 * of the VPU_IPC_MSG_METRIC_STREAMER_INFO command. This is followed by one or more
 * @vpu_jsm_metric_counter_descriptor records.
 * @see VPU_IPC_MSG_METRIC_STREAMER_INFO
 */
struct vpu_jsm_metric_group_descriptor {
    /**
     * Offset to the next metric group (8-byte aligned). If this offset is 0 this
     * is the last descriptor. The value of metric_info_size must be greater than
     * or equal to sizeof(struct vpu_jsm_metric_group_descriptor) + name_string_size
     * + description_string_size and must be 8-byte aligned.
     */
    uint32_t next_metric_group_info_offset;
    /**
     * Offset to the first metric counter description record (8-byte aligned).
     * @see vpu_jsm_metric_counter_descriptor
     */
    uint32_t next_metric_counter_info_offset;
    /** Index of the group. This corresponds to bit index in metric_group_mask. */
    uint32_t group_id;
    /** Number of counters in the metric group. */
    uint32_t num_counters;
    /** Data size for all counters, must be a multiple of 8 bytes.*/
    uint32_t metric_group_data_size;
    /**
     * Metric group domain number. Cannot use multiple, simultaneous metric groups
     * from the same domain.
     */
    uint32_t domain;
    /**
     * Counter name string size. The string must include a null termination character.
     * The FW may use a fixed size name or send a different name for each counter.
     * If the VPU uses fixed size strings, all characters from the end of the name
     * to the of the fixed size character array must be zeroed.
     */
    uint32_t name_string_size;
    /** Counter description string size, @see name_string_size */
    uint32_t description_string_size;
    uint32_t reserved_0[2];
    /**
     * Right after this structure, the VPU writes name and description of
     * the metric group.
     */
};
typedef struct vpu_jsm_metric_group_descriptor vpu_jsm_metric_group_descriptor_t;
static_assert(sizeof(struct vpu_jsm_metric_group_descriptor) % 8 == 0, "vpu_jsm_metric_group_descriptor is misaligned");

/**
 * Metric counter description, placed in the buffer after vpu_jsm_metric_group_descriptor.
 * @see VPU_IPC_MSG_METRIC_STREAMER_INFO
 */
struct vpu_jsm_metric_counter_descriptor {
    /**
     * Offset to the next counter in a group (8-byte aligned). If this offset is
     * 0 this is the last counter in the group.
     */
    uint32_t next_metric_counter_info_offset;
    /**
     * Offset to the counter data from the start of samples in this metric group.
     * Note that metric_data_offset % metric_data_size must be 0.
     */
    uint32_t metric_data_offset;
    /** Size of the metric counter data in bytes. */
    uint32_t metric_data_size;
    /** Metric type, see Level Zero API for definitions. */
    uint32_t tier;
    /** Metric type, see set_metric_type_t for definitions. */
    uint32_t metric_type;
    /** Metric type, see set_value_type_t for definitions. */
    uint32_t metric_value_type;
    /**
     * Counter name string size. The string must include a null termination character.
     * The FW may use a fixed size name or send a different name for each counter.
     * If the VPU uses fixed size strings, all characters from the end of the name
     * to the of the fixed size character array must be zeroed.
     */
    uint32_t name_string_size;
    /** Counter description string size, @see name_string_size */
    uint32_t description_string_size;
    /** Counter component name string size, @see name_string_size */
    uint32_t component_string_size;
    /** Counter string size, @see name_string_size */
    uint32_t units_string_size;
    uint32_t reserved_0[2];
    /**
     * Right after this structure, the VPU writes name, description
     * component and unit strings.
     */
};
typedef struct vpu_jsm_metric_counter_descriptor vpu_jsm_metric_counter_descriptor_t;
static_assert(sizeof(struct vpu_jsm_metric_counter_descriptor) % 8 == 0,
              "vpu_jsm_metric_counter_descriptor is misaligned");

/**
 * Payload for VPU_IPC_MSG_DYNDBG_CONTROL requests.
 *
 * VPU_IPC_MSG_DYNDBG_CONTROL are used to control the VPU FW Dynamic Debug
 * feature, which allows developers to selectively enable / disable MVLOG_DEBUG
 * messages. This is equivalent to the Dynamic Debug functionality provided by
 * Linux
 * (https://www.kernel.org/doc/html/latest/admin-guide/dynamic-debug-howto.html)
 * The host can control Dynamic Debug behavior by sending dyndbg commands, which
 * have the same syntax as Linux
 * dyndbg commands.
 *
 * NOTE: in order for MVLOG_DEBUG messages to be actually printed, the host
 * still has to set the logging level to MVLOG_DEBUG, using the
 * VPU_IPC_MSG_TRACE_SET_CONFIG command.
 *
 * The host can see the current dynamic debug configuration by executing a
 * special 'show' command. The dyndbg configuration will be printed to the
 * configured logging destination using MVLOG_INFO logging level.
 */
struct vpu_ipc_msg_payload_dyndbg_control {
    /**
     * Dyndbg command (same format as Linux dyndbg); must be a NULL-terminated
     * string.
     */
    char dyndbg_cmd[VPU_DYNDBG_CMD_MAX_LEN];
};

/*
 * Payloads union, used to define complete message format.
 */
union vpu_ipc_msg_payload {
    struct vpu_ipc_msg_payload_engine_reset engine_reset;
    struct vpu_ipc_msg_payload_engine_preempt engine_preempt;
    struct vpu_ipc_msg_payload_register_db register_db;
    struct vpu_ipc_msg_payload_unregister_db unregister_db;
    struct vpu_ipc_msg_payload_query_engine_hb query_engine_hb;
    struct vpu_ipc_msg_payload_power_level power_level;
    struct vpu_jsm_metric_streamer_start metric_streamer_start;
    struct vpu_jsm_metric_streamer_stop metric_streamer_stop;
    struct vpu_jsm_metric_streamer_update metric_streamer_update;
    struct vpu_ipc_msg_payload_blob_deinit blob_deinit;
    struct vpu_ipc_msg_payload_ssid_release ssid_release;
    struct vpu_jsm_hws_register_db hws_register_db;
    struct vpu_ipc_msg_payload_job_done job_done;
    struct vpu_ipc_msg_payload_engine_reset_done engine_reset_done;
    struct vpu_ipc_msg_payload_engine_preempt_done engine_preempt_done;
    struct vpu_ipc_msg_payload_register_db_done register_db_done;
    struct vpu_ipc_msg_payload_unregister_db_done unregister_db_done;
    struct vpu_ipc_msg_payload_query_engine_hb_done query_engine_hb_done;
    struct vpu_ipc_msg_payload_get_power_level_count_done get_power_level_count_done;
    struct vpu_jsm_metric_streamer_done metric_streamer_done;
    struct vpu_ipc_msg_payload_blob_deinit_done blob_deinit_done;
    struct vpu_ipc_msg_payload_trace_config trace_config;
    struct vpu_ipc_msg_payload_trace_capability_rsp trace_capability;
    struct vpu_ipc_msg_payload_trace_get_name trace_get_name;
    struct vpu_ipc_msg_payload_trace_get_name_rsp trace_get_name_rsp;
    struct vpu_ipc_msg_payload_dyndbg_control dyndbg_control;
    struct vpu_ipc_msg_payload_hws_priority_band_setup hws_priority_band_setup;
    struct vpu_ipc_msg_payload_hws_create_cmdq hws_create_cmdq;
    struct vpu_ipc_msg_payload_hws_create_cmdq_rsp hws_create_cmdq_rsp;
    struct vpu_ipc_msg_payload_hws_destroy_cmdq hws_destroy_cmdq;
    struct vpu_ipc_msg_payload_hws_set_context_sched_properties hws_set_context_sched_properties;
};
typedef union vpu_ipc_msg_payload vpu_ipc_msg_payload_t;

/*
 * Host <-> LRT IPC message base structure.
 *
 * NOTE: All instances of this object must be aligned on a 64B boundary
 * to allow proper handling of VPU cache operations.
 */
struct vpu_ipc_msg {
    /* Message type, see vpu_ipc_msg_type enum. */
    uint32_t type;
    /* Buffer status, see vpu_ipc_msg_status enum. */
    uint32_t status;
    /*
     * Request ID, provided by the host in a request message and passed
     * back by VPU in the response message.
     */
    uint32_t request_id;
    /* Request return code set by the VPU, see VPU_JSM_STATUS_* defines. */
    uint32_t result;
    /* Message payload depending on message type, see vpu_ipc_msg_payload union. */
    union vpu_ipc_msg_payload payload;
};
typedef struct vpu_ipc_msg vpu_ipc_msg_t;

#pragma pack(pop)

#endif

///@}
