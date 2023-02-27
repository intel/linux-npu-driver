/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "umd_common.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"

#include <level_zero/zet_api.h>

struct _zet_metric_streamer_handle_t {};

namespace L0 {

struct Device;
struct MetricGroup;

struct MetricStreamer : _zet_metric_streamer_handle_t {
    MetricStreamer(MetricGroup *metricGroupInput,
                   uint32_t notifyNReport,
                   VPU::VPUDeviceContext *ctxInput,
                   Device *deviceInput,
                   ze_event_handle_t hNotifyEvent);
    ~MetricStreamer();

    inline zet_metric_streamer_handle_t toHandle() { return this; }
    static MetricStreamer *fromHandle(zet_metric_streamer_handle_t handle) {
        return static_cast<MetricStreamer *>(handle);
    }

    ze_result_t close();
    ze_result_t readData(uint32_t maxReportCount, size_t *pRawDataSize, uint8_t *pRawData);
    inline bool isInitialized() const { return initialized; }

  protected:
    MetricGroup *metricGroup = nullptr;
    void *pMetricData = nullptr;
    uint32_t nReports = 0u;
  private:
    VPU::VPUDeviceContext *ctx = nullptr;
    Device *device = nullptr;
    ze_event_handle_t eventHandle;

    bool initialized = false;
};

} // namespace L0
