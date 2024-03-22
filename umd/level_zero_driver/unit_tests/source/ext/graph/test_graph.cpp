/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero/ze_graph_ext.h"
#include "level_zero_driver/ext/source/graph/graph.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "level_zero_driver/unit_tests/options.hpp"
#include "level_zero_driver/unit_tests/utils.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

namespace L0 {
namespace ult {

struct GraphNativeFixture : ContextFixture {
    void SetUp() override {
        ContextFixture::SetUp();

        ASSERT_FALSE(TestOptions::blobPath.empty()) << "Blob path has not been provided";

        loadBlobFromFile(TestOptions::blobPath, blob);
        ASSERT_NE(0u, blob.size());

        graphDesc.pInput = blob.data();
        graphDesc.inputSize = blob.size();

        ze_graph_handle_t hGraph = nullptr;
        ASSERT_EQ(L0::Graph::create(context, device, &graphDesc, &hGraph), ZE_RESULT_SUCCESS);
        ASSERT_NE(hGraph, nullptr);
        graph = L0::Graph::fromHandle(hGraph);
    }

    void TearDown() override {
        if (graph != nullptr) {
            graph->destroy();
        }

        ContextFixture::TearDown();
    }

    L0::Graph *graph = nullptr;
    std::vector<uint8_t> blob;
    ze_graph_desc_2_t graphDesc = {.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES,
                                   .pNext = nullptr,
                                   .format = ZE_GRAPH_FORMAT_NATIVE,
                                   .inputSize = 0,
                                   .pInput = nullptr,
                                   .pBuildFlags = nullptr,
                                   .flags = 0};
};

using GraphTest = Test<DeviceFixture>;
using GraphNativeTest = Test<GraphNativeFixture>;

TEST_F(GraphTest, givenCallToGetDeviceGraphPropertiesExpectedValuesReturned) {
    ze_device_graph_properties_t prop = {};

    EXPECT_EQ(L0::Graph::getDeviceGraphProperties(device, nullptr),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    EXPECT_EQ(L0::Graph::getDeviceGraphProperties(device, &prop), ZE_RESULT_SUCCESS);
    EXPECT_EQ(prop.pNext, nullptr);
    EXPECT_EQ(prop.graphExtensionVersion, ZE_GRAPH_EXT_VERSION_CURRENT);
    EXPECT_TRUE(prop.graphFormatsSupported == ZE_GRAPH_FORMAT_NGRAPH_LITE ||
                prop.graphFormatsSupported == ZE_GRAPH_FORMAT_NATIVE);
}

TEST_F(GraphTest, givenCallToGetProfilingDataPropertiesExpectedValuesReturned) {
    ze_device_profiling_data_properties_t pDeviceProfilingDataProperties;

    EXPECT_EQ(L0::Graph::getProfilingDataProperties(nullptr), ZE_RESULT_ERROR_INVALID_NULL_POINTER);
    EXPECT_EQ(L0::Graph::getProfilingDataProperties(&pDeviceProfilingDataProperties),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(pDeviceProfilingDataProperties.extensionVersion,
              ZE_PROFILING_DATA_EXT_VERSION_CURRENT);
}

TEST_F(GraphNativeTest, whenCreatingGraphFromMalformedBufferInvalidArgumentIsReturned) {
    size_t memSize = 4096u;
    void *data = ctx->createHostMemAlloc(memSize);
    ASSERT_TRUE(data);
    memset(data, 0xfe, memSize);

    graphDesc.inputSize = memSize;
    graphDesc.pInput = (const uint8_t *)data;
    ze_graph_handle_t hGraphNew;

    auto res = L0::Graph::create(context, device, &graphDesc, &hGraphNew);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT, res);

    EXPECT_TRUE(ctx->freeMemAlloc(data));
}

TEST_F(GraphNativeTest, whenCallgetNativeBinaryWithoutSizePointerExpectInvalidNullPointerError) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, graph->getNativeBinary(nullptr, nullptr));
}

TEST_F(GraphNativeTest,
       whenCallgetNativeBinaryWithAndWithoutGraphNativeBinaryPointerExpectSuccess) {
    size_t size = 0;
    std::vector<uint8_t> graphNativeBinary;

    EXPECT_EQ(ZE_RESULT_SUCCESS, graph->getNativeBinary(&size, nullptr));
    EXPECT_EQ(size, blob.size());
    graphNativeBinary.resize(size, 0xAA);

    EXPECT_EQ(ZE_RESULT_SUCCESS, graph->getNativeBinary(&size, graphNativeBinary.data()));
    EXPECT_EQ(graphNativeBinary, blob);
}

TEST_F(GraphNativeTest, whenCallsetArgumentValueWithInvalidArgumentErrorIsReturned) {
    uint8_t argValue[1] = {
        0,
    };

    // Invalid data pointer.
    auto res = graph->setArgumentValue(0, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, res);

    // Invalid argument input index.
    res = graph->setArgumentValue(1, nullptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, res);

    res = graph->setArgumentValue(2, argValue);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT, res);
}

TEST_F(GraphNativeTest, givenCallgetPropertiesSuccessfullyParsesGraphInformation) {
    ze_graph_properties_t graphProp;

    auto res = graph->getProperties(&graphProp);
    EXPECT_EQ(ZE_RESULT_SUCCESS, res);
    EXPECT_EQ(2u, graphProp.numGraphArgs);
}

TEST_F(GraphNativeTest, whenCallgetArgumentPropertiesSuccessIsReturning) {
    ze_graph_properties_t graphProp;
    EXPECT_EQ(graph->getProperties(&graphProp), ZE_RESULT_SUCCESS);

    ze_graph_argument_properties_t prop = {};
    for (uint32_t index = 0; index < graphProp.numGraphArgs; index++) {
        EXPECT_EQ(ZE_RESULT_SUCCESS, graph->getArgumentProperties(index, &prop));
        // Name
        EXPECT_GT(strnlen(prop.name, 50), 0);
        // Type
        if (index == 0) {
            EXPECT_EQ(ZE_GRAPH_ARGUMENT_TYPE_INPUT, prop.type);
        } else if (index == graphProp.numGraphArgs - 1) {
            EXPECT_EQ(ZE_GRAPH_ARGUMENT_TYPE_OUTPUT, prop.type);
        } else if (prop.type != ZE_GRAPH_ARGUMENT_TYPE_INPUT &&
                   prop.type != ZE_GRAPH_ARGUMENT_TYPE_OUTPUT) {
            FAIL() << "Invalid graph argument type";
        }
        // Dimensions
        for (int i = 0; i < ZE_MAX_GRAPH_ARGUMENT_DIMENSIONS_SIZE; i++)
            EXPECT_GT(prop.dims[i], 0u);
        // Network precision
        EXPECT_NE(prop.networkPrecision, ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN);
        EXPECT_NE(prop.networkPrecision, ZE_GRAPH_ARGUMENT_PRECISION_BIN);
        // Network layout
        EXPECT_GE(prop.networkLayout, ZE_GRAPH_ARGUMENT_LAYOUT_ANY);
        // Device precision
        EXPECT_NE(prop.devicePrecision, ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN);
        EXPECT_NE(prop.devicePrecision, ZE_GRAPH_ARGUMENT_PRECISION_BIN);
        // Device layout
        EXPECT_GE(prop.deviceLayout, ZE_GRAPH_ARGUMENT_LAYOUT_ANY);
    }
}

TEST_F(GraphNativeTest, wrongUserInputShouldBeHandled) {
    ze_graph_properties_t props;
    EXPECT_EQ(graph->getProperties(&props), ZE_RESULT_SUCCESS);

    {
        ze_graph_argument_properties_t prop = {};
        EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT,
                  graph->getArgumentProperties(props.numGraphArgs, &prop));

        EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, graph->getArgumentProperties(0, nullptr));
    }

    {
        ze_graph_argument_properties_2_t prop = {};
        EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT,
                  graph->getArgumentProperties2(props.numGraphArgs, &prop));

        EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, graph->getArgumentProperties2(0, nullptr));
    }

    {
        ze_graph_argument_properties_3_t prop = {};
        EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT,
                  graph->getArgumentProperties3(props.numGraphArgs, &prop));

        EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER, graph->getArgumentProperties3(0, nullptr));
    }
}

TEST_F(GraphNativeTest, whenCallgetArgumentProperties2ExpectSuccess) {
    ze_graph_properties_t props = {};
    graph->getProperties(&props);

    ze_graph_argument_properties_2_t prop = {};
    for (uint32_t index = 0; index < props.numGraphArgs; index++) {
        EXPECT_EQ(ZE_RESULT_SUCCESS, graph->getArgumentProperties2(index, &prop));

        if (prop.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
            EXPECT_EQ(prop.quantReverseScale, 1.0f);
            EXPECT_EQ(prop.quantZeroPoint, 0);
        } else {
            EXPECT_EQ(prop.quantReverseScale, 0.f);
            EXPECT_EQ(prop.quantZeroPoint, 0);
        }
    }
}

TEST_F(GraphNativeTest, whenCallgetArgumentProperties3ExpectSuccess) {
    ze_graph_properties_t props;
    graph->getProperties(&props);

    ze_graph_argument_properties_3_t prop = {};
    for (uint32_t index = 0; index < props.numGraphArgs; index++) {
        EXPECT_EQ(ZE_RESULT_SUCCESS, graph->getArgumentProperties3(index, &prop));

        if (prop.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
            EXPECT_EQ(prop.dims_count, 4);
        } else {
            EXPECT_EQ(prop.dims_count, 2);
        }
    }
}

// TODO: Elf create internal buffer object that are detected as memory in ContextFixture::TearDown()
TEST_F(GraphNativeTest, DISABLED_expectThatContextDestroyDestructGraphObject) {
    graph = nullptr;
}

} // namespace ult
} // namespace L0
