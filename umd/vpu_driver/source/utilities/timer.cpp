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

template <typename F>
static bool timeBoundSignalCheck(uint64_t timeout, F checkSignalFunc) {
    LOG_V("Start signal observing checker for %ld nanoseconds.", timeout);

    bool enableTimeout = (timeout == std::numeric_limits<uint64_t>::max()) ? false : true;
    std::chrono::steady_clock::time_point timeStart, timeNow;
    auto chronoTimeout = std::chrono::nanoseconds(timeout);
    auto timeDiff = std::chrono::nanoseconds(0);

    // Protection against overflow, nanoseconds is int64_t.
    if (timeout > boost::numeric_cast<uint64_t>(std::chrono::nanoseconds::max().count())) {
        chronoTimeout = std::chrono::nanoseconds::max();
    }

    timeStart = std::chrono::steady_clock::now();
    while (timeDiff <= chronoTimeout) {
        if (checkSignalFunc()) {
            LOG_V("Response address has been signaled.");
            return true;
        }

        std::this_thread::yield();

        if (enableTimeout) {
            timeNow = std::chrono::steady_clock::now();
            timeDiff = std::chrono::duration_cast<std::chrono::nanoseconds>(timeNow - timeStart);
        }
    }

    LOG_W("Response address has not been signalled until the timeout %ld", timeout);
    return false;
}

bool waitForSignal(uint64_t timeout, std::function<bool()> check) {
    return timeBoundSignalCheck(timeout, check);
}

static int64_t getAbsoluteTimeoutNanoseconds(int64_t timeout) {
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

bool waitForSignal(uint64_t timeout,
                   const std::vector<std::shared_ptr<VPUJob>> &jobs,
                   VPUHwInfo devInfo) {
    if (devInfo.deviceId == mtlHwInfo.deviceId && devInfo.platformType != 0) {
        return timeBoundSignalCheck(timeout, [&jobs]() {
            for (auto const &job : jobs)
                if (!job->waitForCompletion(0))
                    return false;
            return true;
        });
    }

    int64_t t = getAbsoluteTimeoutNanoseconds(timeout > INT64_MAX ? INT64_MAX : timeout);
    for (auto const &job : jobs)
        if (!job->waitForCompletion(t))
            return false;
    return true;
}

} // namespace VPU
