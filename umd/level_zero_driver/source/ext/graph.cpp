/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph.hpp"

#include "blob_container.hpp"
#include "compiler.hpp"
#include "disk_cache.hpp"
#include "elf_parser.hpp"
#include "interface_parser.hpp"
#include "level_zero/ze_api.h"
#include "level_zero/ze_graph_ext.h"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/context.hpp"
#include "level_zero_driver/source/device.hpp"
#include "level_zero_driver/source/driver.hpp"
#include "npu_driver_compiler.h"
#include "profiling_data.hpp"
#include "umd_common.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpux_elf/utils/version.hpp"
#include "vpux_hpi.hpp"

#include <algorithm>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <string_view>
#include <utility>

namespace L0 {
static thread_local std::string lastFailLog;

GraphBuildLog::GraphBuildLog(Context *pCtx)
    : pContext(pCtx){};

ze_result_t GraphBuildLog::getLogString(uint32_t *pSize, char *pBuildLog) {
    if (pSize == nullptr) {
        LOG_E("Input size pointer is NULL");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (pBuildLog == nullptr) {
        *pSize = static_cast<uint32_t>(log.size() + 1);
        return ZE_RESULT_SUCCESS;
    }
    *pSize = std::min(*pSize, static_cast<uint32_t>(log.size() + 1));
    memcpy(pBuildLog, log.c_str(), *pSize);
    return ZE_RESULT_SUCCESS;
}

ze_result_t GraphBuildLog::destroy() {
    pContext->removeObject(this);
    return ZE_RESULT_SUCCESS;
}

Graph::Graph(Context *pCtx, const ze_graph_desc_2_t *pDesc, std::string &log)
    : pContext(pCtx)
    , ctx(pCtx->getDeviceContext())
    , desc(*pDesc)
    , buildFlags(desc.pBuildFlags != nullptr ? desc.pBuildFlags : "") {
    initialize(log);
}

ze_result_t Graph::create(const ze_context_handle_t hContext,
                          const ze_device_handle_t hDevice,
                          const ze_graph_desc_2_t *pDesc,
                          ze_graph_handle_t *phGraph,
                          ze_graph_build_log_handle_t *phGraphBuildLog) {
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
    lastFailLog.clear();
    auto logObject = std::make_unique<GraphBuildLog>(Context::fromHandle(hContext));
    auto &logBuffer = logObject->getBuffer();
    if (phGraphBuildLog) {
        *phGraphBuildLog = logObject->toHandle();
        Context::fromHandle(hContext)->appendObject(std::move(logObject));
    }
    try {
        auto pGraph = std::make_unique<Graph>(Context::fromHandle(hContext), pDesc, logBuffer);
        *phGraph = pGraph.get();
        Context::fromHandle(hContext)->appendObject(std::move(pGraph));

        LOG(GRAPH, "Graph created - %p", *phGraph);
    } catch (const DriverError &err) {
        lastFailLog = logBuffer;
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

    if (blob == nullptr || blob->size == 0) {
        LOG_E("Native binary does not exist for Graph");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (*pSize == 0 || *pSize > blob->size) {
        *pSize = blob->size;
    }

    if (pGraphNativeBinary != nullptr) {
        memcpy(pGraphNativeBinary, blob->ptr, *pSize);
    }
    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getNativeBinary2(size_t *pSize, const uint8_t **pGraphNativeBinary) {
    *pSize = blob->size;
    *pGraphNativeBinary = blob->ptr;

    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::setArgumentValue(uint32_t argIndex, const void *pArgValue) {
    if (pArgValue == nullptr)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (argIndex >= (argumentProperties.size()))
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (argIndex < inputArgs.size())
        inputArgs[argIndex] = pArgValue;
    else
        outputArgs[argIndex - inputArgs.size()] = pArgValue;
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

ze_result_t Graph::getProperties2(ze_graph_properties_2_t *pGraphProperties) {
    pGraphProperties->numGraphArgs = safe_cast<uint32_t>(argumentProperties.size());
    pGraphProperties->initStageRequired = ZE_GRAPH_STAGE_INITIALIZE;

    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getProperties3(ze_graph_properties_3_t *pGraphProperties) {
    pGraphProperties->numGraphArgs = safe_cast<uint32_t>(argumentProperties.size());
    pGraphProperties->initStageRequired = ZE_GRAPH_STAGE_INITIALIZE;
    pGraphProperties->flags = propFlags;
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
    if (!parser) {
        LOG_E("Graph object is not properly initialized!");
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
        auto profilingMemory =
            parser->allocateInternal(getFwDataCacheAlign(profilingOutputSize) * count);
        auto profilingPool =
            std::make_unique<GraphProfilingPool>(profilingOutputSize,
                                                 count,
                                                 blob.get(),
                                                 std::move(profilingMemory),
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

    pDeviceGraphProperties->graphExtensionVersion = ZE_GRAPH_EXT_VERSION_CURRENT;
    pDeviceGraphProperties->graphFormatsSupported = ZE_GRAPH_FORMAT_NATIVE;

    vcl_compiler_properties_t vclProp = {};
    if (Compiler::getCompilerProperties(&vclProp) == ZE_RESULT_SUCCESS) {
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

    getDeviceGraphProperties(
        hDevice,
        reinterpret_cast<ze_device_graph_properties_t *>(pDeviceGraphProperties2));

    Device *dev = Device::fromHandle(hDevice);
    VPU::VPUDevice *vdev = dev->getVPUDevice();

    const auto &hwInfo = vdev->getHwInfo();

    uint32_t runtimeVer = static_cast<uint32_t>(hwInfo.fwMappedInferenceVersion);
    pDeviceGraphProperties2->runtimeVersion = toVersion<ze_graph_version_info_t>(runtimeVer);

    elf::VersionsProvider elfVer = ElfParser::getElfVer(hwInfo.deviceId);
    pDeviceGraphProperties2->elfVersion = {elfVer.getLibraryELFVersion().getMajor(),
                                           elfVer.getLibraryELFVersion().getMinor(),
                                           elfVer.getLibraryELFVersion().getPatch()};

    return ZE_RESULT_SUCCESS;
}

static void
addOptionToBuildFlags(std::string_view key, std::string_view value, std::string &buildFlags) {
    if (buildFlags.find("--config") == std::string::npos) {
        buildFlags += " --config";
    }

    buildFlags += " " + std::string(key) + "=\"" + std::string(value) + "\"";
}

static void setUpFlagsFromEnvVariable(std::string &graphBuildFlags) {
    std::string buildFlags(graphBuildFlags);

    const char *env = getenv("ZE_INTEL_NPU_COMPILER_EXTRA_BUILD_FLAGS");
    if (!env)
        return;

    auto getKey = [&](std::string &options) {
        auto keyEnd = options.find("=");
        if (keyEnd == std::string::npos)
            return std::string();

        auto key = options.substr(0, keyEnd);
        options.erase(0, keyEnd + 1);
        key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
        return key;
    };
    auto getValue = [&](std::string &options) {
        auto valueStart = options.find("\"");
        if (valueStart == std::string::npos)
            return std::string();
        auto valueEnd = options.find("\"", valueStart + 1);
        if (valueEnd == std::string::npos)
            return std::string();

        auto value = options.substr(valueStart + 1, valueEnd - valueStart - 1);
        options.erase(0, valueEnd + 1);
        value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
        return value;
    };

    std::unordered_map<std::string, std::string> options;
    auto configStart = buildFlags.find("--config");
    auto flagsString = buildFlags.substr(0, configStart);
    auto configString =
        configStart == std::string::npos
            ? std::string("")
            : buildFlags.substr(configStart + sizeof("--config"), std::string::npos);

    for (auto key = getKey(configString); key.size(); key = getKey(configString)) {
        auto value = getValue(configString);
        options.emplace(std::make_pair(key, value));
    }

    std::string optionsFromEnv(env);
    for (auto key = getKey(optionsFromEnv); key.size(); key = getKey(optionsFromEnv)) {
        auto value = getValue(optionsFromEnv);

        auto it = options.find(key);
        if (it == options.end()) {
            LOG_W("Compilation option added from environment variable %s = %s",
                  key.c_str(),
                  value.c_str());
            options.emplace(std::make_pair(key, value));
        } else {
            LOG_W("Compilation option %s will be overridden by environment variable, from value %s "
                  "to %s",
                  key.c_str(),
                  it->second.c_str(),
                  value.c_str());
            it->second = std::move(value);
        }
    }

    flagsString += " --config ";
    for (auto it = options.begin(); it != options.end(); ++it) {
        flagsString += it->first + "=\"" + it->second + "\" ";
    }

    graphBuildFlags = std::move(flagsString);
    LOG_W("Compilation options were modified by environment settings. Configuration string passed "
          "to compiler:");
    LOG_W("%s", graphBuildFlags.c_str());
}

void Graph::addDeviceConfigToBuildFlags() {
    if (desc.flags & ZE_GRAPH_FLAG_ENABLE_PROFILING) {
        addOptionToBuildFlags("PERF_COUNT", "YES", buildFlags);
    }
    setUpFlagsFromEnvVariable(buildFlags);
}

void Graph::initialize(std::string &log) {
    L0_THROW_WHEN(desc.pInput == nullptr,
                  "Invalid input pointer",
                  ZE_RESULT_ERROR_INVALID_NULL_POINTER);
    L0_THROW_WHEN(desc.inputSize == 0, "Invalid size", ZE_RESULT_ERROR_INVALID_SIZE);

    DiskCache &cache = Driver::getInstance()->getDiskCache();
    DiskCache::Key key;

    LOG(GRAPH,
        "ze_graph_desc_2_t = format: %#x, pInput: %p, inputSize: %lu, flags: %#x, pBuildFlags: %s",
        desc.format,
        desc.pInput,
        desc.inputSize,
        desc.flags,
        desc.pBuildFlags);

    bool isInputPersistent = desc.flags & ZE_GRAPH_FLAG_INPUT_GRAPH_PERSISTENT;
    switch (desc.format) {
    case ZE_GRAPH_FORMAT_NATIVE:
        blob = std::make_unique<BlobContainer>(const_cast<uint8_t *>(desc.pInput), desc.inputSize);
        propFlags = ZE_GRAPH_PROPERTIES_FLAG_PRE_COMPILED;
        break;
    case ZE_GRAPH_FORMAT_NGRAPH_LITE:
        // Binary from compilation is owned by driver, so we can assume it is persistent
        isInputPersistent = true;
        addDeviceConfigToBuildFlags();
        desc.pBuildFlags = buildFlags.c_str();

        if (!(desc.flags & ZE_GRAPH_FLAG_DISABLE_CACHING)) {
            key = cache.computeKey(desc);
            blob = cache.getBlob(key);
            if (blob) {
                propFlags = ZE_GRAPH_PROPERTIES_FLAG_LOADED_FROM_CACHE;
                log += "ZE DynamicCaching cache_status_t: cache_status_t::found\n";
                /* Cache status is stored also in fail log due to back compatibility */
                lastFailLog = "ZE DynamicCaching cache_status_t: cache_status_t::found\n";
            }
        }

        if (blob == nullptr) {
            auto ret = Compiler::getCompiledBlob(ctx, desc, blob, log);
            if (ret != ZE_RESULT_SUCCESS) {
                LOG_E("Failed to get compiled blob! Result:%#x", ret);
                throw DriverError(ret);
            }

            propFlags = ZE_GRAPH_PROPERTIES_FLAG_COMPILED;
            if (!(desc.flags & ZE_GRAPH_FLAG_DISABLE_CACHING)) {
                cache.setBlob(key, blob);
                log += "ZE DynamicCaching cache_status_t: cache_status_t::stored\n";
                /* Cache status is stored also in fail log due to back compatibility */
                lastFailLog = "ZE DynamicCaching cache_status_t: cache_status_t::stored\n";
            }
        }
        break;
    default:
        LOG_E("Graph desc (ze_graph_desc_2_t) format invalid.");
        log += "[NPU_DRV] Graph desc (ze_graph_desc_2_t) format invalid.\n";
        throw DriverError(ZE_RESULT_ERROR_INVALID_ARGUMENT);
    }

    if (ElfParser::checkMagic(blob)) {
        LOG(GRAPH, "Detected Elf format");
        parser = ElfParser::getElfParser(ctx, blob, log, isInputPersistent);
    } else {
        LOG_E("Failed to recognize blob format");
        log += "[NPU_DRV] Failed to recognize native binary format\n";
        throw DriverError(ZE_RESULT_ERROR_INVALID_ARGUMENT);
    }

    L0_THROW_WHEN(!parser.get(), "Failed to get parser", ZE_RESULT_ERROR_INVALID_ARGUMENT);

    ze_result_t result = parser->parse(argumentProperties, argumentMetadata, profilingOutputSize);
    L0_THROW_WHEN(result != ZE_RESULT_SUCCESS,
                  "Failed to parse blob",
                  ZE_RESULT_ERROR_INVALID_ARGUMENT);

    for (const auto &prop : argumentProperties) {
        if (prop.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
            inputArgs.emplace_back(nullptr);
        } else {
            outputArgs.emplace_back(nullptr);
        }
    }
}

ze_result_t Graph::parserInitialize() {
    return parser->initialize();
}

std::shared_ptr<VPU::VPUCommand> Graph::allocateGraphInitCommand(VPU::VPUDeviceContext *ctx) {
    return parser->allocateInitCommand(ctx);
}

std::shared_ptr<VPU::VPUCommand>
Graph::allocateGraphExecuteCommand(GraphProfilingQuery *profilingQuery) {
    return parser->allocateExecuteCommand(inputArgs, outputArgs, profilingQuery);
}

ze_result_t Graph::getLogString(uint32_t *pSize, char *pBuildLog) {
    if (pSize == nullptr) {
        LOG_E("Input size pointer is NULL");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (pBuildLog == nullptr) {
        *pSize = static_cast<uint32_t>(lastFailLog.size() + 1);
        return ZE_RESULT_SUCCESS;
    }

    *pSize = std::min(*pSize, static_cast<uint32_t>(lastFailLog.size() + 1));
    memcpy(pBuildLog, lastFailLog.c_str(), *pSize);

    return ZE_RESULT_SUCCESS;
}

ze_result_t Graph::getSupportedOptions(ze_device_handle_t hDevice,
                                       ze_npu_options_type_t type,
                                       size_t *pSize,
                                       char *pSupportedOptions) {
    if (type == ZE_NPU_DRIVER_OPTIONS) {
        *pSize = 0;
        return ZE_RESULT_SUCCESS;
    }

    if (type == ZE_NPU_COMPILER_OPTIONS) {
        Device *dev = Device::fromHandle(hDevice);
        VPU::VPUDevice *vdev = dev->getVPUDevice();

        return Compiler::getSupportedOptions(vdev, pSize, pSupportedOptions);
    }

    return ZE_RESULT_ERROR_INVALID_ENUMERATION;
}

ze_result_t Graph::isOptionSupported(ze_device_handle_t hDevice,
                                     ze_npu_options_type_t type,
                                     const char *pOption,
                                     const char *pValue) {
    if (type == ZE_NPU_DRIVER_OPTIONS) {
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
    }

    if (type == ZE_NPU_COMPILER_OPTIONS) {
        Device *dev = Device::fromHandle(hDevice);
        VPU::VPUDevice *vdev = dev->getVPUDevice();

        return Compiler::isOptionSupported(vdev, pOption, pValue);
    }

    return ZE_RESULT_ERROR_INVALID_ENUMERATION;
}

} // namespace L0
