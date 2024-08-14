/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

// IWYU pragma: no_include <bits/chrono.h>

#include <stdint.h>

#include <chrono> // IWYU pragma: keep

namespace VPU {

std::chrono::steady_clock::time_point getAbsoluteTimePoint(uint64_t userTimeout);

int64_t getAbsoluteTimeoutNanoseconds(uint64_t userTimeout);

} // namespace VPU
