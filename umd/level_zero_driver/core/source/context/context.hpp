/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/zet_api.h>

struct _ze_context_handle_t {};

namespace L0 {
struct DriverHandle;

struct Context : _ze_context_handle_t {
    Context(DriverHandle *driverHandle, std::unique_ptr<VPU::VPUDeviceContext> ctx)
        : driverHandle(driverHandle)
        , ctx(std::move(ctx)){};
    ~Context() = default;

    ze_result_t destroy();
    ze_result_t getStatus();
    DriverHandle *getDriverHandle();

    ze_result_t checkMemInputs(size_t size, size_t alignment, void **ptr);
    ze_result_t
    allocHostMem(ze_host_mem_alloc_flags_t flags, size_t size, size_t alignment, void **ptr);
    ze_result_t allocSharedMem(ze_device_handle_t hDevice,
                               ze_device_mem_alloc_flags_t flagsDev,
                               ze_host_mem_alloc_flags_t flagsHost,
                               size_t size,
                               size_t alignment,
                               void **ptr);
    ze_result_t allocDeviceMem(ze_device_handle_t hDevice,
                               ze_device_mem_alloc_flags_t flags,
                               size_t size,
                               size_t alignment,
                               void **ptr);
    ze_result_t freeMem(void *ptr);

    ze_result_t getMemAllocProperties(const void *ptr,
                                      ze_memory_allocation_properties_t *pMemAllocProperties,
                                      ze_device_handle_t *phDevice);

    ze_result_t getMemAddressRange(const void *ptr, void **basePtr, size_t *pSize);

    ze_result_t createCommandQueue(ze_device_handle_t hDevice,
                                   const ze_command_queue_desc_t *desc,
                                   ze_command_queue_handle_t *commandQueue);
    ze_result_t createCommandList(ze_device_handle_t hDevice,
                                  const ze_command_list_desc_t *desc,
                                  ze_command_list_handle_t *commandList);
    ze_result_t createEventPool(const ze_event_pool_desc_t *desc,
                                uint32_t numDevices,
                                ze_device_handle_t *phDevices,
                                ze_event_pool_handle_t *phEventPool);

    ze_result_t activateMetricGroups(zet_device_handle_t hDevice,
                                     uint32_t count,
                                     zet_metric_group_handle_t *phMetricGroups);
    ze_result_t createMetricQueryPool(zet_device_handle_t hDevice,
                                      zet_metric_group_handle_t hMetricGroup,
                                      const zet_metric_query_pool_desc_t *desc,
                                      zet_metric_query_pool_handle_t *phMetricQueryPool);
    ze_result_t metricStreamerOpen(zet_device_handle_t hDevice,
                                   zet_metric_group_handle_t hMetricGroup,
                                   zet_metric_streamer_desc_t *desc,
                                   ze_event_handle_t hNotificationEvent,
                                   zet_metric_streamer_handle_t *phMetricStreamer);

    inline ze_context_handle_t toHandle() { return this; }
    static Context *fromHandle(ze_context_handle_t handle) {
        return static_cast<Context *>(handle);
    }
    VPU::VPUDeviceContext *getDeviceContext() const { return ctx.get(); }

  private:
    DriverHandle *driverHandle = nullptr;
    std::unique_ptr<VPU::VPUDeviceContext> ctx;
};

} // namespace L0
