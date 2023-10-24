/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

class Device : public UmdTest {};

TEST_F(Device, GetProperties) {
    ze_device_properties_t devProp = {};

    devProp.stype = ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES;
    EXPECT_EQ(zeDeviceGetProperties(zeDevice, &devProp), ZE_RESULT_SUCCESS);
    EXPECT_EQ(devProp.type, ZE_DEVICE_TYPE_VPU);
    EXPECT_EQ(devProp.vendorId, 0x8086u);
    EXPECT_TRUE(test_app::is_vpu(devProp.deviceId)) << "Invalid PCI Device ID" << devProp.deviceId;
    EXPECT_STREQ(devProp.name, "Intel(R) AI Boost");
}
