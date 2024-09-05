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
#include <fstream>
#include <functional>
#include <random>

void PrintTo(const ze_result_t &result, std::ostream *os) {
    *os << ze_result_to_str(result) << " (0x" << std::hex << result << ")" << std::dec;
}

void UmdTest::CommandQueueGroupSetUp(ze_device_handle_t dev,
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
    CommandQueueGroupSetUp(zeDevice, computeGrpOrdinal, copyGrpOrdinal);

    if (test_vars::test_with_gpu) {
        zeDriverGpu = testEnv->getDriverGpu();
        zeDeviceGpu = testEnv->getDeviceGpu();
        scopedContextGpu = zeScope::contextCreate(zeDriverGpu, contextDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        zeContextGpu = scopedContextGpu.get();
        CommandQueueGroupSetUp(zeDeviceGpu, computeGrpOrdinalGpu, copyGrpOrdinalGpu);
    }

    if (!isSilicon()) {
        syncTimeout = 30'000'000'000;       // 30 seconds
        graphSyncTimeout = 600'000'000'000; // 10 minutes
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

std::shared_ptr<void> UmdTest::AllocSharedMemory(size_t size, ze_host_mem_alloc_flags_t flagsHost) {
    return zeMemory::allocShared(zeContext, zeDevice, size, flagsHost);
}

std::shared_ptr<void> UmdTest::AllocDeviceMemory(size_t size) {
    return zeMemory::allocDevice(zeContext, zeDevice, size);
}

std::shared_ptr<void> UmdTest::AllocHostMemory(size_t size, ze_host_mem_alloc_flags_t flagsHost) {
    return zeMemory::allocHost(zeContext, size, flagsHost);
}

std::vector<char> UmdTest::getFlagsFromString(std::string flags) {
    std::vector<char> buildFlags;

    for (auto c : flags)
        buildFlags.push_back(c);
    buildFlags.push_back('\0');
    return buildFlags;
}

void UmdTest::createGraphDescriptorForModel(const std::string &modelPath,
                                            const std::vector<char> &modelBuildFlags,
                                            std::vector<uint8_t> &testModelIR,
                                            ze_graph_desc_2_t &graphDesc) {
    std::vector<char> testModelXml, testModelBin;
    ze_device_graph_properties_t pDeviceGraphProperties;

    ASSERT_TRUE(getModelFromPath(modelPath, testModelXml, testModelBin));

    ASSERT_EQ(zeGraphDDITableExt->pfnDeviceGetGraphProperties(zeDevice, &pDeviceGraphProperties),
              ZE_RESULT_SUCCESS);

    ze_graph_compiler_version_info_t version = {
        .major = pDeviceGraphProperties.compilerVersion.major,
        .minor = pDeviceGraphProperties.compilerVersion.minor};

    uint64_t xml_len = testModelXml.size();
    uint64_t bin_len = testModelBin.size();
    uint32_t numInputs = 2;
    uint64_t modelSize =
        sizeof(version) + sizeof(numInputs) + sizeof(xml_len) + xml_len + sizeof(bin_len) + bin_len;

    testModelIR.resize(modelSize);

    uint64_t offset = 0;
    memcpy(&testModelIR[0], &version, sizeof(version));
    offset += sizeof(version);

    memcpy(&testModelIR[offset], &numInputs, sizeof(numInputs));
    offset += sizeof(numInputs);

    memcpy(&testModelIR[offset], &xml_len, sizeof(xml_len));
    offset += sizeof(xml_len);

    memcpy(&testModelIR[offset], testModelXml.data(), xml_len);
    offset += xml_len;

    memcpy(&testModelIR[offset], &bin_len, sizeof(bin_len));
    offset += sizeof(bin_len);

    memcpy(&testModelIR[offset], testModelBin.data(), bin_len);

    graphDesc.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES;
    graphDesc.pNext = nullptr;
    graphDesc.format = ZE_GRAPH_FORMAT_NGRAPH_LITE;
    graphDesc.inputSize = testModelIR.size();
    graphDesc.pInput = testModelIR.data();
    graphDesc.pBuildFlags = modelBuildFlags.data();
    graphDesc.flags = ZE_GRAPH_FLAG_NONE;
}

bool UmdTest::isHwsModeEnabled() {
    std::vector<char> out;
    if (DataHandle::loadFile("/sys/module/intel_vpu/parameters/sched_mode", out) != 0)
        return false;
    return out.size() > 0 && out[0] == '1';
}

TEST(Umd, ZeDevTypeStr) {
    Environment *testEnv = Environment::getInstance();
    EXPECT_NE(testEnv->zeDevTypeStr((ze_device_type_t)0), nullptr);
    EXPECT_NE(testEnv->zeDevTypeStr((ze_device_type_t)0), nullptr);
    EXPECT_NE(testEnv->zeDevTypeStr((ze_device_type_t)-1), nullptr);
    EXPECT_NE(testEnv->zeDevTypeStr((ze_device_type_t)(ZE_DEVICE_TYPE_MCA + 1)), nullptr);
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
