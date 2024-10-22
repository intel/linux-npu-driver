/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"
#include "ze_stringify.hpp"

#include <level_zero/zet_api.h>
#include <vector>

/*test case definition:
 * std::tuple<network definition node, metric group name, queryIndex >
 */
using metricTestCase_t = std::tuple<YAML::Node, std::string, uint32_t>;

class MetricGroup : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();

        ret = zetMetricGroupGet(zeDevice, &metricGroupsCount, nullptr);
        if (ret == ZE_RESULT_ERROR_UNSUPPORTED_FEATURE) {
            SKIP_("Metrics are not supported");
        }
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        ASSERT_GT(metricGroupsCount, 0u);

        metricGroups.resize(metricGroupsCount);
        ASSERT_EQ(zetMetricGroupGet(zeDevice, &metricGroupsCount, metricGroups.data()),
                  ZE_RESULT_SUCCESS);
        EXPECT_NE(*metricGroups.data(), nullptr);
    }

  protected:
    ze_result_t ret = ZE_RESULT_SUCCESS;
    uint32_t metricGroupsCount = 0;
    std::vector<zet_metric_group_handle_t> metricGroups;
};

class Metric : public MetricGroup {
  public:
    void SetUp() override {
        MetricGroup::SetUp();

        groupProperties.resize(metricGroupsCount);
        for (auto &v : groupProperties)
            v.stype = ZET_STRUCTURE_TYPE_METRIC_GROUP_PROPERTIES;

        for (uint8_t i = 0; i < metricGroupsCount; i++) {
            ASSERT_EQ(zetMetricGroupGetProperties(metricGroups[i], &groupProperties[i]),
                      ZE_RESULT_SUCCESS);

            auto metricCount = groupProperties[i].metricCount;

            std::vector<zet_metric_handle_t> metrics(metricCount);

            ASSERT_EQ(zetMetricGet(metricGroups[i], &metricCount, metrics.data()),
                      ZE_RESULT_SUCCESS);

            std::vector<zet_metric_properties_t> metricsProperties;
            for (uint8_t j = 0; j < metricCount; j++) {
                zet_metric_properties_t properties = {};
                properties.stype = ZET_STRUCTURE_TYPE_METRIC_PROPERTIES;

                EXPECT_EQ(zetMetricGetProperties(metrics[j], &properties), ZE_RESULT_SUCCESS);
                metricsProperties.push_back(properties);
            }
            metricsPropertiesAll.push_back(metricsProperties);
        }
    }

  protected:
    std::vector<zet_metric_group_properties_t> groupProperties;
    std::vector<std::vector<zet_metric_properties_t>> metricsPropertiesAll;
};

TEST_F(Metric, GetProperties) {
    for (uint8_t i = 0; i < metricGroupsCount; i++) {
        zet_metric_group_properties_t &groupProp = groupProperties[i];

        TRACE("MetricGroup[%i].name: %s\n", i, groupProp.name);
        TRACE("MetricGroup[%i].description: %s\n", i, groupProp.description);
        TRACE("MetricGroup[%i].samplingType: %i\n", i, groupProp.samplingType);
        TRACE("MetricGroup[%i].domain: %i\n", i, groupProp.domain);
        TRACE("MetricGroup[%i].metricCount: %i\n", i, groupProp.metricCount);

        EXPECT_TRUE((groupProp.samplingType & ZET_METRIC_GROUP_SAMPLING_TYPE_FLAG_EVENT_BASED) &&
                    (groupProp.samplingType & ZET_METRIC_GROUP_SAMPLING_TYPE_FLAG_TIME_BASED));
        EXPECT_GT(groupProp.domain, 0u);
        EXPECT_GT(groupProp.metricCount, 0u);

        for (uint8_t j = 0; j < groupProp.metricCount; j++) {
            zet_metric_properties_t &metricProp = metricsPropertiesAll[i][j];

            TRACE("\tMetric[%i].name: %s\n", j, metricProp.name);
            TRACE("\tMetric[%i].description: %s\n", j, metricProp.description);
            TRACE("\tMetric[%i].component: %s\n", j, metricProp.component);
            TRACE("\tMetric[%i].tierNumber: %i\n", j, metricProp.tierNumber);
            TRACE("\tMetric[%i].metricType: %i\n", j, metricProp.metricType);
            TRACE("\tMetric[%i].resultType: %s (%i)\n",
                  j,
                  zet_value_type_to_str(metricProp.resultType),
                  metricProp.resultType);
            TRACE("\tMetric[%i].resultUnits: %s\n", j, metricProp.resultUnits);
        }
    }
}

class MetricQueryPool : public MetricGroup {
  public:
    void SetUp() override { MetricGroup::SetUp(); }

  protected:
    zet_metric_query_pool_handle_t hMetricQueryPool = nullptr;
};

TEST_F(MetricQueryPool, ActivateAndCreateMetricQueryPool) {
    EXPECT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 1u, &metricGroups[0]),
              ZE_RESULT_SUCCESS);

    const zet_metric_query_pool_desc_t desc = {.stype = ZET_STRUCTURE_TYPE_METRIC_QUERY_POOL_DESC,
                                               .pNext = nullptr,
                                               .type = ZET_METRIC_QUERY_POOL_TYPE_PERFORMANCE,
                                               .count = 1u};
    EXPECT_EQ(
        zetMetricQueryPoolCreate(zeContext, zeDevice, metricGroups[0], &desc, &hMetricQueryPool),
        ZE_RESULT_SUCCESS);

    EXPECT_EQ(zetMetricQueryPoolDestroy(hMetricQueryPool), ZE_RESULT_SUCCESS);

    EXPECT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 0u, nullptr), ZE_RESULT_SUCCESS);
}

TEST_F(MetricQueryPool, ActivateAndCreateMetricQuery) {
    ASSERT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 1u, &metricGroups[0]),
              ZE_RESULT_SUCCESS);

    const zet_metric_query_pool_desc_t desc = {.stype = ZET_STRUCTURE_TYPE_METRIC_QUERY_POOL_DESC,
                                               .pNext = nullptr,
                                               .type = ZET_METRIC_QUERY_POOL_TYPE_PERFORMANCE,
                                               .count = 1u};

    ASSERT_EQ(
        zetMetricQueryPoolCreate(zeContext, zeDevice, metricGroups[0], &desc, &hMetricQueryPool),
        ZE_RESULT_SUCCESS);

    zet_metric_query_handle_t hMetricQuery;
    EXPECT_EQ(zetMetricQueryCreate(hMetricQueryPool, 0u, &hMetricQuery), ZE_RESULT_SUCCESS);

    EXPECT_EQ(zetMetricQueryDestroy(hMetricQuery), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zetMetricQueryPoolDestroy(hMetricQueryPool), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 0u, nullptr), ZE_RESULT_SUCCESS);
}

class MetricQuery : public Metric, public ::testing::WithParamInterface<metricTestCase_t> {
  public:
    void SetUp() override { Metric::SetUp(); }

    uint32_t findMetricGroupIndex(std::string groupName) {
        uint32_t index;
        for (index = 0; index < groupProperties.size(); index++) {
            if (groupName == groupProperties[index].name)
                break;
        }
        EXPECT_LT(index, groupProperties.size());
        return index;
    }

    void TearDown() override {
        if (query) {
            ASSERT_EQ(zetMetricQueryDestroy(query), ZE_RESULT_SUCCESS);
        }

        if (pool) {
            ASSERT_EQ(zetMetricQueryPoolDestroy(pool), ZE_RESULT_SUCCESS);
        }

        ASSERT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 0u, nullptr),
                  ZE_RESULT_SUCCESS);

        Metric::TearDown();
    }

    /* Functions returns combinations of network and defined
     * for this network metric groups and queryIndex.
     */
    static std::vector<metricTestCase_t>
    createCasesForMetricsTest(std::vector<uint32_t> &queryIndexes) {
        std::vector<metricTestCase_t> combinations;
        try {
            std::vector<YAML::Node> networkList = Environment::getConfiguration("graph_metrics");

            if (networkList.empty())
                return combinations;

            for (auto &network : networkList)
                for (auto &groupName : network["metric_groups"].as<std::vector<std::string>>())
                    for (auto index : queryIndexes)
                        combinations.push_back({network, groupName, index});

            return combinations;
        } catch (YAML::Exception &e) {
            PRINTF("Bad node: Reason: %s\n", e.what());
            combinations.clear();
            return combinations;
        }
    }

    void MetricInitialize(uint8_t groupIndex, uint8_t queryIndex) {
        ASSERT_EQ(
            zetContextActivateMetricGroups(zeContext, zeDevice, 1u, &metricGroups[groupIndex]),
            ZE_RESULT_SUCCESS);

        ASSERT_EQ(
            zetMetricQueryPoolCreate(zeContext, zeDevice, metricGroups[groupIndex], &desc, &pool),
            ZE_RESULT_SUCCESS);
        ASSERT_NE(pool, nullptr);

        ASSERT_EQ(zetMetricQueryCreate(pool, queryIndex, &query), ZE_RESULT_SUCCESS);
        ASSERT_NE(query, nullptr);

        ze_command_queue_desc_t cmdQueueDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                                .pNext = nullptr,
                                                .ordinal = 0u,
                                                .index = 0,
                                                .flags = 0,
                                                .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                                .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};
        ze_command_list_desc_t cmdListDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                              .pNext = nullptr,
                                              .commandQueueGroupOrdinal = 0u,
                                              .flags = 0};

        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();
    }

    zet_metric_query_pool_desc_t desc = {.stype = ZET_STRUCTURE_TYPE_METRIC_QUERY_POOL_DESC,
                                         .pNext = nullptr,
                                         .type = ZET_METRIC_QUERY_POOL_TYPE_PERFORMANCE,
                                         .count = 2u};
    zet_metric_query_pool_handle_t pool = nullptr;
    zet_metric_query_handle_t query = nullptr;
    uint8_t testedMetricIndex = 0xff;

    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;
};

TEST_F(MetricQuery, RunMetricQueryOnEmptyCommandList) {
    MetricInitialize(0u, 0u);

    EXPECT_EQ(zetCommandListAppendMetricQueryBegin(list, query), ZE_RESULT_SUCCESS);

    EXPECT_EQ(zetCommandListAppendMetricQueryEnd(list, query, nullptr, 0u, nullptr),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
}

TEST_F(MetricQuery, MetricGroupCalculateEmptyMetricQuery) {
    size_t groupIndex = 1;
    MetricInitialize(groupIndex, 0);

    size_t queryDataSize = 0u;
    EXPECT_EQ(zetMetricQueryGetData(query, &queryDataSize, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_GT(queryDataSize, 0u);

    std::vector<uint8_t> queryRawData(queryDataSize, 0u);
    EXPECT_EQ(zetMetricQueryGetData(query,
                                    &queryDataSize,
                                    reinterpret_cast<uint8_t *>(queryRawData.data())),
              ZE_RESULT_SUCCESS);

    TRACE_BUF(queryRawData.data(), queryDataSize);
    EXPECT_EQ(queryRawData[0], 0u);

    uint32_t metricValueCount = 0;
    EXPECT_EQ(zetMetricGroupCalculateMetricValues(metricGroups[groupIndex],
                                                  ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                                  queryRawData.size(),
                                                  queryRawData.data(),
                                                  &metricValueCount,
                                                  nullptr),
              ZE_RESULT_SUCCESS);

    EXPECT_GT(metricValueCount, 0);

    std::vector<zet_typed_value_t> metricValues(metricValueCount);
    EXPECT_EQ(zetMetricGroupCalculateMetricValues(metricGroups[groupIndex],
                                                  ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                                  queryRawData.size(),
                                                  queryRawData.data(),
                                                  &metricValueCount,
                                                  metricValues.data()),
              ZE_RESULT_SUCCESS);

    for (uint32_t i = 0; i < metricValueCount; i++) {
        EXPECT_EQ(metricValues[i].type, metricsPropertiesAll[groupIndex][i].resultType);
        EXPECT_EQ(metricValues[i].value.ui64, 0llu);
    }

    TRACE_BUF(metricValues.data(), metricValues.size() * sizeof(zet_typed_value_t));
}

std::vector<uint32_t> queryIndexesComputeEngine = {0, 1};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(MetricQuery);

INSTANTIATE_TEST_SUITE_P(
    ,
    MetricQuery,
    ::testing::ValuesIn(MetricQuery::createCasesForMetricsTest(queryIndexesComputeEngine)),
    [](const testing::TestParamInfo<metricTestCase_t> &p) {
        auto node = std::get<0>(p.param);
        auto metricGroupName = std::get<1>(p.param);
        auto queryIndex = std::get<2>(p.param);

        return generateTestNameFromNode(node) + "_" + metricGroupName + "_OnIndex" +
               std::to_string(queryIndex);
    });

TEST_P(MetricQuery, GetDataValueCheck) {
    auto &[node, metricGroupName, queryIndex] = GetParam();

    std::shared_ptr<Graph> graph =
        Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

    graph->allocateArguments(MemType::SHARED_MEMORY);
    graph->copyInputData();

    uint32_t groupIndex = findMetricGroupIndex(metricGroupName);

    MetricInitialize(groupIndex, queryIndex);

    ASSERT_EQ(zetCommandListAppendMetricQueryBegin(list, query), ZE_RESULT_SUCCESS);
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendBarrier(list, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zetCommandListAppendMetricQueryEnd(list, query, nullptr, 0u, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    size_t queryDataSize = 0u;
    EXPECT_EQ(zetMetricQueryGetData(query, &queryDataSize, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_GT(queryDataSize, 0u);

    std::vector<uint64_t> queryRawData(queryDataSize / sizeof(uint64_t), 0u);
    EXPECT_EQ(zetMetricQueryGetData(query,
                                    &queryDataSize,
                                    reinterpret_cast<uint8_t *>(queryRawData.data())),
              ZE_RESULT_SUCCESS);

    TRACE_BUF(queryRawData.data(), queryDataSize);

    EXPECT_GT(queryRawData[0], 0u);

    uint32_t metricValueCount = 0;
    EXPECT_EQ(zetMetricGroupCalculateMetricValues(metricGroups[groupIndex],
                                                  ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                                  queryDataSize,
                                                  reinterpret_cast<uint8_t *>(queryRawData.data()),
                                                  &metricValueCount,
                                                  nullptr),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(metricValueCount, groupProperties[groupIndex].metricCount);

    std::vector<zet_typed_value_t> metricValues(metricValueCount);
    EXPECT_EQ(zetMetricGroupCalculateMetricValues(metricGroups[groupIndex],
                                                  ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                                  queryDataSize,
                                                  reinterpret_cast<uint8_t *>(queryRawData.data()),
                                                  &metricValueCount,
                                                  metricValues.data()),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(metricValues[0].type, metricsPropertiesAll[groupIndex][0].resultType);

    EXPECT_GT(metricValues[0].value.ui64, 0llu);

    TRACE_BUF(metricValues.data(), metricValues.size() * sizeof(zet_typed_value_t));

    for (uint32_t i = 0; i < metricValueCount; i++) {
        TRACE("Metric %lu -> type: %#x, value: %lu\n",
              static_cast<unsigned long>(i),
              metricValues[i].type,
              metricValues[i].value.ui64);
    }
}

class MetricQueryMemoryCopy : public MetricQuery {
  public:
    void SetUp() override { Metric::SetUp(); }

    void TearDown() override { MetricQuery::TearDown(); }

    /* Functions returns combinations of network and defined
     * for this network metric groups and queryIndex.
     */
    static std::vector<metricTestCase_t>
    createCasesForMetricsTest(std::vector<uint32_t> &queryIndexes) {
        std::vector<metricTestCase_t> combinations;
        try {
            std::vector<YAML::Node> network = Environment::getConfiguration("graph_metrics");

            if (network.empty())
                return combinations;

            for (auto index : queryIndexes)
                combinations.push_back({network[0], std::string("NOC"), index});

            return combinations;
        } catch (YAML::Exception &e) {
            PRINTF("Bad node: Reason: %s\n", e.what());
            combinations.clear();
            return combinations;
        }
    }
};

std::vector<uint32_t> queryIndexes = {0};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(MetricQueryMemoryCopy);

INSTANTIATE_TEST_SUITE_P(
    ,
    MetricQueryMemoryCopy,
    ::testing::ValuesIn(MetricQueryMemoryCopy::createCasesForMetricsTest(queryIndexes)),
    [](const testing::TestParamInfo<metricTestCase_t> &p) {
        auto metricGroupName = std::get<1>(p.param);
        auto queryIndex = std::get<2>(p.param);
        return metricGroupName + "_OnIndex" + std::to_string(queryIndex);
    });

TEST_P(MetricQueryMemoryCopy, GetDataValue) {
    auto &[node, metricGroupName, queryIndex] = GetParam();
    const size_t allocSize = 2048 * 1024;

    std::shared_ptr<void> srcMem, dstMem;
    srcMem = AllocSharedMemory(allocSize);
    dstMem = AllocSharedMemory(allocSize);

    uint32_t groupIndex = findMetricGroupIndex(metricGroupName);
    MetricInitialize(groupIndex, queryIndex);

    ASSERT_EQ(zetCommandListAppendMetricQueryBegin(list, query), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            dstMem.get(),
                                            srcMem.get(),
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendBarrier(list, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zetCommandListAppendMetricQueryEnd(list, query, nullptr, 0u, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    size_t queryDataSize = 0u;
    EXPECT_EQ(zetMetricQueryGetData(query, &queryDataSize, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_GT(queryDataSize, 0u);

    std::vector<uint64_t> queryRawData(queryDataSize / sizeof(uint64_t), 0u);
    EXPECT_EQ(zetMetricQueryGetData(query,
                                    &queryDataSize,
                                    reinterpret_cast<uint8_t *>(queryRawData.data())),
              ZE_RESULT_SUCCESS);

    TRACE_BUF(queryRawData.data(), queryDataSize);

    EXPECT_GT(queryRawData[0], 0u);

    uint32_t metricValueCount = 0;
    EXPECT_EQ(zetMetricGroupCalculateMetricValues(metricGroups[groupIndex],
                                                  ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                                  queryDataSize,
                                                  reinterpret_cast<uint8_t *>(queryRawData.data()),
                                                  &metricValueCount,
                                                  nullptr),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(metricValueCount, groupProperties[groupIndex].metricCount);

    std::vector<zet_typed_value_t> metricValues(metricValueCount);
    EXPECT_EQ(zetMetricGroupCalculateMetricValues(metricGroups[groupIndex],
                                                  ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                                  queryDataSize,
                                                  reinterpret_cast<uint8_t *>(queryRawData.data()),
                                                  &metricValueCount,
                                                  metricValues.data()),
              ZE_RESULT_SUCCESS);

    TRACE_BUF(metricValues.data(), metricValues.size() * sizeof(zet_typed_value_t));

    EXPECT_EQ(metricValues[0].type, metricsPropertiesAll[groupIndex][0].resultType);

    EXPECT_GT(metricValues[0].value.ui64, 0llu);
}
