/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/api/trace/trace_zet_api.hpp"
#include "level_zero_driver/api/trace/trace_zet_api_ddi.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/cmdlist.hpp"
#include "level_zero_driver/source/context.hpp"
#include "level_zero_driver/source/device.hpp"
#include "level_zero_driver/source/event.hpp"
#include "level_zero_driver/source/metric.hpp"
#include "level_zero_driver/source/metric_query.hpp"
#include "level_zero_driver/source/metric_streamer.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/zet_api.h>
#include <level_zero/zet_ddi.h>

namespace L0 {
ze_result_t zetMetricGroupGet(zet_device_handle_t hDevice,
                              uint32_t *pCount,
                              zet_metric_group_handle_t *phMetricGroups) {
    trace_zetMetricGroupGet(hDevice, pCount, phMetricGroups);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        L0::Device::fromHandle(hDevice)->metricGroupGet(pCount, phMetricGroups));

exit:
    trace_zetMetricGroupGet(ret, hDevice, pCount, phMetricGroups);
    return ret;
}

ze_result_t zetMetricGroupGetProperties(zet_metric_group_handle_t hMetricGroup,
                                        zet_metric_group_properties_t *pProperties) {
    trace_zetMetricGroupGetProperties(hMetricGroup, pProperties);
    ze_result_t ret;

    if (hMetricGroup == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::MetricGroup::fromHandle(hMetricGroup)->getProperties(pProperties));

exit:
    trace_zetMetricGroupGetProperties(ret, hMetricGroup, pProperties);
    return ret;
}

ze_result_t zetMetricGet(zet_metric_group_handle_t hMetricGroup,
                         uint32_t *pCount,
                         zet_metric_handle_t *phMetrics) {
    trace_zetMetricGet(hMetricGroup, pCount, phMetrics);
    ze_result_t ret;

    if (hMetricGroup == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        L0::MetricGroup::fromHandle(hMetricGroup)->getMetric(pCount, phMetrics));

exit:
    trace_zetMetricGet(ret, hMetricGroup, pCount, phMetrics);
    return ret;
}

ze_result_t zetMetricGetProperties(zet_metric_handle_t hMetric,
                                   zet_metric_properties_t *pProperties) {
    trace_zetMetricGetProperties(hMetric, pProperties);
    ze_result_t ret;

    if (hMetric == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::Metric::fromHandle(hMetric)->getProperties(pProperties));

exit:
    trace_zetMetricGetProperties(ret, hMetric, pProperties);
    return ret;
}

ze_result_t zetMetricGroupCalculateMetricValues(zet_metric_group_handle_t hMetricGroup,
                                                zet_metric_group_calculation_type_t type,
                                                size_t rawDataSize,
                                                const uint8_t *pRawData,
                                                uint32_t *pMetricValueCount,
                                                zet_typed_value_t *pMetricValues) {
    trace_zetMetricGroupCalculateMetricValues(hMetricGroup,
                                              type,
                                              rawDataSize,
                                              pRawData,
                                              pMetricValueCount,
                                              pMetricValues);
    ze_result_t ret;

    if (hMetricGroup == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        L0::MetricGroup::fromHandle(hMetricGroup)
            ->calculateMetricValues(type, rawDataSize, pRawData, pMetricValueCount, pMetricValues));

exit:
    trace_zetMetricGroupCalculateMetricValues(ret,
                                              hMetricGroup,
                                              type,
                                              rawDataSize,
                                              pRawData,
                                              pMetricValueCount,
                                              pMetricValues);
    return ret;
}

ze_result_t zetContextActivateMetricGroups(zet_context_handle_t hContext,
                                           zet_device_handle_t hDevice,
                                           uint32_t count,
                                           zet_metric_group_handle_t *phMetricGroups) {
    trace_zetContextActivateMetricGroups(hContext, hDevice, count, phMetricGroups);
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        L0::Context::fromHandle(hContext)->activateMetricGroups(hDevice, count, phMetricGroups));

exit:
    trace_zetContextActivateMetricGroups(ret, hContext, hDevice, count, phMetricGroups);
    return ret;
}

ze_result_t zetMetricStreamerOpen(zet_context_handle_t hContext,
                                  zet_device_handle_t hDevice,
                                  zet_metric_group_handle_t hMetricGroup,
                                  zet_metric_streamer_desc_t *pDesc,
                                  ze_event_handle_t hNotificationEvent,
                                  zet_metric_streamer_handle_t *phMetricStreamer) {
    trace_zetMetricStreamerOpen(hContext,
                                hDevice,
                                hMetricGroup,
                                pDesc,
                                hNotificationEvent,
                                phMetricStreamer);
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        L0::Context::fromHandle(hContext)->metricStreamerOpen(hDevice,
                                                                              hMetricGroup,
                                                                              pDesc,
                                                                              hNotificationEvent,
                                                                              phMetricStreamer));

exit:
    trace_zetMetricStreamerOpen(ret,
                                hContext,
                                hDevice,
                                hMetricGroup,
                                pDesc,
                                hNotificationEvent,
                                phMetricStreamer);
    return ret;
}

ze_result_t zetCommandListAppendMetricStreamerMarker(ze_command_list_handle_t hCommandList,
                                                     zet_metric_streamer_handle_t hMetricStreamer,
                                                     uint32_t value) {
    trace_zetCommandListAppendMetricStreamerMarker(hCommandList, hMetricStreamer, value);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zetCommandListAppendMetricStreamerMarker(ret, hCommandList, hMetricStreamer, value);
    return ret;
}

ze_result_t zetMetricStreamerClose(zet_metric_streamer_handle_t hMetricStreamer) {
    trace_zetMetricStreamerClose(hMetricStreamer);
    ze_result_t ret;

    if (hMetricStreamer == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::MetricStreamer::fromHandle(hMetricStreamer)->close());

exit:
    trace_zetMetricStreamerClose(ret, hMetricStreamer);
    return ret;
}

ze_result_t zetMetricStreamerReadData(zet_metric_streamer_handle_t hMetricStreamer,
                                      uint32_t maxReportCount,
                                      size_t *pRawDataSize,
                                      uint8_t *pRawData) {
    trace_zetMetricStreamerReadData(hMetricStreamer, maxReportCount, pRawDataSize, pRawData);
    ze_result_t ret;

    if (hMetricStreamer == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        L0::MetricStreamer::fromHandle(hMetricStreamer)
                            ->readData(maxReportCount, pRawDataSize, pRawData));

exit:
    trace_zetMetricStreamerReadData(ret, hMetricStreamer, maxReportCount, pRawDataSize, pRawData);
    return ret;
}

ze_result_t zetMetricQueryPoolCreate(zet_context_handle_t hContext,
                                     zet_device_handle_t hDevice,
                                     zet_metric_group_handle_t hMetricGroup,
                                     const zet_metric_query_pool_desc_t *desc,
                                     zet_metric_query_pool_handle_t *phMetricQueryPool) {
    trace_zetMetricQueryPoolCreate(hContext, hDevice, hMetricGroup, desc, phMetricQueryPool);
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        L0::Context::fromHandle(hContext)->createMetricQueryPool(hDevice,
                                                                 hMetricGroup,
                                                                 desc,
                                                                 phMetricQueryPool));

exit:
    trace_zetMetricQueryPoolCreate(ret, hContext, hDevice, hMetricGroup, desc, phMetricQueryPool);
    return ret;
}

ze_result_t zetMetricQueryPoolDestroy(zet_metric_query_pool_handle_t hMetricQueryPool) {
    trace_zetMetricQueryPoolDestroy(hMetricQueryPool);
    ze_result_t ret;

    if (hMetricQueryPool == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::MetricQueryPool::fromHandle(hMetricQueryPool)->destroy());

exit:
    trace_zetMetricQueryPoolDestroy(ret, hMetricQueryPool);
    return ret;
}

ze_result_t zetMetricQueryCreate(zet_metric_query_pool_handle_t hMetricQueryPool,
                                 uint32_t index,
                                 zet_metric_query_handle_t *phMetricQuery) {
    trace_zetMetricQueryCreate(hMetricQueryPool, index, phMetricQuery);
    ze_result_t ret;

    if (hMetricQueryPool == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(index, phMetricQuery));

exit:
    trace_zetMetricQueryCreate(ret, hMetricQueryPool, index, phMetricQuery);
    return ret;
}

ze_result_t zetMetricQueryDestroy(zet_metric_query_handle_t hMetricQuery) {
    trace_zetMetricQueryDestroy(hMetricQuery);
    ze_result_t ret;

    if (hMetricQuery == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::MetricQuery::fromHandle(hMetricQuery)->destroy());

exit:
    trace_zetMetricQueryDestroy(ret, hMetricQuery);
    return ret;
}

ze_result_t zetMetricQueryReset(zet_metric_query_handle_t hMetricQuery) {
    trace_zetMetricQueryReset(hMetricQuery);
    ze_result_t ret;

    if (hMetricQuery == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, L0::MetricQuery::fromHandle(hMetricQuery)->reset());

exit:
    trace_zetMetricQueryReset(ret, hMetricQuery);
    return ret;
}

ze_result_t zetCommandListAppendMetricQueryBegin(zet_command_list_handle_t hCommandList,
                                                 zet_metric_query_handle_t hMetricQuery) {
    trace_zetCommandListAppendMetricQueryBegin(hCommandList, hMetricQuery);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::CommandList::fromHandle(hCommandList)->appendMetricQueryBegin(hMetricQuery));

exit:
    trace_zetCommandListAppendMetricQueryBegin(ret, hCommandList, hMetricQuery);
    return ret;
}

ze_result_t zetCommandListAppendMetricQueryEnd(zet_command_list_handle_t hCommandList,
                                               zet_metric_query_handle_t hMetricQuery,
                                               ze_event_handle_t hSignalEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
    trace_zetCommandListAppendMetricQueryEnd(hCommandList,
                                             hMetricQuery,
                                             hSignalEvent,
                                             numWaitEvents,
                                             phWaitEvents);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        L0::CommandList::fromHandle(hCommandList)
            ->appendMetricQueryEnd(hMetricQuery, hSignalEvent, numWaitEvents, phWaitEvents));

exit:
    trace_zetCommandListAppendMetricQueryEnd(ret,
                                             hCommandList,
                                             hMetricQuery,
                                             hSignalEvent,
                                             numWaitEvents,
                                             phWaitEvents);
    return ret;
}

ze_result_t zetCommandListAppendMetricMemoryBarrier(zet_command_list_handle_t hCommandList) {
    trace_zetCommandListAppendMetricMemoryBarrier(hCommandList);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zetCommandListAppendMetricMemoryBarrier(ret, hCommandList);
    return ret;
}

ze_result_t zetMetricQueryGetData(zet_metric_query_handle_t hMetricQuery,
                                  size_t *pRawDataSize,
                                  uint8_t *pRawData) {
    trace_zetMetricQueryGetData(hMetricQuery, pRawDataSize, pRawData);
    ze_result_t ret;

    if (hMetricQuery == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        L0::MetricQuery::fromHandle(hMetricQuery)->getData(pRawDataSize, pRawData));

exit:
    trace_zetMetricQueryGetData(ret, hMetricQuery, pRawDataSize, pRawData);
    return ret;
}
} // namespace L0

extern "C" {
///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Context table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zetGetContextProcAddrTable(
    ze_api_version_t version,         ///< [in] API version requested
    zet_context_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zetGetContextProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    ret = ZE_RESULT_SUCCESS;

    pDdiTable->pfnActivateMetricGroups = L0::zetContextActivateMetricGroups;

exit:
    trace_zetGetContextProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zetGetCommandListExpProcAddrTable(ze_api_version_t version,
                                  zet_command_list_exp_dditable_t *pDdiTable) {
    trace_zetGetCommandListExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    ret = ZE_RESULT_SUCCESS;

    pDdiTable->pfnAppendMarkerExp = nullptr;

exit:
    trace_zetGetCommandListExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's CommandList table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zetGetCommandListProcAddrTable(
    ze_api_version_t version,              ///< [in] API version requested
    zet_command_list_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zetGetCommandListProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    ret = ZE_RESULT_SUCCESS;

    pDdiTable->pfnAppendMetricStreamerMarker = L0::zetCommandListAppendMetricStreamerMarker;

    pDdiTable->pfnAppendMetricQueryBegin = L0::zetCommandListAppendMetricQueryBegin;

    pDdiTable->pfnAppendMetricQueryEnd = L0::zetCommandListAppendMetricQueryEnd;

    pDdiTable->pfnAppendMetricMemoryBarrier = L0::zetCommandListAppendMetricMemoryBarrier;

exit:
    trace_zetGetCommandListProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Metric table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zetGetMetricProcAddrTable(
    ze_api_version_t version,        ///< [in] API version requested
    zet_metric_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zetGetMetricProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    ret = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGet = L0::zetMetricGet;

    pDdiTable->pfnGetProperties = L0::zetMetricGetProperties;

exit:
    trace_zetGetMetricProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's MetricGroup table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zetGetMetricGroupProcAddrTable(
    ze_api_version_t version,              ///< [in] API version requested
    zet_metric_group_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zetGetMetricGroupProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    ret = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGet = L0::zetMetricGroupGet;

    pDdiTable->pfnGetProperties = L0::zetMetricGroupGetProperties;

    pDdiTable->pfnCalculateMetricValues = L0::zetMetricGroupCalculateMetricValues;

exit:
    trace_zetGetMetricGroupProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's MetricQuery table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zetGetMetricQueryProcAddrTable(
    ze_api_version_t version,              ///< [in] API version requested
    zet_metric_query_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zetGetMetricQueryProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    ret = ZE_RESULT_SUCCESS;

    pDdiTable->pfnCreate = L0::zetMetricQueryCreate;

    pDdiTable->pfnDestroy = L0::zetMetricQueryDestroy;

    pDdiTable->pfnReset = L0::zetMetricQueryReset;

    pDdiTable->pfnGetData = L0::zetMetricQueryGetData;

exit:
    trace_zetGetMetricQueryProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's MetricQueryPool table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zetGetMetricQueryPoolProcAddrTable(
    ze_api_version_t version, ///< [in] API version requested
    zet_metric_query_pool_dditable_t
        *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zetGetMetricQueryPoolProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    ret = ZE_RESULT_SUCCESS;

    pDdiTable->pfnCreate = L0::zetMetricQueryPoolCreate;

    pDdiTable->pfnDestroy = L0::zetMetricQueryPoolDestroy;

exit:
    trace_zetGetMetricQueryPoolProcAddrTable(ret, version, pDdiTable);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's MetricStreamer table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zetGetMetricStreamerProcAddrTable(
    ze_api_version_t version, ///< [in] API version requested
    zet_metric_streamer_dditable_t
        *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    trace_zetGetMetricStreamerProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    ret = ZE_RESULT_SUCCESS;

    pDdiTable->pfnOpen = L0::zetMetricStreamerOpen;

    pDdiTable->pfnClose = L0::zetMetricStreamerClose;

    pDdiTable->pfnReadData = L0::zetMetricStreamerReadData;

exit:
    trace_zetGetMetricStreamerProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
