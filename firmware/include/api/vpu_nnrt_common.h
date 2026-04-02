/* SPDX-License-Identifier: MIT */
/*
 * Copyright (c) 2022-2026, Intel Corporation.
 */

/**
 * @file
 * @brief Contains VpuTaskReference used in loader API and in the mapped inference.
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

/**
 * @addtogroup NNRT
 * @{
 */

namespace nn_public {

#pragma pack(push, 1)

/**
 * @brief Holds the start address and count of an array of one or more objects of type T
 */
template <typename T>
struct VPU_ALIGNED_STRUCT(8) VpuTaskReference {
    uint64_t reserved1;
    uint64_t reserved2;
    uint64_t reserved3;

    /**
     * @brief Address of the first T in the array.
     */
    uint64_t address;
    /**
     * @brief Number of elements of T in the array.
     */
    uint64_t count;

    T *data() { return reinterpret_cast<T *>(address); }
    const T *data() const { return reinterpret_cast<T *>(address); }

    T *data(int64_t offset) { return reinterpret_cast<T *>(address + offset); }
    const T *data(int64_t offset) const { return reinterpret_cast<T *>(address + offset); }

    uint64_t size() const { return count; };

    T &at(uint32_t index, int64_t offset = 0) { return (reinterpret_cast<T *>(address + offset))[index]; }
    const T &at(uint32_t index, int64_t offset = 0) const { return (reinterpret_cast<T *>(address + offset))[index]; }

    bool is_valid() const {
        // Check if address is non-null and aligned to the natural alignment of T
        return (address != 0 && !(address & (alignof(T) - 1)));
    }
};

static_assert(sizeof(VpuTaskReference<uint32_t>) == 40, "VpuTaskReference size != 40");

/**
 * @brief VpuTaskReference specialization for void type
 *
 * Identical layout. Used for opaque indirection with deferred type resolution
 * @see VpuTaskReference
 */
template <>
struct VPU_ALIGNED_STRUCT(8) VpuTaskReference<void> {
    uint64_t reserved1;
    uint64_t reserved2;
    uint64_t reserved3;
    uint64_t address;
    uint64_t count;

    void *data() { return reinterpret_cast<void *>(address); }
    const void *data() const { return reinterpret_cast<void *>(address); }

    uint64_t size() const { return count; };

    template <typename T>
    VpuTaskReference<T> &as() {
        return reinterpret_cast<VpuTaskReference<T> &>(*this);
    }
    template <typename T>
    const VpuTaskReference<T> &as() const {
        return reinterpret_cast<const VpuTaskReference<T> &>(*this);
    }
};

static_assert(sizeof(VpuTaskReference<void>) == 40, "VpuTaskReference size != 40");

#pragma pack(pop)

} // namespace nn_public

/**
 * close the "addtogroup NNRT" block
 * @}
 */

#endif