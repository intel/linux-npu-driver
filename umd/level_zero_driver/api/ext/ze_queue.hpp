/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero/ze_api.h"
#include "level_zero/ze_command_queue_npu_ext.h"

namespace L0 {

ze_result_t ZE_APICALL zeCommandQueueSetWorkloadType(ze_command_queue_handle_t hCommandQueue,
                                                     ze_command_queue_workload_type_t workloadType);

} // namespace L0
