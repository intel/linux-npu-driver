/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/ze_api.h"
#include "level_zero/ze_graph_ext.h"
#include "level_zero/ze_intel_vpu_uuid.h"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/core/source/driver/driver.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "level_zero_driver/unit_tests/mocks/mock_driver.hpp"

#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"

#include "gtest/gtest.h"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <stdlib.h>

namespace L0 {
namespace ult {

TEST(zeInit, whenCallingZeInitThenInitializeOnDriverIsCalled) {
    Mock<Driver> driver;
    driver.reset();

    auto result = zeInit(0);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
    EXPECT_EQ(1u, driver.initCalledCount);
}

TEST(zeInit, returnErrorWhenNotInitialized) {
    Mock<Driver> driver;
    driver.reset(1, true);

    uint32_t drvCnt = 0;
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ENUMERATION, zeInit(ZE_INIT_FLAG_GPU_ONLY));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, zeDriverGet(&drvCnt, nullptr));
}

TEST(zeDriverGet, returnsCorrectNumberOfDriverInstances) {
    // Number of driver instances the system has.
    uint32_t nMockDrv = 1;

    Mock<Driver> driver;
    uint32_t drvCnt = 0;
    ze_driver_handle_t hDriver = nullptr;

    /*
     * Spec 1.0.4 - zeDriverGet()
     *
     * if count is non-zero, then the loader will only retrieve that
     * number of drivers. if count is larger than the number of drivers
     * available, then the loader will update the value with the correct
     * number of drivers available.
     */
    driver.reset(nMockDrv);
    zeInit(0);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, zeDriverGet(nullptr, nullptr));

    EXPECT_EQ(ZE_RESULT_SUCCESS, zeDriverGet(&drvCnt, nullptr));
    EXPECT_EQ(nMockDrv, drvCnt);

    drvCnt = 100; // should be adjusted.
    EXPECT_EQ(ZE_RESULT_SUCCESS, zeDriverGet(&drvCnt, &hDriver));
    EXPECT_EQ(nMockDrv, drvCnt);
}

using DriverVersionTest = Test<DeviceFixture>;

TEST_F(DriverVersionTest, returnsExpectedDriverVersion) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, driverHandle->getProperties(nullptr));

    ze_driver_properties_t properties;
    EXPECT_EQ(ZE_RESULT_SUCCESS, driverHandle->getProperties(&properties));
    EXPECT_EQ(DRIVER_VERSION, properties.driverVersion);

    ze_driver_uuid_t uuid = ze_intel_vpu_driver_uuid;
    EXPECT_EQ(memcmp(&properties.uuid, &uuid, sizeof(properties.uuid)), 0);
}

TEST_F(DriverVersionTest, checkGraphExtensionInDriverExtensionProperties) {
    uint32_t count = 0;
    EXPECT_EQ(driverHandle->getExtensionProperties(&count, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_GT(count, 0);

    std::vector<ze_driver_extension_properties_t> props(count);
    EXPECT_EQ(driverHandle->getExtensionProperties(&count, props.data()), ZE_RESULT_SUCCESS);

    EXPECT_EQ(std::string(props[0].name), ZE_GRAPH_EXT_NAME);
    EXPECT_EQ(props[0].version, ZE_GRAPH_EXT_VERSION_1_0);
}

TEST_F(DriverVersionTest, checkGraphExtensionFunctionTable) {
    ze_graph_dditable_ext_t *ddi = nullptr;
    ze_result_t res = driverHandle->getExtensionFunctionAddress(ZE_GRAPH_EXT_NAME,
                                                                reinterpret_cast<void **>(&ddi));
    EXPECT_EQ(ZE_RESULT_SUCCESS, res);

    EXPECT_NE(ddi->pfnCreate, nullptr);
    EXPECT_NE(ddi->pfnDestroy, nullptr);
    EXPECT_NE(ddi->pfnGetProperties, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties, nullptr);
    EXPECT_NE(ddi->pfnSetArgumentValue, nullptr);
    EXPECT_NE(ddi->pfnAppendGraphInitialize, nullptr);
    EXPECT_NE(ddi->pfnAppendGraphExecute, nullptr);
    EXPECT_NE(ddi->pfnGetNativeBinary, nullptr);
    EXPECT_NE(ddi->pfnDeviceGetGraphProperties, nullptr);
}

TEST_F(DriverVersionTest, checkGraphExtension_1_1_FunctionTable) {
    ze_graph_dditable_ext_1_1_t *ddi = nullptr;
    ze_result_t res = driverHandle->getExtensionFunctionAddress(ZE_GRAPH_EXT_NAME_1_1,
                                                                reinterpret_cast<void **>(&ddi));
    EXPECT_EQ(ZE_RESULT_SUCCESS, res);

    EXPECT_NE(ddi->pfnCreate, nullptr);
    EXPECT_NE(ddi->pfnDestroy, nullptr);
    EXPECT_NE(ddi->pfnGetProperties, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties, nullptr);
    EXPECT_NE(ddi->pfnSetArgumentValue, nullptr);
    EXPECT_NE(ddi->pfnAppendGraphInitialize, nullptr);
    EXPECT_NE(ddi->pfnAppendGraphExecute, nullptr);
    EXPECT_NE(ddi->pfnGetNativeBinary, nullptr);
    EXPECT_NE(ddi->pfnDeviceGetGraphProperties, nullptr);
    EXPECT_NE(ddi->pfnGraphGetArgumentMetadata, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties2, nullptr);
}

TEST_F(DriverVersionTest, checkGraphExtension_1_2_FunctionTable) {
    ze_graph_dditable_ext_1_2_t *ddi = nullptr;
    ze_result_t res = driverHandle->getExtensionFunctionAddress(ZE_GRAPH_EXT_NAME_1_2,
                                                                reinterpret_cast<void **>(&ddi));
    EXPECT_EQ(ZE_RESULT_SUCCESS, res);

    EXPECT_NE(ddi->pfnCreate, nullptr);
    EXPECT_NE(ddi->pfnDestroy, nullptr);
    EXPECT_NE(ddi->pfnGetProperties, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties, nullptr);
    EXPECT_NE(ddi->pfnSetArgumentValue, nullptr);
    EXPECT_NE(ddi->pfnAppendGraphInitialize, nullptr);
    EXPECT_NE(ddi->pfnAppendGraphExecute, nullptr);
    EXPECT_NE(ddi->pfnGetNativeBinary, nullptr);
    EXPECT_NE(ddi->pfnDeviceGetGraphProperties, nullptr);
    EXPECT_NE(ddi->pfnGraphGetArgumentMetadata, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties2, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties3, nullptr);
}

TEST_F(DriverVersionTest, checkGraphExtension_1_3_FunctionTable) {
    ze_graph_dditable_ext_1_3_t *ddi = nullptr;
    ze_result_t res = driverHandle->getExtensionFunctionAddress(ZE_GRAPH_EXT_NAME_1_3,
                                                                reinterpret_cast<void **>(&ddi));
    EXPECT_EQ(ZE_RESULT_SUCCESS, res);

    EXPECT_NE(ddi->pfnCreate, nullptr);
    EXPECT_NE(ddi->pfnDestroy, nullptr);
    EXPECT_NE(ddi->pfnGetProperties, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties, nullptr);
    EXPECT_NE(ddi->pfnSetArgumentValue, nullptr);
    EXPECT_NE(ddi->pfnAppendGraphInitialize, nullptr);
    EXPECT_NE(ddi->pfnAppendGraphExecute, nullptr);
    EXPECT_NE(ddi->pfnGetNativeBinary, nullptr);
    EXPECT_NE(ddi->pfnDeviceGetGraphProperties, nullptr);
    EXPECT_NE(ddi->pfnGraphGetArgumentMetadata, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties2, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties3, nullptr);
    EXPECT_NE(ddi->pfnQueryNetworkCreate, nullptr);
    EXPECT_NE(ddi->pfnQueryNetworkDestroy, nullptr);
    EXPECT_NE(ddi->pfnQueryNetworkGetSupportedLayers, nullptr);
}

TEST_F(DriverVersionTest, checkGraphExtension_1_4_FunctionTable) {
    ze_graph_dditable_ext_1_4_t *ddi = nullptr;
    ze_result_t res = driverHandle->getExtensionFunctionAddress(ZE_GRAPH_EXT_NAME_1_4,
                                                                reinterpret_cast<void **>(&ddi));
    EXPECT_EQ(ZE_RESULT_SUCCESS, res);

    EXPECT_NE(ddi->pfnCreate, nullptr);
    EXPECT_NE(ddi->pfnDestroy, nullptr);
    EXPECT_NE(ddi->pfnGetProperties, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties, nullptr);
    EXPECT_NE(ddi->pfnSetArgumentValue, nullptr);
    EXPECT_NE(ddi->pfnAppendGraphInitialize, nullptr);
    EXPECT_NE(ddi->pfnAppendGraphExecute, nullptr);
    EXPECT_NE(ddi->pfnGetNativeBinary, nullptr);
    EXPECT_NE(ddi->pfnDeviceGetGraphProperties, nullptr);
    EXPECT_NE(ddi->pfnGraphGetArgumentMetadata, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties2, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties3, nullptr);
    EXPECT_NE(ddi->pfnQueryNetworkCreate, nullptr);
    EXPECT_NE(ddi->pfnQueryNetworkDestroy, nullptr);
    EXPECT_NE(ddi->pfnQueryNetworkGetSupportedLayers, nullptr);
    EXPECT_NE(ddi->pfnBuildLogGetString, nullptr);
}

TEST_F(DriverVersionTest, checkGraphExtension_1_5_FunctionTable) {
    ze_graph_dditable_ext_1_5_t *ddi = nullptr;
    ze_result_t res = driverHandle->getExtensionFunctionAddress(ZE_GRAPH_EXT_NAME_1_5,
                                                                reinterpret_cast<void **>(&ddi));
    EXPECT_EQ(ZE_RESULT_SUCCESS, res);

    EXPECT_NE(ddi->pfnCreate, nullptr);
    EXPECT_NE(ddi->pfnDestroy, nullptr);
    EXPECT_NE(ddi->pfnGetProperties, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties, nullptr);
    EXPECT_NE(ddi->pfnSetArgumentValue, nullptr);
    EXPECT_NE(ddi->pfnAppendGraphInitialize, nullptr);
    EXPECT_NE(ddi->pfnAppendGraphExecute, nullptr);
    EXPECT_NE(ddi->pfnGetNativeBinary, nullptr);
    EXPECT_NE(ddi->pfnDeviceGetGraphProperties, nullptr);
    EXPECT_NE(ddi->pfnGraphGetArgumentMetadata, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties2, nullptr);
    EXPECT_NE(ddi->pfnGetArgumentProperties3, nullptr);
    EXPECT_NE(ddi->pfnQueryNetworkCreate, nullptr);
    EXPECT_NE(ddi->pfnQueryNetworkDestroy, nullptr);
    EXPECT_NE(ddi->pfnQueryNetworkGetSupportedLayers, nullptr);
    EXPECT_NE(ddi->pfnBuildLogGetString, nullptr);
    EXPECT_NE(ddi->pfnCreate2, nullptr);
    EXPECT_NE(ddi->pfnQueryNetworkCreate2, nullptr);
    EXPECT_NE(ddi->pfnQueryContextMemory, nullptr);
}

TEST_F(DriverVersionTest, checkGraphProfilingDataFunctionTable) {
    ze_graph_profiling_dditable_ext_t *ddi = nullptr;
    ze_result_t res = driverHandle->getExtensionFunctionAddress(ZE_PROFILING_DATA_EXT_NAME,
                                                                reinterpret_cast<void **>(&ddi));
    EXPECT_EQ(ZE_RESULT_SUCCESS, res);

    EXPECT_NE(ddi->pfnProfilingPoolCreate, nullptr);
    EXPECT_NE(ddi->pfnProfilingPoolDestroy, nullptr);
    EXPECT_NE(ddi->pfnProfilingQueryCreate, nullptr);
    EXPECT_NE(ddi->pfnProfilingQueryDestroy, nullptr);
    EXPECT_NE(ddi->pfnProfilingQueryGetData, nullptr);
    EXPECT_NE(ddi->pfnDeviceGetProfilingDataProperties, nullptr);
}

TEST_F(DriverVersionTest, returnsExpectedGetDriverPropertiesResultAndIPCPropertyFlagType) {
    ze_result_t res = driverHandle->getIPCProperties(nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, res);

    ze_driver_ipc_properties_t ipc_properties;
    res = driverHandle->getIPCProperties(&ipc_properties);
    EXPECT_EQ(ZE_RESULT_SUCCESS, res);

    EXPECT_EQ((uint32_t)ZE_IPC_PROPERTY_FLAG_MEMORY, ipc_properties.flags);
}

TEST_F(DriverVersionTest, checkEnvironmentVariableInitialization) {
    char *affinityMaskDefault = getenv("ZE_AFFINITY_MASK");
    char *enableMetricsDefault = getenv("ZET_ENABLE_METRICS");
    char *enablePciIdOrderDefault = getenv("ZE_ENABLE_PCI_ID_DEVICE_ORDER");
    char *sharedForceDeviceAllocDefault = getenv("ZE_SHARED_FORCE_DEVICE_ALLOC");
    char *umdLogLevel = getenv("ZE_INTEL_NPU_LOGLEVEL");

    unsetenv("ZE_AFFINITY_MASK");
    unsetenv("ZET_ENABLE_METRICS");
    unsetenv("ZE_ENABLE_PCI_ID_DEVICE_ORDER");
    unsetenv("ZE_SHARED_FORCE_DEVICE_ALLOC");
    unsetenv("ZE_INTEL_NPU_LOGLEVEL");

    driver.initializeEnvVariables();
    EXPECT_EQ(driver.getEnvVariables().affinityMask, "");
    EXPECT_EQ(driver.getEnvVariables().metrics, false);
    EXPECT_EQ(driver.getEnvVariables().pciIdDeviceOrder, false);
    EXPECT_EQ(driver.getEnvVariables().sharedForceDeviceAlloc, false);
    EXPECT_EQ(driver.getEnvVariables().umdLogLevel, "");

    setenv("ZE_AFFINITY_MASK", "0,1", 1);
    setenv("ZET_ENABLE_METRICS", "1", 1);
    setenv("ZE_ENABLE_PCI_ID_DEVICE_ORDER", "1", 1);
    setenv("ZE_SHARED_FORCE_DEVICE_ALLOC", "1", 1);
    setenv("ZE_INTEL_NPU_LOGLEVEL", "VERBOSE", 1);

    driver.initializeEnvVariables();
    EXPECT_EQ(driver.getEnvVariables().affinityMask, "0,1");
    EXPECT_EQ(driver.getEnvVariables().metrics, true);
    EXPECT_EQ(driver.getEnvVariables().pciIdDeviceOrder, true);
    EXPECT_EQ(driver.getEnvVariables().sharedForceDeviceAlloc, true);
    EXPECT_EQ(driver.getEnvVariables().umdLogLevel, "VERBOSE");

    affinityMaskDefault == nullptr ? unsetenv("ZE_AFFINITY_MASK")
                                   : setenv("ZE_AFFINITY_MASK", affinityMaskDefault, 1);
    enableMetricsDefault == nullptr ? unsetenv("ZET_ENABLE_METRICS")
                                    : setenv("ZET_ENABLE_METRICS", enableMetricsDefault, 1);
    enablePciIdOrderDefault == nullptr
        ? unsetenv("ZE_ENABLE_PCI_ID_DEVICE_ORDER")
        : setenv("ZE_ENABLE_PCI_ID_DEVICE_ORDER", enablePciIdOrderDefault, 1);
    sharedForceDeviceAllocDefault == nullptr
        ? unsetenv("ZE_SHARED_FORCE_DEVICE_ALLOC")
        : setenv("ZE_SHARED_FORCE_DEVICE_ALLOC", sharedForceDeviceAllocDefault, 1);
    umdLogLevel == nullptr ? unsetenv("ZE_INTEL_NPU_LOGLEVEL")
                           : setenv("ZE_INTEL_NPU_LOGLEVEL", umdLogLevel, 1);
}

} // namespace ult
} // namespace L0
