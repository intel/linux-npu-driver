/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "cmdlist.hpp"

#include "context.hpp"
#include "device.hpp"
#include "ext/graph.hpp"
#include "ext/profiling_data.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/include/nested_structs_handler.hpp"
#include "metric_query.hpp"
#include "vpu_driver/source/command/barrier_command.hpp"
#include "vpu_driver/source/command/command.hpp"
#include "vpu_driver/source/command/copy_command.hpp"
#include "vpu_driver/source/command/fill_command.hpp"
#include "vpu_driver/source/command/query_command.hpp"
#include "vpu_driver/source/command/ts_command.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <algorithm>
#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_ext.h>
#include <optional>
#include <string.h>
#include <type_traits>

namespace L0 {

CommandList::CommandList(Context *pContext, bool isMutable)
    : pContext(pContext)
    , isMutable(isMutable)
    , ctx(pContext->getDeviceContext())
    , vpuJob(std::make_shared<VPU::VPUJob>(ctx)) {}

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
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (phCommandList == nullptr) {
        LOG_E("Invalid phCommandList pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    try {
        Device *pDevice = Device::fromHandle(hDevice);
        ze_command_queue_group_property_flags_t flags =
            pDevice->getCommandQeueueGroupFlags(desc->commandQueueGroupOrdinal);
        L0_THROW_WHEN(flags == 0, "Invalid group ordinal", ZE_RESULT_ERROR_INVALID_ARGUMENT);

        Context *pContext = Context::fromHandle(hContext);
        bool isMutable = false;
        if (desc->pNext != nullptr) {
            const ze_structure_type_t stype =
                *reinterpret_cast<const ze_structure_type_t *>(desc->pNext);
            isMutable = stype == ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_LIST_EXP_DESC;
        }
        auto commandList = std::make_unique<CommandList>(pContext, isMutable);

        *phCommandList = commandList.get();
        pContext->appendObject(std::move(commandList));

        LOG(CMDLIST, "CommandList created - %p", *phCommandList);
    } catch (const DriverError &err) {
        return err.result();
    }
    return ZE_RESULT_SUCCESS;
}

ze_result_t CommandList::isImmediate(ze_bool_t *pIsImmediate) {
    if (pIsImmediate == nullptr) {
        LOG_E("Invalid data pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    *pIsImmediate = false;
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
    vpuJob = std::make_shared<VPU::VPUJob>(ctx);
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
    auto srcBo = ctx->findBufferObject(srcptr);
    auto dstBo = ctx->findBufferObject(dstptr);
    return appendCommandWithEvents<VPU::VPUCopyCommand>(hSignalEvent,
                                                        numWaitEvents,
                                                        phWaitEvents,
                                                        ctx,
                                                        srcptr,
                                                        std::move(srcBo),
                                                        dstptr,
                                                        std::move(dstBo),
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

    if (ctx->getDeviceCapabilities().npuArch == VPU::NPU40XX)
        return appendMemoryFillCmd(ptr,
                                   pattern,
                                   patternSize,
                                   size,
                                   hSignalEvent,
                                   numWaitEvents,
                                   phWaitEvents);
    else
        return appendMemoryFillAsCopyCmd(ptr,
                                         pattern,
                                         patternSize,
                                         size,
                                         hSignalEvent,
                                         numWaitEvents,
                                         phWaitEvents);
}

ze_result_t CommandList::appendMemoryFillCmd(void *ptr,
                                             const void *pattern,
                                             size_t patternSize,
                                             size_t size,
                                             ze_event_handle_t hSignalEvent,
                                             uint32_t numWaitEvents,
                                             ze_event_handle_t *phWaitEvents) {
    uint32_t fill_pattern = 0;
    switch (patternSize) {
    case sizeof(uint32_t):
        fill_pattern = *reinterpret_cast<const uint32_t *>(pattern);
        break;

    case sizeof(uint16_t):
        fill_pattern = *reinterpret_cast<const uint16_t *>(pattern);
        fill_pattern |= fill_pattern << 16;
        break;

    case sizeof(uint8_t):
        memset(reinterpret_cast<uint8_t *>(&fill_pattern),
               *static_cast<const uint8_t *>(pattern),
               sizeof(fill_pattern));
        break;
    default:
        LOG_E("Unsupported pattern size");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    auto ptrBo = ctx->findBufferObject(ptr);
    if (ptrBo == nullptr) {
        LOG_E("Buffer object not found");
        return ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    // Because fill operation can not be interrupted
    // for efficiency reason we limit single operation to 8 MB
    // If size is larger than FILL_SIZE_LIMIT, split it into multiple operations
    static constexpr size_t FILL_SIZE_LIMIT = (8 << 20);

    size_t sizeLeft = size;
    size_t offset = 0;
    ze_result_t result = ZE_RESULT_SUCCESS;
    while (sizeLeft > 0) {
        size_t fillSize = std::min(sizeLeft, FILL_SIZE_LIMIT);
        result = appendCommandWithEvents<VPU::VPUFillCommand>(
            sizeLeft <= FILL_SIZE_LIMIT ? hSignalEvent : nullptr,
            numWaitEvents,
            phWaitEvents,
            static_cast<uint8_t *>(ptr) + offset,
            ptrBo,
            fillSize,
            fill_pattern);
        if (result != ZE_RESULT_SUCCESS) {
            LOG_E("Failed to append fill command to list");
            break;
        }
        sizeLeft -= fillSize;
        offset += fillSize;
        numWaitEvents = 0;
        phWaitEvents = nullptr;
    }
    return result;
}

ze_result_t CommandList::appendMemoryFillAsCopyCmd(void *ptr,
                                                   const void *pattern,
                                                   size_t patternSize,
                                                   size_t size,
                                                   ze_event_handle_t hSignalEvent,
                                                   uint32_t numWaitEvents,
                                                   ze_event_handle_t *phWaitEvents) {
    ze_result_t result = checkCommandAppendCondition();
    if (result != ZE_RESULT_SUCCESS)
        return result;

    auto patternBo =
        ctx->createUntrackedBufferObject(size + patternSize, VPU::VPUBufferObject::Type::CachedDma);
    if (patternBo == nullptr) {
        LOG_E("Failed to allocate memory");
        return ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    if (!patternBo->fillBuffer(pattern, patternSize)) {
        LOG_E("Failed to fill memory");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }
    // Implement fill by memory copy from internal filled buffer to user buffer.
    auto fillBo = ctx->findBufferObject(ptr);
    return appendCommandWithEvents<VPU::VPUCopyCommand>(hSignalEvent,
                                                        numWaitEvents,
                                                        phWaitEvents,
                                                        ctx,
                                                        patternBo->getBasePointer(),
                                                        patternBo,
                                                        ptr,
                                                        std::move(fillBo),
                                                        size);
}

ze_result_t CommandList::appendMemoryCopyRegion(void *dstptr,
                                                const ze_copy_region_t *dstRegion,
                                                uint32_t dstPitch,
                                                uint32_t dstSlicePitch,
                                                const void *srcptr,
                                                const ze_copy_region_t *srcRegion,
                                                uint32_t srcPitch,
                                                uint32_t srcSlicePitch,
                                                ze_event_handle_t hSignalEvent,
                                                uint32_t numWaitEvents,
                                                ze_event_handle_t *phWaitEvents) {
    ze_result_t result = ZE_RESULT_SUCCESS;

    if ((dstptr == nullptr) || (srcptr == nullptr)) {
        LOG_E("Source or destination pointer is nullptr");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if ((dstRegion == nullptr) || (srcRegion == nullptr)) {
        LOG_E("The pointer to the source or destination region is nullptr");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if ((dstRegion->width != srcRegion->width) || (dstRegion->height != srcRegion->height) ||
        (dstRegion->depth != srcRegion->depth)) {
        LOG_E("Incorrect region dimensions");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    if (dstRegion->width == 0) {
        LOG_E("Invalid value for the region width");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    if (dstPitch < dstRegion->width) {
        LOG_E("Invalid value for the destination pitch");
        return ZE_RESULT_ERROR_INVALID_SIZE;
    }

    // Only support 2D copies for now
    if ((dstSlicePitch != 0) || (srcSlicePitch != 0)) {
        LOG_E("Slice pitch value other than 0 is not supported");
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
    }

    if ((dstRegion->depth != 1) || (srcRegion->depth != 1)) {
        LOG_E("The region depth value other than 1 is not supported");
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
    }

    if ((dstRegion->originZ != 0) || (srcRegion->originZ != 0)) {
        LOG_E("The origin z offset other than 0 is not supported");
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
    }

    for (uint32_t y = 0; y < dstRegion->height; y++) {
        uint32_t srcOffset = srcRegion->originX + (srcRegion->originY + y) * srcPitch;
        uint32_t dstOffset = dstRegion->originX + (dstRegion->originY + y) * dstPitch;

        auto srcBo = ctx->findBufferObject(static_cast<const uint8_t *>(srcptr) + srcOffset);
        auto dstBo = ctx->findBufferObject(static_cast<uint8_t *>(dstptr) + dstOffset);

        result = appendCommandWithEvents<VPU::VPUCopyCommand>(
            hSignalEvent,
            numWaitEvents,
            phWaitEvents,
            ctx,
            static_cast<const uint8_t *>(srcptr) + srcOffset,
            std::move(srcBo),
            static_cast<uint8_t *>(dstptr) + dstOffset,
            std::move(dstBo),
            dstRegion->width);
        if (result != ZE_RESULT_SUCCESS) {
            LOG_E("Failed to append copy command to list");
            break;
        }
    }

    return result;
}

ze_result_t
CommandList::appendWriteGlobalTimestamp(std::shared_ptr<VPU::VPUBufferObject> timestampBo,
                                        ze_event_handle_t hSignalEvent,
                                        uint32_t numWaitEvents,
                                        ze_event_handle_t *phWaitEvents) {
    return appendCommandWithEvents<VPU::VPUTimeStampCommand>(
        hSignalEvent,
        numWaitEvents,
        phWaitEvents,
        reinterpret_cast<uint64_t *>(timestampBo->getBasePointer()),
        timestampBo,
        ctx->getFwTimestampType());
}

ze_result_t CommandList::appendWriteGlobalTimestamp(uint64_t *dstptr,
                                                    ze_event_handle_t hSignalEvent,
                                                    uint32_t numWaitEvents,
                                                    ze_event_handle_t *phWaitEvents) {
    if (dstptr == nullptr) {
        LOG_E("dstptr is NULL");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto dstBo = ctx->findBufferObject(dstptr);
    if (dstBo == nullptr) {
        LOG_E("Buffer object not found");
        return ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY;
    }

    ze_result_t result = checkCommandAppendCondition();
    if (result != ZE_RESULT_SUCCESS)
        return result;

    auto alignedBo =
        ctx->createUntrackedBufferObject(sizeof(uint64_t), VPU::VPUBufferObject::Type::CachedFw);

    if (alignedBo == nullptr) {
        LOG_E("Failed to allocate memory");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    memset(alignedBo->getBasePointer(), 0, sizeof(uint64_t));

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

    result = appendCommand<VPU::VPUTimeStampCommand>(
        reinterpret_cast<uint64_t *>(alignedBo->getBasePointer()),
        alignedBo,
        ctx->getFwTimestampType());

    if (result != ZE_RESULT_SUCCESS)
        return result;

    result = appendCommand<VPU::VPUCopyCommand>(ctx,
                                                alignedBo->getBasePointer(),
                                                alignedBo,
                                                dstptr,
                                                std::move(dstBo),
                                                sizeof(uint64_t));
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

    return postAppend();
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
    return postAppend();
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

    GraphProfilingQuery *profilingQuery = nullptr;
    if (graph->getProfilingOutputSize()) {
        profilingQuery = GraphProfilingQuery::fromHandle(hProfilingQuery);
        if (!profilingQuery) {
            LOG_E("Invalid profiling query handle");
            return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        }
    }

    auto cmd = graph->allocateGraphExecuteCommand(profilingQuery);
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

    LOG(CMDLIST, "Successfully appended graph execute command to CommandList");
    return postAppend();
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

    result = appendCommand<VPU::VPUEventSignalCommand>(evSyncPtr, event->getAssociatedBo());
    if (result != ZE_RESULT_SUCCESS)
        return result;

    event->associateJob(vpuJob);
    LOG(CMDLIST, "Successfully appended signal event command to CommandList");
    return postAppend();
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

        result = appendCommand<VPU::VPUEventWaitCommand>(evSyncPtr, event->getAssociatedBo());
        if (result != ZE_RESULT_SUCCESS)
            return result;

        LOG(CMDLIST, "Successfully appended event wait command to CommandList");
    }
    return postAppend();
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

    auto cmd = VPU::VPUEventResetCommand::create(evSyncPtr, event->getAssociatedBo());
    if (cmd == nullptr) {
        LOG_E("Failed to initialize reset event Command");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(cmd)) {
        LOG_E("Failed to push reset event command to list!");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    LOG(CMDLIST, "Successfully appended reset event command to CommandList");
    return postAppend();
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

    auto cmd = VPU::VPUQueryBeginCommand::create(metricQuery->getMetricGroupMask(),
                                                 metricQuery->getMetricAddrPtr(),
                                                 metricQuery->getBo());
    if (cmd == nullptr) {
        LOG_E("Failed to initialize metric query begin Command");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (!vpuJob->appendCommand(cmd)) {
        LOG_E("Failed to push metric query begin command to list!");
        return ZE_RESULT_ERROR_UNKNOWN;
    }

    LOG(CMDLIST, "Successfully appended metric query begin command to CommandList");
    return postAppend();
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
                                                            metricQuery->getMetricGroupMask(),
                                                            metricQuery->getMetricAddrPtr(),
                                                            metricQuery->getBo());
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

    switch (desc->flags) {
    case ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENT_DEPRECATED:
    case ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS:
        break;
    default:
        LOG_E("Unsupported flag (%#x) in ze_mutable_command_id_exp_desc_t::flags. Only "
              "ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS is supported",
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
    const auto stype =
        *reinterpret_cast<const std::underlying_type_t<ze_structure_type_t> *>(pNext);

    switch (stype) {
    case ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC:
    case ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC_DEPRECATED: {
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

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
