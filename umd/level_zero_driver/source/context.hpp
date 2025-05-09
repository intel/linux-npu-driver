/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/include/l0_handler.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_ext.h>
#include <level_zero/zet_api.h>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>

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

    ze_result_t checkMemInputs(VPU::VPUBufferObject::Location location,
                               size_t size,
                               size_t alignment,
                               void **ptr);

    ze_result_t allocMemory(size_t size,
                            size_t alignment,
                            void **ptr,
                            VPU::VPUBufferObject::Location location,
                            VPU::VPUBufferObject::Type type);
    ze_result_t importMemory(VPU::VPUBufferObject::Location type, int32_t fd, void **ptr);
    ze_result_t freeMem(void *ptr);

    ze_result_t getMemAllocProperties(const void *ptr,
                                      ze_memory_allocation_properties_t *pMemAllocProperties,
                                      ze_device_handle_t *phDevice);

    ze_result_t getMemAddressRange(const void *ptr, void **basePtr, size_t *pSize);

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

    ze_result_t queryContextMemory(ze_graph_memory_query_type_t type,
                                   ze_graph_memory_query_t *query);

    inline ze_context_handle_t toHandle() { return this; }
    static Context *fromHandle(ze_context_handle_t handle) {
        return static_cast<Context *>(handle);
    }
    VPU::VPUDeviceContext *getDeviceContext() const { return ctx.get(); }

    void appendObject(std::unique_ptr<IContextObject> obj) {
        std::lock_guard<std::mutex> lock(mutex);
        objects.emplace(obj.get(), std::move(obj));
    }

    void removeObject(IContextObject *obj) {
        ctx->removeExpiredInternalBuffers();
        std::lock_guard<std::mutex> lock(mutex);
        objects.erase(obj);
    }

  private:
    DriverHandle *driverHandle = nullptr;
    std::unique_ptr<VPU::VPUDeviceContext> ctx;
    std::unordered_map<void *, std::unique_ptr<IContextObject>> objects;
    std::mutex mutex;
};

} // namespace L0
