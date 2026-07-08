/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>

#include "blob_container.hpp"
#include "npu_driver_compiler.h"

#include <filesystem>
#include <memory>
#include <string>
#include <ze_graph_ext.h>

namespace VPU {
class OsInterface;
} // namespace VPU

namespace L0 {

class DiskCache {
  public:
    DiskCache(VPU::OsInterface &osInfc);

    using Key = std::string;

    Key computeKey(const ze_graph_desc_2_t &desc,
                   const vcl_compiler_properties_t *vclProp = nullptr);
    std::unique_ptr<BlobContainer> getBlob(const Key &key);
    std::unique_ptr<BlobContainer> setBlob(const Key &key, std::unique_ptr<BlobContainer> blob);

    void setMaxSize(size_t size) { maxSize = size; }
    size_t getMaxSize() { return maxSize; }
    size_t getCacheSize();

  private:
    VPU::OsInterface &osInfc;
    std::filesystem::path cachePath;
    size_t maxSize;
};

} // namespace L0
