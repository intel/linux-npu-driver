/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"

#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/event/event.hpp"
#include "level_zero_driver/ext/source/graph/profiling_data.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/include/nested_structs_handler.hpp"
#include "level_zero_driver/tools/source/metrics/metric_query.hpp"
#include "vpu_driver/source/command/vpu_barrier_command.hpp"
#include "vpu_driver/source/command/vpu_command.hpp"
#include "vpu_driver/source/command/vpu_copy_command.hpp"
#include "vpu_driver/source/command/vpu_query_command.hpp"
#include "vpu_driver/source/command/vpu_ts_command.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_ext.h>
#include <optional>
#include <utility>

namespace L0 {

CommandList::CommandList(Context *pContext, bool isCopyOnly, bool isMutable)
    : pContext(pContext)
    , isCopyOnlyCmdList(isCopyOnly)
    , isMutable(isMutable)
    , ctx(pContext->getDeviceContext())
    , vpuJob(std::make_shared<VPU::VPUJob>(ctx, isCopyOnly)) {}

CommandList::~CommandList() {
    for (auto &bo : tracedInternalBos)
        ctx->freeMemAlloc(bo);
}

ze_result_t CommandList::create(ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                const ze_command_list_desc_t *desc,
                                ze_command_list_handle_t *phCommandList) {
    if (hContext == nullptr) {
        LOG_E("Invalid hContext pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    if (hDevice == nullptr) {
        LOG_E("Invalid hDevice pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    if (desc == nullptr) {
        LOG_E("Invalid desc pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    if (phCommandList == nullptr) {
        LOG_E("Invalid phCommandList pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    try {
        Device *pDevice = Device::fromHandle(hDevice);
        ze_command_queue_group_property_flags_t flags =
            pDevice->getCommandQeueueGroupFlags(desc->commandQueueGroupOrdinal);
        L0_THROW_WHEN(flags == 0, "Invalid group ordinal", ZE_RESULT_ERROR_INVALID_ARGUMENT);

        Context *pContext = Context::fromHandle(hContext);
        bool isCopyOnly = flags == ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COPY;
        bool isMutable = false;
        if (desc->pNext != nullptr) {
            const ze_structure_type_t stype =
                *reinterpret_cast<const ze_structure_type_t *>(desc->pNext);
            isMutable = stype == ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_LIST_EXP_DESC;
        }
        auto commandList = std::make_unique<CommandList>(pContext, isCopyOnly, isMutable);

        *phCommandList = commandList.get();
        pContext->appendObject(std::move(commandList));

        LOG(CMDLIST, "CommandList created - %p", *phCommandList);
    } catch (const DriverError &err) {
        return err.result();
    }
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::destroy() {
    pContext->removeObject(this);
    LOG(CMDLIST, "CommandList destroyed");
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::close() {
    if (isCmdListClosed() && !isMutable) {
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
    for (auto &bo : tracedInternalBos)
        ctx->freeMemAlloc(bo);
    tracedInternalBos.clear();
    tracedInferences.clear();
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

    LOG(CMDLIST,
        "Successfully appended the command(%#x) to CommandList with hSignal(%p), %u wait "
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
        LOG_E("Pointer to destination/source memory passed as nullptr");
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
        LOG_E("Pointer to memory passed as nullptr");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (pattern == nullptr) {
        LOG_E("Pointer to value to initialize memory passed as nullptr");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (!(patternSize == 1u || patternSize == 2u || patternSize == 4u)) {
        LOG_E("Invalid pattern size %ld. Value should be power of 2. Max = 4.", patternSize);
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    auto patternBo =
        ctx->createInternalBufferObject(size + patternSize, VPU::VPUBufferObject::Type::CachedDma);
    if (patternBo == nullptr) {
        LOG_E("Failed to allocate memory");
        return ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
    }
    tracedInternalBos.push_back(patternBo);

    if (!patternBo->fillBuffer(pattern, patternSize)) {
        LOG_E("Failed to fill memory");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    // Implemet fill by memory copy from internal filled buffer to user buffer.
    return appendCommandWithEvents<VPU::VPUCopyCommand>(hSignalEvent,
                                                        numWaitEvents,
                                                        phWaitEvents,
                                                        ctx,
                                                        patternBo->getBasePointer(),
                                                        ptr,
                                                        size);
}

ze_result_t CommandList::appendWriteGlobalTimestamp(uint64_t *dstptr,
                                                    ze_event_handle_t hSignalEvent,
                                                    uint32_t numWaitEvents,
                                                    ze_event_handle_t *phWaitEvents,
                                                    bool skipDmaCopy) {
    if (dstptr == nullptr) {
        LOG_E("dstptr is NULL");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto dstBo = ctx->findBuffer(dstptr);
    if (dstBo == nullptr) {
        LOG_E("Buffer object not found");
        return ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    if (skipDmaCopy) {
        return appendCommandWithEvents<VPU::VPUTimeStampCommand>(
            nullptr,
            numWaitEvents,
            phWaitEvents,
            ctx,
            reinterpret_cast<uint64_t *>(dstBo->getBasePointer()));
    }

    auto allignedBo =
        ctx->createInternalBufferObject(sizeof(uint64_t), VPU::VPUBufferObject::Type::CachedFw);

    if (allignedBo == nullptr) {
        LOG_E("Failed to allocate memory");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    tracedInternalBos.push_back(allignedBo);

    ze_result_t ret;
    ret = appendCommandWithEvents<VPU::VPUTimeStampCommand>(
        nullptr,
        numWaitEvents,
        phWaitEvents,
        ctx,
        reinterpret_cast<uint64_t *>(allignedBo->getBasePointer()));
    if (ret != ZE_RESULT_SUCCESS) {
        LOG_E("Failed to append command");
        return ret;
    }

    return appendCommandWithEvents<VPU::VPUCopyCommand>(hSignalEvent,
                                                        0,
                                                        nullptr,
                                                        ctx,
                                                        allignedBo->getBasePointer(),
                                                        dstptr,
                                                        sizeof(uint64_t));
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
        LOG_E("Graph object is NULL");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    auto cmd = graph->allocateGraphInitCommand(ctx);
    if (cmd == nullptr) {
        LOG_E("Graph-Initialize Command failed to be initialized!");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(std::move(cmd))) {
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

    LOG(CMDLIST, "Successfully appended graph initialize command to CommandList");
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

    uint64_t newCommandId = getNumCommands();

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

    if (isMutable) {
        uint64_t newCommandIdAfterEvents = getNumCommands();
        if (newCommandId < newCommandIdAfterEvents) {
            commandIdMap.emplace(newCommandId, newCommandIdAfterEvents);
        }
    }

    Graph *graph = Graph::fromHandle(hGraph);
    if (graph == nullptr) {
        LOG_E("Invalid graph handle");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    void *profilingQueryPtr = nullptr;
    if (graph->getProfilingOutputSize()) {
        auto *profilingQuery = GraphProfilingQuery::fromHandle(hProfilingQuery);
        if (!profilingQuery) {
            LOG_E("Invalid profiling query handle");
            return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        }
        profilingQueryPtr = profilingQuery->getQueryPtr();
    }

    std::unique_ptr<InferenceExecutor> inferenceExecutor;
    try {
        inferenceExecutor = graph->getGraphExecutor(ctx, profilingQueryPtr);
        if (!inferenceExecutor) {
            LOG_E("Fail to prepare inference execution!");
            return ZE_RESULT_ERROR_UNINITIALIZED;
        }
    } catch (const DriverError &err) {
        return err.result();
    }

    auto cmd = inferenceExecutor->getExecuteCommand();
    if (cmd == nullptr) {
        LOG_E("Graph-Execute Command failed to be initialized!");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(std::move(cmd))) {
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

    tracedInferences.push_back(std::move(inferenceExecutor));
    LOG(CMDLIST, "Successfully appended graph execute command to CommandList");
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::appendSignalEvent(ze_event_handle_t hEvent) {
    ze_result_t result = checkCommandAppendCondition();
    if (result != ZE_RESULT_SUCCESS)
        return result;

    auto event = Event::fromHandle(hEvent);
    if (event == nullptr) {
        LOG_E("Failed to get event handle");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    VPU::VPUEventCommand::KMDEventDataType *evSyncPtr = event->getSyncPointer();
    if (evSyncPtr == nullptr) {
        LOG_E("Invalid sync pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    auto cmd = VPU::VPUEventSignalCommand::create(ctx, evSyncPtr);
    if (cmd == nullptr) {
        LOG_E("Failed to initialize signal event Command");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(cmd)) {
        LOG_E("Failed to push signal event command to list!");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    event->associateJob(vpuJob);
    LOG(CMDLIST, "Successfully appended signal event command to CommandList");
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
            LOG_E("Failed to get event handle");
            return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        }

        VPU::VPUEventCommand::KMDEventDataType *evSyncPtr = event->getSyncPointer();
        if (evSyncPtr == nullptr) {
            LOG_E("Invalid sync pointer");
            return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        }

        auto cmd = VPU::VPUEventWaitCommand::create(ctx, evSyncPtr);
        if (cmd == nullptr) {
            LOG_E("Failed to initialize event wait Command");
            return ZE_RESULT_ERROR_UNINITIALIZED;
        }

        if (!vpuJob->appendCommand(cmd)) {
            LOG_E("Failed to push event wait command to list!");
            return ZE_RESULT_ERROR_UNKNOWN;
        }

        LOG(CMDLIST, "Successfully appended event wait command to CommandList");
    }
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::appendEventReset(ze_event_handle_t hEvent) {
    ze_result_t result = checkCommandAppendCondition();
    if (result != ZE_RESULT_SUCCESS)
        return result;

    auto event = Event::fromHandle(hEvent);
    if (event == nullptr) {
        LOG_E("Failed to get event handle");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    VPU::VPUEventCommand::KMDEventDataType *evSyncPtr = event->getSyncPointer();
    if (evSyncPtr == nullptr) {
        LOG_E("Invalid sync pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    auto cmd = VPU::VPUEventResetCommand::create(ctx, evSyncPtr);
    if (cmd == nullptr) {
        LOG_E("Failed to initialize reset event Command");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(cmd)) {
        LOG_E("Failed to push reset event command to list!");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    LOG(CMDLIST, "Successfully appended reset event command to CommandList");
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::appendMetricQueryBegin(zet_metric_query_handle_t hMetricQuery) {
    if (hMetricQuery == nullptr) {
        LOG_E("MetricQuery handle is NULL");
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
        LOG_E("Failed to initialize metric query begin Command");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(cmd)) {
        LOG_E("Failed to push metric query begin command to list!");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    LOG(CMDLIST, "Successfully appended metric query begin command to CommandList");
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::appendMetricQueryEnd(zet_metric_query_handle_t hMetricQuery,
                                              ze_event_handle_t hSignalEvent,
                                              uint32_t numWaitEvents,
                                              ze_event_handle_t *phWaitEvents) {
    if (hMetricQuery == nullptr) {
        LOG_E("MetricQuery handle is NULL");
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

ze_result_t CommandList::getNextCommandId(const ze_mutable_command_id_exp_desc_t *desc,
                                          uint64_t *pCommandId) {
    if (!isMutable) {
        LOG_E("Command list is not mutable. Unable to get the next command id");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (isCmdListClosed()) {
        LOG_E("Command list is closed. Unable to get the next command id");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (desc->flags != ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENT) {
        LOG_E("Unsupported flag (%#x) in ze_mutable_command_id_exp_desc_t::flags. Only "
              "ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENT is supported",
              desc->flags);
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *pCommandId = getNumCommands();

    return ZE_RESULT_SUCCESS;
}

using CommandUpdatesMap = std::unordered_map<uint64_t, // key: command id
                                             VPU::VPUCommand::ArgumentUpdatesMap>;

static std::optional<const void *>
getCommandUpdates(const void *pNext,
                  const std::unordered_map<uint64_t, uint64_t> &commandIdMap,
                  CommandUpdatesMap &updatesMap) {
    const ze_structure_type_graph_ext_t stype =
        *reinterpret_cast<const ze_structure_type_graph_ext_t *>(pNext);

    switch (stype) {
    case ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC: {
        const ze_mutable_graph_argument_exp_desc_t *desc =
            reinterpret_cast<const ze_mutable_graph_argument_exp_desc_t *>(pNext);
        uint64_t commandId = desc->commandId;
        if (commandIdMap.count(commandId) > 0) {
            commandId = commandIdMap.at(commandId);
        }

        uint32_t argIndex = desc->argIndex;

        if (updatesMap[commandId].count(argIndex) > 0) {
            LOG_W("Argument %u for command %lu is being mutated more than once. "
                  "Verify the values in ze_mutable_graph_argument_exp_desc_t structs",
                  argIndex,
                  commandId);
        }

        updatesMap[commandId][argIndex] = desc->pArgValue;
        LOG(CMDLIST, "Mutate GraphArgument[%u] = %p", argIndex, desc->pArgValue);

        return desc->pNext;
    }
    default: {
        LOG_E("Unsupported descriptor type (%#x) to mutate commands. Only "
              "ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC is supported",
              stype);
        return {};
    }
    }
}

static bool gatherCommandUpdates(const void *pNext,
                                 const std::unordered_map<uint64_t, uint64_t> &commandIdMap,
                                 CommandUpdatesMap &updatesMap) {
    return handleNestedStructs(pNext, getCommandUpdates, commandIdMap, updatesMap);
}

ze_result_t CommandList::updateMutableCommands(const ze_mutable_commands_exp_desc_t *desc) {
    if (!isMutable) {
        LOG_E("Command list is not mutable. Unable to update mutable commands");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (desc->flags != 0) {
        LOG_E("ze_mutable_commands_exp_desc_t::flags must be 0");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    CommandUpdatesMap updatesMap;
    if (!gatherCommandUpdates(desc->pNext, commandIdMap, updatesMap) || updatesMap.size() == 0) {
        LOG_E("Unable to gather command updates. Verify the values in "
              "ze_mutable_commands_exp_desc_t struct");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    for (const auto &[commandId, update] : updatesMap) {
        auto command = vpuJob->getCommand(commandId);
        if (command == nullptr) {
            LOG_E("Unable to get command with id %lu", commandId);
            return ZE_RESULT_ERROR_INVALID_ARGUMENT;
        }

        if (!command->setUpdates(update)) {
            LOG_E("Unable to set updates for command with id %lu and type %#x",
                  commandId,
                  command->getCommandType());
            return ZE_RESULT_ERROR_INVALID_ARGUMENT;
        }
    }

    vpuJob->setNeedsUpdate(true);

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
