/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/ext/source/graph/compiler.hpp"

#include <stddef.h>

#include "compiler_common.hpp"
#include "level_zero/ze_api.h"
#include "level_zero_driver/ext/source/graph/blob_container.hpp"
#include "npu_driver_compiler.h"
#include "umd_common.hpp"
#include "vcl_symbols.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <memory>
#include <string.h>
#include <utility>

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

static bool getCompilerExecutable(VPU::VPUDeviceContext *ctx,
                                  vcl_compiler_handle_t &compiler,
                                  ze_graph_desc_2_t &desc,
                                  std::unique_ptr<BlobContainer> &blob) {
    if (!Vcl::sym().ok())
        return false;

    vcl_executable_handle_t executable = nullptr;
    vcl_executable_desc_t exeDesc = {desc.pInput,
                                     desc.inputSize,
                                     desc.pBuildFlags,
                                     strlen(desc.pBuildFlags)};
    LOG(GRAPH, "Compiler options: %s", exeDesc.options);
    vcl_result_t ret = Vcl::sym().executableCreate(compiler, exeDesc, &executable);
    if (ret != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to create compiler executable! Result:%x", ret);
        return false;
    }

    size_t blobSize = 0;
    ret = Vcl::sym().executableGetSerializableBlob(executable, NULL, &blobSize);
    if (ret != VCL_RESULT_SUCCESS || blobSize == 0) {
        LOG_E("Failed to get blob size! Result:%x", ret);
        Vcl::sym().executableDestroy(executable);
        return false;
    }

    auto blobPtr = std::make_unique<uint8_t[]>(blobSize);
    ret = Vcl::sym().executableGetSerializableBlob(executable, blobPtr.get(), &blobSize);
    if (ret != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to get blob! Result:%x", ret);
        Vcl::sym().executableDestroy(executable);
        return false;
    }

    blob = std::make_unique<BlobAllocContainer>(std::move(blobPtr), blobSize);
    Vcl::sym().executableDestroy(executable);
    return true;
}

static uint8_t *vclAllocate(uint64_t size) {
    return new uint8_t[size];
}

static void vclDeallocate(uint8_t *ptr) {
    delete[] ptr;
}

static bool getCompilerExecutableAllocation(VPU::VPUDeviceContext *ctx,
                                            vcl_compiler_handle_t &compiler,
                                            ze_graph_desc_2_t &desc,
                                            std::unique_ptr<BlobContainer> &blob) {
    if (!Vcl::sym().ok())
        return false;

    vcl_executable_desc_t exeDesc = {desc.pInput,
                                     desc.inputSize,
                                     desc.pBuildFlags,
                                     strlen(desc.pBuildFlags)};
    LOG(GRAPH, "Compiler options: %s", exeDesc.options);

    vcl_allocator_t allocator = {.allocate = &vclAllocate, .deallocate = &vclDeallocate};

    uint8_t *graphBuffer = nullptr;
    size_t graphSize = 0;
    vcl_result_t ret = Vcl::sym().allocatedExecutableCreate(compiler,
                                                            exeDesc,
                                                            &allocator,
                                                            &graphBuffer,
                                                            &graphSize);
    if (ret != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to create compiler executable! Result:%x", ret);
        return false;
    }

    blob = std::make_unique<BlobAllocContainer>(std::unique_ptr<uint8_t[]>(graphBuffer), graphSize);
    return true;
}

bool Compiler::getCompiledBlob(VPU::VPUDeviceContext *ctx,
                               ze_graph_desc_2_t &desc,
                               std::unique_ptr<BlobContainer> &blob,
                               std::string &logBuffer) {
    if (!Vcl::sym().ok())
        return false;

    vcl_result_t ret = VCL_RESULT_SUCCESS;

    vcl_compiler_handle_t compiler = NULL;
    vcl_log_handle_t logHandle = NULL;
    vcl_compiler_desc_t compilerDesc = {};

    compilerDesc.platform = static_cast<vcl_platform_t>(compilerPlatform);
    compilerDesc.debug_level = cidLogLevel;

    if (!checkVersion(VCL_COMPILER_VERSION_MAJOR)) {
        LOG_E("Compiler version mismatch! Version expected:%d.%d, current:%d.%d",
              VCL_COMPILER_VERSION_MAJOR,
              VCL_COMPILER_VERSION_MINOR,
              getCompilerVersionMajor(),
              getCompilerVersionMinor());
        logBuffer = "Compiler version mismatch. Expected version: " +
                    std::to_string(VCL_COMPILER_VERSION_MAJOR) + "." +
                    std::to_string(VCL_COMPILER_VERSION_MINOR) +
                    ", current version: " + std::to_string(getCompilerVersionMajor()) + "." +
                    std::to_string(getCompilerVersionMinor());
        return false;
    }

    ret = Vcl::sym().compilerCreate(compilerDesc, &compiler, &logHandle);
    if (ret != VCL_RESULT_SUCCESS) {
        copyCompilerLog(logHandle, logBuffer);
        LOG_E("Failed to create compiler! Result:%x", ret);
        return false;
    }

    if ((getCompilerVersionMajor() == 6 && getCompilerVersionMinor() >= 1) ||
        getCompilerVersionMajor() > 6) {
        if (!getCompilerExecutableAllocation(ctx, compiler, desc, blob)) {
            copyCompilerLog(logHandle, logBuffer);
            LOG_E("Failed to get compiler executable");
            Vcl::sym().compilerDestroy(compiler);
            return false;
        }
    } else if (!getCompilerExecutable(ctx, compiler, desc, blob)) {
        copyCompilerLog(logHandle, logBuffer);
        LOG_E("Failed to get compiler executable");
        Vcl::sym().compilerDestroy(compiler);
        return false;
    }

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

bool Compiler::checkVersion(uint16_t major) {
    if (compilerProperties.version.major == major)
        return true;
    return false;
}

std::string Compiler::getCompilerVersionString() {
    std::string version = "not available";
    if (!compilerProperties.version.major)
        return version;
    version = std::to_string(compilerProperties.version.major) + "." +
              std::to_string(compilerProperties.version.minor) + "(" + compilerProperties.id + ")";
    return version;
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

    vcl_profiling_handle_t profHandle = NULL;
    vcl_profiling_input_t profilingApiInput = {.blobData = blob.ptr,
                                               .blobSize = blob.size,
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
