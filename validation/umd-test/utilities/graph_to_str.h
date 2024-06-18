/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <level_zero/ze_graph_ext.h>

inline const char *zeGraphArgumentToStr(ze_graph_argument_type_t a) {
    switch (a) {
    case ZE_GRAPH_ARGUMENT_TYPE_INPUT:
        return "ZE_GRAPH_ARGUMENT_TYPE_INPUT";
    case ZE_GRAPH_ARGUMENT_TYPE_OUTPUT:
        return "ZE_GRAPH_ARGUMENT_TYPE_OUTPUT";
    default:
        return "UNKNOWN";
    }
}

inline const char *zeGraphArgumentPrecisionToStr(ze_graph_argument_precision_t a) {
    switch (a) {
    case ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN:
        return "ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN";
    case ZE_GRAPH_ARGUMENT_PRECISION_FP32:
        return "ZE_GRAPH_ARGUMENT_PRECISION_FP32";
    case ZE_GRAPH_ARGUMENT_PRECISION_FP16:
        return "ZE_GRAPH_ARGUMENT_PRECISION_FP16";
    case ZE_GRAPH_ARGUMENT_PRECISION_UINT16:
        return "ZE_GRAPH_ARGUMENT_PRECISION_UINT16";
    case ZE_GRAPH_ARGUMENT_PRECISION_UINT8:
        return "ZE_GRAPH_ARGUMENT_PRECISION_UINT8";
    case ZE_GRAPH_ARGUMENT_PRECISION_UINT4:
        return "ZE_GRAPH_ARGUMENT_PRECISION_UINT4";
    case ZE_GRAPH_ARGUMENT_PRECISION_INT32:
        return "ZE_GRAPH_ARGUMENT_PRECISION_INT32";
    case ZE_GRAPH_ARGUMENT_PRECISION_INT16:
        return "ZE_GRAPH_ARGUMENT_PRECISION_INT16";
    case ZE_GRAPH_ARGUMENT_PRECISION_INT8:
        return "ZE_GRAPH_ARGUMENT_PRECISION_INT8";
    case ZE_GRAPH_ARGUMENT_PRECISION_INT4:
        return "ZE_GRAPH_ARGUMENT_PRECISION_INT4";
    case ZE_GRAPH_ARGUMENT_PRECISION_BIN:
        return "ZE_GRAPH_ARGUMENT_PRECISION_BIN";
    case ZE_GRAPH_ARGUMENT_PRECISION_BF16:
        return "ZE_GRAPH_ARGUMENT_PRECISION_BF16";
    default:
        return "UNKNOWN";
    }
}

inline const char *zeGraphArgumentLayoutToStr(ze_graph_argument_layout_t a) {
    switch (a) {
    case ZE_GRAPH_ARGUMENT_LAYOUT_ANY:
        return "ZE_GRAPH_ARGUMENT_LAYOUT_ANY";
    case ZE_GRAPH_ARGUMENT_LAYOUT_NCHW:
        return "ZE_GRAPH_ARGUMENT_LAYOUT_NCHW";
    case ZE_GRAPH_ARGUMENT_LAYOUT_NHWC:
        return "ZE_GRAPH_ARGUMENT_LAYOUT_NHWC";
    case ZE_GRAPH_ARGUMENT_LAYOUT_NCDHW:
        return "ZE_GRAPH_ARGUMENT_LAYOUT_NCDHW";
    case ZE_GRAPH_ARGUMENT_LAYOUT_NDHWC:
        return "ZE_GRAPH_ARGUMENT_LAYOUT_NDHWC";
    case ZE_GRAPH_ARGUMENT_LAYOUT_OIHW:
        return "ZE_GRAPH_ARGUMENT_LAYOUT_OIHW";
    case ZE_GRAPH_ARGUMENT_LAYOUT_C:
        return "ZE_GRAPH_ARGUMENT_LAYOUT_C";
    case ZE_GRAPH_ARGUMENT_LAYOUT_CHW:
        return "ZE_GRAPH_ARGUMENT_LAYOUT_CHW";
    case ZE_GRAPH_ARGUMENT_LAYOUT_HW:
        return "ZE_GRAPH_ARGUMENT_LAYOUT_HW";
    case ZE_GRAPH_ARGUMENT_LAYOUT_NC:
        return "ZE_GRAPH_ARGUMENT_LAYOUT_NC";
    case ZE_GRAPH_ARGUMENT_LAYOUT_CN:
        return "ZE_GRAPH_ARGUMENT_LAYOUT_CN";
    case ZE_GRAPH_ARGUMENT_LAYOUT_BLOCKED:
        return "ZE_GRAPH_ARGUMENT_LAYOUT_BLOCKED";
    default:
        return "UNKNOWN";
    }
}

inline const char *getExecTypeStr(int execType) {
    switch (execType) {
    case ZE_TASK_EXECUTE_NONE:
        return "ZE_TASK_EXECUTE_NONE";
    case ZE_TASK_EXECUTE_DPU:
        return "ZE_TASK_EXECUTE_DPU";
    case ZE_TASK_EXECUTE_SW:
        return "ZE_TASK_EXECUTE_SW";
    case ZE_TASK_EXECUTE_DMA:
        return "ZE_TASK_EXECUTE_DMA";
    default:
        return "";
    }
}

inline const char *getStatusStr(int status) {
    switch (status) {
    case ZE_LAYER_STATUS_NOT_RUN:
        return "ZE_LAYER_STATUS_NOT_RUN";
    case ZE_LAYER_STATUS_OPTIMIZED_OUT:
        return "ZE_LAYER_STATUS_OPTIMIZED_OUT";
    case ZE_LAYER_STATUS_EXECUTED:
        return "ZE_LAYER_STATUS_EXECUTED";
    default:
        return "";
    }
}
