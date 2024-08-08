/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2023, Intel Corporation.
 */

// clang-format off

#ifndef VPU_MEDIA_HW_H
#define VPU_MEDIA_HW_H

#if defined(_MSC_VER)
#define ALIGN_MEDIA(alignment) __declspec(align(alignment))
#elif defined(__GNUC__) || defined(__clang__)
#define ALIGN_MEDIA(alignment) __attribute__((aligned(alignment)))
#else
#error Define alignment macro
#endif

#define MAX_NUM_ROIS (7)
#define MEDIA_MAX_NUM_PLANES (3)

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 1)

typedef struct {
    uint64_t Coeff11 : 16;
    uint64_t Coeff12 : 16;
    uint64_t Coeff13 : 16;
    uint64_t Coeff21 : 16;
    uint64_t Coeff22 : 16;
    uint64_t Coeff23 : 16;
    uint64_t Coeff31 : 16;
    uint64_t Coeff32 : 16;
    uint64_t Coeff33 : 16;
    uint64_t CSCOff1 : 16;
    uint64_t CSCOff2 : 16;
    uint64_t CSCOff3 : 16;
    uint64_t RESERVED : 64; // Reserved register, do not write
} Media_ExtHeader_t;

typedef struct {
    uint64_t width : 16;  // Input width (Pixels)
    uint64_t height : 16; // Input height (Lines)
    uint64_t ls : 24;     // Line Stride
    uint64_t PID : 8;     // LS0 PID
} Media_InSize0_t;

typedef struct {
    uint64_t width : 16;   // Input width (Pixels)
    uint64_t height : 16;  // Input height (Lines)
    uint64_t ls : 24;      // Line Stride
    uint64_t HWPEN : 1;    // Enable bit for hardware profiling
    uint64_t ExtHDR : 1;   // Enable the extended header
    uint64_t RESERVED : 6; // Reserved register, do not write
} Media_InSize1_t;

typedef struct {
    uint64_t width : 16;   // Input width (Pixels)
    uint64_t height : 16;  // Input height (Lines)
    uint64_t ls : 24;      // Line Stride
    uint64_t RESERVED : 8; // Reserved register, do not write
} Media_InSize2_t;

typedef struct {
    uint64_t inFormat : 8;   // Input frame format
    uint64_t outFormat : 8;  // Output frame format
    uint64_t numRois : 16;   // Number of rois
    uint64_t sampleType : 4; // Sampling Type
    uint64_t operations : 4; // CSC / Norm / 10-bit packed / Reserved
    uint64_t IFC : 8;        // Input format config (input buffer format, chroma & luma input order)
    uint64_t IRQMask : 16;   // Interrupts control
} Media_IOCfg_t;

typedef struct {
    uint64_t NormFact0 : 16; // Normalization factor 0
    uint64_t NormFact1 : 16; // Normalization factor 1
    uint64_t NormFact2 : 16; // Normalization factor 2
    uint64_t NormFact3 : 16; // Normalization factor 3
} Media_NormFactor_t;

typedef struct {
    union {
        uint64_t roiBase_offset;           // Used by the compiler to get the offset of the roiBase field
        struct {
            uint64_t roiBase : 32;         // Base output address
            uint64_t outFormatLocal : 8;   // Output format local, optional
            uint64_t samlingTypeLocal : 4; // Local sampling type, optional
            uint64_t OFC : 8;              // Out format config, optional
            uint64_t IRQLocal : 8;         // Local IRQ defines, optional
            uint64_t HWProfEN : 1;         // Enable bit for hardware profiling of each ROI
            uint64_t RESERVED : 3;         // Reserved register, do not write
        };
    };
} Media_RoiDef_t;

typedef struct {
    uint64_t inPS : 32;      // plane stride
    uint64_t outBase : 16;   // Upper 16 bits of the output base address
    uint64_t HWPAddrLO : 16; // Hardware profiler LO address
} Media_PSOB_t;

typedef struct {
    uint64_t X_coord : 16;   // Roi X coordinate
    uint64_t Y_coord : 16;   // Roi Y coordiante
    uint64_t roiWidth : 16;  // Roi Width
    uint64_t roiHeight : 16; // Roi Height
} Media_RoiCfg_t;

typedef struct {
    uint64_t outScale0_width : 16;  // Scale0 output width
    uint64_t outScale0_height : 16; // Scale0 output height
    uint64_t outScale1_width : 16;  // Scale1 output width
    uint64_t outScale1_height : 16; // Scale1 output height
} Media_OutScaleSize_t;

typedef struct {
    uint64_t psSc0Y : 32; // Scale 0 Y plane stride
    uint64_t psSc1Y : 32; // Scale 1 Y plane stride
} Media_ScPSY_t;

typedef struct {
    uint64_t psSc0UV : 32; // Scale 0 UV plane stride
    uint64_t psSc1UV : 32; // Scale 1 UV plane stride
} Media_ScPSUV_t;

typedef struct {
    uint64_t lsSc0Y : 16;  // Output scale0 Y line stride
    uint64_t lsSc1Y : 16;  // Output scale1 Y line stride
    uint64_t lsSc0UV : 16; // Output scale0 UV line stride
    uint64_t lsSc1UV : 16; // Output scale1 UV line stride
} Media_OutLS_t;

typedef struct {
    uint64_t vSc_offset : 32; // Vertical scaler offset
    uint64_t hSc_offset : 32; // Horizontal scaler offset
} Media_ScOffset_t;

typedef struct {
    uint64_t vSc_factor : 32; // Vertical scaler factor
    uint64_t hSc_factor : 32; // Horizontal scaler factor
} Media_ScFactor_t;

// ROI descriptor
typedef struct {
    Media_RoiDef_t roiDef; // Base output address and format/sampling type/ IRQ mask configuration for each ROI
    Media_RoiCfg_t roiCfg; // Roi configuration (X/Y pos and width/height)
    Media_OutScaleSize_t outScaleSize; // Output scale 0/1 size config
    Media_ScPSY_t psScY;               // Scale 0/1 plane stride Y
    Media_ScPSUV_t psScUV;             // Scale 0/1 plane stride UV
    Media_OutLS_t outLS;               // Scale 0/1 Y and UV line stride
    Media_ScOffset_t ScOff;            // Scaler offsets
    Media_ScFactor_t ScFactor;         // Scaler factors
    uint64_t barGateMaskLO;            // Barrier gate mask bits
    uint64_t barGateMaskHI;            // Barrier gate mask bits
    uint64_t barUpdateLO;              // Barrier update mask bits
    uint64_t barUpdateHI;              // Barrier update mask bits
} VpuMediaROIDescriptor;

// Buffer descriptor
typedef struct ALIGN_MEDIA(16) {
    uint64_t inAddr0;                                    // Source base address plane 0 low
    Media_InSize0_t inSize0;                             // Input0 Width/Height/LS/PID
    uint64_t inAddr1;                                    // Source base address plane 1 low
    Media_InSize1_t inSize1;                             // Input1 Width/Height/LS
    uint64_t inAddr2;                                    // Source base address plane 2 low
    Media_InSize2_t inSize2;                             // Input2 Width/Height/LS
    Media_IOCfg_t IOCfg;                                 // In/Out buffer configs struct
    Media_NormFactor_t normFactor[MEDIA_MAX_NUM_PLANES]; // Normalization factor 0/1/2/3
    Media_PSOB_t PSOB;                                   // plane stride and out buffer (HI) config
    union {
        uint64_t nextDesc_offset;                         // Used by the compiler to get the offset of the nextDesc field
        struct {
            uint64_t nextDesc : 48;                      // next descriptor base address, NULL terminates list
            uint64_t HWPAddrHI : 16;                     // Hardware profiler HI address
        };
    };
} VpuMediaBuffDescriptor;

typedef Media_ExtHeader_t VpuMediaExtendedHeader;

static_assert(sizeof(VpuMediaROIDescriptor) == 96, "VpuMediaROIDescriptor size != 96");
static_assert(sizeof(VpuMediaBuffDescriptor) == 96, "VpuMediaBuffDescriptor size != 96");

#pragma pack(pop)

#ifdef __cplusplus
}
#endif

#endif

// clang-format on
