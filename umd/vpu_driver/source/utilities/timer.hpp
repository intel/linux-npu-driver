/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <uapi/drm/ivpu_accel.h>

#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"

namespace VPU {

/**
 * @brief Wait for given timeout until the jobs are all completed by KMD.
 *
 * @param timeout [in] A time out value in nano sec. Give max value for not time bound wait.
 * @param jobs [in] Vector for submitted command buffers.
 * @return true All jobs are completed within given time.
 * @return false Otherwise.
 */
bool waitForSignal(uint64_t timeout, const std::vector<std::shared_ptr<VPUJob>> &jobs);

/**
 * @brief Return the absolute timeout to user timeout that is passed in nanoseconds.
 */
int64_t getAbsoluteTimeoutNanoseconds(uint64_t userTimeout);
} // namespace VPU
