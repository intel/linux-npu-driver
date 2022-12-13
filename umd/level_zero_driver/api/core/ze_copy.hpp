/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include <level_zero/ze_api.h>

namespace L0 {
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
    return L0::CommandList::fromHandle(hCommandList)
        ->appendMemoryCopy(dstptr, srcptr, size, hEvent, numWaitEvents, phWaitEvents);
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
    return L0::CommandList::fromHandle(hCommandList)
        ->appendMemoryFill(ptr, pattern, patternSize, size, hEvent, numWaitEvents, phWaitEvents);
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
} // namespace L0

extern "C" {
ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendMemoryCopy(ze_command_list_handle_t hCommandList,
                              void *dstptr,
                              const void *srcptr,
                              size_t size,
                              ze_event_handle_t hEvent,
                              uint32_t numWaitEvents,
                              ze_event_handle_t *phWaitEvents) {
    return L0::zeCommandListAppendMemoryCopy(hCommandList,
                                             dstptr,
                                             srcptr,
                                             size,
                                             hEvent,
                                             numWaitEvents,
                                             phWaitEvents);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendMemoryFill(ze_command_list_handle_t hCommandList,
                              void *ptr,
                              const void *pattern,
                              size_t patternSize,
                              size_t size,
                              ze_event_handle_t hEvent,
                              uint32_t numWaitEvents,
                              ze_event_handle_t *phWaitEvents) {
    return L0::zeCommandListAppendMemoryFill(hCommandList,
                                             ptr,
                                             pattern,
                                             patternSize,
                                             size,
                                             hEvent,
                                             numWaitEvents,
                                             phWaitEvents);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendMemoryCopyRegion(ze_command_list_handle_t hCommandList,
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
    return L0::zeCommandListAppendMemoryCopyRegion(hCommandList,
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
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendImageCopy(ze_command_list_handle_t hCommandList,
                             ze_image_handle_t hDstImage,
                             ze_image_handle_t hSrcImage,
                             ze_event_handle_t hEvent,
                             uint32_t numWaitEvents,
                             ze_event_handle_t *phWaitEvents) {
    return L0::zeCommandListAppendImageCopy(hCommandList,
                                            hDstImage,
                                            hSrcImage,
                                            hEvent,
                                            numWaitEvents,
                                            phWaitEvents);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendImageCopyRegion(ze_command_list_handle_t hCommandList,
                                   ze_image_handle_t hDstImage,
                                   ze_image_handle_t hSrcImage,
                                   const ze_image_region_t *pDstRegion,
                                   const ze_image_region_t *pSrcRegion,
                                   ze_event_handle_t hEvent,
                                   uint32_t numWaitEvents,
                                   ze_event_handle_t *phWaitEvents) {
    return L0::zeCommandListAppendImageCopyRegion(hCommandList,
                                                  hDstImage,
                                                  hSrcImage,
                                                  pDstRegion,
                                                  pSrcRegion,
                                                  hEvent,
                                                  numWaitEvents,
                                                  phWaitEvents);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendImageCopyToMemory(ze_command_list_handle_t hCommandList,
                                     void *dstptr,
                                     ze_image_handle_t hSrcImage,
                                     const ze_image_region_t *pSrcRegion,
                                     ze_event_handle_t hEvent,
                                     uint32_t numWaitEvents,
                                     ze_event_handle_t *phWaitEvents) {
    return L0::zeCommandListAppendImageCopyToMemory(hCommandList,
                                                    dstptr,
                                                    hSrcImage,
                                                    pSrcRegion,
                                                    hEvent,
                                                    numWaitEvents,
                                                    phWaitEvents);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendImageCopyFromMemory(ze_command_list_handle_t hCommandList,
                                       ze_image_handle_t hDstImage,
                                       const void *srcptr,
                                       const ze_image_region_t *pDstRegion,
                                       ze_event_handle_t hEvent,
                                       uint32_t numWaitEvents,
                                       ze_event_handle_t *phWaitEvents) {
    return L0::zeCommandListAppendImageCopyFromMemory(hCommandList,
                                                      hDstImage,
                                                      srcptr,
                                                      pDstRegion,
                                                      hEvent,
                                                      numWaitEvents,
                                                      phWaitEvents);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendMemoryPrefetch(ze_command_list_handle_t hCommandList,
                                  const void *ptr,
                                  size_t size) {
    return L0::zeCommandListAppendMemoryPrefetch(hCommandList, ptr, size);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendMemAdvise(ze_command_list_handle_t hCommandList,
                             ze_device_handle_t hDevice,
                             const void *ptr,
                             size_t size,
                             ze_memory_advice_t advice) {
    return L0::zeCommandListAppendMemAdvise(hCommandList, hDevice, ptr, size, advice);
}

ZE_APIEXPORT ze_result_t ZE_APICALL
zeCommandListAppendMemoryCopyFromContext(ze_command_list_handle_t hCommandList,
                                         void *dstptr,
                                         ze_context_handle_t hContextSrc,
                                         const void *srcptr,
                                         size_t size,
                                         ze_event_handle_t hSignalEvent,
                                         uint32_t numWaitEvents,
                                         ze_event_handle_t *phWaitEvents) {
    return L0::zeCommandListAppendMemoryCopyFromContext(hCommandList,
                                                        dstptr,
                                                        hContextSrc,
                                                        srcptr,
                                                        size,
                                                        hSignalEvent,
                                                        numWaitEvents,
                                                        phWaitEvents);
}
} // extern "C"
