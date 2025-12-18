/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include "utilities/data_handle.h"
#include "ze_stringify.hpp"

#include <exception>
#include <fcntl.h>
#include <fstream>
#include <functional>
#include <level_zero/ze_api.h>
#include <level_zero/ze_mem_import_system_memory_ext.h>
#include <random>
#include <sys/resource.h>
#include <sys/sysinfo.h>

void PrintTo(const ze_result_t &result, std::ostream *os) {
    *os << ze_result_to_str(result) << " (0x" << std::hex << result << ")" << std::dec;
}

void UmdTest::CommandQueueGroupSetUpNpu(ze_device_handle_t dev) {
    uint32_t cmdGrpCount = 0;
    ASSERT_EQ(zeDeviceGetCommandQueueGroupProperties(dev, &cmdGrpCount, nullptr),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(cmdGrpCount, 1u);
    ze_command_queue_group_properties_t cmdGroupProps = {};

    cmdGroupProps.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_GROUP_PROPERTIES;

    ASSERT_EQ(zeDeviceGetCommandQueueGroupProperties(dev, &cmdGrpCount, &cmdGroupProps),
              ZE_RESULT_SUCCESS);

    EXPECT_TRUE(cmdGroupProps.flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE);
}

void UmdTest::CommandQueueGroupSetUpGpu(ze_device_handle_t dev,
                                        uint32_t &compOrdinal,
                                        uint32_t &copyOrdinal) {
    uint32_t cmdGrpCount = 0;
    ASSERT_EQ(zeDeviceGetCommandQueueGroupProperties(dev, &cmdGrpCount, nullptr),
              ZE_RESULT_SUCCESS);

    std::vector<ze_command_queue_group_properties_t> cmdGroupProps;
    cmdGroupProps.resize(cmdGrpCount);
    for (auto &v : cmdGroupProps)
        v.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_GROUP_PROPERTIES;

    ASSERT_EQ(zeDeviceGetCommandQueueGroupProperties(dev, &cmdGrpCount, cmdGroupProps.data()),
              ZE_RESULT_SUCCESS);

    for (uint32_t i = 0; i < cmdGrpCount; i++) {
        if (cmdGroupProps[i].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE) {
            compOrdinal = i;
        } else if (cmdGroupProps[i].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COPY) {
            copyOrdinal = i;
        }
    }

    ASSERT_NE(compOrdinal, std::numeric_limits<uint32_t>::max());
    ASSERT_NE(copyOrdinal, std::numeric_limits<uint32_t>::max());
}

void UmdTest::SetUp() {
    Environment *testEnv = Environment::getInstance();

    ASSERT_NE(testEnv, nullptr);
    zeDriver = testEnv->getDriver();
    zeDevice = testEnv->getDevice();
    zeGraphDDITableExt = testEnv->getGraphDDITable();
    zeGraphProfilingDDITableExt = testEnv->getGraphProfilingDDITable();
    zeCommandQueueDDITableExt = testEnv->getCommandQueueDDITable();
    maxMemAllocSize = testEnv->getMaxMemAllocSize();
    pciDevId = testEnv->getPciDevId();
    platformType = testEnv->getPlatformType();

    ze_result_t ret;
    ze_context_desc_t contextDesc = {.stype = ZE_STRUCTURE_TYPE_CONTEXT_DESC,
                                     .pNext = nullptr,
                                     .flags = 0};

    scopedContext = zeScope::contextCreate(zeDriver, contextDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    zeContext = scopedContext.get();
    CommandQueueGroupSetUpNpu(zeDevice);

    if (test_vars::forceGpu) {
        zeDriverGpu = testEnv->getDriverGpu();
        zeDeviceGpu = testEnv->getDeviceGpu();
        scopedContextGpu = zeScope::contextCreate(zeDriverGpu, contextDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        zeContextGpu = scopedContextGpu.get();
        CommandQueueGroupSetUpGpu(zeDeviceGpu, computeGrpOrdinalGpu, copyGrpOrdinalGpu);
    }

    if (testEnv->getUserSyncTimeoutNs()) {
        syncTimeout = testEnv->getUserSyncTimeoutNs();
        graphSyncTimeout = testEnv->getUserSyncTimeoutNs();
    } else if (!isSilicon()) {
        syncTimeout = 30'000'000'000; // 30 seconds
    }

    /*Get base configuration from config file*/
    YAML::Node &configuration = Environment::getConfiguration();
    if (configuration["blob_dir"].IsDefined()) {
        globalConfig.blobDir = configuration["blob_dir"].as<std::string>();
    }
    if (configuration["image_dir"].IsDefined()) {
        globalConfig.imageDir = configuration["image_dir"].as<std::string>();
    }

    if (configuration["model_dir"].IsDefined()) {
        globalConfig.modelDir = configuration["model_dir"].as<std::string>();
    }
}

void UmdTest::TearDown() {
    std::this_thread::sleep_for(std::chrono::milliseconds(test_app::pause_after_test_ms));
}

int UmdTest::saveFile(const std::string &filePath, void *dataIn, size_t inputSize) {
    std::ofstream fileOutputStream(filePath, std::ios::out | std::ios::binary);
    if (fileOutputStream.is_open()) {
        fileOutputStream.write((const char *)dataIn, inputSize);

        fileOutputStream.close();

        return 0;
    }
    return -1;
}

bool UmdTest::isSilicon() {
    return (platformType == 0u);
}

bool UmdTest::isImportSystemMemorySupported() {
    ze_device_external_memory_properties_t extMemProps = {};
    extMemProps.stype = ZE_STRUCTURE_TYPE_DEVICE_EXTERNAL_MEMORY_PROPERTIES;
    auto ret = zeDeviceGetExternalMemoryProperties(zeDevice, &extMemProps);
    if (ret != ZE_RESULT_SUCCESS) {
        TRACE("Failed to get external memory properties");
        return false;
    }

    return extMemProps.memoryAllocationImportTypes &
           ZE_EXTERNAL_MEMORY_TYPE_FLAG_STANDARD_ALLOCATION; // NOLINT
}

std::shared_ptr<void> UmdTest::AllocSharedMemory(size_t size, ze_host_mem_alloc_flags_t flagsHost) {
    return zeMemory::allocShared(zeContext, zeDevice, size, flagsHost);
}

std::shared_ptr<void> UmdTest::AllocDeviceMemory(size_t size) {
    return zeMemory::allocDevice(zeContext, zeDevice, size);
}

std::shared_ptr<void> UmdTest::AllocHostMemory(size_t size, ze_host_mem_alloc_flags_t flagsHost) {
    return zeMemory::allocHost(zeContext, size, flagsHost);
}

std::shared_ptr<void>
UmdTest::importSystemMemory(void *ptr, size_t size, bool readOnly /* = false */) {
    ze_external_memory_import_system_memory_t importSystemMemory = {
        ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_SYSTEM_MEMORY, // NOLINT
        nullptr,
        ptr,
        size};
    ze_host_mem_alloc_desc_t hostMemAllocDesc = {
        ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC,
        &importSystemMemory,
        readOnly ? ZE_HOST_MEM_ALLOC_FLAG_BIAS_WRITE_COMBINED : ZE_HOST_MEM_ALLOC_FLAG_BIAS_CACHED,
    };

    auto ret = ZE_RESULT_SUCCESS;
    auto mem = zeScope::memAllocHost(zeContext, hostMemAllocDesc, size, 0, ret);
    if (ret != ZE_RESULT_SUCCESS) {
        TRACE("Failed to import standard allocation, error: %s\n", ze_result_to_str(ret));
        return nullptr;
    }
    if (mem.get() != ptr) {
        TRACE("Imported pointer does not match the original pointer\n");
        return nullptr;
    }
    return mem;
}

bool UmdTest::isHwsModeEnabled() {
    std::string deviceSysFs = getDeviceSysFsDirectory();
    if (deviceSysFs.empty())
        return false;

    std::vector<char> schedMode;
    if (DataHandle::loadFile(deviceSysFs + "/sched_mode", schedMode) != 0)
        return false;

    if (schedMode.size() < std::string("HW").size())
        return false;

    const std::string modeAsString(schedMode.begin(), schedMode.begin() + std::string("HW").size());
    if (modeAsString.compare("HW") == 0)
        return true;
    return false;
}

void UmdTest::printMemoryUsage(const char *prefix) {
    if (!test_app::verbose_logs) {
        return;
    }

    std::string stats;
    ze_graph_memory_query_t memoryUsage = {};
    if (zeGraphDDITableExt->pfnQueryContextMemory(zeContext,
                                                  ZE_GRAPH_QUERY_MEMORY_DDR,
                                                  &memoryUsage) == ZE_RESULT_SUCCESS) {
        stats += "NPU UMD usage: " + std::to_string(memoryUsage.allocated / 1024) + " KB";
    } else {
        TRACE("Failed to get graph memory usage.\n");
    }

    struct sysinfo sysStats = {};
    if (sysinfo(&sysStats) == 0) {
        stats +=
            ", System memory used: " +
            std::to_string(((sysStats.totalram - sysStats.freeram) * sysStats.mem_unit) / 1024) +
            " KB";
    } else {
        TRACE("Failed to get system memory usage.\n");
    }

    struct rusage usage = {};
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        stats += ", Process MaxRSS: " + std::to_string(usage.ru_maxrss) + " KB";
    } else {
        TRACE("Failed to get process memory usage.\n");
    }

    int fd = open("/proc/self/statm", O_RDONLY);
    if (fd != -1) {
        char buffer[64] = {};
        if (read(fd, buffer, sizeof(buffer) - 1) > 0) {
            unsigned long size = 0, resident = 0, shared = 0;
            if (sscanf(buffer, "%lu %lu %lu", &size, &resident, &shared) == 3) {
                long pageSize = sysconf(_SC_PAGESIZE);
                if (pageSize > 0) {
                    stats += ", VmSize: " + std::to_string((size * pageSize) / 1024) + " KB";
                    stats += ", VmRSS: " + std::to_string((resident * pageSize) / 1024) + " KB";
                    stats += ", VmShared: " + std::to_string((shared * pageSize) / 1024) + " KB";
                }
            }
        }
        close(fd);
    } else {
        TRACE("Failed to get process memory usage from statm.\n");
    }

    TRACE("%s: %s\n", prefix, stats.c_str());
}

TEST(Umd, ZeDevTypeStr) {
    Environment *testEnv = Environment::getInstance();
    // NOLINTBEGIN
    EXPECT_NE(testEnv->zeDevTypeStr((ze_device_type_t)0), nullptr);
    EXPECT_NE(testEnv->zeDevTypeStr((ze_device_type_t)-1), nullptr);
    EXPECT_NE(testEnv->zeDevTypeStr((ze_device_type_t)(ZE_DEVICE_TYPE_MCA + 1)), nullptr);
    // NOLINTEND
    EXPECT_NE(testEnv->zeDevTypeStr(ZE_DEVICE_TYPE_VPU), nullptr);
    EXPECT_NE(testEnv->zeDevTypeStr(ZE_DEVICE_TYPE_VPU), "Unknown");
    TRACE_STR(testEnv->zeDevTypeStr(ZE_DEVICE_TYPE_VPU));
}

TEST(Umd, File) {
    const char *testFileName = "/tmp/UmdTest_data.bin";
    std::vector<char> inputFileData;
    uint32_t writeData[1028];

    memset(writeData, 0xA5, sizeof(writeData));

    UmdTest::saveFile(testFileName, writeData, 1);
    DataHandle::loadFile(testFileName, inputFileData);
    EXPECT_EQ(inputFileData.size(), 1u);
    EXPECT_EQ(memcmp(writeData, inputFileData.data(), inputFileData.size()), 0);

    UmdTest::saveFile(testFileName, writeData, sizeof(writeData));
    DataHandle::loadFile(testFileName, inputFileData);
    EXPECT_EQ(inputFileData.size(), sizeof(writeData));
    EXPECT_EQ(memcmp(writeData, inputFileData.data(), inputFileData.size()), 0);
    std::filesystem::remove(testFileName);
}

TEST(Umd, ConfigurationCheck) {
    YAML::Node &config = Environment::getConfiguration();
    EXPECT_GT(config.size(), 0) << "Missed configuration file." << std::endl
                                << "Test scope is reduced." << std::endl
                                << "Provide valid configurtion file, use:" << std::endl
                                << "npu-umd-test -c/--config [CONFIGURATION_PATH]";
}
