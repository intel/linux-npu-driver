/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"

#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "api/vpu_jsm_api.h"

#include <cerrno>
#include <limits>
#include <sys/mman.h>
#include <uapi/drm/ivpu_accel.h>
#include <cassert>

namespace VPU {
VPUDevice::VPUDevice(std::string devnode, OsInterface &osInfc)
    : devnode(std::move(devnode))
    , osInfc(osInfc) {}

bool VPUDevice::initializeCaps(VPUDriverApi *drvApi) {
    struct drm_ivpu_param arg = {};
    uint32_t deviceId;

    arg.param = DRM_IVPU_PARAM_DEVICE_ID;
    if (drvApi->getDeviceParam(&arg)) {
        LOG_E("Failed to call device config ioctl. -errno: %d", errno);
        return false;
    }

    deviceId = safe_cast<uint32_t>(arg.value);

    LOG_I("Pci device ID: %#llx", arg.value);
    for (auto &info : VPUHwInfos) {
        if (info.IsDeviceId(deviceId)) {
            hwInfo = info;
            hwInfo.deviceId = deviceId;
            break;
        }
    }

    if (hwInfo.deviceId == 0) {
        LOG_E("Failed to find a device with PCI ID: %#llx", arg.value);
        return false;
    }

    arg.param = DRM_IVPU_PARAM_DEVICE_REVISION;
    if (drvApi->getDeviceParam(&arg)) {
        LOG_E("Failed to get device revision using ioctl. -errno: %d", errno);
        return false;
    }
    hwInfo.deviceRevision = safe_cast<uint32_t>(arg.value);

    arg.param = DRM_IVPU_PARAM_NUM_CONTEXTS;
    if (drvApi->getDeviceParam(&arg)) {
        LOG_E("Failed to get number of contexts using ioctl. -errno: %d", errno);
        return false;
    }
    hwInfo.maxHardwareContexts = safe_cast<uint32_t>(arg.value);

    arg.param = DRM_IVPU_PARAM_CORE_CLOCK_RATE;
    if (drvApi->getDeviceParam(&arg)) {
        LOG_E("Failed to get core clock rate using ioctl. -errno: %d", errno);
        return false;
    }
    hwInfo.coreClockRate = safe_cast<uint32_t>(arg.value);

    arg.param = DRM_IVPU_PARAM_PLATFORM_TYPE;
    if (drvApi->getDeviceParam(&arg)) {
        LOG_E("Failed to get platform type using ioctl. -errno: %d", errno);
        return false;
    }
    hwInfo.platformType = safe_cast<uint32_t>(arg.value);

    arg.param = DRM_IVPU_PARAM_TILE_CONFIG;
    if (drvApi->getDeviceParam(&arg)) {
        LOG_E("Failed to get tile config using ioctl. -errno: %d", errno);
        return false;
    }
    hwInfo.tileConfig = (~safe_cast<uint32_t>(arg.value)) & hwInfo.tileFuseMask;

    arg.param = DRM_IVPU_PARAM_CONTEXT_BASE_ADDRESS;
    if (drvApi->getDeviceParam(&arg) != 0) {
        LOG_E("Failed to get context base address using ioctl. -errno: %d", errno);
        return false;
    }

    hwInfo.baseLowAddres = arg.value;
    LOG_I("Base address of device is %#lx", hwInfo.baseLowAddres);

    arg.param = DRM_IVPU_PARAM_CAPABILITIES;
    arg.index = DRM_IVPU_CAP_METRIC_STREAMER;
    if (drvApi->getDeviceParam(&arg)) {
        LOG_W("Failed to get metric streamer capabilities using ioctl. -errno: %d", errno);
    } else {
        capMetricStreamer = safe_cast<uint32_t>(arg.value);
    }

    return true;
}

bool VPUDevice::initializeMetricGroups(VPUDriverApi *drvApi) {
    if (capMetricStreamer != 1) {
        LOG_W("Metrics are not supported.");
        return true;
    }

    uint64_t metricGroupMask = -1llu;
    drm_ivpu_metric_streamer_get_data get_info_params = {};

    // to obtain all metric groups
    get_info_params.metric_group_mask = metricGroupMask;
    get_info_params.size = 0;
    get_info_params.buffer_ptr = 0;

    if (drvApi->metricStreamerGetInfo(&get_info_params)) {
        LOG_W("Failed to call metric_streamer_get_info ioctl. -errno: %d. Size information not "
              "obtained.",
              errno);
        return false;
    }

    uint64_t metricGroupsInfoSize = get_info_params.size;
    LOG_V("Metric Groups Info Size: %lu\n", metricGroupsInfoSize);

    if (metricGroupsInfoSize == 0) {
        LOG_W("No metric groups received.");
        return false;
    }

    std::vector<uint8_t> metricGroupsInfo;
    metricGroupsInfo.resize(metricGroupsInfoSize, 0);

    get_info_params = {};
    get_info_params.metric_group_mask = metricGroupMask;
    get_info_params.size = metricGroupsInfoSize;
    get_info_params.buffer_ptr = reinterpret_cast<uint64_t>(metricGroupsInfo.data());

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

        LOG_V("========================================\n");
        LOG_V("Metric Group #%d\n", numberOfMetricGroups);

        LOG_V("next_metric_group_info_offset: %u\n", group_desc->next_metric_group_info_offset);
        LOG_V("next_metric_counter_info_offset: %u\n", firstMetricOffset);
        LOG_V("group_id: %u\n", groupInfo.groupIndex);
        LOG_V("num_counters: %u\n", groupInfo.metricCount);
        LOG_V("metric_group_data_size: %u\n", group_desc->metric_group_data_size);
        LOG_V("domain: %u\n", groupInfo.domain);
        LOG_V("name_string_size: %u\n", groupNameStringSize);
        LOG_V("description_string_size: %u\n", groupDescriptionStringSize);
        LOG_V("metric_group_name: %s\n", groupInfo.metricGroupName.c_str());
        LOG_V("metric_group_description: %s\n", groupInfo.metricGroupDescription.c_str());

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

            LOG_V("----------------------------------------\n");
            LOG_V("next_metric_counter_info_offset: %u\n",
                  counter_desc->next_metric_counter_info_offset);
            LOG_V("metric_data_offset: %u\n", counter_desc->metric_data_offset);
            LOG_V("metric_data_size: %u\n", counter_desc->metric_data_size);
            LOG_V("tier: %u\n", counterInfo.tier);
            LOG_V("metric_type: %u\n", counterInfo.metricType);
            LOG_V("value_type: %u\n", counterInfo.valueType);
            LOG_V("name_string_size: %u\n", nameStringSize);
            LOG_V("description_string_size: %u\n", descriptionStringSize);
            LOG_V("component_string_size: %u\n", componentStringSize);
            LOG_V("units_string_size: %u\n", unitsStringSize);
            LOG_V("metric_name: %s\n", counterInfo.metricName.c_str());
            LOG_V("metric_description: %s\n", counterInfo.metricDescription.c_str());
            LOG_V("component: %s\n", counterInfo.component.c_str());
            LOG_V("units: %s\n", counterInfo.units.c_str());

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

bool VPUDevice::init() {
    LOG_V("Initializing VPU device.");

    if (devnode.empty()) {
        LOG_W("Device node is null.");
        return false;
    }

    auto drvApi = VPUDriverApi::openDriverApi(devnode, osInfc);
    if (drvApi == nullptr || !drvApi->isVpuDevice())
        return false;

    if (!initializeCaps(drvApi.get())) {
        LOG_W("Failed to initialize VPU device capabilities.");
        return false;
    }

    if (!initializeMetricGroups(drvApi.get())) {
        LOG_W("Failed to initialize metric groups.");
        return false;
    }

    LOG_V("VPU device initialized successfully.");
    return true;
}

const VPUHwInfo &VPUDevice::getHwInfo() const {
    return hwInfo;
}

const std::vector<GroupInfo> VPUDevice::getMetricGroupsInfo() const {
    return groupsInfo;
}

uint32_t VPUDevice::getCapMetricStreamer() const {
    return capMetricStreamer;
}

bool VPUDevice::isConnected() {
    bool connected = VPUDriverApi(devnode, osInfc).checkDeviceStatus();
    LOG_V("Device connection status: %u", connected);
    return connected;
}

size_t VPUDevice::getNumberOfEngineGroups(void) const {
    return engineGroups.size();
}

size_t VPUDevice::getEngineMaxMemoryFillSize() {
    return sizeof(uint32_t);
}

EngineType VPUDevice::getEngineType(uint32_t engGrpIdx) {
    if (engGrpIdx >= getNumberOfEngineGroups()) {
        LOG_E("Engine group with index %u does not exist", engGrpIdx);
        return EngineType::INVALID;
    }

    return engineGroups[engGrpIdx];
}

EngineType VPUDevice::getEngineTypeFromOrdinal(uint32_t engGrpOrdinal, bool &isCopyOnly) {
    EngineType engType = getEngineType(engGrpOrdinal);
    if (engType == EngineType::INVALID)
        return engType;

    isCopyOnly = (engineSupportCopy(engType) && !engineSupportCompute(engType) &&
                  !engineSupportCooperativeKernel(engType) && !engineSupportMetrics(engType));

    return engType;
}

bool VPUDevice::engineSupportCompute(EngineType engineType) const {
    return engineType == EngineType::COMPUTE;
}

std::unique_ptr<VPUDeviceContext> VPUDevice::createDeviceContext() {
    auto drvApi = VPUDriverApi::openDriverApi(devnode, osInfc);
    if (drvApi == nullptr) {
        LOG_E("Failed to allocate VPUDriverApi");
        return nullptr;
    }

    return std::make_unique<VPUDeviceContext>(std::move(drvApi), &hwInfo);
}

} // namespace VPU
