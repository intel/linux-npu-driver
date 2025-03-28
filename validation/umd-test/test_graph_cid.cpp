/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "frame_counter.hpp"
#include "graph_utilities.hpp"
#include "umd_dma_heap_system.hpp"

#include <chrono>
#include <future>
#include <level_zero/ze_api.h>
#include <stdexcept>

class CompilerInDriverLongT : public UmdTest {
  protected:
    void SetUp() override {
        UmdTest::SetUp();

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
                         ::testing::ValuesIn(Environment::getConfiguration("graph_execution")),
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

    graph->checkResults();
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

        // TODO: an option with or without profiling (this should apply to blob as well)
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
                         ::testing::ValuesIn(Environment::getConfiguration("graph_execution")),
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

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

        graph->allocateArguments(MemType::SHARED_MEMORY);
        graph->copyInputData();
    }

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

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}

class CompilerInDriverBmpUsingDmaHeap : public CompilerInDriverLongBmp {
  public:
    void SetUp() override {
        CHECK_DMA_HEAP_SUPPORT(dmaHeapSystem);

        CompilerInDriverLongT::SetUp();

        const YAML::Node node = GetParam();

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

        if (graph->images.empty()) {
            GTEST_FAIL() << "No image provided. Check the correctness of the fields used in the "
                            "configuration file.";
        }

        /* Create list of DMA memory buffers outside driver and use it as network inputs */
        ze_external_memory_import_fd_t externalImportFromFdDesc = {
            .stype = ZE_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMPORT_FD,
            .pNext = nullptr,
            .flags = ZE_EXTERNAL_MEMORY_TYPE_FLAG_DMA_BUF,
            .fd = -1};

        ze_device_mem_alloc_desc_t pDeviceMemAllocDesc = {
            .stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
            .pNext = &externalImportFromFdDesc,
            .flags = 0,
            .ordinal = 0};

        dmaBuffers.resize(graph->inputSize.size());

        uint32_t argIndex = 0;
        uint32_t bufferIndex = 0;
        for (auto &dmaBuffer : dmaBuffers) {
            size_t size = graph->inputSize[bufferIndex++];
            ze_result_t ret;

            dmaBuffer = dmaHeapSystem.allocDmaHeapBuffer(size);
            ASSERT_TRUE(dmaBuffer);

            /* Import buffer as device memory */
            externalImportFromFdDesc.fd = dmaBuffer->fd;
            auto scopedImportedMemory =
                zeScope::memAllocDevice(zeContext, pDeviceMemAllocDesc, size, 0, zeDevice, ret);
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
            /* Set as input imported buffer, alocated by device alloc from dma file descriptor
               it should consit image loaded above
             */
            graph->setArgumentValue(argIndex++, scopedImportedMemory.get());
            importedGraphInput.push_back(std::move(scopedImportedMemory));
        }

        graph->allocateOutputArguments(MemType::SHARED_MEMORY);
    }

    void TearDown() override {
        importedGraphInput.clear();
        dmaBuffers.clear();
        UmdTest::TearDown();
    }

    std::vector<std::unique_ptr<DmaHeapBuffer>> dmaBuffers;
    std::vector<std::shared_ptr<void>> importedGraphInput;
    DmaHeapSystem dmaHeapSystem;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverBmpUsingDmaHeap);

INSTANTIATE_TEST_SUITE_P(
    ,
    CompilerInDriverBmpUsingDmaHeap,
    ::testing::ValuesIn(Environment::getConfiguration("image_classification_imagenet")),
    [](const testing::TestParamInfo<YAML::Node> &p) { return generateTestNameFromNode(p.param); });

TEST_P(CompilerInDriverBmpUsingDmaHeap, CompileInitExecuteUsingPrimeBufferInput) {
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

    std::vector<void *> inputPointers;
    for (auto &dmaBuffer : dmaBuffers)
        inputPointers.emplace_back(dmaBuffer->ptr);
    graph->copyInputData(inputPointers);

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}

#define BREAK_ON_FAIL(ret, stats)          \
    if (ret != ZE_RESULT_SUCCESS) {        \
        EXPECT_EQ(ret, ZE_RESULT_SUCCESS); \
        stats.status = ret;                \
        stats.counter.stopTimer();         \
        return stats;                      \
    }

class CompilerInDriverMultiInference : public CompilerInDriverLongT,
                                       public ::testing::WithParamInterface<YAML::Node> {
  public:
    struct LocalInference {
        LocalInference(const YAML::Node &node)
            : model(node){};
        const YAML::Node model;
        std::string modelName;
        std::shared_ptr<Graph> graph;

        uint32_t execTimeSec = 0;
        uint32_t targetFps = 0;
        double fpsDeviation = 0;
        ze_command_queue_priority_t priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL;
        ze_command_queue_workload_type_t workloadType = ZE_WORKLOAD_TYPE_FORCE_UINT32;
        size_t delayInUs = 0;
        size_t parallelReqs = 0;
    };

    struct InferenceStats {
        ze_result_t status = ZE_RESULT_SUCCESS;
        FrameCounter counter;
    };

    std::vector<LocalInference> testInferences = {};

    void SetUp() override {
        CompilerInDriverLongT::SetUp();

        const YAML::Node modelsSet = GetParam()["pipeline"];

        if (modelsSet.size() == 0)
            SKIP_("Missing models for testing");

        for (auto &model : modelsSet) {
            LocalInference inference(model);

            inference.targetFps = model["target_fps"].as<uint32_t>(30);
            inference.execTimeSec = model["exec_time_in_secs"].as<uint32_t>(3);
            inference.delayInUs = model["delay_in_us"].as<size_t>(0);
            inference.fpsDeviation = model["fps_deviation"].as<double>(0);
            inference.parallelReqs = model["parallel_reqs"].as<size_t>(1);
            ASSERT_GT(inference.parallelReqs, 0) << "parallel_reqs field has to be greater than 0";

            if (model["priority"].IsDefined() && model["priority"].as<std::string>().size())
                inference.priority = toZePriority(model["priority"].as<std::string>());

            if (model["workload_type"].IsDefined() &&
                model["workload_type"].as<std::string>().size()) {
                const auto &workloadType = model["workload_type"].as<std::string>();
                if (workloadType == "default") {
                    inference.workloadType = ZE_WORKLOAD_TYPE_DEFAULT;
                } else if (workloadType == "background") {
                    inference.workloadType = ZE_WORKLOAD_TYPE_BACKGROUND;
                } else {
                    FAIL() << "Unsupported workload type in configuration";
                }
            }

            std::shared_ptr<Graph> graph =
                Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, model);

            inference.graph = std::move(graph);
            inference.modelName = model["path"].as<std::string>();
            testInferences.push_back(std::move(inference));
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

    InferenceStats runInference(const CompilerInDriverMultiInference::LocalInference &inference) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        InferenceStats stats = {};
        ze_command_queue_workload_type_t workloadType = inference.workloadType;

        auto cmdQueueDescInference = cmdQueueDesc;
        cmdQueueDescInference.priority = inference.priority;
        auto scopedQueue =
            zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDescInference, ret);
        BREAK_ON_FAIL(ret, stats);
        auto queue = scopedQueue.get();

        inference.graph->allocateArguments(MemType::SHARED_MEMORY);
        inference.graph->copyInputData();
        ret = zeGraphDDITableExt->pfnGraphInitialize(inference.graph->handle);
        BREAK_ON_FAIL(ret, stats);

        std::vector<std::unique_ptr<InferenceRequest>> inferReqs;
        for (size_t i = 0; i < inference.parallelReqs; i++) {
            if (i > 0) {
                inference.graph->allocateArguments(MemType::SHARED_MEMORY);
                inference.graph->copyInputData();
            }
            inferReqs.push_back(inference.graph->newInferRequestUsingQueue(queue));
        }

        /*
         * For the case when config request workload type all inferences are started
         * as background, the requested workload will be set after first inference loop
         */
        if (workloadType != ZE_WORKLOAD_TYPE_FORCE_UINT32) {
            ret = zeCommandQueueDDITableExt->pfnSetWorkloadType(queue, ZE_WORKLOAD_TYPE_BACKGROUND);
            BREAK_ON_FAIL(ret, stats);

            ret = inferReqs[0]->runAsync();
            BREAK_ON_FAIL(ret, stats);

            ret = inferReqs[0]->wait(UINT64_MAX);
            BREAK_ON_FAIL(ret, stats);
        }

        std::this_thread::sleep_for(std::chrono::microseconds(inference.delayInUs));

        stats.counter.startTimer(inference.execTimeSec, inference.targetFps);
        for (size_t i = 1; i < inferReqs.size(); i++) {
            ret = inferReqs[i]->runAsync();
            BREAK_ON_FAIL(ret, stats);
        }

        size_t inferReqIndex = 0;
        while (!stats.counter.isTimeout()) {
            stats.counter.delayNextFrame();

            ret = inferReqs[inferReqIndex]->runAsync();
            BREAK_ON_FAIL(ret, stats);

            inferReqIndex++;
            inferReqIndex = inferReqIndex >= inferReqs.size() ? 0 : inferReqIndex;

            ret = inferReqs[inferReqIndex]->wait(UINT64_MAX);
            BREAK_ON_FAIL(ret, stats);

            if (workloadType != ZE_WORKLOAD_TYPE_FORCE_UINT32) {
                ret = zeCommandQueueDDITableExt->pfnSetWorkloadType(queue, workloadType);
                BREAK_ON_FAIL(ret, stats);
                workloadType = ZE_WORKLOAD_TYPE_FORCE_UINT32;
            }

            // TODO: Add multiple inference request output validation
            if (inference.parallelReqs == 1 && inference.graph->classIndexes.size()) {
                inference.graph->checkResults();
                inference.graph->clearOutput();
            }

            stats.counter.recordFrame(inferReqs[inferReqIndex]->latencyMs);
        }

        stats.counter.stopTimer();
        return stats;
    };
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverMultiInference);

INSTANTIATE_TEST_SUITE_P(,
                         CompilerInDriverMultiInference,
                         ::testing::ValuesIn(Environment::getConfiguration("multi_inference")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return p.param["name"].as<std::string>();
                         });

TEST_P(CompilerInDriverMultiInference, Pipeline) {
    auto testInference =
        [&](const CompilerInDriverMultiInference::LocalInference &inference) -> InferenceStats {
        return runInference(inference);
    };

    std::vector<std::future<InferenceStats>> results;
    for (size_t i = 0; i < testInferences.size(); i++)
        results.push_back(std::async(std::launch::async, testInference, testInferences[i]));

    std::pair<double, uint32_t> defaultFPSInferencesRate = {0, 0};
    std::pair<double, uint32_t> backgroundFPSInferencesRate = {0, 0};
    for (size_t i = 0; i < results.size(); i++) {
        InferenceStats stats = results[i].get();

        PRINTF("----------------------------------------------------\n");
        PRINTF("Model:                %s\n", testInferences[i].modelName.c_str());
        if (stats.status == ZE_RESULT_SUCCESS)
            PRINTF("Status:               SUCCESS\n");
        else
            PRINTF("Status:               FAIL (%#x)\n", stats.status);
        PRINTF("FramesExecuted:       %lu\n", stats.counter.frameCount);
        PRINTF("FramesDropped:        %lu\n", stats.counter.frameDrops);
        PRINTF("CalculatedFPS:        %f\n", stats.counter.fps);
        PRINTF("ExecutionTime[ms]:    %f\n", stats.counter.totalTimeMs);
        PRINTF("MinFrameExecTime[ms]: %f\n", stats.counter.frameMinMs);
        PRINTF("AvgFrameExecTime[ms]: %f\n", stats.counter.frameAvgMs);
        PRINTF("MaxFrameExecTime[ms]: %f\n", stats.counter.frameMaxMs);

        /*If defined acceptance criteraia for fps rate*/
        if (testInferences[i].fpsDeviation) {
            const double minFPS =
                testInferences[i].targetFps * (1 - testInferences[i].fpsDeviation);
            EXPECT_GE(static_cast<double>(stats.counter.fps), minFPS);
        }

        if (testInferences[i].workloadType == ZE_WORKLOAD_TYPE_DEFAULT) {
            defaultFPSInferencesRate.first += stats.counter.fps;
            defaultFPSInferencesRate.second++;
        }
        if (testInferences[i].workloadType == ZE_WORKLOAD_TYPE_BACKGROUND) {
            backgroundFPSInferencesRate.first += stats.counter.fps;
            backgroundFPSInferencesRate.second++;
        }
    }

    /* For test with dynamic priority change when focus is set on dedicated inference there
       is acceptance criteria that focused inference will be min 30% more effective
     */
    if (isHwsModeEnabled() && defaultFPSInferencesRate.second != 0 &&
        backgroundFPSInferencesRate.second != 0) {
        const double avgRateDefaultInferences =
            defaultFPSInferencesRate.first / defaultFPSInferencesRate.second;
        const double avgRateBackgroundInferences =
            backgroundFPSInferencesRate.first / backgroundFPSInferencesRate.second;
        EXPECT_GE(avgRateDefaultInferences, avgRateBackgroundInferences * 1.30);
    }
}

using ModelCacheTest = CompilerInDriverLongT;

int getNumFiles(const std::filesystem::path &dir) {
    int numFiles = 0;
    for ([[maybe_unused]] const auto &entry : std::filesystem::directory_iterator(dir)) {
        numFiles++;
    }
    return numFiles;
}

std::string getBuildLog(graph_dditable_ext_t *graphDDI) {
    uint32_t size = 0;
    ze_result_t result = graphDDI->pfnBuildLogGetString(nullptr, &size, nullptr);
    if (result != ZE_RESULT_SUCCESS)
        return {};
    std::string log(size - 1, 0);
    result = graphDDI->pfnBuildLogGetString(nullptr, &size, log.data());
    if (result != ZE_RESULT_SUCCESS)
        return {};
    return log;
}

TEST_F(ModelCacheTest, CheckIfModelIsCachedWithBuildLog) {
    const char *home = getenv("HOME");
    ASSERT_NE(home, nullptr);
    std::filesystem::path cacheDir = std::filesystem::path(home) / ".cache/ze_intel_npu_cache";
    std::error_code ec;
    for (const auto &entry : std::filesystem::directory_iterator(cacheDir)) {
        ASSERT_GE(std::filesystem::remove_all(entry, ec), 0);
    }
    ASSERT_EQ(getNumFiles(cacheDir), 0);

    if (!Environment::getConfiguration("image_classification_imagenet").size())
        GTEST_SKIP() << "No data to perform the test";

    const std::vector<YAML::Node> nodes =
        Environment::getConfiguration("image_classification_imagenet");
    ASSERT_GT(nodes.size(), 1);

    for (size_t i = 0; i < 2; i++) {
        auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, nodes[i]);
        ASSERT_EQ(getBuildLog(zeGraphDDITableExt),
                  "ZE DynamicCaching cache_status_t: cache_status_t::stored\n");
        ASSERT_EQ(getNumFiles(cacheDir), i + 1);
        auto graph2 =
            Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, nodes[i]);
        ASSERT_EQ(getBuildLog(zeGraphDDITableExt),
                  "ZE DynamicCaching cache_status_t: cache_status_t::found\n");
        ASSERT_EQ(getNumFiles(cacheDir), i + 1);
    }
}
