/*
 * Copyright (C) 2024-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "immediate_cmdlist.hpp"

#include "cmdlist.hpp"
#include "cmdqueue.hpp"
#include "context.hpp"
#include "event.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/include/l0_handler.hpp"
#include "level_zero_driver/source/device.hpp"
#include "vpu_driver/source/command/event_command.hpp"
#include "vpu_driver/source/command/job.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <limits>
#include <memory>
#include <utility>
#include <ze_api.h>

namespace L0 {
ImmediateCommandList::ImmediateCommandList(Context *pCtx,
                                           ze_device_handle_t hDevice,
                                           uint32_t ordinal,
                                           uint32_t index,
                                           ze_command_queue_flags_t flags,
                                           ze_command_queue_mode_t mode,
                                           ze_command_queue_priority_t priority,
                                           CommandQueue *pCmdQueue)
    // Immediate lists have no ze_command_list_desc_t, so no list creation flags apply.
    : CommandList(pCtx, hDevice, ordinal, 0, false)
    , index(index)
    , flags(flags)
    , mode(mode)
    , priority(priority)
    , pCommandQueue(pCmdQueue) {}

ze_result_t ImmediateCommandList::create(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         const ze_command_queue_desc_t *altdesc,
                                         ze_command_list_handle_t *phCommandList) {
    if (hContext == nullptr) {
        LOG_E("Invalid context handler ");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    if (hDevice == nullptr) {
        LOG_E("Invalid device handler");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    if (altdesc == nullptr) {
        LOG_E("Invalid command queue pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (phCommandList == nullptr) {
        LOG_E("Invalid command list pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    try {
        ze_result_t result;
        ze_command_queue_handle_t hCommandQueue = nullptr;
        result = CommandQueue::create(hContext, hDevice, altdesc, &hCommandQueue);
        if (result != ZE_RESULT_SUCCESS) {
            LOG_E("Creation command queue failed");
            return result;
        }

        auto pContext = Context::fromHandle(hContext);
        auto pCmdq = CommandQueue::fromHandle(hCommandQueue);
        auto commandList = std::make_unique<ImmediateCommandList>(pContext,
                                                                  hDevice,
                                                                  altdesc->ordinal,
                                                                  altdesc->index,
                                                                  altdesc->flags,
                                                                  altdesc->mode,
                                                                  altdesc->priority,
                                                                  pCmdq);

        *phCommandList = commandList.get();
        pContext->appendObject(std::move(commandList));

        LOG(CMDLIST, "CommandList created - %p", *phCommandList);
    } catch (const DriverError &err) {
        return err.result();
    }
    return ZE_RESULT_SUCCESS;
}

ze_result_t ImmediateCommandList::destroy() {
    if (pCommandQueue) {
        pCommandQueue->destroy();
    }
    return CommandList::destroy();
}

ze_result_t ImmediateCommandList::isImmediate(ze_bool_t *pIsImmediate) {
    if (pIsImmediate == nullptr) {
        LOG_E("Invalid data pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    *pIsImmediate = true;
    return ZE_RESULT_SUCCESS;
}

ze_result_t ImmediateCommandList::getIndex(uint32_t *pIndex) {
    if (pIndex == nullptr) {
        LOG_E("Invalid data pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    *pIndex = index;
    return ZE_RESULT_SUCCESS;
}

ze_result_t ImmediateCommandList::getImmediateFlags(ze_command_queue_flags_t *pFlags) {
    if (pFlags == nullptr) {
        LOG_E("Invalid data pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    *pFlags = flags;
    return ZE_RESULT_SUCCESS;
}

ze_result_t ImmediateCommandList::getImmediateMode(ze_command_queue_mode_t *pMode) {
    if (pMode == nullptr) {
        LOG_E("Invalid data pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    *pMode = mode;
    return ZE_RESULT_SUCCESS;
}

ze_result_t ImmediateCommandList::getImmediatePriority(ze_command_queue_priority_t *pPriority) {
    if (pPriority == nullptr) {
        LOG_E("Invalid data pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    *pPriority = priority;
    return ZE_RESULT_SUCCESS;
}

ze_result_t ImmediateCommandList::checkCommandAppendCondition() {
    ze_result_t result;

    result = pCommandQueue->synchronize(std::numeric_limits<uint64_t>::max());

    if (!vpuJob || vpuJob->isClosed()) {
        reset();
    }
    return result;
}

ze_result_t ImmediateCommandList::hostSynchronize(uint64_t timeout) {
    ze_result_t result = pCommandQueue->synchronize(timeout);
    if (result == ZE_RESULT_SUCCESS) {
        reset();
    }
    return result;
}

ze_result_t ImmediateCommandList::appendSignalEvent(ze_event_handle_t hEvent) {
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

    ze_result_t result =
        appendCommand<VPU::VPUEventSignalCommand>(evSyncPtr, ctx->findBufferObject(evSyncPtr));

    if (result != ZE_RESULT_SUCCESS)
        return result;

    event->associateJob(vpuJob);
    LOG(CMDLIST, "Successfully appended signal event command to CommandList");
    return ZE_RESULT_SUCCESS;
}

ze_result_t ImmediateCommandList::appendWaitOnEvents(uint32_t numEvents,
                                                     ze_event_handle_t *phEvent) {
    if (phEvent == nullptr)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    if (numEvents == 0u)
        return ZE_RESULT_ERROR_INVALID_SIZE;

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

        ze_result_t result =
            appendCommand<VPU::VPUEventWaitCommand>(evSyncPtr, ctx->findBufferObject(evSyncPtr));

        if (result != ZE_RESULT_SUCCESS)
            return result;

        LOG(CMDLIST, "Successfully appended event wait command to CommandList");
    }
    return ZE_RESULT_SUCCESS;
}

ze_result_t ImmediateCommandList::postAppend() {
    if (pCommandQueue) {
        ze_command_list_handle_t thisList = this;
        ze_result_t result;

        if (!vpuJob->closeCommands()) {
            LOG_E("Failed to close commands on immediate command list");
            return ZE_RESULT_ERROR_INVALID_SYNCHRONIZATION_OBJECT;
        }
        result = pCommandQueue->executeCommandLists(1, &thisList, nullptr);
        if (result != ZE_RESULT_SUCCESS) {
            LOG_E("Immediate command list execution failed");
            return result;
        }
        vpuJob = std::make_shared<VPU::VPUJob>(ctx);
    }
    return ZE_RESULT_SUCCESS;
}

} // namespace L0
