/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <future>
#include <level_zero/ze_api.h>

class Device : public UmdTest {};

TEST_F(Device, GetProperties) {
    ze_device_properties_t devProp = {};

    devProp.stype = ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES;
    EXPECT_EQ(zeDeviceGetProperties(zeDevice, &devProp), ZE_RESULT_SUCCESS);
    EXPECT_EQ(devProp.type, ZE_DEVICE_TYPE_VPU);
    EXPECT_EQ(devProp.vendorId, 0x8086u);
    EXPECT_TRUE(test_app::is_vpu(devProp.deviceId)) << "Invalid PCI Device ID" << devProp.deviceId;
    EXPECT_STREQ(devProp.name, "Intel(R) AI Boost");

    TRACE("PCI Device ID: %#x\n", devProp.deviceId);
    TRACE("Tile count: %u\n", devProp.numSlices);

    ze_pci_ext_properties_t devPci = {};
    devPci.stype = ZE_STRUCTURE_TYPE_PCI_EXT_PROPERTIES;
    EXPECT_EQ(zeDevicePciGetPropertiesExt(zeDevice, &devPci), ZE_RESULT_SUCCESS);
}

TEST_F(Device, GetZesDeviceAndProperties) {
    ASSERT_EQ(zesInit(0), ZE_RESULT_SUCCESS);

    uint32_t driverCount = 0u;
    ASSERT_EQ(zesDriverGet(&driverCount, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_GE(driverCount, 1);

    std::vector<zes_driver_handle_t> zesDrivers(driverCount);
    ASSERT_EQ(zesDriverGet(&driverCount, zesDrivers.data()), ZE_RESULT_SUCCESS);

    bool foundNpu = false;
    for (size_t i = 0; i < zesDrivers.size(); i++) {
        TRACE("Driver[%lu].Handle: %p\n", i, zesDrivers[i]);

        uint32_t deviceCount = 0u;
        ASSERT_EQ(zesDeviceGet(zesDrivers[i], &deviceCount, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_GE(deviceCount, 1);

        std::vector<zes_device_handle_t> zesDevices(deviceCount);
        ASSERT_EQ(zesDeviceGet(zesDrivers[i], &deviceCount, zesDevices.data()), ZE_RESULT_SUCCESS);

        for (size_t j = 0; j < zesDevices.size(); j++) {
            TRACE("Device[%lu].Handle: %p\n", j, zesDevices[j]);

            zes_device_properties_t devProp = {};
            devProp.stype = ZES_STRUCTURE_TYPE_DEVICE_PROPERTIES;

            ASSERT_EQ(zesDeviceGetProperties(zesDevices[j], &devProp), ZE_RESULT_SUCCESS);

            TRACE("Device[%lu].BrandName: %s\n", j, devProp.brandName);
            TRACE("Device[%lu].ModelName: %s\n", j, devProp.modelName);
            TRACE("Device[%lu].VendorName: %s\n", j, devProp.vendorName);
            TRACE("Device[%lu].DriverVersion: %s\n", j, devProp.driverVersion);

            if (std::string(devProp.vendorName) == "INTEL" &&
                std::string(devProp.brandName) == "NPU") {
                EXPECT_NE(std::string(devProp.driverVersion).find("npu-linux-driver"),
                          std::string::npos);
                foundNpu = true;
            }
        }
    }
    EXPECT_TRUE(foundNpu) << "Could not find Intel NPU identifier in zesDeviceGetProperties()";
}

TEST_F(Device, GetZesEngineGetActivity) {
    SKIP_NEEDS_SYSFS_FILE("npu_busy_time_us");
    ASSERT_EQ(zesInit(0), ZE_RESULT_SUCCESS);

    uint32_t zesDrvCount = 0u;
    ASSERT_EQ(zesDriverGet(&zesDrvCount, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_GE(zesDrvCount, 1);

    std::vector<zes_driver_handle_t> zesDrivers(zesDrvCount);
    ASSERT_EQ(zesDriverGet(&zesDrvCount, zesDrivers.data()), ZE_RESULT_SUCCESS);

    bool foundNpu = false;
    for (const auto &driver : zesDrivers) {
        uint32_t zesDevCount = 0u;
        ASSERT_EQ(zesDeviceGet(driver, &zesDevCount, nullptr), ZE_RESULT_SUCCESS);
        if (!zesDevCount)
            continue;

        std::vector<zes_device_handle_t> zesDevices(zesDevCount);
        ASSERT_EQ(zesDeviceGet(driver, &zesDevCount, zesDevices.data()), ZE_RESULT_SUCCESS);
        for (auto &device : zesDevices) {
            zes_device_properties_t devProp = {};
            devProp.stype = ZES_STRUCTURE_TYPE_DEVICE_PROPERTIES;

            ASSERT_EQ(zesDeviceGetProperties(device, &devProp), ZE_RESULT_SUCCESS);

            if (std::string(devProp.vendorName) == "INTEL" &&
                std::string(devProp.brandName) == "NPU") {
                foundNpu = true;
                uint32_t count = 0;

                ASSERT_EQ(zesDeviceEnumEngineGroups(device, &count, nullptr), ZE_RESULT_SUCCESS);
                ASSERT_EQ(count, 1);
                std::vector<zes_engine_handle_t> engineHandlers(count);
                ASSERT_EQ(zesDeviceEnumEngineGroups(device, &count, engineHandlers.data()),
                          ZE_RESULT_SUCCESS);

                zes_engine_properties_t engineProperties = {};
                engineProperties.stype = ZES_STRUCTURE_TYPE_ENGINE_PROPERTIES;
                ASSERT_EQ(zesEngineGetProperties(engineHandlers[0], &engineProperties),
                          ZE_RESULT_SUCCESS);

                EXPECT_EQ(engineProperties.type, ZES_ENGINE_GROUP_COMPUTE_ALL);
                EXPECT_FALSE(engineProperties.onSubdevice);

                zes_engine_stats_t engineStats;
                ASSERT_EQ(zesEngineGetActivity(engineHandlers[0], &engineStats), ZE_RESULT_SUCCESS);
                EXPECT_GT(engineStats.timestamp, 0ULL);
                TRACE("Device active time: %lu\n", engineStats.activeTime);
                TRACE("Device timestamp: %lu\n", engineStats.timestamp);
            }
        }
    }
    EXPECT_TRUE(foundNpu) << "Could not find Intel NPU identifier in zesDeviceGetProperties()";
}

TEST_F(Device, GetPropertiesMutableCmdListDeviceIpVersion) {
    ze_device_ip_version_ext_t deviceIpVersion{
        .stype = ZE_STRUCTURE_TYPE_DEVICE_IP_VERSION_EXT,
        .pNext = nullptr,
        .ipVersion = std::numeric_limits<uint32_t>::max(),
    };
    ze_mutable_command_list_exp_properties_t mutableCmdListProps{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_LIST_EXP_PROPERTIES,
        .pNext = &deviceIpVersion,
        .mutableCommandListFlags = 0,
        .mutableCommandFlags = 0,
    };
    ze_device_properties_t devProp = {};
    devProp.stype = ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES;
    devProp.pNext = &mutableCmdListProps;

    EXPECT_EQ(zeDeviceGetProperties(zeDevice, &devProp), ZE_RESULT_SUCCESS);
    EXPECT_EQ(devProp.type, ZE_DEVICE_TYPE_VPU);
    EXPECT_EQ(devProp.vendorId, 0x8086u);
    EXPECT_TRUE(test_app::is_vpu(devProp.deviceId)) << "Invalid PCI Device ID" << devProp.deviceId;
    EXPECT_STREQ(devProp.name, "Intel(R) AI Boost");

    EXPECT_NE(deviceIpVersion.ipVersion, std::numeric_limits<uint32_t>::max());
    EXPECT_EQ(mutableCmdListProps.mutableCommandListFlags, 0);
    EXPECT_EQ(mutableCmdListProps.mutableCommandFlags,
              ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENT_DEPRECATED |
                  ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS);

    TRACE("PCI Device ID: %#x\n", devProp.deviceId);
    TRACE("Tile count: %u\n", devProp.numSlices);
}

TEST_F(Device, GetGlobalTimestamps) {
    auto checkTimestamp = [&]() {
        using namespace std::chrono_literals;
        uint64_t hostTimestamp1 = 0, deviceTimestamp1 = 0;
        uint64_t hostTimestamp2 = 0, deviceTimestamp2 = 0;

        EXPECT_EQ(zeDeviceGetGlobalTimestamps(zeDevice, &hostTimestamp1, &deviceTimestamp1),
                  ZE_RESULT_SUCCESS);
        std::this_thread::sleep_for(1s);
        EXPECT_EQ(zeDeviceGetGlobalTimestamps(zeDevice, &hostTimestamp2, &deviceTimestamp2),
                  ZE_RESULT_SUCCESS);
        EXPECT_GE(hostTimestamp2 - hostTimestamp1, 1'000'000ULL);
        EXPECT_GT(deviceTimestamp2, deviceTimestamp1);
    };

    const size_t threadsNum = 10;
    std::vector<std::future<void>> tasks(threadsNum);
    for (auto &task : tasks) {
        task = std::async(std::launch::async, checkTimestamp);
    }
    for (const auto &task : tasks) {
        task.wait();
    }
}

TEST_F(Device, VerifySetWorkloadTypeApi) {
    ze_result_t ret;
    ze_command_queue_desc_t cmdQueueDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                            .pNext = nullptr,
                                            .ordinal = 0,
                                            .index = 0,
                                            .flags = 0,
                                            .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                            .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};

    auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    ret = zeCommandQueueDDITableExt->pfnSetWorkloadType(scopedQueue.get(),
                                                        ZE_WORKLOAD_TYPE_BACKGROUND);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    ret =
        zeCommandQueueDDITableExt->pfnSetWorkloadType(scopedQueue.get(), ZE_WORKLOAD_TYPE_DEFAULT);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    ret = zeCommandQueueDDITableExt->pfnSetWorkloadType(nullptr, ZE_WORKLOAD_TYPE_BACKGROUND);
    ASSERT_EQ(ret, ZE_RESULT_ERROR_INVALID_NULL_HANDLE);

    ret = zeCommandQueueDDITableExt->pfnSetWorkloadType(scopedQueue.get(),
                                                        ZE_WORKLOAD_TYPE_FORCE_UINT32);
    ASSERT_EQ(ret, ZE_RESULT_ERROR_INVALID_ENUMERATION);
}
