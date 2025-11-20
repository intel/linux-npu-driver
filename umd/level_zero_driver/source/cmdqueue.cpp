/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include <bits/chrono.h>

#include "cmdqueue.hpp"

#include "cmdlist.hpp"
#include "context.hpp"
#include "device.hpp"
#include "fence.hpp"
#include "level_zero/ze_api.h"
#include "level_zero_driver/include/l0_exception.hpp"
#include "vpu_driver/source/command/job.hpp"
#include "vpu_driver/source/device/vpu_command_queue.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/timer.hpp"

#include <algorithm>
#include <chrono> // IWYU pragma: keep
#include <errno.h>
#include <iterator>
#include <limits>
#include <mutex>
#include <utility>

namespace L0 {

static VPU::VPUDeviceQueue::Priority toVPUDevicePriority(ze_command_queue_priority_t p) {
    switch (p) {
    case ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_LOW:
        return VPU::VPUDeviceQueue::Priority::IDLE;
    case ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_HIGH:
        return VPU::VPUDeviceQueue::Priority::REALTIME;
    case ZE_COMMAND_QUEUE_PRIORITY_NORMAL:
        return VPU::VPUDeviceQueue::Priority::NORMAL;
    case ZE_COMMAND_QUEUE_PRIORITY_FORCE_UINT32:
    default:
        LOG_E("Requested conversion from uninialized priority");
        return VPU::VPUDeviceQueue::Priority::NORMAL;
    }
}

CommandQueue::CommandQueue(Context *context,
                           std::unique_ptr<VPU::VPUDeviceQueue> queue,
                           CommandQueueMode mode)
    : vpuQueue(std::move(queue))
    , pContext(context)
    , queueMode(mode) {
    pContext->getDeviceContext()->preemptionCacheLoad();
}

CommandQueue::~CommandQueue() {
    // TODO: WA to drop all jobs before preemption cache pruning
    synchronize(0);

    if (preemptionBuffer) {
        preemptionBuffer.reset();
    }

    pContext->getDeviceContext()->preemptionCachePrune();
    LOG(CMDQUEUE, "CommandQueue destroyed - %p", this);
}

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
    CommandQueueMode mode = desc->mode == ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS
                                ? CommandQueueMode::SYNCHRONOUS
                                : CommandQueueMode::DEFAULT;

    try {
        Device *pDevice = Device::fromHandle(hDevice);
        ze_command_queue_group_property_flags_t flags =
            pDevice->getCommandQeueueGroupFlags(desc->ordinal);
        L0_THROW_WHEN(flags == 0, "Invalid group ordinal", ZE_RESULT_ERROR_INVALID_ARGUMENT);

        uint32_t queueCreationMode = VPU::VPUDeviceQueue::ModeFlags::DEFAULT;
        if (desc->pNext) {
            const auto *descriptorType = reinterpret_cast<const ze_base_desc_t *>(desc->pNext);
            switch (static_cast<ze_structure_type_command_queue_npu_ext_t>(descriptorType->stype)) {
            case ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC_NPU_EXT: {
                const auto *pNext =
                    reinterpret_cast<const ze_command_queue_desc_npu_ext_t *>(desc->pNext);
                if (pNext->turbo)
                    queueCreationMode |= VPU::VPUDeviceQueue::ModeFlags::TURBO;
            } break;
            case ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC_NPU_EXT_2: {
                const auto *pNext =
                    reinterpret_cast<const ze_command_queue_desc_npu_ext_2_t *>(desc->pNext);
                if (pNext && desc->mode == ZE_COMMAND_QUEUE_MODE_SYNCHRONOUS &&
                    pNext->options & ZE_NPU_COMMAND_QUEUE_OPTION_DEVICE_SYNC) {
                    LOG_E("Unsupported queue configuration, requested both SYNCHRONOUS and DEVICE "
                          "SYNC mode");
                    return ZE_RESULT_ERROR_INVALID_ENUMERATION;
                }

                if (pNext->options & ZE_NPU_COMMAND_QUEUE_OPTION_DEVICE_SYNC)
                    queueCreationMode |= VPU::VPUDeviceQueue::ModeFlags::IN_ORDER;

                /* Low level implementation uses the same fence object for device synchronization
                   and for waiting on job completion in turbo(low latency) mode. When device
                   synchronization is used turbo will not be enabled */
                if (!(pNext->options & ZE_NPU_COMMAND_QUEUE_OPTION_DEVICE_SYNC) &&
                    pNext->options & ZE_NPU_COMMAND_QUEUE_OPTION_TURBO)
                    queueCreationMode |= VPU::VPUDeviceQueue::ModeFlags::TURBO;
            } break;
            default:
                break;
            }
        }

        Context *pContext = Context::fromHandle(hContext);
        auto vpuQueue = VPU::VPUDeviceQueue::create(pContext->getDeviceContext(),
                                                    toVPUDevicePriority(desc->priority),
                                                    queueCreationMode);
        L0_THROW_WHEN(vpuQueue == nullptr,
                      "VPU Command queue creation failed.",
                      ZE_RESULT_ERROR_UNINITIALIZED);

        auto cmdQueue =
            std::make_unique<CommandQueue>(pContext, std::move(vpuQueue), std::move(mode));
        *phCommandQueue = cmdQueue.get();
        pContext->appendObject(std::move(cmdQueue));
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
    if (phCommandLists == nullptr) {
        LOG_E("Invalid pointer to phCommandLists");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (nCommandLists == 0u) {
        LOG_E("Invalid number of command lists");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    for (auto i = 0u; i < nCommandLists; i++) {
        auto cmdList = CommandList::fromHandle(phCommandLists[i]);
        if (cmdList == nullptr) {
            LOG_W("Received nullptr from phCommandList[%i]", i);
            return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        }

        if (!cmdList->isCmdListClosed()) {
            LOG_E("Commands are not closed in phCommandList[%i]: %p", i, cmdList);
            return ZE_RESULT_ERROR_UNINITIALIZED;
        }
    }

    if (preemptionBuffer == nullptr &&
        pContext->getDeviceContext()->isPreemptionBufferSupported()) {
        preemptionBuffer = pContext->getDeviceContext()->preemptionCacheAcquire();
    }

    std::vector<std::shared_ptr<VPU::VPUJob>> jobs;
    jobs.reserve(nCommandLists);

    for (auto i = 0u; i < nCommandLists; i++) {
        auto cmdList = CommandList::fromHandle(phCommandLists[i]);
        if (cmdList == nullptr) {
            LOG_W("Received nullptr from phCommandList[%i]", i);
            return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        }

        if (!cmdList->getNumCommands()) {
            LOG(CMDQUEUE, "Skipping submission of empty phCommandList[%u]: %p", i, cmdList);
            continue;
        }

        std::shared_ptr<VPU::VPUJob> job = cmdList->getJob();
        if (!job) {
            LOG_E("Received nullptr VPUJob from phCommandList[%u]: %p", i, cmdList);
            return ZE_RESULT_ERROR_UNKNOWN;
        }

        if (!job->updateOnSubmit()) {
            LOG_E("Failed to update job on submit");
            return ZE_RESULT_ERROR_UNKNOWN;
        }

        if (preemptionBuffer) {
            job->addPreemptionBuffer(preemptionBuffer);
        }

        if (!vpuQueue->submit(job.get())) {
            LOG_E("Failed to submit VPUJob(%p) from phCommandList[%u]: %p", job.get(), i, cmdList);
            if (errno == -EBADFD)
                return ZE_RESULT_ERROR_DEVICE_LOST;
            return ZE_RESULT_ERROR_UNKNOWN;
        }

        LOG(CMDQUEUE, "VPUJob %p submitted from phCommandList[%u]: %p", job.get(), i, cmdList);
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

    if (queueMode == CommandQueueMode::SYNCHRONOUS)
        return synchronize(std::numeric_limits<uint64_t>::max());

    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandQueue::synchronize(uint64_t timeout) {
    LOG(CMDQUEUE, "CommandQueue synchronize - %p", this);
    auto absTp = VPU::getAbsoluteTimePoint(timeout);

    {
        std::shared_lock lock(fenceMutex);
        if (trackedJobs.empty() && fences.empty()) {
            LOG(CMDQUEUE, "No CommandList submitted");
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

    // Put back the preemption buffer if no jobs are using it.
    // This covers "zeFence" and "zeCommandQueue" synchronization cases
    if (preemptionBuffer && preemptionBuffer.use_count() == 2) {
        preemptionBuffer.reset();
    }
    return Device::jobStatusToResult(jobs);
}

ze_result_t CommandQueue::setWorkloadType(ze_command_queue_workload_type_t workloadType) {
    switch (workloadType) {
    case ZE_WORKLOAD_TYPE_DEFAULT:
        if (!vpuQueue->toDefaultPriority())
            return ZE_RESULT_ERROR_UNKNOWN;
        break;
    case ZE_WORKLOAD_TYPE_BACKGROUND:
        if (!vpuQueue->toBackgroundPriority())
            return ZE_RESULT_ERROR_UNKNOWN;
        break;
    default:
        return ZE_RESULT_ERROR_INVALID_ENUMERATION;
    }
    return ZE_RESULT_SUCCESS;
}
} // namespace L0
