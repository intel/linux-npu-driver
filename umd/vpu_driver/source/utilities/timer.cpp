/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"

#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/timer.hpp"
#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"

#include <cstdint>
#include <memory>
#include <limits>
#include <chrono>
#include <thread>
#include <vector>
#include <uapi/drm/ivpu_accel.h>

namespace VPU {

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

bool waitForSignal(uint64_t userTimeout, const std::vector<std::shared_ptr<VPUJob>> &jobs) {
    int64_t absoluteTimeout = getAbsoluteTimeoutNanoseconds(userTimeout);
    for (auto const &job : jobs)
        if (!job->waitForCompletion(absoluteTimeout))
            return false;
    return true;
}

} // namespace VPU
