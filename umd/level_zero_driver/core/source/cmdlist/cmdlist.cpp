/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/core/source/driver/driver.hpp"
#include "level_zero_driver/core/source/event/event.hpp"
#include "level_zero_driver/tools/source/metrics/metric_query.hpp"
#include "vpu_driver/source/command/vpu_barrier_command.hpp"
#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/command/vpu_memory_fill_command.hpp"
#include "vpu_driver/source/command/vpu_query_command.hpp"
#include "vpu_driver/source/command/vpu_ts_command.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace L0 {

CommandList::CommandList(bool isCopyOnly, VPU::VPUDeviceContext *ctx)
    : isCopyOnlyCmdList(isCopyOnly)
    , ctx(ctx)
    , vpuJob(std::make_shared<VPU::VPUJob>(ctx, isCopyOnly)){};

CommandList *
CommandList::create(bool isCopyOnly, VPU::VPUDeviceContext *ctx, ze_result_t &returnValue) {
    CommandList *commandList = new CommandList(isCopyOnly, ctx);

    returnValue = ZE_RESULT_SUCCESS;
    return commandList;
}

ze_result_t CommandList::destroy() {
    delete this;
    LOG_I("CommandList destroyed.");
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::close() {
    if (isCmdListClosed()) {
        LOG_W("CommandList already closed");
        return ZE_RESULT_SUCCESS;
    }

    if (!vpuJob->closeCommands()) {
        LOG_E("Failed to close VPUJob");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::reset() {
    vpuJob = std::make_shared<VPU::VPUJob>(ctx, isCopyOnlyCmdList);

    return ZE_RESULT_SUCCESS;
}

size_t CommandList::getNumCommands() const {
    return vpuJob->getNumCommands();
}

ze_result_t CommandList::checkCommandAppendCondition() {
    if (isCmdListClosed()) {
        LOG_W("CommandList has already been closed");
        return ZE_RESULT_ERROR_NOT_AVAILABLE;
    }

    return ZE_RESULT_SUCCESS;
}

template <typename Cmd, typename... Args>
ze_result_t CommandList::appendCommandWithEvents(ze_event_handle_t hSignalEvent,
                                                 uint32_t numWaitEvents,
                                                 ze_event_handle_t *phWaitEvents,
                                                 Args... args) {
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

    auto cmd = Cmd::create(args...);
    if (cmd == nullptr) {
        LOG_E("Command(%#x) is NULL / failed to be initialized!",
              (cmd == nullptr) ? 0 : cmd->getCommandType());
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(cmd)) {
        LOG_E("Command(%#x) failed to push to list!", cmd->getCommandType());
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    if (hSignalEvent != nullptr) {
        result = appendSignalEvent(hSignalEvent);
        if (result != ZE_RESULT_SUCCESS) {
            LOG_E("Failed to append signal event command (handle: %p, error: %#x).",
                  hSignalEvent,
                  result);
            return result;
        }
    }

    LOG_V("Successfully appended the command(%#x) to CommandList with hSignal(%p), %u wait "
          "events(%p).",
          cmd->getCommandType(),
          hSignalEvent,
          numWaitEvents,
          phWaitEvents);

    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::appendBarrier(ze_event_handle_t hSignalEvent,
                                       uint32_t numWaitEvents,
                                       ze_event_handle_t *phWaitEvents) {
    // Append a barrier command.
    return appendCommandWithEvents<VPU::VPUBarrierCommand>(hSignalEvent,
                                                           numWaitEvents,
                                                           phWaitEvents);
}

ze_result_t CommandList::appendMemoryCopy(void *dstptr,
                                          const void *srcptr,
                                          size_t size,
                                          ze_event_handle_t hSignalEvent,
                                          uint32_t numWaitEvents,
                                          ze_event_handle_t *phWaitEvents) {
    if ((dstptr == nullptr) || (srcptr == nullptr)) {
        LOG_E("Pointer to destination/source memory passed as nullptr.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    // Append a memory copy command.
    return appendCommandWithEvents<VPU::VPUCopyCommand>(hSignalEvent,
                                                        numWaitEvents,
                                                        phWaitEvents,
                                                        ctx,
                                                        srcptr,
                                                        dstptr,
                                                        size);
}

ze_result_t CommandList::appendMemoryFill(void *ptr,
                                          const void *pattern,
                                          size_t patternSize,
                                          size_t size,
                                          ze_event_handle_t hSignalEvent,
                                          uint32_t numWaitEvents,
                                          ze_event_handle_t *phWaitEvents) {
    if (ptr == nullptr) {
        LOG_E("Pointer to memory passed as nullptr.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (pattern == nullptr) {
        LOG_E("Pointer to value to initialize memory passed as nullptr.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    // Append a memory fill command.
    return appendCommandWithEvents<VPU::VPUMemoryFillCommand>(hSignalEvent,
                                                              numWaitEvents,
                                                              phWaitEvents,
                                                              ctx,
                                                              ptr,
                                                              pattern,
                                                              patternSize,
                                                              size);
}

ze_result_t CommandList::appendWriteGlobalTimestamp(uint64_t *dstptr,
                                                    ze_event_handle_t hSignalEvent,
                                                    uint32_t numWaitEvents,
                                                    ze_event_handle_t *phWaitEvents) {
    if (dstptr == nullptr) {
        LOG_E("dstptr is NULL.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    // Append a TS command.
    return appendCommandWithEvents<VPU::VPUTimeStampCommand>(hSignalEvent,
                                                             numWaitEvents,
                                                             phWaitEvents,
                                                             ctx,
                                                             dstptr);
}

ze_result_t CommandList::appendGraphInitialize(ze_graph_handle_t hGraph,
                                               ze_event_handle_t hSignalEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
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

    Graph *graph = Graph::fromHandle(hGraph);
    if (graph == nullptr) {
        LOG_E("Graph object is NULL.");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    auto cmd = graph->allocateGraphInitCommand(ctx);
    if (cmd == nullptr) {
        LOG_E("Graph-Initialize Command failed to be initialized!");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(cmd)) {
        LOG_E("Failed to push Graph-Initialize command to list!");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    if (hSignalEvent != nullptr) {
        result = appendSignalEvent(hSignalEvent);
        if (result != ZE_RESULT_SUCCESS) {
            LOG_E("Failed to append signal event command (handle: %p, error: %#x).",
                  hSignalEvent,
                  result);
            return result;
        }
    }

    LOG_V("Successfully appended graph initialize command to CommandList.");
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::appendGraphExecute(ze_graph_handle_t hGraph,
                                            ze_graph_profiling_query_handle_t hProfilingQuery,
                                            ze_event_handle_t hSignalEvent,
                                            uint32_t numWaitEvents,
                                            ze_event_handle_t *phWaitEvents) {
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

    Graph *graph = Graph::fromHandle(hGraph);
    if (graph == nullptr) {
        LOG_E("Invalid graph handle.");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    void *profilingQueryPtr = nullptr;
    if (graph->getProfilingOutputSize()) {
        auto *profilingQuery = GraphProfilingQuery::fromHandle(hProfilingQuery);
        if (!profilingQuery) {
            LOG_E("Invalid profiling query handle.");
            return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        }
        profilingQueryPtr = profilingQuery->getQueryPtr();
    }

    auto cmd = graph->allocateGraphExecuteCommand(ctx, profilingQueryPtr);
    if (cmd == nullptr) {
        LOG_E("Graph-Execute Command failed to be initialized!");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(cmd)) {
        LOG_E("Failed to push Graph-Execute command to list!");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    if (hSignalEvent != nullptr) {
        result = appendSignalEvent(hSignalEvent);
        if (result != ZE_RESULT_SUCCESS) {
            LOG_E("Failed to append signal event command (handle: %p, error: %#x).",
                  hSignalEvent,
                  result);
            return result;
        }
    }

    LOG_V("Successfully appended graph execute command to CommandList.");
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::appendSignalEvent(ze_event_handle_t hEvent) {
    ze_result_t result = checkCommandAppendCondition();
    if (result != ZE_RESULT_SUCCESS)
        return result;

    auto event = Event::fromHandle(hEvent);
    if (event == nullptr) {
        LOG_E("Failed to get event handle.");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    VPU::VPUEventCommand::KMDEventDataType *evSyncPtr = event->getSyncPointer();
    if (evSyncPtr == nullptr) {
        LOG_E("Invalid sync pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    auto cmd = VPU::VPUEventSignalCommand::create(ctx, evSyncPtr);
    if (cmd == nullptr) {
        LOG_E("Failed to initialize signal event Command.");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(cmd)) {
        LOG_E("Failed to push signal event command to list!");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    event->associateJob(vpuJob);
    LOG_V("Successfully appended signal event command to CommandList.");
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::appendWaitOnEvents(uint32_t numEvents, ze_event_handle_t *phEvent) {
    if (phEvent == nullptr)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (numEvents == 0u)
        return ZE_RESULT_ERROR_INVALID_SIZE;

    ze_result_t result = checkCommandAppendCondition();
    if (result != ZE_RESULT_SUCCESS)
        return result;

    for (uint32_t i = 0; i < numEvents; ++i) {
        auto event = Event::fromHandle(phEvent[i]);
        if (event == nullptr) {
            LOG_E("Failed to get event handle.");
            return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        }

        VPU::VPUEventCommand::KMDEventDataType *evSyncPtr = event->getSyncPointer();
        if (evSyncPtr == nullptr) {
            LOG_E("Invalid sync pointer.");
            return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        }

        auto cmd = VPU::VPUEventWaitCommand::create(ctx, evSyncPtr);
        if (cmd == nullptr) {
            LOG_E("Failed to initialize event wait Command.");
            return ZE_RESULT_ERROR_UNINITIALIZED;
        }

        if (!vpuJob->appendCommand(cmd)) {
            LOG_E("Failed to push event wait command to list!");
            return ZE_RESULT_ERROR_UNKNOWN;
        }

        event->associateJob(vpuJob);
        LOG_V("Successfully appended event wait command to CommandList.");
    }
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::appendEventReset(ze_event_handle_t hEvent) {
    ze_result_t result = checkCommandAppendCondition();
    if (result != ZE_RESULT_SUCCESS)
        return result;

    auto event = Event::fromHandle(hEvent);
    if (event == nullptr) {
        LOG_E("Failed to get event handle.");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    VPU::VPUEventCommand::KMDEventDataType *evSyncPtr = event->getSyncPointer();
    if (evSyncPtr == nullptr) {
        LOG_E("Invalid sync pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    auto cmd = VPU::VPUEventResetCommand::create(ctx, evSyncPtr);
    if (cmd == nullptr) {
        LOG_E("Failed to initialize reset event Command.");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(cmd)) {
        LOG_E("Failed to push reset event command to list!");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    event->associateJob(vpuJob);
    LOG_V("Successfully appended reset event command to CommandList.");
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::appendMetricQueryBegin(zet_metric_query_handle_t hMetricQuery) {
    if (hMetricQuery == nullptr) {
        LOG_E("MetricQuery handle is NULL.");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    ze_result_t result = checkCommandAppendCondition();
    if (result != ZE_RESULT_SUCCESS)
        return result;

    auto metricQuery = MetricQuery::fromHandle(hMetricQuery);
    if (!metricQuery->isGroupActivated()) {
        LOG_E("MetricQuery (%p) Group is not activated! Please activate metric group before Query "
              "Begin command.",
              metricQuery);
        return ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE;
    }

    auto cmd = VPU::VPUQueryBeginCommand::create(ctx,
                                                 metricQuery->getMetricGroupMask(),
                                                 metricQuery->getMetricAddrPtr());
    if (cmd == nullptr) {
        LOG_E("Failed to initialize metric query begin Command.");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(cmd)) {
        LOG_E("Failed to push metric query begin command to list!");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    LOG_V("Successfully appended metric query begin command to CommandList.");
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::appendMetricQueryEnd(zet_metric_query_handle_t hMetricQuery,
                                              ze_event_handle_t hSignalEvent,
                                              uint32_t numWaitEvents,
                                              ze_event_handle_t *phWaitEvents) {
    if (hMetricQuery == nullptr) {
        LOG_E("MetricQuery handle is NULL.");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    auto metricQuery = MetricQuery::fromHandle(hMetricQuery);
    if (!metricQuery->isGroupActivated()) {
        LOG_E("MetricQuery (%p) Group is not activated! Please activate metric group before Query "
              "End command.",
              metricQuery);
        return ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE;
    }

    return appendCommandWithEvents<VPU::VPUQueryEndCommand>(hSignalEvent,
                                                            numWaitEvents,
                                                            phWaitEvents,
                                                            ctx,
                                                            metricQuery->getMetricGroupMask(),
                                                            metricQuery->getMetricAddrPtr());
}

} // namespace L0
