/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

class GraphQueryNetwork : public UmdTest, public ::testing::WithParamInterface<YAML::Node> {
  protected:
    void SetUp() override {
        UmdTest::SetUp();

        const YAML::Node node = GetParam();

        /* Validate configuration */
        ASSERT_GT(node["path"].as<std::string>().size(), 0);
        std::filesystem::path path = node["path"].as<std::string>();
        if (path.extension() == ".blob") {
            SKIP_("The test is not intended for use with a precompiled blob.");
        }

        ASSERT_GT(node["flags"].as<std::string>().size(), 0);

        graphBuffer = GraphBuffer::get(zeDevice, zeGraphDDITableExt, globalConfig, node);
        ASSERT_NE(graphBuffer, nullptr);
    }

    std::shared_ptr<GraphBuffer> graphBuffer;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(GraphQueryNetwork);

INSTANTIATE_TEST_SUITE_P(,
                         GraphQueryNetwork,
                         ::testing::ValuesIn(Environment::getConfiguration("graph_execution")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(GraphQueryNetwork, GetSupportedLayers) {
    ze_graph_query_network_handle_t hQuery = nullptr;

    ASSERT_EQ(zeGraphDDITableExt->pfnQueryNetworkCreate2(zeContext,
                                                         zeDevice,
                                                         &graphBuffer->desc,
                                                         &hQuery),
              ZE_RESULT_SUCCESS);

    size_t size = 0;
    EXPECT_EQ(zeGraphDDITableExt->pfnQueryNetworkGetSupportedLayers(hQuery, &size, nullptr),
              ZE_RESULT_SUCCESS);
    EXPECT_GT(size, 0);

    std::vector<char> layers(size, '\0');
    EXPECT_EQ(zeGraphDDITableExt->pfnQueryNetworkGetSupportedLayers(hQuery, &size, layers.data()),
              ZE_RESULT_SUCCESS);

    EXPECT_GT(layers.size(), 0);
    TRACE("Supported layers: %s\n", layers.data());

    ASSERT_EQ(zeGraphDDITableExt->pfnQueryNetworkDestroy(hQuery), ZE_RESULT_SUCCESS);
}
