/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/ext/source/graph/elf_parser.hpp"

#include <cstdint>

#include "level_zero/ze_api.h"
#include "level_zero/ze_graph_ext.h"
#include "level_zero_driver/include/l0_exception.hpp"
#include "npu_driver_compiler.h"
#include "umd_common.hpp"
#include "vpu_driver/source/command/vpu_inference_execute.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpux_elf/types/data_types.hpp"
#include "vpux_elf/types/section_header.hpp"
#include "vpux_headers/buffer_specs.hpp"
#include "vpux_headers/managed_buffer.hpp"
#include "vpux_headers/metadata_primitives.hpp"
#include "vpux_headers/platform.hpp"
#include "vpux_hpi.hpp"

#include <algorithm>
#include <array>
#include <exception>
#include <iterator>
#include <string.h>
#include <vpux_elf/accessor.hpp>
#include <vpux_elf/types/vpu_extensions.hpp>
#include <vpux_elf/utils/error.hpp>
#include <vpux_elf/utils/utils.hpp>
#include <vpux_headers/buffer_manager.hpp>
#include <vpux_headers/device_buffer.hpp>
#include <vpux_headers/metadata.hpp>

namespace L0 {

class DriverBufferManager : public elf::BufferManager {
  public:
    DriverBufferManager(VPU::VPUDeviceContext *context)
        : ctx(context) {}
    ~DriverBufferManager() override = default;

    VPU::VPUBufferObject::Type getBufferType(elf::Elf_Xword flag) {
        if (flag & elf::SHF_EXECINSTR)
            return VPU::VPUBufferObject::Type::WriteCombineFw;

        if (flag & elf::VPU_SHF_PROC_SHAVE)
            return VPU::VPUBufferObject::Type::WriteCombineShave;

        return VPU::VPUBufferObject::Type::WriteCombineDma;
    }

    elf::DeviceBuffer allocate(const elf::BufferSpecs &buffSpecs) override {
        LOG(GRAPH,
            "Allocate: size: %#lx, alignment: %#lx, procFlags: %#lx",
            buffSpecs.size,
            buffSpecs.alignment,
            buffSpecs.procFlags);

        size_t size = buffSpecs.size;
        if (size == 0) {
            LOG(GRAPH, "WA for buffSpecs.size == 0 -> set size to 1");
            size = 1;
        }

        VPU::VPUBufferObject *bo =
            ctx->createInternalBufferObject(size, getBufferType(buffSpecs.procFlags));
        if (bo == nullptr) {
            LOG_E("Failed to allocate the memory");
            return elf::DeviceBuffer();
        }

        LOG(GRAPH,
            "Allocated: cpu_addr: %p, vpu_addr: %#lx, size: %#lx",
            bo->getBasePointer(),
            bo->getVPUAddr(),
            bo->getAllocSize());
        return elf::DeviceBuffer(bo->getBasePointer(), bo->getVPUAddr(), size);
    }

    void deallocate(elf::DeviceBuffer &devAddress) override {
        LOG(GRAPH,
            "Deallocate: cpu: %p, vpu: %#lx, size: %lu",
            devAddress.cpu_addr(),
            devAddress.vpu_addr(),
            devAddress.size());
        if (!ctx->freeMemAlloc(devAddress.cpu_addr()))
            LOG_E("Failed to deallocate the memory");
    }

    void lock(elf::DeviceBuffer &devAddress) override {}
    void unlock(elf::DeviceBuffer &devAddress) override {}

    size_t copy(elf::DeviceBuffer &to, const uint8_t *from, size_t count) override {
        LOG(GRAPH,
            "Copy to.cpu_addr: %p, to.vpu_addr: %#lx from: %p, count: %#lx",
            to.cpu_addr(),
            to.vpu_addr(),
            from,
            count);

        if (count == 0) {
            LOG(GRAPH, "Zero copy, skipping");
            return 0;
        }

        VPU::VPUBufferObject *bo = ctx->findBuffer(to.cpu_addr());
        if (bo == nullptr) {
            LOG_E("Failed to find a buffer");
            return 0;
        }

        if (!bo->copyToBuffer(from, count, 0)) {
            LOG_E("Failed to copy a buffer");
            return 0;
        }

        return count;
    }

  private:
    VPU::VPUDeviceContext *ctx;
};

class ElfAccessManager : public elf::AccessManager {
  public:
    ElfAccessManager(uint8_t *ptr, size_t size, DriverBufferManager *manager)
        : AccessManager(size)
        , blob(ptr)
        , bufferManager(manager) {}

    ElfAccessManager(const ElfAccessManager &) = delete;
    ElfAccessManager(ElfAccessManager &&) = delete;
    ElfAccessManager &operator=(const ElfAccessManager &) = delete;
    ElfAccessManager &operator=(ElfAccessManager &&) = delete;
    ~ElfAccessManager() override = default;

    std::unique_ptr<elf::ManagedBuffer> readInternal(size_t offset,
                                                     const elf::BufferSpecs &specs) override {
        VPUX_ELF_THROW_WHEN(offset + specs.size > getSize(),
                            elf::AccessError,
                            "Read request out of bounds");

        uint8_t *start = blob + offset;

        if (hasNPUAccess(specs.procFlags)) {
            auto buffer = std::make_unique<elf::AllocatedDeviceBuffer>(bufferManager, specs);
            elf::DeviceBuffer devBuffer = buffer->getBuffer();
            bufferManager->copy(devBuffer, start, devBuffer.size());
            return buffer;
        }

        if (specs.alignment == 0 || (reinterpret_cast<size_t>(start) % specs.alignment) == 0) {
            return std::make_unique<elf::StaticBuffer>(start, specs);
        }

        auto dynBuffer = std::make_unique<elf::DynamicBuffer>(specs);
        elf::DeviceBuffer devBuffer = dynBuffer->getBuffer();
        memcpy(devBuffer.cpu_addr(), start, devBuffer.size());

        return dynBuffer;
    }

    void readExternal(size_t offset, elf::ManagedBuffer &buffer) override {
        VPUX_ELF_THROW_WHEN(offset + buffer.getBufferSpecs().size > getSize(),
                            elf::AccessError,
                            "Read request out of bounds");

        elf::DeviceBuffer devBuffer = buffer.getBuffer();
        memcpy(devBuffer.cpu_addr(), blob + offset, devBuffer.size());
    }

  private:
    static bool hasNPUAccess(uint64_t flags) {
        return (flags & (elf::SHF_EXECINSTR | elf::VPU_SHF_PROC_DMA | elf::VPU_SHF_PROC_SHAVE)) !=
               0;
    }

    uint8_t *blob = nullptr;
    DriverBufferManager *bufferManager = nullptr;
};

ElfParser::ElfParser(VPU::VPUDeviceContext *ctx,
                     std::unique_ptr<elf::BufferManager> buffer,
                     std::unique_ptr<elf::AccessManager> access,
                     std::shared_ptr<elf::HostParsedInference> hpi)
    : ctx(ctx)
    , bufferManager(std::move(buffer))
    , accessManager(std::move(access))
    , hpi(std::move(hpi)) {}

bool ElfParser::checkMagic(uint8_t *ptr, size_t size) {
    if (size == 0)
        return false;

    return elf::utils::checkELFMagic(ptr);
}

static inline elf::platform::ArchKind toArchKind(int platform) {
    switch (platform) {
    case VCL_PLATFORM_VPU3720:
        return elf::platform::ArchKind::VPUX37XX;
    case VCL_PLATFORM_VPU4000:
        return elf::platform::ArchKind::VPUX40XX;
    default:
        return elf::platform::ArchKind::UNKNOWN;
    }
}

static std::shared_ptr<elf::HostParsedInference>
createHostParsedInference(elf::BufferManager *buffer,
                          elf::AccessManager *access,
                          VPU::VPUDeviceContext *ctx,
                          std::string &errorMsg) {
    elf::HPIConfigs config = {};
    config.nnVersion = toVersion<elf::Version>(ctx->getFwMappedInferenceVersion());
    config.archKind = toArchKind(ctx->getCompilerPlatform());

    try {
        auto hpi = std::make_shared<elf::HostParsedInference>(buffer, access, config);
        hpi->load();
        return hpi;
    } catch (const elf::AllocError &err) {
        LOG_E("Failed to create elf::HostParsedInference, type: elf::AllocError, reason: %s",
              err.what());
        errorMsg = err.what();
        throw DriverError(ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY);
    } catch (elf::VersioningError &err) {
        LOG_E("Failed to create elf::HostParsedInference, type: elf::VersioningError, reason: %s",
              err.what());
        errorMsg = err.what();
        throw DriverError(ZE_RESULT_ERROR_UNSUPPORTED_VERSION);
    } catch (const elf::RuntimeError &err) {
        LOG_E("Failed to create elf::HostParsedInference, type: elf::RuntimeError, reason: %s",
              err.what());
        errorMsg = err.what();
    } catch (const elf::LogicError &err) {
        LOG_E("Failed to create elf::HostParsedInference, type: elf::LogicError, reason: %s",
              err.what());
        errorMsg = err.what();
    } catch (const std::exception &err) {
        LOG_E("Failed to create elf::HostParsedInference, type: std::exception, reason: %s",
              err.what());
        errorMsg = err.what();
    }
    return nullptr;
}

static std::shared_ptr<elf::HostParsedInference>
copyHostParsedInference(std::shared_ptr<elf::HostParsedInference> &hpi) {
    try {
        return std::make_shared<elf::HostParsedInference>(*hpi);
    } catch (const elf::AllocError &err) {
        LOG_E("Failed to copy elf::HostParsedInference, type: elf::AllocError, reason: %s",
              err.what());
        throw DriverError(ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY);
    } catch (elf::VersioningError &err) {
        LOG_E("Failed to copy elf::HostParsedInference, type: elf::VersioningError, reason: %s",
              err.what());
        throw DriverError(ZE_RESULT_ERROR_UNSUPPORTED_VERSION);
    } catch (const elf::RuntimeError &err) {
        LOG_E("Failed to copy elf::HostParsedInference, type: elf::RuntimeError, reason: %s",
              err.what());
    } catch (const elf::LogicError &err) {
        LOG_E("Failed to copy elf::HostParsedInference, type: elf::LogicError, reason: %s",
              err.what());
    } catch (const std::exception &err) {
        LOG_E("Failed to create elf::HostParsedInference, type: std::exception, reason: %s",
              err.what());
    }
    return nullptr;
}

std::unique_ptr<ElfParser> ElfParser::getElfParser(VPU::VPUDeviceContext *ctx,
                                                   uint8_t *ptr,
                                                   size_t size,
                                                   std::string &logBuffer) {
    auto bufferManager = std::make_unique<DriverBufferManager>(ctx);
    auto accessManager = std::make_unique<ElfAccessManager>(ptr, size, bufferManager.get());
    auto hpi = createHostParsedInference(bufferManager.get(), accessManager.get(), ctx, logBuffer);
    if (hpi != nullptr)
        return std::make_unique<ElfParser>(ctx,
                                           std::move(bufferManager),
                                           std::move(accessManager),
                                           std::move(hpi));

    return nullptr;
}

elf::VersionsProvider ElfParser::getElfVer(int arch) {
    return elf::VersionsProvider(toArchKind(arch));
}

static ze_graph_argument_precision_t getTensorPrecision(elf::DType type) {
    switch (type) {
    case elf::DType::DType_NOT_SET:
        return ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN;
    case elf::DType::DType_FP64:
        return ZE_GRAPH_ARGUMENT_PRECISION_FP64;
    case elf::DType::DType_FP32:
        return ZE_GRAPH_ARGUMENT_PRECISION_FP32;
    case elf::DType::DType_FP16:
        return ZE_GRAPH_ARGUMENT_PRECISION_FP16;
    case elf::DType::DType_U64:
        return ZE_GRAPH_ARGUMENT_PRECISION_UINT64;
    case elf::DType::DType_U32:
        return ZE_GRAPH_ARGUMENT_PRECISION_UINT32;
    case elf::DType::DType_U16:
        return ZE_GRAPH_ARGUMENT_PRECISION_UINT16;
    case elf::DType::DType_U8:
        return ZE_GRAPH_ARGUMENT_PRECISION_UINT8;
    case elf::DType::DType_U4:
        return ZE_GRAPH_ARGUMENT_PRECISION_UINT4;
    case elf::DType::DType_I64:
        return ZE_GRAPH_ARGUMENT_PRECISION_INT64;
    case elf::DType::DType_I32:
        return ZE_GRAPH_ARGUMENT_PRECISION_INT32;
    case elf::DType::DType_I16:
        return ZE_GRAPH_ARGUMENT_PRECISION_INT16;
    case elf::DType::DType_I8:
        return ZE_GRAPH_ARGUMENT_PRECISION_INT8;
    case elf::DType::DType_I4:
        return ZE_GRAPH_ARGUMENT_PRECISION_INT4;
    case elf::DType::DType_BIN:
        return ZE_GRAPH_ARGUMENT_PRECISION_BIN;
    case elf::DType::DType_BFP16:
        return ZE_GRAPH_ARGUMENT_PRECISION_BF16;
    default:
        return ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN;
    }
}

static constexpr std::array<std::pair<size_t, ze_graph_argument_layout_t>, 8> orderToLayout = {
    {{0x1, ZE_GRAPH_ARGUMENT_LAYOUT_C},
     {0x12, ZE_GRAPH_ARGUMENT_LAYOUT_NC},
     {0x21, ZE_GRAPH_ARGUMENT_LAYOUT_CN},
     {0x123, ZE_GRAPH_ARGUMENT_LAYOUT_CHW},
     {0x1234, ZE_GRAPH_ARGUMENT_LAYOUT_NCHW},
     {0x1342, ZE_GRAPH_ARGUMENT_LAYOUT_NHWC},
     {0x12345, ZE_GRAPH_ARGUMENT_LAYOUT_NCDHW},
     {0x13452, ZE_GRAPH_ARGUMENT_LAYOUT_NDHWC}}};

constexpr uint64_t DIM_N = 0, DIM_C = 1, DIM_H = 2, DIM_W = 3, DIM_D = 4;

constexpr std::array<std::pair<std::array<uint64_t, 5>, ze_graph_argument_layout_t>, 7>
    stridesToLayout = {{{{DIM_C}, ZE_GRAPH_ARGUMENT_LAYOUT_C},
                        {{DIM_N, DIM_C}, ZE_GRAPH_ARGUMENT_LAYOUT_NC},
                        {{DIM_C, DIM_H, DIM_W}, ZE_GRAPH_ARGUMENT_LAYOUT_CHW},
                        {{DIM_N, DIM_C, DIM_H, DIM_W}, ZE_GRAPH_ARGUMENT_LAYOUT_NCHW},
                        {{DIM_N, DIM_H, DIM_W, DIM_C}, ZE_GRAPH_ARGUMENT_LAYOUT_NHWC},
                        {{DIM_N, DIM_C, DIM_D, DIM_H, DIM_W}, ZE_GRAPH_ARGUMENT_LAYOUT_NCDHW},
                        {{DIM_N, DIM_D, DIM_H, DIM_W, DIM_C}, ZE_GRAPH_ARGUMENT_LAYOUT_NDHWC}}};

static ze_graph_argument_layout_t computeLayoutFromStride(const uint64_t *strides,
                                                          size_t stride_size) {
    const size_t TENSOR_5D_STRIDE_MAX = 6;
    const size_t TENSOR_4D_MAX = 5;

    if (strides == nullptr) {
        LOG_E("Invalid pointer");
        return ZE_GRAPH_ARGUMENT_LAYOUT_ANY;
    }

    auto *stride_begin = strides;
    auto *stride_end = strides + stride_size;

    if (stride_size == TENSOR_5D_STRIDE_MAX) {
        // Supported layouts: NCDHW, NDHWC
        const size_t TENSOR_5D_STRIDE_C = 2;
        const size_t TENSOR_5D_STRIDE_D = 3;

        auto max_stride_index =
            std::distance(stride_begin,
                          std::max_element(stride_begin + TENSOR_5D_STRIDE_C, stride_end));
        if (max_stride_index == TENSOR_5D_STRIDE_C) {
            return ZE_GRAPH_ARGUMENT_LAYOUT_NCDHW;
        } else if (max_stride_index == TENSOR_5D_STRIDE_D) {
            return ZE_GRAPH_ARGUMENT_LAYOUT_NDHWC;
        }
    } else if (stride_size == TENSOR_4D_MAX) {
        // Supported layouts: NCHW, NHWC
        const size_t TENSOR_4D_STRIDE_C = 2;
        const size_t TENSOR_4D_STRIDE_H = 3;
        const size_t TENSOR_4D_STRIDE_W = 4;

        const auto &strides_data = strides;

        auto max_stride_val = *std::max_element(stride_begin + TENSOR_4D_STRIDE_C, stride_end);
        if (max_stride_val == strides_data[TENSOR_4D_STRIDE_C] &&
            strides_data[TENSOR_4D_STRIDE_H] >= strides_data[TENSOR_4D_STRIDE_W]) {
            return ZE_GRAPH_ARGUMENT_LAYOUT_NCHW;
        } else if (max_stride_val == strides_data[TENSOR_4D_STRIDE_H] &&
                   strides_data[TENSOR_4D_STRIDE_W] >= strides_data[TENSOR_4D_STRIDE_C]) {
            return ZE_GRAPH_ARGUMENT_LAYOUT_NHWC;
        }
    }
    return ZE_GRAPH_ARGUMENT_LAYOUT_ANY;
}

static ze_graph_argument_layout_t getNetworkLayout(const elf::TensorRef &tensor) {
    if (tensor.order != 0) {
        for (const auto &itr : orderToLayout) {
            if (itr.first == tensor.order) {
                return itr.second;
            }
        }
    } else {
        if (tensor.strides_size == 0) {
            LOG_E("Invalid pointer");
            return ZE_GRAPH_ARGUMENT_LAYOUT_ANY;
        }

        for (auto &itr : stridesToLayout) {
            auto *stride_begin = tensor.strides;
            auto *stride_end = tensor.strides + tensor.strides_size;
            if (std::equal(stride_begin, stride_end, itr.first.begin())) {
                return itr.second;
            }
        }
    }

    return ZE_GRAPH_ARGUMENT_LAYOUT_ANY;
}

static ze_graph_argument_layout_t getDeviceLayout(const elf::TensorRef &tensor) {
    if (tensor.order != 0) {
        for (const auto &itr : orderToLayout) {
            if (itr.first == tensor.order) {
                return itr.second;
            }
        }
    }

    return computeLayoutFromStride(tensor.strides, tensor.strides_size);
}

static void fillDeviceProperties(const elf::TensorRef &devTensor,
                                 ze_graph_argument_properties_3_t &prop) {
    for (size_t j = 0; j < ZE_MAX_GRAPH_ARGUMENT_DIMENSIONS_SIZE; j++) {
        if (j < devTensor.dimensions_size) {
            prop.dims[j] = devTensor.dimensions[j];
        } else {
            prop.dims[j] = 1;
        }
    }
    prop.dims_count = devTensor.dimensions_size;
    prop.devicePrecision = getTensorPrecision(devTensor.data_type);
    prop.deviceLayout = getDeviceLayout(devTensor);
}

static void fillNetworkProperties(const elf::TensorRef &netTensor,
                                  ze_graph_argument_properties_3_t &prop) {
    memcpy(prop.name, netTensor.name, sizeof(prop.name) - 1);
    prop.networkPrecision = getTensorPrecision(netTensor.data_type);
    prop.networkLayout = getNetworkLayout(netTensor);
}

static void fillOVNodeProperties(const elf::OVNode &node, ze_graph_argument_properties_3_t &prop) {
    memcpy(prop.debug_friendly_name, node.friendly_name, sizeof(prop.debug_friendly_name) - 1);
    if (node.tensor_names_count > ZE_MAX_GRAPH_TENSOR_NAMES_SIZE) {
        LOG_E("Tensor names count exceeds the Graph Extension limits (%u > %u)",
              node.tensor_names_count,
              ZE_MAX_GRAPH_TENSOR_NAMES_SIZE);
        return;
    }

    for (unsigned i = 0; i < node.tensor_names_count; i++) {
        memcpy(prop.associated_tensor_names[i],
               node.tensor_names[i],
               sizeof(prop.associated_tensor_names[i]) - 1);
    }
    prop.associated_tensor_names_count = node.tensor_names_count;
}

bool ElfParser::getArgumentProperties(std::vector<ze_graph_argument_properties_3_t> &props) const {
    auto metadata = hpi->getMetadata();
    props.reserve(metadata->mInTensorDescriptors.size() + metadata->mOutTensorDescriptors.size());

    for (size_t i = 0; i < metadata->mInTensorDescriptors.size(); i++) {
        ze_graph_argument_properties_3_t prop = {};
        prop.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES;
        prop.type = ZE_GRAPH_ARGUMENT_TYPE_INPUT;

        // TODO: Add support for quantization parameters (EISW-72376)
        prop.quantReverseScale = 1.f;
        prop.quantZeroPoint = 0;

        if (metadata->mInTensorDescriptors.size() > i)
            fillDeviceProperties(metadata->mInTensorDescriptors[i], prop);

        if (metadata->mNetInputs.size() > i)
            fillNetworkProperties(metadata->mNetInputs[i], prop);

        if (metadata->mOVParameters.size() > i)
            fillOVNodeProperties(metadata->mOVParameters[i], prop);

        props.push_back(prop);
    }

    for (size_t i = 0; i < metadata->mOutTensorDescriptors.size(); i++) {
        ze_graph_argument_properties_3_t prop = {};
        prop.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES;
        prop.type = ZE_GRAPH_ARGUMENT_TYPE_OUTPUT;

        // TODO: Add support for quantization parameters (EISW-72376)
        prop.quantReverseScale = 0.f;
        prop.quantZeroPoint = 0;

        if (metadata->mOutTensorDescriptors.size() > i)
            fillDeviceProperties(metadata->mOutTensorDescriptors[i], prop);

        if (metadata->mNetOutputs.size() > i)
            fillNetworkProperties(metadata->mNetOutputs[i], prop);

        if (metadata->mOVResults.size() > i)
            fillOVNodeProperties(metadata->mOVResults[i], prop);

        props.push_back(prop);
    }

    return true;
}

constexpr std::array<std::pair<elf::OVNodeType, ze_graph_metadata_type>, 18> toMetadataType = {{
    {elf::OVNodeType_UNDEFINED, ZE_GRAPH_METADATA_TYPE_UNDEFINED},
    {elf::OVNodeType_DYNAMIC, ZE_GRAPH_METADATA_TYPE_DYNAMIC},
    {elf::OVNodeType_BOOLEAN, ZE_GRAPH_METADATA_TYPE_BOOLEAN},
    {elf::OVNodeType_BF16, ZE_GRAPH_METADATA_TYPE_BF16},
    {elf::OVNodeType_F16, ZE_GRAPH_METADATA_TYPE_F16},
    {elf::OVNodeType_F32, ZE_GRAPH_METADATA_TYPE_F32},
    {elf::OVNodeType_F64, ZE_GRAPH_METADATA_TYPE_F64},
    {elf::OVNodeType_I4, ZE_GRAPH_METADATA_TYPE_I4},
    {elf::OVNodeType_I8, ZE_GRAPH_METADATA_TYPE_I8},
    {elf::OVNodeType_I16, ZE_GRAPH_METADATA_TYPE_I16},
    {elf::OVNodeType_I32, ZE_GRAPH_METADATA_TYPE_I32},
    {elf::OVNodeType_I64, ZE_GRAPH_METADATA_TYPE_I64},
    {elf::OVNodeType_U1, ZE_GRAPH_METADATA_TYPE_U1},
    {elf::OVNodeType_U4, ZE_GRAPH_METADATA_TYPE_U4},
    {elf::OVNodeType_U8, ZE_GRAPH_METADATA_TYPE_U8},
    {elf::OVNodeType_U16, ZE_GRAPH_METADATA_TYPE_U16},
    {elf::OVNodeType_U32, ZE_GRAPH_METADATA_TYPE_U32},
    {elf::OVNodeType_U64, ZE_GRAPH_METADATA_TYPE_U64},
}};

bool ElfParser::getArgumentMetadata(std::vector<ze_graph_argument_metadata_t> &args) const {
    auto metadata = hpi->getMetadata();
    args.reserve(metadata->mNetInputs.size() + metadata->mNetOutputs.size());

    auto convert = [](const elf::OVNode &node,
                      ze_graph_argument_type_t type,
                      ze_graph_argument_metadata_t &arg) {
        arg.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_METADATA;
        arg.type = type;

        memcpy(arg.friendly_name, node.friendly_name, sizeof(arg.friendly_name) - 1);

        for (const auto &itr : toMetadataType) {
            if (itr.first == node.type) {
                arg.data_type = itr.second;
                break;
            }
        }

        std::copy(node.shape, node.shape + node.shape_size, &arg.shape[0]);
        arg.shape_size = node.shape_size;

        if (node.tensor_names_count > ZE_MAX_GRAPH_TENSOR_NAMES_SIZE) {
            LOG_E("Tensor names count exceeds the Graph Extension limits (%u > %u)",
                  node.tensor_names_count,
                  ZE_MAX_GRAPH_TENSOR_NAMES_SIZE);
            return false;
        }

        for (unsigned i = 0; i < node.tensor_names_count; i++) {
            memcpy(arg.tensor_names[i], node.tensor_names[i], sizeof(arg.tensor_names[i]) - 1);
        }
        arg.tensor_names_count = node.tensor_names_count;
        memcpy(arg.input_name, node.input_name, sizeof(arg.input_name) - 1);

        return true;
    };

    for (size_t i = 0; i < metadata->mNetInputs.size(); i++) {
        if (i >= metadata->mOVParameters.size()) {
            args.emplace_back();
            continue;
        }

        ze_graph_argument_metadata_t arg = {};
        if (!convert(metadata->mOVParameters[i], ZE_GRAPH_ARGUMENT_TYPE_INPUT, arg))
            return false;

        args.push_back(arg);
    }

    for (size_t i = 0; i < metadata->mNetOutputs.size(); i++) {
        if (i >= metadata->mOVResults.size()) {
            args.emplace_back();
            continue;
        }

        ze_graph_argument_metadata_t arg = {};
        if (!convert(metadata->mOVResults[i], ZE_GRAPH_ARGUMENT_TYPE_OUTPUT, arg))
            return false;

        args.push_back(arg);
    }

    return true;
}

bool ElfParser::getProfilingSize(uint32_t &size) const {
    const auto &profBuffers = hpi->getProfBuffers();

    if (profBuffers.size() == 0) {
        size = 0;
    } else if (profBuffers.size() == 1) {
        size = safe_cast<uint32_t>(profBuffers[0].size());
    } else {
        LOG_E("Multiple profiling tensors are not supported");
        return false;
    }

    return true;
}

bool ElfParser::applyInputOutputs(std::shared_ptr<elf::HostParsedInference> &cmdHpi,
                                  const std::vector<std::pair<const void *, uint32_t>> &inputPtrs,
                                  const std::vector<std::pair<const void *, uint32_t>> &outputPtrs,
                                  const std::pair<const void *, uint32_t> &profilingPtr,
                                  std::vector<VPU::VPUBufferObject *> &bos) {
    auto getDeviceBuffers = [this, &bos](const std::vector<std::pair<const void *, uint32_t>> &ptrs,
                                         std::vector<elf::DeviceBuffer> &buffers) {
        buffers.reserve(ptrs.size());

        for (const auto &[ptr, size] : ptrs) {
            auto *bo = ctx->findBuffer(ptr);
            if (bo == nullptr) {
                LOG_E("Failed to find a user buffer");
                return false;
            }

            bos.push_back(bo);

            uint64_t vpuAddr = ctx->getBufferVPUAddress(ptr);
            uint8_t *basePtr = static_cast<uint8_t *>(const_cast<void *>(ptr));
            buffers.emplace_back(basePtr, vpuAddr, size);
        }

        return true;
    };

    std::vector<elf::DeviceBuffer> inputDeviceBuffers;
    if (!getDeviceBuffers(inputPtrs, inputDeviceBuffers))
        return false;

    std::vector<elf::DeviceBuffer> outputDeviceBuffers;
    if (!getDeviceBuffers(outputPtrs, outputDeviceBuffers))
        return false;

    std::vector<elf::DeviceBuffer> profilingDeviceBuffers;
    if (profilingPtr.first != nullptr && !getDeviceBuffers({profilingPtr}, profilingDeviceBuffers))
        return false;

    try {
        cmdHpi->applyInputOutput(inputDeviceBuffers, outputDeviceBuffers, profilingDeviceBuffers);
    } catch (const elf::RelocError &err) {
        LOG_E("Caught reloc exception in hostParsedInference.applyInputOutput()");
        return false;
    } catch (const elf::LogicError &err) {
        LOG_E("Caught logic exception in hostParsedInference.applyInputOutput()");
        return false;
    } catch (const std::exception &err) {
        LOG_E("Unhandled exception in hostParsedInference.applyInputOutput()");
        return false;
    }
    return true;
}

std::shared_ptr<VPU::VPUInferenceExecute> ElfParser::createInferenceExecuteCommand(
    const std::vector<std::pair<const void *, uint32_t>> &inputPtrs,
    const std::vector<std::pair<const void *, uint32_t>> &outputPtrs,
    const std::pair<void *, uint32_t> &profilingPtr,
    std::shared_ptr<elf::HostParsedInference> &execHpi) {
    uint64_t inferenceId = 0;
    if (!ctx->getUniqueInferenceId(inferenceId))
        return nullptr;

    std::shared_ptr<elf::HostParsedInference> cmdHpi;
    if (needCopy) {
        cmdHpi = copyHostParsedInference(hpi);
        if (cmdHpi == nullptr) {
            LOG_E("Not able to make copy of HostParsedInference");
            return nullptr;
        }
    } else {
        cmdHpi = hpi;
    }

    std::vector<VPU::VPUBufferObject *> bos;
    auto hpiBuffer = cmdHpi->getParsedInference();
    VPU::VPUBufferObject *bo = ctx->findBuffer(hpiBuffer.cpu_addr());
    if (bo == nullptr) {
        LOG_E("Failed to find a buffer in tracked memory");
        return nullptr;
    }
    bos.push_back(bo);

    for (const auto &buffer : cmdHpi->getAllocatedBuffers()) {
        if (buffer.size() == 0)
            continue;

        VPU::VPUBufferObject *bo = ctx->findBuffer(buffer.cpu_addr());
        if (bo == nullptr) {
            LOG_E("Failed to find a buffer in tracked memory");
            return nullptr;
        }

        bos.push_back(bo);
    }

    auto cmd = VPU::VPUInferenceExecute::create(shared_from_this(),
                                                cmdHpi,
                                                inputPtrs,
                                                outputPtrs,
                                                profilingPtr,
                                                inferenceId,
                                                std::move(bos));
    if (cmd == nullptr)
        return nullptr;

    execHpi = std::move(cmdHpi);
    needCopy = true;
    return cmd;
}

ze_result_t ElfParser::parse(std::vector<ze_graph_argument_properties_3_t> &argumentProperties,
                             std::vector<ze_graph_argument_metadata_t> &argumentMetadata,
                             uint32_t &profilingOutputSize) {
    if (!getArgumentProperties(argumentProperties)) {
        LOG_E("Failed to get argument properties");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (!getArgumentMetadata(argumentMetadata)) {
        LOG_E("Failed to get argument metadata");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (!getProfilingSize(profilingOutputSize))
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    return ZE_RESULT_SUCCESS;
}

std::shared_ptr<VPU::VPUCommand> ElfParser::allocateInitCommand(VPU::VPUDeviceContext *ctx,
                                                                uint8_t *graphBlobRawData,
                                                                size_t graphBlobRawSize) {
    /* No initialize command for elf, return empty command that will be ignored */
    return std::make_shared<VPU::VPUCommand>();
}

std::shared_ptr<VPU::VPUCommand>
ElfParser::allocateExecuteCommand(VPU::VPUDeviceContext *ctx,
                                  const std::vector<std::pair<const void *, uint32_t>> &inputArgs,
                                  const std::vector<std::pair<const void *, uint32_t>> &outputArgs,
                                  const std::pair<void *, uint32_t> &profilingPtr,
                                  std::shared_ptr<elf::HostParsedInference> &execHpi) {
    return createInferenceExecuteCommand(inputArgs,
                                         outputArgs,
                                         {profilingPtr.first, profilingPtr.second},
                                         execHpi);
}

} // namespace L0
