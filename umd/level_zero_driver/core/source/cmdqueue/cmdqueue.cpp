/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include <bits/chrono.h>

#include "level_zero_driver/core/source/cmdqueue/cmdqueue.hpp"

#include "level_zero/ze_api.h"
#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/fence/fence.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "vpu_driver/source/command/vpu_command_buffer.hpp"
#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/timer.hpp"

#include <algorithm>
#include <chrono> // IWYU pragma: keep
#include <errno.h>
#include <iterator>
#include <mutex>
#include <utility>

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

CommandQueue::CommandQueue(Context *context,
                           Device *device,
                           bool isCopyOnly,
                           ze_command_queue_priority_t priority)
    : pContext(context)
    , device(device)
    , isCopyOnlyCommandQueue(isCopyOnly)
    , priority(priority) {}

ze_result_t CommandQueue::create(ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 const ze_command_queue_desc_t *desc,
                                 ze_command_queue_handle_t *phCommandQueue) {
    if (hContext == nullptr) {
        LOG_E("Invalid hContext pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    if (hDevice == nullptr) {
        LOG_E("Invalid hDevice pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
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

        LOG(CMDQUEUE, "CommandQueue created - %p", *phCommandQueue);
    } catch (const DriverError &err) {
        return err.result();
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandQueue::destroy() {
    pContext->removeObject(this);
    LOG(CMDQUEUE, "CommandQueue destroyed - %p", this);
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
        auto fence = std::make_unique<Fence>(this, desc);

        *phFence = fence.get();

        std::unique_lock lock(fenceMutex);
        fences.emplace(fence.get(), std::move(fence));

        LOG(CMDQUEUE, "Fence created - %p", *phFence);
    } catch (const DriverError &err) {
        return err.result();
    }

    return ZE_RESULT_SUCCESS;
}

void CommandQueue::destroyFence(Fence *pFence) {
    std::unique_lock lock(fenceMutex);
    fences.erase(pFence);
}

ze_result_t CommandQueue::executeCommandLists(uint32_t nCommandLists,
                                              ze_command_list_handle_t *phCommandLists,
                                              ze_fence_handle_t hFence) {
    LOG(CMDQUEUE, "CommandQueue Execute - %p", this);

    if (phCommandLists == nullptr) {
        LOG_E("Invalid pointer to handle hCommandLists");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (nCommandLists == 0u) {
        LOG_E("Invalid number of command lists");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    for (auto i = 0u; i < nCommandLists; i++) {
        auto cmdList = CommandList::fromHandle(phCommandLists[i]);

        if (isCopyOnlyCommandQueue && !cmdList->isCopyOnly()) {
            LOG_E("Invalid command list type");
            return ZE_RESULT_ERROR_INVALID_COMMAND_LIST_TYPE;
        }

        if (!cmdList->isCmdListClosed()) {
            LOG_E("Command List didn't close");
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
            LOG(CMDQUEUE, "No commands on list. Skipping command list %p.", cmdList);
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

        LOG(CMDQUEUE, "VPUJob %p submitted", job.get());
        jobs.emplace_back(std::move(job));
    }

    if (hFence != nullptr) {
        auto fence = Fence::fromHandle(hFence);
        std::shared_lock lock(fenceMutex);
        if (fences.find(fence) == fences.end()) {
            LOG_E("Fence %p is not from this command queue %p", fence, this);
            return ZE_RESULT_ERROR_INVALID_SYNCHRONIZATION_OBJECT;
        }

        fence->setTrackedJobs(std::move(jobs));
    } else {
        std::copy(jobs.begin(), jobs.end(), std::back_inserter(trackedJobs));
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandQueue::synchronize(uint64_t timeout) {
    LOG(CMDQUEUE, "CommandQueue synchronize - %p", this);
    auto absTp = VPU::getAbsoluteTimePoint(timeout);

    {
        std::shared_lock lock(fenceMutex);
        if (trackedJobs.empty() && fences.empty()) {
            LOG_W("No CommandList submitted");
            return ZE_RESULT_SUCCESS;
        }

        for (auto &[_, fence] : fences) {
            auto result = fence->waitForJobs(absTp);
            if (result != ZE_RESULT_SUCCESS)
                return result;
        }
    }

    ze_result_t result = waitForJobs(absTp, trackedJobs);
    if (result != ZE_RESULT_SUCCESS)
        return result;

    trackedJobs.clear();
    return result;
}

ze_result_t CommandQueue::waitForJobs(std::chrono::steady_clock::time_point absTimePoint,
                                      const std::vector<std::shared_ptr<VPU::VPUJob>> &jobs) {
    for (auto const &job : jobs) {
        if (!job->waitForCompletion(absTimePoint.time_since_epoch().count())) {
            return ZE_RESULT_NOT_READY;
        }
    }

    return Device::jobStatusToResult(jobs);
}

} // namespace L0
