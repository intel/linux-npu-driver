/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <ze_api.h>
#include <zet_ddi.h>

namespace L0 {
extern zet_context_dditable_t zetContextDdiTable;
extern zet_command_list_dditable_t zetCommandListDdiTable;
extern zet_metric_dditable_t zetMetricDdiTable;
extern zet_metric_group_dditable_t zetMetricGroupDdiTable;
extern zet_metric_streamer_dditable_t zetMetricStreamerDdiTable;
extern zet_metric_query_pool_dditable_t zetMetricQueryPoolDdiTable;
extern zet_metric_query_dditable_t zetMetricQueryDdiTable;
extern zet_device_exp_dditable_t zetDeviceExpDdiTable;
} // namespace L0

zet_dditable_driver_t toolsDdiTable = {
    .version = ZE_API_VERSION_CURRENT,
    .isValidFlag = 1,
    .MetricProgrammableExp = nullptr,
    .MetricTracerExp = nullptr,
    .MetricDecoderExp = nullptr,
    .Device = nullptr,
    .DeviceExp = &L0::zetDeviceExpDdiTable,
    .Context = &L0::zetContextDdiTable,
    .CommandList = &L0::zetCommandListDdiTable,
    .CommandListExp = nullptr,
    .Module = nullptr,
    .Kernel = nullptr,
    .Metric = &L0::zetMetricDdiTable,
    .MetricExp = nullptr,
    .MetricGroup = &L0::zetMetricGroupDdiTable,
    .MetricGroupExp = nullptr,
    .MetricStreamer = &L0::zetMetricStreamerDdiTable,
    .MetricQueryPool = &L0::zetMetricQueryPoolDdiTable,
    .MetricQuery = &L0::zetMetricQueryDdiTable,
    .TracerExp = nullptr,
    .Debug = nullptr,
};