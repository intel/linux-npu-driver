/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"

#include <level_zero/ze_api.h>

namespace L0 {
ze_result_t zeCommandListAppendBarrier(ze_command_list_handle_t hCommandList,
                                       ze_event_handle_t hSignalEvent,
                                       uint32_t numWaitEvents,
                                       ze_event_handle_t *phWaitEvents) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    return L0::CommandList::fromHandle(hCommandList)
        ->appendBarrier(hSignalEvent, numWaitEvents, phWaitEvents);
}

ze_result_t zeCommandListAppendMemoryRangesBarrier(ze_command_list_handle_t hCommandList,
                                                   uint32_t numRanges,
                                                   const size_t *pRangeSizes,
                                                   const void **pRanges,
                                                   ze_event_handle_t hSignalEvent,
                                                   uint32_t numWaitEvents,
                                                   ze_event_handle_t *phWaitEvents) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendBarrier(ze_command_list_handle_t hCommandList,
                           ze_event_handle_t hSignalEvent,
                           uint32_t numWaitEvents,
                           ze_event_handle_t *phWaitEvents) {
    return L0::zeCommandListAppendBarrier(hCommandList, hSignalEvent, numWaitEvents, phWaitEvents);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendMemoryRangesBarrier(ze_command_list_handle_t hCommandList,
                                       uint32_t numRanges,
                                       const size_t *pRangeSizes,
                                       const void **pRanges,
                                       ze_event_handle_t hSignalEvent,
                                       uint32_t numWaitEvents,
                                       ze_event_handle_t *phWaitEvents) {
    return L0::zeCommandListAppendMemoryRangesBarrier(hCommandList,
                                                      numRanges,
                                                      pRangeSizes,
                                                      pRanges,
                                                      hSignalEvent,
                                                      numWaitEvents,
                                                      phWaitEvents);
}
} // extern "C"
