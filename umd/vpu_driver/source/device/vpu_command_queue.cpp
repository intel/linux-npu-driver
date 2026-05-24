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
#include <immintrin.h>
#include <memory>
#include <sys/mman.h>
#include <sys/user.h>
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
                                     mode & ModeFlags::TURBO ? true : false,
                                     VPUContext->getDeviceCapabilities().umqCapability)) {
            LOG_E("Command queue creation failed.");
            return nullptr;
        }

        /* Try to set up UMQ fast path if the capability is available */
        if (VPUContext->getDeviceCapabilities().umqCapability) {
            auto umq = VPUDeviceQueueUMQ::tryCreate(pApi, defaultQueue, mode);
            if (umq) {
                LOG(CMDQUEUE, "UMQ fast path enabled for cmdq %u", defaultQueue);
                return umq;
            }
            LOG(CMDQUEUE, "UMQ setup failed, falling back to managed queue");
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

/* ========================================================================
 * VPUDeviceQueueUMQ — User Mode Queue fast path
 * ======================================================================== */

VPUDeviceQueueUMQ::VPUDeviceQueueUMQ(VPUDriverApi *api,
                                     uint32_t cmdqId,
                                     uint32_t mode,
                                     void *ringPtr,
                                     volatile uint32_t *doorbellPtr,
                                     const drm_ivpu_cmdq_info &info,
                                     size_t ringMapSize)
    : VPUDeviceQueue(api)
    , cmdqId(cmdqId)
    , modeFlags(mode)
    , ringBuf(reinterpret_cast<vpu_job_queue *>(ringPtr))
    , ringSize(ringMapSize)
    , doorbell(doorbellPtr)
    , entryCount(info.entry_count)
    , jobIdBase(info.job_id_base)
    , primaryPreemptBufVpuAddr(info.primary_preempt_buf_vpu_addr)
    , primaryPreemptBufSize(info.primary_preempt_buf_size)
    , secondaryPreemptBufVpuAddr(info.secondary_preempt_buf_vpu_addr)
    , secondaryPreemptBufSize(info.secondary_preempt_buf_size)
    , jobIdCounter(0)
    , lastResetCounter(info.reset_counter) {}

VPUDeviceQueueUMQ::~VPUDeviceQueueUMQ() {
    if (doorbell && doorbell != MAP_FAILED)
        pDriverApi->unmap(const_cast<uint32_t *>(doorbell), PAGE_SIZE);
    if (ringBuf && ringBuf != MAP_FAILED)
        pDriverApi->unmap(ringBuf, ringSize);
    pDriverApi->commandQueueUmqDisable(cmdqId);
    pDriverApi->commandQueueDestroy(cmdqId);
}

std::unique_ptr<VPUDeviceQueueUMQ>
VPUDeviceQueueUMQ::tryCreate(VPUDriverApi *api, uint32_t cmdqId, uint32_t mode) {
    /* 1. Query UMQ parameters */
    drm_ivpu_cmdq_info info = {};
    info.cmdq_id = cmdqId;
    if (api->commandQueueGetInfo(&info)) {
        LOG_E("CMDQ_INFO failed for cmdq %u", cmdqId);
        return nullptr;
    }

    /* 2. Enable UMQ — holds a PM reference in the kernel */
    drm_ivpu_cmdq_umq_enable enableArgs = {};
    enableArgs.cmdq_id = cmdqId;
    enableArgs.reset_eventfd = -1; /* no reset eventfd for now */
    if (api->commandQueueUmqEnable(&enableArgs)) {
        LOG_E("CMDQ_UMQ_ENABLE failed for cmdq %u", cmdqId);
        return nullptr;
    }

    /* 3. mmap the job ring buffer */
    const size_t ringSize = static_cast<size_t>(PAGE_SIZE); /* cmdq mem is SZ_4K */
    void *ringPtr = api->mmap(ringSize, static_cast<off_t>(info.cmdq_mmap_offset));
    if (!ringPtr || ringPtr == MAP_FAILED) {
        LOG_E("mmap of cmdq ring buffer failed, offset %#llx", info.cmdq_mmap_offset);
        api->commandQueueUmqDisable(cmdqId);
        return nullptr;
    }

    /* 4. mmap the doorbell MMIO page (write-only, noncached) */
    void *dbPtr = api->mmap(PAGE_SIZE, static_cast<off_t>(info.db_mmap_offset));
    if (!dbPtr || dbPtr == MAP_FAILED) {
        LOG_E("mmap of doorbell failed, offset %#llx", info.db_mmap_offset);
        api->unmap(ringPtr, ringSize);
        api->commandQueueUmqDisable(cmdqId);
        return nullptr;
    }

    LOG(CMDQUEUE,
        "UMQ: cmdq %u ring @%p db @%p entry_count %u job_id_base %#x reset %u",
        cmdqId, ringPtr, dbPtr, info.entry_count, info.job_id_base, info.reset_counter);

    return std::unique_ptr<VPUDeviceQueueUMQ>(
        new VPUDeviceQueueUMQ(api, cmdqId, mode, ringPtr,
                              reinterpret_cast<volatile uint32_t *>(dbPtr), info, ringSize));
}

bool VPUDeviceQueueUMQ::checkReset() {
    drm_ivpu_cmdq_info info = {};
    info.cmdq_id = cmdqId;
    if (pDriverApi->commandQueueGetInfo(&info))
        return true; /* assume reset if ioctl fails */
    if (info.reset_counter != lastResetCounter) {
        LOG(CMDQUEUE, "UMQ: device reset detected (counter %u -> %u)",
            lastResetCounter, info.reset_counter);
        lastResetCounter = info.reset_counter;
        return true;
    }
    return false;
}

int VPUDeviceQueueUMQ::ringJob(uint64_t batchBufAddr, uint32_t jobId,
                               uint32_t preemptBufSize, uint64_t preemptBufAddr,
                               uint32_t secPreemptBufSize, uint64_t secPreemptBufAddr) {
    vpu_job_queue_header *header = &ringBuf->header;
    uint32_t tail = __atomic_load_n(&header->tail, __ATOMIC_ACQUIRE);
    uint32_t nextEntry = (tail + 1) % entryCount;

    if (nextEntry == header->head) {
        /* Ring full */
        errno = EBUSY;
        return -1;
    }

    vpu_job *entry = &ringBuf->slot[tail].job;
    entry->batch_buf_addr           = batchBufAddr;
    entry->job_id                   = jobId;
    entry->flags                    = 0;
    entry->doorbell_timestamp       = 0;
    entry->host_tracking_id         = 0;
    entry->primary_preempt_buf_addr = preemptBufAddr;
    entry->primary_preempt_buf_size = preemptBufSize;
    entry->secondary_preempt_buf_addr = secPreemptBufAddr;
    entry->secondary_preempt_buf_size = secPreemptBufSize;
    entry->reserved_0               = 0;

    /* Ensure entry is written before updating tail */
    __asm__ volatile("sfence" ::: "memory");
    __atomic_store_n(&header->tail, nextEntry, __ATOMIC_RELEASE);
    /* Flush WC buffer and ensure tail is visible before doorbell write */
    __asm__ volatile("sfence" ::: "memory");

    /* Ring the doorbell — MMIO WC write, one word is enough */
    *doorbell = 1u;
    /* Flush the MMIO write */
    __asm__ volatile("sfence" ::: "memory");

    return 0;
}

int VPUDeviceQueueUMQ::submitCommandBuffer(const std::unique_ptr<VPUCommandBuffer> &cmdBuf) {
    /* Enable busy-wait completion (NPU writes fenceValue when done) and UMQ mode
     * (skip BO_WAIT ioctl in waitForCompletion). */
    cmdBuf->useBusyWait();
    cmdBuf->setUmqMode(true);

    uint32_t jobId = jobIdBase | (jobIdCounter.fetch_add(1, std::memory_order_relaxed) &
                                   0x00FFFFFFu);

    int ret = ringJob(cmdBuf->getBuffer()->getVPUAddr() + cmdBuf->getCommandBufferOffset(),
                      jobId,
                      primaryPreemptBufSize,
                      primaryPreemptBufVpuAddr,
                      secondaryPreemptBufSize,
                      secondaryPreemptBufVpuAddr);
    if (ret < 0 && errno != EBUSY) {
        /* Non-EBUSY failure: check whether a device reset occurred */
        if (checkReset()) {
            errno = ENODEV;
        }
    }
    return ret;
}

bool VPUDeviceQueueUMQ::submit(VPUJob *job) {
    if (!job)
        return false;

    return submitWithWait(job, [this](auto &cmdBuf) { return this->submitCommandBuffer(cmdBuf); });
}
} // namespace VPU
