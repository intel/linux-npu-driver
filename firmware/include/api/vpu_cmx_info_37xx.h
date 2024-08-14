/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2023, Intel Corporation.
 */

// clang-format off

// Current file does not contain all CMX addresses used by the firmware,
// the code locations will be adjusted/extended to align with the Runtime view of memory locators"

#ifndef VPU_CMX_INFO_37XX_H_
#define VPU_CMX_INFO_37XX_H_

#include "vpu_nnrt_api_37xx.h"

namespace nn_public {

// Segment sizes in CMX
// VPU_SNN_DATA_SIZE + VPU_SNN_STACK_SIZE + VPU_ACTSHV_SCRATCH_SIZE + VPU_ACTSHV_STACKS_SIZE + VPU_METADATA_SIZE should
// be multiple of 16K or else the workspace will not work with certain DPU operations - swizzling
constexpr uint32_t VPU_SNN_DATA_SIZE = 1024;
constexpr uint32_t VPU_SNN_STACK_SIZE = 1024;
constexpr uint32_t VPU_ACTSHV_SCRATCH_SIZE = 1024;
constexpr uint32_t VPU_ACTSHV_STACKS_SIZE = 14 * 1024;
constexpr uint32_t VPU_METADATA_SIZE = 63 * 1024;
constexpr uint32_t VPU_WORKSPACE_SIZE = 1968 * 1024;

constexpr uint32_t VPU_ACTSHV_STACK_PER_SHAVE = VPU_ACTSHV_STACKS_SIZE / VPU_AS_PER_TILE;

// These should be powers of two - the TaskLocator will round down to next smallest
// power of two internally otherwise so anything else is misleading
constexpr uint32_t VPU_DMA_TASK_COUNT = 256;
constexpr uint32_t VPU_INVARIANT_COUNT = 32;
constexpr uint32_t VPU_VARIANT_COUNT = 256;
constexpr uint32_t VPU_KERNEL_RANGE_COUNT = 32;
constexpr uint32_t VPU_KERNEL_INVO_COUNT = 64;

constexpr uint32_t METADATA_OFFSET =
    VPU_SNN_DATA_SIZE + VPU_SNN_STACK_SIZE + VPU_ACTSHV_SCRATCH_SIZE + VPU_ACTSHV_STACKS_SIZE;
constexpr uint32_t METADATA0_STORAGE_ADDR = VPU_NN_SLICE0_CMX_BASE + METADATA_OFFSET;
constexpr uint32_t METADATA1_STORAGE_ADDR = VPU_NN_SLICE1_CMX_BASE + METADATA_OFFSET;

constexpr uint32_t VPU_WORKSPACE_OFFSET = METADATA_OFFSET + VPU_METADATA_SIZE;
static_assert((VPU_WORKSPACE_OFFSET + VPU_WORKSPACE_SIZE) == VPU_CMX_SLICE_LENGTH, "CMX segmentation error");

constexpr uint32_t VPU_WORKSPACE_ADDR_0 = VPU_NN_SLICE0_CMX_BASE + VPU_WORKSPACE_OFFSET;
constexpr uint32_t VPU_WORKSPACE_ADDR_1 = VPU_NN_SLICE1_CMX_BASE + VPU_WORKSPACE_OFFSET;

struct VpuMetadataMapSingle {
    VpuDMATask dma_storage[VPU_DMA_TASK_COUNT];
    VpuDPUInvariant inv_storage[VPU_INVARIANT_COUNT];
    VpuActKernelRange akr_storage[VPU_KERNEL_RANGE_COUNT];
    VpuDPUVariant var_storage[VPU_VARIANT_COUNT];
    VpuActKernelInvocation aki_storage[VPU_KERNEL_INVO_COUNT];
    VpuDescriptorWrapper desc_storage[VPU_NUM_COMPONENT_FEEDERS];
};
static_assert(sizeof(VpuMetadataMapSingle) <= VPU_METADATA_SIZE, "Metadata storage exceeded");

struct VpuMetadataMapDual0 {
    VpuDMATask dma0_storage[VPU_DMA_TASK_COUNT];
    VpuDPUInvariant inv_storage[VPU_INVARIANT_COUNT];
    VpuActKernelRange akr_storage[VPU_KERNEL_RANGE_COUNT];
    VpuDescriptorWrapper desc_storage[VPU_NUM_COMPONENT_FEEDERS];
};
static_assert(sizeof(VpuMetadataMapDual0) <= VPU_METADATA_SIZE, "Metadata storage exceeded");

struct VpuMetadataMapDual1 {
    VpuDMATask dma1_storage[VPU_DMA_TASK_COUNT];
    VpuDPUVariant var_storage[VPU_VARIANT_COUNT];
    VpuActKernelInvocation aki_storage[VPU_KERNEL_INVO_COUNT];
};
static_assert(sizeof(VpuMetadataMapDual1) <= VPU_METADATA_SIZE, "Metadata storage exceeded");

} // namespace nn_public

#endif

// clang-format on
