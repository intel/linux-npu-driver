/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include <stdint.h>

#include "cmdlist.hpp"

#include <level_zero/ze_api.h>

namespace L0 {
struct CommandQueue;
struct Context;

struct ImmediateCommandList : public CommandList {
    ImmediateCommandList(Context *pCtx, CommandQueue *pCmdQueue);

    static ze_result_t create(ze_context_handle_t hContext,
                              ze_device_handle_t hDevice,
                              const ze_command_queue_desc_t *altdesc,
                              ze_command_list_handle_t *phCommandList);
    ze_result_t isImmediate(ze_bool_t *pIsImmediate) override;
    ze_result_t close() override { return ZE_RESULT_ERROR_UNINITIALIZED; }
    ze_result_t destroy() override;
    ze_result_t hostSynchronize(uint64_t timeout) override;
    ze_result_t appendSignalEvent(ze_event_handle_t hEvent) override;
    ze_result_t appendWaitOnEvents(uint32_t numEvents, ze_event_handle_t *phEvent) override;

  protected:
    ze_result_t checkCommandAppendCondition() override;
    ze_result_t postAppend() override;

    CommandQueue *pCommandQueue = nullptr;
};
} // namespace L0
