/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/include/l0_handler.hpp"
#include "level_zero_driver/source/event.hpp"
#include "vpu_driver/source/command/vpu_event_command.hpp"
#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_profiling_ext.h>
#include <level_zero/zet_api.h>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace L0 {
struct Context;
} // namespace L0
namespace VPU {
class VPUBufferObject;
class VPUCommand;
class VPUDeviceContext;
} // namespace VPU

struct _ze_command_list_handle_t {};

namespace L0 {

struct CommandList : _ze_command_list_handle_t, IContextObject {
    CommandList(Context *pContext, bool isMutable);

    static ze_result_t create(ze_context_handle_t hContext,
                              ze_device_handle_t hDevice,
                              const ze_command_list_desc_t *desc,
                              ze_command_list_handle_t *phCommandList);
    virtual ze_result_t isImmediate(ze_bool_t *pIsImmediate);
    virtual ze_result_t close();
    virtual ze_result_t destroy();
    virtual ze_result_t hostSynchronize(uint64_t timeout) { return ZE_RESULT_ERROR_UNINITIALIZED; }
    ze_result_t reset();
    ze_result_t appendBarrier(ze_event_handle_t hSignalEvent,
                              uint32_t numWaitEvents,
                              ze_event_handle_t *phWaitEvents);
    ze_result_t appendMemoryCopy(void *dstptr,
                                 const void *srcptr,
                                 size_t size,
                                 ze_event_handle_t hSignalEvent,
                                 uint32_t numWaitEvents,
                                 ze_event_handle_t *phWaitEvents);
    ze_result_t appendMemoryFill(void *ptr,
                                 const void *pattern,
                                 size_t patternSize,
                                 size_t size,
                                 ze_event_handle_t hEvent,
                                 uint32_t numWaitEvents,
                                 ze_event_handle_t *phWaitEvents);
    ze_result_t appendWriteGlobalTimestamp(std::shared_ptr<VPU::VPUBufferObject> timestampBo,
                                           ze_event_handle_t hSignalEvent,
                                           uint32_t numWaitEvents,
                                           ze_event_handle_t *phWaitEvents);

    ze_result_t appendWriteGlobalTimestamp(uint64_t *dstptr,
                                           ze_event_handle_t hSignalEvent,
                                           uint32_t numWaitEvents,
                                           ze_event_handle_t *phWaitEvents);
    ze_result_t appendGraphInitialize(ze_graph_handle_t hGraph,
                                      ze_event_handle_t hSignalEvent,
                                      uint32_t numWaitEvents,
                                      ze_event_handle_t *phWaitEvents);
    ze_result_t appendGraphExecute(ze_graph_handle_t hGraph,
                                   ze_graph_profiling_query_handle_t hProfilingQuery,
                                   ze_event_handle_t hSignalEvent,
                                   uint32_t numWaitEvents,
                                   ze_event_handle_t *phWaitEvents);
    virtual ze_result_t appendSignalEvent(ze_event_handle_t hEvent);
    virtual ze_result_t appendWaitOnEvents(uint32_t numEvents, ze_event_handle_t *phEvent);
    ze_result_t appendEventReset(ze_event_handle_t hEvent);
    ze_result_t appendMetricQueryBegin(zet_metric_query_handle_t hMetricQuery);
    ze_result_t appendMetricQueryEnd(zet_metric_query_handle_t hMetricQuery,
                                     ze_event_handle_t hSignalEvent,
                                     uint32_t numWaitEvents,
                                     ze_event_handle_t *phWaitEvents);

    ze_result_t getNextCommandId(const ze_mutable_command_id_exp_desc_t *desc,
                                 uint64_t *pCommandId);
    ze_result_t updateMutableCommands(const ze_mutable_commands_exp_desc_t *desc);

    inline ze_command_list_handle_t toHandle() { return this; }
    static CommandList *fromHandle(ze_command_list_handle_t handle) {
        return static_cast<CommandList *>(handle);
    }

    bool isCmdListClosed() const { return vpuJob->isClosed(); }
    size_t getNumCommands() const;

    const std::vector<std::shared_ptr<VPU::VPUCommand>> &getCommands() const {
        return vpuJob->getCommands();
    }
    std::shared_ptr<VPU::VPUJob> getJob() const { return vpuJob; }

  protected:
    ze_result_t appendMemoryFillCmd(void *ptr,
                                    const void *pattern,
                                    size_t patternSize,
                                    size_t size,
                                    ze_event_handle_t hEvent,
                                    uint32_t numWaitEvents,
                                    ze_event_handle_t *phWaitEvents);
    ze_result_t appendMemoryFillAsCopyCmd(void *ptr,
                                          const void *pattern,
                                          size_t patternSize,
                                          size_t size,
                                          ze_event_handle_t hEvent,
                                          uint32_t numWaitEvents,
                                          ze_event_handle_t *phWaitEvents);

    virtual ze_result_t checkCommandAppendCondition();
    virtual ze_result_t postAppend() { return ZE_RESULT_SUCCESS; }
    VPU::VPUEventCommand::KMDEventDataType *getEventSyncPointerFromHandle(ze_event_handle_t hEvent);

    template <typename Cmd, typename... Args>
    ze_result_t appendCommand(Args &&...args) {
        auto cmd = Cmd::create(std::forward<Args>(args)...);
        if (cmd == nullptr) {
            LOG_E("Command is NULL / failed to be initialized!");
            return ZE_RESULT_ERROR_UNINITIALIZED;
        }

        if (!vpuJob->appendCommand(cmd)) {
            LOG_E("Command(%#x) failed to push to list!", cmd->getCommandType());
            return ZE_RESULT_ERROR_UNKNOWN;
        }

        LOG(CMDLIST,
            "Successfully appended the command(%#x) to CommandList",
            cmd->getCommandType());

        return ZE_RESULT_SUCCESS;
    }

    template <typename Cmd, typename... Args>
    ze_result_t appendCommandWithEvents(ze_event_handle_t hSignalEvent,
                                        uint32_t numWaitEvents,
                                        ze_event_handle_t *phWaitEvents,
                                        Args &&...args) {
        ze_result_t result = checkCommandAppendCondition();
        if (result != ZE_RESULT_SUCCESS)
            return result;

        if (numWaitEvents > 0) {
            if (phWaitEvents == nullptr) {
                LOG_E("Invalid wait event input. phWaitEvents: %p, numWaitEvents: %u",
                      phWaitEvents,
                      numWaitEvents);
                return ZE_RESULT_ERROR_INVALID_SIZE;
            }

            result = appendWaitOnEvents(numWaitEvents, phWaitEvents);
            if (result != ZE_RESULT_SUCCESS) {
                LOG_E("Failed to add %u wait on events.", numWaitEvents);
                return result;
            }
        }

        result = appendCommand<Cmd>(std::forward<Args>(args)...);
        if (result != ZE_RESULT_SUCCESS)
            return result;

        if (hSignalEvent != nullptr) {
            result = appendSignalEvent(hSignalEvent);
            if (result != ZE_RESULT_SUCCESS) {
                LOG_E("Failed to append signal event command (handle: %p, error: %#x).",
                      hSignalEvent,
                      result);
                return result;
            }
        }

        LOG(CMDLIST,
            "Successfully appended the command with hSignal(%p), %u wait events(%p).",
            hSignalEvent,
            numWaitEvents,
            phWaitEvents);

        return postAppend();
    }

    Context *pContext = nullptr;
    bool isMutable = false;
    VPU::VPUDeviceContext *ctx = nullptr;
    std::shared_ptr<VPU::VPUJob> vpuJob = nullptr;
    std::vector<VPU::VPUBufferObject *> tracedInternalBos;
    std::unordered_map<uint64_t, uint64_t> commandIdMap;
};

} // namespace L0
