/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/timer.hpp"

namespace VPU {

std::chrono::steady_clock::time_point getAbsoluteTimePoint(uint64_t userTimeout) {
    int64_t absTimeout = getAbsoluteTimeoutNanoseconds(userTimeout);
    return std::chrono::steady_clock::time_point(std::chrono::nanoseconds(absTimeout));
}

int64_t getAbsoluteTimeoutNanoseconds(uint64_t userTimeout) {
    int64_t timeout = userTimeout > INT64_MAX ? INT64_MAX : static_cast<int64_t>(userTimeout);

    std::chrono::steady_clock::time_point startTimePoint = std::chrono::steady_clock::now();

    int64_t timeout_abs_ns = std::chrono::nanoseconds::max().count();

    if (!(timeout >= std::chrono::nanoseconds::max().count())) {
        if (!(timeout + std::chrono::duration_cast<std::chrono::nanoseconds>(
                            startTimePoint.time_since_epoch())
                            .count() >=
              std::chrono::nanoseconds::max().count())) {
            timeout_abs_ns =
                (startTimePoint + std::chrono::nanoseconds(timeout)).time_since_epoch().count();
        }
    }

    return timeout_abs_ns;
}

} // namespace VPU
