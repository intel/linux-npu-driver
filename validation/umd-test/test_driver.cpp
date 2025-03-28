/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <time.h>

class Driver : public UmdTest {};

TEST_F(Driver, GetApiVersion) {
    ze_api_version_t version;
    EXPECT_EQ(zeDriverGetApiVersion(zeDriver, &version), ZE_RESULT_SUCCESS);
    EXPECT_EQ(version, ZE_API_VERSION_CURRENT);
    TRACE("Driver API Version: %i.%i\n", ZE_MAJOR_VERSION(version), ZE_MINOR_VERSION(version));
}

TEST_F(Driver, GetProperties) {
    ze_driver_properties_t prop = {};
    prop.stype = ZE_STRUCTURE_TYPE_DRIVER_PROPERTIES;

    EXPECT_EQ(zeDriverGetProperties(zeDriver, &prop), ZE_RESULT_SUCCESS);
    EXPECT_GE(prop.driverVersion, 0u);
    TRACE("Driver Version: %i\n", prop.driverVersion);
    long driverDate = prop.driverVersion;
    TRACE("Driver Version Date: %s\n", ::asctime(::gmtime(&driverDate)));
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
    bool has_cmd_queue_extension = false;
    for (auto &v : props) {
        TRACE("Driver Extension name: %s, version: %i.%i\n",
              v.name,
              ZE_MAJOR_VERSION(v.version),
              ZE_MINOR_VERSION(v.version));
        std::string extName(v.name);
        if (extName.find(ZE_GRAPH_EXT_NAME) != std::string::npos && v.version == GRAPH_EXT_VERSION)
            has_graph_extension = true;
        if (extName == ZE_MUTABLE_COMMAND_LIST_EXP_NAME &&
            v.version == ZE_MUTABLE_COMMAND_LIST_EXP_VERSION_1_1)
            has_mutable_cmdlist_extension = true;
        if (extName == COMMAND_QUEUE_EXT_NAME && v.version == COMMAND_QUEUE_NPU_EXT_VERSION)
            has_cmd_queue_extension = true;
    }

    EXPECT_TRUE(has_graph_extension);
    EXPECT_TRUE(has_mutable_cmdlist_extension);
    EXPECT_TRUE(has_cmd_queue_extension);
}
