/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>

#include "umd_test.h"
#include "testenv.hpp"

void PrintTo(const ze_result_t &ze_result, std::ostream *os) {
    *os << "0x" << std::hex << ze_result << std::dec;
}

const char *UmdTest::zeDevTypeStr(ze_device_type_t devType) {
    const char *devStrings[] = {"Unknown", "GPU", "CPU", "FPGA", "MCA", "VPU"};
    // Unknown device type.
    if (devType < ZE_DEVICE_TYPE_GPU || devType > ZE_DEVICE_TYPE_VPU) {
        return devStrings[0];
    }
    return devStrings[(int)devType];
}

void UmdTest::CommandQueueGroupSetUp() {
    uint32_t cmdGrpCount = 0;
    ASSERT_EQ(zeDeviceGetCommandQueueGroupProperties(zeDevice, &cmdGrpCount, nullptr),
              ZE_RESULT_SUCCESS);

    std::vector<ze_command_queue_group_properties_t> cmdGroupProps;
    cmdGroupProps.resize(cmdGrpCount);
    for (auto &v : cmdGroupProps)
        v.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_GROUP_PROPERTIES;

    ASSERT_EQ(zeDeviceGetCommandQueueGroupProperties(zeDevice, &cmdGrpCount, cmdGroupProps.data()),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdGrpCount, 2u);

    for (uint32_t i = 0; i < cmdGrpCount; i++) {
        if (cmdGroupProps[i].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE) {
            computeGrpOrdinal = i;
        } else if (cmdGroupProps[i].flags & ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COPY) {
            copyGrpOrdinal = i;
        }
    }

    ASSERT_NE(computeGrpOrdinal, std::numeric_limits<uint32_t>::max());
    ASSERT_NE(copyGrpOrdinal, std::numeric_limits<uint32_t>::max());
}

void UmdTest::SetUp() {
    Environment *testEnv = Environment::getInstance();

    ASSERT_NE(testEnv, nullptr);
    zeDriver = testEnv->getDriver();
    zeDevice = testEnv->getDevice();
    zeGraphDDITableExt = testEnv->getGraphDDITable();
    zeGraphProfilingDDITableExt = testEnv->getGraphProfilingDDITable();
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
    CommandQueueGroupSetUp();

    if (!isSilicon()) {
        syncTimeout = 30'000'000'000;       // 30 seconds
        graphSyncTimeout = 600'000'000'000; // 10 minutes
    }
}

void UmdTest::TearDown() {
    std::this_thread::sleep_for(std::chrono::milliseconds(test_app::pause_after_test_ms));
}

bool UmdTest::loadFile(const std::string &filePath, std::vector<char> &dataOut) {
    std::ifstream fileInputStream(filePath, std::ios::binary | std::ios::ate);
    if (fileInputStream.is_open()) {
        std::streamsize size = fileInputStream.tellg();
        fileInputStream.seekg(0, std::ios::beg);

        dataOut.resize(size);
        fileInputStream.read((char *)dataOut.data(), size);

        fileInputStream.close();

        return true;
    }
    return false;
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

std::shared_ptr<void> UmdTest::AllocSharedMemory(size_t size, ze_host_mem_alloc_flags_t flagsHost) {
    ze_device_mem_alloc_desc_t deviceMemAllocDesc = {.stype =
                                                         ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
                                                     .pNext = nullptr,
                                                     .flags = 0,
                                                     .ordinal = 0};

    ze_host_mem_alloc_desc_t hostMemAllocDesc = {.stype = ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC,
                                                 .pNext = nullptr,
                                                 .flags = flagsHost};

    ze_result_t ret;
    auto scopedMem = zeScope::memAllocShared(zeContext,
                                             deviceMemAllocDesc,
                                             hostMemAllocDesc,
                                             size,
                                             0,
                                             zeDevice,
                                             ret);
    if (ret != ZE_RESULT_SUCCESS)
        throw std::runtime_error("Failed to allocate shared memory");

    memset(scopedMem.get(), 0, size);
    return scopedMem;
}

std::shared_ptr<void> UmdTest::AllocDeviceMemory(size_t size) {
    ze_device_mem_alloc_desc_t deviceMemAllocDesc = {.stype =
                                                         ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
                                                     .pNext = nullptr,
                                                     .flags = 0,
                                                     .ordinal = 0};

    ze_result_t ret;
    auto scopedMem = zeScope::memAllocDevice(zeContext, deviceMemAllocDesc, size, 0, zeDevice, ret);
    if (ret != ZE_RESULT_SUCCESS)
        throw std::runtime_error("Failed to allocate device memory");

    return scopedMem;
}

std::shared_ptr<void> UmdTest::AllocHostMemory(size_t size, ze_host_mem_alloc_flags_t flagsHost) {
    ze_host_mem_alloc_desc_t hostMemAllocDesc = {.stype = ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC,
                                                 .pNext = nullptr,
                                                 .flags = flagsHost};

    ze_result_t ret;
    auto scopedMem = zeScope::memAllocHost(zeContext, hostMemAllocDesc, size, 0, ret);
    if (ret != ZE_RESULT_SUCCESS)
        throw std::runtime_error("Failed to allocate host memory");

    memset(scopedMem.get(), 0, size);
    return scopedMem;
}

TEST(Umd, ZeDevTypeStr) {
    EXPECT_NE(UmdTest::zeDevTypeStr((ze_device_type_t)0), nullptr);
    EXPECT_NE(UmdTest::zeDevTypeStr((ze_device_type_t)-1), nullptr);
    EXPECT_NE(UmdTest::zeDevTypeStr((ze_device_type_t)(ZE_DEVICE_TYPE_MCA + 1)), nullptr);
    EXPECT_NE(UmdTest::zeDevTypeStr(ZE_DEVICE_TYPE_VPU), nullptr);
    EXPECT_NE(UmdTest::zeDevTypeStr(ZE_DEVICE_TYPE_VPU), "Unknown");
    TRACE_STR(UmdTest::zeDevTypeStr(ZE_DEVICE_TYPE_VPU));
}

TEST(Umd, File) {
    const char *testFileName = "/tmp/UmdTest_data.bin";
    std::vector<char> inputFileData;
    uint32_t writeData[1028];

    memset(writeData, 0xA5, sizeof(writeData));

    UmdTest::saveFile(testFileName, writeData, 1);
    UmdTest::loadFile(testFileName, inputFileData);
    EXPECT_EQ(inputFileData.size(), 1u);
    EXPECT_EQ(memcmp(writeData, inputFileData.data(), inputFileData.size()), 0);

    UmdTest::saveFile(testFileName, writeData, sizeof(writeData));
    UmdTest::loadFile(testFileName, inputFileData);
    EXPECT_EQ(inputFileData.size(), sizeof(writeData));
    EXPECT_EQ(memcmp(writeData, inputFileData.data(), inputFileData.size()), 0);
    std::filesystem::remove(testFileName);
}

TEST(Umd, ConfigurationCheck) {
    YAML::Node &config = Environment::getConfiguration();
    EXPECT_GT(config.size(), 0) << "Missed configuration file." << std::endl
                                << "Test scope is reduced." << std::endl
                                << "Provide valid configurtion file, use:" << std::endl
                                << "vpu-umd-test -c/--config [CONFIGURATION_PATH]";
}
