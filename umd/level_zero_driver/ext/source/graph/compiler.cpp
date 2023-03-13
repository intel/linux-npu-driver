/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/ext/source/graph/compiler.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <boost/numeric/conversion/cast.hpp>
#include <string.h>

namespace L0 {

uint32_t Compiler::cidLogLevel = 0;

#ifdef ENABLE_VPUX_COMPILER

#include "VPUXCompilerL0.h"

static bool getCompilerExecutable(vcl_compiler_handle_t &comp,
                                  vcl_executable_handle_t *exec,
                                  ze_graph_desc_t &desc) {
    std::string options = "";

    if ((desc.pBuildFlags != nullptr) && (desc.pBuildFlags[0] != '\0')) {
        options = std::string(desc.pBuildFlags);
        LOG_V("Compiler options: %s", options.c_str());
    } else {
        LOG_E("Invalid Build Flags!");
        return false;
    }

    vcl_executable_desc_t exeDesc = {desc.pInput, desc.inputSize, options.c_str(), options.size()};
    vcl_result_t ret = vclExecutableCreate(comp, exeDesc, exec);
    if (ret != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to create compiler executable! Result:%x", ret);
        return false;
    }

    return true;
}

bool Compiler::getCompiledBlob(size_t &graphSize,
                               std::vector<uint8_t> &graphBlob,
                               ze_graph_desc_t &desc) {
    vcl_compiler_handle_t compiler = NULL;
    vcl_result_t ret = VCL_RESULT_SUCCESS;
    vcl_compiler_desc_t compilerDesc = {VCL_PLATFORM_VPU3720, cidLogLevel};

    ret = vclCompilerCreate(compilerDesc, &compiler);
    if (ret != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to create compiler! Result:%x", ret);
        return false;
    }

    vcl_executable_handle_t executable;
    if (!getCompilerExecutable(compiler, &executable, desc)) {
        LOG_E("Failed to get compiler executable!");
        vclCompilerDestroy(compiler);
        return false;
    }

    ret = vclExecutableGetSerializableBlob(executable, NULL, &graphSize);
    if (ret != VCL_RESULT_SUCCESS || graphSize == 0) {
        LOG_E("Failed to get blob size! Result:%x", ret);
        vclExecutableDestroy(executable);
        vclCompilerDestroy(compiler);
        return false;
    }

    graphBlob.resize(graphSize);
    ret = vclExecutableGetSerializableBlob(executable, graphBlob.data(), &graphSize);
    if (ret != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to get blob! Result:%x", ret);
        vclExecutableDestroy(executable);
        vclCompilerDestroy(compiler);
        return false;
    }

    vclExecutableDestroy(executable);
    vclCompilerDestroy(compiler);

    return true;
}

bool Compiler::getCompilerProperties(ze_device_graph_properties_t *pDeviceGraphProperties) {
    vcl_compiler_desc_t compilerDesc = {VCL_PLATFORM_VPU3720, 0};
    vcl_compiler_handle_t compiler;
    vcl_compiler_properties_t properties;

    auto ret = vclCompilerCreate(compilerDesc, &compiler);
    if (ret) {
        LOG_E("Failed to create compiler! Result:%x", ret);
        return false;
    }

    ret = vclCompilerGetProperties(compiler, &properties);
    if (ret) {
        LOG_E("Failed to get properties from compiler! Result:%x", ret);
        vclCompilerDestroy(compiler);
        return false;
    }

    pDeviceGraphProperties->compilerVersion.major = properties.version.major;
    pDeviceGraphProperties->compilerVersion.minor = properties.version.minor;
    pDeviceGraphProperties->graphFormatsSupported = ZE_GRAPH_FORMAT_NGRAPH_LITE;
    pDeviceGraphProperties->maxOVOpsetVersionSupported = properties.supportedOpsets;

    vclCompilerDestroy(compiler);

    return true;
}

ze_result_t Compiler::getDecodedProfilingBuffer(ze_graph_profiling_type_t profilingType,
                                                const std::vector<uint8_t> *blobRaw,
                                                const uint8_t *profData,
                                                uint64_t profSize,
                                                uint32_t *pSize,
                                                void *pData) {
    vcl_profiling_handle_t profHandle = nullptr;
    vcl_profiling_input_t profilingApiInput = {.blobData = blobRaw->data(),
                                               .blobSize = blobRaw->size(),
                                               .profData = profData,
                                               .profSize = profSize};

    auto ret = vclProfilingCreate(&profilingApiInput, &profHandle);
    if (ret) {
        LOG_E("Failed to create profiling in compiler.");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    vcl_profiling_output_t profOutput;
    vcl_profiling_request_type_t profType = VCL_PROFILING_LAYER_LEVEL;
    if (profilingType == ZE_GRAPH_PROFILING_TASK_LEVEL)
        profType = VCL_PROFILING_TASK_LEVEL;

    ret = vclGetDecodedProfilingBuffer(profHandle, profType, &profOutput);
    if (ret) {
        LOG_E("Failed to get decoded profiling data in compiler.");
        vclProfilingDestroy(profHandle);
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    if (*pSize == 0 || *pSize > profOutput.size)
        *pSize = boost::numeric_cast<uint32_t>(profOutput.size);

    if (pData != nullptr)
        memcpy(pData, profOutput.data, *pSize);

    vclProfilingDestroy(profHandle);
    return ZE_RESULT_SUCCESS;
}

#else  // ENABLE_VPUX_COMPILER

bool Compiler::getCompiledBlob(size_t &graphSize,
                               std::vector<uint8_t> &graphBlob,
                               ze_graph_desc_t &desc) {
    LOG_W("VPUX Compiler disabled!");
    return false;
}

bool Compiler::getCompilerProperties(ze_device_graph_properties_t *pDeviceGraphProperties) {
    LOG_W("VPUX Compiler disabled!");
    return true;
}

ze_result_t Compiler::getDecodedProfilingBuffer(ze_graph_profiling_type_t profilingType,
                                                const std::vector<uint8_t> *blobRaw,
                                                const uint8_t *profData,
                                                uint64_t profSize,
                                                uint32_t *size,
                                                void *data) {
    LOG_E("VPUX Compiler disabled!");
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

#endif // ENABLE_VPUX_COMPILER

} // namespace L0
