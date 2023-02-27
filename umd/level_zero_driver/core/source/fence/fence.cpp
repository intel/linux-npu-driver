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

Fence::Fence(CommandQueue *cmdQueue, const ze_fence_desc_t *desc)
    : cmdQueue(cmdQueue) {
    if (desc->flags & ZE_FENCE_FLAG_SIGNALED)
        signaled = true;
}

ze_result_t Fence::destroy() {
    delete this;
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

    if (cmdQueue == nullptr)
        return ZE_RESULT_ERROR_UNINITIALIZED;

    auto device = cmdQueue->getDevice();
    if (device == nullptr)
        return ZE_RESULT_ERROR_DEVICE_LOST;

    auto vpuDevice = device->getVPUDevice();
    if (vpuDevice == nullptr)
        return ZE_RESULT_ERROR_DEVICE_LOST;

    bool allSignaled = waitForSignal(timeout, trackedJobs, vpuDevice->getHwInfo());
    if (!allSignaled) {
        LOG_W("Commands execution is not finished");
        return ZE_RESULT_NOT_READY;
    }

    ze_result_t result = ZE_RESULT_SUCCESS;
    for (const auto &job : trackedJobs) {
        if (!job->isSuccess()) {
            result = ZE_RESULT_ERROR_UNKNOWN;
        }
    }

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
    LOG_V("%zu sync jobs copied", trackedJobs.size());
}

} // namespace L0
