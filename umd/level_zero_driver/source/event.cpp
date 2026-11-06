/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include <bits/chrono.h>

#include "event.hpp"

#include "context.hpp"
#include "device.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "metric.hpp"
#include "metric_streamer.hpp"
#include "vpu_driver/source/command/command_buffer.hpp"
#include "vpu_driver/source/command/job.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/timer.hpp"

#include <algorithm>
#include <chrono> // IWYU pragma: keep
#include <compare>
#include <thread>
#include <ze_api.h>

namespace L0 {

Event::Event(VPU::VPUDeviceContext *ctx,
             VPU::VPUEventCommand::KMDEventDataType *ptr,
             const std::shared_ptr<VPU::VPUBufferObject> eventBaseBo,
             uint64_t vpuAddr,
             std::function<void()> &&destroyCb)
    : pDevCtx(ctx)
    , eventState(ptr)
    , eventBase(std::move(eventBaseBo))
    , eventVpuAddr(vpuAddr)
    , destroyCb(std::move(destroyCb)) {
    setEventState(VPU::VPUEventCommand::STATE_EVENT_INITIAL);
}

ze_result_t Event::destroy() {
    destroyCb();
    LOG(EVENT, "Event destroyed - %p", this);
    return ZE_RESULT_SUCCESS;
}

ze_result_t Event::hostSignal() {
    setEventState(VPU::VPUEventCommand::STATE_HOST_SIGNAL);
    return ZE_RESULT_SUCCESS;
}

void Event::trackMetricData(int64_t timeoutNs) {
    auto timeOut = std::chrono::steady_clock::time_point(std::chrono::nanoseconds(timeoutNs));

    if (!msExpectedDataSize || !msGroupMask)
        return;

    do {
        size_t dataSize = 0;
        if (*eventState >= VPU::VPUEventCommand::STATE_HOST_SIGNAL) {
            break;
        }
        if (MetricStreamer::getData(pDevCtx->getDriverApi(), msGroupMask, dataSize, nullptr) ==
            ZE_RESULT_SUCCESS) {
            if (dataSize >= msExpectedDataSize) {
                hostSignal();
                break;
            }
        } else {
            LOG_W("Metric data not available");
            break;
        }
        std::this_thread::sleep_for(
            std::chrono::nanoseconds(MetricContext::MIN_SAMPLING_RATE_NS / 2));
    } while (std::chrono::steady_clock::now() < timeOut);
}

ze_result_t Event::hostSynchronize(uint64_t timeout) {
    auto absoluteTimeout = VPU::getAbsoluteTimeoutNanoseconds(timeout);

    /* Remove dangling weak pointers */
    associatedJobs.erase(std::remove_if(associatedJobs.begin(),
                                        associatedJobs.end(),
                                        [](auto x) { return x.use_count() == 0; }),
                         associatedJobs.end());

    LOG(EVENT, "Waiting for fence in VPUAddr: %#lx", eventVpuAddr);

    for (auto &jobWeak : associatedJobs) {
        if (auto job = jobWeak.lock()) {
            for (const auto &cmdBuffer : job->getCommandBuffers()) {
                if (cmdBuffer->getFenceAddr() == eventVpuAddr) {
                    // TODO: Add check for ABORTED status from command buffer completion
                    if (!cmdBuffer->waitForCompletion(absoluteTimeout)) {
                        LOG_E("Associated command buffer is still in execution!");
                    }
                }
            }
        }
    }

    return queryStatus(absoluteTimeout);
}

ze_result_t Event::queryStatus(int64_t timeout) {
    if (msExpectedDataSize && *eventState < VPU::VPUEventCommand::STATE_DEVICE_SIGNAL)
        trackMetricData(timeout);

    switch (*eventState) {
    case VPU::VPUEventCommand::STATE_EVENT_INITIAL:
        LOG(EVENT, "Sync point %p is still in initial state.", eventState);
        return ZE_RESULT_NOT_READY;
    case VPU::VPUEventCommand::STATE_HOST_RESET:
        LOG(EVENT, "Sync point %p has been resetted by host.", eventState);
        return ZE_RESULT_NOT_READY;
    case VPU::VPUEventCommand::STATE_DEVICE_RESET:
        LOG(EVENT, "Sync point %p has ben resetted by device.", eventState);
        return ZE_RESULT_NOT_READY;
    case VPU::VPUEventCommand::STATE_HOST_SIGNAL:
        LOG(EVENT, "Sync point %p has been signaled by host.", eventState);
        return ZE_RESULT_SUCCESS;
    case VPU::VPUEventCommand::STATE_DEVICE_SIGNAL:
        LOG(EVENT, "Sync point %p has been signaled by device.", eventState);
        return ZE_RESULT_SUCCESS;
    default:
        LOG_E("Unexpected sync value. (%lx)", *eventState);
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }
}

ze_result_t Event::reset() {
    setEventState(VPU::VPUEventCommand::STATE_HOST_RESET);
    return ZE_RESULT_SUCCESS;
}

void Event::setEventState(VPU::VPUEventCommand::KMDEventDataType state) {
    *eventState = state;
    LOG(EVENT, "Event state set to: %#lx", state);
}

const std::shared_ptr<VPU::VPUBufferObject> Event::getAssociatedBo() const {
    return eventBase;
}

/* CounterBasedEvent implementation */

CounterBasedEvent::CounterBasedEvent(Context *pContext,
                                     VPU::VPUEventCommand::KMDEventDataType *ptr,
                                     const std::shared_ptr<VPU::VPUBufferObject> eventBaseBo,
                                     uint64_t vpuAddr,
                                     ze_event_counter_based_flags_t creationFlags)
    : Event(pContext->getDeviceContext(), ptr, eventBaseBo, vpuAddr, []() {})
    , pContext(pContext)
    , creationFlags(creationFlags) {
    /* The CounterBasedEvent need to be created in the signaled state.
     * Due differences in NPU architecture,the initial state is set accordingly.
     * For NPU architectures greater than NPU40XX,
     * the initial state need to be set to 1UL which corresponds to STATE_DEVICE_RESET,
     * otherwise it need to be set to 0UL which corresponds to STATE_EVENT_INITIAL.
     */
    if (pContext->getDeviceContext()->getDeviceCapabilities().npuArch > VPU::NPU40XX) {
        Event::setEventState(VPU::VPUEventCommand::STATE_DEVICE_RESET);
    } else {
        Event::setEventState(VPU::VPUEventCommand::STATE_EVENT_INITIAL);
    }
}

bool CounterBasedEvent::isSupported(ze_device_handle_t hDevice) {
    /* The feature available from JSM CMD API version 4.17 */
    return VPU::isJsmCmdApiGreaterThan(Device::fromHandle(hDevice)->getVPUDevice()->getHwInfo(),
                                       4,
                                       16);
}

ze_result_t CounterBasedEvent::create(ze_context_handle_t hContext,
                                      const ze_event_counter_based_desc_t *desc,
                                      ze_device_handle_t hDevice,
                                      ze_event_handle_t *phEvent) {
    if (hContext == nullptr) {
        LOG_E("Invalid hContext handler");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    if (desc == nullptr) {
        LOG_E("Invalid desc pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (hDevice == nullptr) {
        LOG_E("Invalid hDevice handler");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    if (phEvent == nullptr) {
        LOG_E("Invalid phEvent pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (!isSupported(hDevice)) {
        LOG_E("Counter-based events are not supported on this JSM API version");
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
    }

    auto supported_flags = static_cast<ze_event_counter_based_flags_t>(
        ZE_EVENT_COUNTER_BASED_FLAG_HOST_VISIBLE | ZE_EVENT_COUNTER_BASED_FLAG_IMMEDIATE |
        ZE_EVENT_COUNTER_BASED_FLAG_NON_IMMEDIATE);
    if (desc->flags & ~supported_flags) {
        LOG_W("Unsupported flags in event descriptor: %x", desc->flags);
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
    }

    try {
        Context *pContext = Context::fromHandle(hContext);
        L0_THROW_WHEN(pContext == nullptr,
                      "Invalid context pointer",
                      ZE_RESULT_ERROR_INVALID_NULL_POINTER);
        auto counterEventBo = pContext->getDeviceContext()->createUntrackedBufferObject(
            sizeof(uint64_t),
            VPU::VPUBufferObject::Type::CachedFw);

        if (!counterEventBo) {
            LOG_E("Failed to create buffer object for counter based event");
            return ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
        }
        auto counterEventValue = reinterpret_cast<uint64_t *>(counterEventBo->getBasePointer());
        auto counterEvent = std::make_unique<CounterBasedEvent>(pContext,
                                                                counterEventValue,
                                                                counterEventBo,
                                                                counterEventBo->getVPUAddr(),
                                                                desc->flags);

        *phEvent = counterEvent.get();
        pContext->appendObject(std::move(counterEvent));

        LOG(EVENT, "CounterBasedEvent created - %p", *phEvent);
    } catch (const DriverError &err) {
        return err.result();
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t CounterBasedEvent::destroy() {
    pContext->removeObject(this);
    LOG(EVENT, "Event destroyed - %p", this);
    return ZE_RESULT_SUCCESS;
}

uint64_t CounterBasedEvent::getDeviceAddress() const {
    return eventBase->getVPUAddr();
}

uint64_t CounterBasedEvent::getCompletionValue() const {
    return *reinterpret_cast<uint64_t *>(eventBase->getBasePointer());
}

ze_result_t CounterBasedEvent::queryStatus(int64_t timeout) {
    if (pDevCtx->getDeviceCapabilities().npuArch > VPU::NPU40XX) {
        if (static_cast<uint64_t>(*eventState) > counterValue.load(std::memory_order_acquire)) {
            return ZE_RESULT_SUCCESS;
        }
    } else {
        if (static_cast<uint64_t>(*eventState) >= counterValue.load(std::memory_order_acquire)) {
            return ZE_RESULT_SUCCESS;
        }
    }
    return ZE_RESULT_NOT_READY;
}
} // namespace L0
