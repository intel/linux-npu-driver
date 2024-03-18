/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/cmdqueue/cmdqueue.hpp"
#include "level_zero/ze_api.h"
#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/core/source/fence/fence.hpp"
#include "vpu_driver/source/utilities/timer.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <chrono>
#include <mutex>
#include <thread>

namespace L0 {

static VPU::VPUCommandBuffer::Priority toDriverPriority(ze_command_queue_priority_t p) {
    switch (p) {
    case ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_LOW:
        return VPU::VPUCommandBuffer::Priority::IDLE;
    case ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_HIGH:
        return VPU::VPUCommandBuffer::Priority::REALTIME;
    case ZE_COMMAND_QUEUE_PRIORITY_NORMAL:
    default:
        return VPU::VPUCommandBuffer::Priority::NORMAL;
    }
}

ze_result_t CommandQueue::create(ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 const ze_command_queue_desc_t *desc,
                                 ze_command_queue_handle_t *phCommandQueue) {
    if (hContext == nullptr) {
        LOG_E("Invalid hContext pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (hDevice == nullptr) {
        LOG_E("Invalid hDevice pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (desc == nullptr) {
        LOG_E("Invalid desc pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (phCommandQueue == nullptr) {
        LOG_E("Invalid phCommandQueue pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    try {
        Device *pDevice = Device::fromHandle(hDevice);
        ze_command_queue_group_property_flags_t flags =
            pDevice->getCommandQeueueGroupFlags(desc->ordinal);
        L0_THROW_WHEN(flags == 0, "Invalid group ordinal", ZE_RESULT_ERROR_INVALID_ARGUMENT);

        Context *pContext = Context::fromHandle(hContext);
        bool isCopyOnly = flags == ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COPY;
        auto commandQueue =
            std::make_unique<CommandQueue>(pContext, pDevice, isCopyOnly, desc->priority);

        *phCommandQueue = commandQueue.get();
        pContext->appendObject(std::move(commandQueue));

        LOG_I("CommandQueue created - %p", *phCommandQueue);
    } catch (const DriverError &err) {
        return err.result();
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandQueue::destroy() {
    pContext->removeObject(this);
    LOG_I("CommandQueue destroyed");
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandQueue::createFence(const ze_fence_desc_t *desc, ze_fence_handle_t *phFence) {
    if (desc == nullptr) {
        LOG_E("Invalid desc pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (phFence == nullptr) {
        LOG_E("Invalid phFence pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (desc->stype != ZE_STRUCTURE_TYPE_FENCE_DESC) {
        LOG_E("Invalid descriptor type (%d)", desc->stype);
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto fence = std::make_unique<Fence>(pContext, desc);

        *phFence = fence.get();
        pContext->appendObject(std::move(fence));

        LOG_I("Fence created - %p", *phFence);
    } catch (const DriverError &err) {
        return err.result();
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandQueue::executeCommandLists(uint32_t nCommandLists,
                                              ze_command_list_handle_t *phCommandLists,
                                              ze_fence_handle_t hFence) {
    LOG_V("Executing %u command list(s)", nCommandLists);

    if (phCommandLists == nullptr) {
        LOG_E("Invalid pointer to handle hCommandLists.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (nCommandLists == 0u) {
        LOG_E("Invalid number of command lists.");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    for (auto i = 0u; i < nCommandLists; i++) {
        auto cmdList = CommandList::fromHandle(phCommandLists[i]);

        if (isCopyOnlyCommandQueue && !cmdList->isCopyOnly()) {
            LOG_E("Invalid command list type.");
            return ZE_RESULT_ERROR_INVALID_COMMAND_LIST_TYPE;
        }

        if (!cmdList->isCmdListClosed()) {
            LOG_E("Command List didn't close.");
            return ZE_RESULT_ERROR_UNINITIALIZED;
        }
    }

    std::vector<std::shared_ptr<VPU::VPUJob>> jobs;
    for (auto i = 0u; i < nCommandLists; i++) {
        auto cmdList = CommandList::fromHandle(phCommandLists[i]);
        if (cmdList == nullptr) {
            LOG_W("Failed to get command stream. Skipping command list %p.", cmdList);
            continue;
        }

        if (!cmdList->getNumCommands()) {
            LOG_W("No commands on list. Skipping command list %p.", cmdList);
            continue;
        }

        std::shared_ptr<VPU::VPUJob> job = cmdList->getJob();
        if (!job) {
            LOG_E("Failed to get VPUJob from CommandList");
            return ZE_RESULT_ERROR_UNKNOWN;
        }

        job->setPriority(toDriverPriority(priority));
        if (!pContext->getDeviceContext()->submitJob(job.get())) {
            LOG_E("VPUJob submission failed");
            if (errno == -EBADFD)
                return ZE_RESULT_ERROR_DEVICE_LOST;
            return ZE_RESULT_ERROR_UNKNOWN;
        }

        LOG_I("VPUJob %p submitted", job.get());
        jobs.emplace_back(std::move(job));
    }

    if (hFence != nullptr) {
        LOG_V("A fence is given for command queue exec sync.");
        auto fence = Fence::fromHandle(hFence);
        if (fence == nullptr) {
            LOG_E("Failed to get Fence, invalid fence handle %p.", hFence);
            return ZE_RESULT_ERROR_INVALID_SYNCHRONIZATION_OBJECT;
        }

        fence->setTrackedJobs(jobs);
    }

    const std::lock_guard<std::timed_mutex> trackedJobsLock(trackedJobsMutex);
    std::copy(jobs.begin(), jobs.end(), std::back_inserter(trackedJobs));
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandQueue::synchronize(uint64_t timeout) {
    auto timeoutPoint = VPU::getAbsoluteTimePoint(timeout);
    /*
     * try_lock_until raise SIGABRT signal when timeout is => ~INT64_MAX in Ubuntu20.04 because
     * of signed overflow, detected by compiler flag '-ftrapv'. try_lock_for is free from this
     * limitation and is a first waiting function
     */
    if (!trackedJobsMutex.try_lock_for(std::chrono::nanoseconds(timeout))) {
        LOG_W("Failed to lock mutex on tracked jobs");
        return ZE_RESULT_NOT_READY;
    }

    const std::lock_guard<std::timed_mutex> trackedJobsLock(trackedJobsMutex, std::adopt_lock);
    if (trackedJobs.empty()) {
        LOG_W("No command execution to observe");
        return ZE_RESULT_SUCCESS;
    }

    LOG_V("Synchronize for %lu ns, %zu job count", timeout, trackedJobs.size());

    for (auto const &job : trackedJobs) {
        if (!job->waitForCompletion(timeoutPoint.time_since_epoch().count())) {
            LOG_W("Commands execution is not finished");
            return ZE_RESULT_NOT_READY;
        }
    }

    ze_result_t result = Device::jobStatusToResult(trackedJobs);

    trackedJobs.clear();
    LOG_I("Commands execution is finished");
    return result;
}

} // namespace L0
