/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/cmdqueue/cmdqueue.hpp"

struct _ze_command_list_handle_t {};

namespace L0 {

struct CommandList : _ze_command_list_handle_t, IContextObject {
    CommandList(Context *pContext, bool isCopyOnly);
    ~CommandList();

    ze_result_t destroy();
    bool isCopyOnly() const { return isCopyOnlyCmdList; };

    static ze_result_t create(ze_context_handle_t hContext,
                              ze_device_handle_t hDevice,
                              const ze_command_list_desc_t *desc,
                              ze_command_list_handle_t *phCommandList);

    ze_result_t close();
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
    ze_result_t appendSignalEvent(ze_event_handle_t hEvent);
    ze_result_t appendWaitOnEvents(uint32_t numEvents, ze_event_handle_t *phEvent);
    ze_result_t appendEventReset(ze_event_handle_t hEvent);
    ze_result_t appendMetricQueryBegin(zet_metric_query_handle_t hMetricQuery);
    ze_result_t appendMetricQueryEnd(zet_metric_query_handle_t hMetricQuery,
                                     ze_event_handle_t hSignalEvent,
                                     uint32_t numWaitEvents,
                                     ze_event_handle_t *phWaitEvents);

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

  private:
    ze_result_t checkCommandAppendCondition();
    VPU::VPUEventCommand::KMDEventDataType *getEventSyncPointerFromHandle(ze_event_handle_t hEvent);

    /**
     * @brief Append given command to the command list with events.
     *
     * @param hSignalEvent [in]: Nullable, event to be signaled upon completion.
     * @param numWaitEvents [in]: Number of wait on events.
     * @param phWaitEvents [in]: Nullable, array of waiting on events.
     * @param args[in]: Parameters for the command.
     * @return ze_result_t ZE_RESULT_SUCCESS on successful appending.
     */
    template <typename Cmd, typename... Args>
    ze_result_t appendCommandWithEvents(ze_event_handle_t hSignalEvent,
                                        uint32_t numWaitEvents,
                                        ze_event_handle_t *phWaitEvents,
                                        Args... args);

  protected:
    Context *pContext = nullptr;
    bool isCopyOnlyCmdList = false;
    VPU::VPUDeviceContext *ctx = nullptr;
    std::shared_ptr<VPU::VPUJob> vpuJob = nullptr;
    std::vector<VPU::VPUBufferObject *> tracedInternalBos;
    std::vector<std::unique_ptr<InferenceExecutor>> tracedInferences;
};

} // namespace L0
