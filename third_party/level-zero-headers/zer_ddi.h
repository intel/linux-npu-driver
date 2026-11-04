/*
 *
 * Copyright (C) 2019-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 * @file zer_ddi.h
 * @version v1.18-r1.18.31
 *
 */
#ifndef _ZER_DDI_H
#define _ZER_DDI_H
#if defined(__cplusplus)
#pragma once
#endif
#include "zer_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for zerGetLastErrorDescription 
typedef ze_result_t (ZE_APICALL *zer_pfnGetLastErrorDescription_t)(
    const char**
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for zerTranslateDeviceHandleToIdentifier 
typedef uint32_t (ZE_APICALL *zer_pfnTranslateDeviceHandleToIdentifier_t)(
    ze_device_handle_t
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for zerTranslateIdentifierToDeviceHandle 
typedef ze_device_handle_t (ZE_APICALL *zer_pfnTranslateIdentifierToDeviceHandle_t)(
    uint32_t
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for zerGetDefaultContext 
typedef ze_context_handle_t (ZE_APICALL *zer_pfnGetDefaultContext_t)(
    void
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Table of Global functions pointers
typedef struct _zer_global_dditable_t
{
    zer_pfnGetLastErrorDescription_t                            pfnGetLastErrorDescription;
    zer_pfnTranslateDeviceHandleToIdentifier_t                  pfnTranslateDeviceHandleToIdentifier;
    zer_pfnTranslateIdentifierToDeviceHandle_t                  pfnTranslateIdentifierToDeviceHandle;
    zer_pfnGetDefaultContext_t                                  pfnGetDefaultContext;
} zer_global_dditable_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Exported function for filling application's Global table
///        with current process' addresses
///
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_UNINITIALIZED
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_VERSION
ZE_DLLEXPORT ze_result_t ZE_APICALL
zerGetGlobalProcAddrTable(
    ze_api_version_t version,                                               ///< [in] API version requested
    zer_global_dditable_t* pDdiTable                                        ///< [in,out] pointer to table of DDI function pointers
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Function-pointer for zerGetGlobalProcAddrTable
typedef ze_result_t (ZE_APICALL *zer_pfnGetGlobalProcAddrTable_t)(
    ze_api_version_t,
    zer_global_dditable_t*
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Container for all DDI tables
typedef struct _zer_dditable_t
{
    zer_global_dditable_t               Global;
} zer_dditable_t;
/// @brief Container for all DDI tables with version and tables set by the Driver
typedef struct _zer_dditable_driver_t
{
    ze_api_version_t    version;
    uint8_t             isValidFlag;
    zer_global_dditable_t *             Global;
} zer_dditable_driver_t;

#if defined(__cplusplus)
} // extern "C"
#endif

#endif // _ZER_DDI_H