/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>

#include <chrono>
#include <level_zero/ze_api.h>
#include <memory>
#include <vector>

namespace VPU {
class VPUJob;
} // namespace VPU

struct _ze_fence_handle_t {};

namespace L0 {

struct CommandQueue;

struct Fence : _ze_fence_handle_t {
    Fence(CommandQueue *pCmdQueue, const ze_fence_desc_t *desc);
    ~Fence() = default;

    ze_result_t destroy();
    ze_result_t synchronize(uint64_t timeout);
    ze_result_t queryStatus() { return synchronize(0); }
    ze_result_t reset();

    static Fence *fromHandle(ze_fence_handle_t handle) { return static_cast<Fence *>(handle); }
    inline ze_fence_handle_t toHandle() { return this; }

    void setTrackedJobs(std::vector<std::shared_ptr<VPU::VPUJob>> jobs);
    ze_result_t waitForJobs(std::chrono::steady_clock::time_point absTimePoint);

  protected:
    CommandQueue *pCmdQueue = nullptr;
    bool signaled = false;
    std::vector<std::shared_ptr<VPU::VPUJob>> trackedJobs;
};

} // namespace L0
