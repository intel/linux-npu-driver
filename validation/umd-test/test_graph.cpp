/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "model_params.hpp"
#include "umd_test.h"

#include <climits>

class GraphNative : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();

        if (!Environment::getConfiguration("graph_execution").size())
            GTEST_SKIP() << "Do not find blobs to execute test";

        std::string blobDir = "";
        YAML::Node &configuration = Environment::getConfiguration();
        if (configuration["blob_dir"].IsDefined())
            blobDir = configuration["blob_dir"].as<std::string>();
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

    ze_graph_desc_t graphDesc = {.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES,
                                 .pNext = nullptr,
                                 .format = ZE_GRAPH_FORMAT_NATIVE,
                                 .inputSize = 0,
                                 .pInput = nullptr,
                                 .pBuildFlags = nullptr};

    ze_graph_handle_t graphHandle = nullptr;
    ze_result_t ret;
    size_t size = 0;

    std::vector<std::vector<char>> inputBin, outputBin;
    std::vector<char> vpuBlob, vpuBin;
};

TEST_F(GraphNative, CreatingGraphWithNullptrInputGraph) {
    graphDesc.inputSize = vpuBlob.size();
    graphDesc.pInput = nullptr;
    EXPECT_EQ(zeGraphDDITableExt->pfnCreate(zeContext, zeDevice, &graphDesc, &graphHandle),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);
}

TEST_F(GraphNative, CreatingGraphWithNullPtrDesc) {
    const ze_graph_desc_t *graphDesc = nullptr;
    EXPECT_EQ(zeGraphDDITableExt->pfnCreate(zeContext, zeDevice, graphDesc, &graphHandle),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);
}

TEST_F(GraphNative, CreatingGraphCorrectBlobFileAndDesc) {
    graphDesc.inputSize = vpuBlob.size();
    graphDesc.pInput = reinterpret_cast<uint8_t *>(vpuBlob.data());
    auto scopedGraphHandle =
        zeScope::graphCreate(zeGraphDDITableExt, zeContext, zeDevice, graphDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);
}

TEST_F(GraphNative, GetGraphNativeBinaryWithoutGraphNativeBinaryPointerExpectSuccess) {
    graphDesc.inputSize = vpuBlob.size();
    graphDesc.pInput = reinterpret_cast<uint8_t *>(vpuBlob.data());
    auto scopedGraphHandle =
        zeScope::graphCreate(zeGraphDDITableExt, zeContext, zeDevice, graphDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);
    graphHandle = scopedGraphHandle.get();
    EXPECT_EQ(zeGraphDDITableExt->pfnGetNativeBinary(graphHandle, &size, nullptr),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(size, vpuBlob.size());
}

TEST_F(GraphNative, GetGraphNativeBinaryWithAndWithoutGraphNativeBinaryPointerExpectSuccess) {
    std::vector<uint8_t> graphNativeBinary;
    graphDesc.inputSize = vpuBlob.size();
    graphDesc.pInput = reinterpret_cast<uint8_t *>(vpuBlob.data());
    auto scopedGraphHandle =
        zeScope::graphCreate(zeGraphDDITableExt, zeContext, zeDevice, graphDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);
    graphHandle = scopedGraphHandle.get();

    EXPECT_EQ(zeGraphDDITableExt->pfnGetNativeBinary(graphHandle, &size, nullptr),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(size, vpuBlob.size());
    graphNativeBinary.resize(size, 0xAA);

    EXPECT_EQ(zeGraphDDITableExt->pfnGetNativeBinary(graphHandle, &size, graphNativeBinary.data()),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(size, vpuBlob.size());
    EXPECT_EQ(memcmp(graphNativeBinary.data(), vpuBlob.data(), vpuBlob.size()), 0);
}

TEST_F(GraphNative, GetProfilingDataPropertiesExpectSuccess) {
    ze_device_profiling_data_properties_t pDeviceProfilingDataProperties;
    EXPECT_EQ(zeGraphProfilingDDITableExt->pfnDeviceGetProfilingDataProperties(
                  zeDevice,
                  &pDeviceProfilingDataProperties),
              ZE_RESULT_SUCCESS);
}
