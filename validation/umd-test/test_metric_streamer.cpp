/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <vector>

class MetricStreamer : public UmdTest,
                       public ::testing::WithParamInterface<std::tuple<std::string, YAML::Node>> {
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

    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;

    void SetUp() override {
        UmdTest::SetUp();

        ze_result_t ret;

        YAML::Node &configuration = Environment::getConfiguration();
        if (configuration["blob_dir"].IsDefined())
            blobDir = configuration["blob_dir"].as<std::string>();

        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();

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

    void TearDown() override { UmdTest::TearDown(); }

  protected:
    ze_result_t ret = ZE_RESULT_SUCCESS;
    uint32_t metricGroupsCount = 0;
    std::vector<zet_metric_group_handle_t> metricGroups;

    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList;
    std::string blobDir = "";
};

std::vector<std::string> metricGroupNames = {"ShaveIL1Cache",
                                             "ShaveDL1Cache",
                                             "ShaveL2Cache",
                                             "NOC"};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(MetricStreamer);

INSTANTIATE_TEST_SUITE_P(
    ,
    MetricStreamer,
    ::testing::Combine(::testing::ValuesIn(metricGroupNames),
                       ::testing::ValuesIn(Environment::getConfiguration("graph_metrics"))),
    [](const testing::TestParamInfo<std::tuple<std::string, YAML::Node>> &p) {
        const YAML::Node node(std::get<1>(p.param));
        return std::get<0>(p.param) + "_" + generateTestNameFromNode(node);
    });

TEST_P(MetricStreamer, RunInferenceWithTimeBasedCollection) {
    auto [metric, node] = GetParam();

    std::vector<std::vector<char>> inputBin, outputBin;
    std::vector<char> vpuBlob, vpuBin;

    ASSERT_TRUE(getBlobFromPath(blobDir + node["path"].as<std::string>(),
                                node["in"].as<std::vector<std::string>>(),
                                node["out"].as<std::vector<std::string>>(),
                                vpuBlob,
                                inputBin,
                                outputBin,
                                vpuBin));

    zet_metric_group_handle_t hMetricGroup = nullptr;

    const char *metricName = metric.c_str();
    for (uint32_t i = 0; i < metricGroupsCount; i++) {
        zet_metric_group_properties_t metricGroupProperties = {};
        metricGroupProperties.stype = ZET_STRUCTURE_TYPE_METRIC_GROUP_PROPERTIES;

        ASSERT_EQ(zetMetricGroupGetProperties(metricGroups[i], &metricGroupProperties),
                  ZE_RESULT_SUCCESS);
        if (strcmp(metricName, metricGroupProperties.name) == 0) {
            hMetricGroup = metricGroups[i];
            break;
        }
    }

    ASSERT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 1, &hMetricGroup),
              ZE_RESULT_SUCCESS);

    zet_metric_streamer_handle_t hMetricStreamer = nullptr;
    zet_metric_streamer_desc_t metricStreamerDesc = {};
    metricStreamerDesc.stype = ZET_STRUCTURE_TYPE_METRIC_STREAMER_DESC;
    metricStreamerDesc.samplingPeriod = 10'000'000u; // 10 [ms]
    metricStreamerDesc.notifyEveryNReports = 20;

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
    for (const auto &input : graphInput) {
        ASSERT_EQ(zeGraphDDITableExt->pfnSetArgumentValue(graphHandle, argIndex++, input),
                  ZE_RESULT_SUCCESS);
    }

    for (const auto &output : graphOutput) {
        ASSERT_EQ(zeGraphDDITableExt->pfnSetArgumentValue(graphHandle, argIndex++, output),
                  ZE_RESULT_SUCCESS);
    }

    // Open metric streamer
    ASSERT_EQ(zetMetricStreamerOpen(zeContext,
                                    zeDevice,
                                    hMetricGroup,
                                    &metricStreamerDesc,
                                    nullptr,
                                    &hMetricStreamer),
              ZE_RESULT_SUCCESS);

    // Workload
    ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphInitialize(list, graphHandle, nullptr, 0u, nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphExecute(list, graphHandle, nullptr, nullptr, 0u, nullptr),
        ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    // Read raw data
    size_t rawDataSize = 0;
    // maxReportCount – the maximum number of reports the application wants to receive.
    //                  If UINT32_MAX, then function will retrieve all reports available
    ASSERT_EQ(zetMetricStreamerReadData(hMetricStreamer, UINT32_MAX, &rawDataSize, nullptr),
              ZE_RESULT_SUCCESS);

    std::vector<uint64_t> rawData(rawDataSize / sizeof(uint64_t), 0u);
    ASSERT_EQ(zetMetricStreamerReadData(hMetricStreamer,
                                        UINT32_MAX,
                                        &rawDataSize,
                                        reinterpret_cast<uint8_t *>(rawData.data())),
              ZE_RESULT_SUCCESS);

    TRACE_BUF(rawData.data(), rawDataSize);

    // Close metric streamer
    ASSERT_EQ(zetMetricStreamerClose(hMetricStreamer), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zetContextActivateMetricGroups(zeContext, zeDevice, 0u, nullptr), ZE_RESULT_SUCCESS);
}
