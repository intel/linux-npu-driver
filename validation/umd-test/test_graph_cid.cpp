/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "model_params.hpp"
#include "umd_test.h"

#include <future>
#include <random>

struct __attribute__((packed)) BmpFormat {
    uint16_t header;
    uint32_t size;
    uint32_t reserve;
    uint32_t offset;
};

class CompilerInDriverBase : public UmdTest {
  protected:
    void SetUp() override {
        UmdTest::SetUp();

        /*Get base configuration from config file*/
        YAML::Node &configuration = Environment::getConfiguration();
        if (configuration["model_dir"].IsDefined())
            modelDir = configuration["model_dir"].as<std::string>();

        if (configuration["image_dir"].IsDefined())
            imageDir = configuration["image_dir"].as<std::string>();
    }

    void createModelData(std::vector<uint8_t> &model,
                         std::vector<char> &xml,
                         std::vector<char> &bin,
                         std::vector<char> &flags,
                         ze_graph_desc_t &desc) {
        ze_device_graph_properties_t pDeviceGraphProperties;
        EXPECT_EQ(
            zeGraphDDITableExt->pfnDeviceGetGraphProperties(zeDevice, &pDeviceGraphProperties),
            ZE_RESULT_SUCCESS);

        ze_graph_compiler_version_info_t version = {
            .major = pDeviceGraphProperties.compilerVersion.major,
            .minor = pDeviceGraphProperties.compilerVersion.minor};

        uint64_t xml_len = xml.size();
        uint64_t bin_len = bin.size();
        uint32_t numInputs = 2;
        uint64_t modelSize = sizeof(version) + sizeof(numInputs) + sizeof(xml_len) + xml_len +
                             sizeof(bin_len) + bin_len;

        model.resize(modelSize);

        uint64_t offset = 0;
        memcpy(&model[0], &version, sizeof(version));
        offset += sizeof(version);

        memcpy(&model[offset], &numInputs, sizeof(numInputs));
        offset += sizeof(numInputs);

        memcpy(&model[offset], &xml_len, sizeof(xml_len));
        offset += sizeof(xml_len);

        memcpy(&model[offset], xml.data(), xml_len);
        offset += xml_len;

        memcpy(&model[offset], &bin_len, sizeof(bin_len));
        offset += sizeof(bin_len);

        memcpy(&model[offset], bin.data(), bin_len);

        desc = {.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES,
                .pNext = nullptr,
                .format = ZE_GRAPH_FORMAT_NGRAPH_LITE,
                .inputSize = model.size(),
                .pInput = model.data(),
                .pBuildFlags = flags.data()};
    }

    std::vector<char> getFlagsFromString(std::string flags) {
        std::vector<char> buildFlags;

        for (auto c : flags)
            buildFlags.push_back(c);
        buildFlags.push_back('\0');
        return buildFlags;
    }

    ze_graph_handle_t graphHandle = nullptr;
    ze_graph_desc_t graphDesc = {};
    std::vector<uint8_t> modelIR = {};
    std::vector<char> modelXml, modelBin, buildFlags;

    std::string modelDir = "";
    std::string imageDir = "";
};

class CompilerInDriver : public CompilerInDriverBase {
  public:
    void SetUp() override {
        CompilerInDriverBase::SetUp();
        auto cfgNodes = Environment::getConfiguration("compiler_in_driver");

        if (cfgNodes.size() == 0)
            SKIP_("Missing models for testing");

        const YAML::Node node = cfgNodes.at(0);

        /* Validate configuration */
        ASSERT_GT(node["path"].as<std::string>().size(), 0);
        ASSERT_GT(node["flags"].as<std::string>().size(), 0);

        /* Setup */
        ASSERT_TRUE(
            getModelFromPath(modelDir + node["path"].as<std::string>(), modelXml, modelBin));

        buildFlags = getFlagsFromString(node["flags"].as<std::string>());

        createModelData(modelIR, modelXml, modelBin, buildFlags, graphDesc);
    }
};

TEST_F(CompilerInDriver, CreatingGraphWithNullptrInputGraphExpectFailure) {
    graphDesc.pInput = nullptr;
    EXPECT_EQ(zeGraphDDITableExt->pfnCreate(zeContext, zeDevice, &graphDesc, &graphHandle),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);
}

TEST_F(CompilerInDriver, CreatingGraphWithZeroGraphSizeExpectFailure) {
    graphDesc.inputSize = 0u;
    EXPECT_EQ(zeGraphDDITableExt->pfnCreate(zeContext, zeDevice, &graphDesc, &graphHandle),
              ZE_RESULT_ERROR_INVALID_SIZE);
}

TEST_F(CompilerInDriver, CreatingGraphCorrectBlobFileAndDescExpectSuccess) {
    ze_result_t ret;
    auto scopedGraphHandle =
        zeScope::graphCreate(zeGraphDDITableExt, zeContext, zeDevice, graphDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);
}

class CompilerInDriverLongT : public CompilerInDriverBase {
  protected:
    enum MemType : uint8_t {
        DEVICE_MEMORY,
        HOST_MEMORY,
        SHARED_MEMORY,
    };

    void SetUp() override {
        CompilerInDriverBase::SetUp();

        ze_result_t ret;
        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();
    }

    void getGraphArgumentSize(ze_graph_handle_t hGraph,
                              std::vector<uint32_t> &inputSize,
                              std::vector<uint32_t> &outputSize) {
        ze_graph_properties_t graphProps{};
        ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(hGraph, &graphProps), ZE_RESULT_SUCCESS);

        for (uint32_t i = 0; i < graphProps.numGraphArgs; i++) {
            ze_graph_argument_properties_t graphArgProps{};

            ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties(hGraph, i, &graphArgProps),
                      ZE_RESULT_SUCCESS);

            size_t size = 1u;
            for (int i = 0; i < ZE_MAX_GRAPH_ARGUMENT_DIMENSIONS_SIZE; i++)
                size *= graphArgProps.dims[i];

            switch (graphArgProps.devicePrecision) {
            case ZE_GRAPH_ARGUMENT_PRECISION_FP32:
            case ZE_GRAPH_ARGUMENT_PRECISION_INT32:
            case ZE_GRAPH_ARGUMENT_PRECISION_UINT32:
                size *= sizeof(uint32_t);
                break;
            case ZE_GRAPH_ARGUMENT_PRECISION_BF16:
            case ZE_GRAPH_ARGUMENT_PRECISION_FP16:
            case ZE_GRAPH_ARGUMENT_PRECISION_INT16:
            case ZE_GRAPH_ARGUMENT_PRECISION_UINT16:
                size *= sizeof(uint16_t);
                break;
            case ZE_GRAPH_ARGUMENT_PRECISION_INT8:
            case ZE_GRAPH_ARGUMENT_PRECISION_UINT8:
                size *= sizeof(uint8_t);
                break;
            case ZE_GRAPH_ARGUMENT_PRECISION_INT4:
            case ZE_GRAPH_ARGUMENT_PRECISION_UINT4:
                size /= 2;
                break;
            default:
                ASSERT_TRUE(false) << "Invalid Graph Argument Precision";
            }

            ASSERT_NE(size, 0u);
            if (graphArgProps.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
                inputSize.push_back(size);
            } else {
                outputSize.push_back(size);
            }

            EXPECT_NE(graphArgProps.name, "");
        }
    }

    bool loadImageData(void *graphInput, size_t graphInputSize, const std::string &imagePath) {
        std::vector<char> bmp;

        if (!UmdTest::loadFile(imagePath, bmp)) {
            PRINTF("Image file %s not found.\n", imagePath.c_str());
            return false;
        }

        auto *bmpHeader = reinterpret_cast<const BmpFormat *>(bmp.data());

        size_t imgSize = bmpHeader->size - bmpHeader->offset;
        if (imgSize > graphInputSize) {
            PRINTF("Image file %s too large.\n", imagePath.c_str());
            return false;
        }

        memcpy(graphInput, (char *)bmpHeader + bmpHeader->offset, imgSize);
        return true;
    }

    bool checkOutputDataset(void *graphOutput,
                            size_t graphOutputSize,
                            uint16_t imageClass,
                            bool verbose = false) {
        std::vector<float> output_data(graphOutputSize / sizeof(float));
        memcpy(output_data.data(), graphOutput, graphOutputSize);

        auto it = std::max_element(output_data.begin(), output_data.end());
        size_t index = std::distance(output_data.begin(), it);

        if (verbose)
            PRINTF("Class = %li, Accuracy = %f\n", index, *it);

        EXPECT_EQ(index, imageClass) << "Class index does not match!" << std::endl
                                     << "Class: " << index << "!=" << imageClass;
        return index == imageClass;
    }

    void allocListOfMemory(std::vector<uint32_t> &listOfMemSize,
                           std::vector<void *> &listOfMem,
                           MemType memType) {
        for (const auto &m : listOfMemSize) {
            {
                std::lock_guard<std::mutex> lock(memMutex);
                if (memType == DEVICE_MEMORY) {
                    mem.push_back(AllocDeviceMemory(m));
                } else if (memType == HOST_MEMORY) {
                    mem.push_back(AllocHostMemory(m));
                } else {
                    mem.push_back(AllocSharedMemory(m));
                }
                listOfMem.push_back(mem.back().get());
            }
        }
    }

    inline void generateRandomData(std::vector<char> &data, size_t size) {
        std::random_device rd;
        std::uniform_int_distribution<int8_t> dist;

        data.reserve(size);
        for (size_t i = 0; i < size; i++) {
            data.push_back(dist(rd));
        }
    }

    template <class ProfilingData>
    std::vector<ProfilingData> queryProfilingData(ze_graph_profiling_query_handle_t hProfilingQuery,
                                                  ze_graph_profiling_type_t profilingType) {
        uint32_t dataSize = 0u;
        EXPECT_EQ(zeGraphProfilingDDITableExt->pfnProfilingQueryGetData(hProfilingQuery,
                                                                        profilingType,
                                                                        &dataSize,
                                                                        nullptr),
                  ZE_RESULT_SUCCESS);
        EXPECT_GT(dataSize, 0u);

        std::vector<ProfilingData> profilingData(dataSize / sizeof(ProfilingData));
        EXPECT_EQ(zeGraphProfilingDDITableExt->pfnProfilingQueryGetData(
                      hProfilingQuery,
                      profilingType,
                      &dataSize,
                      reinterpret_cast<uint8_t *>(profilingData.data())),
                  ZE_RESULT_SUCCESS);

        return profilingData;
    }

    inline const char *getExecTypeStr(int execType) {
        switch (execType) {
        case ZE_TASK_EXECUTE_NONE:
            return "ZE_TASK_EXECUTE_NONE";
        case ZE_TASK_EXECUTE_DPU:
            return "ZE_TASK_EXECUTE_DPU";
        case ZE_TASK_EXECUTE_SW:
            return "ZE_TASK_EXECUTE_SW";
        case ZE_TASK_EXECUTE_DMA:
            return "ZE_TASK_EXECUTE_DMA";
        default:
            return "";
        }
    }

    inline const char *getStatusStr(int status) {
        switch (status) {
        case ZE_LAYER_STATUS_NOT_RUN:
            return "ZE_LAYER_STATUS_NOT_RUN";
        case ZE_LAYER_STATUS_OPTIMIZED_OUT:
            return "ZE_LAYER_STATUS_OPTIMIZED_OUT";
        case ZE_LAYER_STATUS_EXECUTED:
            return "ZE_LAYER_STATUS_EXECUTED";
        default:
            return "";
        }
    }

    inline void addGraphProfilingOption(std::vector<char> &flags) {
        while (!flags.empty() && flags.back() == '\0')
            flags.pop_back();

        for (char c : " --config PERF_COUNT=\"YES\"\0")
            flags.push_back(c);
        /* Reassign flags pointer, vector was extended data buffer could be changed */
        graphDesc.pBuildFlags = flags.data();
    }

    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;
    std::vector<void *> graphInput, graphOutput;

    ze_command_queue_desc_t cmdQueueDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                         .pNext = nullptr,
                                         .ordinal = 0,
                                         .index = 0,
                                         .flags = 0,
                                         .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                         .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};

    ze_command_list_desc_t cmdListDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                          .pNext = nullptr,
                                          .commandQueueGroupOrdinal = 0,
                                          .flags = 0};

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
    std::vector<std::shared_ptr<void>> mem;
    std::mutex memMutex;
};

class CompilerInDriverLong : public CompilerInDriverLongT,
                             public ::testing::WithParamInterface<YAML::Node> {
  protected:
    void SetUp() override {
        CompilerInDriverLongT::SetUp();

        const YAML::Node node = GetParam();

        /* Validate configuration */
        ASSERT_GT(node["path"].as<std::string>().size(), 0);
        ASSERT_GT(node["flags"].as<std::string>().size(), 0);

        /* Setup */
        ASSERT_TRUE(
            getModelFromPath(modelDir + node["path"].as<std::string>(), modelXml, modelBin));

        buildFlags = getFlagsFromString(node["flags"].as<std::string>());

        createModelData(modelIR, modelXml, modelBin, buildFlags, graphDesc);
    }
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverLong);

INSTANTIATE_TEST_SUITE_P(,
                         CompilerInDriverLong,
                         ::testing::ValuesIn(Environment::getConfiguration("compiler_in_driver")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(CompilerInDriverLong, QueryNetwork) {
    ze_graph_query_network_handle_t hQuery = nullptr;
    ASSERT_EQ(zeGraphDDITableExt->pfnQueryNetworkCreate(zeContext, zeDevice, &graphDesc, &hQuery),
              ZE_RESULT_SUCCESS);

    size_t size = 0;
    EXPECT_EQ(zeGraphDDITableExt->pfnQueryNetworkGetSupportedLayers(hQuery, &size, nullptr),
              ZE_RESULT_SUCCESS);
    EXPECT_GT(size, 0);

    std::vector<char> layers(size, '\0');
    EXPECT_EQ(zeGraphDDITableExt->pfnQueryNetworkGetSupportedLayers(hQuery, &size, layers.data()),
              ZE_RESULT_SUCCESS);

    EXPECT_GT(size, 0);

    ASSERT_EQ(zeGraphDDITableExt->pfnQueryNetworkDestroy(hQuery), ZE_RESULT_SUCCESS);
}

TEST_P(CompilerInDriverLong, CompileModelWithGraphInitAndExecute) {
    ze_result_t ret;
    auto scopedGraphHandle =
        zeScope::graphCreate(zeGraphDDITableExt, zeContext, zeDevice, graphDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);
    graphHandle = scopedGraphHandle.get();

    std::vector<uint32_t> graphInputSize;
    std::vector<uint32_t> graphOutputSize;
    getGraphArgumentSize(graphHandle, graphInputSize, graphOutputSize);
    ASSERT_NE(graphInputSize.size(), 0);
    ASSERT_NE(graphOutputSize.size(), 0);

    ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphInitialize(list, graphHandle, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

    allocListOfMemory(graphInputSize, graphInput, SHARED_MEMORY);
    allocListOfMemory(graphOutputSize, graphOutput, SHARED_MEMORY);

    for (size_t i = 0; i < graphInputSize.size(); ++i) {
        std::vector<char> inputData;
        generateRandomData(inputData, graphInputSize[i]);
        memcpy(graphInput[i], inputData.data(), graphInputSize[i]);
    }

    uint32_t argIndex = 0;
    for (const auto &ptr : graphInput) {
        ASSERT_EQ(zeGraphDDITableExt->pfnSetArgumentValue(graphHandle, argIndex++, ptr),
                  ZE_RESULT_SUCCESS);
    }
    for (const auto &ptr : graphOutput) {
        ASSERT_EQ(zeGraphDDITableExt->pfnSetArgumentValue(graphHandle, argIndex++, ptr),
                  ZE_RESULT_SUCCESS);
    }

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphExecute(list, graphHandle, nullptr, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
}

TEST_P(CompilerInDriverLong, CompileModelWithGraphProfilingAndRunInference) {
    addGraphProfilingOption(buildFlags);
    ze_result_t ret;
    auto scopedGraphHandle =
        zeScope::graphCreate(zeGraphDDITableExt, zeContext, zeDevice, graphDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    graphHandle = scopedGraphHandle.get();

    std::vector<uint32_t> graphInputSize;
    std::vector<uint32_t> graphOutputSize;
    getGraphArgumentSize(graphHandle, graphInputSize, graphOutputSize);
    ASSERT_NE(graphInputSize.size(), 0);
    ASSERT_NE(graphOutputSize.size(), 0);

    ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphInitialize(list, graphHandle, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

    allocListOfMemory(graphInputSize, graphInput, SHARED_MEMORY);
    allocListOfMemory(graphOutputSize, graphOutput, SHARED_MEMORY);

    for (size_t i = 0; i < graphInputSize.size(); ++i) {
        std::vector<char> inputData;
        generateRandomData(inputData, graphInputSize[i]);
        memcpy(graphInput[i], inputData.data(), graphInputSize[i]);
    }

    uint32_t argIndex = 0;
    for (const auto &ptr : graphInput) {
        ASSERT_EQ(zeGraphDDITableExt->pfnSetArgumentValue(graphHandle, argIndex++, ptr),
                  ZE_RESULT_SUCCESS);
    }
    for (const auto &ptr : graphOutput) {
        ASSERT_EQ(zeGraphDDITableExt->pfnSetArgumentValue(graphHandle, argIndex++, ptr),
                  ZE_RESULT_SUCCESS);
    }

    uint32_t poolSize = 1;
    auto scopedProfilingPool =
        zeScope::profilingPoolCreate(zeGraphProfilingDDITableExt, graphHandle, poolSize, ret);

    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ze_graph_profiling_pool_handle_t hProfilingPool = scopedProfilingPool.get();

    uint32_t index = 0;
    auto scopedProfilingQuery =
        zeScope::profilingQueryCreate(zeGraphProfilingDDITableExt, hProfilingPool, index, ret);

    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ze_graph_profiling_query_handle_t hProfilingQuery = scopedProfilingQuery.get();

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graphHandle, hProfilingQuery, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    // TASK LEVEL INFO
    std::vector<ze_profiling_task_info> taskLevelInfo =
        queryProfilingData<ze_profiling_task_info>(hProfilingQuery, ZE_GRAPH_PROFILING_TASK_LEVEL);

    ASSERT_FALSE(taskLevelInfo.empty());

    TRACE("\nTASK LEVEL INFO\n");
    TRACE("-----------------------------------------------\n");
    TRACE("#0\n");
    TRACE("name:             %s\n", taskLevelInfo[0].name);
    TRACE("layer_type:       %s\n", taskLevelInfo[0].layer_type);
    TRACE("exec_type:        %s\n", getExecTypeStr(taskLevelInfo[0].exec_type));
    TRACE("start_time_ns:    %lu\n", taskLevelInfo[0].start_time_ns);
    TRACE("duration_ns:      %lu\n", taskLevelInfo[0].duration_ns);
    TRACE("active_cycles:    %u\n", taskLevelInfo[0].active_cycles);
    TRACE("stall_cycles:     %u\n", taskLevelInfo[0].stall_cycles);
    TRACE("task_id:          %u\n", taskLevelInfo[0].task_id);
    TRACE("parent_layer_id:  %u\n", taskLevelInfo[0].parent_layer_id);
    TRACE("-----------------------------------------------\n");

    // LAYER LEVEL INFO
    std::vector<ze_profiling_layer_info> layerLevelInfo =
        queryProfilingData<ze_profiling_layer_info>(hProfilingQuery,
                                                    ZE_GRAPH_PROFILING_LAYER_LEVEL);

    ASSERT_FALSE(layerLevelInfo.empty());

    TRACE("\n\nLAYER LEVEL INFO\n");
    TRACE("-----------------------------------------------\n");
    TRACE("#0\n");
    TRACE("name:            %s\n", layerLevelInfo[0].name);
    TRACE("layer_type:      %s\n", layerLevelInfo[0].layer_type);
    TRACE("status:          %s\n", getStatusStr(layerLevelInfo[0].status));
    TRACE("start_time_ns:   %lu\n", layerLevelInfo[0].start_time_ns);
    TRACE("duration_ns:     %lu\n", layerLevelInfo[0].duration_ns);
    TRACE("layer_id:        %u\n", layerLevelInfo[0].layer_id);
    TRACE("fused_layer_id:  %lu\n", layerLevelInfo[0].fused_layer_id);
    TRACE("dpu_ns:          %lu\n", layerLevelInfo[0].dpu_ns);
    TRACE("sw_ns:           %lu\n", layerLevelInfo[0].sw_ns);
    TRACE("dma_ns:          %lu\n", layerLevelInfo[0].dma_ns);
    TRACE("-----------------------------------------------\n");
}

class CompilerInDriverLongBmp : public CompilerInDriverLongT,
                                public ::testing::WithParamInterface<YAML::Node> {
  public:
    void SetUp() override {
        CompilerInDriverLongT::SetUp();

        const YAML::Node node = GetParam();

        /* Validate configuration */
        ASSERT_GT(node["path"].as<std::string>().size(), 0);
        ASSERT_GT(node["flags"].as<std::string>().size(), 0);
        ASSERT_EQ(node["output"].as<std::vector<uint16_t>>().size(),
                  node["input"].as<std::vector<std::string>>().size());

        /* Setup */
        ASSERT_TRUE(
            getModelFromPath(modelDir + node["path"].as<std::string>(), modelXml, modelBin));

        buildFlags = getFlagsFromString(node["flags"].as<std::string>());

        createModelData(modelIR, modelXml, modelBin, buildFlags, graphDesc);

        /*Set expected output*/
        imageClass = node["output"].as<std::vector<uint16_t>>();

        /*Create list of images to load*/
        for (auto &image : node["input"].as<std::vector<std::string>>()) {
            testImages.push_back(imageDir + image);
        }
    }

    std::vector<std::string> testImages; // paths to test images
    std::vector<uint16_t> imageClass;    // expected result
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverLongBmp);

INSTANTIATE_TEST_SUITE_P(
    ,
    CompilerInDriverLongBmp,
    ::testing::ValuesIn(Environment::getConfiguration("image_classification_imagenet")),
    [](const testing::TestParamInfo<YAML::Node> &p) { return generateTestNameFromNode(p.param); });

TEST_P(CompilerInDriverLongBmp, CompileModelWithGraphInitAndExecuteThenCheckAccuracy) {
    ze_result_t ret;
    auto scopedGraphHandle =
        zeScope::graphCreate(zeGraphDDITableExt, zeContext, zeDevice, graphDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);
    graphHandle = scopedGraphHandle.get();

    std::vector<uint32_t> graphInputSize;
    std::vector<uint32_t> graphOutputSize;
    getGraphArgumentSize(graphHandle, graphInputSize, graphOutputSize);
    ASSERT_NE(graphInputSize.size(), 0);
    ASSERT_NE(graphOutputSize.size(), 0);

    ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphInitialize(list, graphHandle, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

    allocListOfMemory(graphInputSize, graphInput, SHARED_MEMORY);
    allocListOfMemory(graphOutputSize, graphOutput, SHARED_MEMORY);

    for (uint32_t imageIndex = 0; imageIndex < imageClass.size(); imageIndex++) {
        ASSERT_TRUE(loadImageData(graphInput.at(0), graphInputSize.at(0), testImages[imageIndex]));
        uint32_t argIndex = 0;
        for (const auto &ptr : graphInput) {
            ASSERT_EQ(zeGraphDDITableExt->pfnSetArgumentValue(graphHandle, argIndex++, ptr),
                      ZE_RESULT_SUCCESS);
        }
        for (const auto &ptr : graphOutput) {
            ASSERT_EQ(zeGraphDDITableExt->pfnSetArgumentValue(graphHandle, argIndex++, ptr),
                      ZE_RESULT_SUCCESS);
        }

        ASSERT_EQ(zeGraphDDITableExt
                      ->pfnAppendGraphExecute(list, graphHandle, nullptr, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

        ASSERT_TRUE(checkOutputDataset(graphOutput.at(0),
                                       graphOutputSize.at(0),
                                       imageClass[imageIndex],
                                       true));
        memset(graphOutput.at(0), 0, graphOutputSize.at(0));
        ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    }
}

class CompilerInDriverThreaded : public CompilerInDriverLongT,
                                 public ::testing::WithParamInterface<YAML::Node> {
  protected:
    void SetUp() override {
        CompilerInDriverLongT::SetUp();

        const YAML::Node node = GetParam();

        /* Validate configuration */
        ASSERT_GT(node["path"].as<std::string>().size(), 0);
        ASSERT_GT(node["flags"].as<std::string>().size(), 0);
        ASSERT_EQ(node["output"].as<std::vector<uint16_t>>().size(),
                  node["input"].as<std::vector<std::string>>().size());

        /* Setup */
        threads = node["input"].as<std::vector<std::string>>().size();
        if (node["iterations"].IsDefined())
            iterations = node["iterations"].as<uint32_t>();
        else
            iterations = 1;

        ASSERT_TRUE(
            getModelFromPath(modelDir + node["path"].as<std::string>(), modelXml, modelBin));

        buildFlags = getFlagsFromString(node["flags"].as<std::string>());

        createModelData(modelIR, modelXml, modelBin, buildFlags, graphDesc);

        /*Set expected output*/
        imageClass = node["output"].as<std::vector<uint16_t>>();

        /*Create list of images to load*/
        for (auto &image : node["input"].as<std::vector<std::string>>()) {
            testImages.push_back(imageDir + image);
        }
    }

    uint32_t threads;
    uint32_t iterations;
    std::vector<std::string> testImages; // paths to test images
    std::vector<uint16_t> imageClass;    // expected result
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverThreaded);

INSTANTIATE_TEST_SUITE_P(
    ,
    CompilerInDriverThreaded,
    ::testing::ValuesIn(Environment::getConfiguration("image_classification_imagenet")),
    [](const testing::TestParamInfo<YAML::Node> &p) { return generateTestNameFromNode(p.param); });

TEST_P(CompilerInDriverThreaded, ImageClassificationUsingImagenet) {
    auto runInference = [&](uint16_t imageClass, std::string testImagePath) -> void {
        ze_result_t ret;

        auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

        auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        auto list = scopedList.get();
        zeScope::SharedPtr<ze_graph_handle_t> scopedGraphHandle =
            zeScope::graphCreate(zeGraphDDITableExt, zeContext, zeDevice, graphDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

        std::vector<uint32_t> graphInputSize, graphOutputSize;
        getGraphArgumentSize(scopedGraphHandle.get(), graphInputSize, graphOutputSize);
        ASSERT_NE(graphInputSize.size(), 0);
        ASSERT_NE(graphOutputSize.size(), 0);

        std::vector<void *> in, out;
        allocListOfMemory(graphInputSize, in, SHARED_MEMORY);
        allocListOfMemory(graphOutputSize, out, SHARED_MEMORY);

        ASSERT_TRUE(loadImageData(in.at(0), graphInputSize.at(0), testImagePath));

        ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphInitialize(list,
                                                               scopedGraphHandle.get(),
                                                               nullptr,
                                                               0,
                                                               nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(scopedQueue.get(), 1, &list, nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(scopedQueue.get(), graphSyncTimeout),
                  ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

        uint32_t argIndex = 0;
        for (const auto &ptr : in) {
            ASSERT_EQ(
                zeGraphDDITableExt->pfnSetArgumentValue(scopedGraphHandle.get(), argIndex++, ptr),
                ZE_RESULT_SUCCESS);
        }
        for (const auto &ptr : out) {
            ASSERT_EQ(
                zeGraphDDITableExt->pfnSetArgumentValue(scopedGraphHandle.get(), argIndex++, ptr),
                ZE_RESULT_SUCCESS);
        }

        ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphExecute(list,
                                                            scopedGraphHandle.get(),
                                                            nullptr,
                                                            nullptr,
                                                            0,
                                                            nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

        for (uint32_t iteration = 0; iteration < iterations; iteration++) {
            memset(out.at(0), 0, graphOutputSize.at(0));

            ASSERT_EQ(zeCommandQueueExecuteCommandLists(scopedQueue.get(), 1, &list, nullptr),
                      ZE_RESULT_SUCCESS);
            ASSERT_EQ(zeCommandQueueSynchronize(scopedQueue.get(), graphSyncTimeout),
                      ZE_RESULT_SUCCESS);

            ASSERT_TRUE(checkOutputDataset(out.at(0), graphOutputSize.at(0), imageClass))
                << "Failed on iteration: " << iteration;
        }
        ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    }; // end of runInference

    std::vector<std::future<void>> results;
    for (uint32_t i = 0; i < threads; i++)
        results.push_back(
            std::async(std::launch::async, runInference, imageClass[i], testImages[i]));

    for (auto &r : results) {
        r.wait();
    }
}

class CompilerInDriverMultiinference : public CompilerInDriverThreaded {
  public:
    struct localInference {
        std::string modelName;
        ze_graph_desc_t graphDesc = {};
        std::shared_ptr<_ze_graph_handle_t> scopedGraphHandle;
        std::vector<uint8_t> modelIR = {};
        uint32_t time;
        uint32_t targetFps;
        std::vector<std::string> testImages; // paths to test images
        std::vector<uint16_t> imageClass;    // expected result
    };
    std::vector<localInference> testInferences = {};

    void SetUp() override {
        CompilerInDriverLongT::SetUp();

        const YAML::Node modelsSet = GetParam();

        if (modelsSet.size() == 0)
            SKIP_("Missing models for testing");

        for (auto &model : modelsSet) {
            ASSERT_GT(model["path"].as<std::string>().size(), 0);
            ASSERT_GT(model["flags"].as<std::string>().size(), 0);

            localInference inference;

            if (model["target_fps"].IsDefined())
                inference.targetFps = model["target_fps"].as<uint32_t>();
            else
                inference.targetFps = 30;

            if (model["exec_time_in_secs"].IsDefined())
                inference.time = model["exec_time_in_secs"].as<uint32_t>();
            else
                inference.time = 3;

            ASSERT_TRUE(
                getModelFromPath(modelDir + model["path"].as<std::string>(), modelXml, modelBin));
            buildFlags = getFlagsFromString(model["flags"].as<std::string>());

            createModelData(inference.modelIR, modelXml, modelBin, buildFlags, inference.graphDesc);

            if (model["input"].IsDefined() &&
                model["input"].as<std::vector<std::string>>().size()) {
                for (auto &image : model["input"].as<std::vector<std::string>>())
                    inference.testImages.push_back(imageDir + image);
            }
            if (model["output"].IsDefined() && model["output"].as<std::vector<uint16_t>>().size())
                inference.imageClass = model["output"].as<std::vector<uint16_t>>();

            ze_result_t ret;
            inference.scopedGraphHandle = zeScope::graphCreate(zeGraphDDITableExt,
                                                               zeContext,
                                                               zeDevice,
                                                               inference.graphDesc,
                                                               ret);
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
            inference.modelName = model["path"].as<std::string>();
            testInferences.push_back(inference);
        }
    }
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverMultiinference);

INSTANTIATE_TEST_SUITE_P(,
                         CompilerInDriverMultiinference,
                         ::testing::Values(Environment::getConfiguration("multi_inference")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return std::to_string(p.param.size()) + std::string("_Inferences");
                         });

#define BREAK_ON_FAIL(operResult, retStats)    \
    {                                          \
        if (operResult != ZE_RESULT_SUCCESS) { \
            retStats.status = operResult;      \
            return retStats;                   \
        }                                      \
    }

TEST_P(CompilerInDriverMultiinference, ImageClassification) {
    struct inferenceStats {
        std::string model;
        ze_result_t status;
        uint32_t totalFrames = 0;
        uint32_t droppedFrames = 0;
        double realFPS = 0;
        double maxExecTimePerFrame;
        double minExecTimePerFrame;
    };

    auto runInference =
        [&](const CompilerInDriverMultiinference::localInference &inference) -> inferenceStats {
        ze_result_t ret;
        inferenceStats stats = {};

        stats.status = ZE_RESULT_SUCCESS;
        stats.minExecTimePerFrame = DBL_MAX;
        stats.model = inference.modelName;

        auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        BREAK_ON_FAIL(ret, stats);

        auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        BREAK_ON_FAIL(ret, stats);
        auto list = scopedList.get();
        BREAK_ON_FAIL(ret, stats);

        std::vector<uint32_t> inSize, outSize;
        getGraphArgumentSize(inference.scopedGraphHandle.get(), inSize, outSize);
        EXPECT_NE(inSize.size(), 0);
        EXPECT_NE(outSize.size(), 0);

        std::vector<void *> in, out;
        allocListOfMemory(inSize, in, SHARED_MEMORY);
        allocListOfMemory(outSize, out, SHARED_MEMORY);

        if (inference.testImages.size()) {
            EXPECT_TRUE(loadImageData(in.at(0), inSize.at(0), inference.testImages[0]));
        } else {
            for (size_t i = 0; i < inSize.size(); ++i) {
                std::vector<char> inputData;
                generateRandomData(inputData, inSize[i]);
                memcpy(in[i], inputData.data(), inSize[i]);
            }
        }
        ret = zeGraphDDITableExt->pfnAppendGraphInitialize(list,
                                                           inference.scopedGraphHandle.get(),
                                                           nullptr,
                                                           0,
                                                           nullptr);
        BREAK_ON_FAIL(ret, stats);

        ret = zeCommandListClose(list);
        BREAK_ON_FAIL(ret, stats);
        ret = zeCommandQueueExecuteCommandLists(scopedQueue.get(), 1, &list, nullptr);
        BREAK_ON_FAIL(ret, stats);

        ret = zeCommandQueueSynchronize(scopedQueue.get(), graphSyncTimeout);
        BREAK_ON_FAIL(ret, stats);

        ret = zeCommandListReset(list);
        BREAK_ON_FAIL(ret, stats);

        uint32_t argIndex = 0;
        for (const auto &ptr : in) {
            ret = zeGraphDDITableExt->pfnSetArgumentValue(inference.scopedGraphHandle.get(),
                                                          argIndex++,
                                                          ptr);
            BREAK_ON_FAIL(ret, stats);
        }
        for (const auto &ptr : out) {
            ret = zeGraphDDITableExt->pfnSetArgumentValue(inference.scopedGraphHandle.get(),
                                                          argIndex++,
                                                          ptr);
            BREAK_ON_FAIL(ret, stats);
        }

        ret = zeGraphDDITableExt->pfnAppendGraphExecute(list,
                                                        inference.scopedGraphHandle.get(),
                                                        nullptr,
                                                        nullptr,
                                                        0,
                                                        nullptr);
        BREAK_ON_FAIL(ret, stats);

        ret = zeCommandListClose(list);
        BREAK_ON_FAIL(ret, stats);
        auto endInferenceTime =
            std::chrono::steady_clock::now() + std::chrono::seconds(inference.time);
        double summaryInferenceTimeMs = 0;
        auto frameTargetTimeUs = std::chrono::microseconds(1000000 / inference.targetFps);
        auto nextFrameStartPoint = std::chrono::steady_clock::now() + std::chrono::microseconds(5);

        while (std::chrono::steady_clock::now() < endInferenceTime) {
            auto frameBeginIncludingWait = std::chrono::steady_clock::now();
            if (std::chrono::steady_clock::now() > nextFrameStartPoint)
                stats.droppedFrames++;

            while (std::chrono::steady_clock::now() < nextFrameStartPoint)
                std::this_thread::yield();

            auto frameBegin = std::chrono::steady_clock::now();
            ret = zeCommandQueueExecuteCommandLists(scopedQueue.get(), 1, &list, nullptr);
            BREAK_ON_FAIL(ret, stats);
            ret = zeCommandQueueSynchronize(scopedQueue.get(), graphSyncTimeout);
            BREAK_ON_FAIL(ret, stats);

            nextFrameStartPoint = frameBegin + frameTargetTimeUs;

            std::chrono::duration<double, std::milli> durationMs =
                std::chrono::steady_clock::now() - frameBeginIncludingWait;
            summaryInferenceTimeMs += durationMs.count();

            /* calculate min max frame time*/
            durationMs = std::chrono::steady_clock::now() - frameBegin;

            stats.maxExecTimePerFrame = std::max(stats.maxExecTimePerFrame, durationMs.count());
            stats.minExecTimePerFrame = std::min(stats.minExecTimePerFrame, durationMs.count());

            stats.totalFrames++;

            if (inference.imageClass.size()) {
                EXPECT_TRUE(checkOutputDataset(out.at(0), outSize.at(0), inference.imageClass[0]))
                    << "Failed ";
                memset(out.at(0), 0, outSize.at(0));
            }
        }
        stats.realFPS = 1000 * stats.totalFrames / summaryInferenceTimeMs;

        return stats;
    }; // end of runInference

    std::vector<std::future<inferenceStats>> results;
    for (uint32_t i = 0; i < testInferences.size(); i++)
        results.push_back(std::async(std::launch::async, runInference, testInferences[i]));

    for (auto &r : results) {
        r.wait();
        inferenceStats s = r.get();
        PRINTF("----------------------------------------------------\n");
        PRINTF("Model:                %s \n", s.model.c_str());
        if (s.status == ZE_RESULT_SUCCESS)
            PRINTF("Status:               SUCCESS \n");
        else
            PRINTF("Status:               FAIL %d \n", s.status);
        PRINTF("FramesExecuted:       %d \n", s.totalFrames);
        PRINTF("FramesDropped:        %d \n", s.droppedFrames);
        PRINTF("CalculatedFPS:        %f \n", s.realFPS);
        PRINTF("MaxFrameExecTime[ms]: %f \n", s.maxExecTimePerFrame);
        PRINTF("MinFrameExecTime[ms]: %f \n", s.minExecTimePerFrame);
    }
}
