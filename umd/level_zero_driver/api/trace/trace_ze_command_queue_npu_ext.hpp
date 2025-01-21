/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Generated header based on ze_command_queue_npu_ext.h

#pragma once

#include "trace_ze_api.hpp"

#include <iostream>
#include <level_zero/ze_api.h>
#include <level_zero/ze_command_queue_npu_ext.h>
#include <sstream>

inline std::string
_trace_zeCommandQueueSetWorkloadType(ze_command_queue_handle_t hCommandQueue,
                                     ze_command_queue_workload_type_t workloadType) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API_EXT] zeCommandQueueSetWorkloadType(";
    ss << "hCommandQueue: " << hCommandQueue;
    ss << ", workloadType: " << workloadType;
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandQueueSetWorkloadType(ze_command_queue_handle_t hCommandQueue,
                                                ze_command_queue_workload_type_t workloadType) {
    TRACE_EVENT_BEGIN("API", "zeCommandQueueSetWorkloadType");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeCommandQueueSetWorkloadType(hCommandQueue, workloadType) + "..\n";
}
inline void trace_zeCommandQueueSetWorkloadType(ze_result_t ret,
                                                ze_command_queue_handle_t hCommandQueue,
                                                ze_command_queue_workload_type_t workloadType) {
    TRACE_EVENT_END("API");
    if (IS_API_EXT_TRACE())
        std::cerr << _trace_zeCommandQueueSetWorkloadType(hCommandQueue, workloadType) +
                         trace_ze_result_t(ret);
}
