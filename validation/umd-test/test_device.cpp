/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <future>

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
    uint32_t zesDrvCount = 0u;
    uint32_t zesDevCount = 0u;

    ASSERT_EQ(zesDriverGet(&zesDrvCount, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zesDrvCount, 1);
    std::vector<zes_driver_handle_t> zesDrivers(zesDrvCount);
    ASSERT_EQ(zeDriverGet(&zesDrvCount, zesDrivers.data()), ZE_RESULT_SUCCESS);

    for (const auto &driver : zesDrivers) {
        ASSERT_EQ(zesDeviceGet(driver, &zesDevCount, nullptr), ZE_RESULT_SUCCESS);
        EXPECT_EQ(zesDevCount, 1);
        if (zesDevCount != 1)
            continue;

        zes_device_handle_t device;
        ASSERT_EQ(zesDeviceGet(driver, &zesDevCount, &device), ZE_RESULT_SUCCESS);
        zes_device_properties_t devProp = {};
        devProp.stype = ZES_STRUCTURE_TYPE_DEVICE_PROPERTIES;

        ASSERT_EQ(zesDeviceGetProperties(device, &devProp), ZE_RESULT_SUCCESS);

        EXPECT_STREQ(devProp.vendorName, "INTEL");
        EXPECT_STREQ(devProp.brandName, "NPU");
        std::string version(devProp.driverVersion);
        EXPECT_NE(version.find("npu-linux-driver"), std::string::npos);
        TRACE("BrandName: %s\n", devProp.brandName);
        TRACE("ModelName: %s\n", devProp.modelName);
        TRACE("VendorName: %s\n", devProp.vendorName);
        TRACE("DriverVersion: %s\n", devProp.driverVersion);
    }
}

TEST_F(Device, GetZesProperties) {
    zes_device_properties_t devProp = {};

    devProp.stype = ZES_STRUCTURE_TYPE_DEVICE_PROPERTIES;
    ASSERT_EQ(zesDeviceGetProperties(static_cast<zes_device_handle_t>(zeDevice), &devProp),
              ZE_RESULT_SUCCESS);

    EXPECT_STREQ(devProp.vendorName, "INTEL");
    EXPECT_STREQ(devProp.brandName, "NPU");
    std::string version(devProp.driverVersion);
    EXPECT_NE(version.find("npu-linux-driver"), std::string::npos);
    TRACE("BrandName: %s\n", devProp.brandName);
    TRACE("ModelName: %s\n", devProp.modelName);
    TRACE("VendorName: %s\n", devProp.vendorName);
    TRACE("DriverVersion: %s\n", devProp.driverVersion);
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
