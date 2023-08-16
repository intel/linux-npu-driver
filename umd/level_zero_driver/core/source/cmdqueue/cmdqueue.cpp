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

#include <thread>

namespace L0 {

CommandQueue *CommandQueue::create(Device *device,
                                   const ze_command_queue_desc_t *desc,
                                   VPU::VPUDeviceContext *ctx) {
    bool isCopyOnly = false;
    VPU::VPUDevice *vpuDevice = device->getVPUDevice();

    if (desc == nullptr) {
        LOG_E("Invalid command queue descriptor.");
        return nullptr;
    }

    if (vpuDevice == nullptr) {
        LOG_E("VPUDevice is not ready.");
        return nullptr;
    }

    VPU::EngineType engType = vpuDevice->getEngineTypeFromOrdinal(desc->ordinal, isCopyOnly);
    if (engType == VPU::EngineType::INVALID) {
        LOG_E("Invalid engine ordinal(%u) has given", desc->ordinal);
        return nullptr;
    }

    CommandQueue *commandQueue = new CommandQueue(device, desc, ctx, isCopyOnly);
    if (commandQueue != nullptr) {
        LOG_I("CommandQueue created.");
    }

    return commandQueue;
}

ze_result_t CommandQueue::destroy() {
    delete this;
    LOG_I("CommandQueue destroyed.");

    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandQueue::createFence(const ze_fence_desc_t *desc, ze_fence_handle_t *phFence) {
    if ((desc == nullptr) || (phFence == nullptr)) {
        LOG_E("Pointer to fence descriptor/pointer to handle hFence passed as nullptr.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (desc->stype != ZE_STRUCTURE_TYPE_FENCE_DESC) {
        LOG_E("Invalid descriptor type. (%d)", desc->stype);
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *phFence = new Fence(this, desc);
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

    auto vpuDevice = device->getVPUDevice();
    if (vpuDevice == nullptr) {
        LOG_W("Failed to submit the buffer. (vpuDevice: %p)", vpuDevice);
        return ZE_RESULT_ERROR_DEVICE_LOST;
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

        if (!ctx->submitJob(job.get())) {
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

    std::copy(jobs.begin(), jobs.end(), std::back_inserter(trackedJobs));
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandQueue::synchronize(uint64_t timeout) {
    if (trackedJobs.empty()) {
        LOG_W("No command execution to observe");
        return ZE_RESULT_SUCCESS;
    }

    LOG_V("Synchronize for %lu ns, %zu job count", timeout, trackedJobs.size());

    bool allSignaled = waitForSignal(timeout, trackedJobs);
    if (!allSignaled) {
        LOG_W("Commands execution is not finished");
        return ZE_RESULT_NOT_READY;
    }

    ze_result_t result = Device::jobStatusToResult(trackedJobs);

    trackedJobs.clear();
    LOG_I("Commands execution is finished");
    return result;
}

} // namespace L0
