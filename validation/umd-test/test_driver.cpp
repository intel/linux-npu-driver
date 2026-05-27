/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <time.h>
#include <ze_driver_npu_ext.h>

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
    bool has_context_npu_extension = false;
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
        if (extName == COMMAND_QUEUE_EXT_NAME &&
            (v.version == ZE_COMMAND_QUEUE_NPU_EXT_VERSION_1_0 ||
             v.version == ZE_COMMAND_QUEUE_NPU_EXT_VERSION_1_1))
            has_cmd_queue_extension = true;
        if (extName == ZE_CONTEXT_NPU_EXT_NAME && v.version == ZE_CONTEXT_NPU_EXT_VERSION_1_0)
            has_context_npu_extension = true;
    }

    EXPECT_TRUE(has_graph_extension);
    EXPECT_TRUE(has_mutable_cmdlist_extension);
    EXPECT_TRUE(has_cmd_queue_extension);
    EXPECT_TRUE(has_context_npu_extension);
}

TEST_F(Driver, TestDriverNpuExtension) {
    ze_driver_npu_dditable_ext_t *driverExtDdi = nullptr;
    ASSERT_EQ(zeDriverGetExtensionFunctionAddress(zeDriver,
                                                  ZE_DRIVER_NPU_EXT_NAME,
                                                  reinterpret_cast<void **>(&driverExtDdi)),
              ZE_RESULT_SUCCESS);
    ASSERT_NE(driverExtDdi, nullptr);
    /*Extensions with DDI tables - expected pass*/
    std::vector<std::pair<std::string, uint32_t>> extensionsWithDdi = {
        {ZE_COMMAND_QUEUE_NPU_EXT_NAME, ZE_COMMAND_QUEUE_NPU_EXT_VERSION_CURRENT},
        {ZE_COMMAND_QUEUE_NPU_EXT_NAME, ZE_COMMAND_QUEUE_NPU_EXT_VERSION_1_0},
        {ZE_CONTEXT_NPU_EXT_NAME, ZE_CONTEXT_NPU_EXT_VERSION_CURRENT},
        {ZE_GRAPH_EXT_NAME, ZE_GRAPH_EXT_VERSION_CURRENT},
        {ZE_GRAPH_EXT_NAME, ZE_GRAPH_EXT_VERSION_1_0},
        {ZE_PROFILING_DATA_EXT_NAME, ZE_PROFILING_DATA_EXT_VERSION_CURRENT},
        {ZE_PROFILING_DATA_EXT_NAME, ZE_PROFILING_DATA_EXT_VERSION_1_0},
        {ZE_DRIVER_NPU_EXT_NAME, ZE_DRIVER_NPU_EXT_VERSION_CURRENT}};

    /* Positive cases with DDI tables */
    for (const auto &extName : extensionsWithDdi) {
        void *funcPtr = nullptr;
        ze_driver_extension_npu_ext_t ext = {};
        ext.stype = ZE_STRUCTURE_TYPE_DRIVER_EXTENSION_NPU_EXT;
        ext.name = extName.first.c_str();
        ext.version = extName.second;
        ext.ppFunctionAddress = &funcPtr;
        ASSERT_EQ(driverExtDdi->pfnGetExtension(zeDriver, &ext), ZE_RESULT_SUCCESS);
        ASSERT_NE(funcPtr, nullptr);
    }

    /* Negative case: unsupported extension */
    {
        void *funcPtr = nullptr;
        ze_driver_extension_npu_ext_t ext = {};
        ext.stype = ZE_STRUCTURE_TYPE_DRIVER_EXTENSION_NPU_EXT;
        ext.name = "UnsupportedExtension";
        ext.version = 1;
        ext.ppFunctionAddress = &funcPtr;
        ASSERT_EQ(driverExtDdi->pfnGetExtension(zeDriver, &ext), ZE_RESULT_ERROR_INVALID_ARGUMENT);
        ASSERT_EQ(funcPtr, nullptr);
    }

    /* Negative case: unsupported version */
    for (const auto &extName : extensionsWithDdi) {
        void *funcPtr = nullptr;
        ze_driver_extension_npu_ext_t ext = {};
        ext.stype = ZE_STRUCTURE_TYPE_DRIVER_EXTENSION_NPU_EXT;
        ext.name = extName.first.c_str();
        ext.version = static_cast<uint32_t>(-1); // Unsupported version
        ext.ppFunctionAddress = &funcPtr;
        ASSERT_EQ(driverExtDdi->pfnGetExtension(zeDriver, &ext), ZE_RESULT_ERROR_INVALID_ARGUMENT);
        ASSERT_EQ(funcPtr, nullptr);
    }
}