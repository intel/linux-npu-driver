/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"
#include "umd_test.h"

#include <filesystem>
#include <climits>
#include <vector>

enum stage { UPLOAD, EXECUTE, READBACK, COUNT };

class GraphObject : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();

        ze_result_t ret;
        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create CommandQueue for GraphInit";
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create CommandList for GraphInit";
        list = scopedList.get();

        scopedFence = zeScope::fenceCreate(queue, fenceDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create Fence for GraphInit";
        fence = scopedFence.get();
    }

    void GraphInitialize() {
        ASSERT_EQ(
            zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph->handle, nullptr, 0, nullptr),
            ZE_RESULT_SUCCESS)
            << "Failed to append GraphInitialize to CommandList";

        ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS)
            << "Failed to close CommandList for GraphInit";

        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, fence), ZE_RESULT_SUCCESS)
            << "Failed to execute CommandQueue for GraphInit";

        ASSERT_EQ(zeFenceHostSynchronize(fence, graphSyncTimeout), ZE_RESULT_SUCCESS)
            << "Failed to synchronize Fence for GraphInit";
    }

    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;
    ze_fence_handle_t fence = nullptr;

  protected:
    std::shared_ptr<Graph> graph;

    ze_command_queue_desc_t cmdQueueDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                         .pNext = nullptr,
                                         .ordinal = 0,
                                         .index = 0,
                                         .flags = 0,
                                         .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                         .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};

    ze_command_list_desc_t cmdListDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                       .pNext = nullptr,
                                       .commandQueueGroupOrdinal = 0,
                                       .flags = 0};

    ze_fence_desc_t fenceDesc{.stype = ZE_STRUCTURE_TYPE_FENCE_DESC, .pNext = nullptr, .flags = 0};

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
    zeScope::SharedPtr<ze_fence_handle_t> scopedFence = nullptr;
};

class GraphPipeline : public GraphObject {
  public:
    void SetUp() override {
        GraphObject::SetUp();

        ze_result_t ret;
        for (uint32_t index = 0; index < stage::COUNT; ++index) {
            scopedQueueVec.push_back(
                zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret));
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create CommandQueue #" << index;
            queueVec.push_back(scopedQueueVec.back().get());

            scopedListVec.push_back(
                zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret));
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create CommandList #" << index;
            listVec.push_back(scopedListVec.back().get());

            scopedFenceVec.push_back(zeScope::fenceCreate(queueVec.back(), fenceDesc, ret));
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create Fence #" << index;
            fenceVec.push_back(scopedFenceVec.back().get());
        }

        scopedEventpool = zeScope::eventPoolCreate(zeContext, eventPoolDesc, 1, zeDevice, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create EventPool#";
        eventPool = scopedEventpool.get();

        for (uint32_t index = 0; index < stage::COUNT; ++index) {
            eventDesc.index = index;
            scopedEventVec.push_back(zeScope::eventCreate(eventPool, eventDesc, ret));
            ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create Event #" << index;
            eventVec.push_back(scopedEventVec.back().get());
        }
    }

    void CreatePipeline(std::vector<void *> &inputHostMem, std::vector<void *> &outputHostMem) {
        for (size_t i = 0; i < graph->inputSize.size(); i++) {
            ASSERT_EQ(zeCommandListAppendMemoryCopy(listVec[stage::UPLOAD],
                                                    graph->inArgs[i],
                                                    inputHostMem[i],
                                                    graph->inputSize[i],
                                                    nullptr,
                                                    0,
                                                    nullptr),
                      ZE_RESULT_SUCCESS)
                << "Failed to append copy command to CommandList[UPLOAD]";
        }

        ASSERT_EQ(zeCommandListAppendBarrier(listVec[stage::UPLOAD], nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS)
            << "Failed to append barrier command to CommandList[UPLOAD]";

        ASSERT_EQ(zeCommandListAppendSignalEvent(listVec[stage::UPLOAD], eventVec[stage::UPLOAD]),
                  ZE_RESULT_SUCCESS)
            << "Failed to append signal event command to CommandList[UPLOAD]";

        for (size_t i = 0; i < graph->outputSize.size(); i++) {
            ASSERT_EQ(zeCommandListAppendMemoryCopy(listVec[stage::READBACK],
                                                    outputHostMem[i],
                                                    graph->outArgs[i],
                                                    graph->outputSize[i],
                                                    nullptr,
                                                    0,
                                                    nullptr),
                      ZE_RESULT_SUCCESS)
                << "Failed to append copy command to CommandList[READBACK]";
        }

        ASSERT_EQ(
            zeCommandListAppendWaitOnEvents(listVec[stage::EXECUTE], 1, &eventVec[stage::UPLOAD]),
            ZE_RESULT_SUCCESS)
            << "Failed to append wait on event command to CommandList[EXECUTE]";

        ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphExecute(listVec[stage::EXECUTE],
                                                            graph->handle,
                                                            nullptr,
                                                            nullptr,
                                                            0,
                                                            nullptr),
                  ZE_RESULT_SUCCESS)
            << "Failed to append GraphExecute to CommandList[EXECUTE]";

        ASSERT_EQ(zeCommandListAppendEventReset(listVec[stage::READBACK], eventVec[stage::UPLOAD]),
                  ZE_RESULT_SUCCESS)
            << "Failed to append event reset command to CommandList[READBACK]";

        for (auto &list : listVec) {
            ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS) << "Failed to close CommandList";
        }
    }

  protected:
    std::vector<ze_command_queue_handle_t> queueVec;
    std::vector<ze_command_list_handle_t> listVec;
    std::vector<ze_fence_handle_t> fenceVec;
    std::vector<ze_event_handle_t> eventVec;

  private:
    ze_event_pool_handle_t eventPool = nullptr;

    ze_event_pool_desc_t eventPoolDesc{.stype = ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                       .pNext = nullptr,
                                       .flags = ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                       .count = stage::COUNT};

    ze_event_desc_t eventDesc{.stype = ZE_STRUCTURE_TYPE_EVENT_DESC,
                              .pNext = nullptr,
                              .index = 0,
                              .signal = 0,
                              .wait = 0};

    std::vector<zeScope::SharedPtr<ze_command_queue_handle_t>> scopedQueueVec;
    std::vector<zeScope::SharedPtr<ze_command_list_handle_t>> scopedListVec;
    std::vector<zeScope::SharedPtr<ze_fence_handle_t>> scopedFenceVec;
    zeScope::SharedPtr<ze_event_pool_handle_t> scopedEventpool;
    std::vector<zeScope::SharedPtr<ze_event_handle_t>> scopedEventVec;
};

class GraphInferenceT : public GraphPipeline {
  protected:
    void SetUp() override {
        GraphPipeline::SetUp();

        for (uint32_t index = 0; index < stage::COUNT; ++index) {
            ASSERT_NE(queueVec[index], nullptr);
            ASSERT_NE(listVec[index], nullptr);
            ASSERT_NE(fenceVec[index], nullptr);
            ASSERT_NE(eventVec[index], nullptr);
        }
    }

    void RunInference() {
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queueVec[stage::UPLOAD],
                                                    1,
                                                    &listVec[stage::UPLOAD],
                                                    nullptr),
                  ZE_RESULT_SUCCESS)
            << "Failed to execute CommandQueue[UPLOAD]";

        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queueVec[stage::EXECUTE],
                                                    1,
                                                    &listVec[stage::EXECUTE],
                                                    fenceVec[stage::EXECUTE]),
                  ZE_RESULT_SUCCESS)
            << "Failed to execute CommandQueue[EXECUTE]";
    }

    void ReadBack() {
        EXPECT_EQ(zeFenceHostSynchronize(fenceVec[stage::EXECUTE], graphSyncTimeout),
                  ZE_RESULT_SUCCESS)
            << "Failed to synchronize Fence[EXECUTE]";

        EXPECT_EQ(zeCommandQueueExecuteCommandLists(queueVec[stage::READBACK],
                                                    1,
                                                    &listVec[stage::READBACK],
                                                    fenceVec[stage::READBACK]),
                  ZE_RESULT_SUCCESS)
            << "Failed to execute CommandQueue[READBACK]";

        EXPECT_EQ(zeFenceHostSynchronize(fenceVec[stage::READBACK], graphSyncTimeout),
                  ZE_RESULT_SUCCESS)
            << "Failed to synchronize Fence[READBACK]";

        for (auto &fence : fenceVec) {
            EXPECT_EQ(zeFenceReset(fence), ZE_RESULT_SUCCESS) << "Failed to reset Fence";
        }
    }
};

class GraphInference : public GraphInferenceT, public ::testing::WithParamInterface<YAML::Node> {
  protected:
    void SetUp() override {
        GraphInferenceT::SetUp();
        const YAML::Node node = GetParam();

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);

        graph->allocateArguments(MemType::DEVICE_MEMORY);
    }
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(GraphInference);

INSTANTIATE_TEST_SUITE_P(,
                         GraphInference,
                         ::testing::ValuesIn(Environment::getConfiguration("graph_execution")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(GraphInference, InferenceTest) {
    std::vector<std::shared_ptr<void>> mem;
    std::vector<void *> inputHostMem, outputHostMem;

    for (auto size : graph->inputSize) {
        mem.push_back(AllocHostMemory(size));
        inputHostMem.push_back(mem.back().get());
    }

    for (auto size : graph->outputSize) {
        mem.push_back(AllocHostMemory(size));
        outputHostMem.push_back(mem.back().get());
    }

    graph->copyInputData(inputHostMem);

    CreatePipeline(inputHostMem, outputHostMem);

    RunInference();

    ReadBack();

    graph->checkResults(outputHostMem);
};
