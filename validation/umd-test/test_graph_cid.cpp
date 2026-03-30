/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "frame_counter.hpp"
#include "graph_utilities.hpp"
#include "umd_dma_heap_system.hpp"

#include <chrono>
#include <future>
#include <stdexcept>
#include <ze_api.h>

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
        ASSERT_NE(graph, nullptr);

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

    ASSERT_TRUE(graph->checkResults());
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
        ASSERT_NE(graph, nullptr);
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
                  ZE_RESULT_SUCCESS)
            << "Failed to get profiling data size for type "
            << getGraphProfilingTypeStr(profilingType);
        EXPECT_GT(dataSize, 0u) << "Profiling data size is zero for type "
                                << getGraphProfilingTypeStr(profilingType);

        std::vector<ProfilingData> profilingData(dataSize / sizeof(ProfilingData));
        EXPECT_EQ(zeGraphProfilingDDITableExt->pfnProfilingQueryGetData(
                      hProfilingQuery,
                      profilingType,
                      &dataSize,
                      reinterpret_cast<uint8_t *>(profilingData.data())),
                  ZE_RESULT_SUCCESS)
            << "Failed to get profiling data for type " << getGraphProfilingTypeStr(profilingType);

        return profilingData;
    }

    void getProfilingLogs(ze_graph_profiling_query_handle_t hProfilingQuery,
                          ze_graph_profiling_type_t profilingType) {
        uint32_t logSize = 0;
        ASSERT_EQ(zeGraphProfilingDDITableExt->pfnProfilingLogGetString(hProfilingQuery,
                                                                        &logSize,
                                                                        nullptr),
                  ZE_RESULT_SUCCESS)
            << "Failed to get the size of error message for "
            << getGraphProfilingTypeStr(profilingType) << " data type";

        std::vector<char> profilingLogBuffer(logSize, 0);
        ASSERT_EQ(zeGraphProfilingDDITableExt->pfnProfilingLogGetString(hProfilingQuery,
                                                                        &logSize,
                                                                        profilingLogBuffer.data()),
                  ZE_RESULT_SUCCESS)
            << "Failed to get the error message for " << getGraphProfilingTypeStr(profilingType)
            << " data type";

        if (logSize - 1 > 0) {
            PRINTF("Graph profiling logs: \n%s\n", profilingLogBuffer.data());
        }
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

    ze_graph_properties_3_t graphProperties = {};
    graphProperties.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES_3;
    ASSERT_EQ(graph->getGraphProperties(&graphProperties), ZE_RESULT_SUCCESS);
    ASSERT_TRUE((graphProperties.flags & ZE_GRAPH_PROPERTIES_FLAG_PROFILING_ENABLED) != 0);

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

    std::vector<ze_profiling_task_info> taskInfo =
        queryProfilingData<ze_profiling_task_info>(hProfilingQuery, ZE_GRAPH_PROFILING_TASK_LEVEL);

    EXPECT_TRUE(!taskInfo.empty()) << "Task level profiling info is empty.";

    getProfilingLogs(hProfilingQuery, ZE_GRAPH_PROFILING_TASK_LEVEL);

    constexpr size_t maxPrint = 3;
    TRACE("\nGraph profiling tasks, size: %lu, print limit: %lu\n", taskInfo.size(), maxPrint);
    TRACE("-----------------------------------------------\n");
    for (size_t i = 0; i < std::min(taskInfo.size(), maxPrint); i++) {
        TRACE("#%lu\n", i);
        TRACE("name:             %s\n", taskInfo[i].name);
        TRACE("layer_type:       %s\n", taskInfo[i].layer_type);
        TRACE("exec_type:        %s\n", getExecTypeStr(taskInfo[i].exec_type));
        TRACE("start_time_ns:    %lu\n", taskInfo[i].start_time_ns);
        TRACE("duration_ns:      %lu\n", taskInfo[i].duration_ns);
        TRACE("active_cycles:    %u\n", taskInfo[i].active_cycles);
        TRACE("stall_cycles:     %u\n", taskInfo[i].stall_cycles);
        TRACE("task_id:          %u\n", taskInfo[i].task_id);
        TRACE("parent_layer_id:  %u\n", taskInfo[i].parent_layer_id);
        TRACE("-----------------------------------------------\n");
    }

    std::vector<ze_profiling_layer_info> layerInfo =
        queryProfilingData<ze_profiling_layer_info>(hProfilingQuery,
                                                    ZE_GRAPH_PROFILING_LAYER_LEVEL);

    EXPECT_TRUE(!layerInfo.empty()) << "Layer level profiling info is empty.";

    getProfilingLogs(hProfilingQuery, ZE_GRAPH_PROFILING_LAYER_LEVEL);

    TRACE("\nGraph profiling layers, size: %lu, print limit: %lu\n", layerInfo.size(), maxPrint);
    TRACE("-----------------------------------------------\n");
    for (size_t i = 0; i < std::min(layerInfo.size(), maxPrint); i++) {
        TRACE("#%lu\n", i);
        TRACE("name:            %s\n", layerInfo[i].name);
        TRACE("layer_type:      %s\n", layerInfo[i].layer_type);
        TRACE("status:          %s\n", getStatusStr(layerInfo[i].status));
        TRACE("start_time_ns:   %lu\n", layerInfo[i].start_time_ns);
        TRACE("duration_ns:     %lu\n", layerInfo[i].duration_ns);
        TRACE("layer_id:        %u\n", layerInfo[i].layer_id);
        TRACE("fused_layer_id:  %lu\n", layerInfo[i].fused_layer_id);
        TRACE("dpu_ns:          %lu\n", layerInfo[i].dpu_ns);
        TRACE("sw_ns:           %lu\n", layerInfo[i].sw_ns);
        TRACE("dma_ns:          %lu\n", layerInfo[i].dma_ns);
        TRACE("-----------------------------------------------\n");
    }
}

class CompilerInDriverLongBmp : public CompilerInDriverLongT,
                                public ::testing::WithParamInterface<YAML::Node> {
  public:
    void SetUp() override {
        CompilerInDriverLongT::SetUp();

        const YAML::Node node = GetParam();

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);
        ASSERT_NE(graph, nullptr);

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

    ASSERT_TRUE(graph->checkResults());
}

class CompilerInDriverBmpUsingDmaHeap : public CompilerInDriverLongBmp {
  public:
    void SetUp() override {
        CHECK_DMA_HEAP_SUPPORT(dmaHeapSystem);

        CompilerInDriverLongT::SetUp();

        const YAML::Node node = GetParam();

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);
        ASSERT_NE(graph, nullptr);

        if (graph->imagePaths.empty()) {
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

    ASSERT_TRUE(graph->checkResults());
}

#define BREAK_ON_FAIL(ret, stats, msg)            \
    if (ret != ZE_RESULT_SUCCESS) {               \
        EXPECT_EQ(ret, ZE_RESULT_SUCCESS) << msg; \
        stats.status = ret;                       \
        stats.counter.stopTimer();                \
        return stats;                             \
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
        bool isTurboMode = false;
        uint64_t iterationCount = 0;
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
    ze_command_queue_desc_npu_ext_t cmdQueueNpuDesc = {};

    void SetUp() override {
        CompilerInDriverLongT::SetUp();

        std::string testSubset("pipeline");
        if (GetParam()["pipeline"].IsDefined() == false) {
            testSubset = "benchmark";
        }
        const YAML::Node modelsSet = GetParam()[testSubset.c_str()];

        if (modelsSet.size() == 0)
            SKIP_("Missing models for testing");

        for (auto &model : modelsSet) {
            LocalInference inference(model);

            inference.targetFps = model["target_fps"].as<uint32_t>(30);
            inference.isTurboMode = false;
            inference.execTimeSec = model["exec_time_in_secs"].as<uint32_t>(3);
            inference.delayInUs = model["delay_in_us"].as<size_t>(0);
            inference.fpsDeviation = model["fps_deviation"].as<double>(0);
            inference.iterationCount = model["iteration_count"].as<uint64_t>(0);
            inference.parallelReqs = model["parallel_reqs"].as<size_t>(1);
            ASSERT_GT(inference.parallelReqs, 0) << "parallel_reqs field has to be greater than 0";

            if (model["priority"].IsDefined() && model["priority"].as<std::string>().size())
                inference.priority = toZePriority(model["priority"].as<std::string>());

            if (model["turbo_mode"].IsDefined() && model["turbo_mode"].as<bool>()) {
                inference.isTurboMode = true;
                cmdQueueNpuDesc = {ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC_NPU_EXT, nullptr, true};
            }

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
            ASSERT_NE(graph, nullptr);

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

    void printInferenceStats(const std::string &model, const InferenceStats &stats) {
        PRINTF("----------------------------------------------------\n");
        PRINTF("Model:                %s\n", model.c_str());
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
    }

    InferenceStats runInference(const CompilerInDriverMultiInference::LocalInference &inference) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        InferenceStats stats = {};
        ze_command_queue_workload_type_t workloadType = inference.workloadType;

        auto cmdQueueDescInference = cmdQueueDesc;
        cmdQueueDescInference.priority = inference.priority;
        cmdQueueDescInference.pNext = inference.isTurboMode ? &cmdQueueNpuDesc : nullptr;
        auto scopedQueue =
            zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDescInference, ret);
        BREAK_ON_FAIL(ret, stats, "Failed to create command queue for inference");
        auto queue = scopedQueue.get();

        std::vector<std::unique_ptr<InferenceRequest>> inferReqs;
        inference.graph->allocateArguments(MemType::SHARED_MEMORY);
        inference.graph->copyInputData();
        ret = zeGraphDDITableExt->pfnGraphInitialize(inference.graph->handle);
        BREAK_ON_FAIL(ret, stats, "Failed to initialize graph");
        inferReqs.push_back(inference.graph->inferenceRequest(queue));

        ret = inferReqs.back()->runAsync();
        BREAK_ON_FAIL(ret, stats, "Failed to run inference request");

        ret = inferReqs.back()->wait(UINT64_MAX);
        BREAK_ON_FAIL(ret, stats, "Failed to wait for inference request");
        if (!inference.graph->hasOutputReference() &&
            !inference.graph->setupCurrentOutputAsReference()) {
            BREAK_ON_FAIL(ZE_RESULT_ERROR_UNKNOWN, stats, "Failed to setup output reference");
        }

        inference.graph->clearOutput();

        for (size_t i = 1; i < inference.parallelReqs; i++) {
            inference.graph->allocateArguments(MemType::SHARED_MEMORY);
            inference.graph->copyInputData();
            inferReqs.push_back(inference.graph->inferenceRequest(queue));
        }

        /*
         * For the case when config request workload type all inferences are started
         * as background, the requested workload will be set after first inference loop
         */
        if (workloadType != ZE_WORKLOAD_TYPE_FORCE_UINT32) {
            ret = zeCommandQueueDDITableExt->pfnSetWorkloadType(queue, ZE_WORKLOAD_TYPE_BACKGROUND);
            BREAK_ON_FAIL(ret, stats, "Failed to set workload type");
        }

        std::this_thread::sleep_for(std::chrono::microseconds(inference.delayInUs));

        stats.counter.startTimer(inference.execTimeSec, inference.targetFps);
        for (size_t i = 1; i < inferReqs.size(); i++) {
            ret = inferReqs[i]->runAsync();
            BREAK_ON_FAIL(ret, stats, "Failed to run async inference");
        }

        if (workloadType != ZE_WORKLOAD_TYPE_FORCE_UINT32) {
            ret = zeCommandQueueDDITableExt->pfnSetWorkloadType(queue, workloadType);
            BREAK_ON_FAIL(ret, stats, "Failed to set workload type");
        }

        size_t inferReqIndex = 0;
        while (inference.iterationCount > 0 ? stats.counter.frameCount < inference.iterationCount
                                            : !stats.counter.isTimeout()) {
            stats.counter.delayNextFrame();

            ret = inferReqs[inferReqIndex]->runAsync();
            BREAK_ON_FAIL(ret, stats, "Failed to run async inference");

            inferReqIndex++;
            inferReqIndex = inferReqIndex >= inferReqs.size() ? 0 : inferReqIndex;

            ret = inferReqs[inferReqIndex]->wait(UINT64_MAX);
            BREAK_ON_FAIL(ret, stats, "Failed to wait for inference request");

            if (!inferReqs[inferReqIndex]->validateOutput()) {
                TRACE("Output validation failed for inference request %s (%ld)",
                      inference.modelName.c_str(),
                      inferReqIndex);
                BREAK_ON_FAIL(ZE_RESULT_ERROR_UNSUPPORTED_IMAGE_FORMAT,
                              stats,
                              "Output validation failed");
            }
            stats.counter.recordFrame(inferReqs[inferReqIndex]->latencyMs);
            inferReqs[inferReqIndex]->clearOutput();
        }

        stats.counter.stopTimer();
        return stats;
    }
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverMultiInference);

INSTANTIATE_TEST_SUITE_P(
    ,
    CompilerInDriverMultiInference,
    ::testing::ValuesIn(Environment::getConfiguration("multi_inference", "pipeline")),
    [](const testing::TestParamInfo<YAML::Node> &p) { return p.param["name"].as<std::string>(); });

TEST_P(CompilerInDriverMultiInference, Pipeline) {
    auto testInference =
        [&](const CompilerInDriverMultiInference::LocalInference &inference) -> InferenceStats {
        return runInference(inference);
    };

    std::vector<std::future<InferenceStats>> results;
    for (size_t i = 0; i < testInferences.size(); i++)
        results.push_back(std::async(std::launch::async, testInference, testInferences[i]));

    for (size_t i = 0; i < results.size(); i++) {
        InferenceStats stats = results[i].get();
        printInferenceStats(testInferences[i].modelName, stats);
        EXPECT_EQ(stats.status, ZE_RESULT_SUCCESS);
    }
};

class CompilerInDriverInferenceBenchmark : public CompilerInDriverMultiInference {};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerInDriverInferenceBenchmark);

INSTANTIATE_TEST_SUITE_P(
    ,
    CompilerInDriverInferenceBenchmark,
    ::testing::ValuesIn(Environment::getConfiguration("multi_inference", "benchmark")),
    [](const testing::TestParamInfo<YAML::Node> &p) { return p.param["name"].as<std::string>(); });

TEST_P(CompilerInDriverInferenceBenchmark, Benchmark) {
    if (!isHwsModeEnabled() || !Environment::getInstance()->isDriverExtensionSupported(
                                   COMMAND_QUEUE_EXT_NAME,
                                   ZE_COMMAND_QUEUE_NPU_EXT_VERSION_1_1)) {
        GTEST_SKIP() << "HW configuration not sufficient for benchmark test.";
    }

    auto testInference =
        [&](const CompilerInDriverInferenceBenchmark::LocalInference &inference) -> InferenceStats {
        return runInference(inference);
    };

    std::vector<std::future<InferenceStats>> results;
    for (size_t i = 0; i < testInferences.size(); i++) {
        results.push_back(std::async(std::launch::async, testInference, testInferences[i]));
    }

    double averageFps = 0;
    uint32_t referenceFrames = 0;
    std::vector<InferenceStats> statsCollection;
    for (size_t i = 0; i < results.size(); i++) {
        InferenceStats stats = results[i].get();
        /* Calculate average fps only for models without benchmark acceptance threshold, they
         * are called reference models */
        if (testInferences[i].fpsDeviation == 0.0) {
            averageFps = (averageFps * referenceFrames + stats.counter.fps) / (referenceFrames + 1);
            referenceFrames++;
        }
        statsCollection.push_back(stats);
        printInferenceStats(testInferences[i].modelName, stats);
        EXPECT_EQ(stats.status, ZE_RESULT_SUCCESS);
    }

    /*If there are defined acceptance criteria for fps rate*/
    for (size_t i = 0; i < testInferences.size(); i++) {
        if (testInferences[i].fpsDeviation != 0.0) {
            PRINTF("\nBenchmark summary:\n");
            PRINTF("CalculatedFPS:        %f\n", statsCollection[i].counter.fps);
            PRINTF("Acceptance criteria:  %f\n", averageFps * testInferences[i].fpsDeviation);
            EXPECT_GE(statsCollection[i].counter.fps, averageFps * testInferences[i].fpsDeviation);
        }
    }
}
