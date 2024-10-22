/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/core/source/cmdlist/immediate_cmdlist.hpp"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/include/l0_exception.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

namespace L0 {
ze_result_t zeCommandListAppendBarrier(ze_command_list_handle_t hCommandList,
                                       ze_event_handle_t hSignalEvent,
                                       uint32_t numWaitEvents,
                                       ze_event_handle_t *phWaitEvents) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::CommandList::fromHandle(hCommandList)
                                       ->appendBarrier(hSignalEvent, numWaitEvents, phWaitEvents));
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

ze_result_t zeCommandListCreate(ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                const ze_command_list_desc_t *desc,
                                ze_command_list_handle_t *phCommandList) {
    L0_HANDLE_EXCEPTION_AND_RETURN(CommandList::create(hContext, hDevice, desc, phCommandList));
}

ze_result_t zeCommandListCreateImmediate(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         const ze_command_queue_desc_t *altdesc,
                                         ze_command_list_handle_t *phCommandList) {
    L0_HANDLE_EXCEPTION_AND_RETURN(
        ImmediateCommandList::create(hContext, hDevice, altdesc, phCommandList));
}

ze_result_t zeCommandListImmediateGetIndex(ze_command_list_handle_t hCommandListImmediate,
                                           uint32_t *pIndex) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeCommandListIsImmediate(ze_command_list_handle_t hCommandList,
                                     ze_bool_t *pIsImmediate) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        CommandList::fromHandle(hCommandList)->isImmediate(pIsImmediate));
}

ze_result_t zeCommandListHostSynchronize(ze_command_list_handle_t hCommandList, uint64_t timeout) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        ImmediateCommandList::fromHandle(hCommandList)->hostSynchronize(timeout));
}

ze_result_t zeCommandListDestroy(ze_command_list_handle_t hCommandList) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(CommandList::fromHandle(hCommandList)->destroy());
}

ze_result_t zeCommandListClose(ze_command_list_handle_t hCommandList) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(CommandList::fromHandle(hCommandList)->close());
}

ze_result_t zeCommandListReset(ze_command_list_handle_t hCommandList) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(CommandList::fromHandle(hCommandList)->reset());
}

ze_result_t zeCommandListAppendMemoryCopy(ze_command_list_handle_t hCommandList,
                                          void *dstptr,
                                          const void *srcptr,
                                          size_t size,
                                          ze_event_handle_t hEvent,
                                          uint32_t numWaitEvents,
                                          ze_event_handle_t *phWaitEvents) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::CommandList::fromHandle(hCommandList)
            ->appendMemoryCopy(dstptr, srcptr, size, hEvent, numWaitEvents, phWaitEvents));
}

ze_result_t zeCommandListAppendMemoryFill(ze_command_list_handle_t hCommandList,
                                          void *ptr,
                                          const void *pattern,
                                          size_t patternSize,
                                          size_t size,
                                          ze_event_handle_t hEvent,
                                          uint32_t numWaitEvents,
                                          ze_event_handle_t *phWaitEvents) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(L0::CommandList::fromHandle(hCommandList)
                                       ->appendMemoryFill(ptr,
                                                          pattern,
                                                          patternSize,
                                                          size,
                                                          hEvent,
                                                          numWaitEvents,
                                                          phWaitEvents));
}

ze_result_t zeCommandListAppendMemoryCopyRegion(ze_command_list_handle_t hCommandList,
                                                void *dstptr,
                                                const ze_copy_region_t *dstRegion,
                                                uint32_t dstPitch,
                                                uint32_t dstSlicePitch,
                                                const void *srcptr,
                                                const ze_copy_region_t *srcRegion,
                                                uint32_t srcPitch,
                                                uint32_t srcSlicePitch,
                                                ze_event_handle_t hEvent,
                                                uint32_t numWaitEvents,
                                                ze_event_handle_t *phWaitEvents) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeCommandListAppendImageCopy(ze_command_list_handle_t hCommandList,
                                         ze_image_handle_t hDstImage,
                                         ze_image_handle_t hSrcImage,
                                         ze_event_handle_t hEvent,
                                         uint32_t numWaitEvents,
                                         ze_event_handle_t *phWaitEvents) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeCommandListAppendImageCopyRegion(ze_command_list_handle_t hCommandList,
                                               ze_image_handle_t hDstImage,
                                               ze_image_handle_t hSrcImage,
                                               const ze_image_region_t *pDstRegion,
                                               const ze_image_region_t *pSrcRegion,
                                               ze_event_handle_t hEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeCommandListAppendImageCopyToMemory(ze_command_list_handle_t hCommandList,
                                                 void *dstptr,
                                                 ze_image_handle_t hSrcImage,
                                                 const ze_image_region_t *pSrcRegion,
                                                 ze_event_handle_t hEvent,
                                                 uint32_t numWaitEvents,
                                                 ze_event_handle_t *phWaitEvents) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeCommandListAppendImageCopyFromMemory(ze_command_list_handle_t hCommandList,
                                                   ze_image_handle_t hDstImage,
                                                   const void *srcptr,
                                                   const ze_image_region_t *pDstRegion,
                                                   ze_event_handle_t hEvent,
                                                   uint32_t numWaitEvents,
                                                   ze_event_handle_t *phWaitEvents) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeCommandListAppendMemoryPrefetch(ze_command_list_handle_t hCommandList,
                                              const void *ptr,
                                              size_t size) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeCommandListAppendMemAdvise(ze_command_list_handle_t hCommandList,
                                         ze_device_handle_t hDevice,
                                         const void *ptr,
                                         size_t size,
                                         ze_memory_advice_t advice) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeCommandListAppendMemoryCopyFromContext(ze_command_list_handle_t hCommandList,
                                                     void *dstptr,
                                                     ze_context_handle_t hContextSrc,
                                                     const void *srcptr,
                                                     size_t size,
                                                     ze_event_handle_t hSignalEvent,
                                                     uint32_t numWaitEvents,
                                                     ze_event_handle_t *phWaitEvents) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeCommandListAppendWriteGlobalTimestamp(ze_command_list_handle_t hCommandList,
                                                    uint64_t *dstptr,
                                                    ze_event_handle_t hSignalEvent,
                                                    uint32_t numWaitEvents,
                                                    ze_event_handle_t *phWaitEvents) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        CommandList::fromHandle(hCommandList)
            ->appendWriteGlobalTimestamp(dstptr, hSignalEvent, numWaitEvents, phWaitEvents));
}

ze_result_t zeCommandListAppendQueryKernelTimestamps(ze_command_list_handle_t hCommandList,
                                                     uint32_t numEvents,
                                                     ze_event_handle_t *phEvents,
                                                     void *dstptr,
                                                     const size_t *pOffsets,
                                                     ze_event_handle_t hSignalEvent,
                                                     uint32_t numWaitEvents,
                                                     ze_event_handle_t *phWaitEvents) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t zeCommandListGetNextCommandIdExp(ze_command_list_handle_t hCommandList,
                                             const ze_mutable_command_id_exp_desc_t *desc,
                                             uint64_t *pCommandId) {
    L0_HANDLE_EXCEPTION_AND_RETURN(
        CommandList::fromHandle(hCommandList)->getNextCommandId(desc, pCommandId));
}

ze_result_t zeCommandListUpdateMutableCommandsExp(ze_command_list_handle_t hCommandList,
                                                  const ze_mutable_commands_exp_desc_t *desc) {
    L0_HANDLE_EXCEPTION_AND_RETURN(
        CommandList::fromHandle(hCommandList)->updateMutableCommands(desc));
}

ze_result_t zeCommandListAppendSignalEvent(ze_command_list_handle_t hCommandList,
                                           ze_event_handle_t hEvent) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(
        CommandList::fromHandle(hCommandList)->CommandList::appendSignalEvent(hEvent));
}

ze_result_t zeCommandListAppendWaitOnEvents(ze_command_list_handle_t hCommandList,
                                            uint32_t numEvents,
                                            ze_event_handle_t *phEvents) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(CommandList::fromHandle(hCommandList)
                                       ->CommandList::appendWaitOnEvents(numEvents, phEvents));
}

ze_result_t zeCommandListAppendEventReset(ze_command_list_handle_t hCommandList,
                                          ze_event_handle_t hEvent) {
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }
    L0_HANDLE_EXCEPTION_AND_RETURN(CommandList::fromHandle(hCommandList)->appendEventReset(hEvent));
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetCommandListProcAddrTable(ze_api_version_t version, ze_command_list_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnAppendBarrier = L0::zeCommandListAppendBarrier;
    pDdiTable->pfnAppendMemoryRangesBarrier = L0::zeCommandListAppendMemoryRangesBarrier;
    pDdiTable->pfnCreate = L0::zeCommandListCreate;
    pDdiTable->pfnCreateImmediate = L0::zeCommandListCreateImmediate;
    pDdiTable->pfnImmediateGetIndex = L0::zeCommandListImmediateGetIndex;
    pDdiTable->pfnIsImmediate = L0::zeCommandListIsImmediate;
    pDdiTable->pfnHostSynchronize = L0::zeCommandListHostSynchronize;
    pDdiTable->pfnDestroy = L0::zeCommandListDestroy;
    pDdiTable->pfnClose = L0::zeCommandListClose;
    pDdiTable->pfnReset = L0::zeCommandListReset;
    pDdiTable->pfnAppendMemoryCopy = L0::zeCommandListAppendMemoryCopy;
    pDdiTable->pfnAppendMemoryCopyRegion = L0::zeCommandListAppendMemoryCopyRegion;
    pDdiTable->pfnAppendMemoryFill = L0::zeCommandListAppendMemoryFill;
    pDdiTable->pfnAppendImageCopy = L0::zeCommandListAppendImageCopy;
    pDdiTable->pfnAppendImageCopyRegion = L0::zeCommandListAppendImageCopyRegion;
    pDdiTable->pfnAppendImageCopyToMemory = L0::zeCommandListAppendImageCopyToMemory;
    pDdiTable->pfnAppendImageCopyFromMemory = L0::zeCommandListAppendImageCopyFromMemory;
    pDdiTable->pfnAppendMemoryPrefetch = L0::zeCommandListAppendMemoryPrefetch;
    pDdiTable->pfnAppendMemAdvise = L0::zeCommandListAppendMemAdvise;
    pDdiTable->pfnAppendSignalEvent = L0::zeCommandListAppendSignalEvent;
    pDdiTable->pfnAppendWaitOnEvents = L0::zeCommandListAppendWaitOnEvents;
    pDdiTable->pfnAppendEventReset = L0::zeCommandListAppendEventReset;
    pDdiTable->pfnAppendLaunchCooperativeKernel =
        nullptr; // zeCommandListAppendLaunchCooperativeKernel
    pDdiTable->pfnAppendLaunchKernelIndirect = nullptr; // zeCommandListAppendLaunchKernelIndirect
    pDdiTable->pfnAppendLaunchMultipleKernelsIndirect =
        nullptr; // zeCommandListAppendLaunchMultipleKernelsIndirect
    pDdiTable->pfnAppendWriteGlobalTimestamp = L0::zeCommandListAppendWriteGlobalTimestamp;
    pDdiTable->pfnAppendMemoryCopyFromContext = L0::zeCommandListAppendMemoryCopyFromContext;
    pDdiTable->pfnAppendQueryKernelTimestamps = L0::zeCommandListAppendQueryKernelTimestamps;
    return ZE_RESULT_SUCCESS;
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zeGetCommandListExpProcAddrTable(ze_api_version_t version,
                                 ze_command_list_exp_dditable_t *pDdiTable) {
    if (nullptr == pDdiTable)
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version))
        return ZE_RESULT_ERROR_UNSUPPORTED_VERSION;

    pDdiTable->pfnGetNextCommandIdExp = L0::zeCommandListGetNextCommandIdExp;
    pDdiTable->pfnUpdateMutableCommandsExp = L0::zeCommandListUpdateMutableCommandsExp;

    return ZE_RESULT_SUCCESS;
}
}
