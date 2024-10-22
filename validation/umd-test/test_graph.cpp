/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

#include <climits>

class GraphApi : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();

        if (!Environment::getConfiguration("graph_execution").size())
            GTEST_SKIP() << "No data to perform the test";

        const YAML::Node node = Environment::getConfiguration("graph_execution")[0];
        ASSERT_GT(node["path"].as<std::string>().size(), 0);

        graphBuffer = GraphBuffer::get(zeDevice, zeGraphDDITableExt, globalConfig, node);
        ASSERT_NE(graphBuffer, nullptr);
    }

    ze_result_t createCommandList(ze_command_list_handle_t *list) {
        ze_command_list_desc_t cmdListDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                              .pNext = nullptr,
                                              .commandQueueGroupOrdinal = 0,
                                              .flags = 0};

        ze_result_t ret = {};
        auto scopeList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        lists.push_back(std::move(scopeList));
        *list = lists.back().get();
        return ret;
    }

    std::shared_ptr<GraphBuffer> graphBuffer;
    std::vector<zeScope::SharedPtr<ze_command_list_handle_t>> lists;
};

TEST_F(GraphApi, GetProfilingDataPropertiesExpectSuccess) {
    ze_device_profiling_data_properties_t profProp = {};
    profProp.stype = ZE_STRUCTURE_TYPE_DEVICE_PROFILING_DATA_PROPERTIES;
    EXPECT_EQ(zeGraphProfilingDDITableExt->pfnDeviceGetProfilingDataProperties(zeDevice, &profProp),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(profProp.extensionVersion, ZE_PROFILING_DATA_EXT_VERSION_CURRENT);
}

TEST_F(GraphApi, CreateGraphReturnsCorrectError) {
    ze_graph_handle_t handle = nullptr;
    EXPECT_EQ(zeGraphDDITableExt->pfnCreate2(zeContext, zeDevice, nullptr, &handle),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    ze_graph_desc_2_t desc = graphBuffer->desc;
    desc.pInput = nullptr;
    EXPECT_EQ(zeGraphDDITableExt->pfnCreate2(zeContext, zeDevice, &desc, &handle),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    desc = graphBuffer->desc;
    desc.inputSize = 0u;
    EXPECT_EQ(zeGraphDDITableExt->pfnCreate2(zeContext, zeDevice, &desc, &handle),
              ZE_RESULT_ERROR_INVALID_SIZE);
}

TEST_F(GraphApi, GetNativeBinaryUsingMemcpy) {
    auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, graphBuffer);
    ASSERT_NE(graph, nullptr);

    size_t size = 0;
    EXPECT_EQ(zeGraphDDITableExt->pfnGetNativeBinary(graph->handle, &size, nullptr),
              ZE_RESULT_SUCCESS);
    EXPECT_GT(size, 0);

    std::vector<uint8_t> graphNativeBinary(size, 0);
    EXPECT_EQ(
        zeGraphDDITableExt->pfnGetNativeBinary(graph->handle, &size, graphNativeBinary.data()),
        ZE_RESULT_SUCCESS);
}

TEST_F(GraphApi, GetNativeBinaryWithoutMemcpy) {
    auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, graphBuffer);
    ASSERT_NE(graph, nullptr);

    const uint8_t *graphNativeBinary = nullptr;
    size_t size = 0;
    EXPECT_EQ(zeGraphDDITableExt->pfnGetNativeBinary2(graph->handle, &size, &graphNativeBinary),
              ZE_RESULT_SUCCESS);
    EXPECT_GT(size, 0);
    EXPECT_NE(graphNativeBinary, nullptr);
}

TEST_F(GraphApi, AppendGraphInitAndExecuteReturnsCorrectError) {
    auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, graphBuffer);
    ASSERT_NE(graph, nullptr);

    ze_command_list_handle_t list = nullptr;
    ASSERT_EQ(createCommandList(&list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphInitialize(list, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_ERROR_UNINITIALIZED);
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_ERROR_UNINITIALIZED);
}

TEST_F(GraphApi, SetArgumentPropertiesReturnsCorrectError) {
    auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, graphBuffer);
    ASSERT_NE(graph, nullptr);

    ze_graph_properties_t prop = {};
    prop.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;
    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph->handle, &prop), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";

    for (size_t index = 0; index < prop.numGraphArgs; index++) {
        ASSERT_EQ(graph->setArgumentValue(index, nullptr), ZE_RESULT_ERROR_INVALID_NULL_POINTER);
    }

    auto mem = AllocSharedMemory(graph->inputSize[0]);
    ASSERT_EQ(graph->setArgumentValue(prop.numGraphArgs, mem.get()),
              ZE_RESULT_ERROR_INVALID_ARGUMENT);
}

TEST_F(GraphApi, GetArgumentPropertiesReturnsCorrectProperties) {
    auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, graphBuffer);
    ASSERT_NE(graph, nullptr);

    ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties(graph->handle, 0, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    ze_graph_properties_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;

    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";

    ze_graph_argument_properties_t pGraphArgumentProperties = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES;

    for (uint32_t index = 0; index < graphProps.numGraphArgs; index++) {
        ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties(graph->handle,
                                                               index,
                                                               &pGraphArgumentProperties),
                  ZE_RESULT_SUCCESS);
        if (index == 0) {
            ASSERT_EQ(pGraphArgumentProperties.type, ZE_GRAPH_ARGUMENT_TYPE_INPUT);
        } else if (index == graphProps.numGraphArgs - 1) {
            ASSERT_EQ(pGraphArgumentProperties.type, ZE_GRAPH_ARGUMENT_TYPE_OUTPUT);
        } else if (pGraphArgumentProperties.type != ZE_GRAPH_ARGUMENT_TYPE_INPUT &&
                   pGraphArgumentProperties.type != ZE_GRAPH_ARGUMENT_TYPE_OUTPUT) {
            FAIL() << "Invalid graph argument type";
        }

        for (int i = 0; i < ZE_MAX_GRAPH_ARGUMENT_DIMENSIONS_SIZE; i++)
            ASSERT_GT(pGraphArgumentProperties.dims[i], 0u);
        ASSERT_GE(pGraphArgumentProperties.networkLayout, ZE_GRAPH_ARGUMENT_LAYOUT_ANY);
        ASSERT_NE(pGraphArgumentProperties.networkPrecision, ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN);
        ASSERT_NE(pGraphArgumentProperties.networkPrecision, ZE_GRAPH_ARGUMENT_PRECISION_BIN);
        ASSERT_GE(pGraphArgumentProperties.deviceLayout, ZE_GRAPH_ARGUMENT_LAYOUT_ANY);
        ASSERT_NE(pGraphArgumentProperties.devicePrecision, ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN);
        ASSERT_NE(pGraphArgumentProperties.devicePrecision, ZE_GRAPH_ARGUMENT_PRECISION_BIN);
    }

    ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties(graph->handle,
                                                           graphProps.numGraphArgs,
                                                           &pGraphArgumentProperties),
              ZE_RESULT_ERROR_INVALID_ARGUMENT);
}

TEST_F(GraphApi, GetProperties2) {
    auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, graphBuffer);
    ASSERT_NE(graph, nullptr);

    ze_graph_properties_2_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;
    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties2(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";
}
