/*
 * Copyright (C) 2023-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "query_network.hpp"

#include <stdint.h>

#include "compiler.hpp"
#include "level_zero/ze_api.h"
#include "level_zero/ze_graph_ext.h"
#include "level_zero_driver/source/context.hpp"
#include "vcl_symbols.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <string.h>

namespace L0 {

ze_result_t QueryNetwork::create(ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 const ze_graph_desc_2_t *desc,
                                 ze_graph_query_network_handle_t *phGraphQueryNetwork) {
    if (!Vcl::sym().ok())
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    if (desc->stype != ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES) {
        LOG_E("Invalid structure type");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (desc->format != ZE_GRAPH_FORMAT_NGRAPH_LITE) {
        LOG_E("Query Network only supports NGraph format");
        return ZE_RESULT_ERROR_UNSUPPORTED_ENUMERATION;
    }

    if (!Compiler::isApiComatible()) {
        LOG_E("Compiler version mismatch! Version expected:%d.%d, current:%d.%d",
              VCL_COMPILER_VERSION_MAJOR,
              VCL_COMPILER_VERSION_MINOR,
              Compiler::getCompilerVersionMajor(),
              Compiler::getCompilerVersionMinor());
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
    }

    auto pCtx = Context::fromHandle(hContext)->getDeviceContext();
    if (pCtx == nullptr) {
        LOG_E("Device Context failed to be retrieved");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    vcl_compiler_handle_t compiler = NULL;
    vcl_log_handle_t logHandle = NULL;
    vcl_result_t ret = Compiler::compilerCreate(pCtx->getDeviceCapabilities(), compiler, logHandle);
    if (ret != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to create compiler! Result:%x", ret);
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    vcl_query_handle_t query = nullptr;
    vcl_query_desc_t queryDesc = {};
    queryDesc.modelIRData = desc->pInput;
    queryDesc.modelIRSize = desc->inputSize;
    queryDesc.options = desc->pBuildFlags;
    queryDesc.optionsSize = strlen(desc->pBuildFlags);
    ret = Vcl::sym().queryNetworkCreate(compiler, queryDesc, &query);
    if (ret != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to create query network! Result:%x", ret);
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    auto *queryNetwork = new QueryNetwork(compiler, query);
    if (queryNetwork == nullptr) {
        LOG_E("Failed to allocate query network");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    *phGraphQueryNetwork = queryNetwork->toHandle();
    return ZE_RESULT_SUCCESS;
}

ze_result_t QueryNetwork::destroy() {
    Vcl::sym().queryNetworkDestroy(query);
    Vcl::sym().compilerDestroy(compiler);
    delete this;

    return ZE_RESULT_SUCCESS;
}

ze_result_t QueryNetwork::getSupportedLayers(size_t *pSize, char *pSupportedLayers) {
    vcl_result_t ret =
        Vcl::sym().queryNetwork(query, reinterpret_cast<uint8_t *>(pSupportedLayers), pSize);
    if (ret != VCL_RESULT_SUCCESS) {
        LOG_E("Failed to execute vclQueryNetwork, ret: %x", ret);
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
