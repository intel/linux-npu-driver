/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/core/source/driver/driver.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/tools/source/metrics/metric.hpp"

#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/zet_api.h>

struct _ze_device_handle_t {};

namespace L0 {

struct DriverHandle;
struct MetricContext;
struct MetricGroup;

struct Device : _ze_device_handle_t {
    Device(DriverHandle *driverHandle, VPU::VPUDevice *vpuDevice);
    ~Device();

    // Block copy constructor and assign operator.
    Device &operator=(const Device &) = delete;
    Device(const Device &rhs) = delete;

    ze_result_t createCommandList(const ze_command_list_desc_t *desc,
                                  ze_command_list_handle_t *commandList,
                                  VPU::VPUDeviceContext *ctx);
    ze_result_t createCommandQueue(const ze_command_queue_desc_t *desc,
                                   ze_command_queue_handle_t *commandQueue,
                                   VPU::VPUDeviceContext *ctx);
    ze_result_t getP2PProperties(ze_device_handle_t hPeerDevice,
                                 ze_device_p2p_properties_t *pP2PProperties);
    ze_result_t getProperties(ze_device_properties_t *pDeviceProperties);
    ze_result_t getSubDevices(uint32_t *pCount, ze_device_handle_t *phSubdevices);
    ze_result_t setIntermediateCacheConfig(ze_cache_config_flags_t cacheConfig);
    ze_result_t setLastLevelCacheConfig(ze_cache_config_flags_t cacheConfig);
    ze_result_t getMemoryProperties(uint32_t *pCount,
                                    ze_device_memory_properties_t *pMemProperties);
    ze_result_t
    getMemoryAccessProperties(ze_device_memory_access_properties_t *pMemAccessProperties);
    ze_result_t getDeviceImageProperties(ze_device_image_properties_t *pDeviceImageProperties);
    ze_result_t getCommandQueueGroupProperties(
        uint32_t *pCount,
        ze_command_queue_group_properties_t *pCommandQueueGroupProperties);
    ze_result_t getStatus() const;

    DriverHandle *getDriverHandle();
    const char *getDeviceMemoryName() const;
    VPU::VPUDevice *getVPUDevice();

    ze_result_t metricGroupGet(uint32_t *pCount, zet_metric_group_handle_t *phMetricGroups);
    ze_result_t activateMetricGroups(uint32_t contextId,
                                     uint32_t count,
                                     zet_metric_group_handle_t *phMetricGroups);
    const std::shared_ptr<MetricContext> getMetricContext() const;
    bool isMetricsLoaded() const { return metricsLoaded; }
    bool isMetricGroupAvailable(MetricGroup *metricGroup) const;

    static Device *fromHandle(ze_device_handle_t handle) { return static_cast<Device *>(handle); }
    inline ze_device_handle_t toHandle() { return this; }

    static Device *create(DriverHandle *driverHandle, VPU::VPUDevice *vpuDevice);

  private:
    void loadDeviceProperties();

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
    VPU::VPUDevice *vpuDevice = nullptr;

    ze_device_properties_t properties = {};

    /**
       Returns if given ordinal is a copy only engine group.
       @param [in]: engGrpOrdinal: Engine group ordinal to check.
       @param [out]: True when the return value is valid, false otherwise.
       @return true if given engine ordinal is for copy only group.
     */
    bool isCopyOnlyEngineGroup(uint32_t enGrpOrdinal, bool &outputValid);

    std::shared_ptr<MetricContext> metricContext = nullptr;
    bool metricsLoaded = false;

    // According to SAS this could be used for NCE compute tiles in the future
    uint32_t numSubDevices = 0;
    std::vector<Device *> subDevices;

    const int NS_IN_SEC = 1'000'000'000;
};

} // namespace L0
