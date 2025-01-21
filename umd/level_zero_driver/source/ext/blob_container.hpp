/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "vpu_driver/source/os_interface/os_interface.hpp"

#include <memory>

namespace L0 {

class BlobContainer {
  public:
    BlobContainer(uint8_t *ptr, size_t size)
        : ptr(ptr)
        , size(size) {}
    virtual ~BlobContainer() = default;

  public:
    uint8_t *ptr;
    size_t size;
};

class BlobFileContainer : public BlobContainer {
  public:
    BlobFileContainer(uint8_t *ptr, size_t size, std::unique_ptr<VPU::OsFile> file)
        : BlobContainer(ptr, size)
        , file(std::move(file)) {}

  private:
    std::unique_ptr<VPU::OsFile> file;
};

class BlobAllocContainer : public BlobContainer {
  public:
    BlobAllocContainer(std::unique_ptr<uint8_t[]> buffer, size_t size)
        : BlobContainer(buffer.get(), size)
        , buffer(std::move(buffer)) {}

  private:
    std::unique_ptr<uint8_t[]> buffer;
};
} // namespace L0
