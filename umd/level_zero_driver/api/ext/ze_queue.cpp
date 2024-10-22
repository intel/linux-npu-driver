/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/api/ext/ze_queue.hpp"

#include "level_zero_driver/api/zet_misc.hpp"
#include "level_zero_driver/core/source/cmdqueue/cmdqueue.hpp"
#include "level_zero_driver/include/l0_exception.hpp"

#include <level_zero/loader/ze_loader.h>
#include <level_zero/ze_api.h>
#include <level_zero/ze_command_queue_npu_ext.h>

namespace L0 {
ze_result_t ZE_APICALL
zeCommandQueueSetWorkloadType(ze_command_queue_handle_t hCommandQueue,
                              ze_command_queue_workload_type_t workloadType) {
    if (hCommandQueue == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    auto result = translateHandle(ZEL_HANDLE_COMMAND_LIST, hCommandQueue);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        CommandQueue::fromHandle(hCommandQueue)->setWorkloadType(workloadType));
}

} // namespace L0
