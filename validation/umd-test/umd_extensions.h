/*
 * Copyright (C) 2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <level_zero/ze_api.h>
#include <level_zero/ze_command_queue_npu_ext.h>
#include <level_zero/ze_graph_ext.h>
#include <level_zero/ze_graph_profiling_ext.h>

#define GRAPH_EXT_VERSION ZE_GRAPH_EXT_VERSION_CURRENT
typedef ze_graph_dditable_ext_t graph_dditable_ext_t;

#define COMMAND_QUEUE_EXT_NAME ZE_COMMAND_QUEUE_NPU_EXT_NAME
#define COMMAND_QUEUE_NPU_EXT_VERSION ZE_COMMAND_QUEUE_NPU_EXT_VERSION_1_0
typedef ze_command_queue_npu_dditable_ext_t command_queue_dditable_t;
