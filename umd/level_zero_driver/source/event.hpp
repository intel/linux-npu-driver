/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/api/zet_misc.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/include/l0_handler.hpp"
#include "vpu_driver/source/command/event_command.hpp"

#include <atomic>
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <ze_api.h>

namespace L0 {
struct Context;
} // namespace L0

namespace VPU {
class VPUBufferObject;
class VPUDeviceContext;
class VPUJob;
} // namespace VPU

struct _ze_event_handle_t : public L0::_ze_generic_handle_t {};

namespace L0 {

struct Event : _ze_event_handle_t, IContextObject {
  public:
    Event(VPU::VPUDeviceContext *ctx,
          VPU::VPUEventCommand::KMDEventDataType *ptr,
          const std::shared_ptr<VPU::VPUBufferObject> eventBaseBo,
          uint64_t vpuAddr,
          std::function<void()> &&destroyCb);
    ~Event() = default;

    inline ze_event_handle_t toHandle() { return this; }
    static Event *fromHandle(ze_event_handle_t handle) { return static_cast<Event *>(handle); }

    virtual ze_result_t destroy();
    virtual ze_result_t hostSignal();
    virtual ze_result_t queryStatus(int64_t timeout = 0L);
    virtual ze_result_t reset();
    virtual bool isCounterBasedEvent() const { return false; }

    ze_result_t hostSynchronize(uint64_t timeout);
    inline VPU::VPUEventCommand::KMDEventDataType *getSyncPointer() const { return eventState; }
    const std::shared_ptr<VPU::VPUBufferObject> getAssociatedBo() const;

    void associateJob(std::weak_ptr<VPU::VPUJob> job) { associatedJobs.push_back(std::move(job)); }
    virtual void setMetricTrackData(uint64_t groupMask, size_t dataSize) {
        msGroupMask = groupMask;
        msExpectedDataSize = dataSize;
    }
    virtual void trackMetricData(int64_t timeoutNs);

  protected:
    void setEventState(VPU::VPUEventCommand::KMDEventDataType updateTo);
    VPU::VPUDeviceContext *pDevCtx = nullptr;
    std::vector<std::weak_ptr<VPU::VPUJob>> associatedJobs;
    VPU::VPUEventCommand::KMDEventDataType *eventState = nullptr;
    const std::shared_ptr<VPU::VPUBufferObject> eventBase;

  private:
    uint64_t eventVpuAddr = 0;
    std::function<void()> destroyCb;
    size_t msExpectedDataSize = 0;
    uint64_t msGroupMask = 0ULL;
};

struct CounterBasedEvent : Event {
  public:
    CounterBasedEvent(Context *pContext,
                      VPU::VPUEventCommand::KMDEventDataType *ptr,
                      const std::shared_ptr<VPU::VPUBufferObject> eventBaseBo,
                      uint64_t vpuAddr,
                      ze_event_counter_based_flags_t creationFlags);
    ~CounterBasedEvent() = default;
    static ze_result_t create(ze_context_handle_t hContext,
                              const ze_event_counter_based_desc_t *desc,
                              ze_device_handle_t hDevice,
                              ze_event_handle_t *phEvent);
    static bool isSupported(ze_device_handle_t hDevice);
    ze_result_t destroy() override;
    ze_result_t hostSignal() override { return ZE_RESULT_ERROR_NOT_AVAILABLE; };
    ze_result_t queryStatus(int64_t timeout = 0L) override;
    ze_result_t reset() override { return ZE_RESULT_ERROR_NOT_AVAILABLE; };
    bool isCounterBasedEvent() const override { return true; }

    std::atomic<uint64_t> &getCounterValue() { return counterValue; }
    ze_event_counter_based_flags_t getCreationFlags() const { return creationFlags; }
    uint64_t getDeviceAddress() const;
    uint64_t getCompletionValue() const;

  private:
    void setMetricTrackData(uint64_t groupMask, size_t dataSize) override {
        L0_THROW_WHEN(true,
                      "CounterBasedEvent does not support metric tracking",
                      ZE_RESULT_ERROR_INVALID_ARGUMENT);
    }
    void trackMetricData(int64_t timeoutNs) override { return; };

  private:
    Context *pContext = nullptr;
    std::atomic<uint64_t> counterValue = 0;
    ze_event_counter_based_flags_t creationFlags = 0;
};
} // namespace L0
