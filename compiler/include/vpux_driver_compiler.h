//
// Copyright (C) 2023 Intel Corporation.
// SPDX-License-Identifier: Apache 2.0
//

#ifndef VPUX_COMPILER_L0_H
#define VPUX_COMPILER_L0_H

#if defined(__cplusplus)
#include <cstdint>
#include <cstdlib>
#else
#include <stdint.h>
#include <stdlib.h>
#endif

#if defined(__cplusplus)
#pragma once
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define VCL_COMPILER_VERSION_MAJOR 5
#define VCL_COMPILER_VERSION_MINOR 5
#define VCL_PROFILING_VERSION_MAJOR 2
#define VCL_PROFILING_VERSION_MINOR 0

///////////////////////////////////////////////////////////////////////////////
#ifndef VCL_APICALL
#if defined(_WIN32)
/// @brief Calling convention for all API functions
#define VCL_APICALL __cdecl
#else
#define VCL_APICALL
#endif  // defined(_WIN32)
#endif  // VCL_APICALL

///////////////////////////////////////////////////////////////////////////////
#ifndef VCL_APIEXPORT
#if defined(_WIN32)
/// @brief Windows-specific dllexport storage-class attribute
#define VCL_APIEXPORT __declspec(dllexport)
#else
#define VCL_APIEXPORT
#endif  // defined(_WIN32)
#endif  // VCL_APIEXPORT

///////////////////////////////////////////////////////////////////////////////
/// @brief Compiler handle
typedef struct __vcl_compiler_handle_t* vcl_compiler_handle_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Executable handle
typedef struct __vcl_executable_handle_t* vcl_executable_handle_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Profiling handle
typedef struct __vcl_profiling_handle_t* vcl_profiling_handle_t;

///////////////////////////////////////////////////////////////////////////////

/// @brief QueryNetwork handle
typedef struct __vcl_query_handle_t* vcl_query_handle_t;

/// @brief Error log handle
typedef struct __vcl_log_handle_t* vcl_log_handle_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Defines type of requested data.
/// Must be in sync with \b _ze_graph_profiling_type_t
typedef enum __vcl_profiling_request_type_t {
    VCL_PROFILING_LAYER_LEVEL = 0x1,
    VCL_PROFILING_TASK_LEVEL = 0x2,
    VCL_PROFILING_RAW = 0x3,

    VCL_PROFILING_FORCE_UINT32 = 0x7fffffff
} vcl_profiling_request_type_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Defines version info for the VPUXCompilerL0 API
typedef struct __vcl_version_info_t {
    uint16_t major;
    uint16_t minor;

} vcl_version_info_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Defines return/error codes
typedef enum __vcl_result_t {
    VCL_RESULT_SUCCESS = 0,                             ///< [Core] success
    VCL_RESULT_ERROR_OUT_OF_MEMORY = 0x70000002,        ///< [Core] insufficient memory to satisfy call
    VCL_RESULT_ERROR_INVALID_ARGUMENT = 0x78000004,     ///< [Validation] generic error code for invalid arguments
    VCL_RESULT_ERROR_INVALID_NULL_HANDLE = 0x78000005,  ///< [Validation] handle argument is not valid
    VCL_RESULT_ERROR_IO = 0x78000006,                   ///< [Core] IO error
    VCL_RESULT_ERROR_INVALID_IR = 0x78000007,           ///< [Validation] the member of modelIR is not valid
    VCL_RESULT_ERROR_UNKNOWN = 0x7ffffffe,              ///< [Core] unknown or internal error

} vcl_result_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Defines compiler properties
typedef struct __vcl_compiler_properties_t {
    const char* id;
    vcl_version_info_t version;
    uint32_t supportedOpsets;

} vcl_compiler_properties_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Defines profiling properties
typedef struct __vcl_profiling_properties_t {
    vcl_version_info_t version;  ///< Profiling module version

} vcl_profiling_properties_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Defines platform for compilation
typedef enum __vcl_platform_t {
    VCL_PLATFORM_UNKNOWN = -1,

    VCL_PLATFORM_VPU3700,  ///< NPU3700
    VCL_PLATFORM_VPU3720,  ///< NPU3720
    VCL_PLATFORM_VPU4000,  ///< NPU4000

} vcl_platform_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Defines debug level for VCL
typedef enum __vcl_log_level_t {
    VCL_LOG_NONE = 0,     ///< Log is disabled
    VCL_LOG_ERROR = 1,    ///< Events which are not expected, containing probable reason
    VCL_LOG_WARNING = 2,  ///< Events which are unusal
    VCL_LOG_INFO = 3,     ///< Short messages about ongoing activity
    VCL_LOG_DEBUG = 4,    ///< Messages with praticular data and explanations
    VCL_LOG_TRACE = 5,    ///< Messages with detailed information about execution

} vcl_log_level_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Defines compiler desc to be passed during creation
typedef struct __vcl_compiler_desc_t {
    vcl_platform_t platform;
    vcl_log_level_t debug_level;

} vcl_compiler_desc_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Defines executable description to be passed during executable
///        creation
///
///        Format of modelIRData (defined in L0 adaptor):
///        1. API version : vcl_version_info_t
///        2. Num of data elements (now only xml + weights = 2) : uint32_t
///        3. Size of data 1 (xml) : uint64_t
///        4. Data 1 : $2 bytes
///        5. Size of data 2 (weights) : uint64_t
///        6. Data 2 : $4 bytes
typedef struct __vcl_executable_desc_t {
    const uint8_t* modelIRData;
    uint64_t modelIRSize;  ///< Size of modelIRData
    const char* options;   ///< Compiler config options
    uint64_t optionsSize;  ///< Size of options
} vcl_executable_desc_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Defines input that is required to create profiling handler
typedef struct __vcl_profiling_input_t {
    const uint8_t* blobData;  ///< Pointer to the buffer with the blob
    uint64_t blobSize;        ///< Size of the blob in bytes
    const uint8_t* profData;  ///< Pointer to the raw profiling output
    uint64_t profSize;        ///< Size of the raw profiling output
} vcl_profiling_input_t, *p_vcl_profiling_input_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Decoded profiling output
typedef struct __vcl_profiling_output_t {
    const uint8_t* data;  ///< Either a pointer to raw data or pointer to the array of structures
    uint64_t size;        ///< Size of the buffer in bytes
} vcl_profiling_output_t, *p_vcl_profiling_output_t;

///////////////////////////////////////////////////////////////////////////////
/// @brief Creates a compiler object and returns the compiler handle
VCL_APIEXPORT vcl_result_t VCL_APICALL vclCompilerCreate(vcl_compiler_desc_t desc, vcl_compiler_handle_t* compiler,
                                                         vcl_log_handle_t* logHandle);

///////////////////////////////////////////////////////////////////////////////
/// @brief Destroys the compiler
VCL_APIEXPORT vcl_result_t VCL_APICALL vclCompilerDestroy(vcl_compiler_handle_t compiler);

///////////////////////////////////////////////////////////////////////////////
/// @brief Retrieves the compiler properties, include the version and supported_opsets
VCL_APIEXPORT vcl_result_t VCL_APICALL vclCompilerGetProperties(vcl_compiler_handle_t compiler,
                                                                vcl_compiler_properties_t* properties);

///////////////////////////////////////////////////////////////////////////////
/// @brief Create an querynetwork object and return the handle
VCL_APIEXPORT vcl_result_t VCL_APICALL vclQueryNetworkCreate(vcl_compiler_handle_t compiler, uint8_t* modelIR,
                                                             uint64_t modelIRSize, vcl_query_handle_t* query);

///////////////////////////////////////////////////////////////////////////////
/// @brief Retrieve result of query network
/// @attention Should be called twice, first time to retrieve data size, second time to get data.
VCL_APIEXPORT vcl_result_t VCL_APICALL vclQueryNetwork(vcl_query_handle_t query, uint8_t* queryResult, uint64_t* size);

///////////////////////////////////////////////////////////////////////////////
/// @brief Destroys the queryNetwork and releases the cached query result
VCL_APIEXPORT vcl_result_t VCL_APICALL vclQueryNetworkDestroy(vcl_query_handle_t query);

///////////////////////////////////////////////////////////////////////////////
/// @brief Creates an executable object and returns the executable handle.
/// Parse modelIRData in the executable descriptor to blob and store it in the executable.
VCL_APIEXPORT vcl_result_t VCL_APICALL vclExecutableCreate(vcl_compiler_handle_t compiler, vcl_executable_desc_t desc,
                                                           vcl_executable_handle_t* executable);

///////////////////////////////////////////////////////////////////////////////
/// @brief Destroys the executable and releases the cached blob.
VCL_APIEXPORT vcl_result_t VCL_APICALL vclExecutableDestroy(vcl_executable_handle_t executable);

///////////////////////////////////////////////////////////////////////////////
/// @brief If blobBuffer is null, the function returns the size of the blob stored in the executable.
/// Otherwise the function copies the executable cached blob to the blobBuffer provided by the caller.
VCL_APIEXPORT vcl_result_t VCL_APICALL vclExecutableGetSerializableBlob(vcl_executable_handle_t executable,
                                                                        uint8_t* blobBuffer, uint64_t* blobSize);

///////////////////////////////////////////////////////////////////////////////
/// @brief Creates a buffer with decoded profiling info.
/// This is the most computationally expensive profiling API.
/// It does all memory allocations and postprocessing.
/// @warning Caller must keep \b p_vcl_profiling_input_t::profData buffer alive until
/// \b vclProfilingDestroy call if \b VCL_PROFILING_RAW request is expected.
/// \b vclProfilingCreate function doesn't copy profiling output buffer but will
/// return pointer to it as a response to \b VCL_PROFILING_RAW request.
VCL_APIEXPORT vcl_result_t VCL_APICALL vclProfilingCreate(p_vcl_profiling_input_t profilingInput,
                                                          vcl_profiling_handle_t* profilingHandle,
                                                          vcl_log_handle_t* logHandle);

///////////////////////////////////////////////////////////////////////////////
/// @brief Provides profiling information based on request argument.
/// @warning For \b VCL_PROFILING_RAW request it returns a pointer to the buffer that was provided to
/// \b vclProfilingCreate function call. This means that original buffer with profiling output must
/// be alive till this call.
VCL_APIEXPORT vcl_result_t VCL_APICALL vclGetDecodedProfilingBuffer(vcl_profiling_handle_t profilingHandle,
                                                                    vcl_profiling_request_type_t requestType,
                                                                    p_vcl_profiling_output_t profilingOutput);

///////////////////////////////////////////////////////////////////////////////
/// @brief Destroys the buffer with decoded profiling info.
/// Now caller may safely dispose raw profiling output.
VCL_APIEXPORT vcl_result_t VCL_APICALL vclProfilingDestroy(vcl_profiling_handle_t profilingHandle);

///////////////////////////////////////////////////////////////////////////////
/// @brief Get version of post-processing module
VCL_APIEXPORT vcl_result_t VCL_APICALL vclProfilingGetProperties(vcl_profiling_handle_t profilingHandle,
                                                                 vcl_profiling_properties_t* properties);

///////////////////////////////////////////////////////////////////////////////
/// @brief Retrieves error message from log handler.
/// Handle is released automatically with related compiler or Profiler.
VCL_APIEXPORT vcl_result_t VCL_APICALL vclLogHandleGetString(vcl_log_handle_t logHandle, size_t* logSize, char* log);

#if defined(__cplusplus)
}  // extern "C"
#endif

#endif  // VPUX_COMPILER_L0_H
