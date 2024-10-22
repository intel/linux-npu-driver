/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <cstddef>

#include "level_zero/loader/ze_loader.h"
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
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    ze_result_t result = ZE_RESULT_SUCCESS;

    pDdiTable->pfnGetDebugProperties = nullptr;

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
