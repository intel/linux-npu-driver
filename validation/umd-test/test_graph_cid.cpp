/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"
#include "umd_prime_buffers.h"

#include <chrono>
#include <future>
#include <level_zero/ze_api.h>
#include <stdexcept>

class CompilerInDriverBase : public UmdTest {
  protected:
    void SetUp() override { UmdTest::SetUp(); }

    zeScope::SharedPtr<ze_graph_handle_t> scopedGraphHandle = nullptr;
    ze_graph_desc_2_t graphDesc = {};
    std::vector<uint8_t> modelIR = {};
    std::vector<char> buildFlags = {};
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
        buildFlags = getFlagsFromString(node["flags"].as<std::string>());
        createGraphDescriptorForModel(globalConfig.modelDir + node["path"].as<std::string>(),
                                      buildFlags,
                                      modelIR,
                                      graphDesc);
    }
};

TEST_F(CompilerInDriver, CreatingGraphWithNullptrInputGraphExpectFailure) {
    ze_graph_handle_t handle = nullptr;
    graphDesc.pInput = nullptr;
    EXPECT_EQ(zeGraphDDITableExt->pfnCreate2(zeContext, zeDevice, &graphDesc, &handle),
              ZE_RESULT_ERROR_INVALID_NULL_POINTER);
}

TEST_F(CompilerInDriver, CreatingGraphWithZeroGraphSizeExpectFailure) {
    ze_graph_handle_t handle = nullptr;
    graphDesc.inputSize = 0u;
    EXPECT_EQ(zeGraphDDITableExt->pfnCreate2(zeContext, zeDevice, &graphDesc, &handle),
              ZE_RESULT_ERROR_INVALID_SIZE);
}

TEST_F(CompilerInDriver, CreatingGraphCorrectBlobFileAndDescExpectSuccess) {
    ze_result_t ret = ZE_RESULT_SUCCESS;
    scopedGraphHandle =
        zeScope::graphCreate2(zeGraphDDITableExt, zeContext, zeDevice, graphDesc, ret);
    EXPECT_EQ(ret, ZE_RESULT_SUCCESS);
}

class CompilerInDriverLayers : public CompilerInDriver,
                               public ::testing::WithParamInterface<YAML::Node> {
  protected:
    void SetUp() override { CompilerInDriver::SetUp(); }
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverLayers);

INSTANTIATE_TEST_SUITE_P(,
                         CompilerInDriverLayers,
                         ::testing::ValuesIn(Environment::getConfiguration("compiler_in_driver")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(CompilerInDriverLayers, QueryNetworkLayers) {
    ze_graph_query_network_handle_t hQuery = nullptr;

    ASSERT_EQ(zeGraphDDITableExt->pfnQueryNetworkCreate2(zeContext, zeDevice, &graphDesc, &hQuery),
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

class CompilerInDriverLongT : public CompilerInDriverBase {
  protected:
    void SetUp() override {
        CompilerInDriverBase::SetUp();

        ze_result_t ret = ZE_RESULT_SUCCESS;
        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();
    }

    ze_command_queue_desc_t cmdQueueDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
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

    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
};

class CompilerInDriverLong : public CompilerInDriverLongT,
                             public ::testing::WithParamInterface<YAML::Node> {
  protected:
    void SetUp() override {
        CompilerInDriverLongT::SetUp();

        const YAML::Node node = GetParam();

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

        graph->allocateArguments(MemType::SHARED_MEMORY);
        graph->copyInputData();
    }

    std::shared_ptr<Graph> graph;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverLong);

INSTANTIATE_TEST_SUITE_P(,
                         CompilerInDriverLong,
                         ::testing::ValuesIn(Environment::getConfiguration("compiler_in_driver")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(CompilerInDriverLong, CompileModelWithGraphInitAndExecute) {
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
}

class CompilerInDriverWithProfiling : public CompilerInDriverLongT,
                                      public ::testing::WithParamInterface<YAML::Node> {
  protected:
    void SetUp() override {
        CompilerInDriverLongT::SetUp();

        const YAML::Node node = GetParam();
        if (node["graph_profiling"].IsDefined() && node["graph_profiling"].as<bool>() == false) {
            SKIP_("The profiling graph test has been disabled.");
        }

        graph = Graph::create(zeContext,
                              zeDevice,
                              zeGraphDDITableExt,
                              globalConfig,
                              node,
                              ZE_GRAPH_FLAG_ENABLE_PROFILING);

        graph->allocateArguments(MemType::SHARED_MEMORY);
        graph->copyInputData();
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

    std::shared_ptr<Graph> graph;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverWithProfiling);

INSTANTIATE_TEST_SUITE_P(,
                         CompilerInDriverWithProfiling,
                         ::testing::ValuesIn(Environment::getConfiguration("compiler_in_driver")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(CompilerInDriverWithProfiling, CompileModelWithGraphProfilingFlag) {
    ze_result_t ret = ZE_RESULT_SUCCESS;

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

    uint32_t poolSize = 1;
    auto scopedProfilingPool =
        zeScope::profilingPoolCreate(zeGraphProfilingDDITableExt, graph->handle, poolSize, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ze_graph_profiling_pool_handle_t hProfilingPool = scopedProfilingPool.get();

    uint32_t index = 0;
    auto scopedProfilingQuery =
        zeScope::profilingQueryCreate(zeGraphProfilingDDITableExt, hProfilingPool, index, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ze_graph_profiling_query_handle_t hProfilingQuery = scopedProfilingQuery.get();

    ASSERT_EQ(
        zeGraphDDITableExt
            ->pfnAppendGraphExecute(list, graph->handle, hProfilingQuery, nullptr, 0, nullptr),
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

        ASSERT_EQ(node["class_index"].as<std::vector<uint16_t>>().size(),
                  node["input"].as<std::vector<std::string>>().size());

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

        graph->allocateArguments(MemType::SHARED_MEMORY);

        /* Set expected output */
        imageClassIndexes = node["class_index"].as<std::vector<uint16_t>>();

        /* Create list of images to load */
        for (auto &image : node["input"].as<std::vector<std::string>>()) {
            testImages.push_back(globalConfig.imageDir + image);
        }
    }

    std::vector<std::string> testImages;     // paths to test images
    std::vector<uint16_t> imageClassIndexes; // expected result

    std::shared_ptr<Graph> graph;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverLongBmp);

INSTANTIATE_TEST_SUITE_P(
    ,
    CompilerInDriverLongBmp,
    ::testing::ValuesIn(Environment::getConfiguration("image_classification_imagenet")),
    [](const testing::TestParamInfo<YAML::Node> &p) { return generateTestNameFromNode(p.param); });

TEST_P(CompilerInDriverLongBmp, CompileModelWithGraphInitAndExecuteThenCheckAccuracy) {
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

    for (size_t i = 0; i < imageClassIndexes.size(); i++) {
        graph->loadInputData(testImages[i]);

        ASSERT_EQ(zeGraphDDITableExt
                      ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

        graph->checkResults(imageClassIndexes[i]);
        graph->clearOutput();

        ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    }
}

class CompilerInDriverBmpWithPrimeBuffers : public CompilerInDriverLongBmp {
  public:
    void SetUp() override {
        if (!primeHelper.hasDMABufferSupport())
            GTEST_SKIP() << "Missed support or insufficient permissions for"
                         << " dma buffer allocation in the system. Skip test";
        CompilerInDriverLongT::SetUp();

        const YAML::Node node = GetParam();

        ASSERT_EQ(node["class_index"].as<std::vector<uint16_t>>().size(),
                  node["input"].as<std::vector<std::string>>().size());

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

        /* Create list of DMA memory buffers outside driver and use it as network inputs */

        ze_device_mem_alloc_desc_t pDeviceMemAllocDesc = {
            .stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
            .pNext = &primeHelper.externalImportFromFdDesc,
            .flags = 0,
            .ordinal = 0};
        dmaBuffers.resize(graph->inputSize.size(), nullptr);
        uint32_t argIndex = 0;
        uint32_t bufferIndex = 0;
        for (auto &dmaBuffer : dmaBuffers) {
            size_t size = graph->inputSize[bufferIndex++];
            ze_result_t ret;
            int32_t dmaBufferFd = -1;
            ASSERT_TRUE(primeHelper.createDMABuffer(size, dmaBufferFd));
            ASSERT_GE(dmaBufferFd, 0);
            dmaBuffer = primeHelper.mmapDmaBuffer(dmaBufferFd);
            /* Import buffer as device memory */
            primeHelper.externalImportFromFdDesc.fd = dmaBufferFd;
            auto scopedImportedMemory =
                zeScope::memAllocDevice(zeContext, pDeviceMemAllocDesc, size, 0, zeDevice, ret);
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
            ASSERT_NE(scopedImportedMemory.get(), nullptr);
            /* Set as input imported buffer, alocated by device alloc from dma file descriptor
               it should consit image loaded above
             */
            graph->setArgumentValue(argIndex++, scopedImportedMemory.get());
            importedGraphInput.push_back(scopedImportedMemory);
        }

        graph->allocateOutputArguments(MemType::SHARED_MEMORY);

        imageClassIndexes = node["class_index"].as<std::vector<uint16_t>>();

        for (auto &image : node["input"].as<std::vector<std::string>>()) {
            testImages.push_back(globalConfig.imageDir + image);
        }
    }

    void TearDown() override {
        importedGraphInput.clear();
        dmaBuffers.clear();
        UmdTest::TearDown();
    }
    std::vector<void *> dmaBuffers;
    std::vector<std::shared_ptr<void>> importedGraphInput;
    PrimeBufferHelper primeHelper;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverBmpWithPrimeBuffers);

INSTANTIATE_TEST_SUITE_P(
    ,
    CompilerInDriverBmpWithPrimeBuffers,
    ::testing::ValuesIn(Environment::getConfiguration("image_classification_imagenet")),
    [](const testing::TestParamInfo<YAML::Node> &p) { return generateTestNameFromNode(p.param); });

TEST_P(CompilerInDriverBmpWithPrimeBuffers, CompileInitExecuteUsingPrimeBufferInput) {
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

    for (uint32_t i = 0; i < imageClassIndexes.size(); i++) {
        /* Load image to dma buffer created by mmap dma file descriptor */
        Image image(testImages[i]);
        ASSERT_EQ(graph->inputSize.at(0), image.getSizeInBytes());
        memcpy(dmaBuffers[0], image.getPtr(), image.getSizeInBytes());

        ASSERT_EQ(zeGraphDDITableExt
                      ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

        graph->checkResults(imageClassIndexes[i]);
        graph->clearOutput();

        ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    }
}

class CompilerInDriverThreaded : public CompilerInDriverLongT,
                                 public ::testing::WithParamInterface<YAML::Node> {
  protected:
    void SetUp() override {
        CompilerInDriverLongT::SetUp();

        const YAML::Node node = GetParam();

        ASSERT_GT(node["path"].as<std::string>().size(), 0);
        ASSERT_EQ(node["class_index"].as<std::vector<uint16_t>>().size(),
                  node["input"].as<std::vector<std::string>>().size());

        threads = node["input"].as<std::vector<std::string>>().size();
        if (node["iterations"].IsDefined())
            iterations = node["iterations"].as<uint32_t>();
        else
            iterations = 1;

        imageClassIndexes = node["class_index"].as<std::vector<uint16_t>>();

        for (auto &image : node["input"].as<std::vector<std::string>>()) {
            testImages.push_back(globalConfig.imageDir + image);
        }
    }

    uint32_t threads;
    uint32_t iterations;
    std::vector<std::string> testImages;
    std::vector<uint16_t> imageClassIndexes;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverThreaded);

INSTANTIATE_TEST_SUITE_P(
    ,
    CompilerInDriverThreaded,
    ::testing::ValuesIn(Environment::getConfiguration("image_classification_imagenet")),
    [](const testing::TestParamInfo<YAML::Node> &p) { return generateTestNameFromNode(p.param); });

TEST_P(CompilerInDriverThreaded, ImageClassificationUsingImagenet) {
    auto runInference = [&](uint16_t imageClassIndex, std::string testImagePath) -> void {
        const YAML::Node node = GetParam();

        ze_result_t ret = ZE_RESULT_SUCCESS;

        auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        auto queue = scopedQueue.get();

        auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        auto list = scopedList.get();

        std::shared_ptr<Graph> graph =
            Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

        graph->allocateArguments(MemType::SHARED_MEMORY);

        graph->loadInputData(testImagePath);

        ASSERT_EQ(
            zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
            ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeGraphDDITableExt
                      ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

        for (uint32_t iteration = 0; iteration < iterations; iteration++) {
            memset(graph->outArgs.at(0), 0, graph->outputSize.at(0));

            ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr),
                      ZE_RESULT_SUCCESS);
            ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

            graph->checkResults(imageClassIndex);
            graph->clearOutput();
        }
        ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    }; // end of runInference

    std::vector<std::future<void>> results;
    for (uint32_t i = 0; i < threads; i++)
        results.push_back(
            std::async(std::launch::async, runInference, imageClassIndexes[i], testImages[i]));

    for (auto &r : results) {
        r.wait();
    }
}

class CompilerInDriverMultiInference : public CompilerInDriverThreaded {
  public:
    struct localInference {
        std::string modelName;
        std::shared_ptr<Graph> graph;

        uint32_t time;
        uint32_t targetFps;
        std::vector<std::string> testImages;     // paths to test images
        std::vector<uint16_t> imageClassIndexes; // expected result
        ze_command_queue_priority_t priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL;
        size_t delayInUs = 0;
    };

    std::vector<localInference> testInferences = {};

    void SetUp() override {
        CompilerInDriverLongT::SetUp();

        const YAML::Node modelsSet = GetParam()["pipeline"];

        if (modelsSet.size() == 0)
            SKIP_("Missing models for testing");

        for (auto &model : modelsSet) {
            localInference inference;

            if (model["target_fps"].IsDefined())
                inference.targetFps = model["target_fps"].as<uint32_t>();
            else
                inference.targetFps = 30;

            if (model["exec_time_in_secs"].IsDefined())
                inference.time = model["exec_time_in_secs"].as<uint32_t>();
            else
                inference.time = 3;
            if (model["input"].IsDefined() &&
                model["input"].as<std::vector<std::string>>().size()) {
                for (auto &image : model["input"].as<std::vector<std::string>>())
                    inference.testImages.push_back(globalConfig.imageDir + image);
            }
            if (model["class_index"].IsDefined() &&
                model["class_index"].as<std::vector<uint16_t>>().size())
                inference.imageClassIndexes = model["class_index"].as<std::vector<uint16_t>>();

            if (model["priority"].IsDefined() && model["priority"].as<std::string>().size())
                inference.priority = toZePriority(model["priority"].as<std::string>());

            if (model["delay_in_us"].IsDefined())
                inference.delayInUs = model["delay_in_us"].as<size_t>();

            std::shared_ptr<Graph> graph =
                Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, model);

            inference.graph = std::move(graph);

            inference.modelName = model["path"].as<std::string>();
            testInferences.push_back(inference);
        }
    }

    ze_command_queue_priority_t toZePriority(const std::string &p) {
        if (p == "high") {
            return ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_HIGH;
        } else if (p == "low") {
            return ZE_COMMAND_QUEUE_PRIORITY_PRIORITY_LOW;
        } else if (p == "normal") {
            return ZE_COMMAND_QUEUE_PRIORITY_NORMAL;
        }
        throw std::runtime_error("Invalid priority, should be: high, low or normal");
    }
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverMultiInference);

INSTANTIATE_TEST_SUITE_P(,
                         CompilerInDriverMultiInference,
                         ::testing::ValuesIn(Environment::getConfiguration("multi_inference")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return p.param["name"].as<std::string>();
                         });

#define BREAK_ON_FAIL(ret, stats)          \
    if (ret != ZE_RESULT_SUCCESS) {        \
        EXPECT_EQ(ret, ZE_RESULT_SUCCESS); \
        stats.status = ret;                \
        return stats;                      \
    }

TEST_P(CompilerInDriverMultiInference, Pipeline) {
    struct InferenceStats {
        ze_result_t status;
        int totalFrames = 0;
        int droppedFrames = 0;
        double realFPS = 0;
        double minExecTimePerFrame;
        double avgExecTimePerFrame;
        double maxExecTimePerFrame;
    };

    auto runInference =
        [&](const CompilerInDriverMultiInference::localInference &inference) -> InferenceStats {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        InferenceStats stats = {};

        stats.status = ZE_RESULT_SUCCESS;
        stats.minExecTimePerFrame = DBL_MAX;

        auto cmdQueueDescInference = cmdQueueDesc;
        cmdQueueDescInference.priority = inference.priority;
        auto scopedQueue =
            zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDescInference, ret);
        BREAK_ON_FAIL(ret, stats);
        auto queue = scopedQueue.get();

        auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        BREAK_ON_FAIL(ret, stats);
        auto list = scopedList.get();

        inference.graph->allocateArguments(MemType::SHARED_MEMORY);

        if (inference.testImages.size()) {
            inference.graph->loadInputData(inference.testImages[0]);
        } else {
            inference.graph->copyInputData();
        }

        ret = zeGraphDDITableExt->pfnAppendGraphInitialize(list,
                                                           inference.graph->handle,
                                                           nullptr,
                                                           0,
                                                           nullptr);
        BREAK_ON_FAIL(ret, stats);

        ret = zeCommandListClose(list);
        BREAK_ON_FAIL(ret, stats);

        ret = zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr);
        BREAK_ON_FAIL(ret, stats);

        ret = zeCommandQueueSynchronize(queue, graphSyncTimeout);
        BREAK_ON_FAIL(ret, stats);

        ret = zeCommandListReset(list);
        BREAK_ON_FAIL(ret, stats);

        ret = zeGraphDDITableExt->pfnAppendGraphExecute(list,
                                                        inference.graph->handle,
                                                        nullptr,
                                                        nullptr,
                                                        0,
                                                        nullptr);
        BREAK_ON_FAIL(ret, stats);

        ret = zeCommandListClose(list);
        BREAK_ON_FAIL(ret, stats);

        std::this_thread::sleep_for(std::chrono::microseconds(inference.delayInUs));

        auto endInferenceTime =
            std::chrono::steady_clock::now() + std::chrono::seconds(inference.time);
        double summaryInferenceTimeMs = 0;
        auto frameTargetTimeUs = std::chrono::microseconds(1000000 / inference.targetFps);
        auto nextFrameStartPoint = std::chrono::steady_clock::now() + std::chrono::microseconds(5);

        while (std::chrono::steady_clock::now() < endInferenceTime) {
            auto frameBeginIncludingWait = std::chrono::steady_clock::now();
            while (std::chrono::steady_clock::now() < nextFrameStartPoint)
                std::this_thread::yield();

            auto frameBegin = std::chrono::steady_clock::now();
            ret = zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr);
            BREAK_ON_FAIL(ret, stats);

            ret = zeCommandQueueSynchronize(
                queue,
                std::chrono::nanoseconds(std::chrono::seconds(inference.time)).count());
            BREAK_ON_FAIL(ret, stats);

            nextFrameStartPoint = frameBegin + frameTargetTimeUs;

            std::chrono::duration<double, std::milli> durationMs =
                std::chrono::steady_clock::now() - frameBeginIncludingWait;
            summaryInferenceTimeMs += durationMs.count();

            durationMs = std::chrono::steady_clock::now() - frameBegin;
            stats.minExecTimePerFrame = std::min(stats.minExecTimePerFrame, durationMs.count());
            stats.avgExecTimePerFrame += durationMs.count();
            stats.maxExecTimePerFrame = std::max(stats.maxExecTimePerFrame, durationMs.count());

            stats.totalFrames++;

            if (inference.imageClassIndexes.size()) {
                inference.graph->checkResults(inference.imageClassIndexes[0]);
                inference.graph->clearOutput();
            }
        }
        stats.realFPS = 1000 * stats.totalFrames / summaryInferenceTimeMs;
        int targetFrames = inference.targetFps * inference.time;
        stats.droppedFrames = std::max(targetFrames - stats.totalFrames, 0);
        stats.avgExecTimePerFrame /= stats.totalFrames;

        return stats;
    }; // end of runInference

    std::vector<std::future<InferenceStats>> results;
    for (size_t i = 0; i < testInferences.size(); i++)
        results.push_back(std::async(std::launch::async, runInference, testInferences[i]));

    for (size_t i = 0; i < results.size(); i++) {
        InferenceStats stats = results[i].get();

        PRINTF("----------------------------------------------------\n");
        PRINTF("Model:                %s \n", testInferences[i].modelName.c_str());
        if (stats.status == ZE_RESULT_SUCCESS)
            PRINTF("Status:               SUCCESS \n");
        else
            PRINTF("Status:               FAIL (%#x) \n", stats.status);
        PRINTF("FramesExecuted:       %d \n", stats.totalFrames);
        PRINTF("FramesDropped:        %d \n", stats.droppedFrames);
        PRINTF("CalculatedFPS:        %f \n", stats.realFPS);
        PRINTF("MinFrameExecTime[ms]: %f \n", stats.minExecTimePerFrame);
        PRINTF("AvgFrameExecTime[ms]: %f \n", stats.avgExecTimePerFrame);
        PRINTF("MaxFrameExecTime[ms]: %f \n", stats.maxExecTimePerFrame);
    }
}
