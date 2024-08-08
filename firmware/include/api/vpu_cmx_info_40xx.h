/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2023, Intel Corporation.
 */

// clang-format off

#ifndef VPU_CMX_INFO_40XX_H_
#define VPU_CMX_INFO_40XX_H_

#include "vpu_nnrt_api_40xx.h"

namespace nn_public {

// SNN_DATA_SIZE + SNN_STACK_SIZE + ACTSHV_SCRATCH_SIZE + METADATA_SIZE should be multiple of 16K
// or else the workspace will not work with certain DPU operations - swizzling
// No SHAVE-NN for NPUReg40XX
constexpr static uint32_t VPU_SNN_DATA_SIZE = 0;
constexpr static uint32_t VPU_SNN_STACK_SIZE = 0;
constexpr static uint32_t VPU_ACTSHV_SCRATCH_SIZE = 1 * 1024;
constexpr static uint32_t VPU_ACTSHV_STACKS_SIZE = 14 * 1024;
constexpr static uint32_t VPU_METADATA_SIZE = 81 * 1024;
constexpr static uint32_t VPU_WORKSPACE_SIZE = 1440 * 1024;
constexpr static uint32_t VPU_RESERVED_SIZE = 512 * 1024;

static_assert(((VPU_SNN_DATA_SIZE + VPU_SNN_STACK_SIZE + VPU_ACTSHV_SCRATCH_SIZE + VPU_ACTSHV_STACKS_SIZE +
                VPU_METADATA_SIZE) &
               0x3FFF) == 0,
              "Workspace size must be multiple of 16k");

constexpr uint32_t VPU_ACTSHV_STACK_PER_SHAVE = VPU_ACTSHV_STACKS_SIZE / VPU_AS_PER_TILE;

// Kept here for backwards compatibility for the moment.
// Will be removed once transition to VpuTaskCounts is complete.
constexpr static uint32_t VPU_INVARIANT_COUNT = 64;
constexpr static uint32_t VPU_VARIANT_COUNT = 128;
constexpr static uint32_t VPU_KERNEL_RANGE_COUNT = 64;
constexpr static uint32_t VPU_KERNEL_INVO_COUNT = 64;
constexpr static uint32_t VPU_MEDIA_COUNT = 4;

// This will be split later into power-of-2 sized fragments
constexpr static uint32_t VPU_DMA_TASK_COUNT = 80;

constexpr uint32_t VPU_VIRTUAL_CMX0_BASE = 0x40200000;
constexpr uint32_t VPU_METADATA_OFFSET = VPU_ACTSHV_SCRATCH_SIZE + VPU_ACTSHV_STACKS_SIZE;
constexpr uint32_t VPU_WORKSPACE_OFFSET = VPU_METADATA_OFFSET + VPU_METADATA_SIZE;

constexpr uint32_t VPU_METADATA_STORAGE_ADDR = VPU_VIRTUAL_CMX0_BASE + VPU_METADATA_OFFSET;
constexpr uint32_t VPU_WORKSPACE_ADDR = VPU_VIRTUAL_CMX0_BASE + VPU_WORKSPACE_OFFSET;

// Alignment must be power of 2
constexpr uintptr_t align_storage(uint32_t alignment, uintptr_t ptr) {
    return (ptr + alignment - 1) & ~static_cast<uintptr_t>(alignment - 1);
};

// As part of the contract, task storage areas will be allocated in the following order:
//  1. Invariants
//  2. Variants
//  3. Kernel ranges
//  4. Kernel invocations
//  5. DMA (from DDR source)
//  6. DMA (from CMX source)
//  7. Media
//
// All areas respect the alignment requirements of the task type they store.
// If any of these areas is empty due to its task count being 0, the corresponding storage will take up 0 space, but
// storage pointer is still adjusted with the alignment of the respective task type, as defined by align_storage(...)

} // namespace nn_public

#endif

// clang-format on
