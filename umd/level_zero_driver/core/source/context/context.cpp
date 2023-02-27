/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/context/context.hpp"

#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/event/eventpool.hpp"
#include "level_zero_driver/tools/source/metrics/metric_query.hpp"
#include "level_zero_driver/tools/source/metrics/metric_streamer.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace L0 {

ze_result_t Context::destroy() {
    delete this;

    return ZE_RESULT_SUCCESS;
}

DriverHandle *Context::getDriverHandle() {
    return this->driverHandle;
}

ze_result_t Context::getStatus() {
    auto device = driverHandle->getPrimaryDevice();
    if (device == nullptr) {
        LOG_E("Driver handle failed to retrieve primary device.");
        return ZE_RESULT_ERROR_DEVICE_LOST;
    }

    auto vpuDevice = device->getVPUDevice();
    if (vpuDevice == nullptr) {
        LOG_E("VPU device failed to be retrieved.");
        return ZE_RESULT_ERROR_DEVICE_LOST;
    }

    return vpuDevice->isConnected() ? ZE_RESULT_SUCCESS : ZE_RESULT_ERROR_DEVICE_LOST;
}

ze_result_t Context::createCommandQueue(ze_device_handle_t hDevice,
                                        const ze_command_queue_desc_t *desc,
                                        ze_command_queue_handle_t *commandQueue) {
    if (hDevice == nullptr) {
        LOG_E("hDevice passed as nullptr.");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    return L0::Device::fromHandle(hDevice)->createCommandQueue(desc, commandQueue, ctx.get());
}

ze_result_t Context::createCommandList(ze_device_handle_t hDevice,
                                       const ze_command_list_desc_t *desc,
                                       ze_command_list_handle_t *commandList) {
    if (hDevice == nullptr) {
        LOG_E("hDevice is NULL!");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    return L0::Device::fromHandle(hDevice)->createCommandList(desc, commandList, ctx.get());
}

ze_result_t Context::createEventPool(const ze_event_pool_desc_t *desc,
                                     uint32_t numDevices,
                                     ze_device_handle_t *phDevices,
                                     ze_event_pool_handle_t *phEventPool) {
    if (desc == nullptr || phEventPool == nullptr) {
        LOG_E("Invalid event pool(%p) descriptor or handler pointer(%p).", desc, phEventPool);
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    // Invalid flag set.
    if (desc->flags > 0x07) {
        return ZE_RESULT_ERROR_INVALID_ENUMERATION;
    }

    if (desc->count == 0 || (numDevices > 0 && phDevices == nullptr)) {
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    EventPool *eventPool =
        EventPool::create(getDriverHandle(), ctx.get(), numDevices, phDevices, desc);
    if (eventPool == nullptr) {
        LOG_E("Failed to create event pool.");
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;
    }
    *phEventPool = eventPool->toHandle();
    return ZE_RESULT_SUCCESS;
}

ze_result_t Context::activateMetricGroups(zet_device_handle_t hDevice,
                                          uint32_t count,
                                          zet_metric_group_handle_t *phMetricGroups) {
    if (hDevice == nullptr) {
        LOG_E("Device handle is NULL.");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    return L0::Device::fromHandle(hDevice)->activateMetricGroups(ctx->getFd(),
                                                                 count,
                                                                 phMetricGroups);
}

ze_result_t Context::createMetricQueryPool(zet_device_handle_t hDevice,
                                           zet_metric_group_handle_t hMetricGroup,
                                           const zet_metric_query_pool_desc_t *desc,
                                           zet_metric_query_pool_handle_t *phMetricQueryPool) {
    if (hDevice == nullptr || hMetricGroup == nullptr) {
        LOG_E("Device(%p) / MetricGroup(%p) handle is NULL.", hDevice, hMetricGroup);
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    if (desc == nullptr || phMetricQueryPool == nullptr) {
        LOG_E("Desc(%p) / MetricQueryPool(%p) handle is NULL.", desc, phMetricQueryPool);
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (desc->type > ZET_METRIC_QUERY_POOL_TYPE_EXECUTION) {
        LOG_E("Desc type value invalid: %x", desc->type);
        return ZE_RESULT_ERROR_INVALID_ENUMERATION;
    }

    if (desc->count == 0) {
        LOG_E("Desc count value invalid: %u", desc->count);
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    auto device = Device::fromHandle(hDevice);
    if (!device->isMetricsLoaded()) {
        LOG_E("Device metrics is not initialized.");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    auto metricGroup = MetricGroup::fromHandle(hMetricGroup);
    if (!metricGroup->isActivated()) {
        LOG_E("MetricGroup (%p) is not activated! Please activate metric group before QueryPool "
              "creation.",
              metricGroup);
        return ZE_RESULT_NOT_READY;
    }

    auto *metricQueryPool = new MetricQueryPool(ctx.get(), metricGroup, desc->count);
    if (metricQueryPool == nullptr) {
        LOG_E("Failed to create metric query pool.");
        return ZE_RESULT_ERROR_NOT_AVAILABLE;
    }

    if (!metricQueryPool->isInitialized()) {
        LOG_E("MetricQueryPool not initialized correctly. Deleting instance.");
        metricQueryPool->destroy();
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;
    }

    *phMetricQueryPool = metricQueryPool->toHandle();

    return ZE_RESULT_SUCCESS;
}

ze_result_t Context::metricStreamerOpen(zet_device_handle_t hDevice,
                                        zet_metric_group_handle_t hMetricGroup,
                                        zet_metric_streamer_desc_t *desc,
                                        ze_event_handle_t hNotificationEvent,
                                        zet_metric_streamer_handle_t *phMetricStreamer) {
    if (hDevice == nullptr || hMetricGroup == nullptr) {
        LOG_E("Device(%p) / MetricGroup(%p) handle is NULL.", hDevice, hMetricGroup);
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    if (desc == nullptr || phMetricStreamer == nullptr) {
        LOG_E("Desc(%p) / MetricStreamer(%p) handle is NULL.", desc, phMetricStreamer);
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto device = Device::fromHandle(hDevice);

    if (!device->isMetricsLoaded()) {
        LOG_E("Device metrics is not initialized.");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    auto metricGroup = MetricGroup::fromHandle(hMetricGroup);
    if (!metricGroup->isActivated()) {
        LOG_E("MetricGroup (%p) is not activated.", metricGroup);
        return ZE_RESULT_NOT_READY;
    }

    auto metricContext = device->getMetricContext().get();
    if (metricContext->getMetricStreamer() != nullptr) {
        LOG_E("Device already has a MetricStreamer opened.");
        return ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE;
    }

    auto pMetricStreamer = new MetricStreamer(metricGroup,
                                              desc->notifyEveryNReports,
                                              ctx.get(),
                                              device,
                                              hNotificationEvent);
    if (pMetricStreamer == nullptr) {
        LOG_E("Failed to create metric streamer.");
        return ZE_RESULT_ERROR_NOT_AVAILABLE;
    }

    if (!pMetricStreamer->isInitialized()) {
        LOG_E("MetricStreamer not initialized correctly. Closing instance.");
        pMetricStreamer->close();
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;
    }

    metricContext->setMetricStreamer(pMetricStreamer);

    drm_ivpu_metric_streamer_start startData = {};
    startData.metric_group_mask = 0x1 << metricGroup->getGroupIndex();

    if (desc->samplingPeriod < L0::MetricContext::MIN_SAMPLING_RATE_NS) {
        LOG_E("Sampling rate is too low.");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    // Sampling rate expressed in nanoseconds
    startData.sampling_rate_ns = desc->samplingPeriod;

    startData.read_rate = desc->notifyEveryNReports;

    const VPU::VPUDriverApi &drvApi = ctx->getDriverApi();

    if (drvApi.metricStreamerStart(&startData) < 0) {
        LOG_E("Failed to start metric streamer.");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    metricContext->sampleSize = startData.sample_size;

    *phMetricStreamer = pMetricStreamer->toHandle();

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
