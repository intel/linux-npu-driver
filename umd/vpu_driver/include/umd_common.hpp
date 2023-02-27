/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <linux/kernel.h>
#include <stdexcept>

template <class To, class From>
To safe_cast(From x) {
    if (x != static_cast<From>(static_cast<To>(x)))
        throw std::overflow_error("Overflow in safe_cast");
    return static_cast<To>(x);
}

#define ALIGN(x, a) __ALIGN_KERNEL((x), (a))

inline size_t getFwDataCacheAlign(size_t a) {
    return ALIGN(a, 64);
}
