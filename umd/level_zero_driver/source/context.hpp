/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

// IWYU pragma: no_include <bits/chrono.h>

#include <stddef.h>
#include <stdint.h>

#include "level_zero_driver/include/l0_handler.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <utility>
#include <ze_api.h>
#include <ze_context_npu_ext.h>
#include <ze_graph_ext.h>
#include <zet_api.h>

using namespace std::literals;

struct _ze_context_handle_t {};

namespace L0 {
struct DriverHandle;
struct Context;

struct ResourceCleaner {
    ResourceCleaner(Context *ctx, std::chrono::milliseconds timeout);
    ResourceCleaner(const ResourceCleaner &) = delete;
    ResourceCleaner(ResourceCleaner &&) = delete;
    ResourceCleaner &operator=(const ResourceCleaner &) = delete;
    ResourceCleaner &operator=(ResourceCleaner &&) = delete;
    ~ResourceCleaner();
    void setIdle();
    void setIdleTimeout(std::chrono::milliseconds timeout);

    std::mutex mutex;
    std::condition_variable cv;
    std::chrono::milliseconds idleTimeout = 30s;
    std::thread thread;

    enum class Action {
        NOP,
        PRUNE_AFTER_TIMEOUT,
        BREAK,
    } action = Action::NOP;
};

struct Context : _ze_context_handle_t {
    Context(DriverHandle *driverHandle, std::unique_ptr<VPU::VPUDeviceContext> ctx);
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
    ze_result_t
    importUserPtr(void *userPtr, size_t size, ze_host_mem_alloc_flags_t flags, void **ptr);
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

    ze_result_t setProperties(const ze_context_properties_npu_ext_t *pContextProperties);
    ze_result_t releaseMemory();

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

    void setIdle();
    void setIdlePruningTimeout(uint64_t timeout);

  private:
    DriverHandle *driverHandle = nullptr;
    std::unique_ptr<VPU::VPUDeviceContext> ctx;
    std::unordered_map<void *, std::unique_ptr<IContextObject>> objects;
    std::mutex mutex;
    std::mutex cleanerMutex;
    std::unique_ptr<ResourceCleaner> resourceCleaner;
    std::chrono::milliseconds idleTimeout = 30s;
};

} // namespace L0
