/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"
#include "vpu_driver/source/command/vpu_job.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "vpu_driver/source/device/devicelist.hpp"
#include "vpu_driver/include/firmware/vpu_jsm_api.h"

#include <boost/numeric/conversion/cast.hpp>
#include <cerrno>
#include <limits>
#include <sys/mman.h>
#include <uapi/drm/ivpu_drm.h>
#include <cassert>

namespace VPU {
VPUDevice::VPUDevice(std::string devnode, OsInterface &osInfc)
    : devnode(devnode)
    , osInfc(osInfc) {}

bool VPUDevice::initializeCaps(VPUDriverApi *drvApi) {
    struct drm_ivpu_param deviceParameter = {};

    deviceParameter.param = DRM_IVPU_PARAM_DEVICE_ID;
    if (0 > drvApi->getDeviceParam(&deviceParameter)) {
        LOG_E("Failed to call device config ioctl. -errno: %d", errno);
        return false;
    }

    switch (deviceParameter.value) {
    case MTL_VPU_PCI_DEVICE_ID:
        deviceInfo = MTLDeviceInfo();
        LOG_I("MTL VPU Device (0x%llx) found!", deviceParameter.value);
        break;
    default:
        LOG_E("Unknown PCI DeviceID 0x%llx.", deviceParameter.value);
        return false;
    }
    deviceInfo.deviceId = boost::numeric_cast<uint32_t>(deviceParameter.value);

    deviceParameter.param = DRM_IVPU_PARAM_DEVICE_REVISION;
    if (drvApi->getDeviceParam(&deviceParameter)) {
        LOG_E("Failed to call device config ioctl. -errno: %d", errno);
        return false;
    }
    deviceInfo.deviceRevision = boost::numeric_cast<uint32_t>(deviceParameter.value);

    deviceParameter.param = DRM_IVPU_PARAM_NUM_CONTEXTS;
    if (drvApi->getDeviceParam(&deviceParameter)) {
        LOG_E("Failed to call device config ioctl. -errno: %d", errno);
        return false;
    }
    deviceInfo.maxHardwareContexts = boost::numeric_cast<uint32_t>(deviceParameter.value);

    deviceParameter.param = DRM_IVPU_PARAM_CORE_CLOCK_RATE;
    if (drvApi->getDeviceParam(&deviceParameter)) {
        LOG_E("Failed to call device config ioctl. -errno: %d", errno);
        return false;
    }
    deviceInfo.coreClockRate = boost::numeric_cast<uint32_t>(deviceParameter.value);

    deviceParameter.param = DRM_IVPU_PARAM_PLATFORM_TYPE;
    if (drvApi->getDeviceParam(&deviceParameter)) {
        LOG_E("Failed to call device config ioctl. -errno: %d", errno);
        return false;
    }
    deviceInfo.platformType = boost::numeric_cast<uint32_t>(deviceParameter.value);

    deviceInfo.initialized = true;

    return true;
}

bool VPUDevice::init() {
    LOG_V("Initializing VPU device.");

    if (devnode.empty()) {
        LOG_W("vpuInfo is null.");
        return false;
    }

    auto drvApi = VPUDriverApi::openDriverApi(devnode, osInfc);
    if (drvApi == nullptr || !drvApi->isVpuDevice())
        return false;

    if (!initializeCaps(drvApi.get())) {
        LOG_W("Failed to initialize VPU device capabilities.");
        return false;
    }

    LOG_V("VPU device initialized successfully.");
    return true;
}

const DeviceInfo &VPUDevice::getDeviceInfo() const {
    return deviceInfo;
}

const std::vector<GroupInfo> VPUDevice::getMetricGroupsInfo() const {
    return deviceInfo.groupsInfo;
}

bool VPUDevice::isConnected() {
    bool connected = VPUDriverApi(devnode, osInfc).checkDeviceStatus();
    LOG_V("Device connection status: %u", connected);
    return connected;
}

size_t VPUDevice::getNumberOfEngineGroups(void) const {
    return engineGroups.size();
}

size_t VPUDevice::getEngineMaxMemoryFillSize(EngineType engineType) {
    if (engineType == EngineType::COPY)
        return std::numeric_limits<uint32_t>::max();

    return 0u;
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

    struct drm_ivpu_param arg = {};
    arg.param = DRM_IVPU_PARAM_CONTEXT_ID;
    if (0 > drvApi->getDeviceParam(&arg)) {
        LOG_E("Failed to get device parameter DRM_IVPU_PARAM_CONTEXT_ID");
        return nullptr;
    }

    uint32_t contextId = boost::numeric_cast<uint32_t>(arg.value);
    LOG_I("Context %u has been created", contextId);

    arg = {};
    arg.param = DRM_IVPU_PARAM_CONTEXT_BASE_ADDRESS;
    if (drvApi->getDeviceParam(&arg) != 0) {
        LOG_E("Failed to get device parameter DRM_IVPU_PARAM_CONTEXT_BASE_ADDRESS");
        return nullptr;
    }

    uint64_t baseAddress = arg.value;
    LOG_I("Base address of device is %lu", baseAddress);

    return std::make_unique<VPUDeviceContext>(std::move(drvApi),
                                              contextId,
                                              deviceInfo.deviceId,
                                              baseAddress);
}

} // namespace VPU
