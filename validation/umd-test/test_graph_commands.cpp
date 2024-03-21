/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

#include <fstream>
#include <functional>
#include <future>
#include <thread>

class CommandGraphBase : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();

        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();
    }

    void TearDown() override { UmdTest::TearDown(); }

    void threadedCommandQueueSyncWrapper(std::promise<_ze_result_t> &&promise);

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

    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;

    ze_result_t ret = ZE_RESULT_SUCCESS;

    std::shared_ptr<Graph> graph;

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
};

// Functor for threaded use case of zeCommandQueueSync with a promise as parameter
// umd-test will utilize std::thread to spawn a new thread with this functor to
// perform zeCommandQueueSynchronize with BLOCKING scenario (UINT64_MAX)
// While the std::promise is used to checked for the zeCommandQueueSynchronize
// result in the main thread with timeout.
void CommandGraphBase::threadedCommandQueueSyncWrapper(std::promise<_ze_result_t> &&promise) {
    // This thread has to be killed instantly, otherwise SEGFAULT could happen
    // if cancellation is DEFERRED post handleFailure()
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    ASSERT_NE(nullptr, queue);
    promise.set_value(zeCommandQueueSynchronize(queue, UINT64_MAX));
}

class CommandGraph : public CommandGraphBase {
  public:
    void SetUp() override {
        CommandGraphBase::SetUp();

        if (!Environment::getConfiguration("graph_execution").size())
            GTEST_SKIP() << "Do not find blobs to execute test";

        /* CommandGraph test will be run on first blob taken from configuration */
        const YAML::Node node = Environment::getConfiguration("graph_execution")[0];

        /* Validate configuration */
        ASSERT_GT(node["path"].as<std::string>().size(), 0);

        graph = Graph::create(zeContext,
                              zeDevice,
                              zeGraphDDITableExt,
                              blobDir + node["path"].as<std::string>(),
                              node);
    }
};

TEST_F(CommandGraph, AppendGraphInitWithNullGraphHandle) {
    ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphInitialize(list, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_ERROR_UNINITIALIZED);
}

TEST_F(CommandGraph, AppendGraphInitExecuteWithoutSettingArgumentForInputOutput) {
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_ERROR_UNINITIALIZED);
}

TEST_F(CommandGraph, SettingNullArgumentForInputOutput) {
    ze_graph_properties_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;
    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";

    for (size_t index = 0; index < graphProps.numGraphArgs; index++) {
        ASSERT_EQ(graph->setArgumentValue(index, nullptr), ZE_RESULT_ERROR_INVALID_NULL_POINTER);
    }
}

TEST_F(CommandGraph, SetArgumentIndexGreaterThanExpectedArgumentIndexLimit) {
    ze_graph_properties_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;
    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";

    ASSERT_EQ(graph->setArgumentValue(graphProps.numGraphArgs, graph->inputBin.at(0).data()),
              ZE_RESULT_ERROR_INVALID_ARGUMENT);
}

TEST_F(CommandGraph, GetArgumentPropertiesReturnsExpectedProperties) {
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

TEST_F(CommandGraph, AppendGraphInitExecuteWithoutAllocatingInputOutputMemory) {
    ASSERT_EQ(graph->setArgumentValue(0, graph->inputBin.at(0).data()), ZE_RESULT_SUCCESS);
    ASSERT_EQ(graph->setArgumentValue(1, nullptr), ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_ERROR_UNINITIALIZED);
}

class CommandGraphLong : public CommandGraphBase, public ::testing::WithParamInterface<YAML::Node> {
  protected:
    void SetUp() override {
        CommandGraphBase::SetUp();
        const YAML::Node node = GetParam();

        /* Validate configuration */
        ASSERT_GT(node["path"].as<std::string>().size(), 0);

        graph = Graph::create(zeContext,
                              zeDevice,
                              zeGraphDDITableExt,
                              blobDir + node["path"].as<std::string>(),
                              node);

        graph->allocateArguments(MemType::SHARED_MEMORY);
    }
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CommandGraphLong);

INSTANTIATE_TEST_SUITE_P(,
                         CommandGraphLong,
                         ::testing::ValuesIn(Environment::getConfiguration("graph_execution")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(CommandGraphLong, AppendGraphInitExecuteAndSynchronize) {
    graph->copyInputData();

    std::chrono::steady_clock::time_point graphInitializeStart, graphInitializeStop,
        executeInferenceStart, executeInferenceStop;

    std::chrono::duration<float> durationGraphInitialize, durationExecuteInference;

    graphInitializeStart = std::chrono::steady_clock::now();

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graphInitializeStop = std::chrono::steady_clock::now();

    durationGraphInitialize = std::chrono::duration_cast<std::chrono::duration<float>>(
        graphInitializeStop - graphInitializeStart);

    TRACE("\nJob with blob initialization took: %f [s]\n", durationGraphInitialize.count());

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

    executeInferenceStart = std::chrono::steady_clock::now();

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    executeInferenceStop = std::chrono::steady_clock::now();

    durationExecuteInference = std::chrono::duration_cast<std::chrono::duration<float>>(
        executeInferenceStop - executeInferenceStart);

    TRACE("\nJob with the blob execution took: %f [s]\n", durationExecuteInference.count());

    printf("\nFirst inference latency: %f [s]\n\n",
           (durationGraphInitialize + durationExecuteInference).count());

    graph->checkResults();
}

TEST_P(CommandGraphLong, AppendGraphInitExecuteAndThreadedSynchronize) {
    graph->copyInputData();

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

    // Use std::promise to track if zeCommandQueueSynchronize completed in separate
    // thread. In scenario that blocking call with UINT64_MAX or TDR mechanism
    // unable to fail gracefully for UMD, userspace app can rely on timeout
    // to kill the zeCommandQueueSynchronize thread.
    std::promise<_ze_result_t> promise;
    std::future<_ze_result_t> futureSync = promise.get_future();
    std::thread cmdQueueSyncThread(&CommandGraphBase::threadedCommandQueueSyncWrapper,
                                   this,
                                   std::move(promise));

    // Keep the native pthread handle to cancellation, prior to detach()
    pthread_t threadID = cmdQueueSyncThread.native_handle();
    cmdQueueSyncThread.detach();

    // Userspace app defined timeout for decision on killing the zeCommandQueueSynchronize
    // thread
    std::chrono::steady_clock::time_point asyncTimeOut =
        std::chrono::steady_clock::now() + std::chrono::microseconds(graphSyncTimeout);

    ze_result_t result = ZE_RESULT_NOT_READY;
    if (std::future_status::ready == futureSync.wait_until(asyncTimeOut)) {
        result = futureSync.get();
    } else {
        // asyncTimeOut and zeCommandQueueSynchronize hasn't returned
        // userspace app needs to cancel the thread prior to handleFailure()
        // which free the objects
        pthread_cancel(threadID);
    }
    ASSERT_EQ(result, ZE_RESULT_SUCCESS) << "TIMEOUT from threadedCommandQueueSyncWrapper";
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}

TEST_P(CommandGraphLong, RunGraphInitOnly) {
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
}

TEST_P(CommandGraphLong, AppendGraphInitTwiceAndExecute) {
    graph->copyInputData();

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}

TEST_P(CommandGraphLong, RunGraphExecuteThreeTimes) {
    graph->copyInputData();

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    for (uint32_t i = 0; i < 3; i++) {
        ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeGraphDDITableExt
                      ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

        graph->checkResults();
        graph->clearOutput();
    }
}

TEST_P(CommandGraphLong, SingleListGraphExecutionWithBarrierTest) {
    std::vector<std::shared_ptr<void>> mem;
    std::vector<void *> inputMemHost, outputMemHost;

    for (auto &input : graph->inputBin) {
        auto memInput = AllocHostMemory(input.size());

        mem.push_back(memInput);
        inputMemHost.push_back(memInput.get());
        memcpy(memInput.get(), input.data(), input.size());
    }

    for (auto &output : graph->outputBin) {
        auto memOutput = AllocHostMemory(output.size());

        mem.push_back(memOutput);
        outputMemHost.push_back(memOutput.get());
    }

    for (size_t i = 0; i < graph->inputBin.size(); i++) {
        ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                                graph->inArgs[i],
                                                inputMemHost[i],
                                                graph->inputBin[i].size(),
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);
    }

    ASSERT_EQ(zeCommandListAppendBarrier(list, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendBarrier(list, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);

    for (size_t i = 0; i < graph->outputBin.size(); i++) {
        ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                                outputMemHost[i],
                                                graph->outArgs[i],
                                                graph->outputBin[i].size(),
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);
    }

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    for (size_t i = 0; i < graph->outArgs.size(); i++)
        ASSERT_EQ(memcmp(outputMemHost[i], graph->outputBin[i].data(), graph->outputBin[i].size()),
                  0);
}

TEST_P(CommandGraphLong, LoadGraphOnceAndRunExecutionTwice) {
    graph->copyInputData();

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}

TEST_P(CommandGraphLong, RunGraphExecuteInTwoSeparateCommandLists) {
    graph->copyInputData();

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

    ze_result_t ret = ZE_RESULT_SUCCESS;
    auto scopedList1 = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto list1 = scopedList1.get();

    const std::vector<void *> &graphOutput1 = graph->outArgs;

    graph->allocateArguments(MemType::SHARED_MEMORY);

    graph->copyInputData();

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list1, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list1), ZE_RESULT_SUCCESS);

    ze_fence_desc_t fenceDesc = {ZE_STRUCTURE_TYPE_FENCE_DESC, nullptr, 0};

    auto scopedFence0 = zeScope::fenceCreate(queue, fenceDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto scopedFence1 = zeScope::fenceCreate(queue, fenceDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto fence0 = scopedFence0.get();
    auto fence1 = scopedFence1.get();

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, fence0), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list1, fence1), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeFenceHostSynchronize(fence0, graphSyncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeFenceHostSynchronize(fence1, graphSyncTimeout), ZE_RESULT_SUCCESS);

    for (size_t i = 0; i < graph->outArgs.size(); i++) {
        EXPECT_EQ(memcmp(graphOutput1[i], graph->outputBin[i].data(), graph->outputBin[i].size()),
                  0);
        EXPECT_EQ(memcmp(graph->outArgs[i], graph->outputBin[i].data(), graph->outputBin[i].size()),
                  0);
    }
}

TEST_P(CommandGraphLong, AppendGraphInitAndExecuteWithSingleMemoryAllocation) {
    auto offset = 0x100;
    size_t totalArgSize = 0;
    for (const auto &arg : graph->inputBin) {
        totalArgSize += arg.size();
    }
    for (const auto &arg : graph->outputBin) {
        totalArgSize += arg.size();
    }

    auto mem = AllocHostMemory(offset + totalArgSize);
    uint8_t *address = static_cast<uint8_t *>(mem.get()) + offset;

    for (size_t i = 0; i < graph->inArgs.size(); i++) {
        memcpy(address, graph->inputBin[i].data(), graph->inputBin[i].size());
        address += graph->inputBin[i].size();
    }

    uint32_t argIndex = 0;
    address = static_cast<uint8_t *>(mem.get()) + offset;
    for (const auto &arg : graph->inputBin) {
        ASSERT_EQ(graph->setArgumentValue(argIndex++, address), ZE_RESULT_SUCCESS);
        address += arg.size();
    }
    graph->outArgs.clear();
    for (const auto &arg : graph->outputBin) {
        ASSERT_EQ(graph->setArgumentValue(argIndex++, address), ZE_RESULT_SUCCESS);
        graph->outArgs.push_back(address);
        address += arg.size();
    }

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}

TEST_P(CommandGraphLong, GraphInitAndExecWith200msDelay) {
    graph->copyInputData();

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}

class CommandGraphLongThreaded
    : public CommandGraphBase,
      public ::testing::WithParamInterface<std::tuple<YAML::Node, uint32_t>> {};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CommandGraphLongThreaded);

INSTANTIATE_TEST_SUITE_P(
    ,
    CommandGraphLongThreaded,
    ::testing::Combine(::testing::ValuesIn(Environment::getConfiguration("graph_execution")),
                       ::testing::Values(2, 4)),
    [](const testing::TestParamInfo<std::tuple<YAML::Node, uint32_t>> &p) {
        const YAML::Node node(std::get<0>(p.param));
        uint32_t threads = std::get<1>(p.param);
        return generateTestNameFromNode(node) + "_" + std::to_string(threads) + "_Threads";
    });

TEST_P(CommandGraphLongThreaded, AppendGraphInitExecuteAndSynchronize) {
    auto param = GetParam();
    const YAML::Node node(std::get<0>(param));
    uint32_t threadParam = std::get<1>(param);

    /* Validate configuration */
    ASSERT_GT(node["path"].as<std::string>().size(), 0);

    std::vector<std::unique_ptr<std::thread>> tasks;
    for (size_t i = 0; i < threadParam; i++) {
        tasks.push_back(std::make_unique<std::thread>([this, node]() {
            std::shared_ptr<Graph> graph = Graph::create(zeContext,
                                                         zeDevice,
                                                         zeGraphDDITableExt,
                                                         blobDir + node["path"].as<std::string>(),
                                                         node);

            ze_result_t ret = ZE_RESULT_SUCCESS;

            auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
            auto queue = scopedQueue.get();

            auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
            auto list = scopedList.get();

            graph->allocateArguments(MemType::SHARED_MEMORY);

            graph->copyInputData();

            ASSERT_EQ(zeGraphDDITableExt
                          ->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
                      ZE_RESULT_SUCCESS);
            ASSERT_EQ(
                zeGraphDDITableExt
                    ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
                ZE_RESULT_SUCCESS);
            ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

            ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr),
                      ZE_RESULT_SUCCESS);
            ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

            graph->checkResults();
        }));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
}

TEST_F(CommandGraphLongThreaded, RunAllBlobsInSingleContextSimultaneously) {
    std::vector<std::unique_ptr<std::thread>> tasks;

    if (!Environment::getConfiguration("graph_execution").size())
        GTEST_SKIP() << "Do not find blobs to execute test";

    for (const auto &node : Environment::getConfiguration("graph_execution")) {
        ASSERT_GT(node["path"].as<std::string>().size(), 0);

        tasks.push_back(std::make_unique<std::thread>([this, node]() {
            std::shared_ptr<Graph> graph;
            {
                graph = Graph::create(zeContext,
                                      zeDevice,
                                      zeGraphDDITableExt,
                                      blobDir + node["path"].as<std::string>(),
                                      node);

                graph->allocateArguments(MemType::SHARED_MEMORY);
            }

            graph->copyInputData();

            auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
            auto queue = scopedQueue.get();

            auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
            auto list = scopedList.get();

            ASSERT_EQ(zeGraphDDITableExt
                          ->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
                      ZE_RESULT_SUCCESS);
            ASSERT_EQ(
                zeGraphDDITableExt
                    ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
                ZE_RESULT_SUCCESS);
            ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

            ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr),
                      ZE_RESULT_SUCCESS);
            ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

            graph->checkResults();
        }));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
}
