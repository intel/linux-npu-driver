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

    free(pKernelData);

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

ze_result_t Graph::getArgumentProperties(boost::safe_numerics::safe<uint32_t> argIndex,
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

    *pGraphArgProps = argumentProperties[argIndex];
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

ze_result_t Graph::getDeviceGraphProperties(ze_device_graph_properties_t *pDeviceGraphProperties) {
    if (pDeviceGraphProperties == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    *pDeviceGraphProperties = {};

    pDeviceGraphProperties->pNext = nullptr;
    pDeviceGraphProperties->graphExtensionVersion = ZE_GRAPH_EXT_VERSION_CURRENT;
    pDeviceGraphProperties->graphFormatsSupported = ZE_GRAPH_FORMAT_NATIVE;

    if (!Compiler::getCompilerProperties(pDeviceGraphProperties)) {
        LOG_E("Failed to get compiler properties!");
        return ZE_RESULT_ERROR_UNKNOWN;
    }
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getUserKernelData() {
    LOG_I("Separate activation kernel will be loaded");

    if (desc.pNext == nullptr) {
        LOG_E("Invalid input pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    auto *actKernelDesc = static_cast<ze_activation_kernel_desc_t *>(desc.pNext);

    if (actKernelDesc->stype != ZE_STRUCTURE_TYPE_GRAPH_ACTIVATION_KERNEL) {
        LOG_E("Invalid structure type");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }
    if (actKernelDesc->kernelDataSize == 0) {
        LOG_E("Invalid size, should be non zero");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }
    if (actKernelDesc->pKernelData == nullptr) {
        LOG_E("Invalid input pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    kernelDataSize = actKernelDesc->kernelDataSize;
    pKernelData = malloc(kernelDataSize);
    if (pKernelData == nullptr) {
        LOG_E("Failed to alloc size %ld\n", kernelDataSize);
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;
    }

    memcpy(pKernelData, actKernelDesc->pKernelData, kernelDataSize);

    return ZE_RESULT_SUCCESS;
}

static uint32_t getArgumentSize(const ze_graph_argument_properties_t &prop) {
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

    elfParser = ElfParser::getElfParser(ctx, graphBlobRaw.data(), graphBlobRaw.size());
    if (!elfParser.has_value()) {
        LOG_E("Failed to get Elf executor");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    elfParser->getArgumentProperties(argumentProperties);

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
    struct drm_ivpu_param deviceParameter = {};

    if (elfParser.has_value()) {
        deviceParameter.param = DRM_IVPU_PARAM_UNIQUE_INFERENCE_ID;
        if (ctx->getDriverApi().getDeviceParam(&deviceParameter)) {
            LOG_E("Failed to get unique inference ID");
            return nullptr;
        }

        blobId = deviceParameter.value;
        LOG_I("Blob Id assigned to graph: %#lx", blobId);

        // TODO: There is no initialize command for elf format
        return VPU::VPUBarrierCommand::create();
    }

    deviceParameter.param = DRM_IVPU_PARAM_UNIQUE_INFERENCE_ID;
    if (ctx->getDriverApi().getDeviceParam(&deviceParameter)) {
        LOG_E("Failed to get unique inference ID");
        return nullptr;
    }

    blobId = deviceParameter.value;
    LOG_I("Blob Id assigned to graph: %#lx", blobId);

    auto cmd = VPU::VPUGraphInitCommand::create(ctx,
                                                blobId,
                                                graphBlobRaw.data(),
                                                graphBlobRaw.size(),
                                                scratchSize,
                                                metadataSize,
                                                pKernelData,
                                                kernelDataSize);
    if (cmd == nullptr) {
        return nullptr;
    }

    graphInitCmds.emplace_back(cmd);
    return graphInitCmds.back();
}

std::shared_ptr<VPU::VPUCommand> Graph::allocateGraphExecuteCommand(VPU::VPUDeviceContext *ctx,
                                                                    void *profilingQueryPtr) {
    if (elfParser.has_value()) {
        if (!elfParser->applyInputOutputs(inputArgs, outputArgs)) {
            LOG_E("Failed to apply inputs and outputs arguments");
            return nullptr;
        }

        return elfParser->getCommand(blobId);
    }

    std::vector<VPU::VPUBufferObject *> graphInitBufferObjects;

    if (graphInitCmds.size() > 0)
        graphInitBufferObjects = graphInitCmds.back()->getAssociateBufferObjects();
    else {
        LOG_E("Invalid graph init not available yet!");
        return nullptr;
    }

    return VPU::VPUGraphExecuteCommand::create(ctx,
                                               blobId,
                                               inputArgs,
                                               outputArgs,
                                               graphInitBufferObjects,
                                               profilingOutputSize,
                                               profilingQueryPtr);
}

} // namespace L0
