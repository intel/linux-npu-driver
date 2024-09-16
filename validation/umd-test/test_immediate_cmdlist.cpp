/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

#include <future>

class ImmediateCmdList : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();

        scopedList = zeScope::immediateCommandListCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();
        ASSERT_NE(list, nullptr);
    }

    ze_command_queue_desc_t cmdQueueDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                         .pNext = nullptr,
                                         .ordinal = 0,
                                         .index = 0,
                                         .flags = 0,
                                         .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                         .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};

    ze_command_list_handle_t list = nullptr;

  protected:
    const size_t size = sizeof(uint64_t);
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList;
    ze_result_t ret;
};

TEST_F(ImmediateCmdList, CreateCloseResetDestroy) {
    ze_bool_t isImmediate = false;
    ASSERT_EQ(zeCommandListIsImmediate(list, &isImmediate), ZE_RESULT_SUCCESS);
    ASSERT_EQ(isImmediate, true);

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_ERROR_UNINITIALIZED);
    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);
}

TEST_F(ImmediateCmdList, CreateAndTryExecute) {
    auto queue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto mem = AllocSharedMemory(size);
    ASSERT_TRUE(mem.get());

    uint64_t *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(zeCommandQueueExecuteCommandLists(queue.get(), 1, &list, nullptr),
              ZE_RESULT_ERROR_UNINITIALIZED);
}

TEST_F(ImmediateCmdList, GetTimestampsUsingSingleList) {
    auto mem = AllocSharedMemory(size);
    ASSERT_TRUE(mem.get()) << "Failed to allocate shared memory";

    auto mem2 = AllocSharedMemory(size);
    ASSERT_TRUE(mem2.get()) << "Failed to allocate shared memory";

    uint64_t *ts1 = static_cast<uint64_t *>(mem.get());
    *ts1 = 0ULL;
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts1, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    uint64_t *ts2 = static_cast<uint64_t *>(mem2.get());
    *ts2 = 0ULL;
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts2, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListHostSynchronize(list, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_LT(*ts1, *ts2);
}

TEST_F(ImmediateCmdList, GetTimestampsUsingTwoLists) {
    auto mem = AllocSharedMemory(size);
    ASSERT_TRUE(mem.get()) << "Failed to allocate shared memory";

    auto mem2 = AllocSharedMemory(size);
    ASSERT_TRUE(mem2.get()) << "Failed to allocate shared memory";

    auto scopedList2 = zeScope::immediateCommandListCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto list2 = scopedList2.get();

    uint64_t *ts1 = static_cast<uint64_t *>(mem.get());
    *ts1 = 0ULL;
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts1, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    uint64_t *ts2 = static_cast<uint64_t *>(mem2.get());
    *ts2 = 0ULL;
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list2, ts2, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListHostSynchronize(list, syncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListHostSynchronize(list2, syncTimeout), ZE_RESULT_SUCCESS);

    EXPECT_LT(*ts1, *ts2);
}

TEST_F(ImmediateCmdList, GetTimestampAndCopy) {
    auto mem = AllocSharedMemory(size);
    ASSERT_TRUE(mem.get()) << "Failed to allocate shared memory";

    auto mem2 = AllocSharedMemory(size);
    ASSERT_TRUE(mem2.get()) << "Failed to allocate shared memory";

    uint64_t *ts1 = static_cast<uint64_t *>(mem.get());
    uint64_t *ts2 = static_cast<uint64_t *>(mem2.get());
    *ts1 = 0ULL;
    *ts2 = 0ULL;
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts1, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListAppendMemoryCopy(list, ts2, ts1, size, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListHostSynchronize(list, syncTimeout), ZE_RESULT_SUCCESS);

    EXPECT_NE(*ts1, 0llu);
    EXPECT_EQ(*ts1, *ts2);
}

TEST_F(ImmediateCmdList, FillCopyUsingBarriers) {
    const size_t allocSize = 16 * KB;
    auto mem1 = AllocSharedMemory(allocSize);
    auto mem2 = AllocSharedMemory(allocSize);
    auto mem3 = AllocSharedMemory(allocSize);

    auto scopedList2 = zeScope::immediateCommandListCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto list2 = scopedList2.get();

    // Command flow:
    // cmdlist1 Fill(mem1)/Copy(mem1->mem2)
    // cmdlist2:Barier/Copy(mem2->mem3)

    char pattern = 0xAB;
    ASSERT_EQ(zeCommandListAppendMemoryFill(list,
                                            mem1.get(),
                                            &pattern,
                                            sizeof(pattern),
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(
        zeCommandListAppendMemoryCopy(list, mem2.get(), mem1.get(), allocSize, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);

    std::future<_ze_result_t> copyFillResult = std::async(std::launch::async, [&] {
        ze_result_t result;
        result = zeCommandListAppendBarrier(list2, nullptr, 0, nullptr);
        if (result != ZE_RESULT_SUCCESS)
            return result;
        result = zeCommandListAppendMemoryCopy(list2,
                                               mem3.get(),
                                               mem2.get(),
                                               allocSize,
                                               nullptr,
                                               0,
                                               nullptr);
        if (result != ZE_RESULT_SUCCESS)
            return result;
        return zeCommandListHostSynchronize(list2, syncTimeout);
    });
    ASSERT_EQ(copyFillResult.get(), ZE_RESULT_SUCCESS);
    EXPECT_EQ(memcmp(mem3.get(), mem1.get(), allocSize), 0);
}

TEST_F(ImmediateCmdList, MetricQuerryTest) {
    zet_metric_query_pool_desc_t desc = {.stype = ZET_STRUCTURE_TYPE_METRIC_QUERY_POOL_DESC,
                                         .pNext = nullptr,
                                         .type = ZET_METRIC_QUERY_POOL_TYPE_PERFORMANCE,
                                         .count = 2u};

    std::vector<zet_metric_group_handle_t> metricGroups;
    uint32_t metricGroupsCount = 0;
    zet_metric_query_pool_handle_t pool = nullptr;
    zet_metric_query_handle_t query = nullptr;
    ret = zetMetricGroupGet(zeDevice, &metricGroupsCount, nullptr);
    if (ret == ZE_RESULT_ERROR_UNSUPPORTED_FEATURE) {
        SKIP_("Metrics are not supported");
    }
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_GT(metricGroupsCount, 0u);

    metricGroups.resize(metricGroupsCount);
    ASSERT_EQ(zetMetricGroupGet(zeDevice, &metricGroupsCount, metricGroups.data()),
              ZE_RESULT_SUCCESS);
    ASSERT_NE(*metricGroups.data(), nullptr);

    const size_t allocSize = 16 * KB;
    auto mem1 = AllocSharedMemory(allocSize);
    auto mem2 = AllocSharedMemory(allocSize);

    EXPECT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 1u, &metricGroups[0]),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(zetMetricQueryPoolCreate(zeContext, zeDevice, metricGroups[0], &desc, &pool),
              ZE_RESULT_SUCCESS);
    EXPECT_NE(pool, nullptr);

    EXPECT_EQ(zetMetricQueryCreate(pool, 0, &query), ZE_RESULT_SUCCESS);
    EXPECT_NE(query, nullptr);

    EXPECT_EQ(zetCommandListAppendMetricQueryBegin(list, query), ZE_RESULT_SUCCESS);

    EXPECT_EQ(
        zeCommandListAppendMemoryCopy(list, mem2.get(), mem1.get(), allocSize, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);

    EXPECT_EQ(zeCommandListAppendBarrier(list, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);

    EXPECT_EQ(zetCommandListAppendMetricQueryEnd(list, query, nullptr, 0u, nullptr),
              ZE_RESULT_SUCCESS);

    if (query) {
        EXPECT_EQ(zetMetricQueryDestroy(query), ZE_RESULT_SUCCESS);
    }

    if (pool) {
        EXPECT_EQ(zetMetricQueryPoolDestroy(pool), ZE_RESULT_SUCCESS);
    }

    EXPECT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 0u, nullptr), ZE_RESULT_SUCCESS);
}

class ImmediateCmdListInference : public ImmediateCmdList,
                                  public ::testing::WithParamInterface<YAML::Node> {
  public:
    void SetUp() override {
        ImmediateCmdList::SetUp();

        const YAML::Node node = GetParam();

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

        graph->allocateArguments(MemType::SHARED_MEMORY);
        graph->copyInputData();
    }

    std::shared_ptr<Graph> graph;
    zeScope::SharedPtr<ze_graph_handle_t> scopedGraphHandle = nullptr;
    ze_graph_desc_2_t graphDesc = {};
    std::vector<uint8_t> modelIR = {};
    std::vector<char> buildFlags = {};
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(ImmediateCmdListInference);

INSTANTIATE_TEST_SUITE_P(
    ,
    ImmediateCmdListInference,
    ::testing::ValuesIn(Environment::getConfiguration("image_classification_imagenet")),
    [](const testing::TestParamInfo<YAML::Node> &p) { return generateTestNameFromNode(p.param); });

TEST_P(ImmediateCmdListInference, CompileModelAndExecute) {
    ze_bool_t isImmediate = false;
    ASSERT_EQ(zeCommandListIsImmediate(list, &isImmediate), ZE_RESULT_SUCCESS);
    ASSERT_NE(isImmediate, false);
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListHostSynchronize(list, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}