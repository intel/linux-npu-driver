/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>

#include "npu_driver_compiler.h"

#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_ext.h>
#include <level_zero/ze_graph_profiling_ext.h>
#include <memory>
#include <string>

namespace VPU {
struct VPUHwInfo;
class VPUDevice;
class VPUDeviceContext;
} // namespace VPU

namespace L0 {

class BlobContainer;

class Compiler {
  public:
    static bool isApiComatible();
    static bool compilerInit(VPU::VPUDevice *vpuDev);
    static vcl_result_t compilerCreate(const VPU::VPUHwInfo &hwInfo,
                                       vcl_compiler_handle_t &compiler,
                                       vcl_log_handle_t &logHandle);
    static bool getCompiledBlob(VPU::VPUDeviceContext *ctx,
                                ze_graph_desc_2_t &desc,
                                std::unique_ptr<BlobContainer> &graphBlob,
                                std::string &logBuffer);
    static bool getCompilerProperties(vcl_compiler_properties_t *pProperties);
    static uint16_t getCompilerVersionMajor();
    static uint16_t getCompilerVersionMinor();
    static std::string getCompilerVersionString();
    static ze_result_t getDecodedProfilingBuffer(ze_graph_profiling_type_t profilingType,
                                                 const BlobContainer &blob,
                                                 const uint8_t *profData,
                                                 uint64_t profSize,
                                                 uint32_t *size,
                                                 void *data,
                                                 std::string &logBuffer);
};

} // namespace L0
