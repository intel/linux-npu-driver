/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/ze_graph_ext.h"
#include "level_zero_driver/ext/source/graph/graph.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "level_zero_driver/unit_tests/options.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <api/vpu_nnrt_api_37xx.h>

#include "gtest/gtest.h"
#include "vpux_driver_compiler.h"

#include <vector>

namespace L0 {
namespace ult {

struct CompilerInDriverFixture : public ContextFixture {
    void SetUp() override {
        ContextFixture::SetUp();

        ASSERT_FALSE(TestOptions::modelPath.empty()) << "Model path has not been provided";
        ASSERT_FALSE(TestOptions::modelFlags.empty()) << "Model config has not been provided";

        ze_device_graph_properties_t pDeviceGraphProperties = {};
        EXPECT_EQ(L0::Graph::getDeviceGraphProperties(device, &pDeviceGraphProperties),
                  ZE_RESULT_SUCCESS);

        ze_device_graph_properties_2_t pDeviceGraphProperties2 = {};
        EXPECT_EQ(L0::Graph::getDeviceGraphProperties2(device, &pDeviceGraphProperties2),
                  ZE_RESULT_SUCCESS);

        EXPECT_EQ(pDeviceGraphProperties2.runtimeVersion.major, VPU_NNRT_37XX_API_VER_MAJOR);
        EXPECT_EQ(pDeviceGraphProperties2.runtimeVersion.minor, VPU_NNRT_37XX_API_VER_MINOR);

        EXPECT_EQ(pDeviceGraphProperties2.elfVersion.major, 1);
        EXPECT_EQ(pDeviceGraphProperties2.elfVersion.minor, 2);

        if (!(pDeviceGraphProperties.graphFormatsSupported & ZE_GRAPH_FORMAT_NGRAPH_LITE))
            GTEST_SKIP_("Compiler in driver is not loaded!");

        createModelData();
    }

    void TearDown() override {
        if (hGraph != nullptr) {
            L0::Graph::fromHandle(hGraph)->destroy();
        }

        ContextFixture::TearDown();
    }

    void appendFileToModel(const std::string &path, std::vector<uint8_t> &data) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        ASSERT_TRUE(file.is_open()) << "Failed to open file: " << path;
        uint64_t fileSize = static_cast<uint64_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        size_t dataSize = data.size();
        data.resize(dataSize + sizeof(uint64_t));
        memcpy(&data[dataSize], &fileSize, sizeof(uint64_t));

        dataSize = data.size();
        data.resize(dataSize + static_cast<size_t>(fileSize));
        file.read(reinterpret_cast<char *>(&data[dataSize]), static_cast<long>(fileSize));
    }

    void createModelData() {
        ze_device_graph_properties_t pDeviceGraphProperties = {};
        EXPECT_EQ(L0::Graph::getDeviceGraphProperties(device, &pDeviceGraphProperties),
                  ZE_RESULT_SUCCESS);

        ze_graph_compiler_version_info_t version = {
            .major = pDeviceGraphProperties.compilerVersion.major,
            .minor = pDeviceGraphProperties.compilerVersion.minor};

        /*
         * NGraph Lite format used in driver:
         *
         * struct ModelIR {
         *   ze_graph_compiler_version_info_t version;
         *   uint32_t numInputs;
         *   struct Buffer {
         *     uint64_t bufferSize;
         *     char buffer[];
         *   } buffers[];
         * };
         */

        uint32_t numInputs = 2;
        modelIR.resize(sizeof(version) + sizeof(numInputs));

        memcpy(&modelIR[0], &version, sizeof(version));
        memcpy(&modelIR[sizeof(version)], &numInputs, sizeof(numInputs));

        appendFileToModel(TestOptions::modelPath, modelIR);

        auto binaryPath = std::filesystem::path(TestOptions::modelPath).replace_extension(".bin");
        appendFileToModel(binaryPath, modelIR);

        buildFlags = TestOptions::modelFlags;
        graphDesc = {.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES,
                     .pNext = nullptr,
                     .format = ZE_GRAPH_FORMAT_NGRAPH_LITE,
                     .inputSize = modelIR.size(),
                     .pInput = modelIR.data(),
                     .pBuildFlags = buildFlags.c_str(),
                     .flags = 0};
    }

    ze_graph_handle_t hGraph = nullptr;
    ze_graph_desc_2_t graphDesc = {};
    std::vector<uint8_t> modelIR;
    std::string buildFlags;
};

using CompilerInDriver = Test<CompilerInDriverFixture>;

TEST_F(CompilerInDriver, versionCheck) {
    ASSERT_GT(Compiler::getCompilerVersionMajor(), 0);
    ASSERT_EQ(Compiler::getCompilerVersionMajor(), VCL_COMPILER_VERSION_MAJOR);
    ASSERT_EQ(Compiler::getCompilerVersionMinor(), VCL_COMPILER_VERSION_MINOR);
    ASSERT_TRUE(Compiler::checkVersion(VCL_COMPILER_VERSION_MAJOR, VCL_COMPILER_VERSION_MINOR));
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithNullInputReturnsFailure) {
    graphDesc.pInput = nullptr;

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, res);
    ASSERT_EQ(nullptr, hGraph);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithInputSizeZeroReturnsFailure) {
    graphDesc.inputSize = 0u;

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_ERROR_INVALID_SIZE, res);
    ASSERT_EQ(nullptr, hGraph);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithNullBuildOptionsReturnsError) {
    graphDesc.pBuildFlags = nullptr;

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_NE(ZE_RESULT_SUCCESS, res);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithNoBuildOptionsReturnsError) {
    graphDesc.pBuildFlags = "";

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_NE(ZE_RESULT_SUCCESS, res);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithBuildOptionsReturnsSuccess) {
    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);
    ASSERT_NE(nullptr, hGraph);

    EXPECT_EQ(L0::Graph::fromHandle(hGraph)->getProfilingOutputSize(), 0);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithPerfCountConfigReturnsSuccess) {
    if (buildFlags.find("--config") == std::string::npos)
        buildFlags += " --config";
    buildFlags += " PERF_COUNT=\"YES\"";
    graphDesc.pBuildFlags = buildFlags.c_str();

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);
    ASSERT_NE(nullptr, hGraph);

    EXPECT_GT(L0::Graph::fromHandle(hGraph)->getProfilingOutputSize(), 0);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithEnableProfilingFlagReturnsSuccess) {
    graphDesc.flags = ZE_GRAPH_FLAG_ENABLE_PROFILING;

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);
    ASSERT_NE(nullptr, hGraph);

    EXPECT_GT(L0::Graph::fromHandle(hGraph)->getProfilingOutputSize(), 0);
}

TEST_F(CompilerInDriver, creatingNgraphLiteWithEnableProfilingFlagAndLogLevelConfigReturnsSuccess) {
    if (buildFlags.find("--config") == std::string::npos)
        buildFlags += " --config";
    buildFlags += " LOG_LEVEL=\"LOG_ERROR\"";
    graphDesc.pBuildFlags = buildFlags.c_str();

    graphDesc.flags = ZE_GRAPH_FLAG_ENABLE_PROFILING;
    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);
    ASSERT_NE(nullptr, hGraph);

    EXPECT_GT(L0::Graph::fromHandle(hGraph)->getProfilingOutputSize(), 0);
}

TEST_F(CompilerInDriver,
       creatingNgraphLiteWithEnableProfilingFlagAndPerfCountConfigReturnsSuccess) {
    if (buildFlags.find("--config") == std::string::npos)
        buildFlags += " --config";
    buildFlags += " PERF_COUNT=\"YES\"";
    graphDesc.pBuildFlags = buildFlags.c_str();

    graphDesc.flags = ZE_GRAPH_FLAG_ENABLE_PROFILING;
    auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
    ASSERT_EQ(ZE_RESULT_SUCCESS, res);
    ASSERT_NE(nullptr, hGraph);

    EXPECT_GT(L0::Graph::fromHandle(hGraph)->getProfilingOutputSize(), 0);
}

} // namespace ult
} // namespace L0
