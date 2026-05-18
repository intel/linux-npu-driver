/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include <stdint.h>

#include <atomic>
#include <memory>
#include <uapi/drm/ivpu_accel.h>

/* vpu_job_queue structures needed for UMQ ring-buffer write */
#include "api/vpu_jsm_api.h"

namespace VPU {
class VPUJob;
class VPUBufferObject;
class VPUCommandBuffer;
class VPUDeviceContext;
class VPUDriverApi;

class VPUDeviceQueue {
  public:
    enum class Priority : uint32_t {
        IDLE = DRM_IVPU_JOB_PRIORITY_IDLE,
        NORMAL = DRM_IVPU_JOB_PRIORITY_NORMAL,
        FOCUS = DRM_IVPU_JOB_PRIORITY_FOCUS,
        REALTIME = DRM_IVPU_JOB_PRIORITY_REALTIME,
    };

    enum ModeFlags : uint32_t { DEFAULT = 0, TURBO = 0x1, IN_ORDER = 0x2 };

    virtual ~VPUDeviceQueue() = default;

    static std::unique_ptr<VPUDeviceQueue>
    create(VPUDeviceContext *VPUContext, Priority queuePriority, uint32_t mode);

    virtual bool submit(VPUJob *job) = 0;
    virtual bool toBackgroundPriority() = 0;
    virtual bool toDefaultPriority() = 0;
    virtual bool isInOrder() = 0;
    virtual bool isTurbo() const = 0;

  protected:
    VPUDeviceQueue(VPUDriverApi *api);
    virtual int submitCommandBuffer(const std::unique_ptr<VPUCommandBuffer> &cmdBuf) = 0;

    VPUDriverApi *pDriverApi;
};

class VPUDeviceQueueLegacy final : public VPUDeviceQueue {
  public:
    VPUDeviceQueueLegacy(VPUDriverApi *api, Priority queuePriority);
    virtual ~VPUDeviceQueueLegacy() = default;

    bool submit(VPUJob *job) override;
    bool toBackgroundPriority() override;
    bool toDefaultPriority() override;
    bool isInOrder() override { return false; }
    bool isTurbo() const override { return false; }

  protected:
    int submitCommandBuffer(const std::unique_ptr<VPUCommandBuffer> &cmdBuf) override;

  private:
    Priority priority;
    const Priority defaultPriority;
};

class VPUDeviceQueueManaged final : public VPUDeviceQueue {
  public:
    VPUDeviceQueueManaged(VPUDriverApi *api, uint32_t defaultQueue, uint32_t mode);
    virtual ~VPUDeviceQueueManaged() override;

    bool submit(VPUJob *job) override;
    bool toBackgroundPriority() override;
    bool toDefaultPriority() override;
    bool isInOrder() override { return modeFlags & IN_ORDER ? true : false; }
    bool isTurbo() const override { return modeFlags & TURBO ? true : false; }

  protected:
    int submitCommandBuffer(const std::unique_ptr<VPUCommandBuffer> &cmdBuf) override;

  private:
    uint32_t currentId;
    uint32_t defaultId;
    uint32_t backgroundId;

    uint32_t modeFlags;
    std::shared_ptr<VPUBufferObject> lastWaitBo;
};

/**
 * VPUDeviceQueueUMQ - User Mode Queue fast path.
 *
 * Wraps a managed command queue and adds direct ring-buffer writes plus
 * doorbell MMIO writes, bypassing the CMDQ_SUBMIT ioctl in the hot path.
 * Falls back to the slow ioctl path on ring-full (EBUSY) or after a device reset.
 */
class VPUDeviceQueueUMQ final : public VPUDeviceQueue {
  public:
    /**
     * @brief Try to create a UMQ queue.  Returns nullptr if UMQ is not supported.
     */
    static std::unique_ptr<VPUDeviceQueueUMQ>
    tryCreate(VPUDriverApi *api, uint32_t cmdqId, uint32_t mode);

    ~VPUDeviceQueueUMQ() override;

    bool submit(VPUJob *job) override;
    bool toBackgroundPriority() override { return true; } /* no background queue for UMQ */
    bool toDefaultPriority() override { return true; }
    bool isInOrder() override { return modeFlags & IN_ORDER ? true : false; }
    bool isTurbo() const override { return modeFlags & TURBO ? true : false; }

  protected:
    int submitCommandBuffer(const std::unique_ptr<VPUCommandBuffer> &cmdBuf) override;

  private:
    VPUDeviceQueueUMQ(VPUDriverApi *api,
                      uint32_t cmdqId,
                      uint32_t mode,
                      void *ringPtr,
                      volatile uint32_t *doorbellPtr,
                      const drm_ivpu_cmdq_info &info,
                      size_t ringSize);

    /** Ring one job entry into the job queue and write the doorbell. */
    int ringJob(uint64_t batchBufAddr, uint32_t jobId, uint32_t preemptBufSize,
                uint64_t preemptBufAddr, uint32_t secPreemptBufSize,
                uint64_t secPreemptBufAddr);

    /** Return true if the device has been reset since we last checked. */
    bool checkReset();

    uint32_t cmdqId;
    uint32_t modeFlags;

    /* Mmap'd ring buffer (vpu_job_queue) */
    vpu_job_queue *ringBuf;
    size_t ringSize;

    /* Mmap'd doorbell MMIO page — write any value to ring the bell */
    volatile uint32_t *doorbell;

    /* Constant info from CMDQ_INFO */
    uint32_t entryCount;
    uint32_t jobIdBase;
    uint64_t primaryPreemptBufVpuAddr;
    uint32_t primaryPreemptBufSize;
    uint64_t secondaryPreemptBufVpuAddr;
    uint32_t secondaryPreemptBufSize;

    /* Monotonically incrementing job-ID counter (lower bits from job_id_base) */
    std::atomic<uint32_t> jobIdCounter;

    /* Reset counter at the time we last enabled UMQ */
    uint32_t lastResetCounter;
};
} // namespace VPU
