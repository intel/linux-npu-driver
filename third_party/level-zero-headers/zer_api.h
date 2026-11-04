/*
 *
 * Copyright (C) 2019-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 * @file zer_api.h
 * @version v1.18-r1.18.31
 *
 */
#ifndef _ZER_API_H
#define _ZER_API_H
#if defined(__cplusplus)
#pragma once
#endif

// 'core' API headers
#include "ze_api.h"

#if defined(__cplusplus)
extern "C" {
#endif

// Intel 'oneAPI' Level-Zero Runtime API common types
#if !defined(__GNUC__)
#pragma region common
#endif
///////////////////////////////////////////////////////////////////////////////
/// @brief Immediate Command List default descriptor for GPU devices
static const ze_command_queue_desc_t zeDefaultGPUImmediateCommandQueueDesc = {
    ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,                                   ///< stype
    0,                                                                      ///< pNext
    0,                                                                      ///< ordinal
    0,                                                                      ///< index
    ZE_COMMAND_QUEUE_FLAG_IN_ORDER | ZE_COMMAND_QUEUE_FLAG_COPY_OFFLOAD_HINT,   ///< flags
    ZE_COMMAND_QUEUE_MODE_ASYNCHRONOUS,                                     ///< mode
    ZE_COMMAND_QUEUE_PRIORITY_NORMAL,                                       ///< priority
};

///////////////////////////////////////////////////////////////////////////////
/// @brief Device Unified Shared Memory Allocation default descriptor for GPU
///        devices
static const ze_device_mem_alloc_desc_t zeDefaultGPUDeviceMemAllocDesc = {
    ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,                                ///< stype
    0,                                                                      ///< pNext
    ZE_DEVICE_MEM_ALLOC_FLAG_BIAS_CACHED,                                   ///< flags
    0,                                                                      ///< ordinal
};

///////////////////////////////////////////////////////////////////////////////
/// @brief Host Unified Shared Memory Allocation default descriptor for GPU
///        devices
static const ze_host_mem_alloc_desc_t zeDefaultGPUHostMemAllocDesc = {
    ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC,                                  ///< stype
    0,                                                                      ///< pNext
    ZE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED | ZE_HOST_MEM_ALLOC_FLAG_BIAS_INITIAL_PLACEMENT, ///< flags
};

///////////////////////////////////////////////////////////////////////////////
/// @brief Retrieves a string describing the last error code returned by the
///        default driver in the current thread.
/// 
/// @details
///     - String returned is thread local.
///     - String is only updated on calls returning an error, i.e., not on calls
///       returning ::ZE_RESULT_SUCCESS.
///     - String may be empty if driver considers error code is already explicit
///       enough to describe cause.
///     - Memory pointed to by ppString is owned by the default driver.
///     - String returned is null-terminated.
/// 
/// @returns
///     - ::ZE_RESULT_SUCCESS
///     - ::ZE_RESULT_ERROR_UNINITIALIZED
///     - ::ZE_RESULT_ERROR_DEVICE_LOST
///     - ::ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY
///     - ::ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY
///     - ::ZE_RESULT_ERROR_INVALID_ARGUMENT
///     - ::ZE_RESULT_ERROR_UNSUPPORTED_FEATURE
///     - ::ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE
///     - ::ZE_RESULT_ERROR_INSUFFICIENT_PERMISSIONS
///     - ::ZE_RESULT_ERROR_NOT_AVAILABLE
///     - ::ZE_RESULT_ERROR_DEVICE_REQUIRES_RESET
///     - ::ZE_RESULT_ERROR_DEVICE_IN_LOW_POWER_STATE
///     - ::ZE_RESULT_ERROR_UNKNOWN
///     - ::ZE_RESULT_ERROR_INVALID_NULL_POINTER
///         + `nullptr == ppString`
ZE_APIEXPORT ze_result_t ZE_APICALL
zerGetLastErrorDescription(
    const char** ppString                                                   ///< [in,out] pointer to a null-terminated array of characters describing
                                                                            ///< cause of error.
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Translates device handle to integer identifier.
/// 
/// @details
///     - The implementation of this function should be lock-free.
///     - This function does not return error code, to get info about failure
///       user may use ::zerGetLastErrorDescription function.
///     - In case of failure, this function returns UINT32_MAX.
/// 
/// @returns
///     - integer identifier for the device
///     - UINT32_MAX
ZE_APIEXPORT uint32_t ZE_APICALL
zerTranslateDeviceHandleToIdentifier(
    ze_device_handle_t hDevice                                              ///< [in] handle of the device
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Translates to integer identifier to a device handle.
/// 
/// @details
///     - The driver must be initialized before calling this function.
///     - The implementation of this function should be lock-free.
///     - This function does not return error code, to get info about failure
///       user may use ::zerGetLastErrorDescription function.
///     - In case of failure, this function returns null.
///     - Details on the error can be retrieved using
///       ::zerGetLastErrorDescription function.
/// 
/// @returns
///     - handle of the device with the given identifier
///     - nullptr
ZE_APIEXPORT ze_device_handle_t ZE_APICALL
zerTranslateIdentifierToDeviceHandle(
    uint32_t identifier                                                     ///< [in] integer identifier of the device
    );

///////////////////////////////////////////////////////////////////////////////
/// @brief Retrieves handle to default context from the default driver.
/// 
/// @details
///     - The driver must be initialized before calling this function.
///     - The implementation of this function should be lock-free.
///     - This returned context contains all the devices available in the
///       default driver.
///     - This function does not return error code, to get info about failure
///       user may use ::zerGetLastErrorDescription function.
///     - In case of failure, this function returns null.
///     - Details on the error can be retrieved using
///       ::zerGetLastErrorDescription function.
/// 
/// @returns
///     - handle of the default context
///     - nullptr
ZE_APIEXPORT ze_context_handle_t ZE_APICALL
zerGetDefaultContext(
    void
    );


#if !defined(__GNUC__)
#pragma endregion
#endif

#if defined(__cplusplus)
} // extern "C"
#endif

#endif // _ZER_API_H
