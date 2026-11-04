/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"
#include "umd_test.h"

using namespace std::literals;

using MemoryResidency = UmdTest;

TEST_F(MemoryResidency, InferencesWithMakeMemoryResident) {
    auto nodes = Environment::getConfiguration("graph_execution");
    ASSERT_GT(nodes.size(), 1);
    std::vector<std::shared_ptr<Graph>> graphs;
    std::vector<std::unique_ptr<InferenceRequest>> inferences;
    for (int i = 0; i < 2; i++) {
        auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, nodes[i]);
        ASSERT_NE(graph, nullptr);
        graph->allocateArguments(MemType::SHARED_MEMORY);
        graph->copyInputData();
        for (size_t j = 0; j < graph->inputSize.size(); j++) {
            ASSERT_EQ(ZE_RESULT_SUCCESS,
                      zeContextMakeMemoryResident(zeContext,
                                                  zeDevice,
                                                  graph->inArgs[j],
                                                  graph->inputSize[j]));
        }
        for (size_t j = 0; j < graph->outputSize.size(); j++) {
            ASSERT_EQ(ZE_RESULT_SUCCESS,
                      zeContextMakeMemoryResident(zeContext,
                                                  zeDevice,
                                                  graph->outArgs[j],
                                                  graph->outputSize[j]));
        }
        auto inference = std::make_unique<InferenceRequest>(*graph.get());
        inference->setUpCommandQueue(zeContext, zeDevice);
        inference->setUpCommandList(zeContext, zeDevice, graph->handle, zeGraphDDITableExt);
        inferences.push_back(std::move(inference));
        graphs.push_back(graph);
    }

    for (auto &inference : inferences) {
        ASSERT_EQ(inference->runAsync(), ZE_RESULT_SUCCESS);
        ASSERT_EQ(inference->wait(graphSyncTimeout), ZE_RESULT_SUCCESS);
        ASSERT_TRUE(inference->validateOutput());
    }

    for (size_t i = 0; i < graphs[0]->inputSize.size(); i++) {
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeContextEvictMemory(zeContext,
                                       zeDevice,
                                       graphs[0]->inArgs[i],
                                       graphs[0]->inputSize[i]));
    }
    for (size_t i = 0; i < graphs[0]->outputSize.size(); i++) {
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeContextEvictMemory(zeContext,
                                       zeDevice,
                                       graphs[0]->outArgs[i],
                                       graphs[0]->outputSize[i]));
    }

    ASSERT_EQ(inferences[1]->runAsync(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(inferences[1]->wait(graphSyncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_TRUE(inferences[1]->validateOutput());
}

TEST_F(MemoryResidency, EvictAfterMemFree) {
    auto nodes = Environment::getConfiguration("graph_execution");
    ASSERT_GT(nodes.size(), 1);
    std::vector<std::shared_ptr<Graph>> graphs;
    std::vector<std::unique_ptr<InferenceRequest>> inferences;
    for (int i = 0; i < 2; i++) {
        auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, nodes[i]);
        ASSERT_NE(graph, nullptr);
        graph->allocateArguments(MemType::SHARED_MEMORY);
        graph->copyInputData();
        for (size_t j = 0; j < graph->inputSize.size(); j++) {
            ASSERT_EQ(ZE_RESULT_SUCCESS,
                      zeContextMakeMemoryResident(zeContext,
                                                  zeDevice,
                                                  graph->inArgs[j],
                                                  graph->inputSize[j]));
        }
        for (size_t j = 0; j < graph->outputSize.size(); j++) {
            ASSERT_EQ(ZE_RESULT_SUCCESS,
                      zeContextMakeMemoryResident(zeContext,
                                                  zeDevice,
                                                  graph->outArgs[j],
                                                  graph->outputSize[j]));
        }
        auto inference = std::make_unique<InferenceRequest>(*graph.get());
        inference->setUpCommandQueue(zeContext, zeDevice);
        inference->setUpCommandList(zeContext, zeDevice, graph->handle, zeGraphDDITableExt);
        inferences.push_back(std::move(inference));
        graphs.push_back(graph);
    }

    for (auto &inference : inferences) {
        ASSERT_EQ(inference->runAsync(), ZE_RESULT_SUCCESS);
        ASSERT_EQ(inference->wait(graphSyncTimeout), ZE_RESULT_SUCCESS);
        ASSERT_TRUE(inference->validateOutput());
    }

    graphs[0]->deallocateAllArguments();
    for (size_t i = 0; i < graphs[0]->inputSize.size(); i++) {
        ASSERT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT,
                  zeContextEvictMemory(zeContext,
                                       zeDevice,
                                       graphs[0]->inArgs[i],
                                       graphs[0]->inputSize[i]));
    }
    for (size_t i = 0; i < graphs[0]->outputSize.size(); i++) {
        ASSERT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT,
                  zeContextEvictMemory(zeContext,
                                       zeDevice,
                                       graphs[0]->outArgs[i],
                                       graphs[0]->outputSize[i]));
    }

    ASSERT_EQ(inferences[1]->runAsync(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(inferences[1]->wait(graphSyncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_TRUE(inferences[1]->validateOutput());
}

TEST_F(MemoryResidency, MultiThreadedInferencesInterleavedWithEvictions) {
    auto nodes = Environment::getConfiguration("graph_execution");
    ASSERT_GT(nodes.size(), 1);
    std::vector<std::shared_ptr<Graph>> graphs;
    std::vector<std::unique_ptr<InferenceRequest>> inferences;
    for (int i = 0; i < 2; i++) {
        auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, nodes[i]);
        ASSERT_NE(graph, nullptr);
        graph->allocateArguments(MemType::SHARED_MEMORY);
        graph->copyInputData();
        for (size_t j = 0; j < graph->inputSize.size(); j++) {
            ASSERT_EQ(ZE_RESULT_SUCCESS,
                      zeContextMakeMemoryResident(zeContext,
                                                  zeDevice,
                                                  graph->inArgs[j],
                                                  graph->inputSize[j]));
        }
        for (size_t j = 0; j < graph->outputSize.size(); j++) {
            ASSERT_EQ(ZE_RESULT_SUCCESS,
                      zeContextMakeMemoryResident(zeContext,
                                                  zeDevice,
                                                  graph->outArgs[j],
                                                  graph->outputSize[j]));
        }
        auto inference = std::make_unique<InferenceRequest>(*graph.get());
        inference->setUpCommandQueue(zeContext, zeDevice);
        inference->setUpCommandList(zeContext, zeDevice, graph->handle, zeGraphDDITableExt);
        inferences.push_back(std::move(inference));
        graphs.push_back(graph);
    }
    size_t bufferSize = 4096;
    std::vector<std::shared_ptr<void>> buffers;
    for (int i = 0; i < 10; i++) {
        buffers.push_back(zeMemory::allocShared(zeContext, zeDevice, bufferSize));
    }

    std::atomic<bool> stop = false;
    std::vector<std::thread> threads;
    for (auto &inference : inferences) {
        threads.emplace_back(
            [&](std::unique_ptr<InferenceRequest> &inference) {
                while (!stop) {
                    ASSERT_EQ(inference->runAsync(), ZE_RESULT_SUCCESS);
                    ASSERT_EQ(inference->wait(graphSyncTimeout), ZE_RESULT_SUCCESS);
                    ASSERT_TRUE(inference->validateOutput());
                }
            },
            std::ref(inference));
    }
    threads.emplace_back([&]() {
        while (!stop) {
            for (auto &buf : buffers) {
                ASSERT_EQ(ZE_RESULT_SUCCESS,
                          zeContextMakeMemoryResident(zeContext, zeDevice, buf.get(), bufferSize));
            }
            for (auto &buf : buffers) {
                ASSERT_EQ(ZE_RESULT_SUCCESS,
                          zeContextEvictMemory(zeContext, zeDevice, buf.get(), bufferSize));
            }
        }
    });

    std::this_thread::sleep_for(2s);
    stop = true;
    for (auto &thread : threads) {
        thread.join();
    }
}

class MemoryResidencyNegative : public UmdTest {
  public:
    void SetUp() {
        UmdTest::SetUp();
        mem = zeMemory::allocShared(zeContext, zeDevice, size);
        ptr = mem.get();
        ze_context_desc_t contextDesc = {ZE_STRUCTURE_TYPE_CONTEXT_DESC, nullptr, 0};
        ze_result_t ret;
        secondContext = zeScope::contextCreate(zeDriver, contextDesc, ret);
        ASSERT_EQ(ZE_RESULT_SUCCESS, ret);
    }

    size_t size = 64;
    std::shared_ptr<void> mem;
    void *ptr;
    zeScope::SharedPtr<ze_context_handle_t> secondContext;
};

TEST_F(MemoryResidencyNegative, InvalidSize) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT,
              zeContextMakeMemoryResident(zeContext, zeDevice, ptr, 0));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT, zeContextEvictMemory(zeContext, zeDevice, ptr, 0));
}

TEST_F(MemoryResidencyNegative, FreedMem) {
    mem = nullptr;
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT,
              zeContextMakeMemoryResident(zeContext, zeDevice, ptr, size));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT,
              zeContextEvictMemory(zeContext, zeDevice, ptr, size));
}

TEST_F(MemoryResidencyNegative, ContextMismatch) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT,
              zeContextMakeMemoryResident(secondContext.get(), zeDevice, ptr, size));
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT,
              zeContextEvictMemory(secondContext.get(), zeDevice, ptr, size));
}
