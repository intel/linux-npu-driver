/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/timer.hpp"

#include <time.h>

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW CLOCK_MONOTONIC
#endif

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

bool getHostTimestamp(uint64_t *hostTimestamp) {
    struct timespec rawHostTs;

    if (!hostTimestamp) {
        return false;
    }

    if (clock_gettime(CLOCK_MONOTONIC_RAW, &rawHostTs) != 0) {
        return false;
    }
    constexpr uint64_t NSEC_PER_SEC = 1'000'000'000;
    *hostTimestamp = (static_cast<uint64_t>(rawHostTs.tv_sec) * NSEC_PER_SEC) +
                     static_cast<uint64_t>(rawHostTs.tv_nsec);
    return true;
}

} // namespace VPU
