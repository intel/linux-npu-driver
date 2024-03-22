/*
 * Copyright (C) 2022 Intel Corporation
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
            GTEST_SKIP() << "Do not find blobs to execute test";

        /* Tests from this group will be run on first blob taken from configuration */
        const YAML::Node node = Environment::getConfiguration("graph_execution")[0];

        /*Validate configuration*/
        ASSERT_GT(node["path"].as<std::string>().size(), 0);
        ASSERT_GT(node["in"].as<std::vector<std::string>>().size(), 0);
        ASSERT_GT(node["out"].as<std::vector<std::string>>().size(), 0);

        ASSERT_TRUE(getBlobFromPath(blobDir + node["path"].as<std::string>(),
                                    node["in"].as<std::vector<std::string>>(),
                                    node["out"].as<std::vector<std::string>>(),
                                    vpuBlob,
                                    inputBin,
                                    outputBin,
                                    vpuBin));
    }

    void TearDown() override { UmdTest::TearDown(); }

    ze_graph_desc_2_t graphDesc = {.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES,
                                   .pNext = nullptr,
                                   .format = ZE_GRAPH_FORMAT_NATIVE,
                                   .inputSize = 0,
                                   .pInput = nullptr,
                                   .pBuildFlags = nullptr,
                                   .flags = ZE_GRAPH_FLAG_NONE};

    ze_graph_handle_t graphHandle = nullptr;
    ze_result_t ret = ZE_RESULT_SUCCESS;
    size_t size = 0;

    std::vector<std::vector<char>> inputBin, outputBin;
    std::vector<char> vpuBlob, vpuBin;
};

TEST_F(GraphNativeBase, CreatingGraphWithNullptrInputGraph) {
    graphDesc.inputSize = vpuBlob.size();
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
            GTEST_SKIP() << "Do not find blobs to execute test";

        /* CommandGraph test will be run on first blob taken from configuration */
        const YAML::Node node = Environment::getConfiguration("graph_execution")[0];

        /* Validate configuration */
        ASSERT_GT(node["path"].as<std::string>().size(), 0);

        graph = Graph::create(zeContext,
                              zeDevice,
                              zeGraphDDITableExt,
                              blobDir + node["path"].as<std::string>(),
                              node);
    }

    void TearDown() override { UmdTest::TearDown(); }

    size_t size = 0;
    std::shared_ptr<Graph> graph;
};

TEST_F(GraphNativeBinary, GetGraphNativeBinaryWithoutGraphNativeBinaryPointerExpectSuccess) {
    EXPECT_EQ(zeGraphDDITableExt->pfnGetNativeBinary(graph->handle, &size, nullptr),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(size, graph->vpuBlob.size());
}

TEST_F(GraphNativeBinary, GetGraphNativeBinaryWithAndWithoutGraphNativeBinaryPointerExpectSuccess) {
    std::vector<uint8_t> graphNativeBinary;
    EXPECT_EQ(zeGraphDDITableExt->pfnGetNativeBinary(graph->handle, &size, nullptr),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(size, graph->vpuBlob.size());
    graphNativeBinary.resize(size, 0xAA);

    EXPECT_EQ(
        zeGraphDDITableExt->pfnGetNativeBinary(graph->handle, &size, graphNativeBinary.data()),
        ZE_RESULT_SUCCESS);

    EXPECT_EQ(memcmp(graphNativeBinary.data(), graph->vpuBlob.data(), graph->vpuBlob.size()), 0);
}
