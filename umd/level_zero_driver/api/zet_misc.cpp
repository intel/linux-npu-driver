/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <cstddef>

#include "level_zero/loader/ze_loader.h"
#include "level_zero_driver/api/trace/trace_zet_api_ddi.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <dlfcn.h>
#include <level_zero/ze_api.h>
#include <level_zero/zet_ddi.h>
#include <string>

extern "C" {
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
    trace_zetGetDeviceProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetDebugProperties = nullptr;

exit:
    trace_zetGetDeviceProcAddrTable(ret, version, pDdiTable);
    return ret;
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
    trace_zetGetKernelProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetProfileInfo = nullptr;

exit:
    trace_zetGetKernelProcAddrTable(ret, version, pDdiTable);
    return ret;
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
    trace_zetGetModuleProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnGetDebugInfo = nullptr;

exit:
    trace_zetGetModuleProcAddrTable(ret, version, pDdiTable);
    return ret;
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
    trace_zetGetDebugProcAddrTable(version, pDdiTable);
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

exit:
    trace_zetGetDebugProcAddrTable(ret, version, pDdiTable);
    return ret;
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
    trace_zetGetTracerExpProcAddrTable(version, pDdiTable);
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

    pDdiTable->pfnCreate = nullptr;

    pDdiTable->pfnDestroy = nullptr;

    pDdiTable->pfnSetPrologues = nullptr;

    pDdiTable->pfnSetEpilogues = nullptr;

    pDdiTable->pfnSetEnabled = nullptr;

exit:
    trace_zetGetTracerExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zetGetMetricDecoderExpProcAddrTable(ze_api_version_t version,
                                    zet_metric_decoder_exp_dditable_t *pDdiTable) {
    trace_zetGetMetricDecoderExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnCreateExp = nullptr;
    pDdiTable->pfnDestroyExp = nullptr;
    pDdiTable->pfnGetDecodableMetricsExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zetGetMetricDecoderExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zetGetMetricTracerExpProcAddrTable(ze_api_version_t version,
                                   zet_metric_tracer_exp_dditable_t *pDdiTable) {
    trace_zetGetMetricTracerExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnCreateExp = nullptr;
    pDdiTable->pfnDestroyExp = nullptr;
    pDdiTable->pfnEnableExp = nullptr;
    pDdiTable->pfnDisableExp = nullptr;
    pDdiTable->pfnReadDataExp = nullptr;
    pDdiTable->pfnDecodeExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zetGetMetricTracerExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zetGetMetricProgrammableExpProcAddrTable(ze_api_version_t version,
                                         zet_metric_programmable_exp_dditable_t *pDdiTable) {
    trace_zetGetMetricProgrammableExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGetExp = nullptr;
    pDdiTable->pfnGetPropertiesExp = nullptr;
    pDdiTable->pfnGetParamInfoExp = nullptr;
    pDdiTable->pfnGetParamValueInfoExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zetGetMetricProgrammableExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zetGetDeviceExpProcAddrTable(ze_api_version_t version, zet_device_exp_dditable_t *pDdiTable) {
    trace_zetGetDeviceExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGetConcurrentMetricGroupsExp = nullptr;
    pDdiTable->pfnCreateMetricGroupsFromMetricsExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zetGetDeviceExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zetGetMetricExpProcAddrTable(ze_api_version_t version, zet_metric_exp_dditable_t *pDdiTable) {
    trace_zetGetMetricExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnCreateFromProgrammableExp2 = nullptr;
    pDdiTable->pfnCreateFromProgrammableExp = nullptr;
    pDdiTable->pfnDestroyExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zetGetMetricExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zetGetMetricGroupExpProcAddrTable(ze_api_version_t version,
                                  zet_metric_group_exp_dditable_t *pDdiTable) {
    trace_zetGetMetricGroupExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnCreateExp = nullptr;
    pDdiTable->pfnCalculateMultipleMetricValuesExp = nullptr;
    pDdiTable->pfnGetGlobalTimestampsExp = nullptr;
    pDdiTable->pfnGetExportDataExp = nullptr;
    pDdiTable->pfnCalculateMetricExportDataExp = nullptr;
    pDdiTable->pfnAddMetricExp = nullptr;
    pDdiTable->pfnRemoveMetricExp = nullptr;
    pDdiTable->pfnCloseExp = nullptr;
    pDdiTable->pfnDestroyExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zetGetMetricGroupExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}

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

ze_result_t translateHandle(zel_handle_type_t type, void *handler, void **pHandler) {
    void *loaderHandle = getLoaderHandle();
    if (loaderHandle == nullptr) {
        LOG_E("Failed to open libze_loader.so.1 library");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    static void *functionPointer = dlsym(loaderHandle, "zelLoaderTranslateHandle");
    if (functionPointer == nullptr) {
        LOG_E("Failed to get 'zelLoaderTranslateHandle' from libze_loader.so.1, reason: %s",
              dlerror());
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    static auto *pLoaderTranslateHandler =
        reinterpret_cast<decltype(zelLoaderTranslateHandle) *>(functionPointer);

    auto result = pLoaderTranslateHandler(type, handler, pHandler);
    if (result != ZE_RESULT_SUCCESS)
        LOG_E("Failed to translate handler of type %i", type);

    return result;
}

} // namespace L0
