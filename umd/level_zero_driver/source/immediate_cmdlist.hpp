/*
 * Copyright (C) 2024-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include <stdint.h>

#include "cmdlist.hpp"

#include <ze_api.h>

namespace L0 {
struct CommandQueue;
struct Context;

struct ImmediateCommandList : public CommandList {
    ImmediateCommandList(Context *pCtx,
                         ze_device_handle_t hDevice,
                         uint32_t ordinal,
                         uint32_t index,
                         ze_command_queue_flags_t flags,
                         ze_command_queue_mode_t mode,
                         ze_command_queue_priority_t priority,
                         CommandQueue *pCmdQueue);

    static ze_result_t create(ze_context_handle_t hContext,
                              ze_device_handle_t hDevice,
                              const ze_command_queue_desc_t *altdesc,
                              ze_command_list_handle_t *phCommandList);
    ze_result_t isImmediate(ze_bool_t *pIsImmediate) override;
    ze_result_t close() override { return ZE_RESULT_ERROR_UNINITIALIZED; }
    ze_result_t destroy() override;
    ze_result_t hostSynchronize(uint64_t timeout) override;
    ze_result_t getIndex(uint32_t *pIndex) override;
    ze_result_t getImmediateFlags(ze_command_queue_flags_t *pFlags) override;
    ze_result_t getImmediateMode(ze_command_queue_mode_t *pMode) override;
    ze_result_t getImmediatePriority(ze_command_queue_priority_t *pPriority) override;
    ze_result_t appendSignalEvent(ze_event_handle_t hEvent) override;
    ze_result_t appendWaitOnEvents(uint32_t numEvents, ze_event_handle_t *phEvent) override;

  protected:
    ze_result_t checkCommandAppendCondition() override;
    ze_result_t postAppend() override;

    uint32_t index = 0;
    ze_command_queue_flags_t flags = 0;
    ze_command_queue_mode_t mode = ZE_COMMAND_QUEUE_MODE_DEFAULT;
    ze_command_queue_priority_t priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL;
    CommandQueue *pCommandQueue = nullptr;
};
} // namespace L0
