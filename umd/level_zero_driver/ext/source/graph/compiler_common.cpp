/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "compiler_common.hpp"

vcl_log_level_t cidLogLevel = VCL_LOG_NONE;

void setCidLogLevel(std::string_view &str) {
    if (str == "TRACE") {
        cidLogLevel = VCL_LOG_TRACE;
    } else if (str == "DEBUG") {
        cidLogLevel = VCL_LOG_DEBUG;
    } else if (str == "INFO") {
        cidLogLevel = VCL_LOG_INFO;
    } else if (str == "WARNING") {
        cidLogLevel = VCL_LOG_WARNING;
    } else if (str == "ERROR") {
        cidLogLevel = VCL_LOG_ERROR;
    } else {
        cidLogLevel = VCL_LOG_NONE;
    }
}
