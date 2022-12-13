/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/tools/source/metrics/metric.hpp"
#include "level_zero_driver/tools/source/metrics/metric_query.hpp"
#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"

#include <level_zero/zet_api.h>

namespace L0 {
ze_result_t zetMetricGroupGet(zet_device_handle_t hDevice,
                              uint32_t *pCount,
                              zet_metric_group_handle_t *phMetricGroups) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Device::fromHandle(hDevice)->metricGroupGet(pCount, phMetricGroups);
}

ze_result_t zetMetricGroupGetProperties(zet_metric_group_handle_t hMetricGroup,
                                        zet_metric_group_properties_t *pProperties) {
    if (hMetricGroup == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::MetricGroup::fromHandle(hMetricGroup)->getProperties(pProperties);
}

ze_result_t zetMetricGet(zet_metric_group_handle_t hMetricGroup,
                         uint32_t *pCount,
                         zet_metric_handle_t *phMetrics) {
    if (hMetricGroup == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::MetricGroup::fromHandle(hMetricGroup)->getMetric(pCount, phMetrics);
}

ze_result_t zetMetricGetProperties(zet_metric_handle_t hMetric,
                                   zet_metric_properties_t *pProperties) {
    if (hMetric == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Metric::fromHandle(hMetric)->getProperties(pProperties);
}

ze_result_t zetMetricGroupCalculateMetricValues(zet_metric_group_handle_t hMetricGroup,
                                                zet_metric_group_calculation_type_t type,
                                                size_t rawDataSize,
                                                const uint8_t *pRawData,
                                                uint32_t *pMetricValueCount,
                                                zet_typed_value_t *pMetricValues) {
    if (hMetricGroup == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::MetricGroup::fromHandle(hMetricGroup)
        ->calculateMetricValues(type, rawDataSize, pRawData, pMetricValueCount, pMetricValues);
}

ze_result_t zetContextActivateMetricGroups(zet_context_handle_t hContext,
                                           zet_device_handle_t hDevice,
                                           uint32_t count,
                                           zet_metric_group_handle_t *phMetricGroups) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Context::fromHandle(hContext)->activateMetricGroups(hDevice, count, phMetricGroups);
}

ze_result_t zetMetricQueryPoolCreate(zet_context_handle_t hContext,
                                     zet_device_handle_t hDevice,
                                     zet_metric_group_handle_t hMetricGroup,
                                     const zet_metric_query_pool_desc_t *desc,
                                     zet_metric_query_pool_handle_t *phMetricQueryPool) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::Context::fromHandle(hContext)->createMetricQueryPool(hDevice,
                                                                    hMetricGroup,
                                                                    desc,
                                                                    phMetricQueryPool);
}

ze_result_t zetMetricQueryPoolDestroy(zet_metric_query_pool_handle_t hMetricQueryPool) {
    if (hMetricQueryPool == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::MetricQueryPool::fromHandle(hMetricQueryPool)->destroy();
}

ze_result_t zetMetricQueryCreate(zet_metric_query_pool_handle_t hMetricQueryPool,
                                 uint32_t index,
                                 zet_metric_query_handle_t *phMetricQuery) {
    if (hMetricQueryPool == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::MetricQueryPool::fromHandle(hMetricQueryPool)
        ->createMetricQuery(index, phMetricQuery);
}

ze_result_t zetMetricQueryDestroy(zet_metric_query_handle_t hMetricQuery) {
    if (hMetricQuery == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::MetricQuery::fromHandle(hMetricQuery)->destroy();
}

ze_result_t zetMetricQueryReset(zet_metric_query_handle_t hMetricQuery) {
    if (hMetricQuery == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::MetricQuery::fromHandle(hMetricQuery)->reset();
}

ze_result_t zetCommandListAppendMetricQueryBegin(zet_command_list_handle_t hCommandList,
                                                 zet_metric_query_handle_t hMetricQuery) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::CommandList::fromHandle(hCommandList)->appendMetricQueryBegin(hMetricQuery);
}

ze_result_t zetCommandListAppendMetricQueryEnd(zet_command_list_handle_t hCommandList,
                                               zet_metric_query_handle_t hMetricQuery,
                                               ze_event_handle_t hSignalEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::CommandList::fromHandle(hCommandList)
        ->appendMetricQueryEnd(hMetricQuery, hSignalEvent, numWaitEvents, phWaitEvents);
}

ze_result_t zetCommandListAppendMetricMemoryBarrier(zet_command_list_handle_t hCommandList) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zetMetricQueryGetData(zet_metric_query_handle_t hMetricQuery,
                                  size_t *pRawDataSize,
                                  uint8_t *pRawData) {
    if (hMetricQuery == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::MetricQuery::fromHandle(hMetricQuery)->getData(pRawDataSize, pRawData);
}
} // namespace L0

extern "C" {
ZE_DLLEXPORT ze_result_t ZE_APICALL zetMetricGroupGet(zet_device_handle_t hDevice,
                                                      uint32_t *pCount,
                                                      zet_metric_group_handle_t *phMetricGroups) {
    return L0::zetMetricGroupGet(hDevice, pCount, phMetricGroups);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zetMetricGroupGetProperties(zet_metric_group_handle_t hMetricGroup,
                            zet_metric_group_properties_t *pProperties) {
    return L0::zetMetricGroupGetProperties(hMetricGroup, pProperties);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL zetMetricGet(zet_metric_group_handle_t hMetricGroup,
                                                 uint32_t *pCount,
                                                 zet_metric_handle_t *phMetrics) {
    return L0::zetMetricGet(hMetricGroup, pCount, phMetrics);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL zetMetricGetProperties(zet_metric_handle_t hMetric,
                                                           zet_metric_properties_t *pProperties) {
    return L0::zetMetricGetProperties(hMetric, pProperties);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zetMetricGroupCalculateMetricValues(zet_metric_group_handle_t hMetricGroup,
                                    zet_metric_group_calculation_type_t type,
                                    size_t rawDataSize,
                                    const uint8_t *pRawData,
                                    uint32_t *pMetricValueCount,
                                    zet_typed_value_t *pMetricValues) {
    return L0::zetMetricGroupCalculateMetricValues(hMetricGroup,
                                                   type,
                                                   rawDataSize,
                                                   pRawData,
                                                   pMetricValueCount,
                                                   pMetricValues);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zetContextActivateMetricGroups(zet_context_handle_t hContext,
                               zet_device_handle_t hDevice,
                               uint32_t count,
                               zet_metric_group_handle_t *phMetricGroups) {
    return L0::zetContextActivateMetricGroups(hContext, hDevice, count, phMetricGroups);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zetMetricStreamerOpen(zet_context_handle_t hContext,
                      zet_device_handle_t hDevice,
                      zet_metric_group_handle_t hMetricGroup,
                      zet_metric_streamer_desc_t *pDesc,
                      ze_event_handle_t hNotificationEvent,
                      zet_metric_streamer_handle_t *phMetricStreamer) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zetCommandListAppendMetricStreamerMarker(ze_command_list_handle_t hCommandList,
                                         zet_metric_streamer_handle_t hMetricStreamer,
                                         uint32_t value) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zetMetricStreamerClose(zet_metric_streamer_handle_t hMetricStreamer) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zetMetricStreamerReadData(zet_metric_streamer_handle_t hMetricStreamer,
                          uint32_t maxReportCount,
                          size_t *pRawDataSize,
                          uint8_t *pRawData) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zetMetricQueryPoolCreate(zet_context_handle_t hContext,
                         zet_device_handle_t hDevice,
                         zet_metric_group_handle_t hMetricGroup,
                         const zet_metric_query_pool_desc_t *desc,
                         zet_metric_query_pool_handle_t *phMetricQueryPool) {
    return L0::zetMetricQueryPoolCreate(hContext, hDevice, hMetricGroup, desc, phMetricQueryPool);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zetMetricQueryPoolDestroy(zet_metric_query_pool_handle_t hMetricQueryPool) {
    return L0::zetMetricQueryPoolDestroy(hMetricQueryPool);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zetMetricQueryCreate(zet_metric_query_pool_handle_t hMetricQueryPool,
                     uint32_t index,
                     zet_metric_query_handle_t *phMetricQuery) {
    return L0::zetMetricQueryCreate(hMetricQueryPool, index, phMetricQuery);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL zetMetricQueryDestroy(zet_metric_query_handle_t hMetricQuery) {
    return L0::zetMetricQueryDestroy(hMetricQuery);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL zetMetricQueryReset(zet_metric_query_handle_t hMetricQuery) {
    return L0::zetMetricQueryReset(hMetricQuery);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zetCommandListAppendMetricQueryBegin(zet_command_list_handle_t hCommandList,
                                     zet_metric_query_handle_t hMetricQuery) {
    return L0::zetCommandListAppendMetricQueryBegin(hCommandList, hMetricQuery);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zetCommandListAppendMetricQueryEnd(zet_command_list_handle_t hCommandList,
                                   zet_metric_query_handle_t hMetricQuery,
                                   ze_event_handle_t hSignalEvent,
                                   uint32_t numWaitEvents,
                                   ze_event_handle_t *phWaitEvents) {
    return L0::zetCommandListAppendMetricQueryEnd(hCommandList,
                                                  hMetricQuery,
                                                  hSignalEvent,
                                                  numWaitEvents,
                                                  phWaitEvents);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zetCommandListAppendMetricMemoryBarrier(zet_command_list_handle_t hCommandList) {
    return L0::zetCommandListAppendMetricMemoryBarrier(hCommandList);
}

ZE_DLLEXPORT ze_result_t ZE_APICALL zetMetricQueryGetData(zet_metric_query_handle_t hMetricQuery,
                                                          size_t *pRawDataSize,
                                                          uint8_t *pRawData) {
    return L0::zetMetricQueryGetData(hMetricQuery, pRawDataSize, pRawData);
}
} // extern "C"
