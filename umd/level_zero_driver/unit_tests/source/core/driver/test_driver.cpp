/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

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
    ze_result_t res = driverHandle->getProperties(nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, res);

    ze_driver_properties_t properties;
    res = driverHandle->getProperties(&properties);
    EXPECT_EQ(ZE_RESULT_SUCCESS, res);

    uint32_t versionMajor = (properties.driverVersion & 0xFF000000) >> 24;
    uint32_t versionMinor = (properties.driverVersion & 0x00FF0000) >> 16;
    uint32_t versionBuild = properties.driverVersion & 0x0000FFFF;

    EXPECT_EQ(boost::numeric_cast<uint32_t>(L0_PROJECT_VERSION_MAJOR), versionMajor);
    EXPECT_EQ(boost::numeric_cast<uint32_t>(L0_PROJECT_VERSION_MINOR), versionMinor);
    EXPECT_EQ(boost::numeric_cast<uint32_t>(VPU_VERSION_BUILD), versionBuild);

    // TODO: Reuse uuid from ze-ext-headers
    ze_driver_uuid_t uuid = {0x01,
                             0x7d,
                             0xe9,
                             0x31,
                             0x6b,
                             0x4d,
                             0x4f,
                             0xd4,
                             0xaa,
                             0x9b,
                             0x5b,
                             0xed,
                             0x77,
                             0xfc,
                             0x8e,
                             0x89};
    EXPECT_EQ(memcmp(&properties.uuid, &uuid, sizeof(properties.uuid)), 0);
}

TEST_F(DriverVersionTest, givenCallToGetExtensionPropertiesThenUnsupportedIsReturned) {
    uint32_t count = 0;
    ze_driver_extension_properties_t properties;
    ze_result_t res = driverHandle->getExtensionProperties(&count, &properties);
    EXPECT_EQ(ZE_RESULT_ERROR_UNSUPPORTED_FEATURE, res);
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
    char *umdLogLevel = getenv("VPU_DRV_UMD_LOGLEVEL");

    unsetenv("ZE_AFFINITY_MASK");
    unsetenv("ZET_ENABLE_METRICS");
    unsetenv("ZE_ENABLE_PCI_ID_DEVICE_ORDER");
    unsetenv("ZE_SHARED_FORCE_DEVICE_ALLOC");
    unsetenv("VPU_DRV_UMD_LOGLEVEL");

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
    setenv("VPU_DRV_UMD_LOGLEVEL", "VERBOSE", 1);

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
    umdLogLevel == nullptr ? unsetenv("VPU_DRV_UMD_LOGLEVEL")
                           : setenv("VPU_DRV_UMD_LOGLEVEL", umdLogLevel, 1);
}

} // namespace ult
} // namespace L0
