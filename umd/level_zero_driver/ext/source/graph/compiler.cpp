/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/ze_api.h"
#include "level_zero_driver/ext/source/graph/compiler.hpp"
#include "umd_common.hpp"
#include "vcl_symbols.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <string.h>

namespace L0 {

static vcl_log_level_t cidLogLevel = VCL_LOG_NONE;
static int compilerPlatform;
static vcl_compiler_properties_t compilerProperties;
static thread_local std::vector<char> lastCompilerLog = {};

bool Compiler::compilerInit(int compilerPlatformType) {
    if (!Vcl::sym().ok())
        return false;

    vcl_compiler_desc_t compilerDesc = {};
    vcl_compiler_handle_t compiler = nullptr;
    vcl_log_handle_t logHandle = nullptr;

    compilerDesc.platform = static_cast<vcl_platform_t>(compilerPlatformType);
    compilerDesc.debug_level = VCL_LOG_NONE;

    auto ret = Vcl::sym().compilerCreate(compilerDesc, &compiler, &logHandle);
    if (ret) {
        LOG_E("Failed to create compiler! Result:%x", ret);
        return false;
    }

    ret = Vcl::sym().compilerGetProperties(compiler, &compilerProperties);
    if (ret) {
        LOG_E("Failed to get compiler version! Result:%x", ret);
        Vcl::sym().compilerDestroy(compiler);
        return false;
    }

    Vcl::sym().compilerDestroy(compiler);
    compilerPlatform = compilerPlatformType;
    return true;
}

void Compiler::setCidLogLevel(std::string_view &str) {
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

static void getInternalCompilerLastError(vcl_log_handle_t logHandle) {
    if (!Vcl::sym().ok())
        return;

    if (logHandle == NULL) {
        return;
    }

    size_t compilerLogSize = 0;
    vcl_result_t logRet = Vcl::sym().logHandleGetString(logHandle, &compilerLogSize, NULL);
    if (logRet != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to get size of error message.");
        return;
    }

    if (compilerLogSize == 0) {
        // No logs
        return;
    }

    lastCompilerLog.resize(compilerLogSize);
    logRet = Vcl::sym().logHandleGetString(logHandle, &compilerLogSize, lastCompilerLog.data());
    if (logRet != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to get content of error message.");
        return;
    }
}

static bool getCompilerExecutable(vcl_compiler_handle_t &comp,
                                  vcl_executable_handle_t *exec,
                                  ze_graph_desc_2_t &desc,
                                  vcl_log_handle_t *logHandle) {
    if (!Vcl::sym().ok())
        return false;

    std::string options = "";

    if ((desc.pBuildFlags != nullptr) && (desc.pBuildFlags[0] != '\0')) {
        options = std::string(desc.pBuildFlags);
        LOG_V("Compiler options: %s", options.c_str());
    } else {
        LOG_E("Invalid Build Flags!");
        return false;
    }

    if (desc.flags & ZE_GRAPH_FLAG_ENABLE_PROFILING) {
        if (options.find("--config") == std::string::npos) {
            options += " --config";
        }
        options += " PERF_COUNT=\"YES\"";
    }

    vcl_executable_desc_t exeDesc = {desc.pInput, desc.inputSize, options.c_str(), options.size()};
    vcl_result_t ret = Vcl::sym().executableCreate(comp, exeDesc, exec);
    if (ret != VCL_RESULT_SUCCESS) {
        getInternalCompilerLastError(*logHandle);
        LOG_E("Failed to create compiler executable! Result:%x", ret);
        return false;
    }

    return true;
}

bool Compiler::getCompiledBlob(size_t &graphSize,
                               std::vector<uint8_t> &graphBlob,
                               ze_graph_desc_2_t &desc) {
    if (!Vcl::sym().ok())
        return false;

    vcl_result_t ret = VCL_RESULT_SUCCESS;

    vcl_compiler_handle_t compiler = NULL;
    vcl_log_handle_t logHandle = NULL;
    vcl_compiler_desc_t compilerDesc = {};

    compilerDesc.platform = static_cast<vcl_platform_t>(compilerPlatform);
    compilerDesc.debug_level = cidLogLevel;

    if (!checkVersion(VCL_COMPILER_VERSION_MAJOR, VCL_COMPILER_VERSION_MINOR)) {
        LOG_E("Compiler version mismatch! Version expected:%d.%d, current:%d.%d",
              VCL_COMPILER_VERSION_MAJOR,
              VCL_COMPILER_VERSION_MINOR,
              getCompilerVersionMajor(),
              getCompilerVersionMinor());
        return false;
    }

    ret = Vcl::sym().compilerCreate(compilerDesc, &compiler, &logHandle);
    if (ret != VCL_RESULT_SUCCESS) {
        getInternalCompilerLastError(logHandle);
        LOG_E("Failed to create compiler! Result:%x", ret);
        return false;
    }

    vcl_executable_handle_t executable;
    if (!getCompilerExecutable(compiler, &executable, desc, &logHandle)) {
        LOG_E("Failed to get compiler executable!");
        Vcl::sym().compilerDestroy(compiler);
        return false;
    }

    ret = Vcl::sym().executableGetSerializableBlob(executable, NULL, &graphSize);
    if (ret != VCL_RESULT_SUCCESS || graphSize == 0) {
        getInternalCompilerLastError(logHandle);
        LOG_E("Failed to get blob size! Result:%x", ret);
        Vcl::sym().executableDestroy(executable);
        Vcl::sym().compilerDestroy(compiler);
        return false;
    }

    graphBlob.resize(graphSize);
    ret = Vcl::sym().executableGetSerializableBlob(executable, graphBlob.data(), &graphSize);
    if (ret != VCL_RESULT_SUCCESS) {
        getInternalCompilerLastError(logHandle);
        LOG_E("Failed to get blob! Result:%x", ret);
        Vcl::sym().executableDestroy(executable);
        Vcl::sym().compilerDestroy(compiler);
        return false;
    }

    Vcl::sym().executableDestroy(executable);
    Vcl::sym().compilerDestroy(compiler);

    return true;
}

bool Compiler::getCompilerProperties(ze_device_graph_properties_t *pDeviceGraphProperties) {
    if (!Vcl::sym().ok())
        return false;

    if (!pDeviceGraphProperties)
        return false;

    pDeviceGraphProperties->compilerVersion.major = compilerProperties.version.major;
    pDeviceGraphProperties->compilerVersion.minor = compilerProperties.version.minor;
    pDeviceGraphProperties->graphFormatsSupported = ZE_GRAPH_FORMAT_NGRAPH_LITE;
    pDeviceGraphProperties->maxOVOpsetVersionSupported = compilerProperties.supportedOpsets;

    return true;
}

uint16_t Compiler::getCompilerVersionMajor() {
    return compilerProperties.version.major;
}

uint16_t Compiler::getCompilerVersionMinor() {
    return compilerProperties.version.minor;
}

bool Compiler::checkVersion(uint16_t major, uint16_t minor) {
    if (compilerProperties.version.major == major && compilerProperties.version.minor >= minor)
        return true;
    return false;
}

ze_result_t Compiler::getDecodedProfilingBuffer(ze_graph_profiling_type_t profilingType,
                                                const std::vector<uint8_t> *blobRaw,
                                                const uint8_t *profData,
                                                uint64_t profSize,
                                                uint32_t *pSize,
                                                void *pData) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    vcl_profiling_handle_t profHandle = NULL;
    vcl_profiling_input_t profilingApiInput = {.blobData = blobRaw->data(),
                                               .blobSize = blobRaw->size(),
                                               .profData = profData,
                                               .profSize = profSize};
    vcl_log_handle_t logHandle = NULL;

    auto ret = Vcl::sym().profilingCreate(&profilingApiInput, &profHandle, &logHandle);
    if (ret != VCL_RESULT_SUCCESS) {
        getInternalCompilerLastError(logHandle);
        LOG_E("Failed to create profiling in compiler.");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    vcl_profiling_output_t profOutput = {};
    vcl_profiling_request_type_t profType = VCL_PROFILING_LAYER_LEVEL;
    if (profilingType == ZE_GRAPH_PROFILING_TASK_LEVEL)
        profType = VCL_PROFILING_TASK_LEVEL;

    ret = Vcl::sym().getDecodedProfilingBuffer(profHandle, profType, &profOutput);
    if (ret != VCL_RESULT_SUCCESS) {
        getInternalCompilerLastError(logHandle);
        LOG_E("Failed to get decoded profiling data in compiler.");
        Vcl::sym().profilingDestroy(profHandle);
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    if (*pSize == 0 || *pSize > profOutput.size)
        *pSize = safe_cast<uint32_t>(profOutput.size);

    if (pData != nullptr)
        memcpy(pData, profOutput.data, *pSize);

    Vcl::sym().profilingDestroy(profHandle);
    return ZE_RESULT_SUCCESS;
}

bool Compiler::logGetString(uint32_t *pSize, char *pLog) {
    if (*pSize == 0) {
        *pSize = static_cast<uint32_t>(lastCompilerLog.size());
        return true;
    }

    if (*pSize > lastCompilerLog.size()) {
        *pSize = static_cast<uint32_t>(lastCompilerLog.size());
    }

    if (pLog != nullptr) {
        memcpy(pLog, lastCompilerLog.data(), *pSize);
    }

    return true;
}

} // namespace L0
