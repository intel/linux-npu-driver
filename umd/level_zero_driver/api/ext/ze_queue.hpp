/*
 * Copyright (C) 2024-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <ze_api.h>
#include <ze_command_queue_npu_ext.h>

namespace L0 {

ze_result_t ZE_APICALL zeCommandQueueSetWorkloadType(ze_command_queue_handle_t hCommandQueue,
                                                     ze_command_queue_workload_type_t workloadType);

} // namespace L0
