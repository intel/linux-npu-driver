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
#include "level_zero_driver/api/zet_misc.hpp"
#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/ext/source/graph/graph.hpp"
#include "level_zero_driver/ext/source/graph/profiling_data.hpp"
#include "level_zero_driver/ext/source/graph/query_network.hpp"
#include "level_zero_driver/include/l0_exception.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <memory>

namespace L0 {

ze_result_t ZE_APICALL zeGraphCreate(ze_context_handle_t hContext,
                                     ze_device_handle_t hDevice,
                                     const ze_graph_desc_t *pDesc,
                                     ze_graph_handle_t *phGraph) {
    if (pDesc == nullptr) {
        LOG_E("Invalid graph descriptor");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    ze_graph_desc_2_t desc = {.stype = pDesc->stype,
                              .pNext = pDesc->pNext,
                              .format = pDesc->format,
                              .inputSize = pDesc->inputSize,
                              .pInput = pDesc->pInput,
                              .pBuildFlags = pDesc->pBuildFlags,
                              .flags = 0};

    return zeGraphCreate2(hContext, hDevice, &desc, phGraph);
}

ze_result_t ZE_APICALL zeGraphDestroy(ze_graph_handle_t hGraph) {
    if (hGraph == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Graph::fromHandle(hGraph)->destroy());
}

ze_result_t ZE_APICALL zeGraphGetProperties(ze_graph_handle_t hGraph,
                                            ze_graph_properties_t *pGraphProperties) {
    if (hGraph == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Graph::fromHandle(hGraph)->getProperties(pGraphProperties));
}

ze_result_t ZE_APICALL zeGraphGetProperties2(ze_graph_handle_t hGraph,
                                             ze_graph_properties_2_t *pGraphProperties) {
    if (hGraph == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    if (pGraphProperties == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Graph::fromHandle(hGraph)->getProperties2(pGraphProperties));
}

ze_result_t ZE_APICALL
zeGraphGetArgumentProperties(ze_graph_handle_t hGraph,
                             uint32_t argIndex,
                             ze_graph_argument_properties_t *pGraphArgumentProperties) {
    if (hGraph == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Graph::fromHandle(hGraph)->getArgumentProperties(argIndex, pGraphArgumentProperties));
}

ze_result_t ZE_APICALL zeGraphSetArgumentValue(ze_graph_handle_t hGraph,
                                               uint32_t argIndex,
                                               const void *pArgValue) {
    if (hGraph == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Graph::fromHandle(hGraph)->setArgumentValue(argIndex, pArgValue));
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
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        appendGraphInitialize(hCommandList, hGraph, hSignalEvent, numWaitEvents, phWaitEvents));
}

ze_result_t ZE_APICALL zeGraphInitialize(ze_graph_handle_t hGraph) {
    if (hGraph == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Graph::fromHandle(hGraph)->parserInitialize());
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
    if (hCommandList == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(appendGraphExecute(hCommandList,
                                                      hGraph,
                                                      hProfilingQuery,
                                                      hSignalEvent,
                                                      numWaitEvents,
                                                      phWaitEvents));
}

ze_result_t ZE_APICALL zeGraphGetNativeBinary(ze_graph_handle_t hGraph,
                                              size_t *pSize,
                                              uint8_t *pGraphNativeBinary) {
    if (hGraph == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Graph::fromHandle(hGraph)->getNativeBinary(pSize, pGraphNativeBinary));
}

ze_result_t ZE_APICALL zeGraphGetNativeBinary2(ze_graph_handle_t hGraph,
                                               size_t *pSize,
                                               const uint8_t **pGraphNativeBinary) {
    if (hGraph == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    if (pSize == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (pGraphNativeBinary == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Graph::fromHandle(hGraph)->getNativeBinary2(pSize, pGraphNativeBinary));
}

ze_result_t ZE_APICALL
zeDeviceGetGraphProperties(ze_device_handle_t hDevice,
                           ze_device_graph_properties_t *pDeviceGraphProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    auto result = translateHandle(ZEL_HANDLE_DEVICE, hDevice);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Graph::getDeviceGraphProperties(hDevice, pDeviceGraphProperties));
}

ze_result_t ZE_APICALL
zeDeviceGetGraphProperties2(ze_device_handle_t hDevice,
                            ze_device_graph_properties_2_t *pDeviceGraphProperties2) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    auto result = translateHandle(ZEL_HANDLE_DEVICE, hDevice);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Graph::getDeviceGraphProperties2(hDevice, pDeviceGraphProperties2));
}

ze_result_t ZE_APICALL
zeGraphGetArgumentMetadata(ze_graph_handle_t hGraph,
                           uint32_t argIndex,
                           ze_graph_argument_metadata_t *pGraphArgumentMetadata) {
    if (hGraph == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Graph::fromHandle(hGraph)->getArgumentMetadata(argIndex, pGraphArgumentMetadata));
}

ze_result_t ZE_APICALL
zeGraphGetArgumentProperties2(ze_graph_handle_t hGraph,
                              uint32_t argIndex,
                              ze_graph_argument_properties_2_t *pGraphArgumentProperties) {
    if (hGraph == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Graph::fromHandle(hGraph)->getArgumentProperties2(argIndex, pGraphArgumentProperties));
}

ze_result_t ZE_APICALL
zeGraphGetArgumentProperties3(ze_graph_handle_t hGraph,
                              uint32_t argIndex,
                              ze_graph_argument_properties_3_t *pGraphArgumentProperties) {
    if (hGraph == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Graph::fromHandle(hGraph)->getArgumentProperties3(argIndex, pGraphArgumentProperties));
}

ze_result_t ZE_APICALL
zeGraphQueryNetworkCreate(ze_context_handle_t hContext,
                          ze_device_handle_t hDevice,
                          const ze_graph_desc_t *pDesc,
                          ze_graph_query_network_handle_t *phGraphQueryNetwork) {
    if (pDesc == nullptr) {
        LOG_E("Invalid graph descriptor");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    ze_graph_desc_2_t desc = {.stype = pDesc->stype,
                              .pNext = pDesc->pNext,
                              .format = pDesc->format,
                              .inputSize = pDesc->inputSize,
                              .pInput = pDesc->pInput,
                              .pBuildFlags = pDesc->pBuildFlags,
                              .flags = 0};

    return zeGraphQueryNetworkCreate2(hContext, hDevice, &desc, phGraphQueryNetwork);
}

ze_result_t ZE_APICALL
zeGraphQueryNetworkDestroy(ze_graph_query_network_handle_t hGraphQueryNetwork) {
    if (hGraphQueryNetwork == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::QueryNetwork::fromHandle(hGraphQueryNetwork)->destroy());
}

ze_result_t ZE_APICALL
zeGraphQueryNetworkGetSupportedLayers(ze_graph_query_network_handle_t hGraphQueryNetwork,
                                      size_t *pSize,
                                      char *pSupportedLayers) {
    if (hGraphQueryNetwork == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::QueryNetwork::fromHandle(hGraphQueryNetwork)
                                       ->getSupportedLayers(pSize, pSupportedLayers));
}

ze_result_t ZE_APICALL zeGraphBuildLogGetString(ze_graph_handle_t hGraph,
                                                uint32_t *pSize,
                                                char *pBuildLog) {
    if (hGraph != nullptr) {
        // Case when the graph creation was successful, no logs from compiler
        return ZE_RESULT_SUCCESS;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Graph::getLogString(pSize, pBuildLog));
}

ze_result_t ZE_APICALL zeGraphCreate2(ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      const ze_graph_desc_2_t *desc,
                                      ze_graph_handle_t *phGraph) {
    if (hDevice == nullptr || hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    auto result = translateHandle(ZEL_HANDLE_CONTEXT, hContext);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    result = translateHandle(ZEL_HANDLE_DEVICE, hDevice);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::Graph::create(hContext, hDevice, desc, phGraph));
}

ze_result_t ZE_APICALL
zeGraphQueryNetworkCreate2(ze_context_handle_t hContext,
                           ze_device_handle_t hDevice,
                           const ze_graph_desc_2_t *desc,
                           ze_graph_query_network_handle_t *phGraphQueryNetwork) {
    if (hDevice == nullptr || hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    auto result = translateHandle(ZEL_HANDLE_CONTEXT, hContext);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    result = translateHandle(ZEL_HANDLE_DEVICE, hDevice);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::QueryNetwork::create(hContext, hDevice, desc, phGraphQueryNetwork));
}

ze_result_t ZE_APICALL zeGraphQueryContextMemory(ze_context_handle_t hContext,
                                                 ze_graph_memory_query_type_t type,
                                                 ze_graph_memory_query_t *query) {
    if (hContext == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    auto result = translateHandle(ZEL_HANDLE_CONTEXT, hContext);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Context::fromHandle(hContext)->queryContextMemory(type, query));
}

ze_result_t ZE_APICALL
zeGraphProfilingPoolCreate(ze_graph_handle_t hGraph,
                           uint32_t count,
                           ze_graph_profiling_pool_handle_t *phProfilingPool) {
    if (hGraph == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Graph::fromHandle(hGraph)->createProfilingPool(count, phProfilingPool));
}

ze_result_t ZE_APICALL
zeGraphProfilingPoolDestroy(ze_graph_profiling_pool_handle_t hProfilingPool) {
    if (hProfilingPool == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::GraphProfilingPool::fromHandle(hProfilingPool)->destroy());
}

ze_result_t ZE_APICALL
zeGraphProfilingQueryCreate(ze_graph_profiling_pool_handle_t hProfilingPool,
                            uint32_t index,
                            ze_graph_profiling_query_handle_t *phProfilingQuery) {
    if (hProfilingPool == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::GraphProfilingPool::fromHandle(hProfilingPool)
                                       ->createProfilingQuery(index, phProfilingQuery));
}

ze_result_t ZE_APICALL
zeGraphProfilingQueryDestroy(ze_graph_profiling_query_handle_t hProfilingQuery) {
    if (hProfilingQuery == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(L0::GraphProfilingQuery::fromHandle(hProfilingQuery)->destroy());
}

ze_result_t ZE_APICALL
zeGraphProfilingQueryGetData(ze_graph_profiling_query_handle_t hProfilingQuery,
                             ze_graph_profiling_type_t type,
                             uint32_t *pSize,
                             uint8_t *pData) {
    if (hProfilingQuery == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::GraphProfilingQuery::fromHandle(hProfilingQuery)->getData(type, pSize, pData));
}

ze_result_t ZE_APICALL zeDeviceGetProfilingDataProperties(
    ze_device_handle_t hDevice,
    ze_device_profiling_data_properties_t *pDeviceProfilingDataProperties) {
    if (hDevice == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    auto result = translateHandle(ZEL_HANDLE_DEVICE, hDevice);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::Graph::getProfilingDataProperties(pDeviceProfilingDataProperties));
}

ze_result_t ZE_APICALL
zeGraphProfilingLogGetString(ze_graph_profiling_query_handle_t hProfilingQuery,
                             uint32_t *pSize,
                             char *pProfilingLog) {
    if (hProfilingQuery == nullptr) {
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    L0_HANDLE_EXCEPTION_AND_RETURN(
        L0::GraphProfilingQuery::fromHandle(hProfilingQuery)->getLogString(pSize, pProfilingLog));
}

} // namespace L0
