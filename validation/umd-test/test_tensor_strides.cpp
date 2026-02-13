/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"
#include "umd_test.h"

class TensorStridesBase : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();
        modelPath = globalConfig.modelDir + "mul_add/mul_add.xml";

        ze_result_t result;
        ze_command_queue_desc_t queueDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                          .pNext = nullptr,
                                          .ordinal = 0u,
                                          .index = 0,
                                          .flags = 0,
                                          .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                          .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};
        queue = zeScope::commandQueueCreate(zeContext, zeDevice, queueDesc, result);
        ASSERT_EQ(ZE_RESULT_SUCCESS, result);

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, modelPath);
        ASSERT_NE(graph, nullptr);
    }

    zeScope::SharedPtr<ze_command_list_handle_t> createCmdList() {
        ze_command_list_desc_t cmdListDesc{
            .stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
            .pNext = nullptr,
            .commandQueueGroupOrdinal = 0u,
            .flags = 0,
        };
        ze_result_t ret;
        return zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    }

  protected:
    std::string modelPath;
    zeScope::SharedPtr<ze_command_queue_handle_t> queue = nullptr;
    std::shared_ptr<Graph> graph = nullptr;
    std::vector<std::string> argumentNames = {"Parameter_1",
                                              "Parameter_2",
                                              "Parameter_3",
                                              "Multiply_4",
                                              "Add_5"};
    std::vector<std::string> friendlyNames = {"Parameter_1",
                                              "Parameter_2",
                                              "Parameter_3",
                                              "Result_6",
                                              "Result_7"};
};

using StridesMap = std::unordered_map<uint32_t, std::array<uint32_t, 5>>;

static std::vector<StridesMap> getParams() {
    return {
        // strides in second input
        {
            {1, {2, 20, 240, 1920}},
        },
        // strides in first output
        {
            {3, {1, 40, 480, 1920}},
        },
        // strides in first input and second output
        {
            {0, {2, 20, 480, 1920}},
            {4, {3, 20, 240, 3840}},
        },
        // all inputs and outputs are strided
        {
            {0, {1, 20, 240, 1920}},
            {1, {2, 20, 240, 1920}},
            {2, {1, 60, 240, 1920}},
            {3, {1, 20, 720, 1920}},
            {4, {1, 20, 240, 3840}},
        },
    };
}

// TODO: EISW-197317
class DISABLED_TensorStrides : public TensorStridesBase,
                               public testing::WithParamInterface<StridesMap> {};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(DISABLED_TensorStrides);
INSTANTIATE_TEST_SUITE_P(,
                         DISABLED_TensorStrides,
                         testing::ValuesIn(getParams()),
                         [](const testing::TestParamInfo<StridesMap> &info) {
                             std::string str;
                             for (const auto &[i, strides] : info.param) {
                                 str += "argument_" + std::to_string(i) + "_strides";
                                 for (auto s : strides) {
                                     str += "_" + std::to_string(s);
                                 }
                                 str += "_";
                             }
                             return str;
                         });

static void initializeBuffer(float16 *input, size_t size, size_t start) {
    for (size_t i = 0; i < size; i++) {
        input[i] = (start + i) % 100;
    }
}

static void verifyOutputs(const std::vector<float16 *> &inputs,
                          const std::vector<float16 *> &outputs,
                          const std::vector<uint32_t> &shape,
                          const StridesMap &stridesMap = {}) {
    size_t numInputs = inputs.size();
    auto getOff = [&](size_t argIdx, size_t i, size_t j, size_t k) {
        auto it = stridesMap.find(argIdx);
        if (it != stridesMap.end()) {
            const auto &strides = it->second;
            size_t off = i * strides[2] + j * strides[1] + k * strides[0];
            return off;
        }
        size_t off = (i * shape[1] + j) * shape[2] + k;
        return off;
    };

    for (size_t i = 0; i < shape[0]; i++) {
        for (size_t j = 0; j < shape[1]; j++) {
            for (size_t k = 0; k < shape[2]; k++) {
                float16 in0 = inputs[0][getOff(0, i, j, k)];
                float16 in1 = inputs[1][getOff(1, i, j, k)];
                float16 in2 = inputs[2][getOff(2, i, j, k)];
                float16 out0 = outputs[0][getOff(numInputs, i, j, k)];
                float16 out1 = outputs[1][getOff(numInputs + 1, i, j, k)];
                float16 ref0 = in0 * in1;
                float16 ref1 = ref0 + in2;
                EXPECT_EQ(ref0, out0);
                EXPECT_EQ(ref1, out1);
            }
        }
    }
}

TEST_P(DISABLED_TensorStrides, SetTensorTogetherWithStrides) {
    const auto &stridesMap = GetParam();
    std::vector<uint32_t> fullShape = {8, 12, 20};
    std::vector<uint32_t> slice = {2, 3, 4};

    size_t fullSize =
        std::accumulate(fullShape.begin(), fullShape.end(), 1, std::multiplies<uint32_t>());
    size_t sliceSize = std::accumulate(slice.begin(), slice.end(), 1, std::multiplies<uint32_t>());

    size_t numInputs = graph->inputSize.size();
    size_t numOutputs = graph->outputSize.size();
    std::vector<float16 *> inputs;
    std::vector<float16 *> outputs;

    for (size_t i = 0; i < numInputs; i++) {
        auto strides = stridesMap.find(i);
        bool hasStrides = strides != stridesMap.end();
        size_t size = hasStrides ? fullSize : sliceSize;
        inputs.push_back(static_cast<float16 *>(
            graph->allocMemory(size * sizeof(float16), MemType::SHARED_MEMORY)));
        initializeBuffer(inputs[i], size, i + 1);
        if (hasStrides) {
            ASSERT_EQ(ZE_RESULT_SUCCESS, graph->setArgumentValue2(i, inputs[i], strides->second));
        } else {
            ASSERT_EQ(ZE_RESULT_SUCCESS, graph->setArgumentValue2(i, inputs[i]));
        }
    }

    for (size_t i = 0; i < numOutputs; i++) {
        auto strides = stridesMap.find(i + numInputs);
        bool hasStrides = strides != stridesMap.end();
        size_t size = hasStrides ? fullSize : sliceSize;
        outputs.push_back(static_cast<float16 *>(
            graph->allocMemory(size * sizeof(float16), MemType::SHARED_MEMORY)));
        if (hasStrides) {
            ASSERT_EQ(ZE_RESULT_SUCCESS,
                      graph->setArgumentValue2(i + numInputs, outputs[i], strides->second));
        } else {
            ASSERT_EQ(ZE_RESULT_SUCCESS, graph->setArgumentValue2(i + numInputs, outputs[i]));
        }
    }

    zeScope::SharedPtr<ze_command_list_handle_t> scopedList;
    ASSERT_NE(nullptr, scopedList = createCmdList());
    ze_command_list_handle_t cmdList = scopedList.get();

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeGraphDDITableExt
                  ->pfnAppendGraphExecute(cmdList, graph->handle, nullptr, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(cmdList));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeCommandQueueExecuteCommandLists(queue.get(), 1, &cmdList, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandQueueSynchronize(queue.get(), graphSyncTimeout));

    verifyOutputs(inputs, outputs, slice, stridesMap);
}

// TODO: EISW-197317
class DISABLED_TensorStridesMutableCmdList : public DISABLED_TensorStrides {
  public:
    zeScope::SharedPtr<ze_command_list_handle_t> createMutableCmdList() {
        ze_mutable_command_list_exp_desc_t mutableCmdListDesc{
            .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_LIST_EXP_DESC,
            .pNext = nullptr,
            .flags = 0,
        };
        ze_command_list_desc_t cmdListDesc{
            .stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
            .pNext = &mutableCmdListDesc,
            .commandQueueGroupOrdinal = 0u,
            .flags = 0,
        };
        ze_result_t ret;
        return zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    }
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(DISABLED_TensorStridesMutableCmdList);
INSTANTIATE_TEST_SUITE_P(,
                         DISABLED_TensorStridesMutableCmdList,
                         testing::ValuesIn(getParams()),
                         [](const testing::TestParamInfo<StridesMap> &info) {
                             std::string str;
                             for (const auto &[i, strides] : info.param) {
                                 str += "argument_" + std::to_string(i) + "_strides";
                                 for (auto s : strides) {
                                     str += "_" + std::to_string(s);
                                 }
                                 str += "_";
                             }
                             return str;
                         });

TEST_P(DISABLED_TensorStridesMutableCmdList, AddStridesThenUpdateStridesAndTensor) {
    auto stridesMap = GetParam();
    std::vector<uint32_t> shape = {8, 12, 20};
    std::vector<uint32_t> slice = {2, 3, 4};

    size_t fullSize = std::accumulate(shape.begin(), shape.end(), 1, std::multiplies<uint32_t>());
    size_t sliceSize = std::accumulate(slice.begin(), slice.end(), 1, std::multiplies<uint32_t>());

    size_t numInputs = graph->inputSize.size();
    size_t numOutputs = graph->outputSize.size();
    std::vector<float16 *> inputs;
    std::vector<float16 *> outputs;

    for (size_t i = 0; i < numInputs; i++) {
        auto strides = stridesMap.find(i);
        bool hasStrides = strides != stridesMap.end();
        size_t size = hasStrides ? fullSize : sliceSize;
        inputs.push_back(static_cast<float16 *>(
            graph->allocMemory(size * sizeof(float16), MemType::SHARED_MEMORY)));
        initializeBuffer(inputs[i], size, i + 1);
        ASSERT_EQ(ZE_RESULT_SUCCESS, graph->setArgumentValue2(i, inputs[i]));
    }

    for (size_t i = 0; i < numOutputs; i++) {
        auto strides = stridesMap.find(i + numInputs);
        bool hasStrides = strides != stridesMap.end();
        size_t size = hasStrides ? fullSize : sliceSize;
        outputs.push_back(static_cast<float16 *>(
            graph->allocMemory(size * sizeof(float16), MemType::SHARED_MEMORY)));
        ASSERT_EQ(ZE_RESULT_SUCCESS, graph->setArgumentValue2(i + numInputs, outputs[i]));
    }

    zeScope::SharedPtr<ze_command_list_handle_t> scopedList;
    ASSERT_NE(nullptr, scopedList = createMutableCmdList());
    ze_command_list_handle_t cmdList = scopedList.get();

    ze_mutable_command_id_exp_desc_t mutableCmdIdDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_ID_EXP_DESC,
        .pNext = nullptr,
        .flags = ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS,
    };
    uint64_t commandId;
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListGetNextCommandIdExp(cmdList, &mutableCmdIdDesc, &commandId));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeGraphDDITableExt
                  ->pfnAppendGraphExecute(cmdList, graph->handle, nullptr, nullptr, 0, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(cmdList));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeCommandQueueExecuteCommandLists(queue.get(), 1, &cmdList, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandQueueSynchronize(queue.get(), graphSyncTimeout));

    verifyOutputs(inputs, outputs, slice);

    // update just strides
    std::vector<ze_graph_argument_value_strides_t> graphStridesDescs(stridesMap.size());
    std::vector<ze_mutable_graph_argument_exp_desc_t> graphArgumentDescs(stridesMap.size());
    auto stridesDesc = graphStridesDescs.begin();
    auto argumentDesc = graphArgumentDescs.begin();
    void *pNext = nullptr;
    for (const auto &[argIndex, strides] : stridesMap) {
        stridesDesc->stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_STRIDES;
        stridesDesc->pNext = pNext;
        for (size_t i = 0; i < strides.size(); i++) {
            stridesDesc->userStrides[i] = strides[i];
        }
        argumentDesc->stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC;
        argumentDesc->pNext = &(*stridesDesc);
        argumentDesc->commandId = commandId;
        argumentDesc->argIndex = argIndex;
        if (argIndex < numInputs) {
            argumentDesc->pArgValue = inputs[argIndex];
        } else {
            argumentDesc->pArgValue = outputs[argIndex - numInputs];
        }
        pNext = &(*argumentDesc);
        stridesDesc++;
        argumentDesc++;
    }
    ze_mutable_commands_exp_desc_t mutableCommandsDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMANDS_EXP_DESC,
        .pNext = &(*(argumentDesc - 1)),
        .flags = 0,
    };
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListUpdateMutableCommandsExp(cmdList, &mutableCommandsDesc));
    ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(cmdList));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeCommandQueueExecuteCommandLists(queue.get(), 1, &cmdList, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandQueueSynchronize(queue.get(), graphSyncTimeout));

    verifyOutputs(inputs, outputs, slice, stridesMap);

    graphArgumentDescs.resize(numInputs + numOutputs);
    stridesDesc = graphStridesDescs.begin();
    argumentDesc = graphArgumentDescs.begin();
    pNext = nullptr;
    for (size_t i = 0; i < numInputs + numOutputs; i++) {
        if (stridesMap.count(i)) {
            auto &strides = stridesMap[i];
            stridesDesc->pNext = pNext;
            for (size_t j = 0; j < strides.size(); j++) {
                strides[j]++;
                stridesDesc->userStrides[j] = strides[j];
            }
            pNext = &(*stridesDesc);
            stridesDesc++;
        }
        argumentDesc->stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC;
        argumentDesc->pNext = pNext;
        argumentDesc->argIndex = i;
        argumentDesc->commandId = commandId;
        if (i < numInputs) {
            inputs[i] = static_cast<float16 *>(
                graph->allocMemory(fullSize * sizeof(float16), MemType::SHARED_MEMORY));
            initializeBuffer(inputs[i], fullSize, i + 10);
            argumentDesc->pArgValue = inputs[i];
        } else {
            outputs[i - numInputs] = static_cast<float16 *>(
                graph->allocMemory(fullSize * sizeof(float16), MemType::SHARED_MEMORY));
            argumentDesc->pArgValue = outputs[i - numInputs];
        }
        pNext = &(*argumentDesc);
        argumentDesc++;
    }

    mutableCommandsDesc.pNext = &(*(argumentDesc - 1));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListUpdateMutableCommandsExp(cmdList, &mutableCommandsDesc));
    ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(cmdList));
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeCommandQueueExecuteCommandLists(queue.get(), 1, &cmdList, nullptr));
    ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandQueueSynchronize(queue.get(), graphSyncTimeout));

    verifyOutputs(inputs, outputs, slice, stridesMap);
}

// TODO: EISW-197317
using DISABLED_StridesProperty = TensorStridesBase;

TEST_F(DISABLED_StridesProperty, GetArgumentProperties) {
    ze_graph_properties_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;
    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";

    ze_graph_argument_property_strides_t stridesProp = {};
    stridesProp.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTY_STRIDES;
    ze_graph_argument_properties_t graphArgumentProps = {};
    graphArgumentProps.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES;
    graphArgumentProps.pNext = &stridesProp;

    for (uint32_t index = 0; index < graphProps.numGraphArgs; index++) {
        ASSERT_EQ(
            zeGraphDDITableExt->pfnGetArgumentProperties(graph->handle, index, &graphArgumentProps),
            ZE_RESULT_SUCCESS);
        if (index < graphProps.numGraphArgs - 2) {
            ASSERT_EQ(graphArgumentProps.type, ZE_GRAPH_ARGUMENT_TYPE_INPUT);
        } else if (index >= graphProps.numGraphArgs - 2) {
            ASSERT_EQ(graphArgumentProps.type, ZE_GRAPH_ARGUMENT_TYPE_OUTPUT);
        } else if (graphArgumentProps.type != ZE_GRAPH_ARGUMENT_TYPE_INPUT &&
                   graphArgumentProps.type != ZE_GRAPH_ARGUMENT_TYPE_OUTPUT) {
            FAIL() << "Invalid graph argument type";
        }
        ASSERT_EQ(graphArgumentProps.dims[0], 2u);
        ASSERT_EQ(graphArgumentProps.dims[1], 3u);
        ASSERT_EQ(graphArgumentProps.dims[2], 4u);
        ASSERT_EQ(graphArgumentProps.networkLayout, ZE_GRAPH_ARGUMENT_LAYOUT_CHW);
        ASSERT_EQ(graphArgumentProps.networkPrecision, ZE_GRAPH_ARGUMENT_PRECISION_FP16);
        ASSERT_EQ(graphArgumentProps.deviceLayout, ZE_GRAPH_ARGUMENT_LAYOUT_CHW);
        ASSERT_EQ(graphArgumentProps.devicePrecision, ZE_GRAPH_ARGUMENT_PRECISION_FP16);
        ASSERT_TRUE(stridesProp.supportsDynamicStrides);
    }
}

TEST_F(DISABLED_StridesProperty, GetArgumentProperties2) {
    ze_graph_properties_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;
    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";

    ze_graph_argument_property_strides_t stridesProp = {};
    stridesProp.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTY_STRIDES;
    ze_graph_argument_properties_2_t graphArgumentProps = {};
    graphArgumentProps.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES_2;
    graphArgumentProps.pNext = &stridesProp;

    for (uint32_t index = 0; index < graphProps.numGraphArgs; index++) {
        ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties2(graph->handle,
                                                                index,
                                                                &graphArgumentProps),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(argumentNames[index], graphArgumentProps.name);
        if (index < graphProps.numGraphArgs - 2) {
            ASSERT_EQ(graphArgumentProps.type, ZE_GRAPH_ARGUMENT_TYPE_INPUT);
        } else if (index >= graphProps.numGraphArgs - 2) {
            ASSERT_EQ(graphArgumentProps.type, ZE_GRAPH_ARGUMENT_TYPE_OUTPUT);
        } else if (graphArgumentProps.type != ZE_GRAPH_ARGUMENT_TYPE_INPUT &&
                   graphArgumentProps.type != ZE_GRAPH_ARGUMENT_TYPE_OUTPUT) {
            FAIL() << "Invalid graph argument type";
        }
        ASSERT_EQ(graphArgumentProps.dims[0], 2u);
        ASSERT_EQ(graphArgumentProps.dims[1], 3u);
        ASSERT_EQ(graphArgumentProps.dims[2], 4u);
        ASSERT_EQ(graphArgumentProps.networkLayout, ZE_GRAPH_ARGUMENT_LAYOUT_CHW);
        ASSERT_EQ(graphArgumentProps.networkPrecision, ZE_GRAPH_ARGUMENT_PRECISION_FP16);
        ASSERT_EQ(graphArgumentProps.deviceLayout, ZE_GRAPH_ARGUMENT_LAYOUT_CHW);
        ASSERT_EQ(graphArgumentProps.devicePrecision, ZE_GRAPH_ARGUMENT_PRECISION_FP16);
        if (graphArgumentProps.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
            ASSERT_EQ(graphArgumentProps.quantReverseScale, 1);
        } else {
            ASSERT_EQ(graphArgumentProps.quantReverseScale, 0);
        }
        ASSERT_EQ(graphArgumentProps.quantZeroPoint, 0);
        ASSERT_TRUE(stridesProp.supportsDynamicStrides);
    }
}

TEST_F(DISABLED_StridesProperty, GetArgumentProperties3) {
    ze_graph_properties_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;
    ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties(graph->handle, &graphProps), ZE_RESULT_SUCCESS)
        << "Failed to get Graph properties";

    ze_graph_argument_property_strides_t stridesProp = {};
    stridesProp.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTY_STRIDES;
    ze_graph_argument_properties_3_t graphArgumentProps = {};
    graphArgumentProps.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES_3;
    graphArgumentProps.pNext = &stridesProp;

    for (uint32_t index = 0; index < graphProps.numGraphArgs; index++) {
        ASSERT_EQ(zeGraphDDITableExt->pfnGetArgumentProperties3(graph->handle,
                                                                index,
                                                                &graphArgumentProps),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(argumentNames[index], graphArgumentProps.name);
        if (index < graphProps.numGraphArgs - 2) {
            ASSERT_EQ(graphArgumentProps.type, ZE_GRAPH_ARGUMENT_TYPE_INPUT);
        } else if (index >= graphProps.numGraphArgs - 2) {
            ASSERT_EQ(graphArgumentProps.type, ZE_GRAPH_ARGUMENT_TYPE_OUTPUT);
        } else if (graphArgumentProps.type != ZE_GRAPH_ARGUMENT_TYPE_INPUT &&
                   graphArgumentProps.type != ZE_GRAPH_ARGUMENT_TYPE_OUTPUT) {
            FAIL() << "Invalid graph argument type";
        }
        ASSERT_EQ(graphArgumentProps.dims[0], 2u);
        ASSERT_EQ(graphArgumentProps.dims[1], 3u);
        ASSERT_EQ(graphArgumentProps.dims[2], 4u);
        ASSERT_EQ(graphArgumentProps.networkLayout, ZE_GRAPH_ARGUMENT_LAYOUT_CHW);
        ASSERT_EQ(graphArgumentProps.networkPrecision, ZE_GRAPH_ARGUMENT_PRECISION_FP16);
        ASSERT_EQ(graphArgumentProps.deviceLayout, ZE_GRAPH_ARGUMENT_LAYOUT_CHW);
        ASSERT_EQ(graphArgumentProps.devicePrecision, ZE_GRAPH_ARGUMENT_PRECISION_FP16);
        if (graphArgumentProps.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
            ASSERT_EQ(graphArgumentProps.quantReverseScale, 1);
        } else {
            ASSERT_EQ(graphArgumentProps.quantReverseScale, 0);
        }
        ASSERT_EQ(graphArgumentProps.quantZeroPoint, 0);
        ASSERT_EQ(graphArgumentProps.dims_count, 3);
        ASSERT_EQ(friendlyNames[index], graphArgumentProps.debug_friendly_name);
        ASSERT_TRUE(stridesProp.supportsDynamicStrides);
    }
}
