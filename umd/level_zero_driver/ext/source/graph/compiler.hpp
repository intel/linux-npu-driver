/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <level_zero/ze_graph_ext.h>

#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpux_driver_compiler.h"

#include <string>
#include <vector>

namespace L0 {

class Compiler {
  public:
    static bool compilerInit(int compilerPlatformType);
    static bool getCompiledBlob(VPU::VPUDeviceContext *ctx,
                                size_t &graphSize,
                                std::vector<uint8_t> &graphBlob,
                                ze_graph_desc_2_t &desc,
                                std::string &logBuffer);
    static bool getCompilerProperties(vcl_compiler_properties_t *pProperties);
    static uint16_t getCompilerVersionMajor();
    static uint16_t getCompilerVersionMinor();
    static bool checkVersion(uint16_t major, uint16_t minor);
    static ze_result_t getDecodedProfilingBuffer(ze_graph_profiling_type_t profilingType,
                                                 const std::vector<uint8_t> *blobRaw,
                                                 const uint8_t *profData,
                                                 uint64_t profSize,
                                                 uint32_t *size,
                                                 void *data,
                                                 std::string &logBuffer);
};

} // namespace L0
