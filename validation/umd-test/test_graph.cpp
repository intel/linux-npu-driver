/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

#include <climits>
#include <ze_graph_ext.h>
#ifdef UMD_TESTS_USE_OPENVINO
#include <openvino/op/add.hpp>
#include <openvino/openvino.hpp>
#endif

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
    EXPECT_GT(devGraphProps2.compilerVersion.major + devGraphProps2.compilerVersion.minor, 0);
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

    std::vector<char> options(size);

    EXPECT_EQ(zeGraphDDITableExt->pfnCompilerGetSupportedOptions(zeDevice,
                                                                 ZE_NPU_DRIVER_OPTIONS,
                                                                 &size,
                                                                 options.data()),
              ZE_RESULT_SUCCESS);
    if (!isCounterBasedEventSupported()) {
        ASSERT_STREQ(options.data(), "OPTIMIZED_DYNAMIC_STRIDES");
    } else {
        ASSERT_STREQ(options.data(), "OPTIMIZED_DYNAMIC_STRIDES COUNTER_BASED_EVENTS");
    }
    TRACE("Driver supported options: %s\n", options.data());
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

TEST_F(GraphApiBase, GetCompilerInfo) {
    size_t size = 0;
    EXPECT_EQ(zeDeviceGetCompilerInfo(zeDevice,
                                      ZE_DEVICE_COMPILER_INFO_COMPILER_OPTIONS,
                                      nullptr,
                                      &size,
                                      nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_GT(size, 0);

    std::vector<char> compilerOptions(size);
    EXPECT_EQ(zeDeviceGetCompilerInfo(zeDevice,
                                      ZE_DEVICE_COMPILER_INFO_COMPILER_OPTIONS,
                                      nullptr,
                                      &size,
                                      compilerOptions.data()),
              ZE_RESULT_SUCCESS);
    TRACE("Compiler supported options: %s\n", compilerOptions.data());

    size = 0;
    EXPECT_EQ(zeDeviceGetCompilerInfo(zeDevice,
                                      ZE_DEVICE_COMPILER_INFO_DRIVER_OPTIONS,
                                      nullptr,
                                      &size,
                                      nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_GT(size, 0);

    std::vector<char> driverOptions(size);
    EXPECT_EQ(zeDeviceGetCompilerInfo(zeDevice,
                                      ZE_DEVICE_COMPILER_INFO_DRIVER_OPTIONS,
                                      nullptr,
                                      &size,
                                      driverOptions.data()),
              ZE_RESULT_SUCCESS);
    if (!isCounterBasedEventSupported()) {
        ASSERT_STREQ(driverOptions.data(), "OPTIMIZED_DYNAMIC_STRIDES");
    } else {
        ASSERT_STREQ(driverOptions.data(), "OPTIMIZED_DYNAMIC_STRIDES COUNTER_BASED_EVENTS");
    }
    TRACE("Driver supported options: %s\n", driverOptions.data());
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

    option = "OPTIMIZED_DYNAMIC_STRIDES";
    EXPECT_EQ(zeGraphDDITableExt->pfnCompilerIsOptionSupported(zeDevice,
                                                               ZE_NPU_DRIVER_OPTIONS,
                                                               option,
                                                               nullptr),
              ZE_RESULT_SUCCESS);
}

TEST_F(GraphApiBase, GetRuntimeRequirements) {
    const YAML::Node node = Environment::getConfiguration("graph_execution")[0];
    ASSERT_GT(node["path"].as<std::string>().size(), 0);

    graphBuffer->desc.flags = ZE_GRAPH_FLAG_DISABLE_CACHING;

    auto graph =
        Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node, graphBuffer);
    ASSERT_NE(graph, nullptr);
    ze_runtime_requirements_graph_desc_t graphDesc = {};
    graphDesc.stype = ZE_STRUCTURE_TYPE_RUNTIME_REQUIREMENTS_GRAPH_DESC;
    graphDesc.requirementsSrc = graph->handle;

    size_t size = 0;
    EXPECT_EQ(zeDeviceGetRuntimeRequirements(zeDevice, &graphDesc, &size, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_GT(size, 0);
    TRACE("Runtime requirements size: %zu\n", size);

    std::string requirements(size, '\0');
    EXPECT_EQ(zeDeviceGetRuntimeRequirements(zeDevice, &graphDesc, &size, requirements.data()),
              ZE_RESULT_SUCCESS);
    TRACE("Runtime requirements: %s\n", requirements.c_str());

    ze_validate_runtime_requirements_output_t output = {};
    output.stype = ZE_STRUCTURE_TYPE_RUNTIME_REQUIREMENTS_OUTPUT;

    EXPECT_EQ(zeDeviceValidateRuntimeRequirements(zeDevice, requirements.c_str(), &output),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(output.result, ZE_VALIDATE_RUNTIME_REQUIREMENTS_RESULT_REQUIREMENTS_MET);
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

    for (uint32_t index = 0; index < graphProps.numGraphArgs; index++) {
        ze_graph_argument_properties_2_t graphArgumentProperties = {};
        graphArgumentProperties.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES_2;
        ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties2(graph->handle,
                                                                index,
                                                                &graphArgumentProperties),
                  ZE_RESULT_SUCCESS);
        ASSERT_GT(strlen(graphArgumentProperties.name), 0);
        if (index == 0) {
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

    ze_graph_argument_properties_2_t graphArgumentProperties = {};
    graphArgumentProperties.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES_2;
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

    for (uint32_t index = 0; index < graphProps.numGraphArgs; index++) {
        ze_graph_argument_properties_3_t graphArgumentProperties = {};
        graphArgumentProperties.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES_3;
        ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties3(graph->handle,
                                                                index,
                                                                &graphArgumentProperties),
                  ZE_RESULT_SUCCESS);
        ASSERT_GT(strlen(graphArgumentProperties.name), 0);
        if (index == 0) {
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
        ASSERT_GT(strlen(graphArgumentProperties.debug_friendly_name), 0);
        for (uint32_t i = 0; i < graphArgumentProperties.associated_tensor_names_count; i++) {
            ASSERT_GT(strlen(graphArgumentProperties.associated_tensor_names[i]), 0);
        }
    }

    ze_graph_argument_properties_3_t graphArgumentProperties = {};
    graphArgumentProperties.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES_3;
    ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties3(graph->handle,
                                                            graphProps.numGraphArgs,
                                                            &graphArgumentProperties),
              ZE_RESULT_ERROR_INVALID_ARGUMENT);
}

#ifdef UMD_TESTS_USE_OPENVINO
TEST_F(GraphApi, GetArgumentProperties4AndGetArgumentNames) {
    ov::Shape shape{2, 3, 4, 5, 6, 7};
    auto input = std::make_shared<ov::op::v0::Parameter>(ov::element::f32, shape);
    input->set_friendly_name("input");
    input->get_output_tensor(0).set_names({"input_tensor"});
    auto add = std::make_shared<ov::op::v1::Add>(input, input);
    add->set_friendly_name("add");
    add->get_output_tensor(0).set_names({"add_tensor"});
    auto result = std::make_shared<ov::op::v0::Result>(add);
    result->set_friendly_name("result");
    result->get_output_tensor(0).set_names({"result_tensor"});
    auto model = std::make_shared<ov::Model>(result, ov::ParameterVector{input});

    auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, model);
    ASSERT_NE(graph, nullptr);
    ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties4(graph->handle, 0, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    ze_graph_properties_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;

    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";

    for (uint32_t index = 0; index < graphProps.numGraphArgs; index++) {
        ze_graph_argument_properties_4_t graphArgumentProperties = {};
        graphArgumentProperties.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES_4;
        ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties4(graph->handle,
                                                                index,
                                                                &graphArgumentProperties),
                  ZE_RESULT_SUCCESS);
        if (index == 0) {
            ASSERT_EQ(graphArgumentProperties.type, ZE_GRAPH_ARGUMENT_TYPE_INPUT);
        } else if (index == graphProps.numGraphArgs - 1) {
            ASSERT_EQ(graphArgumentProperties.type, ZE_GRAPH_ARGUMENT_TYPE_OUTPUT);
        } else if (graphArgumentProperties.type != ZE_GRAPH_ARGUMENT_TYPE_INPUT &&
                   graphArgumentProperties.type != ZE_GRAPH_ARGUMENT_TYPE_OUTPUT) {
            FAIL() << "Invalid graph argument type";
        }

        for (size_t i = 0; i < ZE_MAX_GRAPH_ARGUMENT_DIMENSIONS_SIZE_8; i++) {
            if (i < shape.size()) {
                EXPECT_EQ(graphArgumentProperties.dims[i], shape[i]) << " dim " << i;
            } else {
                EXPECT_EQ(graphArgumentProperties.dims[i], 1) << " dim " << i;
            }
        }
        ASSERT_GE(graphArgumentProperties.layout, ZE_GRAPH_ARGUMENT_LAYOUT_ANY);
        ASSERT_EQ(graphArgumentProperties.precision, ZE_GRAPH_ARGUMENT_PRECISION_FP32);

        size_t size = 0;
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeGraphDDITableExt->pfnGetArgumentNames(graph->handle,
                                                          index,
                                                          ZE_GRAPH_ARGUMENT_NAME,
                                                          &size,
                                                          nullptr));
        std::vector<char> name(size, 0);
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeGraphDDITableExt->pfnGetArgumentNames(graph->handle,
                                                          index,
                                                          ZE_GRAPH_ARGUMENT_NAME,
                                                          &size,
                                                          name.data()));
        ASSERT_EQ(name.data(), index == 0 ? input->get_friendly_name() : add->get_friendly_name());
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeGraphDDITableExt->pfnGetArgumentNames(graph->handle,
                                                          index,
                                                          ZE_GRAPH_ARGUMENT_DEBUG_FRIENDLY_NAME,
                                                          &size,
                                                          nullptr));
        std::vector<char> debugName(size, 0);
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeGraphDDITableExt->pfnGetArgumentNames(graph->handle,
                                                          index,
                                                          ZE_GRAPH_ARGUMENT_DEBUG_FRIENDLY_NAME,
                                                          &size,
                                                          debugName.data()));
        ASSERT_EQ(debugName.data(),
                  index == 0 ? input->get_friendly_name() : result->get_friendly_name());
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeGraphDDITableExt->pfnGetArgumentNames(graph->handle,
                                                          index,
                                                          ZE_GRAPH_ARGUMENT_ASSOCIATED_TENSOR_NAMES,
                                                          &size,
                                                          nullptr));
        std::vector<char> tensorNames(size, 0);
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeGraphDDITableExt->pfnGetArgumentNames(graph->handle,
                                                          index,
                                                          ZE_GRAPH_ARGUMENT_ASSOCIATED_TENSOR_NAMES,
                                                          &size,
                                                          tensorNames.data()));
        ASSERT_EQ(tensorNames.data(),
                  index == 0 ? input->get_output_tensor(0).get_any_name()
                             : add->get_output_tensor(0).get_any_name());
    }

    ze_graph_argument_properties_4_t graphArgumentProperties = {};
    graphArgumentProperties.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES_4;
    ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties4(graph->handle,
                                                            graphProps.numGraphArgs,
                                                            &graphArgumentProperties),
              ZE_RESULT_ERROR_INVALID_ARGUMENT);
}
#endif

TEST_F(GraphApi, GetArgumentMetadata) {
    ASSERT_EQ(zeGraphDDITableExt->pfnGraphGetArgumentMetadata(graph->handle, 0, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    ze_graph_properties_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;

    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";

    for (uint32_t index = 0; index < graphProps.numGraphArgs; index++) {
        ze_graph_argument_metadata_t metadata = {};
        metadata.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_METADATA;
        ASSERT_EQ(zeGraphDDITableExt->pfnGraphGetArgumentMetadata(graph->handle, index, &metadata),
                  ZE_RESULT_SUCCESS);
        if (index == 0) {
            ASSERT_EQ(metadata.type, ZE_GRAPH_ARGUMENT_TYPE_INPUT);
        } else if (index == graphProps.numGraphArgs - 1) {
            ASSERT_EQ(metadata.type, ZE_GRAPH_ARGUMENT_TYPE_OUTPUT);
        } else if (metadata.type != ZE_GRAPH_ARGUMENT_TYPE_INPUT &&
                   metadata.type != ZE_GRAPH_ARGUMENT_TYPE_OUTPUT) {
            FAIL() << "Invalid graph argument type";
        }

        ASSERT_GT(strlen(metadata.friendly_name), 0);
        ASSERT_LT(metadata.shape_size, ZE_MAX_GRAPH_TENSOR_REF_DIMS);
        ASSERT_LT(metadata.tensor_names_count, ZE_MAX_GRAPH_TENSOR_NAMES_SIZE);
        ASSERT_GT(strlen(metadata.input_name), 0);
    }

    ze_graph_argument_metadata_t metadata = {};
    metadata.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_METADATA;
    ASSERT_EQ(zeGraphDDITableExt->pfnGraphGetArgumentMetadata(graph->handle,
                                                              graphProps.numGraphArgs,
                                                              &metadata),
              ZE_RESULT_ERROR_INVALID_ARGUMENT);
}

TEST_F(GraphApi, GraphEvictDoesNothing) {
    EXPECT_EQ(zeGraphDDITableExt->pfnEvict(graph->handle), ZE_RESULT_SUCCESS);
}
