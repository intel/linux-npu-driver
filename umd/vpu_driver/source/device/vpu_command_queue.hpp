/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include <stdint.h>

#include <memory>
#include <uapi/drm/ivpu_accel.h>

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

  protected:
    int submitCommandBuffer(const std::unique_ptr<VPUCommandBuffer> &cmdBuf) override;

  private:
    uint32_t currentId;
    uint32_t defaultId;
    uint32_t backgroundId;

    uint32_t modeFlags;
    std::shared_ptr<VPUBufferObject> lastWaitBo;
};
} // namespace VPU
