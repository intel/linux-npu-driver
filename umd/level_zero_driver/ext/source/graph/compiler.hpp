/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <level_zero/ze_graph_ext.h>
#include <string>

#include <vector>

namespace L0 {
class Compiler {
  public:
    static bool
    getCompiledBlob(size_t &graphSize, std::vector<uint8_t> &graphBlob, ze_graph_desc_t &desc);
    static bool getCompilerProperties(ze_device_graph_properties_t *pDeviceGraphProperties);
    static ze_result_t getDecodedProfilingBuffer(ze_graph_profiling_type_t profilingType,
                                                 const std::vector<uint8_t> *blobRaw,
                                                 const uint8_t *profData,
                                                 uint64_t profSize,
                                                 uint32_t *size,
                                                 void *data);

    static void setCidLogLevel(std::string_view &str) {
        if (str == "TRACE") {
            cidLogLevel = 5;
        } else if (str == "DEBUG") {
            cidLogLevel = 4;
        } else if (str == "INFO") {
            cidLogLevel = 3;
        } else if (str == "WARNING") {
            cidLogLevel = 2;
        } else if (str == "ERROR") {
            cidLogLevel = 1;
        } else {
            cidLogLevel = 0;
        }
    }

  private:
    static uint32_t cidLogLevel;
};

} // namespace L0
