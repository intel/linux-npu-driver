/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/api/trace/trace_ze_api.hpp"
#include "level_zero_driver/api/trace/trace_ze_api_ddi.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/cmdlist.hpp"
#include "level_zero_driver/source/context.hpp"
#include "level_zero_driver/source/immediate_cmdlist.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_ddi.h>

namespace L0 {
ze_result_t zeCommandListAppendBarrier(ze_command_list_handle_t hCommandList,
                                       ze_event_handle_t hSignalEvent,
                                       uint32_t numWaitEvents,
                                       ze_event_handle_t *phWaitEvents) {
    trace_zeCommandListAppendBarrier(hCommandList, hSignalEvent, numWaitEvents, phWaitEvents);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret,
                        L0::CommandList::fromHandle(hCommandList)
                            ->appendBarrier(hSignalEvent, numWaitEvents, phWaitEvents));

exit:
    trace_zeCommandListAppendBarrier(ret, hCommandList, hSignalEvent, numWaitEvents, phWaitEvents);
    return ret;
}

ze_result_t zeCommandListAppendMemoryRangesBarrier(ze_command_list_handle_t hCommandList,
                                                   uint32_t numRanges,
                                                   const size_t *pRangeSizes,
                                                   const void **pRanges,
                                                   ze_event_handle_t hSignalEvent,
                                                   uint32_t numWaitEvents,
                                                   ze_event_handle_t *phWaitEvents) {
    trace_zeCommandListAppendMemoryRangesBarrier(hCommandList,
                                                 numRanges,
                                                 pRangeSizes,
                                                 pRanges,
                                                 hSignalEvent,
                                                 numWaitEvents,
                                                 phWaitEvents);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeCommandListAppendMemoryRangesBarrier(ret,
                                                 hCommandList,
                                                 numRanges,
                                                 pRangeSizes,
                                                 pRanges,
                                                 hSignalEvent,
                                                 numWaitEvents,
                                                 phWaitEvents);
    return ret;
}

ze_result_t zeCommandListCreate(ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                const ze_command_list_desc_t *desc,
                                ze_command_list_handle_t *phCommandList) {
    trace_zeCommandListCreate(hContext, hDevice, desc, phCommandList);
    ze_result_t ret;

    L0_HANDLE_EXCEPTION(ret, CommandList::create(hContext, hDevice, desc, phCommandList));

    trace_zeCommandListCreate(ret, hContext, hDevice, desc, phCommandList);
    return ret;
}

ze_result_t zeCommandListCreateImmediate(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         const ze_command_queue_desc_t *altdesc,
                                         ze_command_list_handle_t *phCommandList) {
    trace_zeCommandListCreateImmediate(hContext, hDevice, altdesc, phCommandList);
    ze_result_t ret;

    L0_HANDLE_EXCEPTION(ret,
                        ImmediateCommandList::create(hContext, hDevice, altdesc, phCommandList));

    trace_zeCommandListCreateImmediate(hContext, hDevice, altdesc, phCommandList);
    return ret;
}

ze_result_t zeCommandListImmediateGetIndex(ze_command_list_handle_t hCommandListImmediate,
                                           uint32_t *pIndex) {
    trace_zeCommandListImmediateGetIndex(hCommandListImmediate, pIndex);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeCommandListImmediateGetIndex(ret, hCommandListImmediate, pIndex);
    return ret;
}

ze_result_t zeCommandListIsImmediate(ze_command_list_handle_t hCommandList,
                                     ze_bool_t *pIsImmediate) {
    trace_zeCommandListIsImmediate(hCommandList, pIsImmediate);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, CommandList::fromHandle(hCommandList)->isImmediate(pIsImmediate));

exit:
    trace_zeCommandListIsImmediate(ret, hCommandList, pIsImmediate);
    return ret;
}

ze_result_t zeCommandListHostSynchronize(ze_command_list_handle_t hCommandList, uint64_t timeout) {
    trace_zeCommandListHostSynchronize(hCommandList, timeout);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        ImmediateCommandList::fromHandle(hCommandList)->hostSynchronize(timeout));

exit:
    trace_zeCommandListHostSynchronize(ret, hCommandList, timeout);
    return ret;
}

ze_result_t zeCommandListDestroy(ze_command_list_handle_t hCommandList) {
    trace_zeCommandListDestroy(hCommandList);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, CommandList::fromHandle(hCommandList)->destroy());

exit:
    trace_zeCommandListDestroy(ret, hCommandList);
    return ret;
}

ze_result_t zeCommandListClose(ze_command_list_handle_t hCommandList) {
    trace_zeCommandListClose(hCommandList);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, CommandList::fromHandle(hCommandList)->close());

exit:
    trace_zeCommandListClose(ret, hCommandList);
    return ret;
}

ze_result_t zeCommandListReset(ze_command_list_handle_t hCommandList) {
    trace_zeCommandListReset(hCommandList);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, CommandList::fromHandle(hCommandList)->reset());

exit:
    trace_zeCommandListReset(ret, hCommandList);
    return ret;
}

ze_result_t zeCommandListAppendMemoryCopy(ze_command_list_handle_t hCommandList,
                                          void *dstptr,
                                          const void *srcptr,
                                          size_t size,
                                          ze_event_handle_t hEvent,
                                          uint32_t numWaitEvents,
                                          ze_event_handle_t *phWaitEvents) {
    trace_zeCommandListAppendMemoryCopy(hCommandList,
                                        dstptr,
                                        srcptr,
                                        size,
                                        hEvent,
                                        numWaitEvents,
                                        phWaitEvents);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        L0::CommandList::fromHandle(hCommandList)
            ->appendMemoryCopy(dstptr, srcptr, size, hEvent, numWaitEvents, phWaitEvents));

exit:
    trace_zeCommandListAppendMemoryCopy(ret,
                                        hCommandList,
                                        dstptr,
                                        srcptr,
                                        size,
                                        hEvent,
                                        numWaitEvents,
                                        phWaitEvents);
    return ret;
}

ze_result_t zeCommandListAppendMemoryFill(ze_command_list_handle_t hCommandList,
                                          void *ptr,
                                          const void *pattern,
                                          size_t patternSize,
                                          size_t size,
                                          ze_event_handle_t hEvent,
                                          uint32_t numWaitEvents,
                                          ze_event_handle_t *phWaitEvents) {
    trace_zeCommandListAppendMemoryFill(hCommandList,
                                        ptr,
                                        pattern,
                                        patternSize,
                                        size,
                                        hEvent,
                                        numWaitEvents,
                                        phWaitEvents);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        L0::CommandList::fromHandle(hCommandList)
                            ->appendMemoryFill(ptr,
                                               pattern,
                                               patternSize,
                                               size,
                                               hEvent,
                                               numWaitEvents,
                                               phWaitEvents));

exit:
    trace_zeCommandListAppendMemoryFill(ret,
                                        hCommandList,
                                        ptr,
                                        pattern,
                                        patternSize,
                                        size,
                                        hEvent,
                                        numWaitEvents,
                                        phWaitEvents);
    return ret;
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
    trace_zeCommandListAppendMemoryCopyRegion(hCommandList,
                                              dstptr,
                                              dstRegion,
                                              dstPitch,
                                              dstSlicePitch,
                                              srcptr,
                                              srcRegion,
                                              srcPitch,
                                              srcSlicePitch,
                                              hEvent,
                                              numWaitEvents,
                                              phWaitEvents);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeCommandListAppendMemoryCopyRegion(ret,
                                              hCommandList,
                                              dstptr,
                                              dstRegion,
                                              dstPitch,
                                              dstSlicePitch,
                                              srcptr,
                                              srcRegion,
                                              srcPitch,
                                              srcSlicePitch,
                                              hEvent,
                                              numWaitEvents,
                                              phWaitEvents);
    return ret;
}

ze_result_t zeCommandListAppendImageCopy(ze_command_list_handle_t hCommandList,
                                         ze_image_handle_t hDstImage,
                                         ze_image_handle_t hSrcImage,
                                         ze_event_handle_t hEvent,
                                         uint32_t numWaitEvents,
                                         ze_event_handle_t *phWaitEvents) {
    trace_zeCommandListAppendImageCopy(hCommandList,
                                       hDstImage,
                                       hSrcImage,
                                       hEvent,
                                       numWaitEvents,
                                       phWaitEvents);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeCommandListAppendImageCopy(ret,
                                       hCommandList,
                                       hDstImage,
                                       hSrcImage,
                                       hEvent,
                                       numWaitEvents,
                                       phWaitEvents);
    return ret;
}

ze_result_t zeCommandListAppendImageCopyRegion(ze_command_list_handle_t hCommandList,
                                               ze_image_handle_t hDstImage,
                                               ze_image_handle_t hSrcImage,
                                               const ze_image_region_t *pDstRegion,
                                               const ze_image_region_t *pSrcRegion,
                                               ze_event_handle_t hEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
    trace_zeCommandListAppendImageCopy(hCommandList,
                                       hDstImage,
                                       hSrcImage,
                                       hEvent,
                                       numWaitEvents,
                                       phWaitEvents);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeCommandListAppendImageCopy(ret,
                                       hCommandList,
                                       hDstImage,
                                       hSrcImage,
                                       hEvent,
                                       numWaitEvents,
                                       phWaitEvents);
    return ret;
}

ze_result_t zeCommandListAppendImageCopyToMemory(ze_command_list_handle_t hCommandList,
                                                 void *dstptr,
                                                 ze_image_handle_t hSrcImage,
                                                 const ze_image_region_t *pSrcRegion,
                                                 ze_event_handle_t hEvent,
                                                 uint32_t numWaitEvents,
                                                 ze_event_handle_t *phWaitEvents) {
    trace_zeCommandListAppendImageCopyToMemory(hCommandList,
                                               dstptr,
                                               hSrcImage,
                                               pSrcRegion,
                                               hEvent,
                                               numWaitEvents,
                                               phWaitEvents);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeCommandListAppendImageCopyToMemory(ret,
                                               hCommandList,
                                               dstptr,
                                               hSrcImage,
                                               pSrcRegion,
                                               hEvent,
                                               numWaitEvents,
                                               phWaitEvents);
    return ret;
}

ze_result_t zeCommandListAppendImageCopyFromMemory(ze_command_list_handle_t hCommandList,
                                                   ze_image_handle_t hDstImage,
                                                   const void *srcptr,
                                                   const ze_image_region_t *pDstRegion,
                                                   ze_event_handle_t hEvent,
                                                   uint32_t numWaitEvents,
                                                   ze_event_handle_t *phWaitEvents) {
    trace_zeCommandListAppendImageCopyFromMemory(hCommandList,
                                                 hDstImage,
                                                 srcptr,
                                                 pDstRegion,
                                                 hEvent,
                                                 numWaitEvents,
                                                 phWaitEvents);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeCommandListAppendImageCopyFromMemory(ret,
                                                 hCommandList,
                                                 hDstImage,
                                                 srcptr,
                                                 pDstRegion,
                                                 hEvent,
                                                 numWaitEvents,
                                                 phWaitEvents);
    return ret;
}

ze_result_t zeCommandListAppendMemoryPrefetch(ze_command_list_handle_t hCommandList,
                                              const void *ptr,
                                              size_t size) {
    trace_zeCommandListAppendMemoryPrefetch(hCommandList, ptr, size);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeCommandListAppendMemoryPrefetch(ret, hCommandList, ptr, size);
    return ret;
}

ze_result_t zeCommandListAppendMemAdvise(ze_command_list_handle_t hCommandList,
                                         ze_device_handle_t hDevice,
                                         const void *ptr,
                                         size_t size,
                                         ze_memory_advice_t advice) {
    trace_zeCommandListAppendMemAdvise(hCommandList, hDevice, ptr, size, advice);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeCommandListAppendMemAdvise(ret, hCommandList, hDevice, ptr, size, advice);
    return ret;
}

ze_result_t zeCommandListAppendMemoryCopyFromContext(ze_command_list_handle_t hCommandList,
                                                     void *dstptr,
                                                     ze_context_handle_t hContextSrc,
                                                     const void *srcptr,
                                                     size_t size,
                                                     ze_event_handle_t hSignalEvent,
                                                     uint32_t numWaitEvents,
                                                     ze_event_handle_t *phWaitEvents) {
    trace_zeCommandListAppendMemoryCopyFromContext(hCommandList,
                                                   dstptr,
                                                   hContextSrc,
                                                   srcptr,
                                                   size,
                                                   hSignalEvent,
                                                   numWaitEvents,
                                                   phWaitEvents);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeCommandListAppendMemoryCopyFromContext(ret,
                                                   hCommandList,
                                                   dstptr,
                                                   hContextSrc,
                                                   srcptr,
                                                   size,
                                                   hSignalEvent,
                                                   numWaitEvents,
                                                   phWaitEvents);
    return ret;
}

ze_result_t zeCommandListAppendWriteGlobalTimestamp(ze_command_list_handle_t hCommandList,
                                                    uint64_t *dstptr,
                                                    ze_event_handle_t hSignalEvent,
                                                    uint32_t numWaitEvents,
                                                    ze_event_handle_t *phWaitEvents) {
    trace_zeCommandListAppendWriteGlobalTimestamp(hCommandList,
                                                  dstptr,
                                                  hSignalEvent,
                                                  numWaitEvents,
                                                  phWaitEvents);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        CommandList::fromHandle(hCommandList)
            ->appendWriteGlobalTimestamp(dstptr, hSignalEvent, numWaitEvents, phWaitEvents));

exit:
    trace_zeCommandListAppendWriteGlobalTimestamp(ret,
                                                  hCommandList,
                                                  dstptr,
                                                  hSignalEvent,
                                                  numWaitEvents,
                                                  phWaitEvents);
    return ret;
}

ze_result_t zeCommandListAppendQueryKernelTimestamps(ze_command_list_handle_t hCommandList,
                                                     uint32_t numEvents,
                                                     ze_event_handle_t *phEvents,
                                                     void *dstptr,
                                                     const size_t *pOffsets,
                                                     ze_event_handle_t hSignalEvent,
                                                     uint32_t numWaitEvents,
                                                     ze_event_handle_t *phWaitEvents) {
    trace_zeCommandListAppendQueryKernelTimestamps(hCommandList,
                                                   numEvents,
                                                   phEvents,
                                                   dstptr,
                                                   pOffsets,
                                                   hSignalEvent,
                                                   numWaitEvents,
                                                   phWaitEvents);
    ze_result_t ret = ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;

    trace_zeCommandListAppendQueryKernelTimestamps(ret,
                                                   hCommandList,
                                                   numEvents,
                                                   phEvents,
                                                   dstptr,
                                                   pOffsets,
                                                   hSignalEvent,
                                                   numWaitEvents,
                                                   phWaitEvents);
    return ret;
}

ze_result_t zeCommandListGetNextCommandIdExp(ze_command_list_handle_t hCommandList,
                                             const ze_mutable_command_id_exp_desc_t *desc,
                                             uint64_t *pCommandId) {
    trace_zeCommandListGetNextCommandIdExp(hCommandList, desc, pCommandId);
    ze_result_t ret;

    L0_HANDLE_EXCEPTION(ret,
                        CommandList::fromHandle(hCommandList)->getNextCommandId(desc, pCommandId));

    trace_zeCommandListGetNextCommandIdExp(ret, hCommandList, desc, pCommandId);
    return ret;
}

ze_result_t zeCommandListUpdateMutableCommandsExp(ze_command_list_handle_t hCommandList,
                                                  const ze_mutable_commands_exp_desc_t *desc) {
    trace_zeCommandListUpdateMutableCommandsExp(hCommandList, desc);
    ze_result_t ret;

    L0_HANDLE_EXCEPTION(ret, CommandList::fromHandle(hCommandList)->updateMutableCommands(desc));

    trace_zeCommandListUpdateMutableCommandsExp(ret, hCommandList, desc);
    return ret;
}

ze_result_t zeCommandListAppendSignalEvent(ze_command_list_handle_t hCommandList,
                                           ze_event_handle_t hEvent) {
    trace_zeCommandListAppendSignalEvent(hCommandList, hEvent);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(
        ret,
        CommandList::fromHandle(hCommandList)->CommandList::appendSignalEvent(hEvent));

exit:
    trace_zeCommandListAppendSignalEvent(ret, hCommandList, hEvent);
    return ret;
}

ze_result_t zeCommandListAppendWaitOnEvents(ze_command_list_handle_t hCommandList,
                                            uint32_t numEvents,
                                            ze_event_handle_t *phEvents) {
    trace_zeCommandListAppendWaitOnEvents(hCommandList, numEvents, phEvents);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret,
                        CommandList::fromHandle(hCommandList)
                            ->CommandList::appendWaitOnEvents(numEvents, phEvents));

exit:
    trace_zeCommandListAppendWaitOnEvents(ret, hCommandList, numEvents, phEvents);
    return ret;
}

ze_result_t zeCommandListAppendEventReset(ze_command_list_handle_t hCommandList,
                                          ze_event_handle_t hEvent) {
    trace_zeCommandListAppendEventReset(hCommandList, hEvent);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }
    L0_HANDLE_EXCEPTION(ret, CommandList::fromHandle(hCommandList)->appendEventReset(hEvent));

exit:
    trace_zeCommandListAppendEventReset(ret, hCommandList, hEvent);
    return ret;
}
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL
zeGetCommandListProcAddrTable(ze_api_version_t version, ze_command_list_dditable_t *pDdiTable) {
    trace_zeGetCommandListProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

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
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetCommandListProcAddrTable(ret, version, pDdiTable);
    return ret;
}

ZE_DLLEXPORT ze_result_t ZE_APICALL
zeGetCommandListExpProcAddrTable(ze_api_version_t version,
                                 ze_command_list_exp_dditable_t *pDdiTable) {
    trace_zeGetCommandListExpProcAddrTable(version, pDdiTable);
    ze_result_t ret;

    if (nullptr == pDdiTable) {
        ret = ZE_RESULT_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    if (ZE_MAJOR_VERSION(ZE_API_VERSION_CURRENT) != ZE_MAJOR_VERSION(version)) {
        ret = ZE_RESULT_ERROR_UNSUPPORTED_VERSION;
        goto exit;
    }

    pDdiTable->pfnGetNextCommandIdExp = L0::zeCommandListGetNextCommandIdExp;
    pDdiTable->pfnUpdateMutableCommandsExp = L0::zeCommandListUpdateMutableCommandsExp;
    ret = ZE_RESULT_SUCCESS;

exit:
    trace_zeGetCommandListExpProcAddrTable(ret, version, pDdiTable);
    return ret;
}
}
