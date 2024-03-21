/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

#include <future>
#include <vector>

/*test case definition:
 * std::tuple<network definition node, metric group name, required inference execution time>
 */
using metricTestCase_t = std::tuple<YAML::Node, std::string, uint32_t>;

class MetricStreamer : public UmdTest, public ::testing::WithParamInterface<metricTestCase_t> {
  public:
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

    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList;
    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;
    bool useCopyOrdinal = false;

    void SetUp() override {
        UmdTest::SetUp();

        if (useCopyOrdinal) {
            cmdQueueDesc.ordinal = UmdTest::copyGrpOrdinal;
            cmdListDesc.commandQueueGroupOrdinal = UmdTest::copyGrpOrdinal;
        }

        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();

        std::vector<zet_metric_group_handle_t> metricGroups;
        uint32_t metricGroupsCount = 0;

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

        auto [node, metricGroupName, execTime] = GetParam();

        for (uint32_t i = 0; i < metricGroupsCount; i++) {
            metricGroupProperties = {};
            metricGroupProperties.stype = ZET_STRUCTURE_TYPE_METRIC_GROUP_PROPERTIES;

            ASSERT_EQ(zetMetricGroupGetProperties(metricGroups[i], &metricGroupProperties),
                      ZE_RESULT_SUCCESS);
            if (metricGroupName == metricGroupProperties.name) {
                testedMetricGroup = metricGroups[i];
                break;
            }
        }

        ASSERT_NE(testedMetricGroup, nullptr);

        testedMetricCount = metricGroupProperties.metricCount;
        ASSERT_GT(testedMetricCount, 0);

        std::vector<zet_metric_handle_t> metrics(testedMetricCount);

        ASSERT_EQ(zetMetricGet(testedMetricGroup, &testedMetricCount, metrics.data()),
                  ZE_RESULT_SUCCESS);

        for (uint8_t i = 0; i < testedMetricCount; i++) {
            zet_metric_properties_t properties = {};
            properties.stype = ZET_STRUCTURE_TYPE_METRIC_PROPERTIES;

            EXPECT_EQ(zetMetricGetProperties(metrics[i], &properties), ZE_RESULT_SUCCESS);
            metricsProperties.push_back(properties);
        }
    }

    void TearDown() override {
        if (hMetricStreamer) {
            ASSERT_EQ(zetMetricStreamerClose(hMetricStreamer), ZE_RESULT_SUCCESS);
            ASSERT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 0u, nullptr),
                      ZE_RESULT_SUCCESS);
        }
        UmdTest::TearDown();
    }

    /* Functions returns combinations of network and defined
     * for this network metric groups and inference execution time
     * required to gather metrics.
     */
    static std::vector<metricTestCase_t>
    createCasesForMetricsTest(std::vector<uint32_t> &executionTime) {
        std::vector<metricTestCase_t> combinations;
        try {
            std::vector<YAML::Node> networkList = Environment::getConfiguration("graph_metrics");

            if (networkList.empty())
                return combinations;

            for (auto &network : networkList)
                for (auto &groupName : network["metric_groups"].as<std::vector<std::string>>())
                    for (auto execTime : executionTime)
                        combinations.push_back({network, groupName, execTime});

            return combinations;
        } catch (YAML::Exception &e) {
            PRINTF("Bad node: Reason: %s\n", e.what());
            combinations.clear();
            return combinations;
        }
    }

    void getMetrics(uint32_t &numReports, std::vector<zet_typed_value_t> &metricValues) {
        ASSERT_NE(hMetricStreamer, nullptr);

        size_t rawDataSize = 0;
        EXPECT_EQ(zetMetricStreamerReadData(hMetricStreamer, UINT32_MAX, &rawDataSize, nullptr),
                  ZE_RESULT_SUCCESS);

        ASSERT_GT(rawDataSize, 0);

        std::vector<uint64_t> rawData(rawDataSize / sizeof(uint64_t), 0u);
        EXPECT_EQ(zetMetricStreamerReadData(hMetricStreamer,
                                            UINT32_MAX,
                                            &rawDataSize,
                                            reinterpret_cast<uint8_t *>(rawData.data())),
                  ZE_RESULT_SUCCESS);

        TRACE_BUF(rawData.data(), rawDataSize);

        EXPECT_GT(rawData[0], 0u);

        uint32_t metricValueCount = 0;
        EXPECT_EQ(
            zetMetricGroupCalculateMetricValues(testedMetricGroup,
                                                ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                                rawDataSize,
                                                reinterpret_cast<uint8_t *>(rawData.data()),
                                                &metricValueCount,
                                                nullptr),
            ZE_RESULT_SUCCESS);

        metricValues.resize(metricValueCount);
        EXPECT_EQ(
            zetMetricGroupCalculateMetricValues(testedMetricGroup,
                                                ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                                rawDataSize,
                                                reinterpret_cast<uint8_t *>(rawData.data()),
                                                &metricValueCount,
                                                metricValues.data()),
            ZE_RESULT_SUCCESS);

        TRACE_BUF(metricValues.data(), metricValues.size() * sizeof(zet_typed_value_t));
        numReports = testedMetricCount ? (metricValueCount / testedMetricCount) : 0;
    }

  protected:
    zet_metric_streamer_handle_t hMetricStreamer = nullptr;
    ze_result_t ret = ZE_RESULT_SUCCESS;
    zet_metric_group_handle_t testedMetricGroup = nullptr;
    zet_metric_group_properties_t metricGroupProperties;
    uint32_t testedMetricCount = 0;
    std::vector<zet_metric_properties_t> metricsProperties;
};

std::vector<uint32_t> execTimeComputeEngineMs = {200};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(MetricStreamer);

INSTANTIATE_TEST_SUITE_P(
    ,
    MetricStreamer,
    ::testing::ValuesIn(MetricStreamer::createCasesForMetricsTest(execTimeComputeEngineMs)),
    [](const testing::TestParamInfo<metricTestCase_t> &p) {
        const YAML::Node node(std::get<0>(p.param));
        return generateTestNameFromNode(node) + "_" + std::get<1>(p.param);
    });

TEST_P(MetricStreamer, RunInferenceWithTimeBasedCollection) {
    auto [node, metricGroupName, execTime] = GetParam();
    std::filesystem::path path(node["path"].as<std::string>());

    std::shared_ptr<Graph> graph =
        Graph::create(zeContext,
                      zeDevice,
                      zeGraphDDITableExt,
                      path.extension() == ".xml" ? modelDir + node["path"].as<std::string>()
                                                 : blobDir + node["path"].as<std::string>(),
                      node);

    graph->allocateArguments(MemType::SHARED_MEMORY);

    if (path.extension() == ".xml") {
        graph->setRandomInput();
    }

    ASSERT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 1, &testedMetricGroup),
              ZE_RESULT_SUCCESS);

    zet_metric_streamer_desc_t metricStreamerDesc = {};
    metricStreamerDesc.stype = ZET_STRUCTURE_TYPE_METRIC_STREAMER_DESC;
    metricStreamerDesc.samplingPeriod = 10'000'000u; // 10 [ms]
    metricStreamerDesc.notifyEveryNReports = 20;

    ASSERT_EQ(zetMetricStreamerOpen(zeContext,
                                    zeDevice,
                                    testedMetricGroup,
                                    &metricStreamerDesc,
                                    nullptr,
                                    &hMetricStreamer),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0u, nullptr),
        ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0u, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    std::chrono::steady_clock::time_point timeOut =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(execTime);

    while (std::chrono::steady_clock::now() < timeOut) {
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
    }

    uint32_t numReports = 0;
    std::vector<zet_typed_value_t> metricValues;

    getMetrics(numReports, metricValues);
    ASSERT_GT(numReports, 0);

    std::vector<long long> sumOfMetricValues(testedMetricCount, 0);

    int report = 1;
    for (size_t i = 0; i < metricValues.size(); i++) {
        sumOfMetricValues[i % testedMetricCount] += metricValues[i].value.ui64;

        if (i % testedMetricCount == 0) {
            TRACE("Report #%d\n", report);
            report++;
        }

        TRACE("Metric %lu -> type: %#x, value: %lu\n",
              i % (testedMetricCount),
              metricValues[i].type,
              metricValues[i].value.ui64);
    }

    TRACE("Summed values from individual metrics:\n");
    for (uint32_t i = 0; i < sumOfMetricValues.size(); i++) {
        EXPECT_GT(sumOfMetricValues[i], 0llu)
            << "Sum of values ​​for the metric " << i << " is equal to 0";
        TRACE("Sum for Metric #%lu: %llu\n", static_cast<unsigned long>(i), sumOfMetricValues[i]);
    }
}

TEST_P(MetricStreamer, RunInferenceUseEventToCollectMetrics) {
    const uint32_t samplingPeriodMs = 10;
    const uint32_t nReportsNotification = 3;

    auto [node, metricGroupName, execTime] = GetParam();
    std::filesystem::path path(node["path"].as<std::string>());

    ASSERT_GT(execTime, samplingPeriodMs * nReportsNotification) << "Too short execution time set";

    std::shared_ptr<Graph> graph =
        Graph::create(zeContext,
                      zeDevice,
                      zeGraphDDITableExt,
                      path.extension() == ".xml" ? modelDir + node["path"].as<std::string>()
                                                 : blobDir + node["path"].as<std::string>(),
                      node);

    graph->allocateArguments(MemType::SHARED_MEMORY);

    if (path.extension() == ".xml") {
        graph->setRandomInput();
    }
    ASSERT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 1, &testedMetricGroup),
              ZE_RESULT_SUCCESS);

    zet_metric_streamer_desc_t metricStreamerDesc = {};
    metricStreamerDesc.stype = ZET_STRUCTURE_TYPE_METRIC_STREAMER_DESC;
    metricStreamerDesc.samplingPeriod = static_cast<uint64_t>(samplingPeriodMs) * 1'000'000u;
    metricStreamerDesc.notifyEveryNReports = nReportsNotification;

    const ze_event_pool_desc_t eventPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                                nullptr,
                                                ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                                1};
    const ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                       nullptr,
                                       0,
                                       ZE_EVENT_SCOPE_FLAG_HOST,
                                       ZE_EVENT_SCOPE_FLAG_HOST};
    auto scopedEventPool = zeScope::eventPoolCreate(zeContext, eventPoolDesc, 1, zeDevice, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_NE(scopedEventPool.get(), nullptr);
    auto metricEvent = zeScope::eventCreate(scopedEventPool.get(), eventDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_NE(metricEvent.get(), nullptr);

    ASSERT_EQ(zetMetricStreamerOpen(zeContext,
                                    zeDevice,
                                    testedMetricGroup,
                                    &metricStreamerDesc,
                                    metricEvent.get(),
                                    &hMetricStreamer),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0u, nullptr),
        ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0u, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    /* warm up - memory allocation and HW wake up */
    EXPECT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    uint32_t inferenceTime = (samplingPeriodMs * nReportsNotification) + (2 * samplingPeriodMs);
    std::chrono::steady_clock::time_point testTimeOut =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(execTime);

    std::chrono::steady_clock::time_point inferenceTimeOut =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(inferenceTime);

    /* Check if event is signalled after expected time */
    while (std::chrono::steady_clock::now() < inferenceTimeOut) {
        EXPECT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        EXPECT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
    }
    ASSERT_EQ(zeEventQueryStatus(metricEvent.get()), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeEventHostReset(metricEvent.get()), ZE_RESULT_SUCCESS);

    /* Metric data still stored in buffer, check if event signalled */
    ASSERT_EQ(zeEventQueryStatus(metricEvent.get()), ZE_RESULT_SUCCESS);

    /* clear buffer and reset event before continue test */
    {
        uint32_t numReports = 0;
        std::vector<zet_typed_value_t> metricValues;
        getMetrics(numReports, metricValues);
        ASSERT_EQ(zeEventHostReset(metricEvent.get()), ZE_RESULT_SUCCESS);
    }

    /* check zeEventHostSynchronize on metric event */
    while (std::chrono::steady_clock::now() < testTimeOut) {
        inferenceTimeOut =
            std::chrono::steady_clock::now() + std::chrono::milliseconds(inferenceTime);

        EXPECT_NE(zeEventQueryStatus(metricEvent.get()), ZE_RESULT_SUCCESS);

        std::future<_ze_result_t> eventSynchronize = std::async(std::launch::async, [&] {
            return zeEventHostSynchronize(metricEvent.get(), inferenceTime * 1'000'000);
        });

        while (std::chrono::steady_clock::now() < inferenceTimeOut) {
            EXPECT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr),
                      ZE_RESULT_SUCCESS);
            EXPECT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
        }

        EXPECT_EQ(eventSynchronize.get(), ZE_RESULT_SUCCESS);
        EXPECT_EQ(zeEventQueryStatus(metricEvent.get()), ZE_RESULT_SUCCESS);

        uint32_t numReports = 0;
        std::vector<zet_typed_value_t> metricValues;

        getMetrics(numReports, metricValues);
        /* It is expected greater or equal number of reports */
        EXPECT_GE(numReports, nReportsNotification);
        EXPECT_EQ(zeEventHostReset(metricEvent.get()), ZE_RESULT_SUCCESS);
    }
}

class MetricStreamerCopyEngine : public MetricStreamer {
  public:
    void SetUp() override {
        useCopyOrdinal = true;
        MetricStreamer::SetUp();
    }

    void TearDown() override { MetricStreamer::TearDown(); }

    /* Generates test cases for copy engine, it ignores network and returns
     * combinations of metric groups and inference execution time of copy job
     * required to gather metrics.
     */
    static std::vector<metricTestCase_t>
    createCasesForMetricsTest(std::vector<uint32_t> &executionTime) {
        std::vector<metricTestCase_t> combinations;
        try {
            std::vector<YAML::Node> network = Environment::getConfiguration("graph_metrics");

            if (network.empty())
                return combinations;
            /* network is pushed only to satisfy type definition,  it is not used */
            for (auto &execTime : executionTime)
                combinations.push_back({network[0], std::string("NOC"), execTime});

            return combinations;
        } catch (YAML::Exception &e) {
            PRINTF("Bad node: Reason: %s\n", e.what());
            combinations.clear();
            return combinations;
        }
    }
};

/* Note: When execution time is shorter than sampling time then device can enter D3 state and
 * stop sampling timer. In this case for first sampling period host doesn't get samples,
 * the buffer will not be returned.
 * The  current implementation in firmware creates buffer when the first sampling period has passed
 * To overcome this problem the job execution time must be longer than sampling preiod.
 * (details:EISW-96802)
 */
std::vector<uint32_t> execTimeCopyEngineMs = {20, 100};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(MetricStreamerCopyEngine);

INSTANTIATE_TEST_SUITE_P(
    ,
    MetricStreamerCopyEngine,
    ::testing::ValuesIn(MetricStreamerCopyEngine::createCasesForMetricsTest(execTimeCopyEngineMs)),
    [](const testing::TestParamInfo<metricTestCase_t> &p) {
        std::string groupName = std::get<1>(p.param);
        uint32_t execTime = std::get<2>(p.param);
        return groupName + "_" + std::to_string(execTime) + "ms";
    });

TEST_P(MetricStreamerCopyEngine, RunCopyWithTimeBasedCollection) {
    auto [node, metricGroupName, execTime] = GetParam();

    ASSERT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 1, &testedMetricGroup),
              ZE_RESULT_SUCCESS);

    zet_metric_streamer_desc_t metricStreamerDesc = {};
    metricStreamerDesc.stype = ZET_STRUCTURE_TYPE_METRIC_STREAMER_DESC;
    metricStreamerDesc.samplingPeriod = 10'000'000u; // 10 [ms]
    metricStreamerDesc.notifyEveryNReports = 20;

    ASSERT_EQ(zetMetricStreamerOpen(zeContext,
                                    zeDevice,
                                    testedMetricGroup,
                                    &metricStreamerDesc,
                                    nullptr,
                                    &hMetricStreamer),
              ZE_RESULT_SUCCESS);

    const size_t allocSize = 64 * KB;
    std::shared_ptr<void> srcMem, dstMem;
    srcMem = AllocSharedMemory(allocSize);
    dstMem = AllocSharedMemory(allocSize);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(list,
                                            dstMem.get(),
                                            srcMem.get(),
                                            allocSize,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    /* Warm up NPU to set metric streamer in busy state */
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    std::chrono::steady_clock::time_point timeOut =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(execTime);

    while (std::chrono::steady_clock::now() < timeOut) {
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
    }

    uint32_t numReports = 0;
    std::vector<zet_typed_value_t> metricValues;

    getMetrics(numReports, metricValues);
    EXPECT_GT(numReports, 0);
    ASSERT_GT(metricValues.size(), 0);
    EXPECT_EQ(metricValues[0].type, metricsProperties[0].resultType);

    EXPECT_GT(metricValues[0].value.ui64, 0llu);

    TRACE_BUF(metricValues.data(), metricValues.size() * sizeof(zet_typed_value_t));
}
