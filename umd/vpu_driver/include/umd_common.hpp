/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstdint>

#include <limits>
#include <linux/kernel.h>
#include <stdexcept>
#include <type_traits>
#include <typeinfo>

template <typename T>
bool checkPtrAlignment(const void *p) noexcept {
    return !(reinterpret_cast<std::uintptr_t>(p) % alignof(T));
};

template <class To, class From>
To safe_cast(From x) {
    static_assert(!std::is_floating_point_v<From>,
                  "safe_cast From floating point type is not supported");
    static_assert(!std::is_floating_point_v<To>,
                  "safe_cast To floating point type is not supported");

    if constexpr (std::is_signed_v<From> && std::is_signed_v<To>) {
        if constexpr (sizeof(From) > sizeof(To)) {
            if (x > std::numeric_limits<To>::max() || x < std::numeric_limits<To>::min()) {
                throw std::overflow_error(
                    "Failed to convert signed to signed: value is not in range");
            }
        }
    } else if constexpr (std::is_unsigned_v<From> && std::is_unsigned_v<To>) {
        if constexpr (sizeof(From) > sizeof(To)) {
            if (x > std::numeric_limits<To>::max()) {
                throw std::overflow_error(
                    "Failed to convert unsigned to unsigned: value is greater than max");
            }
        }
    } else if constexpr (std::is_unsigned_v<From> && std::is_signed_v<To>) {
        if constexpr (sizeof(From) >= sizeof(To)) {
            if (x > static_cast<uint64_t>(std::numeric_limits<To>::max())) {
                throw std::overflow_error(
                    "Failed to convert unsigned to signed: value is greater than max");
            }
        }
    } else if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>) {
        if (x < 0) {
            throw std::overflow_error("Failed to convert signed to unsigned: value is less than 0");
        }
        if constexpr (sizeof(From) > sizeof(To)) {
            if (x > std::numeric_limits<To>::max()) {
                throw std::overflow_error(
                    "Failed to convert signed to unsigned: value is greater than max");
            }
        }
    }
    return static_cast<To>(x);
}

#define ALIGN(x, a) __ALIGN_KERNEL((x), (a))

inline size_t getFwDataCacheAlign(size_t a) {
    return ALIGN(a, 64);
}
