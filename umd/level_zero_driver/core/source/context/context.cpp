/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/context/context.hpp"

#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/core/source/event/event.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/tools/source/metrics/metric.hpp"
#include "level_zero_driver/tools/source/metrics/metric_query.hpp"
#include "level_zero_driver/tools/source/metrics/metric_streamer.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <errno.h>
#include <linux/sysinfo.h>
#include <string.h>
#include <sys/sysinfo.h>

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
        LOG_E("Driver handle failed to retrieve primary device");
        return ZE_RESULT_ERROR_DEVICE_LOST;
    }

    auto vpuDevice = device->getVPUDevice();
    if (vpuDevice == nullptr) {
        LOG_E("VPU device failed to be retrieved");
        return ZE_RESULT_ERROR_DEVICE_LOST;
    }

    return vpuDevice->isConnected() ? ZE_RESULT_SUCCESS : ZE_RESULT_ERROR_DEVICE_LOST;
}

ze_result_t Context::activateMetricGroups(zet_device_handle_t hDevice,
                                          uint32_t count,
                                          zet_metric_group_handle_t *phMetricGroups) {
    if (hDevice == nullptr) {
        LOG_E("Device handle is NULL");
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
        LOG_E("Device metrics is not initialized");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    auto metricGroup = MetricGroup::fromHandle(hMetricGroup);
    if (!metricGroup->isActivated()) {
        LOG_E("MetricGroup (%p) is not activated! Please activate metric group before QueryPool "
              "creation.",
              metricGroup);
        return ZE_RESULT_NOT_READY;
    }

    try {
        auto metricQueryPool = std::make_unique<MetricQueryPool>(this, metricGroup, desc->count);
        *phMetricQueryPool = metricQueryPool.get();

        this->appendObject(std::move(metricQueryPool));

        LOG(CONTEXT, "MetricQueryPool created - %p", *phMetricQueryPool);
    } catch (const DriverError &err) {
        return err.result();
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t Context::metricStreamerOpen(zet_device_handle_t hDevice,
                                        zet_metric_group_handle_t hMetricGroup,
                                        zet_metric_streamer_desc_t *desc,
                                        ze_event_handle_t hNotificationEvent,
                                        zet_metric_streamer_handle_t *phMetricStreamer) {
    if (hDevice == nullptr) {
        LOG_E("Invalid hDevice pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    if (hMetricGroup == nullptr) {
        LOG_E("Invalid hMetricGroup pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    if (desc == nullptr) {
        LOG_E("Invalid desc pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (phMetricStreamer == nullptr) {
        LOG_E("Invalid phMetricStreamer pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto device = Device::fromHandle(hDevice);
    if (!device->isMetricsLoaded()) {
        LOG_E("Device metrics is not initialized");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    auto metricGroup = MetricGroup::fromHandle(hMetricGroup);
    if (!metricGroup->isActivated()) {
        LOG_E("MetricGroup (%p) is not activated.", metricGroup);
        return ZE_RESULT_NOT_READY;
    }

    if (desc->samplingPeriod < MetricContext::MIN_SAMPLING_RATE_NS) {
        LOG_E("Sampling rate is too low");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    try {
        auto metricStreamer =
            std::make_unique<MetricStreamer>(this, metricGroup, desc, hNotificationEvent);
        *phMetricStreamer = metricStreamer.get();
        this->appendObject(std::move(metricStreamer));

        LOG(CONTEXT, "MetricStreamer created - %p", *phMetricStreamer);
    } catch (const DriverError &err) {
        return err.result();
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t Context::queryContextMemory(ze_graph_memory_query_type_t type,
                                        ze_graph_memory_query_t *query) {
    if (type != ZE_GRAPH_QUERY_MEMORY_DDR) {
        LOG_E("Unsupported type");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    struct sysinfo info = {};
    if (sysinfo(&info) < 0) {
        LOG_E("Failed to get total ram using sysinfo, errno: %i, str: %s", errno, strerror(errno));
    } else {
        query->total = info.totalram * info.mem_unit;
    }
    query->allocated = ctx->getAllocatedSize();

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
