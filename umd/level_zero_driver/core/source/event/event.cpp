/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/event/event.hpp"
#include "level_zero_driver/core/source/device/device.hpp"

#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/timer.hpp"

#include <level_zero/ze_api.h>
#include <thread>

namespace L0 {

Event::Event(VPU::VPUDeviceContext *ctx,
             VPU::VPUEventCommand::KMDEventDataType *ptr,
             uint64_t vpuAddr,
             std::function<void()> &&destroyCb)
    : pDevCtx(ctx)
    , eventState(ptr)
    , eventVpuAddr(vpuAddr)
    , destroyCb(std::move(destroyCb)) {
    setEventState(VPU::VPUEventCommand::STATE_EVENT_INITIAL);
}

ze_result_t Event::destroy() {
    destroyCb();
    LOG_I("Event destroyed - %p", this);
    return ZE_RESULT_SUCCESS;
}

ze_result_t Event::hostSignal() {
    setEventState(VPU::VPUEventCommand::STATE_HOST_SIGNAL);
    return ZE_RESULT_SUCCESS;
}

void Event::trackMetricData(int64_t timeoutNs) {
    auto timeOut = std::chrono::steady_clock::time_point(std::chrono::nanoseconds(timeoutNs));

    if (!msExpectedDataSize || !msGroupMask)
        return;

    do {
        size_t dataSize = 0;
        if (MetricStreamer::getData(pDevCtx->getDriverApi(), msGroupMask, dataSize, nullptr) ==
            ZE_RESULT_SUCCESS) {
            if (dataSize >= msExpectedDataSize) {
                hostSignal();
                break;
            }
        }
        std::this_thread::yield();
    } while (std::chrono::steady_clock::now() < timeOut);
}

ze_result_t Event::hostSynchronize(uint64_t timeout) {
    auto absoluteTimeout = VPU::getAbsoluteTimeoutNanoseconds(timeout);

    /* Remove dangling weak pointers */
    associatedJobs.erase(std::remove_if(associatedJobs.begin(),
                                        associatedJobs.end(),
                                        [](auto x) { return x.use_count() == 0; }),
                         associatedJobs.end());

    LOG_I("Waiting for fence in VPUAddr: %#lx", eventVpuAddr);

    for (auto &jobWeak : associatedJobs) {
        if (auto job = jobWeak.lock()) {
            for (const auto &cmdBuffer : job->getCommandBuffers()) {
                if (cmdBuffer->getFenceAddr() == eventVpuAddr) {
                    // TODO: Add check for ABORTED status from command buffer completion
                    if (!cmdBuffer->waitForCompletion(absoluteTimeout)) {
                        LOG_E("Associated command buffer is still in execution!\n");
                    }
                }
            }
        }
    }

    return queryStatus(absoluteTimeout);
}

ze_result_t Event::queryStatus(uint64_t timeout) {
    if (msExpectedDataSize && *eventState < VPU::VPUEventCommand::STATE_DEVICE_SIGNAL)
        trackMetricData(timeout);

    switch (*eventState) {
    case VPU::VPUEventCommand::STATE_EVENT_INITIAL:
        LOG_V("Sync point %p is still in initial state.", eventState);
        return ZE_RESULT_NOT_READY;
    case VPU::VPUEventCommand::STATE_HOST_RESET:
        LOG_V("Sync point %p has been resetted by host.", eventState);
        return ZE_RESULT_NOT_READY;
    case VPU::VPUEventCommand::STATE_DEVICE_RESET:
        LOG_V("Sync point %p has ben resetted by device.", eventState);
        return ZE_RESULT_NOT_READY;
    case VPU::VPUEventCommand::STATE_HOST_SIGNAL:
        LOG_V("Sync point %p has been signaled by host.", eventState);
        return ZE_RESULT_SUCCESS;
    case VPU::VPUEventCommand::STATE_DEVICE_SIGNAL:
        LOG_V("Sync point %p has been signaled by device.", eventState);
        return ZE_RESULT_SUCCESS;
    default:
        LOG_E("Unexpected sync value. (%lx)", *eventState);
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }
}

ze_result_t Event::reset() {
    setEventState(VPU::VPUEventCommand::STATE_HOST_RESET);
    return ZE_RESULT_SUCCESS;
}

void Event::setEventState(VPU::VPUEventCommand::KMDEventDataType updateTo) {
    if (*eventState == updateTo) {
        LOG_W("The sync pointer(%p) already in target status(%lu).", eventState, updateTo);
    } else {
        LOG_V("Sync updated from %#lx to %#lx.", *eventState, updateTo);
        *eventState = updateTo;
    }
}

} // namespace L0
