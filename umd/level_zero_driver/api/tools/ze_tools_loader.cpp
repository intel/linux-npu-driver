/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/loader/ze_loader.h"
#include "level_zero_driver/api/tools/zet_metric.hpp"

#include <dlfcn.h>
#include <level_zero/ze_api.h>
#include <level_zero/zet_ddi.h>

#if defined(__cplusplus)
extern "C" {
#endif
///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Device table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zetGetDeviceProcAddrTable(
    ze_api_version_t version,        ///< [in] API version requested
    zet_device_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetDebugProperties = nullptr;

    return result;
}

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
/// @brief Exported function for filling application's Kernel table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zetGetKernelProcAddrTable(
    ze_api_version_t version,        ///< [in] API version requested
    zet_kernel_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetProfileInfo = nullptr;

    return result;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Module table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zetGetModuleProcAddrTable(
    ze_api_version_t version,        ///< [in] API version requested
    zet_module_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetDebugInfo = nullptr;

    return result;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Debug table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zetGetDebugProcAddrTable(
    ze_api_version_t version,       ///< [in] API version requested
    zet_debug_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnAttach = nullptr;

    pDdiTable->pfnDetach = nullptr;

    pDdiTable->pfnReadEvent = nullptr;

    pDdiTable->pfnAcknowledgeEvent = nullptr;

    pDdiTable->pfnInterrupt = nullptr;

    pDdiTable->pfnResume = nullptr;

    pDdiTable->pfnReadMemory = nullptr;

    pDdiTable->pfnWriteMemory = nullptr;

    pDdiTable->pfnGetRegisterSetProperties = nullptr;

    pDdiTable->pfnReadRegisters = nullptr;

    pDdiTable->pfnWriteRegisters = nullptr;

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

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's TracerExp table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL zetGetTracerExpProcAddrTable(
    ze_api_version_t version,            ///< [in] API version requested
    zet_tracer_exp_dditable_t *pDdiTable ///< [in,out] pointer to table of DDI function pointers
) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnCreate = nullptr;

    pDdiTable->pfnDestroy = nullptr;

    pDdiTable->pfnSetPrologues = nullptr;

    pDdiTable->pfnSetEpilogues = nullptr;

    pDdiTable->pfnSetEnabled = nullptr;

    return result;
}
#if defined(__cplusplus)
}
#endif

namespace L0 {

void *getLoaderHandle() {
    static void *loaderHandle = dlopen("libze_loader.so.1", RTLD_LAZY | RTLD_LOCAL);
    return loaderHandle;
}

std::string getLoaderVersion() {
    std::string version = "not available";
    void *loaderHandle = getLoaderHandle();

    if (loaderHandle == nullptr) {
        return version;
    }

    static void *functionPointer = dlsym(loaderHandle, "zelLoaderGetVersions");
    if (functionPointer == nullptr) {
        return version;
    }

    zel_component_version_t loaderVersion;
    size_t num = 1;
    static auto *pLoaderGetVersions =
        reinterpret_cast<decltype(zelLoaderGetVersions) *>(functionPointer);
    if (pLoaderGetVersions(&num, &loaderVersion) != ZE_RESULT_SUCCESS) {
        return version;
    }

    version = std::to_string(loaderVersion.component_lib_version.major) + ".";
    version += std::to_string(loaderVersion.component_lib_version.minor) + ".";
    version += std::to_string(loaderVersion.component_lib_version.patch);
    return version;
}
} // namespace L0