/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

#include <climits>

class GraphNativeBase : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();

        if (!Environment::getConfiguration("graph_execution").size())
            GTEST_SKIP() << "No data to perform the test";

        const YAML::Node node = Environment::getConfiguration("graph_execution")[0];

        ASSERT_GT(node["path"].as<std::string>().size(), 0);
        std::filesystem::path path = node["path"].as<std::string>();

        if (path.extension() == ".blob") {
            ASSERT_TRUE(
                loadBlobFromPath(globalConfig.blobDir + node["path"].as<std::string>(), npuBlob));

            graphDesc = {.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES,
                         .pNext = nullptr,
                         .format = ZE_GRAPH_FORMAT_NATIVE,
                         .inputSize = npuBlob.size(),
                         .pInput = reinterpret_cast<uint8_t *>(npuBlob.data()),
                         .pBuildFlags = nullptr,
                         .flags = ZE_GRAPH_FLAG_NONE};

        } else {
            ASSERT_GT(node["flags"].as<std::string>().size(), 0);

            buildFlags = getFlagsFromString(node["flags"].as<std::string>());
            createGraphDescriptorForModel(globalConfig.modelDir + node["path"].as<std::string>(),
                                          buildFlags,
                                          modelIR,
                                          graphDesc);
        }
    }

    void TearDown() override { UmdTest::TearDown(); }

    ze_graph_handle_t graphHandle = nullptr;
    ze_result_t ret = ZE_RESULT_SUCCESS;
    size_t size = 0;

    std::vector<char> npuBlob;

    ze_graph_desc_2_t graphDesc = {};
    std::vector<uint8_t> modelIR = {};
    std::vector<char> buildFlags = {};
};

TEST_F(GraphNativeBase, CreatingGraphWithNullptrInputGraph) {
    graphDesc.pInput = nullptr;
    EXPECT_EQ(zeGraphDDITableExt->pfnCreate2(zeContext, zeDevice, &graphDesc, &graphHandle),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);
}

TEST_F(GraphNativeBase, CreatingGraphWithNullPtrDesc) {
    const ze_graph_desc_2_t *graphDesc = nullptr;
    EXPECT_EQ(zeGraphDDITableExt->pfnCreate2(zeContext, zeDevice, graphDesc, &graphHandle),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);
}

TEST_F(GraphNativeBase, GetProfilingDataPropertiesExpectSuccess) {
    ze_device_profiling_data_properties_t pDeviceProfilingDataProperties;
    EXPECT_EQ(zeGraphProfilingDDITableExt->pfnDeviceGetProfilingDataProperties(
                  zeDevice,
                  &pDeviceProfilingDataProperties),
              ZE_RESULT_SUCCESS);
}

class GraphNativeBinary : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();

        if (!Environment::getConfiguration("graph_execution").size())
            GTEST_SKIP() << "No data to perform the test";

        const YAML::Node node = Environment::getConfiguration("graph_execution")[0];

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);
    }

    void TearDown() override { UmdTest::TearDown(); }

    size_t size = 0;
    std::shared_ptr<Graph> graph;
};

TEST_F(GraphNativeBinary, GetGraphNativeBinaryWithoutGraphNativeBinaryPointerExpectSuccess) {
    EXPECT_EQ(zeGraphDDITableExt->pfnGetNativeBinary(graph->handle, &size, nullptr),
              ZE_RESULT_SUCCESS);
}

TEST_F(GraphNativeBinary, GetGraphNativeBinaryWithAndWithoutGraphNativeBinaryPointerExpectSuccess) {
    std::vector<uint8_t> graphNativeBinary;
    EXPECT_EQ(zeGraphDDITableExt->pfnGetNativeBinary(graph->handle, &size, nullptr),
              ZE_RESULT_SUCCESS);
    graphNativeBinary.resize(size, 0xAA);

    EXPECT_EQ(
        zeGraphDDITableExt->pfnGetNativeBinary(graph->handle, &size, graphNativeBinary.data()),
        ZE_RESULT_SUCCESS);
}
