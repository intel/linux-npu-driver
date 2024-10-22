/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/tools/source/metrics/metric.hpp"
#include "level_zero_driver/tools/source/metrics/metric_query.hpp"
#include "level_zero_driver/tools/source/metrics/metric_streamer.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/zet_api.h>
#include <level_zero/zet_ddi.h>

namespace L0 {
ze_result_t zetMetricGroupGet(zet_device_handle_t hDevice,
                              uint32_t *pCount,
                              zet_metric_group_handle_t *phMetricGroups) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Device::fromHandle(hDevice)->metricGroupGet(pCount, phMetricGroups));
}

ze_result_t zetMetricGroupGetProperties(zet_metric_group_handle_t hMetricGroup,
                                        zet_metric_group_properties_t *pProperties) {
    if (hMetricGroup == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::MetricGroup::fromHandle(hMetricGroup)->getProperties(pProperties));
}

ze_result_t zetMetricGet(zet_metric_group_handle_t hMetricGroup,
                         uint32_t *pCount,
                         zet_metric_handle_t *phMetrics) {
    if (hMetricGroup == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::MetricGroup::fromHandle(hMetricGroup)->getMetric(pCount, phMetrics));
}

ze_result_t zetMetricGetProperties(zet_metric_handle_t hMetric,
                                   zet_metric_properties_t *pProperties) {
    if (hMetric == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Metric::fromHandle(hMetric)->getProperties(pProperties));
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
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::MetricGroup::fromHandle(hMetricGroup)
            ->calculateMetricValues(type, rawDataSize, pRawData, pMetricValueCount, pMetricValues));
}

ze_result_t zetContextActivateMetricGroups(zet_context_handle_t hContext,
                                           zet_device_handle_t hDevice,
                                           uint32_t count,
                                           zet_metric_group_handle_t *phMetricGroups) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Context::fromHandle(hContext)->activateMetricGroups(hDevice, count, phMetricGroups));
}

ze_result_t zetMetricStreamerOpen(zet_context_handle_t hContext,
                                  zet_device_handle_t hDevice,
                                  zet_metric_group_handle_t hMetricGroup,
                                  zet_metric_streamer_desc_t *pDesc,
                                  ze_event_handle_t hNotificationEvent,
                                  zet_metric_streamer_handle_t *phMetricStreamer) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Context::fromHandle(hContext)->metricStreamerOpen(hDevice,
                                                              hMetricGroup,
                                                              pDesc,
                                                              hNotificationEvent,
                                                              phMetricStreamer));
}

ze_result_t zetCommandListAppendMetricStreamerMarker(ze_command_list_handle_t hCommandList,
                                                     zet_metric_streamer_handle_t hMetricStreamer,
                                                     uint32_t value) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zetMetricStreamerClose(zet_metric_streamer_handle_t hMetricStreamer) {
    if (hMetricStreamer == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::MetricStreamer::fromHandle(hMetricStreamer)->close());
}

ze_result_t zetMetricStreamerReadData(zet_metric_streamer_handle_t hMetricStreamer,
                                      uint32_t maxReportCount,
                                      size_t *pRawDataSize,
                                      uint8_t *pRawData) {
    if (hMetricStreamer == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::MetricStreamer::fromHandle(hMetricStreamer)
                                       ->readData(maxReportCount, pRawDataSize, pRawData));
}

ze_result_t zetMetricQueryPoolCreate(zet_context_handle_t hContext,
                                     zet_device_handle_t hDevice,
                                     zet_metric_group_handle_t hMetricGroup,
                                     const zet_metric_query_pool_desc_t *desc,
                                     zet_metric_query_pool_handle_t *phMetricQueryPool) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Context::fromHandle(hContext)->createMetricQueryPool(hDevice,
                                                                 hMetricGroup,
                                                                 desc,
                                                                 phMetricQueryPool));
}

ze_result_t zetMetricQueryPoolDestroy(zet_metric_query_pool_handle_t hMetricQueryPool) {
    if (hMetricQueryPool == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::MetricQueryPool::fromHandle(hMetricQueryPool)->destroy());
}

ze_result_t zetMetricQueryCreate(zet_metric_query_pool_handle_t hMetricQueryPool,
                                 uint32_t index,
                                 zet_metric_query_handle_t *phMetricQuery) {
    if (hMetricQueryPool == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::MetricQueryPool::fromHandle(hMetricQueryPool)->createMetricQuery(index, phMetricQuery));
}

ze_result_t zetMetricQueryDestroy(zet_metric_query_handle_t hMetricQuery) {
    if (hMetricQuery == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::MetricQuery::fromHandle(hMetricQuery)->destroy());
}

ze_result_t zetMetricQueryReset(zet_metric_query_handle_t hMetricQuery) {
    if (hMetricQuery == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::MetricQuery::fromHandle(hMetricQuery)->reset());
}

ze_result_t zetCommandListAppendMetricQueryBegin(zet_command_list_handle_t hCommandList,
                                                 zet_metric_query_handle_t hMetricQuery) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::CommandList::fromHandle(hCommandList)->appendMetricQueryBegin(hMetricQuery));
}

ze_result_t zetCommandListAppendMetricQueryEnd(zet_command_list_handle_t hCommandList,
                                               zet_metric_query_handle_t hMetricQuery,
                                               ze_event_handle_t hSignalEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::CommandList::fromHandle(hCommandList)
            ->appendMetricQueryEnd(hMetricQuery, hSignalEvent, numWaitEvents, phWaitEvents));
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
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::MetricQuery::fromHandle(hMetricQuery)->getData(pRawDataSize, pRawData));
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnActivateMetricGroups = L0::zetContextActivateMetricGroups;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnAppendMetricStreamerMarker = L0::zetCommandListAppendMetricStreamerMarker;

    pDdiTable->pfnAppendMetricQueryBegin = L0::zetCommandListAppendMetricQueryBegin;

    pDdiTable->pfnAppendMetricQueryEnd = L0::zetCommandListAppendMetricQueryEnd;

    pDdiTable->pfnAppendMetricMemoryBarrier = L0::zetCommandListAppendMetricMemoryBarrier;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGet = L0::zetMetricGet;

    pDdiTable->pfnGetProperties = L0::zetMetricGetProperties;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGet = L0::zetMetricGroupGet;

    pDdiTable->pfnGetProperties = L0::zetMetricGroupGetProperties;

    pDdiTable->pfnCalculateMetricValues = L0::zetMetricGroupCalculateMetricValues;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnCreate = L0::zetMetricQueryCreate;

    pDdiTable->pfnDestroy = L0::zetMetricQueryDestroy;

    pDdiTable->pfnReset = L0::zetMetricQueryReset;

    pDdiTable->pfnGetData = L0::zetMetricQueryGetData;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnCreate = L0::zetMetricQueryPoolCreate;

    pDdiTable->pfnDestroy = L0::zetMetricQueryPoolDestroy;

    return result;
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnOpen = L0::zetMetricStreamerOpen;

    pDdiTable->pfnClose = L0::zetMetricStreamerClose;

    pDdiTable->pfnReadData = L0::zetMetricStreamerReadData;

    return result;
}
}
