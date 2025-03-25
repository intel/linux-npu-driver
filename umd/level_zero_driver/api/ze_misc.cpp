/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/api/trace/trace_ze_api_ddi.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeGetImageProcAddrTable(ze_api_version_t version,
                                                            ze_image_dditable_t *pDdiTable) {
    trace_zeGetImageProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGetProperties = nullptr;
    pDdiTable->pfnCreate = nullptr;
    pDdiTable->pfnDestroy = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetImageProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetModuleProcAddrTable(ze_api_version_t version,
                                                             ze_module_dditable_t *pDdiTable) {
    trace_zeGetModuleProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnCreate = nullptr;
    pDdiTable->pfnDestroy = nullptr;
    pDdiTable->pfnDynamicLink = nullptr;
    pDdiTable->pfnGetNativeBinary = nullptr;
    pDdiTable->pfnGetGlobalPointer = nullptr;
    pDdiTable->pfnGetKernelNames = nullptr;
    pDdiTable->pfnGetFunctionPointer = nullptr;
    pDdiTable->pfnGetProperties = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetModuleProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetModuleBuildLogProcAddrTable(ze_api_version_t version,
                                 ze_module_build_log_dditable_t *pDdiTable) {
    trace_zeGetModuleBuildLogProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnDestroy = nullptr;
    pDdiTable->pfnGetString = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetModuleBuildLogProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetKernelProcAddrTable(ze_api_version_t version,
                                                             ze_kernel_dditable_t *pDdiTable) {
    trace_zeGetKernelProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnCreate = nullptr;
    pDdiTable->pfnDestroy = nullptr;
    pDdiTable->pfnSetGroupSize = nullptr;
    pDdiTable->pfnSuggestGroupSize = nullptr;
    pDdiTable->pfnSuggestMaxCooperativeGroupCount = nullptr;
    pDdiTable->pfnSetArgumentValue = nullptr;
    pDdiTable->pfnSetIndirectAccess = nullptr;
    pDdiTable->pfnGetIndirectAccess = nullptr;
    pDdiTable->pfnGetSourceAttributes = nullptr;
    pDdiTable->pfnGetProperties = nullptr;
    pDdiTable->pfnSetCacheConfig = nullptr;
    pDdiTable->pfnGetName = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetKernelProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetSamplerProcAddrTable(ze_api_version_t version,
                                                              ze_sampler_dditable_t *pDdiTable) {
    trace_zeGetSamplerProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnCreate = nullptr;
    pDdiTable->pfnDestroy = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetSamplerProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetRTASBuilderExpProcAddrTable(ze_api_version_t version,
                                 ze_rtas_builder_exp_dditable_t *pDdiTable) {
    trace_zeGetRTASBuilderExpProcAddrTable(version, pDdiTable);
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
    pDdiTable->pfnGetBuildPropertiesExp = nullptr;
    pDdiTable->pfnBuildExp = nullptr;
    pDdiTable->pfnDestroyExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetRTASBuilderExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetRTASParallelOperationExpProcAddrTable(ze_api_version_t version,
                                           ze_rtas_parallel_operation_exp_dditable_t *pDdiTable) {
    trace_zeGetRTASParallelOperationExpProcAddrTable(version, pDdiTable);
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
    pDdiTable->pfnGetPropertiesExp = nullptr;
    pDdiTable->pfnJoinExp = nullptr;
    pDdiTable->pfnDestroyExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetRTASParallelOperationExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetDriverExpProcAddrTable(ze_api_version_t version, ze_driver_exp_dditable_t *pDdiTable) {
    trace_zeGetDriverExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnRTASFormatCompatibilityCheckExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetDriverExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetDeviceExpProcAddrTable(ze_api_version_t version, ze_device_exp_dditable_t *pDdiTable) {
    trace_zeGetDeviceExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGetFabricVertexExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetDeviceExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetEventExpProcAddrTable(ze_api_version_t version,
                                                               ze_event_exp_dditable_t *pDdiTable) {
    trace_zeGetEventExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnQueryTimestampsExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetEventExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetImageExpProcAddrTable(ze_api_version_t version,
                                                               ze_image_exp_dditable_t *pDdiTable) {
    trace_zeGetImageExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGetMemoryPropertiesExp = nullptr;
    pDdiTable->pfnViewCreateExp = nullptr;
    pDdiTable->pfnGetDeviceOffsetExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetImageExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetKernelExpProcAddrTable(ze_api_version_t version, ze_kernel_exp_dditable_t *pDdiTable) {
    trace_zeGetKernelExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnSetGlobalOffsetExp = nullptr;
    pDdiTable->pfnSchedulingHintExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetKernelExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetMemExpProcAddrTable(ze_api_version_t version,
                                                             ze_mem_exp_dditable_t *pDdiTable) {
    trace_zeGetMemExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGetIpcHandleFromFileDescriptorExp = nullptr;
    pDdiTable->pfnGetFileDescriptorFromIpcHandleExp = nullptr;
    pDdiTable->pfnSetAtomicAccessAttributeExp = nullptr;
    pDdiTable->pfnGetAtomicAccessAttributeExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetMemExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetFabricEdgeExpProcAddrTable(ze_api_version_t version,
                                ze_fabric_edge_exp_dditable_t *pDdiTable) {
    trace_zeGetFabricEdgeExpProcAddrTable(version, pDdiTable);
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
    pDdiTable->pfnGetVerticesExp = nullptr;
    pDdiTable->pfnGetPropertiesExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetFabricEdgeExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetFabricVertexExpProcAddrTable(ze_api_version_t version,
                                  ze_fabric_vertex_exp_dditable_t *pDdiTable) {
    trace_zeGetFabricVertexExpProcAddrTable(version, pDdiTable);
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
    pDdiTable->pfnGetSubVerticesExp = nullptr;
    pDdiTable->pfnGetPropertiesExp = nullptr;
    pDdiTable->pfnGetDeviceExp = nullptr;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetFabricVertexExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
