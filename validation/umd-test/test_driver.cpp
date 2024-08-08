/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

class Driver : public UmdTest {};

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

    bool has_graph_extension = false;
    bool has_mutable_cmdlist_extension = false;
    for (auto &v : props) {
        if (std::string(v.name) == GRAPH_EXT_NAME && v.version == GRAPH_EXT_VERSION)
            has_graph_extension = true;
        if (std::string(v.name) == ZE_MUTABLE_COMMAND_LIST_EXP_NAME &&
            v.version == ZE_MUTABLE_COMMAND_LIST_EXP_VERSION_1_0)
            has_mutable_cmdlist_extension = true;
    }

    EXPECT_TRUE(has_graph_extension);
    EXPECT_TRUE(has_mutable_cmdlist_extension);
}
