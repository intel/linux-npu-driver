/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include "perfetto.h"

#include "elf_parser.hpp"

#include <cstdint>

#include "blob_container.hpp"
#include "level_zero/ze_api.h"
#include "level_zero/ze_graph_ext.h"
#include "level_zero_driver/include/l0_exception.hpp"
#include "profiling_data.hpp"
#include "umd_common.hpp"
#include "vpu_driver/source/command/inference_execute.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_37xx/vpu_hw_37xx.hpp"
#include "vpu_driver/source/device/vpu_40xx/vpu_hw_40xx.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/trace_perfetto.hpp" // IWYU pragma: keep

#include <algorithm>
#include <array>
#include <bitset>
#include <exception>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <string.h>
#include <vpux_elf/accessor.hpp>
#include <vpux_elf/types/data_types.hpp>
#include <vpux_elf/types/section_header.hpp>
#include <vpux_elf/types/vpu_extensions.hpp>
#include <vpux_elf/utils/error.hpp>
#include <vpux_elf/utils/utils.hpp>
#include <vpux_headers/buffer_manager.hpp>
#include <vpux_headers/buffer_specs.hpp>
#include <vpux_headers/device_buffer.hpp>
#include <vpux_headers/managed_buffer.hpp>
#include <vpux_headers/metadata.hpp>
#include <vpux_headers/metadata_primitives.hpp>
#include <vpux_headers/platform.hpp>
#include <vpux_hpi.hpp>

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
        TRACE_EVENT("NPU_ELF", "elf::BufferManager::allocate");
        LOG(GRAPH,
            "Allocate: size: %#lx, alignment: %#lx, procFlags: %#lx",
            buffSpecs.size,
            buffSpecs.alignment,
            buffSpecs.procFlags);

        auto range = getBufferType(buffSpecs.procFlags);
        // If zero-sized buffer is requested, return only NPU address from required range. Use
        // unmappable allocation to quickly deallocate buffer at function exit
        if (buffSpecs.size == 0) {
            constexpr size_t allocSize = 1;
            auto bo =
                ctx->createUntrackedBufferObject(allocSize,
                                                 VPU::VPUBufferObject::convertToUnmappable(range));
            VPUX_ELF_THROW_WHEN(bo == nullptr, elf::AllocError, "Failed to get zero-sized buffer");

            LOG(GRAPH, "Zero-sized buffer, returning only vpu address: %#lx", bo->getVPUAddr());
            return elf::DeviceBuffer(nullptr, bo->getVPUAddr(), buffSpecs.size);
        }

        // If buffer is sharable, return empty DeviceBuffer. The buffer will be added on submission
        if (buffSpecs.isSharable() && range == VPU::VPUBufferObject::Type::WriteCombineDma) {
            LOG(GRAPH, "Shared scratch buffer size: %lu", buffSpecs.size);
            sharedScratchSize = buffSpecs.size;
            ctx->scratchCachePreload(buffSpecs.size);
            return elf::DeviceBuffer();
        }

        auto bo = ctx->createUntrackedBufferObject(buffSpecs.size, range);
        VPUX_ELF_THROW_WHEN(bo == nullptr, elf::AllocError, "Failed to allocate device buffer");

        LOG(GRAPH,
            "Allocated: cpu_addr: %p, vpu_addr: %#lx, size: %#lx",
            bo->getBasePointer(),
            bo->getVPUAddr(),
            bo->getAllocSize());

        void *ptr = bo->getBasePointer();
        const std::lock_guard<std::mutex> lock(mtx);
        auto [it, success] = tracedElfParserBuffers.emplace(ptr, std::move(bo));
        VPUX_ELF_THROW_WHEN(!success, elf::AllocError, "Failed to trace new device buffer");

        return elf::DeviceBuffer(it->second->getBasePointer(),
                                 it->second->getVPUAddr(),
                                 buffSpecs.size);
    }

    void deallocate(elf::DeviceBuffer &devAddress) override {
        TRACE_EVENT("NPU_ELF", "elf::BufferManager::deallocate");
        LOG(GRAPH,
            "Deallocate: cpu: %p, vpu: %#lx, size: %lu",
            devAddress.cpu_addr(),
            devAddress.vpu_addr(),
            devAddress.size());

        if (devAddress.cpu_addr() == nullptr) {
            return;
        }

        const std::lock_guard<std::mutex> lock(mtx);
        auto it = tracedElfParserBuffers.find(devAddress.cpu_addr());
        if (it == tracedElfParserBuffers.end()) {
            LOG_E("Could not find a buffer to deallocate: cpu: %p, vpu: %lx, size: %lu",
                  devAddress.cpu_addr(),
                  devAddress.vpu_addr(),
                  devAddress.size());
            return;
        }

        tracedElfParserBuffers.erase(it);
    }

    void lock(elf::DeviceBuffer &devAddress) override {
        TRACE_EVENT("NPU_ELF", "elf::BufferManager::lock");
    }
    void unlock(elf::DeviceBuffer &devAddress) override {
        TRACE_EVENT("NPU_ELF", "elf::BufferManager::unlock");
    }

    size_t copy(elf::DeviceBuffer &to, const uint8_t *from, size_t count) override {
        TRACE_EVENT("NPU_ELF", "elf::BufferManager::copy");
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

        auto bo = findBuffer(to.cpu_addr());
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

    std::shared_ptr<VPU::VPUBufferObject> findBuffer(const void *ptr) {
        const std::lock_guard<std::mutex> lock(mtx);
        auto it = tracedElfParserBuffers.lower_bound(ptr);
        if (it == tracedElfParserBuffers.end()) {
            return nullptr;
        }
        return it->second;
    }

    size_t sharedScratchSize = 0;

  private:
    mutable std::mutex mtx;
    VPU::VPUDeviceContext *ctx;
    std::map<const void *, std::shared_ptr<VPU::VPUBufferObject>, std::greater<const void *>>
        tracedElfParserBuffers;
};

class DriverAccessManager : public elf::AccessManager {
  public:
    DriverAccessManager(uint8_t *ptr,
                        size_t size,
                        DriverBufferManager *manager,
                        bool isInputPersist)
        : AccessManager(size)
        , blob(ptr)
        , bufferManager(manager)
        , useStaticBuffer(isInputPersist) {}

    DriverAccessManager(const DriverAccessManager &) = delete;
    DriverAccessManager(DriverAccessManager &&) = delete;
    DriverAccessManager &operator=(const DriverAccessManager &) = delete;
    DriverAccessManager &operator=(DriverAccessManager &&) = delete;
    ~DriverAccessManager() override = default;

    std::unique_ptr<elf::ManagedBuffer> readInternal(size_t offset,
                                                     const elf::BufferSpecs &specs) override {
        VPUX_ELF_THROW_WHEN(offset + specs.size > getSize(),
                            elf::AccessError,
                            "Read request out of bounds");

        uint8_t *start = blob + offset;

        if (hasNPUAccess(specs.procFlags)) {
            // AllocatedDeviceBuffer is a wrapper for a buffer allocated by DriverBufferManager
            auto buffer = std::make_unique<elf::AllocatedDeviceBuffer>(bufferManager, specs);
            elf::DeviceBuffer devBuffer = buffer->getBuffer();
            bufferManager->copy(devBuffer, start, devBuffer.size());
            return buffer;
        }

        if (useStaticBuffer) {
            // StaticBuffer is a wrapper for a raw pointer
            auto statBuffer = std::make_unique<elf::StaticBuffer>(start, specs);
            return statBuffer;
        }

        // DynamicBuffer is a wrapper for a std::vector
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
        return (flags & (elf::SHF_EXECINSTR | elf::VPU_SHF_PROC_DMA | elf::VPU_SHF_PROC_SHAVE |
                         elf::SHF_ALLOC)) != 0;
    }

    uint8_t *blob = nullptr;
    DriverBufferManager *bufferManager = nullptr;
    bool useStaticBuffer;
};

ElfParser::ElfParser(VPU::VPUDeviceContext *ctx,
                     std::unique_ptr<elf::BufferManager> buffer,
                     std::unique_ptr<elf::AccessManager> access,
                     std::shared_ptr<elf::HostParsedInference> hpi)
    : ctx(ctx)
    , bufferManager(std::move(buffer))
    , accessManager(std::move(access))
    , hpiManager(std::make_unique<HostParsedInferenceManager>(std::move(hpi))) {}

ElfParser::~ElfParser() {
    ctx->scratchCachePrune(getSharedScratchSize());
}

bool ElfParser::checkMagic(const std::unique_ptr<BlobContainer> &blob) {
    if (blob->size == 0)
        return false;

    return elf::utils::checkELFMagic(blob->ptr);
}

static inline elf::platform::ArchKind toArchKind(uint32_t deviceId) {
    switch (deviceId) {
    case PCI_DEVICE_ID_MTL:
    case PCI_DEVICE_ID_ARL:
        return elf::platform::ArchKind::VPUX37XX;
    case PCI_DEVICE_ID_LNL:
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
    config.archKind = toArchKind(ctx->getPciDevId());

    elf::DeviceDescriptor devDesc = {};

    devDesc.size = sizeof(elf::DeviceDescriptor);
    devDesc.deviceID = ctx->getDeviceCapabilities().deviceId;
    devDesc.revision = ctx->getDeviceCapabilities().deviceRevision;
    devDesc.tileCount =
        static_cast<uint32_t>(std::bitset<32>(ctx->getDeviceCapabilities().tileConfig).count());

    try {
        TRACE_EVENT("NPU_ELF", "elf::HostParsedInference::ctor");
        return std::make_shared<elf::HostParsedInference>(buffer, access, config, &devDesc);
    } catch (const elf::AllocError &err) {
        LOG_E("Failed to create elf::HostParsedInference, type: elf::AllocError, reason: %s",
              err.what());
        errorMsg += std::string(err.what()) + "\n";
        throw DriverError(ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY);
    } catch (elf::CompatibilityError &err) {
        LOG_E(
            "Failed to create elf::HostParsedInference, type: elf::CompatibilityError, reason: %s",
            err.what());
        errorMsg += std::string(err.what()) + "\n";
        throw DriverError(ZE_RESULT_ERROR_UNSUPPORTED_VERSION);
    } catch (const elf::RuntimeError &err) {
        LOG_E("Failed to create elf::HostParsedInference, type: elf::RuntimeError, reason: %s",
              err.what());
        errorMsg += std::string(err.what()) + "\n";
    } catch (const elf::LogicError &err) {
        LOG_E("Failed to create elf::HostParsedInference, type: elf::LogicError, reason: %s",
              err.what());
        errorMsg += std::string(err.what()) + "\n";
    } catch (const std::exception &err) {
        LOG_E("Failed to create elf::HostParsedInference, type: std::exception, reason: %s",
              err.what());
        errorMsg += std::string(err.what()) + "\n";
    }
    return nullptr;
}

static std::shared_ptr<elf::HostParsedInference>
copyHostParsedInference(std::shared_ptr<elf::HostParsedInference> &hpi) {
    try {
        TRACE_EVENT("NPU_ELF", "elf::HostParsedInference::copy");
        return std::make_shared<elf::HostParsedInference>(*hpi);
    } catch (const elf::AllocError &err) {
        LOG_E("Failed to copy elf::HostParsedInference, type: elf::AllocError, reason: %s",
              err.what());
        throw DriverError(ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY);
    } catch (elf::CompatibilityError &err) {
        LOG_E("Failed to copy elf::HostParsedInference, type: elf::CompatibilityError, reason: %s",
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

static void loadHostParsedInference(const std::shared_ptr<elf::HostParsedInference> &hpi) {
    try {
        TRACE_EVENT("NPU_ELF", "elf::HostParsedInference::load");
        hpi->load();
        return;
    } catch (const elf::AllocError &err) {
        LOG_E("Failed to load elf::HostParsedInference, type: elf::AllocError, reason: %s",
              err.what());
        throw DriverError(ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY);
    } catch (elf::CompatibilityError &err) {
        LOG_E("Failed to load elf::HostParsedInference, type: elf::CompatibilityError, reason: %s",
              err.what());
        throw DriverError(ZE_RESULT_ERROR_UNSUPPORTED_VERSION);
    } catch (const elf::RuntimeError &err) {
        LOG_E("Failed to load elf::HostParsedInference, type: elf::RuntimeError, reason: %s",
              err.what());
    } catch (const elf::LogicError &err) {
        LOG_E("Failed to load elf::HostParsedInference, type: elf::LogicError, reason: %s",
              err.what());
    } catch (const std::exception &err) {
        LOG_E("Failed to load elf::HostParsedInference, type: std::exception, reason: %s",
              err.what());
    }
    throw DriverError(ZE_RESULT_ERROR_UNKNOWN);
}

std::shared_ptr<elf::HostParsedInference> HostParsedInferenceManager::acquire() {
    std::lock_guard<std::mutex> lock(mtx);
    if (!loaded) {
        loadHostParsedInference(headHpi);
        loaded = true;
    }

    if (headHpi.use_count() == 1)
        return headHpi;

    for (auto &hpi : hpis) {
        if (hpi.use_count() == 1) {
            return hpi;
        }
    }

    auto hpi = copyHostParsedInference(headHpi);
    if (hpi != nullptr) {
        hpis.push_back(hpi);
    }

    return hpi;
}

std::unique_ptr<ElfParser> ElfParser::getElfParser(VPU::VPUDeviceContext *ctx,
                                                   const std::unique_ptr<BlobContainer> &blob,
                                                   std::string &logBuffer,
                                                   bool isInputPersistent) {
    auto bufferManager = std::make_unique<DriverBufferManager>(ctx);
    auto accessManager = std::make_unique<DriverAccessManager>(blob->ptr,
                                                               blob->size,
                                                               bufferManager.get(),
                                                               isInputPersistent);
    auto hpi = createHostParsedInference(bufferManager.get(), accessManager.get(), ctx, logBuffer);
    if (hpi != nullptr)
        return std::make_unique<ElfParser>(ctx,
                                           std::move(bufferManager),
                                           std::move(accessManager),
                                           std::move(hpi));

    return nullptr;
}

elf::VersionsProvider ElfParser::getElfVer(uint32_t deviceId) {
    return elf::VersionsProvider(toArchKind(deviceId));
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
    case elf::DType::DType_F8E4M3FN:
        return ZE_GRAPH_ARGUMENT_PRECISION_FP8_E4M3;
    case elf::DType::DType_F8E5M2:
        return ZE_GRAPH_ARGUMENT_PRECISION_FP8_E5M2;
    case elf::DType::DType_F8E8M0:
        return ZE_GRAPH_ARGUMENT_PRECISION_FP8_E8M0;
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
    case elf::DType::DType_I2X:
        return ZE_GRAPH_ARGUMENT_PRECISION_UINT2;
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
    case elf::DType::DType_I4X:
        return ZE_GRAPH_ARGUMENT_PRECISION_NF4;
    case elf::DType::DType_I2:
    case elf::DType::DType_LOG:
        return ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN;
    }
    return ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN;
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

    uint32_t tensorNamesCount = node.tensor_names_count;
    if (tensorNamesCount > ZE_MAX_GRAPH_TENSOR_NAMES_SIZE) {
        LOG_W("Tensor names count exceeds the Graph Extension limits (%u > %u)",
              node.tensor_names_count,
              ZE_MAX_GRAPH_TENSOR_NAMES_SIZE);
        tensorNamesCount = ZE_MAX_GRAPH_TENSOR_NAMES_SIZE;
    }

    for (uint32_t i = 0; i < tensorNamesCount; i++) {
        memcpy(prop.associated_tensor_names[i],
               node.tensor_names[i],
               sizeof(prop.associated_tensor_names[i]) - 1);
    }
    prop.associated_tensor_names_count = tensorNamesCount;
}

bool ElfParser::getArgumentProperties(std::vector<ze_graph_argument_properties_3_t> &props) const {
    TRACE_EVENT_BEGIN("NPU_ELF", "elf::HostParsedInference::getMetadata");
    auto metadata = hpiManager->head()->getMetadata();
    TRACE_EVENT_END("NPU_ELF");
    props.reserve(metadata->mInTensorDescriptors.size() + metadata->mOutTensorDescriptors.size());

    for (size_t i = 0; i < metadata->mInTensorDescriptors.size(); i++) {
        ze_graph_argument_properties_3_t prop = {};
        prop.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES;
        prop.type = ZE_GRAPH_ARGUMENT_TYPE_INPUT;

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
    TRACE_EVENT_BEGIN("NPU_ELF", "elf::HostParsedInference::getMetadata");
    auto metadata = hpiManager->head()->getMetadata();
    TRACE_EVENT_END("NPU_ELF");
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
    TRACE_EVENT("NPU_ELF", "elf::HostParsedInference::getProfBuffers");
    const auto &profBuffers = hpiManager->head()->getProfBuffers();

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

std::shared_ptr<VPU::VPUBufferObject> ElfParser::findBuffer(const void *ptr) {
    if (bufferManager == nullptr)
        return nullptr;
    return dynamic_cast<DriverBufferManager &>(*bufferManager).findBuffer(ptr);
}

size_t ElfParser::getSharedScratchSize() const {
    if (bufferManager == nullptr)
        return 0;
    return dynamic_cast<DriverBufferManager &>(*bufferManager).sharedScratchSize;
}

void ElfParser::updateSharedScratchBuffers(std::shared_ptr<elf::HostParsedInference> &cmdHpi,
                                           std::shared_ptr<VPU::VPUBufferObject> &bo) {
    std::vector<elf::DeviceBuffer> buffers{
        elf::DeviceBuffer(bo->getBasePointer(), bo->getVPUAddr(), bo->getAllocSize())};

    LOG(GRAPH,
        "HostParsedInference[%p]->updateSharedScratchBuffers: cpu: %p, vpu: %#lx, size: %lu",
        cmdHpi.get(),
        buffers.at(0).cpu_addr(),
        buffers.at(0).vpu_addr(),
        buffers.at(0).size());

    TRACE_EVENT("NPU_ELF", "elf::HostParsedInference::updateSharedScratchBuffers");
    cmdHpi->updateSharedScratchBuffers(buffers);
}

bool ElfParser::applyInputOutputs(std::shared_ptr<elf::HostParsedInference> &cmdHpi,
                                  const std::vector<const void *> &inputPtrs,
                                  const std::vector<const void *> &outputPtrs,
                                  GraphProfilingQuery *profilingQuery,
                                  std::vector<std::shared_ptr<VPU::VPUBufferObject>> &bos) {
    auto getDeviceBuffers = [this, &bos](const std::vector<const void *> &ptrs,
                                         std::vector<elf::DeviceBuffer> &buffers) {
        buffers.reserve(ptrs.size());

        if (ptrs.size() != buffers.size()) {
            return false;
        }

        for (size_t i = 0; i < ptrs.size(); i++) {
            auto bo = ctx->findBufferObject(ptrs[i]);
            if (bo == nullptr) {
                LOG_E("Failed to find a user buffer");
                return false;
            }

            uint64_t offset = reinterpret_cast<uint64_t>(ptrs[i]) -
                              reinterpret_cast<uint64_t>(bo->getBasePointer());

            if (bo->getAllocSize() - offset < buffers[i].size()) {
                LOG_E("Graph argument at position: %zu with size: %lu exceedes expected size: %lu",
                      i,
                      bo->getAllocSize() - offset,
                      buffers[i].size());
                return false;
            }

            bos.push_back(bo);

            uint64_t vpuAddr = bo->getVPUAddr(ptrs[i]);
            uint8_t *basePtr = static_cast<uint8_t *>(const_cast<void *>(ptrs[i]));
            buffers[i] = elf::DeviceBuffer(basePtr, vpuAddr, 0);
        }

        return true;
    };

    std::vector<elf::DeviceBuffer> inputDeviceBuffers = cmdHpi->getInputBuffers();
    if (!getDeviceBuffers(inputPtrs, inputDeviceBuffers))
        return false;

    std::vector<elf::DeviceBuffer> outputDeviceBuffers = cmdHpi->getOutputBuffers();
    if (!getDeviceBuffers(outputPtrs, outputDeviceBuffers))
        return false;

    std::vector<elf::DeviceBuffer> profilingDeviceBuffers;
    if (profilingQuery) {
        auto profilingBo = profilingQuery->getBo();
        auto profilingMemPtr = profilingQuery->getQueryPtr();

        if (!profilingMemPtr || !profilingBo)
            return false;

        bos.push_back(profilingBo);
        profilingDeviceBuffers.emplace_back(profilingMemPtr,
                                            profilingBo->getVPUAddr(profilingMemPtr),
                                            profilingQuery->getSize());
    }

    try {
        TRACE_EVENT("NPU_ELF", "elf::HostParsedInference::applyInputOutput");
        LOG(GRAPH,
            "HostParsedInference[%p]->applyInputOutput: input size: %lu, output size: %lu, "
            "profiling size: %lu",
            cmdHpi.get(),
            inputDeviceBuffers.size(),
            outputDeviceBuffers.size(),
            profilingDeviceBuffers.size());
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

std::shared_ptr<VPU::VPUInferenceExecute>
ElfParser::createInferenceExecuteCommand(const std::vector<const void *> &inputPtrs,
                                         const std::vector<const void *> &outputPtrs,
                                         GraphProfilingQuery *profilingQuery) {
    uint64_t inferenceId = 0;
    if (!ctx->getUniqueInferenceId(inferenceId))
        return nullptr;

    std::shared_ptr<elf::HostParsedInference> cmdHpi = hpiManager->acquire();
    std::vector<std::shared_ptr<VPU::VPUBufferObject>> bos;
    auto hpiBuffer = cmdHpi->getParsedInference();
    auto bo = findBuffer(hpiBuffer.cpu_addr());
    if (bo == nullptr) {
        LOG_E("Failed to find a buffer in tracked memory");
        return nullptr;
    }
    bos.push_back(std::move(bo));

    for (const auto &buffer : cmdHpi->getAllocatedBuffers()) {
        // Skip not allocated buffers
        if (buffer.size() == 0) {
            continue;
        }

        // SharedScratchBuffer does not set the cpu address
        if (buffer.cpu_addr() == nullptr && buffer.size() == getSharedScratchSize()) {
            continue;
        }

        auto bo = findBuffer(buffer.cpu_addr());
        if (bo == nullptr) {
            LOG_E("Failed to find a buffer in tracked memory");
            return nullptr;
        }

        bos.push_back(std::move(bo));
    }

    auto cmd = VPU::VPUInferenceExecute::create(shared_from_this(),
                                                cmdHpi,
                                                inputPtrs,
                                                outputPtrs,
                                                profilingQuery,
                                                inferenceId,
                                                bos);
    if (cmd == nullptr)
        return nullptr;

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

ze_result_t ElfParser::initialize() {
    try {
        hpiManager->acquire();
        return ZE_RESULT_SUCCESS;
    } catch (const DriverError &e) {
        return e.result();
    }
    return ZE_RESULT_ERROR_UNKNOWN;
}

std::shared_ptr<VPU::VPUBufferObject> ElfParser::allocateInternal(size_t size) {
    if (!size)
        return nullptr;
    elf::BufferSpecs spec = {};
    spec.size = size;
    elf::DeviceBuffer buffer = bufferManager->allocate(spec);
    return findBuffer(buffer.cpu_addr());
}

std::shared_ptr<VPU::VPUCommand> ElfParser::allocateInitCommand(VPU::VPUDeviceContext *ctx) {
    ze_result_t result = initialize();
    if (result != ZE_RESULT_SUCCESS)
        return nullptr;
    /* No initialize command for elf, return empty command that will be ignored */
    return std::make_shared<VPU::VPUCommand>();
}

std::shared_ptr<VPU::VPUCommand>
ElfParser::allocateExecuteCommand(const std::vector<const void *> &inputArgs,
                                  const std::vector<const void *> &outputArgs,
                                  GraphProfilingQuery *profilingQuery) {
    return createInferenceExecuteCommand(inputArgs, outputArgs, profilingQuery);
}

} // namespace L0
