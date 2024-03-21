/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

class Driver : public UmdTest {};

/* TODO: Test disabled: EISW-107116 */
TEST(Init, DISABLED_HandleWrongInitFlagError) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ENUMERATION, zeInit(ZE_INIT_FLAG_GPU_ONLY));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ENUMERATION, zeInit(0x04));
}

TEST(Init, WithValidInitFlag) {
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(ZE_INIT_FLAG_VPU_ONLY));
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(0));
}

TEST(Init, WithValidInitFlagMultiThreaded) {
    std::vector<std::shared_ptr<std::thread>> tasks;
    uint32_t numThreads = std::thread::hardware_concurrency();
    for (uint32_t i = 0; i < numThreads; i++) {
        tasks.push_back(std::make_shared<std::thread>([]() {
            EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(ZE_INIT_FLAG_VPU_ONLY));
            EXPECT_EQ(ZE_RESULT_SUCCESS, zeInit(0));
        }));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
}

TEST_F(Driver, GetApiVersion) {
    ze_api_version_t version;
    EXPECT_EQ(zeDriverGetApiVersion(zeDriver, &version), ZE_RESULT_SUCCESS);
    EXPECT_EQ(version, ZE_API_VERSION_CURRENT);
}

TEST_F(Driver, GetProperties) {
    ze_driver_properties_t prop = {};
    prop.stype = ZE_STRUCTURE_TYPE_DRIVER_PROPERTIES;

    EXPECT_EQ(zeDriverGetProperties(zeDriver, &prop), ZE_RESULT_SUCCESS);
    EXPECT_GE(prop.driverVersion, 0u);
}

TEST_F(Driver, GetIpcProperties) {
    ze_driver_ipc_properties_t prop = {};
    prop.stype = ZE_STRUCTURE_TYPE_DRIVER_IPC_PROPERTIES;

    EXPECT_EQ(zeDriverGetIpcProperties(zeDriver, &prop), ZE_RESULT_SUCCESS);
}

TEST_F(Driver, GetExtensionProperties) {
    uint32_t count = 0;
    EXPECT_EQ(zeDriverGetExtensionProperties(zeDriver, &count, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_GT(count, 0);

    std::vector<ze_driver_extension_properties_t> props(count);
    EXPECT_EQ(zeDriverGetExtensionProperties(zeDriver, &count, props.data()), ZE_RESULT_SUCCESS);

    bool extension_match = false;
    for (auto &v : props) {
        if (std::string(v.name) == GRAPH_EXT_NAME && v.version == GRAPH_EXT_VERSION)
            extension_match = true;
    }
    EXPECT_TRUE(extension_match);
}
