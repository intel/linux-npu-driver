/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>
#include <linux/kernel.h>
#include <stdexcept>

// VPU PCI Device ID definition
constexpr uint16_t MTL_VPU_PCI_DEVICE_ID = 0x7D1D;

constexpr uint16_t INTEL_PCI_VENDOR_ID = 0x8086;

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
