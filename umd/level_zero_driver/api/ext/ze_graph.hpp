/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "level_zero/ze_api.h"
#include "level_zero/ze_graph_ext.h"
#include "level_zero/ze_graph_profiling_ext.h"

namespace L0 {

ze_result_t ZE_APICALL zeGraphCreate(ze_context_handle_t hContext,
                                     ze_device_handle_t hDevice,
                                     const ze_graph_desc_t *pDesc,
                                     ze_graph_handle_t *phGraph);

ze_result_t ZE_APICALL zeGraphDestroy(ze_graph_handle_t hGraph);

ze_result_t ZE_APICALL zeGraphGetProperties(ze_graph_handle_t hGraph,
                                            ze_graph_properties_t *pGraphProperties);

ze_result_t ZE_APICALL zeGraphGetProperties2(ze_graph_handle_t hGraph,
                                             ze_graph_properties_2_t *pGraphProperties);

ze_result_t ZE_APICALL
zeGraphGetArgumentProperties(ze_graph_handle_t hGraph,
                             uint32_t argIndex,
                             ze_graph_argument_properties_t *pGraphArgumentProperties);

ze_result_t ZE_APICALL zeGraphSetArgumentValue(ze_graph_handle_t hGraph,
                                               uint32_t argIndex,
                                               const void *pArgValue);

ze_result_t ZE_APICALL zeAppendGraphInitialize(ze_command_list_handle_t hCommandList,
                                               ze_graph_handle_t hGraph,
                                               ze_event_handle_t hSignalEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents);

ze_result_t ZE_APICALL zeGraphInitialize(ze_graph_handle_t hGraph);

ze_result_t ZE_APICALL zeAppendGraphExecute(ze_command_list_handle_t hCommandList,
                                            ze_graph_handle_t hGraph,
                                            ze_graph_profiling_query_handle_t hProfilingQuery,
                                            ze_event_handle_t hSignalEvent,
                                            uint32_t numWaitEvents,
                                            ze_event_handle_t *phWaitEvents);

ze_result_t ZE_APICALL zeGraphGetNativeBinary(ze_graph_handle_t hGraph,
                                              size_t *pSize,
                                              uint8_t *pGraphNativeBinary);

ze_result_t ZE_APICALL zeGraphGetNativeBinary2(ze_graph_handle_t hGraph,
                                               size_t *pSize,
                                               const uint8_t **pGraphNativeBinary);

ze_result_t ZE_APICALL
zeDeviceGetGraphProperties(ze_device_handle_t hDevice,
                           ze_device_graph_properties_t *pDeviceGraphProperties);

ze_result_t ZE_APICALL
zeDeviceGetGraphProperties2(ze_device_handle_t hDevice,
                            ze_device_graph_properties_2_t *pDeviceGraphProperties2);

ze_result_t ZE_APICALL
zeGraphGetArgumentMetadata(ze_graph_handle_t hGraph,
                           uint32_t argIndex,
                           ze_graph_argument_metadata_t *pGraphArgumentMetadata);

ze_result_t ZE_APICALL
zeGraphGetArgumentProperties2(ze_graph_handle_t hGraph,
                              uint32_t argIndex,
                              ze_graph_argument_properties_2_t *pGraphArgumentProperties);

ze_result_t ZE_APICALL
zeGraphGetArgumentProperties3(ze_graph_handle_t hGraph,
                              uint32_t argIndex,
                              ze_graph_argument_properties_3_t *pGraphArgumentProperties);

ze_result_t ZE_APICALL
zeGraphQueryNetworkCreate(ze_context_handle_t hContext,
                          ze_device_handle_t hDevice,
                          const ze_graph_desc_t *desc,
                          ze_graph_query_network_handle_t *phGraphQueryNetwork);

ze_result_t ZE_APICALL
zeGraphQueryNetworkDestroy(ze_graph_query_network_handle_t hGraphQueryNetwork);

ze_result_t ZE_APICALL
zeGraphQueryNetworkGetSupportedLayers(ze_graph_query_network_handle_t hGraphQueryNetwork,
                                      size_t *pSize,
                                      char *pSupportedLayers);

ze_result_t ZE_APICALL zeGraphBuildLogGetString(ze_graph_handle_t hGraph,
                                                uint32_t *pSize,
                                                char *pBuildLog);

ze_result_t ZE_APICALL zeGraphCreate2(ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      const ze_graph_desc_2_t *desc,
                                      ze_graph_handle_t *phGraph);

ze_result_t ZE_APICALL
zeGraphQueryNetworkCreate2(ze_context_handle_t hContext,
                           ze_device_handle_t hDevice,
                           const ze_graph_desc_2_t *desc,
                           ze_graph_query_network_handle_t *phGraphQueryNetwork);

ze_result_t ZE_APICALL zeGraphQueryContextMemory(ze_context_handle_t hContext,
                                                 ze_graph_memory_query_type_t type,
                                                 ze_graph_memory_query_t *query);

ze_result_t ZE_APICALL
zeGraphProfilingPoolCreate(ze_graph_handle_t hGraph,
                           uint32_t count,
                           ze_graph_profiling_pool_handle_t *phProfilingPool);

ze_result_t ZE_APICALL zeGraphProfilingPoolDestroy(ze_graph_profiling_pool_handle_t hProfilingPool);

ze_result_t ZE_APICALL
zeGraphProfilingQueryCreate(ze_graph_profiling_pool_handle_t hProfilingPool,
                            uint32_t index,
                            ze_graph_profiling_query_handle_t *phProfilingQuery);

ze_result_t ZE_APICALL
zeGraphProfilingQueryDestroy(ze_graph_profiling_query_handle_t hProfilingQuery);

ze_result_t ZE_APICALL
zeGraphProfilingQueryGetData(ze_graph_profiling_query_handle_t hProfilingQuery,
                             ze_graph_profiling_type_t type,
                             uint32_t *pSize,
                             uint8_t *pData);

ze_result_t ZE_APICALL zeDeviceGetProfilingDataProperties(
    ze_device_handle_t hDevice,
    ze_device_profiling_data_properties_t *pDeviceProfilingDataProperties);

ze_result_t ZE_APICALL
zeGraphProfilingLogGetString(ze_graph_profiling_query_handle_t hProfilingQuery,
                             uint32_t *pSize,
                             char *pProfilingLog);

ze_result_t ZE_APICALL zeGraphCompilerGetSupportedOptions(ze_device_handle_t hDevice,
                                                          ze_npu_options_type_t type,
                                                          size_t *pSize,
                                                          char *pSupportedOptions);

ze_result_t ZE_APICALL zeGraphCompilerIsOptionSupported(ze_device_handle_t hDevice,
                                                        ze_npu_options_type_t type,
                                                        const char *pOption,
                                                        const char *pValue);

ze_result_t ZE_APICALL zeGraphCreate3(ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      const ze_graph_desc_2_t *desc,
                                      ze_graph_handle_t *phGraph,
                                      ze_graph_build_log_handle_t *phGraphBuildLog);

ze_result_t ZE_APICALL zeGraphBuildLogGetString2(ze_graph_build_log_handle_t hGraphBuildLog,
                                                 uint32_t *pSize,
                                                 char *pBuildLog);

ze_result_t ZE_APICALL zeGraphBuildLogDestroy(ze_graph_build_log_handle_t hGraphBuildLog);

ze_result_t ZE_APICALL zeGraphGetProperties3(ze_graph_handle_t hGraph,
                                             ze_graph_properties_3_t *pGraphProperties);
} // namespace L0
