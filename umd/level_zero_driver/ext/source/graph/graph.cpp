/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero/ze_api.h"
#include "level_zero/ze_graph_ext.h"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/ext/source/graph/graph.hpp"

#include "vpu_driver/source/command/vpu_barrier_command.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <string.h>
#include <boost/numeric/conversion/cast.hpp>

namespace L0 {
Graph::Graph(VPU::VPUDeviceContext *pCtx, const ze_graph_desc_t *pDesc)
    : ctx(pCtx)
    , desc(*pDesc) {}

ze_result_t Graph::create(const ze_context_handle_t hContext,
                          const ze_device_handle_t hDevice,
                          const ze_graph_desc_t *pDesc,
                          ze_graph_handle_t *phGraph) {
    if (pDesc == nullptr) {
        LOG_E("Invalid graph descriptor");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (phGraph == nullptr) {
        LOG_E("Invalid graph pointer to handle");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto pCtx = Context::fromHandle(hContext)->getDeviceContext();
    if (pCtx == nullptr) {
        LOG_E("Device Context failed to be retrieved");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    Graph *pGraph = new Graph(pCtx, pDesc);
    if (pGraph == nullptr) {
        LOG_E("Failed to allocate Graph object");
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;
    }

    ze_result_t ret = pGraph->initialize();
    if (ret != ZE_RESULT_SUCCESS) {
        LOG_E("Graph initialization failed, destroying graph object");
        pGraph->destroy();
        return ret;
    }

    *phGraph = pGraph;
    LOG_I("Graph created - %p", pGraph);
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::destroy() {
    LOG_V("Destroying graph.");

    delete this;
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getNativeBinary(size_t *pSize, uint8_t *pGraphNativeBinary) {
    if (pSize == nullptr) {
        LOG_E("Input size pointer is NULL");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (graphBlobRaw.size() == 0) {
        LOG_E("Graph contain invalid descriptor");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    } else {
        *pSize = graphBlobRaw.size();
    }

    if (pGraphNativeBinary == nullptr) {
        LOG_W("Input Graph Native Binary pointer is NULL");
    } else {
        if (graphBlobRaw.size() > *pSize) {
            LOG_E("Failed to copy Graph Native Binary! graphBlobRaw > *pSize");
            return ZE_RESULT_ERROR_UNKNOWN;
        }
        memcpy(pGraphNativeBinary, graphBlobRaw.data(), *pSize);
    }
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::setArgumentValue(uint32_t argIndex, const void *pArgValue) {
    if (pArgValue == nullptr)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (argIndex >= (argumentProperties.size()))
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (argIndex < inputArgs.size())
        inputArgs[argIndex].first = pArgValue;
    else
        outputArgs[argIndex - inputArgs.size()].first = pArgValue;
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getProperties(ze_graph_properties_t *pGraphProperties) {
    if (pGraphProperties == nullptr) {
        LOG_E("Invalid pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    pGraphProperties->numGraphArgs = boost::numeric_cast<uint32_t>(argumentProperties.size());
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getArgumentProperties(uint32_t argIndex,
                                         ze_graph_argument_properties_t *pGraphArgProps) {
    if (pGraphArgProps == nullptr) {
        LOG_E("Invalid pointer for argument properties.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (argIndex >= (argumentProperties.size())) {
        LOG_E("Invalid index (idx (%u) >= size (%lu))",
              static_cast<uint32_t>(argIndex),
              argumentProperties.size());
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    memcpy(pGraphArgProps, &argumentProperties[argIndex], sizeof(ze_graph_argument_properties_t));
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getArgumentProperties2(uint32_t argIndex,
                                          ze_graph_argument_properties_2_t *pGraphArgProps) {
    if (pGraphArgProps == nullptr) {
        LOG_E("Invalid pointer for argument properties.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (argIndex >= (argumentProperties.size())) {
        LOG_E("Invalid index (idx (%u) >= size (%lu))",
              static_cast<uint32_t>(argIndex),
              argumentProperties.size());
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    memcpy(pGraphArgProps, &argumentProperties[argIndex], sizeof(ze_graph_argument_properties_2_t));
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getArgumentProperties3(uint32_t argIndex,
                                          ze_graph_argument_properties_3_t *pGraphArgProps) {
    if (pGraphArgProps == nullptr) {
        LOG_E("Invalid pointer for argument properties.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (argIndex >= (argumentProperties.size())) {
        LOG_E("Invalid index (idx (%u) >= size (%lu))",
              static_cast<uint32_t>(argIndex),
              argumentProperties.size());
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *pGraphArgProps = argumentProperties[argIndex];
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getArgumentMetadata(uint32_t argIndex,
                                       ze_graph_argument_metadata_t *pGraphArgMetadata) {
    if (pGraphArgMetadata == nullptr) {
        LOG_E("Invalid pointer for argument properties.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (argIndex >= (argumentMetadata.size())) {
        LOG_E("Invalid index (idx (%u) >= size (%lu))",
              static_cast<uint32_t>(argIndex),
              argumentMetadata.size());
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *pGraphArgMetadata = argumentMetadata[argIndex];
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::createProfilingPool(uint32_t count,
                                       ze_graph_profiling_pool_handle_t *phProfilingPool) {
    if (ctx == nullptr) {
        LOG_E("Context is nullptr!");
        return ZE_RESULT_ERROR_DEVICE_LOST;
    }

    if (!profilingOutputSize) {
        LOG_W("Invalid profiling output size %u", profilingOutputSize);
        return ZE_RESULT_ERROR_NOT_AVAILABLE;
    }

    if (!count) {
        LOG_E("Invalid count value: %u", count);
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    if (phProfilingPool == nullptr) {
        LOG_E("ProfilingPool(%p) handle is NULL", phProfilingPool);
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    size_t profilingPoolSize = count * getFwDataCacheAlign(profilingOutputSize);
    auto *profilingPoolBuffer =
        ctx->createInternalBufferObject(profilingPoolSize, VPU::VPUBufferObject::Type::CachedHigh);
    if (profilingPoolBuffer == nullptr) {
        LOG_E("Failed to allocate buffer object for profiling pool");
        return ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    auto *profilingPool =
        new GraphProfilingPool(ctx, profilingOutputSize, count, profilingPoolBuffer, &graphBlobRaw);
    if (profilingPool == nullptr) {
        LOG_E("Failed to create profiling pool");
        if (!ctx->freeMemAlloc(profilingPoolBuffer))
            LOG_W("Failed to free profiling pool");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    *phProfilingPool = profilingPool->toHandle();
    return ZE_RESULT_SUCCESS;
};

ze_result_t Graph::getProfilingDataProperties(
    ze_device_profiling_data_properties_t *pDeviceProfilingDataProperties) {
    if (pDeviceProfilingDataProperties == nullptr) {
        LOG_E("Invalid profiling data properties pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    pDeviceProfilingDataProperties->extensionVersion = ZE_PROFILING_DATA_EXT_VERSION_CURRENT;

    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getDeviceGraphProperties(ze_device_handle_t hDevice,
                                            ze_device_graph_properties_t *pDeviceGraphProperties) {
    if (pDeviceGraphProperties == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    *pDeviceGraphProperties = {};

    pDeviceGraphProperties->pNext = nullptr;
    pDeviceGraphProperties->graphExtensionVersion = ZE_GRAPH_EXT_VERSION_CURRENT;

    if (!Compiler::getCompilerProperties(pDeviceGraphProperties)) {
        LOG_E("Failed to get compiler properties!");
        return ZE_RESULT_ERROR_UNKNOWN;
    }
    return ZE_RESULT_SUCCESS;
}

static uint32_t getArgumentSize(const ze_graph_argument_properties_3_t &prop) {
    size_t size = 1;
    for (uint32_t i = 0; i < ZE_MAX_GRAPH_ARGUMENT_DIMENSIONS_SIZE; i++)
        size *= prop.dims[i];

    switch (prop.devicePrecision) {
    case ZE_GRAPH_ARGUMENT_PRECISION_FP32:
    case ZE_GRAPH_ARGUMENT_PRECISION_UINT32:
    case ZE_GRAPH_ARGUMENT_PRECISION_INT32:
        size *= sizeof(uint32_t);
        break;
    case ZE_GRAPH_ARGUMENT_PRECISION_BF16:
    case ZE_GRAPH_ARGUMENT_PRECISION_FP16:
    case ZE_GRAPH_ARGUMENT_PRECISION_UINT16:
    case ZE_GRAPH_ARGUMENT_PRECISION_INT16:
        size *= sizeof(uint16_t);
        break;
    case ZE_GRAPH_ARGUMENT_PRECISION_UINT8:
    case ZE_GRAPH_ARGUMENT_PRECISION_INT8:
        size *= sizeof(uint8_t);
        break;
    case ZE_GRAPH_ARGUMENT_PRECISION_INT4:
    case ZE_GRAPH_ARGUMENT_PRECISION_UINT4:
        size /= 2;
        break;
    default:
        break;
    }

    return boost::numeric_cast<uint32_t>(size);
}

ze_result_t Graph::initialize() {
    if (desc.pInput == nullptr) {
        LOG_E("Invalid input pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (desc.inputSize == 0) {
        LOG_E("Invalid size, should be non zero");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    size_t graphSize = desc.inputSize;

    if (desc.format == ZE_GRAPH_FORMAT_NGRAPH_LITE) {
        if (!Compiler::getCompiledBlob(graphSize, graphBlobRaw, desc)) {
            LOG_E("Failed to get compiled blob!");
            return ZE_RESULT_ERROR_UNKNOWN;
        }
    } else {
        graphBlobRaw.resize(graphSize);
        memcpy(graphBlobRaw.data(), desc.pInput, graphBlobRaw.size());
    }

    parser = ElfParser::getElfParser(ctx, graphBlobRaw.data(), graphBlobRaw.size());
    if (!parser.get()) {
        LOG_E("Failed to read blob as a elf.");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    ze_result_t result = parser->parse(argumentProperties, argumentMetadata, profilingOutputSize);
    if (result != ZE_RESULT_SUCCESS) {
        LOG_E("Failed to parse blob.");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    for (const auto &prop : argumentProperties) {
        uint32_t size = getArgumentSize(prop);
        if (prop.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
            inputArgs.emplace_back(nullptr, size);
        } else {
            outputArgs.emplace_back(nullptr, size);
        }
    }

    LOG_V("Graph initialized.");
    return ZE_RESULT_SUCCESS;
}

std::shared_ptr<VPU::VPUCommand> Graph::allocateGraphInitCommand(VPU::VPUDeviceContext *ctx) {
    return parser->allocateInitCommand(ctx, graphBlobRaw.data(), graphBlobRaw.size());
}

std::shared_ptr<VPU::VPUCommand> Graph::allocateGraphExecuteCommand(VPU::VPUDeviceContext *ctx,
                                                                    void *profilingQueryPtr) {
    return parser->allocateExecuteCommand(ctx,
                                          inputArgs,
                                          outputArgs,
                                          {profilingQueryPtr, profilingOutputSize});
}

} // namespace L0
