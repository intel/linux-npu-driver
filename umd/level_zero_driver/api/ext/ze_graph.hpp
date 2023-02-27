/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero/ze_graph_ext.h"

namespace L0 {

ze_result_t ZE_APICALL zeGraphCreate(ze_context_handle_t hContext,
                                     ze_device_handle_t hDevice,
                                     const ze_graph_desc_t *pDesc,
                                     ze_graph_handle_t *phGraph);

ze_result_t ZE_APICALL zeGraphDestroy(ze_graph_handle_t hGraph);

ze_result_t ZE_APICALL zeGraphGetNativeBinary(ze_graph_handle_t hGraph,
                                              size_t *pSize,
                                              uint8_t *pGraphNativeBinary);

ze_result_t ZE_APICALL zeGraphGetProperties(ze_graph_handle_t hGraph,
                                            ze_graph_properties_t *pGraphProperties);

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

ze_result_t ZE_APICALL zeAppendGraphExecute(ze_command_list_handle_t hCommandList,
                                            ze_graph_handle_t hGraph,
                                            ze_graph_profiling_query_handle_t hProfilingQuery,
                                            ze_event_handle_t hSignalEvent,
                                            uint32_t numWaitEvents,
                                            ze_event_handle_t *phWaitEvents);

ze_result_t ZE_APICALL
zeDeviceGetGraphProperties(ze_device_handle_t hDevice,
                           ze_device_graph_properties_t *pDeviceGraphProperties);

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

} // namespace L0
