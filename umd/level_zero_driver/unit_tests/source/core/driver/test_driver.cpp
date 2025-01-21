/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "gtest/gtest.h"
#include "level_zero/ze_intel_npu_uuid.h"
#include "level_zero_driver/source/driver.hpp"
#include "level_zero_driver/source/driver_handle.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "level_zero_driver/unit_tests/mocks/mock_driver.hpp"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <level_zero/ze_api.h>
#include <memory>
#include <stdlib.h>
#include <string.h>

namespace L0 {
namespace ult {

TEST(zeInit, whenCallingZeInitThenInitializeOnDriverIsCalled) {
    Mock<Driver> driver;
    driver.reset();

    auto result = L0::init(0);
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);
    EXPECT_EQ(1u, driver.initCalledCount);
}

TEST(zeInit, returnErrorWhenNotInitialized) {
    Mock<Driver> driver;
    driver.reset(1, true);

    uint32_t drvCnt = 0;
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, L0::init(ZE_INIT_FLAG_GPU_ONLY));
    EXPECT_EQ(ZE_RESULT_ERROR_UNINITIALIZED, L0::driverHandleGet(&drvCnt, nullptr));
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
    L0::init(0);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, L0::driverHandleGet(nullptr, nullptr));

    EXPECT_EQ(ZE_RESULT_SUCCESS, L0::driverHandleGet(&drvCnt, nullptr));
    EXPECT_EQ(nMockDrv, drvCnt);

    drvCnt = 100; // should be adjusted.
    EXPECT_EQ(ZE_RESULT_SUCCESS, L0::driverHandleGet(&drvCnt, &hDriver));
    EXPECT_EQ(nMockDrv, drvCnt);
}

using DriverVersionTest = Test<DeviceFixture>;

TEST_F(DriverVersionTest, returnsExpectedDriverVersion) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, driverHandle->getProperties(nullptr));

    ze_driver_properties_t properties;
    EXPECT_EQ(ZE_RESULT_SUCCESS, driverHandle->getProperties(&properties));
    EXPECT_EQ(DRIVER_VERSION, properties.driverVersion);

    ze_driver_uuid_t uuid = ze_intel_npu_driver_uuid;
    EXPECT_EQ(memcmp(&properties.uuid, &uuid, sizeof(properties.uuid)), 0);
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
    EXPECT_EQ(VPU::getLogLevel(), UMD_LOGLEVEL);

    setenv("ZE_AFFINITY_MASK", "0,1", 1);
    setenv("ZET_ENABLE_METRICS", "1", 1);
    setenv("ZE_ENABLE_PCI_ID_DEVICE_ORDER", "1", 1);
    setenv("ZE_SHARED_FORCE_DEVICE_ALLOC", "1", 1);
    setenv("ZE_INTEL_NPU_LOGLEVEL", "INFO", 1);

    driver.initializeEnvVariables();
    driver.initializeLogging();
    EXPECT_EQ(driver.getEnvVariables().affinityMask, "0,1");
    EXPECT_EQ(driver.getEnvVariables().metrics, true);
    EXPECT_EQ(driver.getEnvVariables().pciIdDeviceOrder, true);
    EXPECT_EQ(driver.getEnvVariables().sharedForceDeviceAlloc, true);
    EXPECT_EQ(VPU::getLogLevel(), INFO);

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
