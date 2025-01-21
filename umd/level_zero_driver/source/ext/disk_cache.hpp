/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>

#include "blob_container.hpp"
#include "level_zero/ze_graph_ext.h"

#include <filesystem>
#include <memory>
#include <string>

namespace VPU {
class OsInterface;
} // namespace VPU

namespace L0 {

class DiskCache {
  public:
    DiskCache(VPU::OsInterface &osInfc);

    using Key = std::string;

    Key computeKey(const ze_graph_desc_2_t &desc);
    std::unique_ptr<BlobContainer> getBlob(const Key &key);
    void setBlob(const Key &key, const std::unique_ptr<BlobContainer> &blob);

  private:
    VPU::OsInterface &osInfc;
    std::filesystem::path cachePath;
    size_t maxSize;
};

} // namespace L0
