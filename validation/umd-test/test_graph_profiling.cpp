/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

#include <ze_api.h>

class CompilerGraphProfiling : public UmdTest, public ::testing::WithParamInterface<YAML::Node> {
  protected:
    void SetUp() override {
        UmdTest::SetUp();

        const YAML::Node node = GetParam();

        if (node["graph_profiling"].IsDefined() && node["graph_profiling"].as<bool>() == false) {
            SKIP_("The profiling graph test has been disabled.");
        }

        // TODO: an option with or without profiling (this should apply to blob as well)
        graph = Graph::create(zeContext,
                              zeDevice,
                              zeGraphDDITableExt,
                              globalConfig,
                              node,
                              ZE_GRAPH_FLAG_ENABLE_PROFILING);
        ASSERT_NE(graph, nullptr);
        graph->allocateArguments(MemType::SHARED_MEMORY);
        graph->copyInputData();
        ASSERT_TRUE(graph->isProfilingEnabled())
            << "Graph profiling should be enabled for this test.";

        ze_result_t ret = ZE_RESULT_SUCCESS;
        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();
    }

    void queryProfilingData(const std::unique_ptr<GraphProfiling> &profiling,
                            ze_graph_profiling_query_handle_t hProfilingQuery) {
        std::vector<ze_profiling_task_info> taskInfo =
            profiling->queryProfilingData<ze_profiling_task_info>(hProfilingQuery,
                                                                  ZE_GRAPH_PROFILING_TASK_LEVEL);
        EXPECT_TRUE(!taskInfo.empty()) << "Task level profiling info is empty.";
        profiling->printProfilingData(taskInfo);
        profiling->printProfilingLog(hProfilingQuery, ZE_GRAPH_PROFILING_TASK_LEVEL);

        std::vector<ze_profiling_layer_info> layerInfo =
            profiling->queryProfilingData<ze_profiling_layer_info>(hProfilingQuery,
                                                                   ZE_GRAPH_PROFILING_LAYER_LEVEL);
        EXPECT_TRUE(!layerInfo.empty()) << "Layer level profiling info is empty.";
        profiling->printProfilingData(layerInfo);
        profiling->printProfilingLog(hProfilingQuery, ZE_GRAPH_PROFILING_LAYER_LEVEL);
    }

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
    std::shared_ptr<Graph> graph;

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CompilerGraphProfiling);

INSTANTIATE_TEST_SUITE_P(,
                         CompilerGraphProfiling,
                         ::testing::ValuesIn(Environment::getConfiguration("graph_execution")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(CompilerGraphProfiling, RunInferenceQueryProfilingData) {
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandListReset(list), ZE_RESULT_SUCCESS);

    auto profiling = graph->createGraphProfiling(zeGraphProfilingDDITableExt, 1);
    ASSERT_NE(profiling, nullptr) << "Failed to get GraphProfiling instance from graph";

    auto hProfilingQuery = profiling->getQueryHandle(0);
    ASSERT_NE(hProfilingQuery, nullptr);

    ASSERT_EQ(
        zeGraphDDITableExt
            ->pfnAppendGraphExecute(list, graph->handle, hProfilingQuery, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_TRUE(graph->checkResults());
    queryProfilingData(profiling, hProfilingQuery);
}

TEST_P(CompilerGraphProfiling, MutateGraphArgumentsAndQueryProfilingData) {
    ASSERT_EQ(zeGraphDDITableExt->pfnGraphInitialize(graph->handle), ZE_RESULT_SUCCESS);

    auto profiling = graph->createGraphProfiling(zeGraphProfilingDDITableExt, 1);
    ASSERT_NE(profiling, nullptr) << "Failed to get GraphProfiling instance from graph";

    auto hProfilingQuery = profiling->getQueryHandle(0);
    ASSERT_NE(hProfilingQuery, nullptr);

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

    ze_result_t ret = ZE_RESULT_SUCCESS;
    auto scopedMutableList = zeScope::commandListCreate(zeContext, zeDevice, commandListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    auto mutableList = scopedMutableList.get();

    ze_mutable_command_id_exp_desc_t mutableCmdIdDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_ID_EXP_DESC,
        .pNext = nullptr,
        .flags = ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS,
    };
    uint64_t commandId = 0;
    ASSERT_EQ(zeCommandListGetNextCommandIdExp(mutableList, &mutableCmdIdDesc, &commandId),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphExecute(mutableList,
                                                        graph->handle,
                                                        hProfilingQuery,
                                                        nullptr,
                                                        0,
                                                        nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(mutableList), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &mutableList, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_TRUE(graph->checkResults());
    queryProfilingData(profiling, hProfilingQuery);

    auto prevOutputBuffer = graph->outArgs[0];
    graph->allocateOutputArguments(MemType::HOST_MEMORY);

    // update the output argument with the new buffer
    ze_mutable_graph_argument_exp_desc_t graphArgumentDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
        .pNext = nullptr,
        .commandId = commandId,
        .argIndex = static_cast<uint32_t>(graph->inArgs.size()),
        .pArgValue = graph->outArgs[0],
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

    ASSERT_TRUE(graph->checkResults());
    queryProfilingData(profiling, hProfilingQuery);
    EXPECT_EQ(memcmp(prevOutputBuffer, graph->outArgs[0], graph->outputSize[0]), 0)
        << "Output buffer content should be the same after mutation since we used the same input "
           "buffer.";

    // realloc the first argument and update the graph
    graphArgumentDesc.pArgValue = graph->reallocArgument(0, MemType::HOST_MEMORY);
    graphArgumentDesc.argIndex = 0;
    ASSERT_EQ(zeCommandListUpdateMutableCommandsExp(mutableList, &mutableCommandsDesc),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(mutableList), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &mutableList, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    ASSERT_TRUE(graph->checkResults());
    queryProfilingData(profiling, hProfilingQuery);
}
