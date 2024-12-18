/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/device/vpu_device.hpp"

#include "api/vpu_jsm_api.h"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <cerrno>
#include <charconv>
#include <exception>
#include <filesystem>
#include <sys/types.h>
#include <system_error>
#include <uapi/drm/ivpu_accel.h>
#include <utility>

namespace VPU {
VPUDevice::VPUDevice(std::string devPath, OsInterface &osInfc)
    : devPath(std::move(devPath))
    , osInfc(osInfc) {}

bool VPUDevice::initializeCaps(VPUDriverApi *drvApi) {
    try {
        uint32_t deviceId = drvApi->getDeviceParam<uint32_t>(DRM_IVPU_PARAM_DEVICE_ID);
        LOG(DEVICE, "PCI device ID: %#x", deviceId);

        hwInfo = getHwInfoByDeviceId(deviceId);
        hwInfo.deviceId = deviceId;
        hwInfo.deviceRevision = drvApi->getDeviceParam<uint32_t>(DRM_IVPU_PARAM_DEVICE_REVISION);
        hwInfo.maxHardwareContexts = drvApi->getDeviceParam<uint32_t>(DRM_IVPU_PARAM_NUM_CONTEXTS);
        hwInfo.coreClockRate = drvApi->getDeviceParam<uint32_t>(DRM_IVPU_PARAM_CORE_CLOCK_RATE);
        hwInfo.platformType = drvApi->getDeviceParam<uint32_t>(DRM_IVPU_PARAM_PLATFORM_TYPE);
        hwInfo.baseLowAddress = drvApi->getDeviceParam(DRM_IVPU_PARAM_CONTEXT_BASE_ADDRESS);
        hwInfo.fwMappedInferenceVersion =
            drvApi->getDeviceParam(DRM_IVPU_PARAM_FW_API_VERSION, hwInfo.fwMappedInferenceIndex);
        LOG(DEVICE, "Base address of device is %#lx", hwInfo.baseLowAddress);

        uint32_t tileConfigParam = drvApi->getDeviceParam<uint32_t>(DRM_IVPU_PARAM_TILE_CONFIG);
        hwInfo.tileConfig = ~tileConfigParam & hwInfo.tileFuseMask;
    } catch (const std::exception &err) {
        LOG_E("Failed to initialize hardware info, error: %s", err.what());
        return false;
    }

    if (drvApi->checkPrimeBuffersCapability())
        hwInfo.primeBuffersCapability = true;
    if (drvApi->checkDeviceCapability(DRM_IVPU_CAP_METRIC_STREAMER))
        hwInfo.metricStreamerCapability = true;
    if (drvApi->checkDeviceCapability(DRM_IVPU_CAP_DMA_MEMORY_RANGE))
        hwInfo.dmaMemoryRangeCapability = true;

    mappedInferenceVersion = drvApi->getFWComponentVersion(hwInfo.fwMappedInferenceIndex);
    jsmApiVersion = drvApi->getFWComponentVersion(hwInfo.fwJsmCmdApiVerIndex);

    return true;
}

bool VPUDevice::initializeMetricGroups(VPUDriverApi *drvApi) {
    uint64_t metricGroupMask = -1llu;
    drm_ivpu_metric_streamer_get_data get_info_params = {};

    // to obtain all metric groups
    get_info_params.metric_group_mask = metricGroupMask;
    get_info_params.buffer_ptr = 0;
    get_info_params.buffer_size = 0;

    if (drvApi->metricStreamerGetInfo(&get_info_params)) {
        LOG_W("Failed to call metric_streamer_get_info ioctl. -errno: %d. Size information not "
              "obtained.",
              errno);
        return false;
    }

    uint64_t metricGroupsInfoSize = get_info_params.data_size;
    LOG(METRIC, "Metric Groups Info Size: %lu", metricGroupsInfoSize);

    if (metricGroupsInfoSize == 0) {
        LOG_W("No metric groups received");
        return false;
    }

    std::vector<uint8_t> metricGroupsInfo;
    metricGroupsInfo.resize(metricGroupsInfoSize, 0);

    get_info_params = {};
    get_info_params.metric_group_mask = metricGroupMask;
    get_info_params.buffer_ptr = reinterpret_cast<uint64_t>(metricGroupsInfo.data());
    get_info_params.buffer_size = metricGroupsInfoSize;

    if (drvApi->metricStreamerGetInfo(&get_info_params)) {
        LOG_E("Failed to call metric_streamer_get_info ioctl. -errno: %d. No data obtained.",
              errno);
        return false;
    }

    vpu_jsm_metric_counter_descriptor *counter_desc = nullptr;
    vpu_jsm_metric_group_descriptor *group_desc =
        reinterpret_cast<vpu_jsm_metric_group_descriptor *>(metricGroupsInfo.data());

    int numberOfMetricGroups = 0;

    while (true) {
        numberOfMetricGroups++;
        GroupInfo groupInfo = {};

        uint32_t firstMetricOffset = group_desc->next_metric_counter_info_offset;
        groupInfo.groupIndex = group_desc->group_id;
        groupInfo.metricCount = group_desc->num_counters;
        groupInfo.domain = group_desc->domain;
        uint32_t groupNameStringSize = group_desc->name_string_size;
        uint32_t groupDescriptionStringSize = group_desc->description_string_size;

        if (groupNameStringSize != 0) {
            groupInfo.metricGroupName = reinterpret_cast<char *>(
                reinterpret_cast<uint64_t>(group_desc) + sizeof(vpu_jsm_metric_group_descriptor));
        }

        if (groupDescriptionStringSize != 0) {
            groupInfo.metricGroupDescription = reinterpret_cast<char *>(
                reinterpret_cast<uint64_t>(group_desc) + sizeof(vpu_jsm_metric_group_descriptor) +
                groupNameStringSize);
        }

        LOG(METRIC, "========================================");
        LOG(METRIC, "Metric Group #%d", numberOfMetricGroups);

        LOG(METRIC, "next_metric_group_info_offset: %u", group_desc->next_metric_group_info_offset);
        LOG(METRIC, "next_metric_counter_info_offset: %u", firstMetricOffset);
        LOG(METRIC, "group_id: %u", groupInfo.groupIndex);
        LOG(METRIC, "num_counters: %u", groupInfo.metricCount);
        LOG(METRIC, "metric_group_data_size: %u", group_desc->metric_group_data_size);
        LOG(METRIC, "domain: %u", groupInfo.domain);
        LOG(METRIC, "name_string_size: %u", groupNameStringSize);
        LOG(METRIC, "description_string_size: %u", groupDescriptionStringSize);
        LOG(METRIC, "metric_group_name: %s", groupInfo.metricGroupName.c_str());
        LOG(METRIC, "metric_group_description: %s", groupInfo.metricGroupDescription.c_str());

        counter_desc = reinterpret_cast<vpu_jsm_metric_counter_descriptor *>(
            reinterpret_cast<uint64_t>(group_desc) + firstMetricOffset);

        for (uint i = 0; i < groupInfo.metricCount; i++) {
            CounterInfo counterInfo = {};

            counterInfo.tier = counter_desc->tier;
            counterInfo.metricType =
                static_cast<CounterInfo::MetricType>(counter_desc->metric_type);
            counterInfo.valueType =
                static_cast<CounterInfo::ValueType>(counter_desc->metric_value_type);
            uint32_t nameStringSize = counter_desc->name_string_size;
            uint32_t descriptionStringSize = counter_desc->description_string_size;
            uint32_t componentStringSize = counter_desc->component_string_size;
            uint32_t unitsStringSize = counter_desc->units_string_size;

            if (nameStringSize != 0) {
                counterInfo.metricName =
                    reinterpret_cast<char *>(reinterpret_cast<uint64_t>(counter_desc) +
                                             sizeof(vpu_jsm_metric_counter_descriptor));
            }

            if (descriptionStringSize != 0) {
                counterInfo.metricDescription = reinterpret_cast<char *>(
                    reinterpret_cast<uint64_t>(counter_desc) +
                    sizeof(vpu_jsm_metric_counter_descriptor) + nameStringSize);
            }

            if (componentStringSize != 0) {
                counterInfo.component =
                    reinterpret_cast<char *>(reinterpret_cast<uint64_t>(counter_desc) +
                                             sizeof(vpu_jsm_metric_counter_descriptor) +
                                             nameStringSize + descriptionStringSize);
            }

            if (unitsStringSize != 0) {
                counterInfo.units = reinterpret_cast<char *>(
                    reinterpret_cast<uint64_t>(counter_desc) +
                    sizeof(vpu_jsm_metric_counter_descriptor) + nameStringSize +
                    descriptionStringSize + componentStringSize);
            }

            LOG(METRIC, "----------------------------------------");
            LOG(METRIC,
                "next_metric_counter_info_offset: %u",
                counter_desc->next_metric_counter_info_offset);
            LOG(METRIC, "metric_data_offset: %u", counter_desc->metric_data_offset);
            LOG(METRIC, "metric_data_size: %u", counter_desc->metric_data_size);
            LOG(METRIC, "tier: %u", counterInfo.tier);
            LOG(METRIC, "metric_type: %u", counterInfo.metricType);
            LOG(METRIC, "value_type: %u", counterInfo.valueType);
            LOG(METRIC, "name_string_size: %u", nameStringSize);
            LOG(METRIC, "description_string_size: %u", descriptionStringSize);
            LOG(METRIC, "component_string_size: %u", componentStringSize);
            LOG(METRIC, "units_string_size: %u", unitsStringSize);
            LOG(METRIC, "metric_name: %s", counterInfo.metricName.c_str());
            LOG(METRIC, "metric_description: %s", counterInfo.metricDescription.c_str());
            LOG(METRIC, "component: %s", counterInfo.component.c_str());
            LOG(METRIC, "units: %s", counterInfo.units.c_str());

            groupInfo.counterInfo.push_back(counterInfo);

            counter_desc = reinterpret_cast<vpu_jsm_metric_counter_descriptor *>(
                reinterpret_cast<uint64_t>(counter_desc) +
                counter_desc->next_metric_counter_info_offset);
        }

        groupsInfo.push_back(groupInfo);

        if (group_desc->next_metric_group_info_offset) {
            group_desc = reinterpret_cast<vpu_jsm_metric_group_descriptor *>(
                reinterpret_cast<uint64_t>(group_desc) + group_desc->next_metric_group_info_offset);
        } else {
            break;
        }
    }

    return true;
}

bool VPUDevice::init(bool enableMetrics) {
    if (devPath.empty()) {
        LOG_W("Device node is null");
        return false;
    }

    auto drvApi = VPUDriverApi::openDriverApi(devPath, osInfc);
    if (drvApi == nullptr || !drvApi->isVpuDevice())
        return false;

    if (!initializeCaps(drvApi.get())) {
        LOG_W("Failed to initialize VPU device capabilities");
        return false;
    }

    if (enableMetrics && getCapMetricStreamer()) {
        if (!initializeMetricGroups(drvApi.get())) {
            LOG_W("Failed to initialize metric groups");
            return false;
        }
    }

    LOG(DEVICE, "VPU device initialized successfully (%s)", devPath.c_str());
    return true;
}

const VPUHwInfo &VPUDevice::getHwInfo() const {
    return hwInfo;
}

const std::vector<GroupInfo> VPUDevice::getMetricGroupsInfo() const {
    return groupsInfo;
}

bool VPUDevice::getCapMetricStreamer() const {
    return hwInfo.metricStreamerCapability;
}

bool VPUDevice::isConnected() {
    auto drvApi = VPUDriverApi::openDriverApi(devPath, osInfc);
    if (drvApi == nullptr || !drvApi->isVpuDevice())
        return false;

    try {
        drvApi->getDeviceParam<uint32_t>(DRM_IVPU_PARAM_ENGINE_HEARTBEAT);
        LOG(DEVICE, "Device connected");
        return true;

    } catch (const std::exception &e) {
        LOG_E("Device not connected");
        return false;
    }
}

std::unique_ptr<VPUDeviceContext> VPUDevice::createDeviceContext() {
    auto drvApi = VPUDriverApi::openDriverApi(devPath, osInfc);
    if (drvApi == nullptr) {
        LOG_E("Failed to allocate VPUDriverApi");
        return nullptr;
    }

    return std::make_unique<VPUDeviceContext>(std::move(drvApi), &hwInfo);
}

int VPUDevice::getBDF(uint32_t *domain, uint32_t *bus, uint32_t *dev, uint32_t *func) {
    auto drvApi = VPUDriverApi::openDriverApi(devPath, osInfc);
    if (drvApi == nullptr) {
        LOG_E("Failed to open openDriverApi");
        return -1;
    }

    std::string devLink = drvApi->getDeviceLink();
    if (devLink.empty()) {
        LOG_E("Failed to find deviec link path");
        return -1;
    }

    // Expect to get devLink = '../../devices/pci0000:00/0000:00:0b.0/accel/accel0'
    //                                            1. Try find 'accel' ^
    //          2. Check if BDF string (-13chars) exists ^^^^^^^^^^^^^
    size_t pos = devLink.find("accel");
    if (pos < 13 || pos >= devLink.size()) {
        LOG_E("Failed to find 'accel' in device link path");
        return -1;
    }

    std::from_chars_result ret;
    ret = std::from_chars(&devLink.data()[pos - 13], &devLink.data()[pos - 9], *domain, 16);
    if (ret.ec != std::errc())
        LOG_W("Failed to get domain from '%s'", devLink.data());

    ret = std::from_chars(&devLink.data()[pos - 8], &devLink.data()[pos - 6], *bus, 16);
    if (ret.ec != std::errc())
        LOG_W("Failed to get bus from '%s'", devLink.data());

    ret = std::from_chars(&devLink.data()[pos - 5], &devLink.data()[pos - 3], *dev, 16);
    if (ret.ec != std::errc())
        LOG_W("Failed to get dev from '%s'", devLink.data());

    ret = std::from_chars(&devLink.data()[pos - 2], &devLink.data()[pos - 1], *func, 16);
    if (ret.ec != std::errc())
        LOG_W("Failed to get func from '%s'", devLink.data());

    return 0;
}

bool VPUDevice::getActiveTime(uint64_t &activeTimeUs) {
    auto drvApi = VPUDriverApi::openDriverApi(devPath, osInfc);
    if (drvApi == nullptr) {
        LOG_E("Failed to open openDriverApi");
        return false;
    }
    std::string devSysPath = drvApi->getSysDeviceAbsolutePath();

    auto activeTime = osInfc.osiReadFile(devSysPath + "npu_busy_time_us");
    if (activeTime.empty())
        return false;

    auto [ptr, ec] =
        std::from_chars(activeTime.data(), activeTime.data() + activeTime.size(), activeTimeUs);
    if (ec != std::errc()) {
        auto err = std::make_error_condition(ec);
        LOG_E("Failed to read active driver time: %s", err.message().c_str());
        return false;
    }
    return true;
}
} // namespace VPU
