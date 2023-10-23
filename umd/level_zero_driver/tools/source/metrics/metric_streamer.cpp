/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/ze_api.h"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/tools/source/metrics/metric_streamer.hpp"
#include "level_zero_driver/tools/source/metrics/metric.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace L0 {

MetricStreamer::MetricStreamer(MetricGroup *metricGroupInput,
                               uint32_t notifyNReport,
                               VPU::VPUDeviceContext *ctxInput,
                               Device *deviceInput,
                               ze_event_handle_t hNotifyEvent)
    : metricGroup(metricGroupInput)
    , nReports(notifyNReport)
    , ctx(ctxInput)
    , device(deviceInput)
    , eventHandle(hNotifyEvent) {
    if (eventHandle)
        LOG_W("No support for event handle in MetricStreamer");
}

ze_result_t MetricStreamer::close() {
    const VPU::VPUDriverApi &drvApi = ctx->getDriverApi();

    drm_ivpu_metric_streamer_stop stopData = {};
    stopData.metric_group_mask = 0x1 << metricGroup->getGroupIndex();

    if (drvApi.metricStreamerStop(&stopData) < 0) {
        LOG_E("Failed to stop metric streamer.");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    device->getMetricContext()->setMetricStreamer(nullptr);

    delete this;

    return ZE_RESULT_SUCCESS;
}

ze_result_t
MetricStreamer::readData(uint32_t maxReportCount, size_t *pRawDataSize, uint8_t *pRawData) {
    if (pRawDataSize == nullptr) {
        LOG_E("Invalid pRawDataSize pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (maxReportCount > nReports)
        maxReportCount = nReports;

    auto metricContext = device->getMetricContext();

    const VPU::VPUDriverApi &drvApi = ctx->getDriverApi();

    if (*pRawDataSize == 0) {
        drm_ivpu_metric_streamer_get_data data = {};
        data.metric_group_mask = 0x1 << metricGroup->getGroupIndex();
        data.size = 0;
        data.buffer_ptr = 0;

        if (drvApi.metricStreamerGetData(&data) < 0) {
            LOG_E("Failed to get metric streamer data.");
            return ZE_RESULT_ERROR_UNKNOWN;
        }

        // Actual size of the buffer
        *pRawDataSize = data.size;
        metricContext->actualBufferSize = data.size;

        return ZE_RESULT_SUCCESS;
    }

    if (*pRawDataSize > metricContext->actualBufferSize) {
        LOG_W("Size requested (%lu) is larger than actual data buffer size: %lu",
              *pRawDataSize,
              metricContext->actualBufferSize);
        *pRawDataSize = metricContext->actualBufferSize;
    }

    if (maxReportCount * metricContext->sampleSize < metricContext->actualBufferSize) {
        *pRawDataSize = maxReportCount * metricContext->sampleSize;
    }

    if (pRawData != nullptr) {
        drm_ivpu_metric_streamer_get_data data = {};
        data.metric_group_mask = 0x1 << metricGroup->getGroupIndex();
        data.size = *pRawDataSize;
        data.buffer_ptr = reinterpret_cast<long long unsigned int>(pRawData);

        if (drvApi.metricStreamerGetData(&data) < 0) {
            LOG_E("Failed to get metric streamer data.");
            return ZE_RESULT_ERROR_UNKNOWN;
        }

        *pRawDataSize = data.size;
    } else {
        LOG_W("Input raw data pointer is NULL.");
    }

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
