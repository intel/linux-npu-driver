/*
 * Copyright (C) 2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

#include <numeric>

using InferenceDuration = std::chrono::duration<float, std::milli>;

class InferencePerformance : public UmdTest, public ::testing::WithParamInterface<YAML::Node> {
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
    ze_result_t ret = ZE_RESULT_SUCCESS;

    std::vector<InferenceDuration> inferenceDuration;

    void SetUp() override {
        UmdTest::SetUp();

        SKIP_PRESILICON("The test does not apply to the Simics platform");

        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();
    }

    InferenceDuration sectionDuration(std::chrono::steady_clock::time_point &start) {
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        auto time = end - start;
        start = end;
        return std::chrono::duration_cast<InferenceDuration>(time);
    }

    void TearDown() override { UmdTest::TearDown(); }

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(InferencePerformance);

INSTANTIATE_TEST_SUITE_P(,
                         InferencePerformance,
                         ::testing::ValuesIn(Environment::getConfiguration("graph_execution")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(InferencePerformance, MeasureTimeBetweenTwoInferencesAfterPutVPUInIdleState) {
    const YAML::Node node = GetParam();

    ASSERT_GT(node["path"].as<std::string>().size(), 0);

    std::shared_ptr<Graph> graph = Graph::create(zeContext,
                                                 zeDevice,
                                                 zeGraphDDITableExt,
                                                 blobDir + node["path"].as<std::string>(),
                                                 node);

    graph->allocateArguments(MemType::HOST_MEMORY);

    graph->copyInputData();

    std::chrono::steady_clock::time_point start;
    size_t execIter = 3;

    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    for (size_t i = 0; i < execIter; i++) {
        start = std::chrono::steady_clock::now();
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
        inferenceDuration.push_back(sectionDuration(start));

        graph->checkResults();
        graph->clearOutput();

        PRINTF("Inference #%zu took: %f ms\n", i, inferenceDuration[i].count());
    }

    InferenceDuration inferenceTime =
        std::reduce(inferenceDuration.begin(), inferenceDuration.end());

    PRINTF("\nThe duration of the VPU wakeup to be able to run the inference: %f ms\n",
           (inferenceDuration[0].count() - inferenceDuration[1].count()));

    TRACE("\nThroughput: %f FPS\n\n", static_cast<float>(execIter) * 1000 / inferenceTime.count());
}
