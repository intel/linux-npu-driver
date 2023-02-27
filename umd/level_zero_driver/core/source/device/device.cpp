/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"

#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/cmdlist/cmdlist.hpp"
#include "level_zero_driver/core/source/cmdqueue/cmdqueue.hpp"
#include "level_zero_driver/ext/source/graph/graph.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/tools/source/metrics/metric.hpp"

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"

#include <algorithm>
#include <string.h>

namespace L0 {

Device::Device(DriverHandle *driverHandle, VPU::VPUDevice *vpuDevice)
    : driverHandle(driverHandle)
    , vpuDevice(vpuDevice)
    , metricContext(std::make_shared<MetricContext>(this)) {
    if (vpuDevice != nullptr) {
        loadDeviceProperties();
        Driver *pDriver = Driver::getInstance();
        if (pDriver && pDriver->getEnvVariables().metrics) {
            std::vector<VPU::GroupInfo> metricGroupsInfo = vpuDevice->getMetricGroupsInfo();
            loadMetricGroupsInfo(metricGroupsInfo);
        }
    }
}

DriverHandle *Device::getDriverHandle() {
    return driverHandle;
}

bool Device::isCopyOnlyEngineGroup(uint32_t ordinal, bool &outputValid) {
    bool isCopyOnly = false;
    if (vpuDevice == nullptr ||
        vpuDevice->getEngineTypeFromOrdinal(ordinal, isCopyOnly) == VPU::EngineType::INVALID) {
        outputValid = false;
        LOG_E("Failed to get engine type");
        return false;
    }

    outputValid = true;
    return isCopyOnly;
}

ze_result_t Device::createCommandList(const ze_command_list_desc_t *desc,
                                      ze_command_list_handle_t *commandList,
                                      VPU::VPUDeviceContext *ctx) {
    bool isCopyOnly = false;
    bool isValid = false;
    ze_result_t returnValue = ZE_RESULT_SUCCESS;

    if ((desc == nullptr) || (commandList == nullptr)) {
        LOG_E("Command list descriptor/pointer commandList passed as nullptr.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    isCopyOnly = isCopyOnlyEngineGroup(desc->commandQueueGroupOrdinal, isValid);
    if (!isValid) {
        LOG_E("Wrong ordinal value received: CommandList could not be created");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *commandList = CommandList::create(isCopyOnly, ctx, returnValue);
    return returnValue;
}

ze_result_t Device::createCommandQueue(const ze_command_queue_desc_t *desc,
                                       ze_command_queue_handle_t *commandQueue,
                                       VPU::VPUDeviceContext *ctx) {
    if ((nullptr == desc) || (nullptr == commandQueue)) {
        LOG_E("Command Queue descriptor/pointer commandQueue passed as nullptr.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (nullptr == vpuDevice) {
        LOG_E("VPU Device lost / failed to be retrieved.");
        return ZE_RESULT_ERROR_DEVICE_LOST;
    }

    // Check that ordinal value is not >=  max number of engines
    if (desc->ordinal >= safe_cast<uint32_t>(VPU::EngineType::ENGINE_MAX)) {
        LOG_E("Command Queue Descriptor ordinal value %u is invalid which should be less than %u.",
              desc->ordinal,
              safe_cast<uint32_t>(VPU::EngineType::ENGINE_MAX));
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    *commandQueue = CommandQueue::create(this, desc, ctx);

    if (*commandQueue == nullptr)
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;

    return ZE_RESULT_SUCCESS;
}

ze_result_t Device::getP2PProperties(ze_device_handle_t hPeerDevice,
                                     ze_device_p2p_properties_t *pP2PProperties) {
    if (nullptr == hPeerDevice) {
        LOG_E("Invalid PeerDevice handle.");
        return ZE_RESULT_ERROR_INVALID_NULL_HANDLE;
    }

    if (nullptr == pP2PProperties) {
        LOG_E("Invalid pP2PProperties pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    // Setting property flag
    pP2PProperties->flags = 0;
    return ZE_RESULT_SUCCESS;
}

ze_result_t Device::getProperties(ze_device_properties_t *pDeviceProperties) {
    if (pDeviceProperties == nullptr) {
        LOG_E("Invalid pDeviceProperties pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (pDeviceProperties->stype == ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES_1_2) {
        properties.stype = pDeviceProperties->stype;

        // the units are in cycles/sec
        properties.timerResolution = 38'400'000;
    } else if (pDeviceProperties->stype == ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES) {
        properties.stype = pDeviceProperties->stype;

        // the units are in nanoseconds
        properties.timerResolution = NS_IN_SEC / 38'400'000;
    }

    *pDeviceProperties = properties;

    LOG_I("Returning device properties.");
    return ZE_RESULT_SUCCESS;
}

ze_result_t Device::getSubDevices(uint32_t *pCount, ze_device_handle_t *phSubdevices) {
    if (nullptr == pCount) {
        LOG_E("Invalid pCount pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }
    if (*pCount == 0) {
        *pCount = numSubDevices;
        return ZE_RESULT_SUCCESS;
    }

    if (phSubdevices == nullptr) {
        LOG_E("Invalid phSubdevices pointer.");
        return ZE_RESULT_ERROR_INVALID_ARGUMENT;
    }

    if (*pCount > numSubDevices) {
        *pCount = numSubDevices;
    }

    for (uint32_t i = 0; i < *pCount; i++) {
        phSubdevices[i] = subDevices[i];
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t Device::setIntermediateCacheConfig(ze_cache_config_flags_t cacheConfig) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

ze_result_t Device::setLastLevelCacheConfig(ze_cache_config_flags_t cacheConfig) {
    return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
}

const char *Device::getDeviceMemoryName() const {
    return "DDR";
}

ze_result_t Device::getMemoryProperties(uint32_t *pCount,
                                        ze_device_memory_properties_t *pMemProperties) {
    if (nullptr == pCount) {
        LOG_E("Invalid memory properties count pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (*pCount == 0) {
        *pCount = 1;
        return ZE_RESULT_SUCCESS;
    }

    *pCount = 1;

    if (nullptr == vpuDevice) {
        LOG_E("VPU device instance is invalid.");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (nullptr == pMemProperties) {
        LOG_I("Input memory properties pointer is NULL.");
    } else {
        const auto &hwInfo = vpuDevice->getHwInfo();

        pMemProperties->flags = 0u;
        pMemProperties->maxClockRate = hwInfo.coreClockRate;
        pMemProperties->maxBusWidth = 32u;
        pMemProperties->totalSize = hwInfo.maxMemAllocSize;
        strncpy(pMemProperties->name, getDeviceMemoryName(), ZE_MAX_DEVICE_NAME - 1);
        pMemProperties->name[ZE_MAX_DEVICE_NAME - 1] = '\0';
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t
Device::getMemoryAccessProperties(ze_device_memory_access_properties_t *pMemAccessProperties) {
    if (pMemAccessProperties == nullptr) {
        LOG_E("Invalid pMemAccessProperties pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    pMemAccessProperties->hostAllocCapabilities =
        ZE_MEMORY_ACCESS_CAP_FLAG_RW | ZE_MEMORY_ACCESS_CAP_FLAG_ATOMIC;
    pMemAccessProperties->deviceAllocCapabilities =
        ZE_MEMORY_ACCESS_CAP_FLAG_RW | ZE_MEMORY_ACCESS_CAP_FLAG_ATOMIC;
    pMemAccessProperties->sharedSingleDeviceAllocCapabilities = 0;
    pMemAccessProperties->sharedCrossDeviceAllocCapabilities = 0;
    pMemAccessProperties->sharedSystemAllocCapabilities = 0;
    return ZE_RESULT_SUCCESS;
}

ze_result_t Device::getDeviceImageProperties(ze_device_image_properties_t *pDeviceImageProperties) {
    if (pDeviceImageProperties == nullptr) {
        LOG_E("Invalid pDeviceImageProperties pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    // Maximum image dimensions for 1D resources
    pDeviceImageProperties->maxImageDims1D = 16384u;

    // Maximum image dimensions for 2D resources
    pDeviceImageProperties->maxImageDims2D = 16384u;

    // Maximum image dimensions for 3D resources
    pDeviceImageProperties->maxImageDims3D = 2048u;

    // Maximum image dimensions for 1D resources
    pDeviceImageProperties->maxImageBufferSize = 262144u;

    // Maximum image array slices
    pDeviceImageProperties->maxImageArraySlices = 2048u;

    // Maximum image array slices
    pDeviceImageProperties->maxSamplers = 16u;

    // Maximum number of image objects that can be read from by a kernel
    pDeviceImageProperties->maxReadImageArgs = 128u;

    // Maximum number of image objects that can be written to by a kernel
    pDeviceImageProperties->maxWriteImageArgs = 128u;

    return ZE_RESULT_SUCCESS;
}

ze_result_t
Device::getDeviceComputeProperties(ze_device_compute_properties_t *pDeviceComputeProperties) {
    if (pDeviceComputeProperties == nullptr) {
        LOG_E("Invalid pDeviceComputeProperties pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    // Maximum items per compute group
    pDeviceComputeProperties->maxTotalGroupSize = 0u;

    // Maximum items for X dimension in group
    pDeviceComputeProperties->maxGroupSizeX = 0u;

    // Maximum items for Y dimension in group
    pDeviceComputeProperties->maxGroupSizeY = 0u;

    // Maximum items for Z dimension in group
    pDeviceComputeProperties->maxGroupSizeZ = 0u;

    // Maximum groups that can be launched for x dimension
    pDeviceComputeProperties->maxGroupCountX = 0u;

    // Maximum groups that can be launched for y dimension
    pDeviceComputeProperties->maxGroupCountY = 0u;

    // Maximum groups that can be launched for z dimension
    pDeviceComputeProperties->maxGroupCountZ = 0u;

    // Maximum shared local memory per group
    pDeviceComputeProperties->maxSharedLocalMemory = 0u;

    // Number of subgroup sizes supported
    pDeviceComputeProperties->numSubGroupSizes = 0u;

    memset(pDeviceComputeProperties->subGroupSizes, 0, sizeof(uint32_t) * ZE_SUBGROUPSIZE_COUNT);

    return ZE_RESULT_SUCCESS;
}

ze_result_t Device::getCommandQueueGroupProperties(
    uint32_t *pCount,
    ze_command_queue_group_properties_t *pCommandQueueGroupProperties) {
    if (pCount == nullptr) {
        LOG_E("Invalid queue group properties count pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto vpuDevice = getVPUDevice();
    if (vpuDevice == nullptr) {
        LOG_E("Failed to get VPUDevice instance.");
        return ZE_RESULT_ERROR_DEVICE_LOST;
    }

    uint32_t count = boost::numeric_cast<uint32_t>(vpuDevice->getNumberOfEngineGroups());
    // Set engine group counts.
    if (*pCount == 0) {
        *pCount = count;
        return ZE_RESULT_SUCCESS;
    }

    if (*pCount > count)
        *pCount = count;

    if (pCommandQueueGroupProperties != nullptr) {
        // Set queue group properties.
        for (uint32_t i = 0; i < *pCount; i++) {
            auto egProp = &(pCommandQueueGroupProperties[i]);
            auto eg = vpuDevice->getEngineType(i);
            if (eg == VPU::EngineType::INVALID) {
                LOG_W("Invalid engine group index (%u / %u)", i, *pCount);
                continue;
            }

            // Set flags.
            egProp->flags = 0u;
            if (vpuDevice->engineSupportCompute(eg)) {
                egProp->flags |= ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE;
            }
            if (vpuDevice->engineSupportCopy(eg)) {
                egProp->flags |= ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COPY;
            }
            if (vpuDevice->engineSupportCooperativeKernel(eg)) {
                egProp->flags |= ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COOPERATIVE_KERNELS;
            }
            if (vpuDevice->engineSupportMetrics(eg)) {
                egProp->flags |= ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_METRICS;
            }

            // Number of engines in the group.
            egProp->numQueues = 1u;

            // Maximum memory fill patern size.
            egProp->maxMemoryFillPatternSize = vpuDevice->getEngineMaxMemoryFillSize(eg);
        }
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t Device::getStatus() const {
    if (vpuDevice == nullptr) {
        LOG_W("VPU device instance is invalid.");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    return vpuDevice->isConnected() ? ZE_RESULT_SUCCESS : ZE_RESULT_ERROR_DEVICE_LOST;
}

// Create L0 device from VPUDevice.
Device *Device::create(DriverHandle *driverHandle, VPU::VPUDevice *vpuDevice) {
    auto device = new Device(driverHandle, vpuDevice);
    if (device == nullptr) {
        LOG_E("New Device creation failed!");
        return nullptr;
    }

    return device;
}

Device::~Device() {
    if (vpuDevice != nullptr) {
        delete vpuDevice;
    }

    for (uint32_t i = 0; i < numSubDevices; i++) {
        delete subDevices[i];
    }
}

VPU::VPUDevice *Device::getVPUDevice() {
    return vpuDevice;
}

void Device::loadDeviceProperties() {
    const auto &hwInfo = vpuDevice->getHwInfo();

    // Device type.
    properties.type = ZE_DEVICE_TYPE_VPU;

    // IDs.
    properties.vendorId = INTEL_PCI_VENDOR_ID;
    properties.deviceId = hwInfo.deviceId;

    // Sub device ID.
    properties.subdeviceId = hwInfo.subdeviceId;

    // Core clock rate.
    properties.coreClockRate = hwInfo.coreClockRate;

    // Max mem alloc size.
    properties.maxMemAllocSize = hwInfo.maxMemAllocSize;

    // Max hardware contexts.
    properties.maxHardwareContexts = hwInfo.maxHardwareContexts;

    // Max command queue priority.
    properties.maxCommandQueuePriority = hwInfo.maxCommandQueuePriority;

    // Number of threads per EU.
    properties.numThreadsPerEU = hwInfo.numThreadsPerEU;

    // Physical EU SIMD width.
    properties.physicalEUSimdWidth = hwInfo.physicalEUSimdWidth;

    // Number of EUs per sub-slice
    properties.numEUsPerSubslice = hwInfo.nExecUnits;

    // Number of sub-slices per slice.
    properties.numSubslicesPerSlice = hwInfo.numSubslicesPerSlice;

    // Number of slices.
    properties.numSlices = 1u;

    // Resolution of device timer in nanoseconsds used for profiling, timestamps, etc.
    properties.timerResolution = 0u;

    // Number of valid bits in the timestamp values.
    properties.timestampValidBits = 64u;

    // Number of valid bits in the kernel timestamp values.
    properties.kernelTimestampValidBits = 0u;

    // Device name.
    strncpy(properties.name, hwInfo.name, ZE_MAX_DEVICE_NAME - 1);
    properties.name[ZE_MAX_DEVICE_NAME - 1] = '\0';

    // Property flags.
    properties.flags = 0u;
    if (hwInfo.isIntegrated) {
        properties.flags |= ZE_DEVICE_PROPERTY_FLAG_INTEGRATED;
    }

    if (hwInfo.isSubdevice) {
        properties.flags |= ZE_DEVICE_PROPERTY_FLAG_SUBDEVICE;
    }

    if (hwInfo.isSupportEcc) {
        properties.flags |= ZE_DEVICE_PROPERTY_FLAG_ECC;
    }

    if (hwInfo.isSupportOnDemandPaging) {
        properties.flags |= ZE_DEVICE_PROPERTY_FLAG_ONDEMANDPAGING;
    }

    // UUID.
    reinterpret_cast<uint32_t *>(properties.uuid.id)[0] = INTEL_PCI_VENDOR_ID;
    reinterpret_cast<uint32_t *>(properties.uuid.id)[1] = hwInfo.deviceId;
    reinterpret_cast<uint32_t *>(properties.uuid.id)[2] = 0;
    reinterpret_cast<uint32_t *>(properties.uuid.id)[3] = hwInfo.platformType;
}

void Device::loadMetricGroupsInfo(std::vector<VPU::GroupInfo> &metricGroupsInfo) {
    size_t numberOfMetricGroups = metricGroupsInfo.size();
    LOG_I("Number of metric groups: %lu", numberOfMetricGroups);

    metricGroups.reserve(metricGroupsInfo.size());

    for (auto const &metricGroupInfo : metricGroupsInfo) {
        zet_metric_group_properties_t groupProperties = {};

        groupProperties.stype = ZET_STRUCTURE_TYPE_METRIC_GROUP_PROPERTIES;
        strncpy(groupProperties.name,
                metricGroupInfo.metricGroupName.c_str(),
                ZET_MAX_METRIC_GROUP_NAME - 1);
        groupProperties.name[ZET_MAX_METRIC_GROUP_NAME - 1] = '\0';
        groupProperties.samplingType = ZET_METRIC_GROUP_SAMPLING_TYPE_FLAG_EVENT_BASED |
                                       ZET_METRIC_GROUP_SAMPLING_TYPE_FLAG_TIME_BASED;
        groupProperties.domain = metricGroupInfo.domain;
        groupProperties.metricCount = metricGroupInfo.metricCount;

        std::vector<std::shared_ptr<Metric>> metrics;
        metrics.reserve(groupProperties.metricCount);

        size_t allocationSize = 0u;

        for (auto const &counter : metricGroupInfo.counterInfo) {
            zet_metric_properties_t properties = {};

            properties.stype = ZET_STRUCTURE_TYPE_METRIC_PROPERTIES;
            strncpy(properties.name, counter.metricName.c_str(), ZET_MAX_METRIC_NAME - 1);
            properties.name[ZET_MAX_METRIC_NAME - 1] = '\0';
            properties.metricType = Metric::getMetricType(counter.metricType);
            properties.resultType = Metric::getValueType(counter.valueType);

            allocationSize += Metric::getMetricValueSize(counter.valueType);

            auto pMetric = std::make_shared<Metric>(properties);
            metrics.push_back(pMetric);
        }

        auto pMetricGroup = std::make_shared<MetricGroup>(groupProperties,
                                                          allocationSize,
                                                          metrics,
                                                          metricGroupInfo.groupIndex,
                                                          numberOfMetricGroups);
        metricGroups.push_back(pMetricGroup);
    }

    metricsLoaded = true;
}

bool Device::isMetricGroupAvailable(MetricGroup *metricGroup) const {
    for (auto &group : metricGroups) {
        if (group.get() == metricGroup) {
            LOG_I("MetricGroup is available on device.");
            return true;
        }
    }

    return false;
}

ze_result_t Device::metricGroupGet(uint32_t *pCount, zet_metric_group_handle_t *phMetricGroups) {
    if (getVPUDevice()->getCapMetricStreamer() != 1) {
        LOG_E("Metrics are not supported.");
        return ZE_RESULT_ERROR_UNSUPPORTED_FEATURE;
    }

    if (!metricsLoaded) {
        LOG_E("Metrics data not loaded for device (%p)", this);
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (pCount == nullptr) {
        LOG_E("pCount is NULL.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (*pCount == 0) {
        *pCount = boost::numeric_cast<uint32_t>(metricGroups.size());
        return ZE_RESULT_SUCCESS;
    } else if (*pCount > metricGroups.size()) {
        *pCount = boost::numeric_cast<uint32_t>(metricGroups.size());
    }

    if (phMetricGroups != nullptr) {
        for (uint32_t i = 0; i < *pCount; i++) {
            phMetricGroups[i] = metricGroups[i]->toHandle();
        }
    } else {
        LOG_I("Input metric group handle pointer is NULL.");
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t
Device::activateMetricGroups(int vpuFd, uint32_t count, zet_metric_group_handle_t *phMetricGroups) {
    if (metricContext == nullptr) {
        LOG_E("MetricContext not initialized.");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    return metricContext->activateMetricGroups(vpuFd, count, phMetricGroups);
}

const std::shared_ptr<MetricContext> Device::getMetricContext() const {
    return metricContext;
}

} // namespace L0
