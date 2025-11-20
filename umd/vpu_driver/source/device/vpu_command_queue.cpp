/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include <bits/chrono.h>

#include "vpu_driver/source/device/vpu_command_queue.hpp"

#include "umd_common.hpp"
#include "vpu_driver/source/command/command_buffer.hpp"
#include "vpu_driver/source/command/job.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <chrono> // IWYU pragma: keep
#include <errno.h>
#include <memory>
#include <thread>
#include <uapi/drm/ivpu_accel.h>
#include <vector>

namespace VPU {

template <typename T>
bool submitWithWait(const VPUJob *job, T &&submitFunc) {
    if (job == nullptr) {
        LOG_W("Invalid argument - job is nullptr");
        return false;
    }

    if (job->getCommandBuffers().empty()) {
        LOG_E("Invalid argument - no command buffer in job");
        return false;
    }

    for (const auto &cmdBuffer : job->getCommandBuffers()) {
        constexpr auto pollTime = std::chrono::seconds(2);
        const auto timeoutPoint = std::chrono::steady_clock::now() + pollTime;

        while ((submitFunc)(cmdBuffer) < 0) {
            /*
             * SUBMIT ioctl returns EBUSY if command queue is full. Driver should wait till firmware
             * completes a job and make a space for new job in queue. Polling time is set to 2
             * seconds to match with TDR timeout.
             */
            if (errno != EBUSY) {
                LOG_E("Failed to submit command buffer: %p", cmdBuffer.get());
                return false;
            }

            if (std::chrono::steady_clock::now() > timeoutPoint) {
                LOG_E("Timed out waiting for driver to submit a job");
                return false;
            }

            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
    LOG(DEVICE, "Buffers execution successfully triggered");
    return true;
}

VPUDeviceQueue::VPUDeviceQueue(VPUDriverApi *api)
    : pDriverApi(api) {}

std::unique_ptr<VPUDeviceQueue>
VPUDeviceQueue::create(VPUDeviceContext *VPUContext, Priority queuePriority, uint32_t mode) {
    if (!VPUContext) {
        LOG_E("Invalid VPUContext pointer");
        return nullptr;
    }
    VPUDriverApi *pApi = &VPUContext->getDriverApi();
    if (!pApi) {
        LOG_E("Driver Api does not exist");
        return nullptr;
    }
    if (VPUContext->getDeviceCapabilities().cmdQueueCreationCapability) {
        uint32_t defaultQueue;
        if (pApi->commandQueueCreate(static_cast<uint32_t>(queuePriority),
                                     defaultQueue,
                                     mode & ModeFlags::TURBO ? true : false)) {
            LOG_E("Command queue creation failed.");
            return nullptr;
        }

        return std::make_unique<VPUDeviceQueueManaged>(pApi, defaultQueue, mode);
    }
    if (mode & ModeFlags::IN_ORDER) {
        LOG_E("In order mode not supported. Command queue creation failed.");
        return nullptr;
    }

    LOG(CMDQUEUE, "Continue creating queue with default mode");
    return std::make_unique<VPUDeviceQueueLegacy>(pApi, queuePriority);
}

VPUDeviceQueueLegacy::VPUDeviceQueueLegacy(VPUDriverApi *api, Priority queuePriority)
    : VPUDeviceQueue(api)
    , priority(queuePriority)
    , defaultPriority(queuePriority) {}

int VPUDeviceQueueLegacy::submitCommandBuffer(const std::unique_ptr<VPUCommandBuffer> &cmdBuf) {
    drm_ivpu_submit execParam = {};
    execParam.buffers_ptr = reinterpret_cast<uint64_t>(cmdBuf->getBufferHandles().data());
    execParam.buffer_count = safe_cast<uint32_t>(cmdBuf->getBufferHandles().size());
    execParam.engine = DRM_IVPU_ENGINE_COMPUTE;
    execParam.commands_offset = cmdBuf->getCommandBufferOffset();
    execParam.priority = static_cast<uint32_t>(priority);

    LOG(DEVICE,
        "Submit params -> engine: %u, flags: %u, offset: %u, count: %u, ptr: "
        "%#llx, prior: %u",
        execParam.engine,
        execParam.flags,
        execParam.commands_offset,
        execParam.buffer_count,
        execParam.buffers_ptr,
        execParam.priority);

    return pDriverApi->submitCommandBuffer(&execParam);
}

bool VPUDeviceQueueLegacy::submit(VPUJob *job) {
    if (!job || job->isInOrder()) {
        LOG_E("Submit failed, INORDER request on queue without INORDER support");
        return false;
    }
    return submitWithWait(job, [this](auto &cmdBuf) { return this->submitCommandBuffer(cmdBuf); });
}

bool VPUDeviceQueueLegacy::toBackgroundPriority() {
    priority = Priority::IDLE;
    return true;
}

bool VPUDeviceQueueLegacy::toDefaultPriority() {
    priority = defaultPriority;
    return true;
}

VPUDeviceQueueManaged::VPUDeviceQueueManaged(VPUDriverApi *api,
                                             uint32_t defaultQueue,
                                             uint32_t mode)
    : VPUDeviceQueue(api)
    , currentId(defaultQueue)
    , defaultId(defaultQueue)
    , backgroundId(defaultQueue)
    , modeFlags(mode) {}

VPUDeviceQueueManaged::~VPUDeviceQueueManaged() {
    if (backgroundId != defaultId && pDriverApi->commandQueueDestroy(backgroundId))
        LOG_E("Removing background command queue id %d failed", backgroundId);
    if (pDriverApi->commandQueueDestroy(defaultId))
        LOG_E("Removing command queue id %d failed", defaultId);
}

int VPUDeviceQueueManaged::submitCommandBuffer(const std::unique_ptr<VPUCommandBuffer> &cmdBuf) {
    drm_ivpu_cmdq_submit submitArgs = {};
    submitArgs.buffers_ptr = reinterpret_cast<uint64_t>(cmdBuf->getBufferHandles().data());
    submitArgs.buffer_count = safe_cast<uint32_t>(cmdBuf->getBufferHandles().size());
    submitArgs.commands_offset = cmdBuf->getCommandBufferOffset();
    // If preempt_buffer_index is 0, then kernel allocates preemption buffer
    submitArgs.preempt_buffer_index = cmdBuf->getPreemptionBufferIndex();
    submitArgs.cmdq_id = currentId;

    if (isTurbo())
        cmdBuf->useBusyWait();

    return pDriverApi->commandQueueSubmit(&submitArgs);
}

bool VPUDeviceQueueManaged::submit(VPUJob *job) {
    if (!job)
        return false;

    if (isInOrder()) {
        if (!job->makeInOrder(lastWaitBo)) {
            LOG_E("Failed to create in order workload");
            return false;
        }
    } else if (job->isInOrder()) {
        if (!job->stripInOrder()) {
            LOG_E("Failed to prepare job to submit.");
            return false;
        }
    }

    return submitWithWait(job, [this](auto &cmdBuf) { return this->submitCommandBuffer(cmdBuf); });
}

bool VPUDeviceQueueManaged::toBackgroundPriority() {
    if (backgroundId == defaultId) {
        if (pDriverApi->commandQueueCreate(static_cast<uint32_t>(Priority::IDLE),
                                           backgroundId,
                                           modeFlags & ModeFlags::TURBO ? true : false)) {
            LOG_E("Background command queue creation failed.");
            return false;
        }
    }
    currentId = backgroundId;
    return true;
}

bool VPUDeviceQueueManaged::toDefaultPriority() {
    currentId = defaultId;
    return true;
}
} // namespace VPU
