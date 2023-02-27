/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/ext/source/graph/elf_parser.hpp"
#include "level_zero/ze_graph_ext.h"

#include <stdexcept>
#include <umd_common.hpp>
#include <api/vpu_nnrt_api.h>

#include <algorithm>
#include <boost/numeric/conversion/cast.hpp>
#include <exception>
#include <string.h>
#include <vpux_elf/utils/error.hpp>
#include <vpux_elf/accessor.hpp>
#include <vpux_elf/reader.hpp>
#include <vpux_headers/metadata.hpp>
#include <vpux_loader/vpux_loader.hpp>
#include <vpux_sym_tab/3720/SymTabGen.h>

namespace elf {
SymbolEntry SymTabGen::symTab_[SymTabGen::N_TABS][SymTabGen::SPECIAL_SYMTAB_SIZE] = {};
} // namespace elf

namespace L0 {

class DriverBufferManager : public elf::BufferManager {
  public:
    DriverBufferManager(VPU::VPUDeviceContext *context)
        : ctx(context) {}
    ~DriverBufferManager() override = default;

    VPU::VPUBufferObject::Type getBufferType(elf::Elf_Xword flag) {
        if (flag & elf::SHF_EXECINSTR)
            return VPU::VPUBufferObject::Type::CachedLow;

        return VPU::VPUBufferObject::Type::CachedHigh;
    }

    elf::DeviceBuffer allocate(const elf::BufferSpecs &buffSpecs) override {
        LOG_I("Allocate: size: %#lx, alignment: %#lx, procFlags: %#lx",
              buffSpecs.size,
              buffSpecs.alignment,
              buffSpecs.procFlags);

        size_t size = buffSpecs.size;
        if (buffSpecs.size == 0) {
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
        return elf::DeviceBuffer(bo->getBasePointer(), bo->getVPUAddr(), buffSpecs.size);
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
                     std::unique_ptr<elf::BufferManager> manager,
                     std::unique_ptr<elf::VPUXLoader> loader)
    : ctx(ctx)
    , manager(std::move(manager))
    , loader(std::move(loader))
    , hostParsedInference(nullptr) {}

ElfParser::~ElfParser() {
    if (hostParsedInference)
        if (!ctx->freeMemAlloc(hostParsedInference))
            LOG_E("Failed to free hostParsedInference memory");
}

static elf::ResourceRequirements readResourcesFromElf(elf::ElfDDRAccessManager *elfAccess) {
    /* TODO: Temporary solution */
    // The loader must be initialized with a pre-generated symtab.
    // To generate a symtab for a configuration (one cluster/two clusters), the resource
    // requirements shoud be read before the loader starts to apply relocations.
    elf::Reader<elf::ELF_Bitness::Elf64> notReader(elfAccess);

    auto nSections = notReader.getSectionsNum();

    for (size_t i = 0; i < nSections; i++) {
        const auto &section = notReader.getSection(i);

        const auto sectionHeader = section.getHeader();
        auto sectionType = sectionHeader->sh_type;

        if (sectionType == elf::VPU_SHT_NETDESC) {
            LOG_I("Found section with resource, name: %s, type: %u",
                  section.getName(),
                  sectionType);
            return *(section.getData<elf::ResourceRequirements>());
        }
    }

    throw std::runtime_error("Failed to find a resource");
}

std::optional<ElfParser>
ElfParser::getElfParser(VPU::VPUDeviceContext *ctx, uint8_t *ptr, size_t size) {
    elf::SymTabGen::initSymTab();
    auto bufferManager = std::make_unique<DriverBufferManager>(ctx);
    auto elfAccess = std::make_unique<elf::ElfDDRAccessManager>(ptr, size);

    try {
        auto resReq = readResourcesFromElf(elfAccess.get());
        auto loader =
            std::make_unique<elf::VPUXLoader>(elfAccess.get(),
                                              bufferManager.get(),
                                              elf::SymTabGen::symTab(resReq.nn_slice_count_));
        return ElfParser(ctx, std::move(bufferManager), std::move(loader));
    } catch (const elf::RuntimeError &err) {
        LOG_E("Failed to create elf::VPUXLoader, type: elf::RuntimeError, reason: %s", err.what());
    } catch (const elf::LogicError &err) {
        LOG_E("Failed to create elf::VPUXLoader, type: elf::LogicError, reason: %s", err.what());
    }
    return std::nullopt;
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

void ElfParser::getArgumentProperties(std::vector<ze_graph_argument_properties_t> &props) const {
    auto metadata = loader->getNetworkMetadata();

    props.reserve(metadata.in_tenosr_count + metadata.out_tensor_count);

    auto convert = [](const elf::TensorRef &devTensor,
                      const elf::TensorRef &netTensor,
                      ze_graph_argument_type_t argType) {
        ze_graph_argument_properties_t prop = {};

        strncpy(prop.name, netTensor.name, std::min(sizeof(prop.name), strlen(netTensor.name)));
        prop.type = argType;
        for (size_t j = 0; j < ZE_MAX_GRAPH_ARGUMENT_DIMENSIONS_SIZE; j++) {
            if (j < devTensor.dimensions_size) {
                prop.dims[j] = devTensor.dimensions[j];
            } else {
                prop.dims[j] = 1;
            }
        }
        prop.networkPrecision = getTensorPrecision(netTensor.data_type);
        prop.networkLayout = getNetworkLayout(netTensor);
        prop.devicePrecision = getTensorPrecision(devTensor.data_type);
        prop.deviceLayout = getDeviceLayout(devTensor);

        return prop;
    };

    for (size_t i = 0; i < metadata.in_tenosr_count; i++) {
        auto prop = convert(metadata.in_tensor_desc[i],
                            metadata.net_input[i],
                            ZE_GRAPH_ARGUMENT_TYPE_INPUT);
        props.push_back(prop);
    }

    for (size_t i = 0; i < metadata.out_tensor_count; i++) {
        auto prop = convert(metadata.out_tensor_desc[i],
                            metadata.net_output[i],
                            ZE_GRAPH_ARGUMENT_TYPE_OUTPUT);
        props.push_back(prop);
    }
}

bool ElfParser::applyInputOutputs(
    const std::vector<std::pair<const void *, uint32_t>> &inputPtrs,
    const std::vector<std::pair<const void *, uint32_t>> &outputPtrs) {
    auto getDeviceBuffers = [this](const std::vector<std::pair<const void *, uint32_t>> &ptrs,
                                   std::vector<elf::DeviceBuffer> &buffers) {
        buffers.reserve(ptrs.size());

        for (const auto &[ptr, size] : ptrs) {
            auto *bo = ctx->findBuffer(ptr);
            if (bo == nullptr) {
                LOG_E("Failed to find a user buffer");
                return false;
            }

            userBuffers.push_back(bo);

            uint64_t vpuAddr = ctx->getBufferVPUAddress(ptr);
            uint8_t *basePtr = static_cast<uint8_t *>(const_cast<void *>(ptr));
            buffers.emplace_back(basePtr, vpuAddr, size);
        }

        return true;
    };

    std::vector<elf::DeviceBuffer> inputs;
    if (!getDeviceBuffers(inputPtrs, inputs))
        return false;

    std::vector<elf::DeviceBuffer> outputs;
    if (!getDeviceBuffers(outputPtrs, outputs))
        return false;

    loader->applyJitRelocations(inputs, outputs);

    return true;
}

void ElfParser::addArtificalBarrierConfig() {
    /* TODO: Termporary WA to allow elf execution when barrierConfigs is not defined*/
    auto vpuAddr = loader->getEntry();

    for (const auto &buf : loader->getAllocatedBuffers()) {
        if (buf.vpu_addr() == vpuAddr) {
            uint8_t *ptr = const_cast<uint8_t *>(buf.cpu_addr());
            auto *mappedInference = reinterpret_cast<nn_public::VpuMappedInference *>(ptr);
            if (mappedInference->barrier_configs.address == 0) {
                LOG_E("Applying a WA for barrierConfigs_ address!!");
                mappedInference->barrier_configs.address = 0xC000'0000;
                return;
            }
        }
    }
}

static void printMappedInference(nn_public::VpuMappedInference *mi) {
    LOG_I("MappedInference->vpu_nnrt_api_ver: %d", mi->vpu_nnrt_api_ver);
    for (uint32_t dma_engine = 0; dma_engine < nn_public::VPU_MAX_DMA_ENGINES; dma_engine++) {
        LOG_I("MappedInference->dma_tasks[%u].count: %lu",
              dma_engine,
              mi->dma_tasks[dma_engine].count);

        /* TODO: Convert VPU address to cpu address
        for (uint32_t i = 0; i < mi->dma_tasks[dma_engine].count; i++) {
            const auto &task = mi->dma_tasks[dma_engine].at(i);
            LOG_I("MappedInference->dma_tasks[%i]->transaction_.dst: %#lx",
                  i,
                  task.transaction_.dst);
            LOG_I("MappedInference->dma_tasks[%i]->transaction_.src: %#lx",
                  i,
                  task.transaction_.src);
        }
        */
    }
    LOG_I("MappedInference->invariants.count: %lu", mi->invariants.count);
    LOG_I("MappedInference->variants.count: %lu", mi->variants.count);
    LOG_I("MappedInference->barrier_configs.count: %lu", mi->barrier_configs.count);
    LOG_I("MappedInference->act_kernel_ranges.count: %lu", mi->act_kernel_ranges.count);
    LOG_I("MappedInference->act_kernel_invocations.count: %lu", mi->act_kernel_invocations.count);

    for (uint32_t desc = 0; desc < nn_public::VPU_NUM_METADATA_FEEDERS; desc++) {
        LOG_I("MappedInference->feeder_descriptors[%u].descriptor_.dst: %#lx",
              desc,
              mi->feeder_descriptors[desc].descriptor_.dst);
        LOG_I("MappedInference->feeder_descriptors[%u].descriptor_.src: %#lx",
              desc,
              mi->feeder_descriptors[desc].descriptor_.src);
    }

    for (uint32_t dma_engine = 0; dma_engine < nn_public::VPU_MAX_DMA_ENGINES; dma_engine++) {
        LOG_I("MappedInference->leading_dma_tasks[%u]: %u",
              dma_engine,
              mi->leading_dma_tasks[dma_engine]);
    }

    LOG_I("MappedInference->shv_rt_configs.runtime_entry: %#x", mi->shv_rt_configs.runtime_entry);
    LOG_I("MappedInference->shv_rt_configs.act_rt_window_base: %#x",
          mi->shv_rt_configs.act_rt_window_base);
    for (uint32_t i = 0; i < nn_public::VPU_AS_TOTAL; i++) {
        LOG_I("MappedInference->shv_rt_configs.stack_frames[%u]: %#x",
              i,
              mi->shv_rt_configs.stack_frames[i]);
    }
    LOG_I("MappedInference->shv_rt_configs.stack_size: %#x", mi->shv_rt_configs.stack_size);
    LOG_I("MappedInference->shv_rt_configs.code_window_buffer_size: %#x",
          mi->shv_rt_configs.code_window_buffer_size);
    LOG_I("MappedInference->shv_rt_configs.perf_metrics_mask: %#x",
          mi->shv_rt_configs.perf_metrics_mask);
    LOG_I("MappedInference->shv_rt_configs.runtime_version: %#x",
          mi->shv_rt_configs.runtime_version);
    LOG_I("MappedInference->shv_rt_configs.use_schedule_embedded_rt: %#x",
          mi->shv_rt_configs.use_schedule_embedded_rt);
    LOG_I("MappedInference->shv_rt_configs.dpu_perf_mode: %#hhx",
          static_cast<uint8_t>(mi->shv_rt_configs.dpu_perf_mode));
}

std::shared_ptr<VPU::VPUInferenceExecute> ElfParser::getCommand(uint64_t inferenceId) {
    /*
     * All associated buffer objects needs to be added to command. Thanks to it kernel pin pages
     */
    std::vector<VPU::VPUBufferObject *> bos;
    for (const auto &buffer : loader->getAllocatedBuffers()) {
        if (buffer.size() == 0)
            continue;

        VPU::VPUBufferObject *bo = ctx->findBuffer(buffer.cpu_addr());
        if (bo == nullptr) {
            LOG_E("Failed to find a buffer in tracked memory");
            return nullptr;
        }

        bos.push_back(bo);
    }

    addArtificalBarrierConfig();

    hostParsedInference = ctx->createInternalBufferObject(
        getFwDataCacheAlign(sizeof(nn_public::VpuHostParsedInference)),
        VPU::VPUBufferObject::Type::CachedLow);
    if (hostParsedInference == nullptr) {
        LOG_E("Failed to allocate Host Parsed Inference buffer");
        return nullptr;
    }

    bos.push_back(hostParsedInference);

    for (auto *bo : userBuffers)
        bos.push_back(bo);

    auto *hpi = reinterpret_cast<nn_public::VpuHostParsedInference *>(
        hostParsedInference->getBasePointer());

    auto resource = loader->getResourceRequirements();
    hpi->resource_requirements_ = {};
    /* TODO: Below three fields should not be passed */
    // hpi->resource_requirements_.nn_slice_length_ = resource.nn_slice_length_;
    // hpi->resource_requirements_.ddr_scratch_length_ = resource.ddr_scratch_length_;
    // hpi->resource_requirements_.nn_barrier_count_ = resource.nn_barrier_count_;
    hpi->resource_requirements_.nn_slice_count_ = resource.nn_slice_count_;
    hpi->resource_requirements_.nn_barriers_ = resource.nn_barriers_;

    hpi->performance_metrics_ = {};

    hpi->mapped_.address = loader->getEntry();
    hpi->mapped_.count = 1;

    LOG_I("HostParsedInference->resource_requirements_\n"
          "\tnn_slice_length_: %d\n"
          "\tddr_scratch_length_: %d\n"
          "\tnn_barrier_count_: %d\n"
          "\tnn_slice_count_: %d\n"
          "\tnn_barriers_: %d\n",
          hpi->resource_requirements_.nn_slice_length_,
          hpi->resource_requirements_.ddr_scratch_length_,
          hpi->resource_requirements_.nn_barrier_count_,
          hpi->resource_requirements_.nn_slice_count_,
          hpi->resource_requirements_.nn_barriers_);
    LOG_I("HostParsedInference->mapped_\n"
          "\taddress = %#lx\n"
          "\tcount = %lu\n",
          hpi->mapped_.address,
          hpi->mapped_.count);

    for (const auto &bo : bos) {
        if (bo->getVPUAddr() == hpi->mapped_.address) {
            printMappedInference(
                reinterpret_cast<nn_public::VpuMappedInference *>(bo->getBasePointer()));
        }
    }

    return VPU::VPUInferenceExecute::create(inferenceId,
                                            hostParsedInference->getVPUAddr(),
                                            hostParsedInference->getAllocSize(),
                                            bos);
}

} // namespace L0
