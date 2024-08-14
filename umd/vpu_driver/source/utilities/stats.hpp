/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <fstream>
#include <mutex>
#include <string_view>

class MemoryStatistics final {
    MemoryStatistics() = default;

  public:
    static MemoryStatistics &get();
    void enable(std::string_view statsPath);
    bool isEnabled();
    void inc(VPU::VPUBufferObject::Location loc, size_t size);
    void dec(VPU::VPUBufferObject::Location loc, size_t size);
    void snapshot();

  private:
    bool enabled = false;
    std::ofstream statOut;
    std::mutex mtx;

    uint64_t internalAllocSize = 0;
    uint64_t sharedAllocSize = 0;
    uint64_t deviceAllocSize = 0;
    uint64_t hostAllocSize = 0;
};
