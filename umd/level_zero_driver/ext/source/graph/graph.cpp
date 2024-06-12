/*
 * Copyright (C) 2022-2024 Intel Corporation
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
#include "vpux_driver_compiler.h"

namespace L0 {

static thread_local std::string lastErrorMsg = {};

Graph::Graph(Context *pCtx, const ze_graph_desc_2_t *pDesc)
    : pContext(pCtx)
    , ctx(pCtx->getDeviceContext())
    , desc(*pDesc) {
    initialize();
}

ze_result_t Graph::create(const ze_context_handle_t hContext,
                          const ze_device_handle_t hDevice,
                          const ze_graph_desc_2_t *pDesc,
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

    try {
        auto pGraph = std::make_unique<Graph>(Context::fromHandle(hContext), pDesc);
        *phGraph = pGraph.get();
        Context::fromHandle(hContext)->appendObject(std::move(pGraph));

        LOG(GRAPH, "Graph created - %p", *phGraph);
    } catch (const DriverError &err) {
        return err.result();
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::destroy() {
    pContext->removeObject(this);
    LOG(GRAPH, "Graph destroyed - %p", this);
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getNativeBinary(size_t *pSize, uint8_t *pGraphNativeBinary) {
    if (pSize == nullptr) {
        LOG_E("Input size pointer is NULL");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (graphBlobRaw.size() == 0) {
        LOG_E("Native binary does not exist for Graph");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (*pSize == 0 || *pSize > graphBlobRaw.size()) {
        *pSize = graphBlobRaw.size();
    }

    if (pGraphNativeBinary != nullptr) {
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
        LOG_E("Invalid pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    pGraphProperties->numGraphArgs = safe_cast<uint32_t>(argumentProperties.size());
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getArgumentProperties(uint32_t argIndex,
                                         ze_graph_argument_properties_t *pGraphArgProps) {
    if (pGraphArgProps == nullptr) {
        LOG_E("Invalid pointer for argument properties");
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
        LOG_E("Invalid pointer for argument properties");
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
        LOG_E("Invalid pointer for argument properties");
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
        LOG_E("Invalid pointer for argument properties");
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
        LOG(GRAPH, "GraphProfiling is not available for this inference");
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

    try {
        auto profilingPool =
            std::make_unique<GraphProfilingPool>(ctx,
                                                 profilingOutputSize,
                                                 count,
                                                 &graphBlobRaw,
                                                 [this](auto *x) { profilingPools.erase(x); });
        auto [it, success] = profilingPools.emplace(profilingPool.get(), std::move(profilingPool));
        L0_THROW_WHEN(!success,
                      "Failed to place new GraphProfilingPool in profiling pools map",
                      ZE_RESULT_ERROR_UNKNOWN);

        *phProfilingPool = it->second.get();
        LOG(GRAPH, "GraphProfilingPool created - %p", *phProfilingPool);
    } catch (const DriverError &err) {
        return err.result();
    }

    return ZE_RESULT_SUCCESS;
};

ze_result_t Graph::getProfilingDataProperties(
    ze_device_profiling_data_properties_t *pDeviceProfilingDataProperties) {
    if (pDeviceProfilingDataProperties == nullptr) {
        LOG_E("Invalid profiling data properties pointer");
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
    pDeviceGraphProperties->graphExtensionVersion = ZE_GRAPH_EXT_VERSION_CURRENT;
    pDeviceGraphProperties->graphFormatsSupported = ZE_GRAPH_FORMAT_NATIVE;

    vcl_compiler_properties_t vclProp = {};
    if (Compiler::getCompilerProperties(&vclProp)) {
        pDeviceGraphProperties->compilerVersion.major = vclProp.version.major;
        pDeviceGraphProperties->compilerVersion.minor = vclProp.version.minor;
        pDeviceGraphProperties->graphFormatsSupported = ZE_GRAPH_FORMAT_NGRAPH_LITE;
        pDeviceGraphProperties->maxOVOpsetVersionSupported = vclProp.supportedOpsets;
    } else {
        LOG_W("Failed to get compiler properties!");
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t
Graph::getDeviceGraphProperties2(ze_device_handle_t hDevice,
                                 ze_device_graph_properties_2_t *pDeviceGraphProperties2) {
    if (pDeviceGraphProperties2 == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    ze_device_graph_properties_t deviceGraphProperties = {};
    if (getDeviceGraphProperties(hDevice, &deviceGraphProperties))
        LOG_W("Failed to get compiler properties!");

    Device *dev = Device::fromHandle(hDevice);
    VPU::VPUDevice *vdev = dev->getVPUDevice();

    const auto &hwInfo = vdev->getHwInfo();

    uint32_t runtimeVer = static_cast<uint32_t>(hwInfo.fwMappedInferenceVersion);
    pDeviceGraphProperties2->runtimeVersion = toVersion<ze_graph_version_info_t>(runtimeVer);

    elf::VersionsProvider elfVer = ElfParser::getElfVer(hwInfo.compilerPlatform);
    pDeviceGraphProperties2->elfVersion = {elfVer.getLibraryELFVersion().getMajor(),
                                           elfVer.getLibraryELFVersion().getMinor(),
                                           elfVer.getLibraryELFVersion().getPatch()};

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

    return safe_cast<uint32_t>(size);
}

void Graph::initialize() {
    L0_THROW_WHEN(desc.pInput == nullptr,
                  "Invalid input pointer",
                  ZE_RESULT_ERROR_INVALID_NULL_POINTER);
    L0_THROW_WHEN(desc.inputSize == 0, "Invalid size", ZE_RESULT_ERROR_INVALID_SIZE);

    size_t graphSize = desc.inputSize;
    if (desc.format == ZE_GRAPH_FORMAT_NGRAPH_LITE) {
        DiskCache &cache = Driver::getInstance()->getDiskCache();
        DiskCache::Key key;
        if (!(desc.flags & ZE_GRAPH_FLAG_DISABLE_CACHING)) {
            key = cache.computeKey(desc);
            graphBlobRaw = cache.getBlob(key);
        }

        if (graphBlobRaw.empty()) {
            if (!Compiler::getCompiledBlob(ctx, graphSize, graphBlobRaw, desc, lastErrorMsg)) {
                LOG_E("Failed to get compiled blob!");
                throw DriverError(ZE_RESULT_ERROR_UNKNOWN);
            }

            if (!(desc.flags & ZE_GRAPH_FLAG_DISABLE_CACHING)) {
                cache.setBlob(key, graphBlobRaw);
            }
        }
    } else {
        graphBlobRaw.resize(graphSize);
        memcpy(graphBlobRaw.data(), desc.pInput, graphBlobRaw.size());
    }

    if (ElfParser::checkMagic(graphBlobRaw.data(), graphBlobRaw.size())) {
        LOG(GRAPH, "Detected Elf format");
        parser =
            ElfParser::getElfParser(ctx, graphBlobRaw.data(), graphBlobRaw.size(), lastErrorMsg);
    } else {
        LOG_E("Failed to recognize blob format");
        lastErrorMsg = "Failed to recognize native binary format";
        throw DriverError(ZE_RESULT_ERROR_INVALID_ARGUMENT);
    }

    L0_THROW_WHEN(!parser.get(), "Failed to get parser", ZE_RESULT_ERROR_INVALID_ARGUMENT);

    ze_result_t result = parser->parse(argumentProperties, argumentMetadata, profilingOutputSize);
    L0_THROW_WHEN(result != ZE_RESULT_SUCCESS,
                  "Failed to parse blob",
                  ZE_RESULT_ERROR_INVALID_ARGUMENT);

    for (const auto &prop : argumentProperties) {
        uint32_t size = getArgumentSize(prop);
        if (prop.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
            inputArgs.emplace_back(nullptr, size);
        } else {
            outputArgs.emplace_back(nullptr, size);
        }
    }
}

std::shared_ptr<VPU::VPUCommand> Graph::allocateGraphInitCommand(VPU::VPUDeviceContext *ctx) {
    return parser->allocateInitCommand(ctx, graphBlobRaw.data(), graphBlobRaw.size());
}

std::unique_ptr<InferenceExecutor> Graph::getGraphExecutor(VPU::VPUDeviceContext *ctx,
                                                           void *profilingQueryPtr) {
    return std::make_unique<InferenceExecutor>(
        parser,
        ctx,
        inputArgs,
        outputArgs,
        std::make_pair(profilingQueryPtr, profilingOutputSize));
}

ze_result_t Graph::getLogString(uint32_t *pSize, char *pBuildLog) {
    if (pSize == nullptr) {
        LOG_E("Input size pointer is NULL");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (*pSize == 0) {
        *pSize = static_cast<uint32_t>(lastErrorMsg.size() + 1);
        return ZE_RESULT_SUCCESS;
    }

    if (pBuildLog == nullptr) {
        LOG_E("Invalid pBuildLog pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    *pSize = std::min(*pSize, static_cast<uint32_t>(lastErrorMsg.size() + 1));
    memcpy(pBuildLog, lastErrorMsg.c_str(), *pSize);

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
