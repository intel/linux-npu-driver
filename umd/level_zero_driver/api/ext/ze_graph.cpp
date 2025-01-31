/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/api/ext/ze_graph.hpp"

#include "level_zero/loader/ze_loader.h"
#include "level_zero/ze_api.h"
#include "level_zero/ze_graph_ext.h"
#include "level_zero/ze_graph_profiling_ext.h"
#include "level_zero_driver/api/trace/trace_ze_graph_ext.hpp"
#include "level_zero_driver/api/trace/trace_ze_graph_profiling_ext.hpp"
#include "level_zero_driver/api/zet_misc.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "level_zero_driver/source/cmdlist.hpp"
#include "level_zero_driver/source/context.hpp"
#include "level_zero_driver/source/device.hpp"
#include "level_zero_driver/source/ext/graph.hpp"
#include "level_zero_driver/source/ext/profiling_data.hpp"
#include "level_zero_driver/source/ext/query_network.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <memory>

namespace L0 {

ze_result_t ZE_APICALL zeGraphCreate(ze_context_handle_t hContext,
                                     ze_device_handle_t hDevice,
                                     const ze_graph_desc_t *pDesc,
                                     ze_graph_handle_t *phGraph) {
    trace_zeGraphCreate(hContext, hDevice, pDesc, phGraph);
    ze_graph_desc_2_t desc{};
    ze_result_t ret;

    if (pDesc == nullptr) {
        LOG_E("Invalid graph descriptor");
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    desc.stype = pDesc->stype;
    desc.pNext = pDesc->pNext;
    desc.format = pDesc->format;
    desc.inputSize = pDesc->inputSize;
    desc.pInput = pDesc->pInput;
    desc.pBuildFlags = pDesc->pBuildFlags;
    desc.flags = 0;

    ret = zeGraphCreate2(hContext, hDevice, &desc, phGraph);

exit:
    trace_zeGraphCreate(ret, hContext, hDevice, pDesc, phGraph);
    return ret;
}

ze_result_t ZE_APICALL zeGraphDestroy(ze_graph_handle_t hGraph) {
    trace_zeGraphDestroy(hGraph);
    ze_result_t ret;

    if (hGraph == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::Graph::fromHandle(hGraph)->destroy());

exit:
    trace_zeGraphDestroy(ret, hGraph);
    return ret;
}

ze_result_t ZE_APICALL zeGraphGetProperties(ze_graph_handle_t hGraph,
                                            ze_graph_properties_t *pGraphProperties) {
    trace_zeGraphGetProperties(hGraph, pGraphProperties);
    ze_result_t ret;

    if (hGraph == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::Graph::fromHandle(hGraph)->getProperties(pGraphProperties));

exit:
    trace_zeGraphGetProperties(ret, hGraph, pGraphProperties);
    return ret;
}

ze_result_t ZE_APICALL zeGraphGetProperties2(ze_graph_handle_t hGraph,
                                             ze_graph_properties_2_t *pGraphProperties) {
    trace_zeGraphGetProperties2(hGraph, pGraphProperties);
    ze_result_t ret;

    if (hGraph == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    if (pGraphProperties == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::Graph::fromHandle(hGraph)->getProperties2(pGraphProperties));

exit:
    trace_zeGraphGetProperties2(ret, hGraph, pGraphProperties);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphGetArgumentProperties(ze_graph_handle_t hGraph,
                             uint32_t argIndex,
                             ze_graph_argument_properties_t *pGraphArgumentProperties) {
    trace_zeGraphGetArgumentProperties(hGraph, argIndex, pGraphArgumentProperties);
    ze_result_t ret;

    if (hGraph == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(
        ret,
        L0::Graph::fromHandle(hGraph)->getArgumentProperties(argIndex, pGraphArgumentProperties));

exit:
    trace_zeGraphGetArgumentProperties(ret, hGraph, argIndex, pGraphArgumentProperties);
    return ret;
}

ze_result_t ZE_APICALL zeGraphSetArgumentValue(ze_graph_handle_t hGraph,
                                               uint32_t argIndex,
                                               const void *pArgValue) {
    trace_zeGraphSetArgumentValue(hGraph, argIndex, pArgValue);
    ze_result_t ret;

    if (hGraph == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::Graph::fromHandle(hGraph)->setArgumentValue(argIndex, pArgValue));

exit:
    trace_zeGraphSetArgumentValue(ret, hGraph, argIndex, pArgValue);
    return ret;
}

static ze_result_t appendGraphInitialize(ze_command_list_handle_t hCommandList,
                                         ze_graph_handle_t hGraph,
                                         ze_event_handle_t hSignalEvent,
                                         uint32_t numWaitEvents,
                                         ze_event_handle_t *phWaitEvents) {
    auto result = translateHandle(ZEL_HANDLE_COMMAND_LIST, hCommandList);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    if (hSignalEvent != nullptr) {
        auto result = translateHandle(ZEL_HANDLE_EVENT, hSignalEvent);
        if (result != ZE_RESULT_SUCCESS)
            return result;
    }

    std::unique_ptr<ze_event_handle_t[]> phWaitEventsTranslated = nullptr;
    if (numWaitEvents > 0) {
        if (phWaitEvents == nullptr)
            return ZE_RESULT_ERROR_INVALID_SIZE;

        phWaitEventsTranslated =
            std::unique_ptr<ze_event_handle_t[]>(new ze_event_handle_t[numWaitEvents]);
        for (uint32_t i = 0; i < numWaitEvents; i++) {
            auto result =
                translateHandle(ZEL_HANDLE_EVENT, phWaitEvents[i], &phWaitEventsTranslated[i]);
            if (result != ZE_RESULT_SUCCESS)
                return result;
        }
    }

    return L0::CommandList::fromHandle(hCommandList)
        ->appendGraphInitialize(hGraph, hSignalEvent, numWaitEvents, phWaitEventsTranslated.get());
}

ze_result_t ZE_APICALL zeAppendGraphInitialize(ze_command_list_handle_t hCommandList,
                                               ze_graph_handle_t hGraph,
                                               ze_event_handle_t hSignalEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
    trace_zeAppendGraphInitialize(hCommandList, hGraph, hSignalEvent, numWaitEvents, phWaitEvents);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(
        ret,
        appendGraphInitialize(hCommandList, hGraph, hSignalEvent, numWaitEvents, phWaitEvents));

exit:
    trace_zeAppendGraphInitialize(ret,
                                  hCommandList,
                                  hGraph,
                                  hSignalEvent,
                                  numWaitEvents,
                                  phWaitEvents);
    return ret;
}

ze_result_t ZE_APICALL zeGraphInitialize(ze_graph_handle_t hGraph) {
    trace_zeGraphInitialize(hGraph);
    ze_result_t ret;

    if (hGraph == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::Graph::fromHandle(hGraph)->parserInitialize());

exit:
    trace_zeGraphInitialize(ret, hGraph);
    return ret;
}

static ze_result_t appendGraphExecute(ze_command_list_handle_t hCommandList,
                                      ze_graph_handle_t hGraph,
                                      ze_graph_profiling_query_handle_t hProfilingQuery,
                                      ze_event_handle_t hSignalEvent,
                                      uint32_t numWaitEvents,
                                      ze_event_handle_t *phWaitEvents) {
    auto result = translateHandle(ZEL_HANDLE_COMMAND_LIST, hCommandList);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    if (hSignalEvent != nullptr) {
        auto result = translateHandle(ZEL_HANDLE_EVENT, hSignalEvent);
        if (result != ZE_RESULT_SUCCESS)
            return result;
    }

    std::unique_ptr<ze_event_handle_t[]> phWaitEventsTranslated = nullptr;
    if (numWaitEvents > 0) {
        if (phWaitEvents == nullptr)
            return ZE_RESULT_ERROR_INVALID_SIZE;

        phWaitEventsTranslated =
            std::unique_ptr<ze_event_handle_t[]>(new ze_event_handle_t[numWaitEvents]);
        for (uint32_t i = 0; i < numWaitEvents; i++) {
            auto result =
                translateHandle(ZEL_HANDLE_EVENT, phWaitEvents[i], &phWaitEventsTranslated[i]);
            if (result != ZE_RESULT_SUCCESS)
                return result;
        }
    }

    return L0::CommandList::fromHandle(hCommandList)
        ->appendGraphExecute(hGraph,
                             hProfilingQuery,
                             hSignalEvent,
                             numWaitEvents,
                             phWaitEventsTranslated.get());
}

ze_result_t ZE_APICALL zeAppendGraphExecute(ze_command_list_handle_t hCommandList,
                                            ze_graph_handle_t hGraph,
                                            ze_graph_profiling_query_handle_t hProfilingQuery,
                                            ze_event_handle_t hSignalEvent,
                                            uint32_t numWaitEvents,
                                            ze_event_handle_t *phWaitEvents) {
    trace_zeAppendGraphExecute(hCommandList,
                               hGraph,
                               hProfilingQuery,
                               hSignalEvent,
                               numWaitEvents,
                               phWaitEvents);
    ze_result_t ret;

    if (hCommandList == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret,
                        appendGraphExecute(hCommandList,
                                           hGraph,
                                           hProfilingQuery,
                                           hSignalEvent,
                                           numWaitEvents,
                                           phWaitEvents));

exit:
    trace_zeAppendGraphExecute(ret,
                               hCommandList,
                               hGraph,
                               hProfilingQuery,
                               hSignalEvent,
                               numWaitEvents,
                               phWaitEvents);
    return ret;
}

ze_result_t ZE_APICALL zeGraphGetNativeBinary(ze_graph_handle_t hGraph,
                                              size_t *pSize,
                                              uint8_t *pGraphNativeBinary) {
    trace_zeGraphGetNativeBinary(hGraph, pSize, pGraphNativeBinary);
    ze_result_t ret;

    if (hGraph == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret,
                        L0::Graph::fromHandle(hGraph)->getNativeBinary(pSize, pGraphNativeBinary));

exit:
    trace_zeGraphGetNativeBinary(ret, hGraph, pSize, pGraphNativeBinary);
    return ret;
}

ze_result_t ZE_APICALL zeGraphGetNativeBinary2(ze_graph_handle_t hGraph,
                                               size_t *pSize,
                                               const uint8_t **pGraphNativeBinary) {
    trace_zeGraphGetNativeBinary2(hGraph, pSize, pGraphNativeBinary);
    ze_result_t ret;

    if (hGraph == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    if (pSize == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    if (pGraphNativeBinary == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret,
                        L0::Graph::fromHandle(hGraph)->getNativeBinary2(pSize, pGraphNativeBinary));

exit:
    trace_zeGraphGetNativeBinary2(ret, hGraph, pSize, pGraphNativeBinary);
    return ret;
}

ze_result_t ZE_APICALL
zeDeviceGetGraphProperties(ze_device_handle_t hDevice,
                           ze_device_graph_properties_t *pDeviceGraphProperties) {
    trace_zeDeviceGetGraphProperties(hDevice, pDeviceGraphProperties);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    ret = translateHandle(ZEL_HANDLE_DEVICE, hDevice);
    if (ret != ZE_RESULT_SUCCESS) {
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::Graph::getDeviceGraphProperties(hDevice, pDeviceGraphProperties));

exit:
    trace_zeDeviceGetGraphProperties(ret, hDevice, pDeviceGraphProperties);
    return ret;
}

ze_result_t ZE_APICALL
zeDeviceGetGraphProperties2(ze_device_handle_t hDevice,
                            ze_device_graph_properties_2_t *pDeviceGraphProperties2) {
    trace_zeDeviceGetGraphProperties2(hDevice, pDeviceGraphProperties2);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    ret = translateHandle(ZEL_HANDLE_DEVICE, hDevice);
    if (ret != ZE_RESULT_SUCCESS) {
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret,
                        L0::Graph::getDeviceGraphProperties2(hDevice, pDeviceGraphProperties2));

exit:
    trace_zeDeviceGetGraphProperties2(ret, hDevice, pDeviceGraphProperties2);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphGetArgumentMetadata(ze_graph_handle_t hGraph,
                           uint32_t argIndex,
                           ze_graph_argument_metadata_t *pGraphArgumentMetadata) {
    trace_zeGraphGetArgumentMetadata(hGraph, argIndex, pGraphArgumentMetadata);
    ze_result_t ret;

    if (hGraph == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(
        ret,
        L0::Graph::fromHandle(hGraph)->getArgumentMetadata(argIndex, pGraphArgumentMetadata));

exit:
    trace_zeGraphGetArgumentMetadata(ret, hGraph, argIndex, pGraphArgumentMetadata);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphGetArgumentProperties2(ze_graph_handle_t hGraph,
                              uint32_t argIndex,
                              ze_graph_argument_properties_2_t *pGraphArgumentProperties) {
    trace_zeGraphGetArgumentProperties2(hGraph, argIndex, pGraphArgumentProperties);
    ze_result_t ret;

    if (hGraph == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(
        ret,
        L0::Graph::fromHandle(hGraph)->getArgumentProperties2(argIndex, pGraphArgumentProperties));

exit:
    trace_zeGraphGetArgumentProperties2(ret, hGraph, argIndex, pGraphArgumentProperties);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphGetArgumentProperties3(ze_graph_handle_t hGraph,
                              uint32_t argIndex,
                              ze_graph_argument_properties_3_t *pGraphArgumentProperties) {
    trace_zeGraphGetArgumentProperties3(hGraph, argIndex, pGraphArgumentProperties);
    ze_result_t ret;

    if (hGraph == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(
        ret,
        L0::Graph::fromHandle(hGraph)->getArgumentProperties3(argIndex, pGraphArgumentProperties));

exit:
    trace_zeGraphGetArgumentProperties3(ret, hGraph, argIndex, pGraphArgumentProperties);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphQueryNetworkCreate(ze_context_handle_t hContext,
                          ze_device_handle_t hDevice,
                          const ze_graph_desc_t *pDesc,
                          ze_graph_query_network_handle_t *phGraphQueryNetwork) {
    trace_zeGraphQueryNetworkCreate(hContext, hDevice, pDesc, phGraphQueryNetwork);
    ze_graph_desc_2_t desc{};
    ze_result_t ret;

    if (pDesc == nullptr) {
        LOG_E("Invalid graph descriptor");
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        goto exit;
    }

    desc.stype = pDesc->stype;
    desc.pNext = pDesc->pNext;
    desc.format = pDesc->format;
    desc.inputSize = pDesc->inputSize;
    desc.pInput = pDesc->pInput;
    desc.pBuildFlags = pDesc->pBuildFlags;
    desc.flags = 0;

    ret = zeGraphQueryNetworkCreate2(hContext, hDevice, &desc, phGraphQueryNetwork);

exit:
    trace_zeGraphQueryNetworkCreate(ret, hContext, hDevice, pDesc, phGraphQueryNetwork);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphQueryNetworkDestroy(ze_graph_query_network_handle_t hGraphQueryNetwork) {
    trace_zeGraphQueryNetworkDestroy(hGraphQueryNetwork);
    ze_result_t ret;

    if (hGraphQueryNetwork == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::QueryNetwork::fromHandle(hGraphQueryNetwork)->destroy());

exit:
    trace_zeGraphQueryNetworkDestroy(ret, hGraphQueryNetwork);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphQueryNetworkGetSupportedLayers(ze_graph_query_network_handle_t hGraphQueryNetwork,
                                      size_t *pSize,
                                      char *pSupportedLayers) {
    trace_zeGraphQueryNetworkGetSupportedLayers(hGraphQueryNetwork, pSize, pSupportedLayers);
    ze_result_t ret;

    if (hGraphQueryNetwork == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret,
                        L0::QueryNetwork::fromHandle(hGraphQueryNetwork)
                            ->getSupportedLayers(pSize, pSupportedLayers));

exit:
    trace_zeGraphQueryNetworkGetSupportedLayers(ret, hGraphQueryNetwork, pSize, pSupportedLayers);
    return ret;
}

ze_result_t ZE_APICALL zeGraphBuildLogGetString(ze_graph_handle_t hGraph,
                                                uint32_t *pSize,
                                                char *pBuildLog) {
    trace_zeGraphBuildLogGetString(hGraph, pSize, pBuildLog);
    ze_result_t ret;

    if (hGraph != nullptr) {
        // Case when the graph creation was successful, no logs from compiler
        ret = ZE_RESULT_SUCCESS;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::Graph::getLogString(pSize, pBuildLog));

exit:
    trace_zeGraphBuildLogGetString(ret, hGraph, pSize, pBuildLog);
    return ret;
}

ze_result_t ZE_APICALL zeGraphCreate2(ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      const ze_graph_desc_2_t *desc,
                                      ze_graph_handle_t *phGraph) {
    trace_zeGraphCreate2(hContext, hDevice, desc, phGraph);
    ze_result_t ret;

    if (hDevice == nullptr || hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    ret = translateHandle(ZEL_HANDLE_CONTEXT, hContext);
    if (ret != ZE_RESULT_SUCCESS) {
        goto exit;
    }

    ret = translateHandle(ZEL_HANDLE_DEVICE, hDevice);
    if (ret != ZE_RESULT_SUCCESS) {
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::Graph::create(hContext, hDevice, desc, phGraph));

exit:
    trace_zeGraphCreate2(ret, hContext, hDevice, desc, phGraph);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphQueryNetworkCreate2(ze_context_handle_t hContext,
                           ze_device_handle_t hDevice,
                           const ze_graph_desc_2_t *desc,
                           ze_graph_query_network_handle_t *phGraphQueryNetwork) {
    trace_zeGraphQueryNetworkCreate2(hContext, hDevice, desc, phGraphQueryNetwork);
    ze_result_t ret;

    if (hDevice == nullptr || hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    ret = translateHandle(ZEL_HANDLE_CONTEXT, hContext);
    if (ret != ZE_RESULT_SUCCESS) {
        goto exit;
    }

    ret = translateHandle(ZEL_HANDLE_DEVICE, hDevice);
    if (ret != ZE_RESULT_SUCCESS) {
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret,
                        L0::QueryNetwork::create(hContext, hDevice, desc, phGraphQueryNetwork));

exit:
    trace_zeGraphQueryNetworkCreate2(ret, hContext, hDevice, desc, phGraphQueryNetwork);
    return ret;
}

ze_result_t ZE_APICALL zeGraphQueryContextMemory(ze_context_handle_t hContext,
                                                 ze_graph_memory_query_type_t type,
                                                 ze_graph_memory_query_t *query) {
    trace_zeGraphQueryContextMemory(hContext, type, query);
    ze_result_t ret;

    if (hContext == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    ret = translateHandle(ZEL_HANDLE_CONTEXT, hContext);
    if (ret != ZE_RESULT_SUCCESS) {
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::Context::fromHandle(hContext)->queryContextMemory(type, query));

exit:
    trace_zeGraphQueryContextMemory(ret, hContext, type, query);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphProfilingPoolCreate(ze_graph_handle_t hGraph,
                           uint32_t count,
                           ze_graph_profiling_pool_handle_t *phProfilingPool) {
    trace_zeGraphProfilingPoolCreate(hGraph, count, phProfilingPool);
    ze_result_t ret;

    if (hGraph == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret,
                        L0::Graph::fromHandle(hGraph)->createProfilingPool(count, phProfilingPool));

exit:
    trace_zeGraphProfilingPoolCreate(ret, hGraph, count, phProfilingPool);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphProfilingPoolDestroy(ze_graph_profiling_pool_handle_t hProfilingPool) {
    trace_zeGraphProfilingPoolDestroy(hProfilingPool);
    ze_result_t ret;

    if (hProfilingPool == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::GraphProfilingPool::fromHandle(hProfilingPool)->destroy());

exit:
    trace_zeGraphProfilingPoolDestroy(ret, hProfilingPool);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphProfilingQueryCreate(ze_graph_profiling_pool_handle_t hProfilingPool,
                            uint32_t index,
                            ze_graph_profiling_query_handle_t *phProfilingQuery) {
    trace_zeGraphProfilingQueryCreate(hProfilingPool, index, phProfilingQuery);
    ze_result_t ret;

    if (hProfilingPool == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret,
                        L0::GraphProfilingPool::fromHandle(hProfilingPool)
                            ->createProfilingQuery(index, phProfilingQuery));

exit:
    trace_zeGraphProfilingQueryCreate(ret, hProfilingPool, index, phProfilingQuery);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphProfilingQueryDestroy(ze_graph_profiling_query_handle_t hProfilingQuery) {
    trace_zeGraphProfilingQueryDestroy(hProfilingQuery);
    ze_result_t ret;

    if (hProfilingQuery == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::GraphProfilingQuery::fromHandle(hProfilingQuery)->destroy());

exit:
    trace_zeGraphProfilingQueryDestroy(ret, hProfilingQuery);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphProfilingQueryGetData(ze_graph_profiling_query_handle_t hProfilingQuery,
                             ze_graph_profiling_type_t type,
                             uint32_t *pSize,
                             uint8_t *pData) {
    trace_zeGraphProfilingQueryGetData(hProfilingQuery, type, pSize, pData);
    ze_result_t ret;

    if (hProfilingQuery == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(
        ret,
        L0::GraphProfilingQuery::fromHandle(hProfilingQuery)->getData(type, pSize, pData));

exit:
    trace_zeGraphProfilingQueryGetData(ret, hProfilingQuery, type, pSize, pData);
    return ret;
}

ze_result_t ZE_APICALL zeDeviceGetProfilingDataProperties(
    ze_device_handle_t hDevice,
    ze_device_profiling_data_properties_t *pDeviceProfilingDataProperties) {
    trace_zeDeviceGetProfilingDataProperties(hDevice, pDeviceProfilingDataProperties);
    ze_result_t ret;

    if (hDevice == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    ret = translateHandle(ZEL_HANDLE_DEVICE, hDevice);
    if (ret != ZE_RESULT_SUCCESS) {
        goto exit;
    }

    L0_HANDLE_EXCEPTION(ret, L0::Graph::getProfilingDataProperties(pDeviceProfilingDataProperties));

exit:
    trace_zeDeviceGetProfilingDataProperties(ret, hDevice, pDeviceProfilingDataProperties);
    return ret;
}

ze_result_t ZE_APICALL
zeGraphProfilingLogGetString(ze_graph_profiling_query_handle_t hProfilingQuery,
                             uint32_t *pSize,
                             char *pProfilingLog) {
    trace_zeGraphProfilingLogGetString(hProfilingQuery, pSize, pProfilingLog);
    ze_result_t ret;

    if (hProfilingQuery == nullptr) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
        goto exit;
    }

    L0_HANDLE_EXCEPTION(
        ret,
        L0::GraphProfilingQuery::fromHandle(hProfilingQuery)->getLogString(pSize, pProfilingLog));

exit:
    trace_zeGraphProfilingLogGetString(ret, hProfilingQuery, pSize, pProfilingLog);
    return ret;
}

} // namespace L0
