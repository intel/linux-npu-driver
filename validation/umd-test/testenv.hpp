/*
 * Copyright (C) 2023-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "test_app.h"
#include "umd_extensions.h"
#include "umd_test.h"

#include <filesystem>
#include <gtest/gtest.h>
#include <level_zero/zes_api.h>
#include <yaml-cpp/yaml.h>

namespace test_vars {
extern bool forceGpu;
extern bool disable_metrics;
extern bool forceZeInitTests;
extern int userRequestedTimeoutMs;
} // namespace test_vars

class Environment : public ::testing::Environment {
  public:
    Environment(const Environment &obj) = delete;
    Environment &operator=(const Environment &) = delete;

    struct tc {
        bool metricsEnable;
    } config =
        {
            .metricsEnable = 1,
        },
      configWithGpu =
          {
              .metricsEnable = 0,
          },
      configWithoutMetrics = {
          .metricsEnable = 0,
      };

    const char *zeDevTypeStr(ze_device_type_t devType) {
        const char *devStrings[] = {"Unknown", "GPU", "CPU", "FPGA", "MCA", "NPU"};
        // Unknown device type.
        if (devType < ZE_DEVICE_TYPE_GPU || devType > ZE_DEVICE_TYPE_VPU)
            return devStrings[0];

        return devStrings[(int)devType];
    }

    void SetUp() {
        if (test_vars::forceGpu) {
            config = configWithGpu;
            PRINTF("Testing with GPU L0.\n");
            PRINTF("Disabling metrics (ZET_ENABLE_METRICS=%d) (EISW-131452).\n",
                   config.metricsEnable);
        }

        if (test_vars::disable_metrics) {
            config = configWithoutMetrics;
            PRINTF("Disabling metrics (ZET_ENABLE_METRICS=%d). No metric test will be run.\n",
                   config.metricsEnable);
        }

        if (test_vars::forceZeInitTests) {
            return;
        }
        if (test_vars::userRequestedTimeoutMs > 0) {
            userRequestedTimeoutNs =
                static_cast<uint64_t>(test_vars::userRequestedTimeoutMs) * 1'000'000;
            PRINTF("Synchronization timeout changed to %d ms.\n",
                   test_vars::userRequestedTimeoutMs);
        }

        EXPECT_EQ(setenv("ZET_ENABLE_METRICS", config.metricsEnable ? "1" : "0", 0), 0);
        EXPECT_EQ(setenv("ZE_ENABLE_VALIDATION_LAYER", "1", 0), 0);
        EXPECT_EQ(setenv("ZE_ENABLE_PARAMETER_VALIDATION", "1", 0), 0);

        uint32_t drvCount = 0u;
        ze_init_driver_type_desc_t initDriverDesc{
            .stype = ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC,
            .pNext = nullptr,
            .flags = UINT32_MAX, // UINT32_MAX - all driver types requested
        };
        ASSERT_EQ(zeInitDrivers(&drvCount, nullptr, &initDriverDesc), ZE_RESULT_SUCCESS);
        ASSERT_GT(drvCount, 0);

        std::vector<ze_driver_handle_t> drivers(drvCount);
        ASSERT_EQ(zeInitDrivers(&drvCount, &drivers[0], &initDriverDesc), ZE_RESULT_SUCCESS);

        for (const auto &driver : drivers) {
            uint32_t devCount = 0u;
            ASSERT_EQ(zeDeviceGet(driver, &devCount, nullptr), ZE_RESULT_SUCCESS);
            if (devCount != 1)
                continue;

            ze_device_handle_t device;
            ASSERT_EQ(zeDeviceGet(driver, &devCount, &device), ZE_RESULT_SUCCESS);

            // The ze_device_ip_version_ext_t structure used only to query the driver
            // for the platform type. The 'platfromType' value will be stored in ipVersion.
            ze_device_ip_version_ext_t devDetails = {};
            devDetails.stype = ZE_STRUCTURE_TYPE_DEVICE_IP_VERSION_EXT;
            devDetails.pNext = nullptr;

            ze_device_properties_t devProp = {};
            devProp.stype = ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES;
            devProp.pNext = &devDetails;

            ASSERT_EQ(zeDeviceGetProperties(device, &devProp), ZE_RESULT_SUCCESS);

            TRACE("Driver: %p - Device %s: %p\n", driver, zeDevTypeStr(devProp.type), device);
            if (devProp.type == ZE_DEVICE_TYPE_VPU) {
                zeDriver = driver;
                zeDevice = device;
                maxMemAllocSize = devProp.maxMemAllocSize;
                pciDevId = devProp.deviceId;
                ASSERT_NE(pciDevId, 0u);

                platformType = devDetails.ipVersion;
            } else if (devProp.type == ZE_DEVICE_TYPE_GPU) {
                zeDriverGpu = driver;
                zeDeviceGpu = device;
            };
        }

        ASSERT_NE(zeDriver, nullptr) << "Failed to initialize Driver";
        ASSERT_NE(zeDevice, nullptr) << "Failed to initialize Device";
        ASSERT_GT(maxMemAllocSize, 0);

        if (test_vars::forceGpu) {
            ASSERT_NE(zeDriverGpu, nullptr) << "Failed to initialize Driver GPU L0";
            ASSERT_NE(zeDeviceGpu, nullptr) << "Failed to initialize Device GPU L0";
        }

        ASSERT_EQ(
            zeDriverGetExtensionFunctionAddress(zeDriver,
                                                ZE_GRAPH_EXT_NAME,
                                                reinterpret_cast<void **>(&zeGraphDDITableExt)),
            ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeDriverGetExtensionFunctionAddress(
                      zeDriver,
                      ZE_PROFILING_DATA_EXT_NAME,
                      reinterpret_cast<void **>(&zeGraphProfilingDDITableExt)),
                  ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeDriverGetExtensionFunctionAddress(
                      zeDriver,
                      ZE_COMMAND_QUEUE_NPU_EXT_NAME,
                      reinterpret_cast<void **>(&zeCommandQueueDDITableExt)),
                  ZE_RESULT_SUCCESS);

        ASSERT_NE(zeGraphDDITableExt, nullptr) << "Failed to find graph DDI table";
        ASSERT_NE(zeGraphProfilingDDITableExt, nullptr)
            << "Failed to find graph profiling DDI table";
        ASSERT_NE(zeCommandQueueDDITableExt, nullptr) << "Failed to find command queue DDI table";
    }

    ze_driver_handle_t getDriver() { return zeDriver; }
    ze_device_handle_t getDevice() { return zeDevice; }
    graph_dditable_ext_t *getGraphDDITable() { return zeGraphDDITableExt; }
    ze_graph_profiling_dditable_ext_t *getGraphProfilingDDITable() {
        return zeGraphProfilingDDITableExt;
    }
    command_queue_dditable_t *getCommandQueueDDITable() { return zeCommandQueueDDITableExt; }
    uint64_t getMaxMemAllocSize() { return maxMemAllocSize; }
    uint16_t getPciDevId() { return pciDevId; }
    uint16_t getPlatformType() { return platformType; }
    uint64_t getUserSyncTimeoutNs() { return userRequestedTimeoutNs; }

    ze_driver_handle_t getDriverGpu() { return zeDriverGpu; }
    ze_device_handle_t getDeviceGpu() { return zeDeviceGpu; }

    static Environment *getInstance() {
        static Environment *testEnv = nullptr;

        if (!testEnv)
            testEnv = new Environment();
        return testEnv;
    }

    static YAML::Node &getConfiguration() {
        static YAML::Node config;

        return config;
    }

    static std::vector<YAML::Node> getConfiguration(const char *nodeName) {
        std::vector<YAML::Node> emptyNodeVector;

        if (!getConfiguration()[nodeName].IsDefined())
            return emptyNodeVector;
        try {
            return getConfiguration()[nodeName].as<std::vector<YAML::Node>>();
        } catch (YAML::Exception &e) {
            PRINTF("Bad node: %s reason: %s\n", nodeName, e.what());
            return emptyNodeVector;
        }
    }

    static bool loadConfiguration(const char *configFilePath) {
        if (!configFilePath)
            return true;

        std::vector<std::filesystem::path> searchPathPrefixes = {""};

        /* For not absolute paths add search directories*/
        if (configFilePath[0] != '/') {
            searchPathPrefixes.push_back("./");
            searchPathPrefixes.push_back("/usr/share/vpu/");
            searchPathPrefixes.push_back("/usr/share/vpu/validation/umd-test/configs/");
        }

        for (auto &pathPrefix : searchPathPrefixes) {
            std::string configFile = pathPrefix;

            configFile.append(configFilePath);
            if (std::filesystem::exists(configFile)) {
                YAML::Node &config = getConfiguration();
                try {
                    config = YAML::LoadFile(configFile);
                } catch (YAML::Exception &e) {
                    PRINTF("Parsing configuration file failed:\n");
                    PRINTF("%s\n", e.what());
                    return false;
                }
                return true;
            }
        }

        PRINTF("Configuration file not found!\n");
        return false;
    }

    static bool setupGlobalConfig(const char *configFilePath) {
        if (!Environment::loadConfiguration(configFilePath))
            return false;

        YAML::Node &config = getConfiguration();
        try {
            if (config["log_level"].IsDefined()) {
                std::vector<std::string> validLevels = {"QUIET",
                                                        "ERROR",
                                                        "WARNING",
                                                        "INFO",
                                                        "VERBOSE"};
                std::string level = config["log_level"].as<std::string>();

                if (std::find(validLevels.begin(), validLevels.end(), level) != validLevels.end()) {
                    if (setenv("ZE_INTEL_NPU_LOGLEVEL", level.c_str(), 0) != 0) {
                        PRINTF("Set log level to requested %s failed.\n", level.c_str());
                        return false;
                    }
                } else {
                    PRINTF("Requested log level: %s is not valid value.\n", level.c_str());
                    return false;
                }
            }
        } catch (YAML::Exception &e) {
            PRINTF("Parsing configuration file failed\n");
            PRINTF("%s\n", e.what());
            return false;
        }
        return true;
    }

  private:
    Environment() = default;

    ze_driver_handle_t zeDriver = nullptr;
    ze_device_handle_t zeDevice = nullptr;
    graph_dditable_ext_t *zeGraphDDITableExt = nullptr;
    ze_graph_profiling_dditable_ext_t *zeGraphProfilingDDITableExt = nullptr;
    /** @brief Pointer to the Level Zero API command queue extension DDI table */
    command_queue_dditable_t *zeCommandQueueDDITableExt = nullptr;
    uint64_t maxMemAllocSize = 0;
    uint16_t pciDevId = 0;
    uint32_t platformType = 0;
    uint64_t userRequestedTimeoutNs = 0;

    ze_driver_handle_t zeDriverGpu = nullptr;
    ze_device_handle_t zeDeviceGpu = nullptr;
};
