/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

#include <fcntl.h>
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

#ifndef ANDROID
    // Functor for threaded use case of zeCommandQueueSync with a promise as parameter
    // umd-test will utilize std::thread to spawn a new thread with this functor to
    // perform zeCommandQueueSynchronize with BLOCKING scenario (UINT64_MAX)
    // While the std::promise is used to check for the zeCommandQueueSynchronize
    // result in the main thread with timeout.
    void threadedCommandQueueSyncWrapper(std::promise<_ze_result_t> &&promise) {
        // This thread has to be killed instantly, otherwise SEGFAULT could happen
        // if cancellation is DEFERRED post handleFailure()
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        ASSERT_NE(nullptr, queue);
        promise.set_value(zeCommandQueueSynchronize(queue, UINT64_MAX));
    }
#endif

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

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
};

class CommandGraphLong : public CommandGraphBase, public ::testing::WithParamInterface<YAML::Node> {
  protected:
    void SetUp() override {
        CommandGraphBase::SetUp();
        const YAML::Node node = GetParam();

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

        graph->allocateArguments(MemType::SHARED_MEMORY);
        graph->copyInputData();
    }

    std::shared_ptr<Graph> graph;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CommandGraphLong);

INSTANTIATE_TEST_SUITE_P(,
                         CommandGraphLong,
                         ::testing::ValuesIn(Environment::getConfiguration("graph_execution")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(CommandGraphLong, AppendGraphInitExecuteAndSynchronize) {
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

#ifndef ANDROID
TEST_P(CommandGraphLong, AppendGraphInitExecuteAndThreadedSynchronize) {
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
#endif

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

TEST_P(CommandGraphLong, RunInferenceWithGraphInitialize) {
    ASSERT_EQ(zeGraphDDITableExt->pfnGraphInitialize(graph->handle), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
    graph->clearOutput();
}

TEST_P(CommandGraphLong, SingleListGraphExecutionWithBarrierTest) {
    std::vector<std::shared_ptr<void>> mem;
    std::vector<void *> inputHostMem, outputHostMem;

    for (auto size : graph->inputSize) {
        mem.push_back(AllocHostMemory(size));
        inputHostMem.push_back(mem.back().get());
    }

    for (auto size : graph->outputSize) {
        mem.push_back(AllocHostMemory(size));
        outputHostMem.push_back(mem.back().get());
    }

    graph->copyInputData(inputHostMem);

    for (size_t i = 0; i < graph->inputSize.size(); i++) {
        ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                                graph->inArgs[i],
                                                inputHostMem[i],
                                                graph->inputSize[i],
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

    for (size_t i = 0; i < graph->outputSize.size(); i++) {
        ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                                outputHostMem[i],
                                                graph->outArgs[i],
                                                graph->outputSize[i],
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);
    }

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults(outputHostMem);
}

TEST_P(CommandGraphLong, LoadGraphOnceAndRunExecutionTwice) {
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

    std::vector<void *> &graphOutput = graph->outArgs;

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

    graph->checkResults(graphOutput);

    graph->checkResults();
}

TEST_P(CommandGraphLong, AppendGraphInitAndExecuteWithSingleMemoryAllocation) {
    auto offset = 0x100;
    size_t totalArgSize = 0;
    for (auto size : graph->inputSize) {
        totalArgSize += size;
    }

    for (auto size : graph->outputSize) {
        totalArgSize += size;
    }

    auto mem = AllocHostMemory(offset + totalArgSize);
    uint8_t *address = static_cast<uint8_t *>(mem.get()) + offset;

    for (size_t i = 0; i < graph->inArgs.size(); i++) {
        memcpy(address, graph->inArgs[i], graph->inputSize[i]);
        address += graph->inputSize[i];
    }

    uint32_t argIndex = 0;
    address = static_cast<uint8_t *>(mem.get()) + offset;
    for (auto size : graph->inputSize) {
        ASSERT_EQ(graph->setArgumentValue(argIndex++, address), ZE_RESULT_SUCCESS);
        address += size;
    }

    graph->outArgs.clear();
    for (auto size : graph->outputSize) {
        ASSERT_EQ(graph->setArgumentValue(argIndex++, address), ZE_RESULT_SUCCESS);
        graph->outArgs.push_back(address);
        address += size;
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

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}

static void resetDevice() {
    std::string path = getDeviceSysFsDirectory() + "/reset";
    int fd = open(path.c_str(), O_WRONLY);
    ASSERT_NE(fd, -1);

    ASSERT_EQ(write(fd, "1", 1), 1);
    ASSERT_EQ(close(fd), 0);
}

TEST_P(CommandGraphLong, InferenceDeviceResetInference) {
    SKIP_NEEDS_ROOT();

    ze_result_t result = zeGraphDDITableExt->pfnAppendGraphExecute(list,
                                                                   graph->handle,
                                                                   nullptr,
                                                                   nullptr,
                                                                   0,
                                                                   nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandListClose(list);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueSynchronize(queue, syncTimeout);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    graph->checkResults();

    resetDevice();

    result = zeContextGetStatus(zeContext);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeDeviceGetStatus(zeDevice);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    graph->clearOutput();

    result = zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueSynchronize(queue, syncTimeout);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    graph->checkResults();
}

TEST_P(CommandGraphLong, GetNativeBinaryAndReleaseAfterAppendGraphInitializeRunInference) {
    // Convert model to native format, zeGraphGetNativeBinary function is used
    auto nativeGraphBuffer = graph->getNativeBinaryAsNewBuffer();
    ASSERT_NE(nativeGraphBuffer, nullptr) << "Unable to get native binary from Graph";

    const YAML::Node node = GetParam();

    // Swap the graph with native format type
    graph = Graph::create(zeContext,
                          zeDevice,
                          zeGraphDDITableExt,
                          globalConfig,
                          node,
                          nativeGraphBuffer);

    ASSERT_NE(graph, nullptr) << "Unable to create new Graph object from native binary";

    graph->allocateArguments(MemType::SHARED_MEMORY);
    graph->copyInputData();

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    // Release the blob container
    nativeGraphBuffer.reset();

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}

TEST_P(CommandGraphLong, GetNativeBinaryAndReleaseAfterGraphInitializeRunInference) {
    // Convert model to native format, zeGraphGetNativeBinary function is used
    auto nativeGraphBuffer = graph->getNativeBinaryAsNewBuffer();
    ASSERT_NE(nativeGraphBuffer, nullptr) << "Unable to get native binary from Graph";

    const YAML::Node node = GetParam();

    // Swap the graph with native format type
    graph = Graph::create(zeContext,
                          zeDevice,
                          zeGraphDDITableExt,
                          globalConfig,
                          node,
                          nativeGraphBuffer);

    ASSERT_NE(graph, nullptr) << "Unable to create new Graph object from native binary";

    graph->allocateArguments(MemType::SHARED_MEMORY);
    graph->copyInputData();

    ASSERT_EQ(zeGraphDDITableExt->pfnGraphInitialize(graph->handle), ZE_RESULT_SUCCESS);

    // Release the blob container
    nativeGraphBuffer.reset();

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}

TEST_P(CommandGraphLong, GetNativeBinary2AndReleaseAfterAppendGraphInitializeRunInference) {
    // Convert model to native format, zeGraphGetNativeBinary2 function is used
    auto nativeGraphBuffer = graph->getNativeBinary2AsNewBuffer();
    ASSERT_NE(nativeGraphBuffer, nullptr) << "Unable to get native binary from Graph";

    const YAML::Node node = GetParam();

    // Swap the graph with native format type
    graph = Graph::create(zeContext,
                          zeDevice,
                          zeGraphDDITableExt,
                          globalConfig,
                          node,
                          nativeGraphBuffer);

    ASSERT_NE(graph, nullptr) << "Unable to create new Graph object from native binary";

    graph->allocateArguments(MemType::SHARED_MEMORY);
    graph->copyInputData();

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    // Release the blob container
    nativeGraphBuffer.reset();

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}

TEST_P(CommandGraphLong, GetNativeBinary2AndReleaseAfterGraphInitializeRunInference) {
    // Convert model to native format, zeGraphGetNativeBinary2 function is used
    auto nativeGraphBuffer = graph->getNativeBinary2AsNewBuffer();
    ASSERT_NE(nativeGraphBuffer, nullptr) << "Unable to get native binary from Graph";

    const YAML::Node node = GetParam();

    // Swap the graph with native format type
    graph = Graph::create(zeContext,
                          zeDevice,
                          zeGraphDDITableExt,
                          globalConfig,
                          node,
                          nativeGraphBuffer);

    ASSERT_NE(graph, nullptr) << "Unable to create new Graph object from native binary";

    graph->allocateArguments(MemType::SHARED_MEMORY);
    graph->copyInputData();

    ASSERT_EQ(zeGraphDDITableExt->pfnGraphInitialize(graph->handle), ZE_RESULT_SUCCESS);

    // Release the blob container
    nativeGraphBuffer.reset();

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

TEST_P(CommandGraphLongThreaded, RunInferenceUseCommandQueueSynchronize) {
    auto param = GetParam();
    const YAML::Node node(std::get<0>(param));
    uint32_t threadParam = std::get<1>(param);

    std::vector<std::unique_ptr<std::thread>> tasks;
    for (size_t i = 0; i < threadParam; i++) {
        tasks.push_back(std::make_unique<std::thread>([this, node]() {
            std::shared_ptr<Graph> graph =
                Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

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

TEST_P(CommandGraphLongThreaded, RunInferenceUseFenceSynchronize) {
    auto param = GetParam();
    const YAML::Node node(std::get<0>(param));
    uint32_t threadParam = std::get<1>(param);

    ze_result_t ret = ZE_RESULT_SUCCESS;
    auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto queue = scopedQueue.get();

    std::shared_ptr<Graph> graph =
        Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

    graph->allocateArguments(MemType::SHARED_MEMORY);
    graph->copyInputData();

    std::vector<std::unique_ptr<std::thread>> tasks;
    for (size_t i = 0; i < threadParam; i++) {
        tasks.push_back(std::make_unique<std::thread>([this, node, queue, &graph]() {
            ze_result_t ret = ZE_RESULT_SUCCESS;
            auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
            auto list = scopedList.get();

            ze_fence_desc_t fenceDesc = {ZE_STRUCTURE_TYPE_FENCE_DESC, nullptr, 0};
            auto scopedFence = zeScope::fenceCreate(queue, fenceDesc, ret);
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
            auto fence = scopedFence.get();

            ASSERT_EQ(zeGraphDDITableExt
                          ->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
                      ZE_RESULT_SUCCESS);
            ASSERT_EQ(
                zeGraphDDITableExt
                    ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
                ZE_RESULT_SUCCESS);
            ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

            ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, fence), ZE_RESULT_SUCCESS);
            ASSERT_EQ(zeFenceHostSynchronize(fence, graphSyncTimeout), ZE_RESULT_SUCCESS);

            graph->checkResults();
        }));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
}

TEST_P(CommandGraphLong, MutableCmdList) {
    ze_mutable_command_list_exp_desc_t mutableListDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_LIST_EXP_DESC,
        .pNext = nullptr,
        .flags = 0,
    };
    ze_command_list_desc_t commandListDesc{
        .stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
        .pNext = &mutableListDesc,
        .commandQueueGroupOrdinal = 0u,
        .flags = 0,
    };
    ze_command_list_handle_t mutableList;
    ASSERT_EQ(zeCommandListCreate(zeContext, zeDevice, &commandListDesc, &mutableList),
              ZE_RESULT_SUCCESS);

    ze_mutable_command_id_exp_desc_t mutableCmdIdDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_ID_EXP_DESC,
        .pNext = nullptr,
        .flags = ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS,
    };
    uint64_t commandId;
    ASSERT_EQ(zeCommandListGetNextCommandIdExp(mutableList, &mutableCmdIdDesc, &commandId),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(mutableList, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(mutableList), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &mutableList, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
    graph->clearOutput();

    // update the first argument with the same input buffer
    ze_mutable_graph_argument_exp_desc_t graphArgumentDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
        .pNext = nullptr,
        .commandId = commandId,
        .argIndex = 0,
        .pArgValue = graph->inArgs[0],
    };
    ze_mutable_commands_exp_desc_t mutableCommandsDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMANDS_EXP_DESC,
        .pNext = &graphArgumentDesc,
        .flags = 0,
    };
    ASSERT_EQ(zeCommandListUpdateMutableCommandsExp(mutableList, &mutableCommandsDesc),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(mutableList), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &mutableList, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
    graph->clearOutput();

    // realloc the first argument and update the graph
    graphArgumentDesc.pArgValue = graph->reallocArgument(0, MemType::SHARED_MEMORY);
    ASSERT_EQ(zeCommandListUpdateMutableCommandsExp(mutableList, &mutableCommandsDesc),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(mutableList), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &mutableList, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}
