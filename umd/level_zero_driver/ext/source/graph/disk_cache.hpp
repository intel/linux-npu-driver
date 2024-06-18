/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero/ze_graph_ext.h"
#include "vpu_driver/source/os_interface/os_interface.hpp"

#include <stdint.h>
#include <string>
#include <vector>

namespace L0 {

class DiskCache {
  public:
    DiskCache(VPU::OsInterface &osInfc);

    using Key = std::string;
    using Blob = std::vector<uint8_t>;

    Key computeKey(const ze_graph_desc_2_t &desc);
    Blob getBlob(const Key &key);
    void setBlob(const Key &key, const Blob &val);

  private:
    VPU::OsInterface &osInfc;
    std::filesystem::path cachePath;
    size_t maxSize;
};

} // namespace L0
