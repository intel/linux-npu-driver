/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL zeGetImageProcAddrTable(ze_api_version_t version,
                                                            ze_image_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnGetProperties = nullptr; // zeImageGetProperties
    pDdiTable->pfnCreate = nullptr;        // zeImageCreate
    pDdiTable->pfnDestroy = nullptr;       // zeImageDestroy
    return ZE_RESULT_SUCCESS;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetModuleProcAddrTable(ze_api_version_t version,
                                                             ze_module_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnCreate = nullptr;             // zeModuleCreate
    pDdiTable->pfnDestroy = nullptr;            // zeModuleDestroy
    pDdiTable->pfnDynamicLink = nullptr;        // zeModuleDynamicLink
    pDdiTable->pfnGetNativeBinary = nullptr;    // zeModuleGetNativeBinary
    pDdiTable->pfnGetGlobalPointer = nullptr;   // zeModuleGetGlobalPointer
    pDdiTable->pfnGetKernelNames = nullptr;     // zeModuleGetKernelNames
    pDdiTable->pfnGetFunctionPointer = nullptr; // zeModuleGetFunctionPointer
    pDdiTable->pfnGetProperties = nullptr;      // zeModuleGetProperties
    return ZE_RESULT_SUCCESS;
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetModuleBuildLogProcAddrTable(ze_api_version_t version,
                                 ze_module_build_log_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnDestroy = nullptr;   // zeModuleBuildLogDestroy
    pDdiTable->pfnGetString = nullptr; // zeModuleBuildLogGetString
    return ZE_RESULT_SUCCESS;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetKernelProcAddrTable(ze_api_version_t version,
                                                             ze_kernel_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnCreate = nullptr;           // zeKernelCreate
    pDdiTable->pfnDestroy = nullptr;          // zeKernelDestroy
    pDdiTable->pfnSetGroupSize = nullptr;     // zeKernelSetGroupSize
    pDdiTable->pfnSuggestGroupSize = nullptr; // zeKernelSuggestGroupSize
    pDdiTable->pfnSuggestMaxCooperativeGroupCount =
        nullptr;                                 // zeKernelSuggestMaxCooperativeGroupCount
    pDdiTable->pfnSetArgumentValue = nullptr;    // zeKernelSetArgumentValue
    pDdiTable->pfnSetIndirectAccess = nullptr;   // zeKernelSetIndirectAccess
    pDdiTable->pfnGetIndirectAccess = nullptr;   // zeKernelGetIndirectAccess
    pDdiTable->pfnGetSourceAttributes = nullptr; // zeKernelGetSourceAttributes
    pDdiTable->pfnGetProperties = nullptr;       // zeKernelGetProperties
    pDdiTable->pfnSetCacheConfig = nullptr;      // zeKernelSetCacheConfig
    pDdiTable->pfnGetName = nullptr;             // zeKernelGetName
    return ZE_RESULT_SUCCESS;
}

ZE_APIEXPORT ze_result_t ZE_APICALL zeGetSamplerProcAddrTable(ze_api_version_t version,
                                                              ze_sampler_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnCreate = nullptr;  // zeSamplerCreate
    pDdiTable->pfnDestroy = nullptr; // zeSamplerDestroy
    return ZE_RESULT_SUCCESS;
}
}
