/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2024, Intel Corporation.
 */

#ifndef VPU_NNRT_COMMON_H
#define VPU_NNRT_COMMON_H

/*
 * IMPORTANT:
 *
 * In order to guarantee that layout of structures will be the same
 * on different compilers and platforms the following means are used:
 *
 * 1. pack(1) enabled to disallow compiler to add any padding.
 *    Padding has been added manually to align every member to its
 *    natural alignment.
 * 2. Required alignment for struct as a whole is set explicitly -
 *    structures are aligned to the biggest alignment of all its members.
 *
 * In case of changing any of the structures - make sure that the data alignment is kept:
 *
 * 1. Struct alignment should be at least the size of the widest struct member.
 * 2. Struct size should be a multiple of its alignment, use padding as necessary at the end of the struct.
 * 3. The offset of each struct member should be a multiple of it's natural alignment, e.g. the offset of a uint64_t
 *    member should be a multiple of 8.
 */

#if defined(_MSC_VER)
#define VPU_ALIGNED_STRUCT(alignment) __declspec(align(alignment))
#elif defined(__GNUC__) || defined(__clang__)
#define VPU_ALIGNED_STRUCT(alignment) __attribute__((aligned(alignment)))
#else
#error Define alignment macro
#endif

namespace nn_public {

// base resources
constexpr uint32_t VPU_MAX_TILES = 6;
constexpr uint32_t VPU_BARRIERS_PER_GROUP = 16;
constexpr uint32_t VPU_DPU_PER_TILE = 1;
constexpr uint32_t VPU_SNN_PER_TILE = VPU_DPU_PER_TILE;
constexpr uint32_t VPU_SNN_TOTAL = VPU_SNN_PER_TILE * VPU_MAX_TILES;
constexpr uint32_t VPU_AS_PER_TILE = 2;
constexpr uint32_t VPU_AS_TOTAL = VPU_AS_PER_TILE * VPU_MAX_TILES;
constexpr uint32_t VPU_MAX_DMA_ENGINES = 2;

#pragma pack(push, 1)

template <typename T>
struct VPU_ALIGNED_STRUCT(8) VpuTaskReference {
    uint64_t reserved1;
    uint64_t reserved2;
    uint64_t reserved3;

    uint64_t address;
    uint64_t count;

    T *data() { return reinterpret_cast<T *>(address); }
    const T *data() const { return reinterpret_cast<T *>(address); }

    T *data(int64_t offset) { return reinterpret_cast<T *>(address + offset); }
    const T *data(int64_t offset) const { return reinterpret_cast<T *>(address + offset); }

    uint64_t size() const { return count; };

    T &at(uint32_t index, int64_t offset = 0) { return (reinterpret_cast<T *>(address + offset))[index]; }
    const T &at(uint32_t index, int64_t offset = 0) const { return (reinterpret_cast<T *>(address + offset))[index]; }
};

static_assert(sizeof(VpuTaskReference<uint32_t>) == 40, "VpuTaskReference size != 40");

struct VPU_ALIGNED_STRUCT(8) VpuNNShaveRuntimeConfigs {
    uint64_t reserved;
    uint64_t runtime_entry; // when useScheduleEmbeddedRt = true this is a windowed address
    uint64_t act_rt_window_base;
    uint32_t stack_frames[VPU_AS_TOTAL];
    uint32_t stack_size;
    uint32_t code_window_buffer_size;
    uint32_t perf_metrics_mask;
    uint32_t runtime_version;
    uint8_t use_schedule_embedded_rt; // when useScheduleEmbeddedRt = false; FW copies ActRt to this buffer
                                      // when useScheduleEmbeddedRt = true; buffer already contains the ActRt
    VpuHWPStatMode dpu_perf_mode;
    uint8_t pad_[6];
};

static_assert(sizeof(VpuNNShaveRuntimeConfigs) == 96, "VpuNNShaveRuntimeConfigs size != 96");

#pragma pack(pop)

} // namespace nn_public

#endif