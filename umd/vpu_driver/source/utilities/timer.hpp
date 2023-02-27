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
 * @brief Generic wait for given timeout until check function return true

 * @param timeout [in] A time out value in nano sec. Give max value for not time bound wait.
 * @param check [in] Function to verify the condition of success.
 * @return true The address has been signaled within given time.
 * @return false Otherwise.
 */
bool waitForSignal(uint64_t timeout, std::function<bool()> check);

/**
 * @brief Wait for given timeout until the jobs are all signalled by KMD.
 *
 * @param timeout [in] A time out value in nano sec. Give max value for not time bound wait.
 * @param jobs [in] Vector for submitted command buffers.
 * @return true All command buffers have been signaled within given time.
 * @return false Otherwise.
 */
bool waitForSignal(uint64_t timeout,
                   const std::vector<std::shared_ptr<VPUJob>> &jobs,
                   VPUHwInfo devInfo);
} // namespace VPU
