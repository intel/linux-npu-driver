/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

#include <climits>

class GraphApiBase : public UmdTest {
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

TEST_F(GraphApiBase, GetDeviceGraphProperties) {
    ze_device_graph_properties_t devGraphProps = {};
    devGraphProps.stype = ZE_STRUCTURE_TYPE_DEVICE_GRAPH_PROPERTIES;
    EXPECT_EQ(zeGraphDDITableExt->pfnDeviceGetGraphProperties(zeDevice, &devGraphProps),
              ZE_RESULT_SUCCESS);

    ze_device_graph_properties_2_t devGraphProps2 = {};
    devGraphProps2.stype = ZE_STRUCTURE_TYPE_DEVICE_GRAPH_PROPERTIES_2;
    EXPECT_EQ(zeGraphDDITableExt->pfnDeviceGetGraphProperties2(zeDevice, &devGraphProps2),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(devGraphProps.graphExtensionVersion, devGraphProps2.graphExtensionVersion);
    EXPECT_EQ(devGraphProps.compilerVersion.major, devGraphProps2.compilerVersion.major);
    EXPECT_EQ(devGraphProps.compilerVersion.minor, devGraphProps2.compilerVersion.minor);
    EXPECT_EQ(devGraphProps.graphFormatsSupported, devGraphProps2.graphFormatsSupported);
    EXPECT_EQ(devGraphProps.maxOVOpsetVersionSupported, devGraphProps2.maxOVOpsetVersionSupported);
    EXPECT_GT(devGraphProps2.graphExtensionVersion, 0);
    EXPECT_GT(devGraphProps2.compilerVersion.major, 0);
    EXPECT_GT(devGraphProps2.compilerVersion.minor, 0);
    EXPECT_GT(devGraphProps2.graphFormatsSupported, 0);
    EXPECT_GT(devGraphProps2.elfVersion.major, 0);
    EXPECT_GT(devGraphProps2.runtimeVersion.major, 0);
}

TEST_F(GraphApiBase, GetProfilingDataPropertiesExpectSuccess) {
    ze_device_profiling_data_properties_t profProp = {};
    profProp.stype = ZE_STRUCTURE_TYPE_DEVICE_PROFILING_DATA_PROPERTIES;
    EXPECT_EQ(zeGraphProfilingDDITableExt->pfnDeviceGetProfilingDataProperties(zeDevice, &profProp),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(profProp.extensionVersion, ZE_PROFILING_DATA_EXT_VERSION_CURRENT);
}

TEST_F(GraphApiBase, CreateGraphReturnsCorrectError) {
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

TEST_F(GraphApiBase, GetDriverProperties) {
    size_t size = 0;
    EXPECT_EQ(zeGraphDDITableExt->pfnCompilerGetSupportedOptions(zeDevice,
                                                                 ZE_NPU_DRIVER_OPTIONS,
                                                                 &size,
                                                                 nullptr),
              ZE_RESULT_SUCCESS);

    std::string options;
    options.resize(size, '\0');

    EXPECT_EQ(zeGraphDDITableExt->pfnCompilerGetSupportedOptions(zeDevice,
                                                                 ZE_NPU_DRIVER_OPTIONS,
                                                                 &size,
                                                                 options.data()),
              ZE_RESULT_SUCCESS);
    TRACE("Driver supported options: %s\n", options.c_str());
}

TEST_F(GraphApiBase, GetCompilerProperties) {
    size_t size = 0;
    EXPECT_EQ(zeGraphDDITableExt->pfnCompilerGetSupportedOptions(zeDevice,
                                                                 ZE_NPU_COMPILER_OPTIONS,
                                                                 &size,
                                                                 nullptr),
              ZE_RESULT_SUCCESS);

    std::string options;
    options.resize(size, '\0');

    EXPECT_EQ(zeGraphDDITableExt->pfnCompilerGetSupportedOptions(zeDevice,
                                                                 ZE_NPU_COMPILER_OPTIONS,
                                                                 &size,
                                                                 options.data()),
              ZE_RESULT_SUCCESS);
    TRACE("Compiler supported options: %s\n", options.c_str());
}

TEST_F(GraphApiBase, IsCompilerOptionSupported) {
    const char *option = "PERF_COUNT";
    EXPECT_EQ(zeGraphDDITableExt->pfnCompilerIsOptionSupported(zeDevice,
                                                               ZE_NPU_COMPILER_OPTIONS,
                                                               option,
                                                               nullptr),
              ZE_RESULT_SUCCESS);

    option = "LOG_LEVEL";
    EXPECT_EQ(zeGraphDDITableExt->pfnCompilerIsOptionSupported(zeDevice,
                                                               ZE_NPU_COMPILER_OPTIONS,
                                                               option,
                                                               nullptr),
              ZE_RESULT_SUCCESS);

    /*
     * TODO: The compiler does not support passing value to vclGetCompilerIsOptionSupported in 7.3
    EXPECT_EQ(zeGraphDDITableExt->pfnCompilerIsOptionSupported(zeDevice,
                                                               ZE_NPU_COMPILER_OPTIONS,
                                                               option,
                                                               logLevelValue),
              ZE_RESULT_SUCCESS);
      */

    option = "NON_EXISTING";
    EXPECT_EQ(zeGraphDDITableExt->pfnCompilerIsOptionSupported(zeDevice,
                                                               ZE_NPU_COMPILER_OPTIONS,
                                                               option,
                                                               nullptr),
              ZE_RESULT_ERROR_UNSUPPORTED_FEATURE);
}

class GraphApi : public GraphApiBase {
  protected:
    void SetUp() override {
        GraphApiBase::SetUp();

        if (!Environment::getConfiguration("graph_execution").size())
            GTEST_SKIP() << "No data to perform the test";

        const YAML::Node node = Environment::getConfiguration("graph_execution")[0];
        ASSERT_GT(node["path"].as<std::string>().size(), 0);

        graph =
            Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node, graphBuffer);
        ASSERT_NE(graph, nullptr);
    }

    std::shared_ptr<Graph> graph;
    std::array<std::string, 4> expectedArgNames = {"A", "B", "C", "Y"};
};

TEST_F(GraphApi, GetNativeBinaryUsingMemcpy) {
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
    const uint8_t *graphNativeBinary = nullptr;
    size_t size = 0;
    EXPECT_EQ(zeGraphDDITableExt->pfnGetNativeBinary2(graph->handle, &size, &graphNativeBinary),
              ZE_RESULT_SUCCESS);
    EXPECT_GT(size, 0);
    EXPECT_NE(graphNativeBinary, nullptr);
}

TEST_F(GraphApi, AppendGraphInitAndExecuteReturnsCorrectError) {
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

TEST_F(GraphApi, GetProperties2) {
    ze_graph_properties_2_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES_2;
    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties2(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";
}

TEST_F(GraphApi, GetArgumentProperties) {
    ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties(graph->handle, 0, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    ze_graph_properties_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;

    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";

    ze_graph_argument_properties_t pGraphArgumentProperties = {};
    pGraphArgumentProperties.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES;

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

TEST_F(GraphApi, GetArgumentProperties2) {
    ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties2(graph->handle, 0, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    ze_graph_properties_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;

    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";

    ze_graph_argument_properties_2_t graphArgumentProperties = {};
    graphArgumentProperties.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES_2;

    for (uint32_t index = 0; index < graphProps.numGraphArgs; index++) {
        ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties2(graph->handle,
                                                                index,
                                                                &graphArgumentProperties),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(expectedArgNames[index], graphArgumentProperties.name);
        if (index < graphProps.numGraphArgs - 1) {
            ASSERT_EQ(graphArgumentProperties.type, ZE_GRAPH_ARGUMENT_TYPE_INPUT);
        } else if (index == graphProps.numGraphArgs - 1) {
            ASSERT_EQ(graphArgumentProperties.type, ZE_GRAPH_ARGUMENT_TYPE_OUTPUT);
        } else if (graphArgumentProperties.type != ZE_GRAPH_ARGUMENT_TYPE_INPUT &&
                   graphArgumentProperties.type != ZE_GRAPH_ARGUMENT_TYPE_OUTPUT) {
            FAIL() << "Invalid graph argument type";
        }

        for (int i = 0; i < ZE_MAX_GRAPH_ARGUMENT_DIMENSIONS_SIZE; i++)
            ASSERT_GT(graphArgumentProperties.dims[i], 0u);
        ASSERT_GE(graphArgumentProperties.networkLayout, ZE_GRAPH_ARGUMENT_LAYOUT_ANY);
        ASSERT_NE(graphArgumentProperties.networkPrecision, ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN);
        ASSERT_NE(graphArgumentProperties.networkPrecision, ZE_GRAPH_ARGUMENT_PRECISION_BIN);
        ASSERT_GE(graphArgumentProperties.deviceLayout, ZE_GRAPH_ARGUMENT_LAYOUT_ANY);
        ASSERT_NE(graphArgumentProperties.devicePrecision, ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN);
        ASSERT_NE(graphArgumentProperties.devicePrecision, ZE_GRAPH_ARGUMENT_PRECISION_BIN);
        if (graphArgumentProperties.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
            ASSERT_EQ(graphArgumentProperties.quantReverseScale, 1);
        } else {
            ASSERT_EQ(graphArgumentProperties.quantReverseScale, 0);
        }
        ASSERT_EQ(graphArgumentProperties.quantZeroPoint, 0);
    }

    ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties2(graph->handle,
                                                            graphProps.numGraphArgs,
                                                            &graphArgumentProperties),
              ZE_RESULT_ERROR_INVALID_ARGUMENT);
}

TEST_F(GraphApi, GetArgumentProperties3) {
    ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties3(graph->handle, 0, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    ze_graph_properties_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;

    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";

    ze_graph_argument_properties_3_t graphArgumentProperties = {};
    graphArgumentProperties.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES_3;

    for (uint32_t index = 0; index < graphProps.numGraphArgs; index++) {
        ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties3(graph->handle,
                                                                index,
                                                                &graphArgumentProperties),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(expectedArgNames[index], graphArgumentProperties.name);
        if (index < graphProps.numGraphArgs - 1) {
            ASSERT_EQ(graphArgumentProperties.type, ZE_GRAPH_ARGUMENT_TYPE_INPUT);
        } else if (index == graphProps.numGraphArgs - 1) {
            ASSERT_EQ(graphArgumentProperties.type, ZE_GRAPH_ARGUMENT_TYPE_OUTPUT);
        } else if (graphArgumentProperties.type != ZE_GRAPH_ARGUMENT_TYPE_INPUT &&
                   graphArgumentProperties.type != ZE_GRAPH_ARGUMENT_TYPE_OUTPUT) {
            FAIL() << "Invalid graph argument type";
        }

        for (int i = 0; i < ZE_MAX_GRAPH_ARGUMENT_DIMENSIONS_SIZE; i++)
            ASSERT_GT(graphArgumentProperties.dims[i], 0u);
        ASSERT_GE(graphArgumentProperties.networkLayout, ZE_GRAPH_ARGUMENT_LAYOUT_ANY);
        ASSERT_NE(graphArgumentProperties.networkPrecision, ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN);
        ASSERT_NE(graphArgumentProperties.networkPrecision, ZE_GRAPH_ARGUMENT_PRECISION_BIN);
        ASSERT_GE(graphArgumentProperties.deviceLayout, ZE_GRAPH_ARGUMENT_LAYOUT_ANY);
        ASSERT_NE(graphArgumentProperties.devicePrecision, ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN);
        ASSERT_NE(graphArgumentProperties.devicePrecision, ZE_GRAPH_ARGUMENT_PRECISION_BIN);
        if (graphArgumentProperties.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
            ASSERT_EQ(graphArgumentProperties.quantReverseScale, 1);
        } else {
            ASSERT_EQ(graphArgumentProperties.quantReverseScale, 0);
        }
        ASSERT_EQ(graphArgumentProperties.quantZeroPoint, 0);
        ASSERT_EQ(graphArgumentProperties.dims_count, 1);
        if (graphArgumentProperties.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
            ASSERT_EQ(expectedArgNames[index], graphArgumentProperties.debug_friendly_name);
        } else {
            ASSERT_EQ(expectedArgNames[index] + "/sink_port_0",
                      graphArgumentProperties.debug_friendly_name);
        }
        ASSERT_EQ(graphArgumentProperties.associated_tensor_names_count, 1);
        ASSERT_EQ(expectedArgNames[index], graphArgumentProperties.associated_tensor_names[0]);
    }

    ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties3(graph->handle,
                                                            graphProps.numGraphArgs,
                                                            &graphArgumentProperties),
              ZE_RESULT_ERROR_INVALID_ARGUMENT);
}

TEST_F(GraphApi, GraphEvictDoesNothing) {
    EXPECT_EQ(zeGraphDDITableExt->pfnEvict(graph->handle), ZE_RESULT_SUCCESS);
}
