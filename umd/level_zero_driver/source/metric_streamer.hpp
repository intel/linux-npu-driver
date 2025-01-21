/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/include/l0_handler.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/zet_api.h>

namespace VPU {
class VPUDeviceContext;
class VPUDriverApi;
} // namespace VPU

struct _zet_metric_streamer_handle_t {};

namespace L0 {

struct Context;
struct MetricGroup;

struct MetricStreamer : _zet_metric_streamer_handle_t, IContextObject {
    MetricStreamer(Context *pContext,
                   MetricGroup *metricGroupInput,
                   zet_metric_streamer_desc_t *desc,
                   ze_event_handle_t hNotifyEvent);
    ~MetricStreamer();

    inline zet_metric_streamer_handle_t toHandle() { return this; }
    static MetricStreamer *fromHandle(zet_metric_streamer_handle_t handle) {
        return static_cast<MetricStreamer *>(handle);
    }

    ze_result_t close();
    ze_result_t readData(uint32_t maxReportCount, size_t *pRawDataSize, uint8_t *pRawData);

    static ze_result_t getData(const VPU::VPUDriverApi &drvApi,
                               const uint64_t groupMask,
                               size_t &rawDataSize,
                               uint8_t *pRawData);

  private:
    Context *pContext = nullptr;
    MetricGroup *metricGroup = nullptr;
    VPU::VPUDeviceContext *ctx = nullptr;
    uint64_t sampleSize = 0u;
    uint64_t actualBufferSize = 0u;
};

} // namespace L0
