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

Event *Event::create(EventPool *pEventPool, const ze_event_desc_t *desc) {
    if (!desc) {
        LOG_E("Invalid descriptor.");
        return nullptr;
    }

    if (pEventPool == nullptr) {
        LOG_E("Invalid pointer to eventPool");
        return nullptr;
    }

    auto pSyncPointer = pEventPool->allocateEvent(desc->index);
    if (pSyncPointer == nullptr) {
        LOG_E("Failed to get pointer from EventPool");
        return nullptr;
    }

    return new Event(pEventPool, desc->index, pSyncPointer);
}

Event::Event(EventPool *eventPool, uint32_t index, VPU::VPUEventCommand::KMDEventDataType *ptr)
    : nIndex(index)
    , pEventPool(eventPool)
    , pSyncPointer(ptr) {
    updateSyncState(VPU::VPUEventCommand::STATE_EVENT_INITIAL);
}

ze_result_t Event::destroy() {
    ze_result_t res = ZE_RESULT_SUCCESS;
    if (pEventPool == nullptr) {
        LOG_E("Invalid event pool pointer.");
        res = ZE_RESULT_ERROR_UNINITIALIZED;
    } else if (!pEventPool->freeEvent(nIndex)) {
        LOG_E("Failed to deallocate event pointer: %p", pSyncPointer);
        res = ZE_RESULT_ERROR_UNKNOWN;
    }
    LOG_V("Destroying event instance.(res: %#x)", res);

    delete this;
    return res;
}

ze_result_t Event::hostSignal() {
    updateSyncState(VPU::VPUEventCommand::STATE_HOST_SIGNAL);
    return ZE_RESULT_SUCCESS;
}

ze_result_t Event::hostSynchronize(uint64_t timeout) {
    /* Remove dangling weak pointers */
    associatedJobs.erase(std::remove_if(associatedJobs.begin(),
                                        associatedJobs.end(),
                                        [](auto x) { return x.use_count() == 0; }),
                         associatedJobs.end());

    bool signaled = VPU::waitForSignal(timeout, [this]() {
        if (queryStatus() == ZE_RESULT_SUCCESS)
            return true;

        /*
         * If there is no job in execution with Event then return to user to avoid waiting
         * infinitiely. This approach is a hack to break loop that waits infinitely for
         * inference result in ZeroBackend in VPUX plugin.
         */
        for (auto weakPtr : associatedJobs) {
            std::shared_ptr<VPU::VPUJob> job = weakPtr.lock();
            if (job.get() != nullptr && !job->waitForCompletion(0)) {
                return false;
            }
        }

        return true;
    });

    LOG_I("Sync completed. Time: %ld, signaled: %u", timeout, signaled);
    /*
     * 'queryStatus()' is used instead of 'signaled' beause waitForSignal can return true when
     * there all jobs are finished
     */
    return queryStatus();
}

ze_result_t Event::queryStatus() {
    switch (*pSyncPointer) {
    case VPU::VPUEventCommand::STATE_EVENT_INITIAL:
        LOG_V("Sync point %p is still in initial state.", pSyncPointer);
        return ZE_RESULT_NOT_READY;
    case VPU::VPUEventCommand::STATE_HOST_RESET:
        LOG_V("Sync point %p has been resetted by host.", pSyncPointer);
        return ZE_RESULT_NOT_READY;
    case VPU::VPUEventCommand::STATE_DEVICE_RESET:
        LOG_V("Sync point %p has ben resetted by device.", pSyncPointer);
        return ZE_RESULT_NOT_READY;
    case VPU::VPUEventCommand::STATE_HOST_SIGNAL:
        LOG_V("Sync point %p has been signaled by host.", pSyncPointer);
        return ZE_RESULT_SUCCESS;
    case VPU::VPUEventCommand::STATE_DEVICE_SIGNAL:
        LOG_V("Sync point %p has been signaled by device.", pSyncPointer);
        return ZE_RESULT_SUCCESS;
    default:
        LOG_E("Unexpected sync value. (%lx)", *pSyncPointer);
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }
}

ze_result_t Event::reset() {
    updateSyncState(VPU::VPUEventCommand::STATE_HOST_RESET);
    return ZE_RESULT_SUCCESS;
}

void Event::updateSyncState(VPU::VPUEventCommand::KMDEventDataType updateTo) {
    if (*pSyncPointer == updateTo) {
        LOG_W("The sync pointer(%p) already in target status(%lu).", pSyncPointer, updateTo);
    } else {
        LOG_V("Sync updated from %#lx to %#lx.", *pSyncPointer, updateTo);
        *pSyncPointer = updateTo;
    }
}

} // namespace L0
