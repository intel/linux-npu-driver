/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/api/ext/ze_queue.hpp"

#include "level_zero_driver/api/trace/trace_ze_command_queue_npu_ext.hpp"
#include "level_zero_driver/api/zet_misc.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/cmdqueue.hpp"

#include <level_zero/loader/ze_loader.h>
#include <level_zero/ze_api.h>
#include <level_zero/ze_command_queue_npu_ext.h>

namespace L0 {
ze_result_t ZE_APICALL
zeCommandQueueSetWorkloadType(ze_command_queue_handle_t hCommandQueue,
                              ze_command_queue_workload_type_t workloadType) {
    trace_zeCommandQueueSetWorkloadType(hCommandQueue, workloadType);
    ze_result_t ret;

    if (hCommandQueue == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    ret = translateHandle(ZEL_HANDLE_COMMAND_LIST, hCommandQueue);
    if (ret != ZE_RESULT_SUCCESS) {
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret,
                        CommandQueue::fromHandle(hCommandQueue)->setWorkloadType(workloadType));

exit:
    trace_zeCommandQueueSetWorkloadType(ret, hCommandQueue, workloadType);
    return ret;
}

} // namespace L0
