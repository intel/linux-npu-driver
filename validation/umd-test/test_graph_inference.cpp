/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "blob_params.hpp"
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

    void GraphInitialize(const ze_graph_desc_2_t graphDesc,
                         std::vector<uint32_t> &sizeGraphInput,
                         std::vector<uint32_t> &sizeGraphOutput) {
        ze_result_t ret;
        scopedGraph =
            zeScope::graphCreate2(zeGraphDDITableExt, zeContext, zeDevice, graphDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create Graph Object";
        graph = scopedGraph.get();

        ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph, &graphProps), ZE_RESULT_SUCCESS)
            << "Failed to get Graph properties";

        for (uint32_t index = 0; index < graphProps.numGraphArgs; ++index) {
            ze_graph_argument_properties_t graphArgProps;

            ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties(graph, index, &graphArgProps),
                      ZE_RESULT_SUCCESS)
                << "Failed to get Graph arg properties";

            size_t expSize = 1u;
            for (int i = 0; i < ZE_MAX_GRAPH_ARGUMENT_DIMENSIONS_SIZE; i++)
                expSize *= graphArgProps.dims[i];

            switch (graphArgProps.devicePrecision) {
            case ZE_GRAPH_ARGUMENT_PRECISION_FP32:
            case ZE_GRAPH_ARGUMENT_PRECISION_INT32:
            case ZE_GRAPH_ARGUMENT_PRECISION_UINT32:
                expSize *= sizeof(uint32_t);
                break;
            case ZE_GRAPH_ARGUMENT_PRECISION_BF16:
            case ZE_GRAPH_ARGUMENT_PRECISION_FP16:
            case ZE_GRAPH_ARGUMENT_PRECISION_INT16:
            case ZE_GRAPH_ARGUMENT_PRECISION_UINT16:
                expSize *= sizeof(uint16_t);
                break;
            case ZE_GRAPH_ARGUMENT_PRECISION_INT8:
            case ZE_GRAPH_ARGUMENT_PRECISION_UINT8:
                expSize *= sizeof(uint8_t);
                break;
            case ZE_GRAPH_ARGUMENT_PRECISION_INT4:
            case ZE_GRAPH_ARGUMENT_PRECISION_UINT4:
                expSize /= 2;
                break;
            default:
                ASSERT_TRUE(false) << "Invalid Graph Argument Precision";
            }

            if (graphArgProps.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
                sizeGraphInput.push_back(expSize);
            } else {
                sizeGraphOutput.push_back(expSize);
            }

            EXPECT_NE(graphArgProps.name, "");
        }

        ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphInitialize(list, graph, nullptr, 0, nullptr),
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

    ze_graph_properties_t graphProps{};
    std::vector<uint32_t> graphInputSize;
    std::vector<uint32_t> graphOutputSize;

  protected:
    ze_graph_handle_t graph = nullptr;

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
    zeScope::SharedPtr<ze_graph_handle_t> scopedGraph = nullptr;
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

    void CreatePipeline(std::vector<void *> &inputHostMem,
                        std::vector<void *> &inputDevMem,
                        std::vector<void *> &outputHostMem,
                        std::vector<void *> &outputDevMem,
                        std::vector<uint32_t> &inputSize,
                        std::vector<uint32_t> &outputSize) {
        ASSERT_TRUE(graph) << "Failed to retrieve graph handle";

        uint32_t argIndex = 0u;
        for (size_t i = 0; i < inputSize.size(); i++) {
            ASSERT_EQ(zeCommandListAppendMemoryCopy(listVec[stage::UPLOAD],
                                                    inputDevMem[i],
                                                    inputHostMem[i],
                                                    inputSize[i],
                                                    nullptr,
                                                    0,
                                                    nullptr),
                      ZE_RESULT_SUCCESS)
                << "Failed to append copy command to CommandList[UPLOAD]";

            ASSERT_EQ(zeGraphDDITableExt->pfnSetArgumentValue(graph, argIndex, inputDevMem[i]),
                      ZE_RESULT_SUCCESS)
                << "Failed to set Graph Object argument #" << argIndex;
            argIndex++;
        }

        ASSERT_EQ(zeCommandListAppendBarrier(listVec[stage::UPLOAD], nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS)
            << "Failed to append barrier command to CommandList[UPLOAD]";

        ASSERT_EQ(zeCommandListAppendSignalEvent(listVec[stage::UPLOAD], eventVec[stage::UPLOAD]),
                  ZE_RESULT_SUCCESS)
            << "Failed to append signal event command to CommandList[UPLOAD]";

        for (size_t i = 0; i < outputSize.size(); i++) {
            ASSERT_EQ(zeCommandListAppendMemoryCopy(listVec[stage::READBACK],
                                                    outputHostMem[i],
                                                    outputDevMem[i],
                                                    outputSize[i],
                                                    nullptr,
                                                    0,
                                                    nullptr),
                      ZE_RESULT_SUCCESS)
                << "Failed to append copy command to CommandList[READBACK]";

            ASSERT_EQ(zeGraphDDITableExt->pfnSetArgumentValue(graph, argIndex, outputDevMem[i]),
                      ZE_RESULT_SUCCESS)
                << "Failed to set Graph Object argument #" << argIndex;
            argIndex++;
        }

        ASSERT_EQ(
            zeCommandListAppendWaitOnEvents(listVec[stage::EXECUTE], 1, &eventVec[stage::UPLOAD]),
            ZE_RESULT_SUCCESS)
            << "Failed to append wait on event command to CommandList[EXECUTE]";

        ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphExecute(listVec[stage::EXECUTE],
                                                            graph,
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

        /* Validate configuration */
        ASSERT_GT(node["path"].as<std::string>().size(), 0);
        ASSERT_GT(node["in"].as<std::vector<std::string>>().size(), 0);
        ASSERT_GT(node["out"].as<std::vector<std::string>>().size(), 0);

        ASSERT_TRUE(getBlobFromPath(blobDir + node["path"].as<std::string>(),
                                    node["in"].as<std::vector<std::string>>(),
                                    node["out"].as<std::vector<std::string>>(),
                                    vpuBlob,
                                    inputBin,
                                    outputBin,
                                    vpuBin));
    }

    std::vector<std::vector<char>> inputBin, outputBin;
    std::vector<char> vpuBlob, vpuBin;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(GraphInference);

INSTANTIATE_TEST_SUITE_P(,
                         GraphInference,
                         ::testing::ValuesIn(Environment::getConfiguration("graph_execution")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(GraphInference, InferenceTest) {
    ze_activation_kernel_desc_t actShaveKernel = {
        .stype = ZE_STRUCTURE_TYPE_GRAPH_ACTIVATION_KERNEL,
        .pNext = nullptr,
        .kernelDataSize = vpuBin.size(),
        .pKernelData = reinterpret_cast<uint8_t *>(vpuBin.data())};

    const ze_graph_desc_2_t graphDesc = {.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES,
                                         .pNext = &actShaveKernel,
                                         .format = ZE_GRAPH_FORMAT_NATIVE,
                                         .inputSize = vpuBlob.size(),
                                         .pInput = reinterpret_cast<uint8_t *>(vpuBlob.data()),
                                         .pBuildFlags = nullptr,
                                         .flags = ZE_GRAPH_FLAG_NONE};

    GraphInitialize(graphDesc, graphInputSize, graphOutputSize);

    ASSERT_EQ(inputBin.size(), graphInputSize.size());
    for (size_t i = 0; i < inputBin.size(); i++) {
        ASSERT_EQ(inputBin[i].size(), graphInputSize[i]);
    }

    ASSERT_EQ(outputBin.size(), graphOutputSize.size());
    for (size_t i = 0; i < outputBin.size(); i++) {
        ASSERT_EQ(outputBin[i].size(), graphOutputSize[i]);
    }

    std::vector<std::shared_ptr<void>> mem;
    std::vector<void *> inputHostMem, outputHostMem, inputDevMem, outputDevMem;
    for (const auto &s : graphInputSize) {
        mem.push_back(AllocHostMemory(s));
        inputHostMem.push_back(mem.back().get());

        mem.push_back(AllocDeviceMemory(s));
        inputDevMem.push_back(mem.back().get());
    }

    for (const auto &s : graphOutputSize) {
        mem.push_back(AllocHostMemory(s));
        outputHostMem.push_back(mem.back().get());

        mem.push_back(AllocDeviceMemory(s));
        outputDevMem.push_back(mem.back().get());
    }

    for (size_t i = 0; i < graphInputSize.size(); i++) {
        memcpy(inputHostMem[i], inputBin[i].data(), inputBin[i].size());
    }

    CreatePipeline(inputHostMem,
                   inputDevMem,
                   outputHostMem,
                   outputDevMem,
                   graphInputSize,
                   graphOutputSize);

    RunInference();

    ReadBack();

    for (size_t i = 0; i < graphOutputSize.size(); i++) {
        EXPECT_EQ(memcmp(outputHostMem[i], outputBin[i].data(), outputBin[i].size()), 0);
    }
};
