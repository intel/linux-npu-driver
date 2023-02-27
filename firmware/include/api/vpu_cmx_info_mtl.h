/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020-2023 Intel Corporation
 */

// Current file does not contain all CMX addresses used by the firmware,
// the code locations will be adjusted/extended to align with the Runtime view of memory locators"

#pragma once

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

constexpr uint32_t DMA_METADATA_SIZE = VPU_DMA_TASK_COUNT * sizeof(VpuDMATask);
constexpr uint32_t INV_METADATA_SIZE = VPU_INVARIANT_COUNT * sizeof(VpuDPUInvariant);
constexpr uint32_t AKR_METADATA_SIZE = VPU_KERNEL_RANGE_COUNT * sizeof(VpuActKernelRange);

constexpr uint32_t METADATA_OFFSET =
    VPU_SNN_DATA_SIZE + VPU_SNN_STACK_SIZE + VPU_ACTSHV_SCRATCH_SIZE + VPU_ACTSHV_STACKS_SIZE;
constexpr uint32_t DMA_METADATA_OFFSET = METADATA_OFFSET;
constexpr uint32_t INV_METADATA_OFFSET = DMA_METADATA_OFFSET + DMA_METADATA_SIZE;
constexpr uint32_t AKR_METADATA_OFFSET = INV_METADATA_OFFSET + INV_METADATA_SIZE;

constexpr uint32_t WORKSPACE_OFFSET = METADATA_OFFSET + VPU_METADATA_SIZE;
static_assert((WORKSPACE_OFFSET + VPU_WORKSPACE_SIZE) == VPU_CMX_SLICE_LENGTH, "CMX segmentation error");

constexpr uint32_t CONFIG_1_TILE_INV_METADATA_ADDR_0 = VPU_NN_SLICE0_CMX_BASE + INV_METADATA_OFFSET;
constexpr uint32_t CONFIG_1_TILE_AKR_METADATA_ADDR_0 = VPU_NN_SLICE0_CMX_BASE + AKR_METADATA_OFFSET;
constexpr uint32_t CONFIG_1_TILE_INV_METADATA_ADDR_1 = VPU_NN_SLICE1_CMX_BASE + INV_METADATA_OFFSET;
constexpr uint32_t CONFIG_1_TILE_AKR_METADATA_ADDR_1 = VPU_NN_SLICE1_CMX_BASE + AKR_METADATA_OFFSET;

constexpr uint32_t CONFIG_2_TILES_INV_METADATA_ADDR = VPU_NN_SLICE0_CMX_BASE + INV_METADATA_OFFSET;
constexpr uint32_t CONFIG_2_TILES_AKR_METADATA_ADDR = VPU_NN_SLICE0_CMX_BASE + AKR_METADATA_OFFSET;

constexpr uint32_t DMA0_STORAGE_ADDR = VPU_NN_SLICE0_CMX_BASE + DMA_METADATA_OFFSET;
constexpr uint32_t DMA1_STORAGE_ADDR = VPU_NN_SLICE1_CMX_BASE + DMA_METADATA_OFFSET;

constexpr uint32_t WORKSPACE_ADDR_0 = VPU_NN_SLICE0_CMX_BASE + WORKSPACE_OFFSET;
constexpr uint32_t WORKSPACE_ADDR_1 = VPU_NN_SLICE1_CMX_BASE + WORKSPACE_OFFSET;

} // namespace nn_public
