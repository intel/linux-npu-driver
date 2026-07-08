/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "device_fixture.hpp"
#include "level_zero_driver/source/ext/graph.hpp"
#include "level_zero_driver/unit_tests/options.hpp"

#include <api/vpu_nnrt_api_37xx.h>
#include <fstream>

namespace L0 {
namespace ult {

struct GraphNGraphLiteFixture : public ContextFixture {
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
        EXPECT_EQ(pDeviceGraphProperties2.elfVersion.minor, 3);

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
        int64_t fileSize = file.tellg();
        ASSERT_GE(fileSize, 0) << "File is empty: " << path;
        file.seekg(0, std::ios::beg);

        size_t dataSize = data.size();
        data.resize(dataSize + sizeof(uint64_t));
        memcpy(&data[dataSize], &fileSize, sizeof(uint64_t));

        dataSize = data.size();
        data.resize(dataSize + static_cast<size_t>(fileSize));
        file.read(reinterpret_cast<char *>(&data[dataSize]), static_cast<long>(fileSize));
        ASSERT_EQ(file.gcount(), fileSize) << "Failed to read entire file: " << path;
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

        auto binaryPath =
            std::move(std::filesystem::path(TestOptions::modelPath).replace_extension(".bin"));
        appendFileToModel(std::move(binaryPath), modelIR);

        buildFlags = TestOptions::modelFlags;
        graphDesc = {.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_2,
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

} // namespace ult
} // namespace L0
