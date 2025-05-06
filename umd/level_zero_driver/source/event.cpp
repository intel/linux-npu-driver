/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include <bits/chrono.h>

#include "event.hpp"

#include "metric.hpp"
#include "metric_streamer.hpp"
#include "vpu_driver/source/command/vpu_command_buffer.hpp"
#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/timer.hpp"

#include <algorithm>
#include <chrono> // IWYU pragma: keep
#include <level_zero/ze_api.h>
#include <thread>

namespace VPU {
class VPUBufferObject;
}

namespace L0 {

Event::Event(VPU::VPUDeviceContext *ctx,
             VPU::VPUEventCommand::KMDEventDataType *ptr,
             const std::shared_ptr<VPU::VPUBufferObject> eventBaseBo,
             uint64_t vpuAddr,
             std::function<void()> &&destroyCb)
    : pDevCtx(ctx)
    , eventState(ptr)
    , eventBase(std::move(eventBaseBo))
    , eventVpuAddr(vpuAddr)
    , destroyCb(std::move(destroyCb)) {
    setEventState(VPU::VPUEventCommand::STATE_EVENT_INITIAL);
}

ze_result_t Event::destroy() {
    destroyCb();
    LOG(EVENT, "Event destroyed - %p", this);
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
        if (*eventState >= VPU::VPUEventCommand::STATE_HOST_SIGNAL) {
            break;
        }
        if (MetricStreamer::getData(pDevCtx->getDriverApi(), msGroupMask, dataSize, nullptr) ==
            ZE_RESULT_SUCCESS) {
            if (dataSize >= msExpectedDataSize) {
                hostSignal();
                break;
            }
        } else {
            LOG_W("Metric data not available");
            break;
        }
        std::this_thread::sleep_for(
            std::chrono::nanoseconds(MetricContext::MIN_SAMPLING_RATE_NS / 2));
    } while (std::chrono::steady_clock::now() < timeOut);
}

ze_result_t Event::hostSynchronize(uint64_t timeout) {
    auto absoluteTimeout = VPU::getAbsoluteTimeoutNanoseconds(timeout);

    /* Remove dangling weak pointers */
    associatedJobs.erase(std::remove_if(associatedJobs.begin(),
                                        associatedJobs.end(),
                                        [](auto x) { return x.use_count() == 0; }),
                         associatedJobs.end());

    LOG(EVENT, "Waiting for fence in VPUAddr: %#lx", eventVpuAddr);

    for (auto &jobWeak : associatedJobs) {
        if (auto job = jobWeak.lock()) {
            for (const auto &cmdBuffer : job->getCommandBuffers()) {
                if (cmdBuffer->getFenceAddr() == eventVpuAddr) {
                    // TODO: Add check for ABORTED status from command buffer completion
                    if (!cmdBuffer->waitForCompletion(absoluteTimeout)) {
                        LOG_E("Associated command buffer is still in execution!");
                    }
                }
            }
        }
    }

    return queryStatus(absoluteTimeout);
}

ze_result_t Event::queryStatus(int64_t timeout) {
    if (msExpectedDataSize && *eventState < VPU::VPUEventCommand::STATE_DEVICE_SIGNAL)
        trackMetricData(timeout);

    switch (*eventState) {
    case VPU::VPUEventCommand::STATE_EVENT_INITIAL:
        LOG(EVENT, "Sync point %p is still in initial state.", eventState);
        return ZE_RESULT_NOT_READY;
    case VPU::VPUEventCommand::STATE_HOST_RESET:
        LOG(EVENT, "Sync point %p has been resetted by host.", eventState);
        return ZE_RESULT_NOT_READY;
    case VPU::VPUEventCommand::STATE_DEVICE_RESET:
        LOG(EVENT, "Sync point %p has ben resetted by device.", eventState);
        return ZE_RESULT_NOT_READY;
    case VPU::VPUEventCommand::STATE_HOST_SIGNAL:
        LOG(EVENT, "Sync point %p has been signaled by host.", eventState);
        return ZE_RESULT_SUCCESS;
    case VPU::VPUEventCommand::STATE_DEVICE_SIGNAL:
        LOG(EVENT, "Sync point %p has been signaled by device.", eventState);
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

void Event::setEventState(VPU::VPUEventCommand::KMDEventDataType state) {
    *eventState = state;
    LOG(EVENT, "Event state set to: %#lx", state);
}

const std::shared_ptr<VPU::VPUBufferObject> Event::getAssociatedBo() const {
    return eventBase;
}

} // namespace L0
