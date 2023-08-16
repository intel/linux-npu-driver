/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/event/event.hpp"
#include "level_zero/ze_api.h"
#include "level_zero_driver/core/source/event/eventpool.hpp"
#include "level_zero_driver/core/source/device/device.hpp"

#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/timer.hpp"

namespace L0 {

Event::Event(EventPool *eventPool,
             uint32_t index,
             VPU::VPUEventCommand::KMDEventDataType *ptr,
             uint64_t vpuAddr)
    : pEventPool(eventPool)
    , nIndex(index)
    , eventState(ptr)
    , eventVpuAddr(vpuAddr) {
    setEventState(VPU::VPUEventCommand::STATE_EVENT_INITIAL);
}

ze_result_t Event::destroy() {
    ze_result_t res = ZE_RESULT_SUCCESS;
    if (pEventPool == nullptr) {
        LOG_E("Invalid event pool pointer.");
        res = ZE_RESULT_ERROR_UNINITIALIZED;
    } else if (!pEventPool->freeEvent(nIndex)) {
        LOG_E("Failed to deallocate event pointer: %p", eventState);
        res = ZE_RESULT_ERROR_UNKNOWN;
    }
    LOG_V("Destroying event instance.(res: %#x)", res);

    delete this;
    return res;
}

ze_result_t Event::hostSignal() {
    setEventState(VPU::VPUEventCommand::STATE_HOST_SIGNAL);
    return ZE_RESULT_SUCCESS;
}

ze_result_t Event::hostSynchronize(uint64_t timeout) {
    auto absoluteTimeout = VPU::getAbsoluteTimeoutNanoseconds(timeout);

    /* Remove dangling weak pointers */
    associatedJobs.erase(std::remove_if(associatedJobs.begin(),
                                        associatedJobs.end(),
                                        [](auto x) { return x.use_count() == 0; }),
                         associatedJobs.end());

    LOG_I("Waiting for fence in VPUAddr: %#lx", eventVpuAddr);
    /* Check if all jobs with this event are finished */
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

    return queryStatus();
}

ze_result_t Event::queryStatus() {
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
