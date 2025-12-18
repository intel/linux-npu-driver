/*
 * Copyright (C) 2024-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"

#include <memory>
#include <variant>

namespace L0 {

class BlobContainer final {
  public:
    BlobContainer(uint8_t *ptr, size_t size)
        : ptr(ptr)
        , size(size) {}

    BlobContainer(uint8_t *ptr, size_t size, std::unique_ptr<VPU::OsFile> file)
        : ptr(ptr)
        , size(size)
        , backingStore(std::move(file)) {}

    BlobContainer(std::unique_ptr<uint8_t[]> buffer, size_t size)
        : ptr(buffer.get())
        , size(size)
        , backingStore(std::move(buffer)) {}

    BlobContainer(uint8_t *ptr, size_t size, std::shared_ptr<VPU::VPUBufferObject> bo)
        : ptr(ptr)
        , size(size)
        , backingStore()
        , bo(std::move(bo)) {}

    std::shared_ptr<VPU::VPUBufferObject> getNpuBuffer() const { return bo; }
    void setNpuBuffer(std::shared_ptr<VPU::VPUBufferObject> newBo) { bo = std::move(newBo); }
    // In case no backing store, the user can free the ptr after Graph initialization
    bool hasBackingStore() const {
        return !std::holds_alternative<std::monostate>(backingStore) || bo != nullptr;
    }

  public:
    uint8_t *ptr;
    size_t size;

  private:
    std::variant<std::monostate, std::unique_ptr<VPU::OsFile>, std::unique_ptr<uint8_t[]>>
        backingStore;
    std::shared_ptr<VPU::VPUBufferObject> bo;
};
} // namespace L0
