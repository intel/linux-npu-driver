/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2023, Intel Corporation.
 */

// clang-format off

#ifndef VPU_NCE_HW_40XX_H
#define VPU_NCE_HW_40XX_H

namespace nn_public {

#pragma pack(push, 1)

typedef struct {
    uint32_t cmx_slice0_low_addr;
    uint32_t cmx_slice1_low_addr;
    uint32_t cmx_slice2_low_addr;
    uint32_t cmx_slice3_low_addr;
    uint32_t cmx_slice_size;
    uint32_t se_addr;
    uint32_t sparsity_addr;
    uint32_t se_size; // se_sp_size

    union {
        uint32_t z_config;
        struct {
            uint32_t se_z_split : 4;
            uint32_t num_ses_in_z_dir : 9;
            uint32_t cm_sp_pattern : 16;
            uint32_t npo2_se_z_split_enable : 1;
            uint32_t reserved : 1;
            uint32_t addr_format_sel : 1;
        } z_config_bf;
    } z_config;
    union {
        uint32_t kernel_pad_cfg;
        struct {
            uint32_t mpe_assign : 1;
            uint32_t pad_right_en : 1;
            uint32_t pad_left_en : 1;
            uint32_t pad_bottom_en : 1;
            uint32_t pad_top_en : 1;
            uint32_t kernel_y : 4;
            uint32_t kernel_x : 4;
            uint32_t wt_plt_cfg : 2;
            uint32_t act_dense : 1;
            uint32_t wt_dense : 1;
            uint32_t stride_y_en : 1;
            uint32_t stride_y : 3;
            uint32_t dynamic_bw_en : 1;
            uint32_t dw_wt_sp_ins : 1;
            uint32_t layer1_wt_sp_ins : 1;
            uint32_t layer1_cmp_en : 1;
            uint32_t pool_opt_en : 1;
            uint32_t unused1 : 3;
            uint32_t sp_se_tbl_segment : 1;
            uint32_t rst_ctxt : 1;
            uint32_t unused2 : 1;
        } kernel_pad_cfg_bf;
    } kernel_pad_cfg;
    union {
        uint32_t tensor_size0;
        struct {
            uint32_t tensor_size_x : 14;
            uint32_t tensor_size_y : 14;
            uint32_t unused : 4;
        } tensor_size0_bf;
    } tensor_size0;
    union {
        uint32_t tensor_size1;
        struct {
            uint32_t tensor_size_z : 14;
            uint32_t npo2_se_size : 9;
            uint32_t reserved1 : 4;
            uint32_t reserved2 : 4;
            uint32_t unused : 1;
        } tensor_size1_bf;
    } tensor_size1;
    uint32_t tensor_start;
    union {
        uint32_t tensor_mode;
        struct {
            uint32_t wmode : 4;
            uint32_t amode : 4;
            uint32_t stride : 3;
            uint32_t zm_input : 1;
            uint32_t dw_input : 1;
            uint32_t cm_input : 1;
            uint32_t workload_operation : 2;
            uint32_t pad_value : 16;
        } tensor_mode_bf;
    } tensor_mode;
    uint32_t elops_sparsity_addr;
    uint32_t elops_se_addr;

    union {
        uint32_t elops_wload;
        struct {
            uint32_t elop_wload : 1;
            uint32_t seed_wload : 1;
            uint32_t fifo_wr_wload : 1;
            uint32_t elop_wload_type : 1;
            uint32_t pool_wt_data : 16;
            uint32_t unused1 : 6;
            uint32_t pool_wt_rd_dis : 1;
            uint32_t reserved1 : 3;
            uint32_t reserved2 : 1;
            uint32_t unused2 : 1;
        } elops_wload_bf;
    } elops_wload;
    uint32_t act_offset[4];
    uint32_t base_offset_a;

    union {
        uint32_t base_offset_b;
        struct {
            uint32_t base_offset2 : 9;
            uint32_t base_offset3 : 9;
            uint32_t dw_opt_offset : 6;
            uint32_t dw_opt_en : 1;
            uint32_t dw_3x3s1_opt_dis : 1;
            uint32_t reserved1 : 1;
            uint32_t reserved2 : 1;
            uint32_t reserved3 : 2;
            uint32_t reserved_0 : 2;
        } base_offset_b_bf;
    } base_offset_b;

    uint32_t wt_offset;

    union {
        uint32_t odu_cfg;
        struct {
            uint32_t dtype : 3;
            uint32_t wcb_ac_mode : 1;
            uint32_t wcb_sp_mode : 1;
            uint32_t sp_value : 8;
            uint32_t sp_out_en : 1;
            uint32_t cmx_port_muxing_disable : 1;
            uint32_t write_sp : 1;
            uint32_t write_pt : 1;
            uint32_t write_ac : 1;
            uint32_t mode : 2;
            uint32_t grid : 1;
            uint32_t swizzle_key : 3;
            uint32_t wl_bp_on_start_en : 1;
            uint32_t nthw : 2;
            uint32_t permutation : 3;
            uint32_t wcb_stall_avoidance : 1;
            uint32_t wcb_bypass : 1;
        } odu_cfg_bf;
    } odu_cfg;
    uint32_t odu_be_size;
    uint32_t odu_be_cnt;
    uint32_t odu_se_size;
    union {
        uint32_t te_dim0;
        struct {
            uint32_t te_dim_y : 13;
            uint32_t te_dim_z : 13;
            uint32_t unused : 6;
        } te_dim0_bf;
    } te_dim0;
    union {
        uint32_t te_dim1;
        struct {
            uint32_t te_dim_x : 13;
            uint32_t unused : 19;
        } te_dim1_bf;
    } te_dim1;
    uint32_t pt_base;
    uint32_t sp_base;

    union {
        uint32_t mpe_cfg;
        struct {
            uint32_t mpe_wtbias : 8;
            uint32_t mpe_actbias : 8;
            uint32_t mpe_mode : 3;
            uint32_t mpe_dense : 1;
            uint32_t mrm_weight_dense : 1;
            uint32_t mrm_act_dense : 1;
            uint32_t mpe_daz : 1;
            uint32_t mpe_ftz : 1;
            uint32_t unused : 8;
        } mpe_cfg_bf;
    } mpe_cfg;
    uint32_t mpe_bus_data_sel;
    union {
        uint32_t elop_scale;
        struct {
            uint32_t elop_scale_b : 16;
            uint32_t elop_scale_a : 16;
        } elop_scale_bf;
    } elop_scale;
    union {
        uint32_t ppe_cfg;
        struct {
            uint32_t ppe_g8_bias_c : 9;
            uint32_t ppe_g8_bias_b : 9;
            uint32_t ppe_g8_bias_a : 9;
            uint32_t unused : 5;
        } ppe_cfg_bf;
    } ppe_cfg;
    uint32_t ppe_bias;
    union {
        uint32_t ppe_scale;
        struct {
            uint32_t unused : 2;
            uint32_t ppe_scale_shift : 6;
            uint32_t unused1 : 2;
            uint32_t ppe_scale_round : 2;
            uint32_t unused2 : 4;
            uint32_t ppe_scale_mult : 16;
        } ppe_scale_bf;
    } ppe_scale;
    union {
        uint32_t ppe_scale_ctrl;
        struct {
            uint32_t ppe_scale_override : 1;
            uint32_t ppe_fp_scale_override : 1;
            uint32_t unused : 30;
        } ppe_scale_ctrl_bf;
    } ppe_scale_ctrl;
    union {
        uint32_t ppe_prelu;
        struct {
            uint32_t unused : 8;          // 0-7
            uint32_t ppe_prelu_shift : 5; // 8-12
            uint32_t unused1 : 3;         // 13-15
            uint32_t ppe_prelu_mult : 11; // 16-26
            uint32_t unused2 : 5;         // 27-31
        } ppe_prelu_bf;
    } ppe_prelu;

    uint32_t ppe_scale_hclamp;
    uint32_t ppe_scale_lclamp;
    union {
        uint32_t ppe_misc;
        struct {
            uint32_t reserved1 : 2;
            uint32_t reserved2 : 2;
            uint32_t reserved3 : 2;
            uint32_t ppe_fp16_ftz : 1;
            uint32_t ppe_fp16_clamp : 1;
            uint32_t ppe_i32_convert : 2;
            uint32_t reserved4 : 2;
            uint32_t reserved5 : 1;
            uint32_t unused : 19;
        } ppe_misc_bf;
    } ppe_misc;
    uint32_t ppe_fp_bias;
    uint32_t ppe_fp_scale;
    uint32_t ppe_fp_prelu;
    union {
        uint32_t ppe_fp_cfg;
        struct {
            uint32_t ppe_fp_convert : 3;
            uint32_t ppe_fp_bypass : 1;
            uint32_t ppe_bf16_round : 1;
            uint32_t ppe_fp_prelu_en : 1;
            uint32_t unused : 26;
        } ppe_fp_cfg_bf;
    } ppe_fp_cfg;

    union {
        uint32_t odu_ac_base;
        struct {
            uint32_t unused : 4;
            uint32_t ac_base : 28;
        } odu_ac_base_bf;
    } odu_ac_base;

    union {
        uint32_t hwp_ctrl;
        struct {
            uint32_t hwp_en : 1;
            uint32_t hwp_stat_mode : 3;
            uint32_t local_timer_en : 1;
            uint32_t local_timer_rst : 1;
            uint32_t rsvd : 10;
            uint32_t unique_id : 16;
        } hwp_ctrl_bf;
    } hwp_ctrl;

    union {
        uint32_t hwp_cmx_mem_addr;
        struct {
            uint32_t mem_addr : 27;
            uint32_t rsvd : 5;
        } hwp_cmx_mem_addr_bf;
    } hwp_cmx_mem_addr;

    union {
        uint32_t odu_cast;
        struct {
            uint32_t cast_enable : 1;
            uint32_t reserved : 3;
            uint32_t cast_offset : 28;
        } odu_cast_bf;
    } odu_cast[3];

    uint32_t reserved0;

    union {
        uint32_t reserved1;
        struct {
            uint32_t reserved2 : 16;
            uint32_t unused_1 : 2;
            uint32_t reserved3 : 1;
            uint32_t unused_2 : 13;
        } reserved2;
    } reserved1;

    uint32_t nvar_tag;

    uint32_t pallet[8];

    uint32_t se_addr1;
    uint32_t sparsity_addr1;
    uint32_t se_addr2;
    uint32_t sparsity_addr2;
    uint32_t se_addr3;
    uint32_t sparsity_addr3;
    uint32_t se_sp_size1;
    uint32_t se_sp_size2;

} VpuDPUInvariantRegisters;

static_assert(sizeof(VpuDPUInvariantRegisters) == 288, "VpuDPUInvariantRegisters size != 288");

/* VPU$ HALO Support */
#define NUM_HALO_REGIONS 6

typedef struct {
    union {
        uint32_t halo_region_a;
        struct {
            uint32_t sp_adr_offset : 22;
            uint32_t tile_select : 7;
            uint32_t rsvd : 2;
            uint32_t enable : 1;
        } halo_region_a_bf;
    } halo_region_a;

    union {
        uint32_t halo_region_b;
        struct {
            uint32_t ac_adr_offset : 22;
            uint32_t target_width_lsb : 10;
        } halo_region_b_bf;
    } halo_region_b;

    union {
        uint32_t halo_region_c;
        struct {
            uint32_t begin_x : 13;
            uint32_t begin_y : 13;
            uint32_t target_width_msb : 4;
            uint32_t rsvd : 2;
        } halo_region_c_bf;
    } halo_region_c;

    union {
        uint32_t halo_region_d;
        struct {
            uint32_t end_x : 13;
            uint32_t end_y : 13;
            uint32_t rsvd : 6;
        } halo_region_d_bf;
    } halo_region_d;

} halo_region_t;

typedef struct {
    union {
        uint32_t invar_ptr;
        struct {
            uint32_t invar_ptr : 16;
            uint32_t var_tag : 16;
        } invar_ptr_bf;
    } invar_ptr;
    union {
        uint32_t workload_size0;
        struct {
            uint32_t workload_size_x : 14;
            uint32_t workload_size_y : 14;
            uint32_t unused : 4;
        } workload_size0_bf;
    } workload_size0;
    union {
        uint32_t workload_size1;
        struct {
            uint32_t workload_size_z : 14;
            uint32_t pad_count_up : 3;
            uint32_t pad_count_left : 3;
            uint32_t pad_count_down : 3;
            uint32_t pad_count_right : 3;
            uint32_t unused : 6;
        } workload_size1_bf;
    } workload_size1;
    union {
        uint32_t workload_start0;
        struct {
            uint32_t workload_start_x : 14;
            uint32_t workload_start_y : 14;
            uint32_t unused : 4;
        } workload_start0_bf;
    } workload_start0;
    union {
        uint32_t workload_start1;
        struct {
            uint32_t workload_start_z : 14;
            uint32_t unused : 18;
        } workload_start1_bf;
    } workload_start1;
    union {
        uint32_t offset_addr;
        struct {
            uint32_t nthw_ntk : 2;
            uint32_t bin_cfg : 1;
            uint32_t conv_cond : 1;
            uint32_t dense_se : 1;
            uint32_t idx_quad : 1;
            uint32_t swizzle_key : 3;
            uint32_t idu_mrm_clk_en : 1;
            uint32_t odu_clk_en : 1;
            uint32_t mpe_clk_en : 1;
            uint32_t ppe_clk_en : 1;
            uint32_t odu_stat_en : 1;
            uint32_t idu_stat_en : 1;
            uint32_t reserved_1 : 1;
            uint32_t odu_stat_clr_mode : 1;
            uint32_t idu_stat_clr_mode : 1;
            uint32_t reserved1 : 1;
            uint32_t shave_l2_cache_en : 1;
            uint32_t idu_dbg_en : 2;
            uint32_t reserved2 : 1;
            uint32_t reserved3 : 1;
            uint32_t reserved_2 : 3;
            uint32_t wt_swizzle_key : 3;
            uint32_t wt_swizzle_sel : 1;
            uint32_t reserved4 : 1;
        } offset_addr_bf;
    } offset_addr;

    union {
        uint32_t hwp_wload_id;
        struct {
            uint32_t wload_id : 16;
            uint32_t rsvd : 16;
        } hwp_wload_id_bf;
    } hwp_wload_id;

    union {
        uint32_t var_cfg;
        struct {
            uint32_t reserved_0 : 8;
            uint32_t reserved1 : 1;
            uint32_t invar_line_cnt_en : 1;
            uint32_t invar_line_cnt_cnt : 4;
            uint32_t invar_lptr_force : 1;
            uint32_t next_sram_job_valid : 1;
            uint32_t next_sram_job_addr : 16;
        } var_cfg_bf;
    } var_cfg;

    uint64_t cbarrier_lo;
    uint64_t cbarrier_hi;
    uint64_t pbarrier_lo;
    uint64_t pbarrier_hi;

    halo_region_t halo_region[NUM_HALO_REGIONS];

    union {
        uint32_t dpu_cfg;
        struct {
            uint32_t workload_start_odu : 1;
            uint32_t workload_start_idu : 1;
            uint32_t workload_prm_sel : 1;
            uint32_t workload_valid : 1;
            uint32_t workload_shad_odu : 1;
            uint32_t workload_shad_idu : 1;
            uint32_t workload_idu_auto_upd_0 : 1;
            uint32_t workload_idu_auto_upd_1 : 1;
            uint32_t workload_odu_auto_upd : 1;
            uint32_t cfg_Reserved_0 : 1;
            uint32_t cfg_Reserved_1 : 1;
            uint32_t cfg_Reserved_2 : 1;
            uint32_t rst_ctxt_new : 1;
            uint32_t cfg_Reserved_3 : 1;
            uint32_t cfg_Reserved_4 : 1;
            uint32_t odu_stat_clr : 1;
            uint32_t idu_stat_clr : 1;
            uint32_t cfg_Reserved_5 : 1;
            uint32_t cfg_Reserved_6 : 14;
        } dpu_cfg_bf;
    } dpu_cfg;
    union {
        uint32_t te_beg0;
        struct {
            uint32_t te_beg_y : 13;
            uint32_t te_beg_z : 13;
            uint32_t unused : 6;
        } te_beg0_bf;
    } te_beg0;
    union {
        uint32_t te_beg1;
        struct {
            uint32_t te_beg_x : 13;
            uint32_t unused : 19;
        } te_beg1_bf;
    } te_beg1;
    union {
        uint32_t te_end0;
        struct {
            uint32_t te_end_y : 13;
            uint32_t te_end_z : 13;
            uint32_t unused : 6;
        } te_end0_bf;
    } te_end0;
    union {
        uint32_t te_end1;
        struct {
            uint32_t te_end_x : 13;
            uint32_t unused : 19;
        } te_end1_bf;
    } te_end1;

    uint32_t weight_size;
    uint32_t weight_num;
    uint32_t weight_start;

} VpuDPUVariantRegisters;

static_assert(sizeof(VpuDPUVariantRegisters) == 192, "VpuDPUVariantRegisters size != 192");

#pragma pack(pop)

// HW related utility functions and enum values (enums correspond to HW register settings)

// base resources
constexpr uint32_t VPU_MAX_TILES = 6;
constexpr uint32_t VPU_DPU_PER_TILE = 1;
constexpr uint32_t VPU_SNN_PER_TILE = VPU_DPU_PER_TILE;
constexpr uint32_t VPU_SNN_TOTAL = VPU_SNN_PER_TILE * VPU_MAX_TILES;
constexpr uint32_t VPU_AS_PER_TILE = 2;
constexpr uint32_t VPU_AS_TOTAL = VPU_AS_PER_TILE * VPU_MAX_TILES;
constexpr uint32_t VPU_MAX_DMA_ENGINES = 2;

// NCE_DPU_HWP_CTRL dtype 2 bit field
enum class VpuHWPStatMode : uint8_t {
    // odu_time_stat, idu_time_stat
    MODE0 = 00,
    // dense_act, sparse_act, dense_wt, sparse_wt when odu done
    MODE1 = 01,
    // dense_wt, sparse_wt, idu_time_stat
    MODE2 = 02,
    // dense_act, sparse_act, idu_time_stat
    MODE3 = 03,
    // invalid
    INVALID_MODE = 04
};

enum class VpuActWLType : uint8_t {
    WL_KERNEL = 0x00,
    WL_DXIL_KERNEL,
    WL_DEBUG = 0x04,
    WL_CACHE_OP_FLUSH,
    WL_CACHE_OP_INVALIDATE,
    WL_CACHE_OP_FLUSHINV,
    WL_CACHE_OP_PREFETCH,
    WL_UNKNOWN
};

// NCE_DPU_TENSOR_MODE wmode&amode - IDU types for activation&weights
enum class VpuInputTensorDType : uint8_t {
    FP16 = 0x0,
    U8 = 0x01,
    I8 = 0x02,
    I4 = 0x03,
    I2 = 0x04,
    BF16 = 0x05,
    U4 = 0x06,
    BIN = 0x07,
    FP8 = 0x08,
    RESERVED = 0x09,
    INPUT_DTYPE_UNKNOWN
};

// NCE_DPU_MPE_CFG mpe_mode
enum class VpuMpeActivationWeightDtype : uint8_t {
    BIN = 0x7,
    I2 = 0x4,
    I2X = 0x6, // not used
    I4 = 0x3,
    I4X = 0x5, // not used
    I8 = 0x2,
    U8 = 0x1,
    FP16 = 0x0,
    MPE_DTYPE_UNKNOWN
};

// NCE_DPU_ODU_CFG  dtype 4 bit field
enum class VpuOutputTensorDType : uint8_t {
    BIN = 0x0,
    I2 = 0x1,
    I4 = 0x2,
    U4 = I4, // The U4 is same as I4 since NCE_DPU_ODU_CFG enums are based on dtype size
    G8 = 0x3,
    U8F = G8,
    I8 = G8,
    FP16 = 0x4,
    BF16 = FP16,
    I32 = 0x5,
    FP32 = I32, // The FP32 is same as I32 since NCE_DPU_ODU_CFG enums are based on dtype size
    LOG = I32,
    OUTPUT_DTYPE_UNKNOWN = 0x0A
};

enum VpuIDUNthw_Ntk {
    IDU_NTHW_NTK_8_8 = 0,
    IDU_NTHW_NTK_4_16 = 1,
    IDU_NTHW_NTK_UNUSED = 2,
    IDU_NTHW_NTK_16_4 = 3,
};

enum VpuODUGrid { ODU_GRID_4x4, ODU_GRID_16x1, ODU_GRID_4x1 };

enum VpuODUNthw {
    ODU_NTHW_1,
    ODU_NTHW_4,
    ODU_NTHW_8,
    ODU_NTHW_16,
};

enum VpuMPEGrid { MPE_GRID_4x4, MPE_GRID_16x1 };

} // namespace nn_public

#endif

// clang-format on
