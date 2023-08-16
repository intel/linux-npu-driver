/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/ext/source/graph/elf_parser.hpp"
#include "level_zero/ze_graph_ext.h"

#include <api/vpu_nnrt_api.h>
#include <boost/numeric/conversion/cast.hpp>
#include <string.h>
#include <vpux_elf/accessor.hpp>
#include <vpux_headers/buffer_manager.hpp>
#include <vpux_headers/device_buffer.hpp>
#include <vpux_headers/metadata.hpp>
#include <vpux_elf/types/vpu_extensions.hpp>
#include <vpux_elf/utils/error.hpp>

namespace L0 {

class DriverBufferManager : public elf::BufferManager {
  public:
    DriverBufferManager(VPU::VPUDeviceContext *context)
        : ctx(context) {}
    ~DriverBufferManager() override = default;

    VPU::VPUBufferObject::Type getBufferType(elf::Elf_Xword flag) {
        if (flag & elf::SHF_EXECINSTR)
            return VPU::VPUBufferObject::Type::WriteCombineLow;

        return VPU::VPUBufferObject::Type::WriteCombineHigh;
    }

    elf::DeviceBuffer allocate(const elf::BufferSpecs &buffSpecs) override {
        LOG_I("Allocate: size: %#lx, alignment: %#lx, procFlags: %#lx",
              buffSpecs.size,
              buffSpecs.alignment,
              buffSpecs.procFlags);

        size_t size = buffSpecs.size;
        if (size == 0) {
            LOG_W("WA for buffSpecs.size == 0 -> set size to 1");
            size = 1;
        }

        VPU::VPUBufferObject *bo =
            ctx->createInternalBufferObject(size, getBufferType(buffSpecs.procFlags));
        if (bo == nullptr) {
            LOG_E("Failed to allocate the memory");
            return elf::DeviceBuffer();
        }

        LOG_I("Allocated: cpu_addr: %p, vpu_addr: %#lx, size: %#lx",
              bo->getBasePointer(),
              bo->getVPUAddr(),
              bo->getAllocSize());
        return elf::DeviceBuffer(bo->getBasePointer(), bo->getVPUAddr(), size);
    }

    void deallocate(elf::DeviceBuffer &devAddress) override {
        LOG_I("Deallocate: cpu: %p, vpu: %#lx, size: %lu",
              devAddress.cpu_addr(),
              devAddress.vpu_addr(),
              devAddress.size());
        if (!ctx->freeMemAlloc(devAddress.cpu_addr()))
            LOG_E("Failed to deallocate the memory");
    }

    void lock(elf::DeviceBuffer &devAddress) override {}
    void unlock(elf::DeviceBuffer &devAddress) override {}

    size_t copy(elf::DeviceBuffer &to, const uint8_t *from, size_t count) override {
        LOG_I("Copy to.cpu_addr: %p, to.vpu_addr: %#lx from: %p, count: %#lx",
              to.cpu_addr(),
              to.vpu_addr(),
              from,
              count);

        if (count == 0) {
            LOG_W("Zero copy, skipping");
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

ElfParser::ElfParser(VPU::VPUDeviceContext *ctx,
                     std::unique_ptr<elf::BufferManager> buffer,
                     std::unique_ptr<elf::AccessManager> access,
                     std::unique_ptr<elf::HostParsedInference> hpi)
    : ctx(ctx)
    , bufferManager(std::move(buffer))
    , accessManager(std::move(access))
    , hpi(std::move(hpi)) {}

ElfParser::~ElfParser() {}

static std::unique_ptr<elf::HostParsedInference>
createHostParsedInference(elf::BufferManager *buffer, elf::AccessManager *access) {
    try {
        return std::make_unique<elf::HostParsedInference>(buffer, access);
    } catch (const elf::RuntimeError &err) {
        LOG_E("Failed to create elf::HostParsedInference, type: elf::RuntimeError, reason: %s",
              err.what());
    } catch (const elf::LogicError &err) {
        LOG_E("Failed to create elf::HostParsedInference, type: elf::LogicError, reason: %s",
              err.what());
    } catch (const std::exception &err) {
        LOG_E("Unhandled exception while creating elf::HostParsedInference, reason: %s",
              err.what());
    }
    return nullptr;
}

std::unique_ptr<ElfParser>
ElfParser::getElfParser(VPU::VPUDeviceContext *ctx, uint8_t *ptr, size_t size) {
    auto bufferManager = std::make_unique<DriverBufferManager>(ctx);
    auto accessManager = std::make_unique<elf::ElfDDRAccessManager>(ptr, size);
    auto hpi = createHostParsedInference(bufferManager.get(), accessManager.get());
    if (hpi != nullptr)
        return std::make_unique<ElfParser>(ctx,
                                           std::move(bufferManager),
                                           std::move(accessManager),
                                           std::move(hpi));

    return nullptr;
}

ze_graph_argument_precision_t ElfParser::getTensorPrecision(elf::DType type) {
    switch (type) {
    case elf::DType::DType_NOT_SET:
        return ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN;
    case elf::DType::DType_FP32:
        return ZE_GRAPH_ARGUMENT_PRECISION_FP32;
    case elf::DType::DType_FP16:
        return ZE_GRAPH_ARGUMENT_PRECISION_FP16;
    case elf::DType::DType_U16:
        return ZE_GRAPH_ARGUMENT_PRECISION_UINT16;
    case elf::DType::DType_U8:
        return ZE_GRAPH_ARGUMENT_PRECISION_UINT8;
    case elf::DType::DType_U4:
        return ZE_GRAPH_ARGUMENT_PRECISION_UINT4;
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

constexpr float DIM_N = 0.f, DIM_C = 1.f, DIM_H = 2.f, DIM_W = 3.f, DIM_D = 4.f;

constexpr std::array<std::pair<std::array<float, 5>, ze_graph_argument_layout_t>, 7>
    stridesToLayout = {{{{DIM_C}, ZE_GRAPH_ARGUMENT_LAYOUT_C},
                        {{DIM_N, DIM_C}, ZE_GRAPH_ARGUMENT_LAYOUT_NC},
                        {{DIM_C, DIM_H, DIM_W}, ZE_GRAPH_ARGUMENT_LAYOUT_CHW},
                        {{DIM_N, DIM_C, DIM_H, DIM_W}, ZE_GRAPH_ARGUMENT_LAYOUT_NCHW},
                        {{DIM_N, DIM_H, DIM_W, DIM_C}, ZE_GRAPH_ARGUMENT_LAYOUT_NHWC},
                        {{DIM_N, DIM_C, DIM_D, DIM_H, DIM_W}, ZE_GRAPH_ARGUMENT_LAYOUT_NCDHW},
                        {{DIM_N, DIM_D, DIM_H, DIM_W, DIM_C}, ZE_GRAPH_ARGUMENT_LAYOUT_NDHWC}}};

/* Compute layout from strides. Copied from blobParser and kmb-plugin. */
static ze_graph_argument_layout_t computeLayoutFromStride(const float *strides,
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

void ElfParser::getArgumentProperties(std::vector<ze_graph_argument_properties_3_t> &props) const {
    auto metadata = hpi->getMetadata();

    props.reserve(metadata.in_tenosr_count + metadata.out_tensor_count);

    auto convert = [](const elf::TensorRef &devTensor,
                      const elf::TensorRef &netTensor,
                      const elf::OVNode &node,
                      ze_graph_argument_type_t argType) {
        ze_graph_argument_properties_3_t prop = {};
        prop.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES;

        strncpy(prop.name, netTensor.name, std::min(sizeof(prop.name), strlen(netTensor.name)));
        prop.type = argType;
        for (size_t j = 0; j < ZE_MAX_GRAPH_ARGUMENT_DIMENSIONS_SIZE; j++) {
            if (j < devTensor.dimensions_size) {
                prop.dims[j] = devTensor.dimensions[j];
            } else {
                prop.dims[j] = 1;
            }
        }
        prop.dims_count = devTensor.dimensions_size;
        prop.networkPrecision = getTensorPrecision(netTensor.data_type);
        prop.networkLayout = getNetworkLayout(netTensor);
        prop.devicePrecision = getTensorPrecision(devTensor.data_type);
        prop.deviceLayout = getDeviceLayout(devTensor);

        // TODO: Add support for quantization parameters
        prop.quantReverseScale = 1.f;
        prop.quantZeroPoint = 0;

        strncpy(prop.debug_friendly_name,
                node.friendly_name,
                std::min(sizeof(prop.debug_friendly_name), strlen(node.friendly_name)));

        for (unsigned i = 0; i < node.tensor_names_count; i++) {
            strncpy(prop.associated_tensor_names[i],
                    node.tensor_names[i],
                    sizeof(prop.associated_tensor_names[i]));
        }
        prop.associated_tensor_names_count = node.tensor_names_count;

        return prop;
    };

    for (size_t i = 0; i < metadata.in_tenosr_count; i++) {
        auto prop = convert(metadata.in_tensor_desc[i],
                            metadata.net_input[i],
                            metadata.ov_parameters[i],
                            ZE_GRAPH_ARGUMENT_TYPE_INPUT);
        props.push_back(prop);
    }

    for (size_t i = 0; i < metadata.out_tensor_count; i++) {
        auto prop = convert(metadata.out_tensor_desc[i],
                            metadata.net_output[i],
                            metadata.ov_results[i],
                            ZE_GRAPH_ARGUMENT_TYPE_OUTPUT);
        props.push_back(prop);
    }
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

void ElfParser::getArgumentMetadata(std::vector<ze_graph_argument_metadata_t> &args) const {
    auto metadata = hpi->getMetadata();

    args.reserve(metadata.net_input_count + metadata.net_output_count);

    auto convert = [](const elf::OVNode &node, ze_graph_argument_type_t type) {
        ze_graph_argument_metadata_t arg = {};
        arg.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_METADATA;
        arg.type = type;

        strncpy(arg.friendly_name,
                node.friendly_name,
                std::min(sizeof(arg.friendly_name), strlen(node.friendly_name)));

        for (const auto &itr : toMetadataType) {
            if (itr.first == node.type) {
                arg.data_type = itr.second;
                break;
            }
        }

        std::copy(node.shape, node.shape + node.shape_size, &arg.shape[0]);
        arg.shape_size = node.shape_size;

        for (unsigned i = 0; i < node.tensor_names_count; i++) {
            strncpy(arg.tensor_names[i], node.tensor_names[i], sizeof(arg.tensor_names[i]));
        }
        arg.tensor_names_count = node.tensor_names_count;
        strncpy(arg.input_name,
                node.input_name,
                std::min(sizeof(arg.input_name), strlen(node.input_name)));

        return arg;
    };

    for (size_t i = 0; i < metadata.net_input_count; i++) {
        if (i >= metadata.ov_parameters_count) {
            args.emplace_back();
            continue;
        }

        auto arg = convert(metadata.ov_parameters[i], ZE_GRAPH_ARGUMENT_TYPE_INPUT);
        args.push_back(arg);
    }

    for (size_t i = 0; i < metadata.net_output_count; i++) {
        if (i >= metadata.ov_results_count) {
            args.emplace_back();
            continue;
        }

        auto arg = convert(metadata.ov_results[i], ZE_GRAPH_ARGUMENT_TYPE_OUTPUT);
        args.push_back(arg);
    }
}

bool ElfParser::getProfilingSize(uint32_t &size) const {
    const auto &profBuffers = hpi->getProfBuffers();

    if (profBuffers.size() == 0) {
        size = 0;
    } else if (profBuffers.size() == 1) {
        size = boost::numeric_cast<uint32_t>(profBuffers[0].size());
    } else {
        LOG_E("Multiple profiling tensors are not supported");
        return false;
    }

    return true;
}

bool ElfParser::applyInputOutputs(elf::HostParsedInference &hostParsedInference,
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
        hostParsedInference.applyInputOutput(inputDeviceBuffers,
                                             outputDeviceBuffers,
                                             profilingDeviceBuffers);
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
    const std::pair<void *, uint32_t> &profilingPtr) {
    uint64_t inferenceId = 0;
    if (!ctx->getUniqueInferenceId(inferenceId))
        return nullptr;

    if (!firstInference) {
        auto newHpi = createHostParsedInference(bufferManager.get(), accessManager.get());
        if (newHpi == nullptr) {
            LOG_E("Not able to create new HostParsedInference");
            return nullptr;
        }

        otherHpis.push_back(std::move(hpi));
        hpi = std::move(newHpi);
    }

    std::vector<VPU::VPUBufferObject *> bos;
    if (!applyInputOutputs(*hpi, inputPtrs, outputPtrs, profilingPtr, bos)) {
        LOG_E("Failed to apply arguments to elf executor");
        return nullptr;
    }

    /*
     * All associated buffer objects needs to be added to command. Thanks to it kernel pin pages
     */
    for (const auto &buffer : hpi->getAllocatedBuffers()) {
        if (buffer.size() == 0)
            continue;

        VPU::VPUBufferObject *bo = ctx->findBuffer(buffer.cpu_addr());
        if (bo == nullptr) {
            LOG_E("Failed to find a buffer in tracked memory");
            return nullptr;
        }

        bos.push_back(bo);
    }

    auto hpiBuffer = hpi->getParsedInference();
    VPU::VPUBufferObject *bo = ctx->findBuffer(hpiBuffer.cpu_addr());
    if (bo == nullptr) {
        LOG_E("Failed to find a buffer in tracked memory");
        return nullptr;
    }
    bos.push_back(bo);

    firstInference = false;
    return VPU::VPUInferenceExecute::create(inferenceId,
                                            hpiBuffer.vpu_addr(),
                                            hpiBuffer.size(),
                                            bos);
}

ze_result_t ElfParser::parse(std::vector<ze_graph_argument_properties_3_t> &argumentProperties,
                             std::vector<ze_graph_argument_metadata_t> &argumentMetadata,
                             uint32_t &profilingOutputSize) {
    getArgumentProperties(argumentProperties);
    getArgumentMetadata(argumentMetadata);
    getProfilingSize(profilingOutputSize);

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
                                  const std::pair<void *, uint32_t> &profilingPtr) {
    return createInferenceExecuteCommand(inputArgs,
                                         outputArgs,
                                         {profilingPtr.first, profilingPtr.second});
}

} // namespace L0
