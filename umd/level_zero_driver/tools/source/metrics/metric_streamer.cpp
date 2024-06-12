/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/ze_api.h"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/event/event.hpp"
#include "level_zero_driver/tools/source/metrics/metric_streamer.hpp"
#include "level_zero_driver/tools/source/metrics/metric.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/timer.hpp"

namespace L0 {

MetricStreamer::MetricStreamer(Context *pContext,
                               MetricGroup *metricGroupInput,
                               zet_metric_streamer_desc_t *desc,
                               ze_event_handle_t notifyHandle)
    : pContext(pContext)
    , metricGroup(metricGroupInput)
    , ctx(pContext->getDeviceContext()) {
    drm_ivpu_metric_streamer_start startData = {};
    startData.metric_group_mask = 0x1 << metricGroup->getGroupIndex();
    // Sampling rate expressed in nanoseconds
    startData.sampling_period_ns = desc->samplingPeriod;
    startData.read_period_samples = desc->notifyEveryNReports;

    L0_THROW_WHEN(ctx->getDriverApi().metricStreamerStart(&startData) < 0,
                  "Failed to start metric streamer",
                  ZE_RESULT_ERROR_UNKNOWN);
    sampleSize = startData.sample_size;

    if (notifyHandle && desc->notifyEveryNReports) {
        auto notifyEvent = L0::Event::fromHandle(notifyHandle);

        notifyEvent->setMetricTrackData(0x1ULL << metricGroup->getGroupIndex(),
                                        sampleSize * desc->notifyEveryNReports);
    }
}

MetricStreamer::~MetricStreamer() {
    drm_ivpu_metric_streamer_stop stopData = {};
    stopData.metric_group_mask = 0x1ULL << metricGroup->getGroupIndex();
    if (ctx->getDriverApi().metricStreamerStop(&stopData) < 0) {
        LOG_W("Failed to stop metric streamer");
    }
}

ze_result_t MetricStreamer::close() {
    pContext->removeObject(this);
    LOG(METRIC, "MetricStreamer destroyed - %p", this);
    return ZE_RESULT_SUCCESS;
}

ze_result_t MetricStreamer::getData(const VPU::VPUDriverApi &drvApi,
                                    const uint64_t groupMask,
                                    size_t &rawDataSize,
                                    uint8_t *pRawData) {
    drm_ivpu_metric_streamer_get_data data = {};
    data.metric_group_mask = groupMask;
    data.buffer_ptr = reinterpret_cast<long long unsigned int>(pRawData);
    data.buffer_size = rawDataSize;

    if (drvApi.metricStreamerGetData(&data) < 0) {
        LOG_E("Failed to get metric streamer data");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    rawDataSize = data.data_size;

    return ZE_RESULT_SUCCESS;
}
ze_result_t
MetricStreamer::readData(uint32_t maxReportCount, size_t *pRawDataSize, uint8_t *pRawData) {
    if (pRawDataSize == nullptr) {
        LOG_E("Invalid pRawDataSize pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    const VPU::VPUDriverApi &drvApi = ctx->getDriverApi();
    if (*pRawDataSize == 0) {
        size_t dataSize = 0;
        if (getData(drvApi, 0x1ULL << metricGroup->getGroupIndex(), dataSize, nullptr) !=
            ZE_RESULT_SUCCESS)
            return ZE_RESULT_ERROR_UNKNOWN;

        actualBufferSize = dataSize;
        *pRawDataSize = actualBufferSize;

        return ZE_RESULT_SUCCESS;
    }

    if (*pRawDataSize > actualBufferSize) {
        LOG_W("Size requested (%lu) is larger than actual data buffer size: %lu",
              *pRawDataSize,
              actualBufferSize);
        *pRawDataSize = actualBufferSize;
    }

    *pRawDataSize = std::min(*pRawDataSize, maxReportCount * sampleSize);
    if (pRawData != nullptr) {
        return getData(drvApi, 0x1ULL << metricGroup->getGroupIndex(), *pRawDataSize, pRawData);
    } else {
        LOG_W("Input raw data pointer is NULL");
    }

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
