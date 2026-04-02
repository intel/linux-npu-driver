/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2026, Intel Corporation.
 */

#ifndef VPU_NNRT_API_VER_H
#define VPU_NNRT_API_VER_H

/*
 * When an RT API change is made that breaks backwards compatibility (old blob does not
 * work with new firmware) VPU_NNRT_40XX_API_VER_MAJOR must be incremented.
 *
 * If a change preserves backwards compatibility but breaks forwards compatibility
 * (new blob does not work with old firmware) then VPU_NNRT_40XX_API_VER_MINOR
 * should be incremented. It resets to 0 when the major version is incremented.
 *
 * If vpu_nnrt_api_40xx.h is modified (field names, documentation, formatting) but the API
 * itself is not changed, then VPU_NNRT_40XX_API_VER_PATCH should be incremented.
 *
 * When the compiler creates a MappedInference in an ELF blob
 * VpuMappedInference.vpu_nnrt_api_ver is set to the version of the NNRT API used.
 * NNRuntime checks this version at inference time to ensure it is current and
 * returns an error if the major version does not match.
 * Note: VPU_NNRT_40XX_API_VER_PATCH is not stored in the MappedInference as
 * compatibility is not affected if this changes.
 */
#ifdef CONFIG_TARGET_SOC_7000
/*
 * API changelog
 * ------------
 * 12.4.1:
 *   - Replaced stack frame storage in VpuNNShaveRuntimeConfigs with a union,
 *     providing a VpuTaskReference<uint32_t> stack_frames_ref for NPU7+
 *
 * 12.4:
 *   - Added P_GPI_1/P_GPO_1 flow in management kernel
 *
 * 12.3:
 *   - Switched stacks to indexed mode to allow allocation in disjoint memory ranges.
 *
 * 12.2:
 *   - Moved 1KB Shave scratch region to the end of CMX and increased default CMX Shave stacks to 7.5kB per Shave.
 *
 * 12.1:
 *   - Increased VPU_MAX_DMA_ENGINES to 4 to support NPU7 hardware configuration.
 *
 * 12.0.1:
 *   - Added SAFE documentation for VpuBarrierConfiguration, VpuTaskReference, VpuResourceRequirements and
 * VpuHostParsedInference.
 *
 * 12.0:
 *   - Increased VPU_AS_PER_TILE to 4 and replaced stack_frames array with VpuTaskReference<uint32_t> for NPU7.
 */
// TODO: Replace with VPU_NNRT_70XX_API_VER_MAJOR for NPU7 (EISW-166400).
#define VPU_NNRT_40XX_API_VER_MAJOR 12
#define VPU_NNRT_40XX_API_VER_MINOR 4
#define VPU_NNRT_40XX_API_VER_PATCH 2
#else
/*
 * API changelog
 * ------------
 * 11.13.1:
 *   - Replaced stack frame storage in VpuNNShaveRuntimeConfigs with a union,
 *     providing a fixed-size array for NPU4-6.
 *
 * 11.13:
 *   - Accept CMX Shave stack frames from the blob
 * 11.12:
 *   - 1KB Shave scratch region is moved to the end of CMX,
 *     default CMX Shave stacks increased to 7.5kB per Shave
 * 11.11:
 *   - Add the ManagedMappedInference directly to the VpuHostParsedInference.
 *
 * 11.10.3:
 *   - update Field name from reserved_1 to noc_clk_en in DPU Descriptor.
 *
 * 11.10:
 *   - Increase the minor version number to uniquely identify the UD24 release from earlier versions
 *
 * 11.9:
 *   - Added VpuManagedMappedInference::model_identifier to enable the compiler to assign a unique identifier
 *     to an inference.
 *
 * 11.8.1:
 *   - Added VpuActKernelInvocation::invo_index to track the ID of enqueued ActShave workloads.
 *
 * 11.8
 *   - Added support for shave sub_unit selection for work items to allow compiler to specify
 *     (if it wants to) which shave is to execute a work item.
 *
 * 11.7:
 *   - Added VpuManagedMappedInference::inference_feature_cfg to allow the passing
 *     of additional information.
 *     Added disable_dma_sw_fifo_ to allow skipping the use of DMA SW FIFO
 *     when it is not required by an inference.
 *
 * 11.6:
 *   - Added VpuWorkItem::next_workitem_idx to allow a linked list of work items to be enqueued.
 *
 * 11.5:
 *   - Added barrier configuration data (barriers_configuration, num_of_barrier_reprogrammings,
 *     barrier_programming_mode and barrier_configuration_stride in VpuManagedMappedInference)
 *     to allow runtime to efficiently fill barrier FIFOs.
 */
#define VPU_NNRT_40XX_API_VER_MAJOR 11
#define VPU_NNRT_40XX_API_VER_MINOR 13
#define VPU_NNRT_40XX_API_VER_PATCH 1
#endif
#define VPU_NNRT_40XX_API_VER ((VPU_NNRT_40XX_API_VER_MAJOR << 16) | VPU_NNRT_40XX_API_VER_MINOR)

/* Index in the API version table, same for all HW generations */
#define VPU_NNRT_40XX_API_VER_INDEX 7

/*
 * When a change is made to the Activation Shave Runtime / Management kernel
 * (nnActEntry.cpp), that breaks backwards compatibility (e.g. changing the
 * nnActEntry function parameters) VPU_ACT_RT_VER_MAJOR must be incremented.
 *
 * If a change preserves backwards compatibility then VPU_ACT_RT_VER_MINOR
 * should be incremented. It resets to 0 when the major version is incremented.
 *
 * Act Runtime changelog:
 * ----------------------
 * 1.17:
 *  - Improved preemption handling
 *
 * 1.16:
 *  - Transition to one Shave cache op per Shave, removing the RISC communication path.
 *
 * 1.15:
 *  - Add cleanup function to clear specific registers at the beginning of shave entry
 *
 * 1.14:
 *  - Preemption handling fixes
 *
 * 1.13:
 *  - Window 1F reset to the beginning of CMX tile
 *
 * 1.12:
 *  - Cache operation fix
 *
 * 1.11:
 *  - Improve compatibility
 *
 * 1.10:
 *   - Support for executing shave tasks directly from DDR (expects two FIFO pushes
 *     with the full 32 bit AKI address and NW_PAGE is already correct)
 *
 * 1.9.2 (NPU4 only):
 *  - Preemption handling fixes
 *
 * 1.9.1 (NPU4 only):
 *  - Window 1F reset to the beginning of CMX tile
 *
 * 1.9:
 *   - Add NVL clock gating support
 *
 * 1.8:
 *   - Support Shave Shutdown control message
 *
 */
#if !defined(CONFIG_TARGET_SOC_5000)
#define VPU_ACT_RT_VER_MAJOR 1
#define VPU_ACT_RT_VER_MINOR 9
#define VPU_ACT_RT_VER_PATCH 3
#else
#define VPU_ACT_RT_VER_MAJOR 1
#define VPU_ACT_RT_VER_MINOR 17
#define VPU_ACT_RT_VER_PATCH 0
#endif
#define VPU_ACT_RT_VER ((VPU_ACT_RT_VER_MAJOR << 16) | VPU_ACT_RT_VER_MINOR)

#endif // VPU_NNRT_API_VER_H
