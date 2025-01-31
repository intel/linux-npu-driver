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

    virtual ~VPUDeviceQueue() = default;

    static std::unique_ptr<VPUDeviceQueue> create(VPUDeviceContext *VPUContext,
                                                  Priority queuePriority);

    virtual bool submit(const VPUJob *job) = 0;
    virtual bool toBackgroundPriority() = 0;
    virtual bool toDefaultPriority() = 0;

  protected:
    VPUDeviceQueue(VPUDriverApi *api);
    virtual int submitCommandBuffer(const std::unique_ptr<VPUCommandBuffer> &cmdBuf) = 0;

    VPUDriverApi *pDriverApi;
};

class VPUDeviceQueueLegacy final : public VPUDeviceQueue {
  public:
    VPUDeviceQueueLegacy(VPUDriverApi *api, Priority queuePriority);
    virtual ~VPUDeviceQueueLegacy() = default;

    bool submit(const VPUJob *job) override;
    bool toBackgroundPriority() override;
    bool toDefaultPriority() override;

  protected:
    int submitCommandBuffer(const std::unique_ptr<VPUCommandBuffer> &cmdBuf) override;

  private:
    Priority priority;
    const Priority defaultPriority;
};

class VPUDeviceQueueManaged final : public VPUDeviceQueue {
  public:
    VPUDeviceQueueManaged(VPUDriverApi *api, uint32_t defaultQueue);
    virtual ~VPUDeviceQueueManaged() override;

    bool submit(const VPUJob *job) override;
    bool toBackgroundPriority() override;
    bool toDefaultPriority() override;

  protected:
    int submitCommandBuffer(const std::unique_ptr<VPUCommandBuffer> &cmdBuf) override;

  private:
    uint32_t currentId;
    uint32_t defaultId;
    uint32_t backgroundId;
};
} // namespace VPU