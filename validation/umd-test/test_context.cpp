/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"
#include "umd_test.h"

#include <functional>
#include <gtest/gtest.h>

class Context : public UmdTest {
  public:
    const size_t size = sizeof(uint64_t);

    ze_context_desc_t contextDesc = {.stype = ZE_STRUCTURE_TYPE_CONTEXT_DESC,
                                     .pNext = nullptr,
                                     .flags = 0};

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
};

TEST_F(Context, CreateContextRepeat) {
    uint32_t ctxs_count = 5;
    ze_result_t ret;

    for (uint32_t i = 0; i < ctxs_count; i++) {
        zeScope::contextCreate(zeDriver, contextDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    }
}

class MultiContext : public Context, public ::testing::WithParamInterface<uint32_t> {
  public:
    void SetUp() override {
        Context::SetUp();

        ze_result_t ret;
        numOfContext = GetParam();
        for (uint32_t i = 0; i < numOfContext; i++) {
            auto scopedContext = zeScope::contextCreate(zeDriver, contextDesc, ret);
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

            ctxs.push_back(std::move(scopedContext));
        }
    }

    void AllocateAndFreeMemory(ze_context_handle_t ctx, size_t size, uint32_t iteration);

    void RunAppendGlobalTimestampAndSynchronize(ze_context_handle_t ctx);

    uint32_t numOfContext;
    std::vector<zeScope::SharedPtr<ze_context_handle_t>> ctxs;
    std::vector<std::shared_ptr<std::thread>> tasks;
    std::vector<std::shared_ptr<void>> mem;
};

INSTANTIATE_TEST_SUITE_P(,
                         MultiContext,
                         ::testing::Values(2, 8),
                         [](const testing::TestParamInfo<uint32_t> &info) {
                             return std::to_string(info.param) + "contexts";
                         });

TEST_P(MultiContext, AllocatedMemoryCannotBeUsedInDifferentContext) {
    ze_result_t ret;

    if (numOfContext % 2)
        numOfContext--;

    for (size_t i = 0; i < numOfContext; i += 2) {
        auto ctx1 = ctxs.at(i).get();
        auto ctx2 = ctxs.at(i + 1).get();

        auto scopedList = zeScope::commandListCreate(ctx1, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        auto list = scopedList.get();

        auto mem = zeMemory::allocHost(ctx2, size);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

        uint64_t *ts = static_cast<uint64_t *>(mem.get());
        ASSERT_NE(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
    }
}

void MultiContext::AllocateAndFreeMemory(ze_context_handle_t ctx, size_t size, uint32_t iteration) {
    for (uint32_t i = 0; i < iteration; i++) {
        auto memHost = zeMemory::allocHost(ctx, size);
        auto memShared = zeMemory::allocShared(ctx, zeDevice, size);
        auto memDevice = zeMemory::allocDevice(ctx, zeDevice, size);
    }
}

TEST_P(MultiContext, AllocateAndFreeMemorySequentially) {
    const size_t memSize = 4096;
    const uint32_t iteration = 4;

    for (uint32_t i = 0; i < numOfContext; i++) {
        AllocateAndFreeMemory(ctxs[i].get(), memSize, iteration);
    }
}

TEST_P(MultiContext, AllocateAndFreeMemorySimultaneously) {
    const size_t memSize = 4096;
    const uint32_t iteration = 4;

    for (uint32_t i = 0; i < numOfContext; i++) {
        tasks.push_back(std::make_shared<std::thread>(&MultiContext::AllocateAndFreeMemory,
                                                      this,
                                                      ctxs[i].get(),
                                                      memSize,
                                                      iteration));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
}

void MultiContext::RunAppendGlobalTimestampAndSynchronize(ze_context_handle_t ctx) {
    ze_result_t ret;

    auto scopedQueue = zeScope::commandQueueCreate(ctx, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ze_command_queue_handle_t queue = scopedQueue.get();

    auto scopedList = zeScope::commandListCreate(ctx, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ze_command_list_handle_t list = scopedList.get();

    auto sharedMem = zeMemory::allocShared(ctx, zeDevice, size);
    uint64_t *ts = static_cast<uint64_t *>(sharedMem.get());
    ASSERT_TRUE(ts) << "Failed to allocate memory";

    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_NE(*ts, 0llu) << "Timestamp should not be 0";
}

TEST_P(MultiContext, AppendGlobalTimestampAndSynchronizeSequentially) {
    for (uint32_t i = 0; i < numOfContext; i++) {
        RunAppendGlobalTimestampAndSynchronize(ctxs[i].get());
    }
}

TEST_P(MultiContext, AppendGlobalTimestampAndSynchronizeSimultaneously) {
    for (uint32_t i = 0; i < numOfContext; i++) {
        tasks.push_back(
            std::make_shared<std::thread>(&MultiContext::RunAppendGlobalTimestampAndSynchronize,
                                          this,
                                          ctxs[i].get()));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
}

class MultiContextGraph : public Context,
                          public ::testing::WithParamInterface<std::tuple<uint32_t, YAML::Node>> {
  public:
    void SetUp() override {
        Context::SetUp();

        auto [numOfContext, node] = GetParam();

        ze_result_t ret;
        for (uint32_t i = 0; i < numOfContext; i++) {
            auto scopedContext = zeScope::contextCreate(zeDriver, contextDesc, ret);
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

            ctxs.push_back(std::move(scopedContext));
        }
    }

    void RunInference(ze_context_handle_t ctx);

    std::vector<zeScope::SharedPtr<ze_context_handle_t>> ctxs;
    std::vector<std::shared_ptr<std::thread>> tasks;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(MultiContextGraph);

INSTANTIATE_TEST_SUITE_P(
    ,
    MultiContextGraph,
    ::testing::Combine(::testing::Values(2, 8),
                       ::testing::ValuesIn(Environment::getConfiguration("graph_execution"))),
    [](const testing::TestParamInfo<std::tuple<uint32_t, YAML::Node>> &info) {
        auto numOfContext = std::get<0>(info.param);
        const YAML::Node node(std::get<1>(info.param));

        return std::to_string(numOfContext) + "Contexts_" + std::to_string(info.index) + "_" +
               generateTestNameFromNode(node);
    });

void MultiContextGraph::RunInference(ze_context_handle_t ctx) {
    ze_result_t ret = ZE_RESULT_SUCCESS;

    auto param = GetParam();
    const YAML::Node node(std::get<1>(param));

    std::shared_ptr<Graph> graph =
        Graph::create(ctx, zeDevice, zeGraphDDITableExt, globalConfig, node);

    graph->allocateArguments(MemType::SHARED_MEMORY);
    graph->copyInputData();

    auto scopedQueue = zeScope::commandQueueCreate(ctx, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ze_command_queue_handle_t queue = scopedQueue.get();

    auto scopedList = zeScope::commandListCreate(ctx, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ze_command_list_handle_t list = scopedList.get();

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

TEST_P(MultiContextGraph, RunGraphInferenceSequentially) {
    auto [numOfContext, _] = GetParam();
    for (uint32_t i = 0; i < numOfContext; i++) {
        RunInference(ctxs[i].get());
    }
}

TEST_P(MultiContextGraph, RunGraphInferenceSimultaneously) {
    auto [numOfContext, _] = GetParam();
    for (uint32_t i = 0; i < numOfContext; i++) {
        tasks.push_back(
            std::make_shared<std::thread>(&MultiContextGraph::RunInference, this, ctxs[i].get()));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
}
