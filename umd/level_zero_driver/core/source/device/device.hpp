/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/core/source/driver/driver.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/ext/source/graph/graph.hpp"
#include "level_zero_driver/tools/source/metrics/metric.hpp"

#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/zet_api.h>
#include <level_zero/zes_api.h>
#include <level_zero/ze_graph_ext.h>
#include <level_zero/ze_intel_npu_uuid.h>

struct _ze_device_handle_t {};

namespace L0 {

struct DriverHandle;
struct MetricContext;
struct MetricGroup;
struct CommandQueue;
struct CommandList;

struct Device : _ze_device_handle_t {
    Device(DriverHandle *driverHandle, std::unique_ptr<VPU::VPUDevice> device);

    ze_result_t getP2PProperties(ze_device_handle_t hPeerDevice,
                                 ze_device_p2p_properties_t *pP2PProperties);
    ze_result_t getProperties(ze_device_properties_t *pDeviceProperties);
    ze_result_t getProperties(zes_device_properties_t *pDeviceProperties);
    ze_result_t getSubDevices(uint32_t *pCount, ze_device_handle_t *phSubdevices);
    ze_result_t setIntermediateCacheConfig(ze_cache_config_flags_t cacheConfig);
    ze_result_t setLastLevelCacheConfig(ze_cache_config_flags_t cacheConfig);
    ze_result_t getMemoryProperties(uint32_t *pCount,
                                    ze_device_memory_properties_t *pMemProperties);
    ze_result_t getGetExternalMemoryProperties(
        ze_device_external_memory_properties_t *pExternalMemoryProperties);
    ze_result_t
    getMemoryAccessProperties(ze_device_memory_access_properties_t *pMemAccessProperties);
    ze_result_t getDeviceImageProperties(ze_device_image_properties_t *pDeviceImageProperties);
    ze_result_t
    getDeviceComputeProperties(ze_device_compute_properties_t *pDeviceComputeProperties);
    ze_result_t getCommandQueueGroupProperties(
        uint32_t *pCount,
        ze_command_queue_group_properties_t *pCommandQueueGroupProperties);
    ze_command_queue_group_property_flags_t getCommandQeueueGroupFlags(uint32_t ordinal);
    ze_result_t getStatus() const;
    ze_result_t getGlobalTimestamps(uint64_t *hostTimestamp, uint64_t *deviceTimestamp);
    ze_result_t getPciProperties(ze_pci_ext_properties_t *pPciProperties);

    DriverHandle *getDriverHandle();
    const char *getDeviceMemoryName() const;
    VPU::VPUDevice *getVPUDevice();

    ze_result_t metricGroupGet(uint32_t *pCount, zet_metric_group_handle_t *phMetricGroups);
    ze_result_t
    activateMetricGroups(int vpuFd, uint32_t count, zet_metric_group_handle_t *phMetricGroups);
    const std::shared_ptr<MetricContext> getMetricContext() const;
    bool isMetricsLoaded() const { return metricsLoaded; }
    bool isMetricGroupAvailable(MetricGroup *metricGroup) const;

    static Device *fromHandle(ze_device_handle_t handle) { return static_cast<Device *>(handle); }
    inline ze_device_handle_t toHandle() { return this; }

    static ze_result_t jobStatusToResult(const std::vector<std::shared_ptr<VPU::VPUJob>> &jobs) {
        for (const auto &job : jobs) {
            auto jobStatus = job->getStatus();
            switch (jobStatus) {
            case DRM_IVPU_JOB_STATUS_SUCCESS:
                break;
            case DRM_IVPU_JOB_STATUS_ABORTED:
                return ZE_RESULT_ERROR_DEVICE_LOST;
            default:
                return ZE_RESULT_ERROR_UNKNOWN;
            }
        }
        return ZE_RESULT_SUCCESS;
    }

  protected:
    /**
       A container for keeping all metric groups available for the context.
     */
    std::vector<std::shared_ptr<MetricGroup>> metricGroups;

    /**
       Load data into metricGroups map
     */
    void loadMetricGroupsInfo(std::vector<VPU::GroupInfo> &metricGroupsInfo);

  private:
    DriverHandle *driverHandle = nullptr;
    std::unique_ptr<VPU::VPUDevice> vpuDevice = nullptr;

    std::shared_ptr<MetricContext> metricContext = nullptr;
    bool metricsLoaded = false;

    const uint NS_IN_SEC = 1'000'000'000;

    template <class T>
    using UniquePtrT = std::unique_ptr<T, std::function<void(T *)>>;
    ze_result_t createInternalJob(UniquePtrT<Context> &context,
                                  CommandQueue **commandQueue,
                                  CommandList **commandList);
};

} // namespace L0
