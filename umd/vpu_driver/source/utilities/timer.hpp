/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>
#include <chrono>

namespace VPU {

std::chrono::steady_clock::time_point getAbsoluteTimePoint(uint64_t userTimeout);

int64_t getAbsoluteTimeoutNanoseconds(uint64_t userTimeout);

} // namespace VPU
