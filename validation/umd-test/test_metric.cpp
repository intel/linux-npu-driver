/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <vector>

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

TEST_F(MetricGroup, RetrieveMetricGroupProperties) {
    std::vector<zet_metric_group_properties_t> properties(metricGroupsCount);

    for (auto &v : properties)
        v.stype = ZET_STRUCTURE_TYPE_METRIC_GROUP_PROPERTIES;

    // Retrieve all metric group properties and compare
    for (uint8_t i = 0; i < metricGroupsCount; i++) {
        EXPECT_EQ(zetMetricGroupGetProperties(metricGroups[i], &properties[i]), ZE_RESULT_SUCCESS);

        EXPECT_TRUE(
            (properties[i].samplingType & ZET_METRIC_GROUP_SAMPLING_TYPE_FLAG_EVENT_BASED) &&
            (properties[i].samplingType & ZET_METRIC_GROUP_SAMPLING_TYPE_FLAG_TIME_BASED));
        EXPECT_GT(properties[i].domain, 0u);
        EXPECT_GT(properties[i].metricCount, 0u);
    }
}

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

TEST_F(Metric, ValidatePropertiesForMetric) {
    // Group index 0, first metric
    EXPECT_GT(metricsPropertiesAll[0][0].metricType, 0);
    EXPECT_GT(metricsPropertiesAll[0][0].resultType, 0);
    EXPECT_GE(metricsPropertiesAll[0][0].tierNumber, 0);
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

struct MetricQueryExecParam {
    std::string groupName;
    uint8_t groupIndex;
    uint8_t queryIndex;
};

class MetricQuery
    : public Metric,
      public ::testing::WithParamInterface<std::tuple<YAML::Node, MetricQueryExecParam>> {
    void SetUp() override {
        Metric::SetUp();

        YAML::Node &configuration = Environment::getConfiguration();
        if (configuration["blob_dir"].IsDefined())
            blobDir = configuration["blob_dir"].as<std::string>();
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

  protected:
    zet_metric_query_pool_desc_t desc = {.stype = ZET_STRUCTURE_TYPE_METRIC_QUERY_POOL_DESC,
                                         .pNext = nullptr,
                                         .type = ZET_METRIC_QUERY_POOL_TYPE_PERFORMANCE,
                                         .count = 2u};
    zet_metric_query_pool_handle_t pool = nullptr;
    zet_metric_query_handle_t query = nullptr;

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
    std::string blobDir = "";

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

        ze_result_t ret;
        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();
    }

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
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

TEST_F(MetricQuery, RunMetricQueryOnEmptyCommandList_CopyEngine) {
    cmdQueueDesc.ordinal = 1;
    cmdListDesc.commandQueueGroupOrdinal = 1;
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
}
GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(MetricQuery);

INSTANTIATE_TEST_SUITE_P(
    ,
    MetricQuery,
    ::testing::Combine(::testing::ValuesIn(Environment::getConfiguration("graph_metrics")),
                       ::testing::Values(MetricQueryExecParam{"ShaveIL1Cache", 0u, 0u},
                                         MetricQueryExecParam{"ShaveIL1Cache", 0u, 1u},
                                         MetricQueryExecParam{"ShaveDL1Cache", 1u, 0u},
                                         MetricQueryExecParam{"ShaveDL1Cache", 1u, 1u},
                                         MetricQueryExecParam{"ShaveL2Cache", 2u, 0u},
                                         MetricQueryExecParam{"ShaveL2Cache", 2u, 1u},
                                         MetricQueryExecParam{"NOC", 3u, 0u},
                                         MetricQueryExecParam{"NOC", 3u, 1u})),
    [](const testing::TestParamInfo<std::tuple<YAML::Node, MetricQueryExecParam>> &info) {
        auto node = std::get<0>(info.param);
        auto metricParam = std::get<1>(info.param);
        std::string postfix = metricParam.groupName;
        postfix += (metricParam.queryIndex != 0u)
                       ? "OnIndex" + std::to_string(metricParam.queryIndex)
                       : "";
        return generateTestNameFromNode(node) + postfix;
    });

TEST_P(MetricQuery, GetDataValueCheck) {
    auto &[node, metricParam] = GetParam();

    if (!isSilicon() && metricParam.groupName == "NOC")
        SKIP_("Feature not supported");

    std::vector<std::vector<char>> inputBin, outputBin;
    std::vector<char> vpuBlob, vpuBin;

    ASSERT_TRUE(getBlobFromPath(blobDir + node["path"].as<std::string>(),
                                node["in"].as<std::vector<std::string>>(),
                                node["out"].as<std::vector<std::string>>(),
                                vpuBlob,
                                inputBin,
                                outputBin,
                                vpuBin));
    bool checkShaveCounters = false;
    if (node["act_shave_tasks"].IsDefined())
        checkShaveCounters = node["act_shave_tasks"].as<bool>();

    MetricInitialize(metricParam.groupIndex, metricParam.queryIndex);

    ze_activation_kernel_desc_t actKernelDesc = {};
    if (!vpuBin.empty()) {
        actKernelDesc = {.stype = ZE_STRUCTURE_TYPE_GRAPH_ACTIVATION_KERNEL,
                         .pNext = nullptr,
                         .kernelDataSize = vpuBin.size(),
                         .pKernelData = reinterpret_cast<uint8_t *>(vpuBin.data())};
    }

    const ze_graph_desc_t graphDesc = {.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES,
                                       .pNext = !vpuBin.empty() ? &actKernelDesc : nullptr,
                                       .format = ZE_GRAPH_FORMAT_NATIVE,
                                       .inputSize = vpuBlob.size(),
                                       .pInput = reinterpret_cast<uint8_t *>(vpuBlob.data()),
                                       .pBuildFlags = nullptr};

    ze_result_t ret;
    auto scopedGraphHandle =
        zeScope::graphCreate(zeGraphDDITableExt, zeContext, zeDevice, graphDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto graphHandle = scopedGraphHandle.get();

    std::vector<std::shared_ptr<void>> mem;
    std::vector<void *> graphInput, graphOutput;
    for (const auto &s : inputBin) {
        mem.push_back(AllocHostMemory(s.size()));
        graphInput.push_back(mem.back().get());
    }

    for (const auto &s : outputBin) {
        mem.push_back(AllocHostMemory(s.size()));
        graphOutput.push_back(mem.back().get());
    }

    for (size_t i = 0; i < graphInput.size(); i++) {
        memcpy(graphInput[i], inputBin[i].data(), inputBin[i].size());
    }

    uint32_t argIndex = 0;
    for (const auto &s : graphInput) {
        ASSERT_EQ(zeGraphDDITableExt->pfnSetArgumentValue(graphHandle, argIndex++, s),
                  ZE_RESULT_SUCCESS);
    }
    for (const auto &s : graphOutput) {
        ASSERT_EQ(zeGraphDDITableExt->pfnSetArgumentValue(graphHandle, argIndex++, s),
                  ZE_RESULT_SUCCESS);
    }

    ASSERT_EQ(zetCommandListAppendMetricQueryBegin(list, query), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphInitialize(list, graphHandle, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphExecute(list, graphHandle, nullptr, nullptr, 0, nullptr),
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
    if (metricParam.groupName == "NOC" || checkShaveCounters) {
        // TODO: Temporary WA to 0 value in hit counter
        if (metricParam.groupName == "ShaveDL1Cache") {
            ASSERT_GT(queryRawData.size(), 1);
            EXPECT_TRUE(queryRawData[0] > 0 || queryRawData[1] > 0);
        } else {
            EXPECT_GT(queryRawData[0], 0u);
        }
    } else {
        EXPECT_EQ(queryRawData[0], 0u);
    }

    uint32_t metricValueCount = 0;
    EXPECT_EQ(zetMetricGroupCalculateMetricValues(metricGroups[metricParam.groupIndex],
                                                  ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                                  queryRawData.size(),
                                                  reinterpret_cast<uint8_t *>(queryRawData.data()),
                                                  &metricValueCount,
                                                  nullptr),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(metricValueCount, groupProperties[metricParam.groupIndex].metricCount);

    std::vector<zet_typed_value_t> metricValues(metricValueCount);
    EXPECT_EQ(zetMetricGroupCalculateMetricValues(metricGroups[metricParam.groupIndex],
                                                  ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                                  queryRawData.size(),
                                                  reinterpret_cast<uint8_t *>(queryRawData.data()),
                                                  &metricValueCount,
                                                  metricValues.data()),
              ZE_RESULT_SUCCESS);

    EXPECT_EQ(metricValues[0].type, metricsPropertiesAll[metricParam.groupIndex][0].resultType);
    if (metricParam.groupName == "NOC" || checkShaveCounters) {
        // TODO: Temporary WA to 0 value in hit counter
        if (metricParam.groupName == "ShaveDL1Cache") {
            ASSERT_GT(metricValues.size(), 1u);
            EXPECT_TRUE(metricValues[0].value.ui64 > 0llu || metricValues[1].value.ui64 > 0llu);
        } else {
            EXPECT_GT(metricValues[0].value.ui64, 0llu);
        }
    } else {
        EXPECT_EQ(metricValues[0].value.ui64, 0llu);
    }
}
