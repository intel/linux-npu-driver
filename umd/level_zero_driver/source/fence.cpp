/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "fence.hpp"

#include "cmdqueue.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/timer.hpp"

#include <level_zero/ze_api.h>
#include <utility>

namespace L0 {

Fence::Fence(CommandQueue *pCmdQueue, const ze_fence_desc_t *desc)
    : pCmdQueue(pCmdQueue) {
    if (desc->flags & ZE_FENCE_FLAG_SIGNALED)
        signaled = true;
}

ze_result_t Fence::destroy() {
    LOG(FENCE, "Fence destroy - %p", this);
    pCmdQueue->destroyFence(this);
    return ZE_RESULT_SUCCESS;
}

ze_result_t Fence::synchronize(uint64_t timeout) {
    LOG(FENCE, "Fence synchronize - %p", this);
    if (signaled)
        return ZE_RESULT_SUCCESS;

    if (trackedJobs.empty())
        return ZE_RESULT_NOT_READY;

    auto absTp = VPU::getAbsoluteTimePoint(timeout);
    return waitForJobs(absTp);
}

ze_result_t Fence::reset() {
    LOG(FENCE, "Fence reset - %p", this);
    signaled = false;
    return ZE_RESULT_SUCCESS;
}

void Fence::setTrackedJobs(std::vector<std::shared_ptr<VPU::VPUJob>> jobs) {
    if (jobs.empty())
        signaled = true;
    trackedJobs = std::move(jobs);
}

ze_result_t Fence::waitForJobs(std::chrono::steady_clock::time_point absTimePoint) {
    if (trackedJobs.empty())
        return ZE_RESULT_SUCCESS;

    ze_result_t result = pCmdQueue->waitForJobs(absTimePoint, trackedJobs);
    if (result != ZE_RESULT_SUCCESS)
        return result;

    trackedJobs.clear();
    signaled = true;

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
