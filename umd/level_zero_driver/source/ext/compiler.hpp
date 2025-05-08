/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "npu_driver_compiler.h"

#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_ext.h>
#include <level_zero/ze_graph_profiling_ext.h>
#include <memory>
#include <string>
#include <string_view>

namespace VPU {
struct VPUHwInfo;
class VPUDevice;
class VPUDeviceContext;
} // namespace VPU

namespace L0 {

class BlobContainer;

class Compiler {
  public:
    static bool isVclCompilerApiCompatible();
    static ze_result_t compilerInit(VPU::VPUDevice *vpuDev);
    static ze_result_t compilerCreate(const VPU::VPUHwInfo &hwInfo,
                                      vcl_compiler_handle_t &compiler,
                                      vcl_log_handle_t &logHandle);
    static ze_result_t compilerDestroy(vcl_compiler_handle_t compiler);
    static ze_result_t getCompiledBlob(VPU::VPUDeviceContext *ctx,
                                       ze_graph_desc_2_t &desc,
                                       std::unique_ptr<BlobContainer> &graphBlob,
                                       std::string &log);
    static ze_result_t getCompilerProperties(vcl_compiler_properties_t *pProperties);

    static vcl_version_info_t getVclCompilerApiVersion();
    static std::string getCompilerVersionString();
    static ze_result_t getDecodedProfilingBuffer(ze_graph_profiling_type_t profilingType,
                                                 const BlobContainer &blob,
                                                 const uint8_t *profData,
                                                 uint64_t profSize,
                                                 uint32_t *size,
                                                 void *data,
                                                 std::string &log);
    static ze_result_t queryNetworkCreate(vcl_compiler_handle_t compiler,
                                          vcl_query_desc_t &queryDesc,
                                          vcl_query_handle_t *query);
    static ze_result_t
    queryNetwork(vcl_query_handle_t query, uint8_t *pSupportedLayers, size_t *pSize);
    static ze_result_t queryNetworkDestroy(vcl_query_handle_t query);

    static ze_result_t
    getSupportedOptions(VPU::VPUDevice *vpuDevice, size_t *pSize, char *pSupportedOptions);
    static ze_result_t
    isOptionSupported(VPU::VPUDevice *vpuDevice, const char *pOption, const char *pValue);
    static void setLogLevel(const std::string_view &str);
};

} // namespace L0
