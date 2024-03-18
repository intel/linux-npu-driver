/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/fence/fence.hpp"
#include "vpu_driver/source/utilities/timer.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace L0 {

Fence::Fence(Context *pContext, const ze_fence_desc_t *desc)
    : pContext(pContext) {
    if (desc->flags & ZE_FENCE_FLAG_SIGNALED)
        signaled = true;
}

ze_result_t Fence::destroy() {
    pContext->removeObject(this);
    LOG_I("Fence destroyed");
    return ZE_RESULT_SUCCESS;
}

ze_result_t Fence::hostSynchronize(uint64_t timeout) {
    LOG_V("Fence status: %d", signaled);
    if (signaled)
        return ZE_RESULT_SUCCESS;

    if (trackedJobs.empty()) {
        LOG_E("Fence does not have any jobs to track");
        return ZE_RESULT_NOT_READY;
    }

    LOG_V("Synchronize for %lu ns, %zu jobs count", timeout, trackedJobs.size());

    auto absTimeout = VPU::getAbsoluteTimeoutNanoseconds(timeout);
    for (auto const &job : trackedJobs) {
        if (!job->waitForCompletion(absTimeout)) {
            LOG_W("Commands execution is not finished");
            return ZE_RESULT_NOT_READY;
        }
    }

    ze_result_t result = Device::jobStatusToResult(trackedJobs);

    trackedJobs.clear();
    signaled = true;
    LOG_V("Commands execution is finished");
    return result;
}

ze_result_t Fence::reset() {
    LOG_V("Reset the fence");
    signaled = false;
    return ZE_RESULT_SUCCESS;
}

void Fence::setTrackedJobs(std::vector<std::shared_ptr<VPU::VPUJob>> &jobs) {
    trackedJobs = jobs;
    if (!trackedJobs.size())
        signaled = true;
    LOG_V("%zu sync jobs copied", trackedJobs.size());
}

} // namespace L0
