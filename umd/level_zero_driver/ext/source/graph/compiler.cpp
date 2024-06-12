/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/ze_api.h"
#include "level_zero_driver/ext/source/graph/compiler.hpp"
#include "umd_common.hpp"
#include "compiler_common.hpp"
#include "vcl_symbols.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpux_driver_compiler.h"

#include <string.h>

namespace L0 {

static int compilerPlatform;
static vcl_compiler_properties_t compilerProperties;

bool Compiler::compilerInit(int compilerPlatformType) {
    if (!Vcl::sym().ok())
        return false;

    vcl_compiler_desc_t compilerDesc = {};
    vcl_compiler_handle_t compiler = nullptr;
    vcl_log_handle_t logHandle = nullptr;

    compilerDesc.platform = static_cast<vcl_platform_t>(compilerPlatformType);
    compilerDesc.debug_level = cidLogLevel;

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

    LOG(GRAPH, "Compiler ID: %s", compilerProperties.id);
    Vcl::sym().compilerDestroy(compiler);
    compilerPlatform = compilerPlatformType;
    return true;
}

static void copyCompilerLog(vcl_log_handle_t logHandle, std::string &buffer) {
    if (!Vcl::sym().ok())
        return;

    if (logHandle == NULL) {
        return;
    }

    size_t compilerLogSize = 0;
    vcl_result_t logRet = Vcl::sym().logHandleGetString(logHandle, &compilerLogSize, NULL);
    if (logRet != VCL_RESULT_SUCCESS) {
        buffer.clear();
        LOG_E("Failed to get size of error message");
        return;
    }

    if (compilerLogSize == 0) {
        buffer.clear();
        return;
    }

    buffer.resize(compilerLogSize);
    logRet = Vcl::sym().logHandleGetString(logHandle, &compilerLogSize, buffer.data());
    if (logRet != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to get content of error message");
        return;
    }

    LOG(GRAPH, "Saved compiler message to log buffer, message: %s", buffer.c_str());
}

static void
addOptionToBuildFlags(std::string_view key, std::string_view value, std::string &buildFlags) {
    if (buildFlags.find("--config") == std::string::npos) {
        buildFlags += " --config";
    }

    buildFlags += " " + std::string(key) + "=\"" + std::string(value) + "\"";
}

static bool getCompilerExecutable(VPU::VPUDeviceContext *ctx,
                                  vcl_compiler_handle_t &comp,
                                  vcl_executable_handle_t *exec,
                                  ze_graph_desc_2_t &desc,
                                  vcl_log_handle_t *logHandle,
                                  std::string &logBuffer) {
    if (!Vcl::sym().ok())
        return false;

    std::string buildFlags = "";

    if (desc.pBuildFlags != nullptr && desc.pBuildFlags[0] != '\0') {
        buildFlags = std::string(desc.pBuildFlags);
        LOG(GRAPH, "Compiler options: %s", buildFlags.c_str());
    } else {
        logBuffer = "Invalid pBuildFlags pointer!";
        LOG_E("Invalid pBuildFlags pointer");
        return false;
    }

    // Stepping and max_tiles are not supported in versions < 5.3
    if (compilerProperties.version.major >= 5 && compilerProperties.version.minor >= 3) {
        if (buildFlags.find("STEPPING") == std::string::npos) {
            uint32_t deviceRevision = ctx->getDeviceRevision();
            addOptionToBuildFlags("NPU_STEPPING", std::to_string(deviceRevision), buildFlags);
        }

        if (buildFlags.find("MAX_TILES") == std::string::npos) {
            uint32_t numSlices = ctx->getNumSlices();
            addOptionToBuildFlags("NPU_MAX_TILES", std::to_string(numSlices), buildFlags);
        }
    }

    if (desc.flags & ZE_GRAPH_FLAG_ENABLE_PROFILING) {
        addOptionToBuildFlags("PERF_COUNT", "YES", buildFlags);
    }

    vcl_executable_desc_t exeDesc = {desc.pInput,
                                     desc.inputSize,
                                     buildFlags.c_str(),
                                     buildFlags.size()};
    vcl_result_t ret = Vcl::sym().executableCreate(comp, exeDesc, exec);
    if (ret != VCL_RESULT_SUCCESS) {
        copyCompilerLog(*logHandle, logBuffer);
        LOG_E("Failed to create compiler executable! Result:%x", ret);
        return false;
    }

    return true;
}

bool Compiler::getCompiledBlob(VPU::VPUDeviceContext *ctx,
                               size_t &graphSize,
                               std::vector<uint8_t> &graphBlob,
                               ze_graph_desc_2_t &desc,
                               std::string &logBuffer) {
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
        logBuffer = "Compiler version mismatch";
        return false;
    }

    ret = Vcl::sym().compilerCreate(compilerDesc, &compiler, &logHandle);
    if (ret != VCL_RESULT_SUCCESS) {
        copyCompilerLog(logHandle, logBuffer);
        LOG_E("Failed to create compiler! Result:%x", ret);
        return false;
    }

    vcl_executable_handle_t executable;
    if (!getCompilerExecutable(ctx, compiler, &executable, desc, &logHandle, logBuffer)) {
        LOG_E("Failed to get compiler executable");
        Vcl::sym().compilerDestroy(compiler);
        return false;
    }

    ret = Vcl::sym().executableGetSerializableBlob(executable, NULL, &graphSize);
    if (ret != VCL_RESULT_SUCCESS || graphSize == 0) {
        copyCompilerLog(logHandle, logBuffer);
        LOG_E("Failed to get blob size! Result:%x", ret);
        Vcl::sym().executableDestroy(executable);
        Vcl::sym().compilerDestroy(compiler);
        return false;
    }

    graphBlob.resize(graphSize);
    ret = Vcl::sym().executableGetSerializableBlob(executable, graphBlob.data(), &graphSize);
    if (ret != VCL_RESULT_SUCCESS) {
        copyCompilerLog(logHandle, logBuffer);
        LOG_E("Failed to get blob! Result:%x", ret);
        Vcl::sym().executableDestroy(executable);
        Vcl::sym().compilerDestroy(compiler);
        return false;
    }

    Vcl::sym().executableDestroy(executable);
    Vcl::sym().compilerDestroy(compiler);

    return true;
}

bool Compiler::getCompilerProperties(vcl_compiler_properties_t *pProperties) {
    if (!Vcl::sym().ok())
        return false;

    if (pProperties == nullptr)
        return false;

    *pProperties = compilerProperties;
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
                                                void *pData,
                                                std::string &logBuffer) {
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
        copyCompilerLog(logHandle, logBuffer);
        LOG_E("Failed to create profiling in compiler");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    vcl_profiling_output_t profOutput = {};
    vcl_profiling_request_type_t profType = VCL_PROFILING_LAYER_LEVEL;
    if (profilingType == ZE_GRAPH_PROFILING_TASK_LEVEL)
        profType = VCL_PROFILING_TASK_LEVEL;

    ret = Vcl::sym().getDecodedProfilingBuffer(profHandle, profType, &profOutput);
    if (ret != VCL_RESULT_SUCCESS) {
        copyCompilerLog(logHandle, logBuffer);
        LOG_E("Failed to get decoded profiling data in compiler");
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

} // namespace L0
