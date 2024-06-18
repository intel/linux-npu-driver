/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "perf_counter.h"
#include "graph_utilities.hpp"

#include <chrono>
#include <future>
#include <level_zero/ze_api.h>
#include <level_zero/zet_api.h>
#include <vector>

/*test case definition:
 * std::tuple<network definition node, metric group name, required inference execution time>
 */
using metricTestCase_t = std::tuple<YAML::Node, std::string, uint32_t>;

class MetricStreamer : public UmdTest, public ::testing::WithParamInterface<metricTestCase_t> {
  public:
    void SetUp() override {
        UmdTest::SetUp();

        uint32_t count = 0;
        if (zetMetricGroupGet(zeDevice, &count, nullptr) == ZE_RESULT_ERROR_UNSUPPORTED_FEATURE)
            SKIP_("Metrics are not supported");

        uint32_t ordinal = useCopyOrdinal ? copyGrpOrdinal : computeGrpOrdinal;
        ASSERT_EQ(createCommandQueue(ordinal, &queue), ZE_RESULT_SUCCESS);
        ASSERT_EQ(createCommandList(ordinal, &list), ZE_RESULT_SUCCESS);

        auto [node, groupName, execTime] = GetParam();
        getMetricGroupByName(groupName);

        ASSERT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 1, &hMetricGroup),
                  ZE_RESULT_SUCCESS);
    }

    void TearDown() override {
        if (hMetricStreamer) {
            EXPECT_EQ(zetMetricStreamerClose(hMetricStreamer), ZE_RESULT_SUCCESS);
        }

        EXPECT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 0u, nullptr),
                  ZE_RESULT_SUCCESS);

        UmdTest::TearDown();
    }

    /* Function returns combinations of network and defined
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

    ze_result_t createCommandQueue(uint32_t ordinal, ze_command_queue_handle_t *handle) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        ze_command_queue_desc_t desc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                        .pNext = nullptr,
                                        .ordinal = ordinal,
                                        .index = 0,
                                        .flags = 0,
                                        .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                        .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};
        auto scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, desc, ret);
        queues.push_back(std::move(scopedQueue));
        *handle = queues.back().get();
        return ret;
    }

    ze_result_t createCommandList(uint32_t ordinal, ze_command_list_handle_t *handle) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        ze_command_list_desc_t desc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                       .pNext = nullptr,
                                       .commandQueueGroupOrdinal = ordinal,
                                       .flags = 0};
        auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, desc, ret);
        lists.push_back(std::move(scopedList));
        *handle = lists.back().get();
        return ret;
    }

    ze_result_t createEvent(ze_event_handle_t *handle) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        const ze_event_pool_desc_t eventPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                                    nullptr,
                                                    ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                                    1};

        auto scopedEventPool = zeScope::eventPoolCreate(zeContext, eventPoolDesc, 1, zeDevice, ret);
        if (ret != ZE_RESULT_SUCCESS)
            return ret;

        ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                     nullptr,
                                     0,
                                     ZE_EVENT_SCOPE_FLAG_HOST,
                                     ZE_EVENT_SCOPE_FLAG_HOST};
        auto scopedEvent = zeScope::eventCreate(scopedEventPool.get(), eventDesc, ret);

        eventPools.push_back(std::move(scopedEventPool));
        events.push_back(std::move(scopedEvent));
        *handle = events.back().get();
        return ret;
    }

    void getMetricGroupByName(std::string &name) {
        uint32_t count = 0;
        ASSERT_EQ(zetMetricGroupGet(zeDevice, &count, nullptr), ZE_RESULT_SUCCESS);

        std::vector<zet_metric_group_handle_t> groups(count);
        ASSERT_EQ(zetMetricGroupGet(zeDevice, &count, groups.data()), ZE_RESULT_SUCCESS);

        for (const auto &group : groups) {
            zet_metric_group_properties_t prop = {};
            prop.stype = ZET_STRUCTURE_TYPE_METRIC_GROUP_PROPERTIES;

            ASSERT_EQ(zetMetricGroupGetProperties(group, &prop), ZE_RESULT_SUCCESS);

            if (name != prop.name) {
                continue;
            }

            hMetricGroup = group;
            metricGroupProperties = prop;
            break;
        }
    }

    void openMetricStreamer(uint32_t notify = 20,
                            uint32_t periodMs = 10,
                            ze_event_handle_t hEvent = nullptr) {
        zet_metric_streamer_desc_t metricStreamerDesc = {
            .stype = ZET_STRUCTURE_TYPE_METRIC_STREAMER_DESC,
            .pNext = nullptr,
            .notifyEveryNReports = notify,
            .samplingPeriod = periodMs * 1'000'000, // [ns]
        };
        ASSERT_EQ(zetMetricStreamerOpen(zeContext,
                                        zeDevice,
                                        hMetricGroup,
                                        &metricStreamerDesc,
                                        hEvent,
                                        &hMetricStreamer),
                  ZE_RESULT_SUCCESS);
        TRACE("Opened MetricStreamer with notifyEveryNReports: %u, samplingPeriod: %u ms",
              notify,
              periodMs);
    }

    void readReports() {
        size_t dataSize = 0;
        ASSERT_EQ(zetMetricStreamerReadData(hMetricStreamer, UINT32_MAX, &dataSize, nullptr),
                  ZE_RESULT_SUCCESS);
        if (dataSize == 0) {
            reportCount = 0;
            return;
        }

        std::vector<uint8_t> data(dataSize);
        ASSERT_EQ(zetMetricStreamerReadData(hMetricStreamer, UINT32_MAX, &dataSize, data.data()),
                  ZE_RESULT_SUCCESS);
        TRACE_BUF(data.data(), data.size());

        uint32_t valueCount = 0;
        ASSERT_EQ(
            zetMetricGroupCalculateMetricValues(hMetricGroup,
                                                ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                                dataSize,
                                                data.data(),
                                                &valueCount,
                                                nullptr),
            ZE_RESULT_SUCCESS);

        metricValues.resize(valueCount);
        ASSERT_EQ(
            zetMetricGroupCalculateMetricValues(hMetricGroup,
                                                ZET_METRIC_GROUP_CALCULATION_TYPE_METRIC_VALUES,
                                                dataSize,
                                                data.data(),
                                                &valueCount,
                                                metricValues.data()),
            ZE_RESULT_SUCCESS);

        TRACE_BUF(metricValues.data(), metricValues.size() * sizeof(zet_typed_value_t));
        reportCount =
            static_cast<uint32_t>(std::floor(valueCount / metricGroupProperties.metricCount));
        TRACE("ReportCount: %lu\n", reportCount);
    }

  public:
    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;
    bool useCopyOrdinal = false;

    zet_metric_streamer_handle_t hMetricStreamer = nullptr;
    zet_metric_group_handle_t hMetricGroup = nullptr;
    zet_metric_group_properties_t metricGroupProperties = {};
    ze_result_t ret = ZE_RESULT_SUCCESS;

    std::vector<zet_typed_value_t> metricValues = {};
    size_t reportCount = 0;

  private:
    std::vector<zeScope::SharedPtr<ze_command_queue_handle_t>> queues;
    std::vector<zeScope::SharedPtr<ze_command_list_handle_t>> lists;
    std::vector<zeScope::SharedPtr<ze_event_pool_handle_t>> eventPools;
    std::vector<zeScope::SharedPtr<ze_event_handle_t>> events;
};

std::vector<uint32_t> execTimeComputeEngineMs = {500};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(MetricStreamer);

INSTANTIATE_TEST_SUITE_P(
    ,
    MetricStreamer,
    ::testing::ValuesIn(MetricStreamer::createCasesForMetricsTest(execTimeComputeEngineMs)),
    [](const testing::TestParamInfo<metricTestCase_t> &p) {
        const YAML::Node node(std::get<0>(p.param));
        return generateTestNameFromNode(node) + "_" + std::get<1>(p.param);
    });

TEST_P(MetricStreamer, RunInferenceExpectAnyReport) {
    auto [node, metricGroupName, execTime] = GetParam();

    openMetricStreamer();

    std::shared_ptr<Graph> graph =
        Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

    graph->allocateArguments(MemType::SHARED_MEMORY);
    graph->copyInputData();

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0u, nullptr),
        ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0u, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    PerfCounter counter(execTime);
    counter.start();
    while (!counter.isTimedOut()) {
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
        counter.countFrame();
    }
    counter.stop();

    readReports();
    ASSERT_GT(reportCount, 0) << "Failed to get any reports after running inference for "
                              << counter.duration() << " s, frame count: " << counter.getCount();
}

TEST_P(MetricStreamer, RunInferenceExpectReportNotification) {
    const uint32_t samplingPeriodMs = 10;
    const uint32_t nReportsNotification = 20;

    auto [node, metricGroupName, execTime] = GetParam();

    ASSERT_GT(execTime, samplingPeriodMs * nReportsNotification) << "Too short execution time set";

    ze_event_handle_t hEvent = nullptr;
    ASSERT_EQ(createEvent(&hEvent), ZE_RESULT_SUCCESS);
    openMetricStreamer(nReportsNotification, samplingPeriodMs, hEvent);

    std::shared_ptr<Graph> graph =
        Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

    graph->allocateArguments(MemType::SHARED_MEMORY);
    graph->copyInputData();

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

    uint32_t inferenceTimeMs = samplingPeriodMs * (nReportsNotification + 2);
    std::chrono::steady_clock::time_point testTimeOut =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(execTime);

    while (std::chrono::steady_clock::now() < testTimeOut) {
        ASSERT_EQ(zeEventQueryStatus(hEvent), ZE_RESULT_NOT_READY);

        PerfCounter counter(inferenceTimeMs);
        counter.start();
        while (!counter.isTimedOut()) {
            EXPECT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr),
                      ZE_RESULT_SUCCESS);
            EXPECT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
            counter.countFrame();
        }
        counter.stop();

        EXPECT_EQ(zeEventQueryStatus(hEvent), ZE_RESULT_SUCCESS)
            << "Failed to get " << nReportsNotification << " reports after running inference for "
            << counter.duration() << " s, frame count: " << counter.getCount();

        /* clear buffer and reset event before continue test */
        readReports();
        ASSERT_GE(reportCount, nReportsNotification)
            << "Received report count: " << reportCount
            << ", expected report count: " << nReportsNotification;
        ASSERT_EQ(zeEventHostReset(hEvent), ZE_RESULT_SUCCESS);
    }
}

TEST_P(MetricStreamer, RunInferenceExpectReportNotificationFromEventHostSynchronize) {
    const uint32_t samplingPeriodMs = 10;
    const uint32_t nReportsNotification = 20;

    auto [node, metricGroupName, execTime] = GetParam();

    ASSERT_GT(execTime, samplingPeriodMs * nReportsNotification) << "Too short execution time set";

    ze_event_handle_t hEvent = nullptr;
    ASSERT_EQ(createEvent(&hEvent), ZE_RESULT_SUCCESS);
    openMetricStreamer(nReportsNotification, samplingPeriodMs, hEvent);

    std::shared_ptr<Graph> graph =
        Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

    graph->allocateArguments(MemType::SHARED_MEMORY);
    graph->copyInputData();

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0u, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0u, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    EXPECT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    uint32_t inferenceTimeMs = samplingPeriodMs * (nReportsNotification + 2);
    std::chrono::steady_clock::time_point testTimeOut =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(execTime);

    while (std::chrono::steady_clock::now() < testTimeOut) {
        EXPECT_EQ(zeEventQueryStatus(hEvent), ZE_RESULT_NOT_READY);

        std::future<_ze_result_t> eventSynchronize = std::async(std::launch::async, [&] {
            return zeEventHostSynchronize(hEvent, inferenceTimeMs * 1'000'000);
        });

        PerfCounter counter(inferenceTimeMs);
        counter.start();
        while (!counter.isTimedOut()) {
            EXPECT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr),
                      ZE_RESULT_SUCCESS);
            EXPECT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
            counter.countFrame();
        }
        counter.stop();

        EXPECT_EQ(eventSynchronize.get(), ZE_RESULT_SUCCESS)
            << "Failed to get " << nReportsNotification << " reports after running inference for "
            << counter.duration() << " s, frame count: " << counter.getCount();
        EXPECT_EQ(zeEventQueryStatus(hEvent), ZE_RESULT_SUCCESS);

        readReports();
        /* It is expected greater or equal number of reports */
        ASSERT_GE(reportCount, nReportsNotification)
            << "Received report count: " << reportCount
            << ", expected report count: " << nReportsNotification;
        EXPECT_EQ(zeEventHostReset(hEvent), ZE_RESULT_SUCCESS);
    }
}

class MetricStreamerCopyEngine : public MetricStreamer {
  public:
    MetricStreamerCopyEngine() { useCopyOrdinal = true; }

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

TEST_P(MetricStreamerCopyEngine, RunCopyExpectAnyReport) {
    auto [node, metricGroupName, execTime] = GetParam();

    openMetricStreamer();

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

    PerfCounter counter(execTime);
    counter.start();
    while (!counter.isTimedOut()) {
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
        counter.countData(allocSize);
    }
    counter.stop();

    readReports();
    EXPECT_GT(reportCount, 0) << "Failed to get any report after running copy job for "
                              << counter.duration() << " s, frame count: " << counter.getCount()
                              << " bytes, bandwith: " << counter.getMbps() << " Mbps";
    ASSERT_GT(metricValues.size(), 0);
    EXPECT_GT(metricValues[0].value.ui64, 0llu);
    TRACE_BUF(metricValues.data(), metricValues.size() * sizeof(zet_typed_value_t));
}
