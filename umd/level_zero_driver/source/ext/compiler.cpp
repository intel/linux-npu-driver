/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include "perfetto.h"

#include "compiler.hpp"

#include <stddef.h>

#include "blob_container.hpp"
#include "level_zero/ze_api.h"
#include "npu_driver_compiler.h"
#include "umd_common.hpp"
#include "vcl_symbols.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/trace_perfetto.hpp" // IWYU pragma: keep

#include <bitset>
#include <memory>
#include <string.h>

namespace L0 {

static vcl_compiler_properties_t compilerProperties;

static vcl_version_info_t vclCompilerApiVersion = {};
static vcl_version_info_t vclProfilingApiVersion = {};
static vcl_log_level_t cidLogLevel = VCL_LOG_NONE;

void Compiler::setLogLevel(const std::string_view &str) {
    if (str == "TRACE") {
        cidLogLevel = VCL_LOG_TRACE;
    } else if (str == "DEBUG") {
        cidLogLevel = VCL_LOG_DEBUG;
    } else if (str == "INFO") {
        cidLogLevel = VCL_LOG_INFO;
    } else if (str == "WARNING") {
        cidLogLevel = VCL_LOG_WARNING;
    } else if (str == "ERROR") {
        cidLogLevel = VCL_LOG_ERROR;
    } else {
        cidLogLevel = VCL_LOG_NONE;
    }
}

static ze_result_t vclToL0Err(vcl_result_t vclErr) {
    switch (vclErr) {
    case VCL_RESULT_SUCCESS:
        return ZE_RESULT_SUCCESS;
    case VCL_RESULT_ERROR_OUT_OF_MEMORY:
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;
    case VCL_RESULT_ERROR_UNSUPPORTED_FEATURE:
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
    case VCL_RESULT_ERROR_INVALID_ARGUMENT:
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    case VCL_RESULT_ERROR_INVALID_NULL_HANDLE:
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    case VCL_RESULT_ERROR_IO:
        return ZE_RESULT_ERROR_MODULE_BUILD_FAILURE;
    case VCL_RESULT_ERROR_INVALID_IR:
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
    case VCL_RESULT_ERROR_UNKNOWN:
    default:
        return ZE_RESULT_ERROR_UNKNOWN;
    }
}

bool Compiler::isVclCompilerApiCompatible() {
    if (vclCompilerApiVersion.major != VCL_COMPILER_VERSION_MAJOR) {
        LOG_E("VCL Compiler API version mismatch! Version expected:%d.%d, current:%d.%d",
              VCL_COMPILER_VERSION_MAJOR,
              VCL_COMPILER_VERSION_MINOR,
              vclCompilerApiVersion.major,
              vclCompilerApiVersion.minor);
        return false;
    }

    return true;
}

static bool isVclProfilingApiCompatible() {
    if (vclProfilingApiVersion.major != VCL_PROFILING_VERSION_MAJOR) {
        LOG_E("VCL Profiling API version mismatch! Version expected:%d.%d, current:%d.%d",
              VCL_PROFILING_VERSION_MAJOR,
              VCL_PROFILING_VERSION_MINOR,
              vclProfilingApiVersion.major,
              vclProfilingApiVersion.minor);
        return false;
    }

    return true;
}

ze_result_t Compiler::compilerCreate(const VPU::VPUHwInfo &hwInfo,
                                     vcl_compiler_handle_t &compiler,
                                     vcl_log_handle_t &logHandle) {
    if (!isVclCompilerApiCompatible()) {
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
    }

    vcl_compiler_desc_t compilerDesc = {};
    compilerDesc.version.major = VCL_COMPILER_VERSION_MAJOR;
    compilerDesc.version.minor = VCL_COMPILER_VERSION_MINOR;
    compilerDesc.debugLevel = cidLogLevel;

    vcl_device_desc_t deviceDesc = {};
    deviceDesc.size = sizeof(vcl_device_desc_t);
    deviceDesc.deviceID = hwInfo.deviceId;
    deviceDesc.revision = hwInfo.deviceRevision;
    deviceDesc.tileCount = static_cast<uint32_t>(std::bitset<32>(hwInfo.tileConfig).count());

    TRACE_EVENT("NPU_COMPILER", "vclCompilerCreate");
    return vclToL0Err(Vcl::sym().compilerCreate(&compilerDesc, &deviceDesc, &compiler, &logHandle));
}

ze_result_t Compiler::compilerDestroy(vcl_compiler_handle_t compiler) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    TRACE_EVENT("NPU_COMPILER", "vclCompilerDestroy");
    return vclToL0Err(Vcl::sym().compilerDestroy(compiler));
}

ze_result_t Compiler::compilerInit(VPU::VPUDevice *vpuDevice) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    TRACE_EVENT_BEGIN("NPU_COMPILER", "vclGetVersion");
    ze_result_t ret =
        vclToL0Err(Vcl::sym().getVersion(&vclCompilerApiVersion, &vclProfilingApiVersion));
    TRACE_EVENT_END("NPU_COMPILER");
    if (ret != ZE_RESULT_SUCCESS) {
        LOG_E("Failed to call vclGetVersion, ret: %#x", ret);
        return ret;
    }

    vcl_compiler_handle_t compiler = nullptr;
    vcl_log_handle_t logHandle = nullptr;
    ret = compilerCreate(vpuDevice->getHwInfo(), compiler, logHandle);
    if (ret != ZE_RESULT_SUCCESS) {
        LOG_E("Failed to create compiler! Result:%#x", ret);
        return ret;
    }

    TRACE_EVENT_BEGIN("NPU_COMPILER", "vclCompilerGetProperties");
    ret = vclToL0Err(Vcl::sym().compilerGetProperties(compiler, &compilerProperties));
    TRACE_EVENT_END("NPU_COMPILER");
    if (ret != ZE_RESULT_SUCCESS) {
        LOG_E("Failed to get compiler version! Result:%#x", ret);
        TRACE_EVENT("NPU_COMPILER", "vclCompilerDestroy");
        Vcl::sym().compilerDestroy(compiler);
        return ret;
    }

    TRACE_EVENT("NPU_COMPILER", "vclCompilerDestroy");
    Vcl::sym().compilerDestroy(compiler);
    return ret;
}

static void appendCompilerLog(vcl_log_handle_t logHandle, std::string &buffer) {
    if (!Vcl::sym().ok())
        return;

    if (logHandle == NULL) {
        return;
    }

    size_t compilerLogSize = 0;
    TRACE_EVENT_BEGIN("NPU_COMPILER", "vclLogHandleGetString");
    vcl_result_t logRet = Vcl::sym().logHandleGetString(logHandle, &compilerLogSize, NULL);
    TRACE_EVENT_END("NPU_COMPILER");
    if (logRet != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to get size of error message");
        return;
    }

    if (compilerLogSize == 0) {
        return;
    }

    size_t currentSize = buffer.size();
    buffer.resize(currentSize + compilerLogSize);

    TRACE_EVENT("NPU_COMPILER", "vclLogHandleGetString");
    logRet =
        Vcl::sym().logHandleGetString(logHandle, &compilerLogSize, buffer.data() + currentSize);
    if (logRet != VCL_RESULT_SUCCESS) {
        buffer.resize(currentSize);
        buffer += "[NPU_DRV] Failed to get content of log from compiler\n";
        LOG_E("Failed to get content of log from compiler");
        return;
    }

    // Remove the zero byte from the end of string to add driver messages
    if (!buffer.empty() && buffer.back() == '\0')
        buffer.pop_back();

    buffer.push_back('\n');
    LOG(GRAPH, "Saved compiler message to log buffer, message: %s", buffer.c_str());
}

static uint8_t *vclAllocate(uint64_t size) {
    return new uint8_t[size];
}

static void vclDeallocate(uint8_t *ptr) {
    delete[] ptr;
}

static uint8_t *vclAllocate2(vcl_allocator2_t *allocator, uint64_t size) {
    return new uint8_t[size];
}

static void vclDeallocate2(vcl_allocator2_t *allocator, uint8_t *ptr) {
    delete[] ptr;
}

static ze_result_t getCompilerExecutableAllocation(VPU::VPUDeviceContext *ctx,
                                                   vcl_compiler_handle_t &compiler,
                                                   ze_graph_desc_2_t &desc,
                                                   std::unique_ptr<BlobContainer> &blob,
                                                   std::string &log) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    vcl_executable_desc_t exeDesc = {desc.pInput,
                                     desc.inputSize,
                                     desc.pBuildFlags,
                                     strlen(desc.pBuildFlags)};
    LOG(GRAPH, "Compiler options: %s", exeDesc.options);

    uint8_t *graphBuffer = nullptr;
    size_t graphSize = 0;

    if (Vcl::sym().allocatedExecutableCreate2 != nullptr) {
        vcl_allocator2_t allocator = {.allocate = &vclAllocate2, .deallocate = &vclDeallocate2};
        TRACE_EVENT("NPU_COMPILER", "vclAllocatedExecutableCreate2");
        ze_result_t ret = vclToL0Err(Vcl::sym().allocatedExecutableCreate2(compiler,
                                                                           exeDesc,
                                                                           &allocator,
                                                                           &graphBuffer,
                                                                           &graphSize));
        if (ret != ZE_RESULT_SUCCESS) {
            log += "[NPU_DRV] Driver reports a failure from vclAllocatedExecutableCreate2, return "
                   "code: " +
                   std::to_string(ret) + '\n';
            LOG_E("Failed to create compiler executable! Result:%#x", ret);
            return ret;
        }
    } else {
        vcl_allocator_t allocator = {.allocate = &vclAllocate, .deallocate = &vclDeallocate};
        TRACE_EVENT("NPU_COMPILER", "vclAllocatedExecutableCreate");
        ze_result_t ret = vclToL0Err(Vcl::sym().allocatedExecutableCreate(compiler,
                                                                          exeDesc,
                                                                          &allocator,
                                                                          &graphBuffer,
                                                                          &graphSize));
        if (ret != ZE_RESULT_SUCCESS) {
            log += "[NPU_DRV] Driver reports a failure from vclAllocatedExecutableCreate, return "
                   "code: " +
                   std::to_string(ret) + '\n';
            LOG_E("Failed to create compiler executable! Result:%#x", ret);
            return ret;
        }
    }
    blob = std::make_unique<BlobAllocContainer>(std::unique_ptr<uint8_t[]>(graphBuffer), graphSize);
    return ZE_RESULT_SUCCESS;
}

ze_result_t Compiler::getCompiledBlob(VPU::VPUDeviceContext *ctx,
                                      ze_graph_desc_2_t &desc,
                                      std::unique_ptr<BlobContainer> &blob,
                                      std::string &log) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    if (!isVclCompilerApiCompatible()) {
        log += "[NPU_DRV] Driver reports VCL Compiler API version mismatch. Expected version: " +
               std::to_string(VCL_COMPILER_VERSION_MAJOR) + "." +
               std::to_string(VCL_COMPILER_VERSION_MINOR) +
               ", current version: " + std::to_string(vclCompilerApiVersion.major) + "." +
               std::to_string(vclCompilerApiVersion.minor) + '\n';
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
    }

    vcl_compiler_handle_t compiler = NULL;
    vcl_log_handle_t logHandle = NULL;
    ze_result_t ret = compilerCreate(ctx->getDeviceCapabilities(), compiler, logHandle);
    if (ret != ZE_RESULT_SUCCESS) {
        log += "[NPU_DRV] Driver reports a failure from vclCompilerCreate, return code: " +
               std::to_string(ret) + '\n';
        appendCompilerLog(logHandle, log);
        LOG_E("Failed to create compiler! Result:%#x", ret);
        return ret;
    }

    ret = getCompilerExecutableAllocation(ctx, compiler, desc, blob, log);

    appendCompilerLog(logHandle, log);

    TRACE_EVENT("NPU_COMPILER", "vclCompilerDestroy");
    Vcl::sym().compilerDestroy(compiler);
    return ret;
}

ze_result_t Compiler::getCompilerProperties(vcl_compiler_properties_t *pProperties) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    if (pProperties == nullptr)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    *pProperties = compilerProperties;
    return ZE_RESULT_SUCCESS;
}

vcl_version_info_t Compiler::getVclCompilerApiVersion() {
    return vclCompilerApiVersion;
}

std::string Compiler::getCompilerVersionString() {
    std::string version = "not available";
    if (!compilerProperties.version.major)
        return version;
    return std::to_string(compilerProperties.version.major) + "." +
           std::to_string(compilerProperties.version.minor) + "(" + compilerProperties.id + ")";
}

ze_result_t Compiler::getDecodedProfilingBuffer(ze_graph_profiling_type_t profilingType,
                                                const BlobContainer &blob,
                                                const uint8_t *profData,
                                                uint64_t profSize,
                                                uint32_t *pSize,
                                                void *pData,
                                                std::string &logBuffer) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    if (!isVclProfilingApiCompatible())
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    vcl_profiling_handle_t profHandle = NULL;
    vcl_profiling_input_t profilingApiInput = {.blobData = blob.ptr,
                                               .blobSize = blob.size,
                                               .profData = profData,
                                               .profSize = profSize};
    vcl_log_handle_t logHandle = NULL;

    TRACE_EVENT_BEGIN("NPU_COMPILER", "vclProfilingCreate");
    ze_result_t ret =
        vclToL0Err(Vcl::sym().profilingCreate(&profilingApiInput, &profHandle, &logHandle));
    TRACE_EVENT_END("NPU_COMPILER");
    if (ret != ZE_RESULT_SUCCESS) {
        logBuffer += "[NPU_DRV] Driver reports a failure from vclProfilingCreate, return code: " +
                     std::to_string(ret) + '\n';
        appendCompilerLog(logHandle, logBuffer);
        LOG_E("Failed to create profiling in compiler");
        return ret;
    }

    vcl_profiling_output_t profOutput = {};
    vcl_profiling_request_type_t profType = VCL_PROFILING_LAYER_LEVEL;
    if (profilingType == ZE_GRAPH_PROFILING_TASK_LEVEL)
        profType = VCL_PROFILING_TASK_LEVEL;

    TRACE_EVENT_BEGIN("NPU_COMPILER", "vclGetDecodedProfilingBuffer");
    ret = vclToL0Err(Vcl::sym().getDecodedProfilingBuffer(profHandle, profType, &profOutput));
    TRACE_EVENT_END("NPU_COMPILER");
    if (ret != ZE_RESULT_SUCCESS) {
        logBuffer +=
            "[NPU_DRV] Driver reports a failure from vclGetDecodedProfilingBuffer, return code: " +
            std::to_string(ret) + '\n';
        appendCompilerLog(logHandle, logBuffer);
        LOG_E("Failed to get decoded profiling data in compiler");

        TRACE_EVENT("NPU_COMPILER", "vclProfilingDestroy");
        Vcl::sym().profilingDestroy(profHandle);
        return ret;
    }

    if (*pSize == 0 || *pSize > profOutput.size)
        *pSize = safe_cast<uint32_t>(profOutput.size);

    if (pData != nullptr)
        memcpy(pData, profOutput.data, *pSize);

    TRACE_EVENT("NPU_COMPILER", "vclProfilingDestroy");
    Vcl::sym().profilingDestroy(profHandle);
    return ZE_RESULT_SUCCESS;
}

ze_result_t Compiler::queryNetworkCreate(vcl_compiler_handle_t compiler,
                                         vcl_query_desc_t &queryDesc,
                                         vcl_query_handle_t *query) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    TRACE_EVENT("NPU_COMPILER", "vclQueryNetworkCreate");
    return vclToL0Err(Vcl::sym().queryNetworkCreate(compiler, queryDesc, query));
}

ze_result_t
Compiler::queryNetwork(vcl_query_handle_t query, uint8_t *pSupportedLayers, size_t *pSize) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    TRACE_EVENT("NPU_COMPILER", "vclQueryNetwork");
    return vclToL0Err(Vcl::sym().queryNetwork(query, pSupportedLayers, pSize));
}

ze_result_t Compiler::queryNetworkDestroy(vcl_query_handle_t query) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    TRACE_EVENT("NPU_COMPILER", "vclQueryNetworkDestroy");
    return vclToL0Err(Vcl::sym().queryNetworkDestroy(query));
}

ze_result_t
Compiler::getSupportedOptions(VPU::VPUDevice *vpuDevice, size_t *pSize, char *pSupportedOptions) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    if (Vcl::sym().getCompilerSupportedOptions == nullptr)
        return ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE;

    if (vpuDevice == nullptr)
        return ZE_RESULT_ERROR_UNKNOWN;

    vcl_compiler_handle_t compiler = nullptr;
    vcl_log_handle_t compilerLog = nullptr;

    ze_result_t ret = compilerCreate(vpuDevice->getHwInfo(), compiler, compilerLog);
    if (ret != ZE_RESULT_SUCCESS) {
        LOG_E("Failed to create compiler! Result:%#x", ret);
        return ret;
    }

    TRACE_EVENT_BEGIN("NPU_COMPILER", "vclGetCompilerSupportedOptions");
    ret = vclToL0Err(Vcl::sym().getCompilerSupportedOptions(compiler, pSupportedOptions, pSize));
    TRACE_EVENT_END("NPU_COMPILER");
    if (ret != ZE_RESULT_SUCCESS) {
        LOG_E("Failed to call vclGetCompilerSupportedOptions, ret: %#x", ret);
    }

    TRACE_EVENT("NPU_COMPILER", "vclCompilerDestroy");
    Vcl::sym().compilerDestroy(compiler);
    return ret;
}

ze_result_t
Compiler::isOptionSupported(VPU::VPUDevice *vpuDevice, const char *pOption, const char *pValue) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    if (Vcl::sym().getCompilerIsOptionSupported == nullptr)
        return ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE;

    if (vpuDevice == nullptr)
        return ZE_RESULT_ERROR_UNKNOWN;

    vcl_compiler_handle_t compiler = nullptr;
    vcl_log_handle_t compilerLog = nullptr;

    ze_result_t ret = compilerCreate(vpuDevice->getHwInfo(), compiler, compilerLog);
    if (ret != ZE_RESULT_SUCCESS) {
        LOG_E("Failed to create compiler! Result:%#x", ret);
        return ret;
    }

    TRACE_EVENT_BEGIN("NPU_COMPILER", "vclGetCompilerIsOptionSupported");
    ret = vclToL0Err(Vcl::sym().getCompilerIsOptionSupported(compiler, pOption, pValue));
    TRACE_EVENT_END("NPU_COMPILER");
    if (ret != ZE_RESULT_SUCCESS && ret != ZE_RESULT_ERROR_UNSUPPORTED_FEATURE) {
        LOG_E("Failed to call vclGetCompilerIsOptionSupported, ret: %#x", ret);
    }

    TRACE_EVENT("NPU_COMPILER", "vclCompilerDestroy");
    Vcl::sym().compilerDestroy(compiler);
    return ret;
}

} // namespace L0
