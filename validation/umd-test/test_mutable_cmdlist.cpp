/*
 * Copyright (C) 2024-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"
#include "openvino/core/graph_util.hpp"
#include "openvino/core/model.hpp"
#include "openvino/op/add.hpp"
#include "openvino/op/multiply.hpp"
#include "openvino/op/parameter.hpp"
#include "umd_test.h"

struct ModelSetup {
    ModelSetup() = default;
    ModelSetup(ModelSetup &&) = delete;
    ModelSetup(const ModelSetup &) = delete;
    ModelSetup &operator=(const ModelSetup &) = delete;
    ModelSetup &operator=(ModelSetup &&) = delete;

    void createModel() {
        if (!modelPath.empty())
            return;

        // model with 3 inputs and 2 outputs:
        //
        // +----------+        +----------+
        // |  Input   |        |  Input   |
        // | (param1) |        | (param2) |
        // +----+-----+        +----+-----+
        //      |                   |
        //      +-------+   +-------+
        //              |   |
        //              v   v              +----------+
        //           +----------+          |  Input   |
        //           | Multiply |          | (param3) |
        //           +----+-----+          +----+-----+
        //                |                     |
        //                +---------+   +-------+
        //                |         |   |
        //                |         v   v
        //                |      +---------+
        //                |      |   Add   |
        //                |      +----+----+
        //                |           |
        //                v           v
        //           +--------+  +--------+
        //           | Output |  | Output |
        //           | (mul)  |  | (add)  |
        //           +--------+  +--------+
        //
        auto param1 = std::make_shared<ov::op::v0::Parameter>(ov::element::f16, ov::Shape{2, 3});
        auto param2 = std::make_shared<ov::op::v0::Parameter>(ov::element::f16, ov::Shape{2, 3});
        auto param3 = std::make_shared<ov::op::v0::Parameter>(ov::element::f16, ov::Shape{2, 3});
        auto mul = std::make_shared<ov::op::v1::Multiply>(param1, param2);
        auto add = std::make_shared<ov::op::v1::Add>(mul, param3);

        auto model = std::make_shared<ov::Model>(
            ov::NodeVector{std::move(mul), std::move(add)},
            ov::ParameterVector{std::move(param1), std::move(param2), std::move(param3)});

        modelPath = createModelPath();
        ASSERT_FALSE(modelPath.empty());

        ov::serialize(model, modelPath);

        buildFlags = createBuildFlags(model);
    }

    std::filesystem::path createModelPath() {
        char modelPathTemplate[] = "/tmp/tmpmodel-XXXXXX";
        int fd = mkstemp(modelPathTemplate);
        if (fd == -1)
            return {};
        close(fd);
        std::filesystem::remove(modelPathTemplate);
        return std::string(modelPathTemplate) + ".xml";
    }

    std::string createBuildFlags(const std::shared_ptr<ov::Model> &model) {
        std::string inputPrecisions;
        std::string inputLayouts;

        for (const auto &param : model->get_parameters()) {
            const auto &name = param->get_friendly_name();
            inputPrecisions += name + ":fp16 ";
            inputLayouts += name + ":NC ";
        }

        std::string outputPrecisions;
        std::string outputLayouts;

        for (const auto &result : model->get_results()) {
            const auto &name = result->get_input_node_ptr(0)->get_friendly_name();
            outputPrecisions += name + ":fp16 ";
            outputLayouts += name + ":NC ";
        }

        return "--inputs_precisions=\"" + inputPrecisions + "\" " + "--inputs_layouts=\"" +
               inputLayouts + "\" " + "--outputs_precisions=\"" + outputPrecisions + "\" " +
               "--outputs_layouts=\"" + outputLayouts + "\"";
    }

    ~ModelSetup() {
        if (modelPath.empty())
            return;
        std::filesystem::remove(modelPath);
        std::filesystem::remove(modelPath.replace_extension(".bin"));
    }

    std::filesystem::path modelPath;
    std::string buildFlags;
};

static ModelSetup modelSetup;

class MutableCmdList : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();
        ze_mutable_command_list_exp_properties_t mutableCmdListProps{
            .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_LIST_EXP_PROPERTIES,
            .pNext = nullptr,
            .mutableCommandListFlags = 0,
            .mutableCommandFlags = 0,
        };
        ze_device_properties_t devProp = {};
        devProp.stype = ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES;
        devProp.pNext = &mutableCmdListProps;
        ze_result_t result = zeDeviceGetProperties(zeDevice, &devProp);
        ASSERT_EQ(ZE_RESULT_SUCCESS, result);
        ASSERT_EQ(ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENT_DEPRECATED |
                      ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS,
                  mutableCmdListProps.mutableCommandFlags);

        ze_command_queue_desc_t cmdQueueDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                             .pNext = nullptr,
                                             .ordinal = 0u,
                                             .index = 0,
                                             .flags = 0,
                                             .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                             .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};
        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, result);
        ASSERT_EQ(ZE_RESULT_SUCCESS, result);

        queue = scopedQueue.get();

        modelSetup.createModel();
    }

    ze_command_list_handle_t createMutableCmdList() {
        ze_mutable_command_list_exp_desc_t mutableCmdListDesc{
            .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_LIST_EXP_DESC,
            .pNext = nullptr,
            .flags = 0,
        };
        ze_command_list_desc_t commandListDesc{
            .stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
            .pNext = &mutableCmdListDesc,
            .commandQueueGroupOrdinal = 0u,
            .flags = 0,
        };
        ze_command_list_handle_t commandList;
        ze_result_t result =
            zeCommandListCreate(zeContext, zeDevice, &commandListDesc, &commandList);
        if (result != ZE_RESULT_SUCCESS)
            return nullptr;

        return commandList;
    }

  protected:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    ze_command_queue_handle_t queue;
};

// struct that represents float16 type and provides implicit conversions from/to float.
struct float16 {
    uint16_t value;

    float16(float f) {
        // use 32-bit `tmp` variable (instead of 16-bit `value`) as an argument for movss
        // to make sure movss instruction doesn't write out of bounds
        uint32_t tmp = 0;
        asm("movss %1, %%xmm6\n"
            "vcvtps2ph $0, %%xmm6, %%xmm7\n"
            "movss %%xmm7, %0\n"
            : "=m"(tmp)
            : "m"(f)
            : "xmm6", "xmm7");
        value = static_cast<uint16_t>(tmp);
    }

    operator float() const {
        float ret = 0;
        uint32_t tmp = static_cast<uint32_t>(value);

        asm("movss %1, %%xmm6\n"
            "vcvtph2ps %%xmm6, %%xmm7\n"
            "movss %%xmm7, %0\n"
            : "=m"(ret)
            : "m"(tmp)
            : "xmm6", "xmm7");

        return ret;
    }
};
static_assert(sizeof(float16) == 2);

static void
verifyOutputs(const std::vector<void *> &inputs, const std::vector<void *> &outputs, size_t n) {
    float16 *input0 = reinterpret_cast<float16 *>(inputs[0]);
    float16 *input1 = reinterpret_cast<float16 *>(inputs[1]);
    float16 *input2 = reinterpret_cast<float16 *>(inputs[2]);

    const float16 *output0 = reinterpret_cast<const float16 *>(outputs[0]);
    const float16 *output1 = reinterpret_cast<const float16 *>(outputs[1]);

    for (size_t i = 0; i < n / sizeof(float16); i++) {
        float16 out0 = input0[i] * input1[i];
        float16 out1 = out0 + input2[i];
        // so far, the output values are small integers so there's no need
        // for comparison with epsilon
        EXPECT_EQ(out0, output0[i]);
        EXPECT_EQ(out1, output1[i]);
    }
}

// a very basic scenario:
// - create a graph
// - execute command list and check results
// - update graph's first argument
// - execute command list again and check results
TEST_F(MutableCmdList, UpdateGraphFirstInput) {
    auto graph = Graph::create(zeContext,
                               zeDevice,
                               zeGraphDDITableExt,
                               modelSetup.modelPath,
                               modelSetup.buildFlags);
    ASSERT_NE(graph, nullptr);

    // allocate arguments and fill inputs
    graph->allocateArguments(MemType::SHARED_MEMORY);

    float16 *input0 = reinterpret_cast<float16 *>(graph->inArgs[0]);
    float16 *input1 = reinterpret_cast<float16 *>(graph->inArgs[1]);
    float16 *input2 = reinterpret_cast<float16 *>(graph->inArgs[2]);

    std::iota(input0, input0 + graph->inputSize[0] / sizeof(float16), 0);
    std::iota(input1, input1 + graph->inputSize[1] / sizeof(float16), 1);
    std::iota(input2, input2 + graph->inputSize[2] / sizeof(float16), 2);

    ze_command_list_handle_t commandList = createMutableCmdList();
    ASSERT_NE(nullptr, commandList);

    // get the id of graph execute command
    ze_mutable_command_id_exp_desc_t mutableCmdIdDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_ID_EXP_DESC,
        .pNext = nullptr,
        .flags = ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS,
    };
    uint64_t commandId;
    ze_result_t result =
        zeCommandListGetNextCommandIdExp(commandList, &mutableCmdIdDesc, &commandId);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeGraphDDITableExt
                 ->pfnAppendGraphExecute(commandList, graph->handle, nullptr, nullptr, 0, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandListClose(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueExecuteCommandLists(queue, 1, &commandList, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueSynchronize(queue, graphSyncTimeout);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    verifyOutputs(graph->inArgs, graph->outArgs, graph->outputSize[0]);

    // next, the test allocates a new input buffer and uses it to update graph's first argument

    float16 *newInput0 = reinterpret_cast<float16 *>(
        graph->allocMemory(graph->inputSize[0], MemType::SHARED_MEMORY));
    ASSERT_NE(nullptr, newInput0);

    std::iota(newInput0, newInput0 + graph->inputSize[0] / sizeof(float16), -2.0f);

    // double check it's not the same as previous input
    ASSERT_NE(0, memcmp(input0, newInput0, graph->inputSize[0]));

    ze_mutable_graph_argument_exp_desc_t graphArgumentDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
        .pNext = nullptr,
        .commandId = commandId,
        .argIndex = 0,
        .pArgValue = newInput0,
    };
    ze_mutable_commands_exp_desc_t mutableCommandsDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMANDS_EXP_DESC,
        .pNext = &graphArgumentDesc,
        .flags = 0,
    };
    result = zeCommandListUpdateMutableCommandsExp(commandList, &mutableCommandsDesc);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandListClose(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueExecuteCommandLists(queue, 1, &commandList, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueSynchronize(queue, graphSyncTimeout);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    verifyOutputs({newInput0, graph->inArgs[1], graph->inArgs[2]},
                  graph->outArgs,
                  graph->outputSize[0]);

    result = zeCommandListDestroy(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);
}

// The test does:
// - create a graph
// - execute command list and check results
// - update graph's second and third input buffer and second output buffer
// - execute command list again and check results
TEST_F(MutableCmdList, UpdateGraphInputsAndOutputs) {
    auto graph = Graph::create(zeContext,
                               zeDevice,
                               zeGraphDDITableExt,
                               modelSetup.modelPath,
                               modelSetup.buildFlags);
    ASSERT_NE(graph, nullptr);

    graph->allocateArguments(MemType::SHARED_MEMORY);

    float16 *input0 = reinterpret_cast<float16 *>(graph->inArgs[0]);
    float16 *input1 = reinterpret_cast<float16 *>(graph->inArgs[1]);
    float16 *input2 = reinterpret_cast<float16 *>(graph->inArgs[2]);

    std::iota(input0, input0 + graph->inputSize[0] / sizeof(float16), 0);
    std::iota(input1, input1 + graph->inputSize[1] / sizeof(float16), 1);
    std::iota(input2, input2 + graph->inputSize[2] / sizeof(float16), 2);

    ze_command_list_handle_t commandList = createMutableCmdList();
    ASSERT_NE(nullptr, commandList);

    // get the id of graph execute command
    ze_mutable_command_id_exp_desc_t mutableCmdIdDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_ID_EXP_DESC,
        .pNext = nullptr,
        .flags = ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS,
    };
    uint64_t commandId;
    ze_result_t result =
        zeCommandListGetNextCommandIdExp(commandList, &mutableCmdIdDesc, &commandId);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeGraphDDITableExt
                 ->pfnAppendGraphExecute(commandList, graph->handle, nullptr, nullptr, 0, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandListClose(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueExecuteCommandLists(queue, 1, &commandList, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueSynchronize(queue, graphSyncTimeout);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    verifyOutputs(graph->inArgs, graph->outArgs, graph->outputSize[0]);

    // create new input buffers for second and third input and new buffer for second output.
    // use those buffers to update graph's arguments

    float16 *newInput1 = reinterpret_cast<float16 *>(
        graph->allocMemory(graph->inputSize[1], MemType::SHARED_MEMORY));
    float16 *newInput2 = reinterpret_cast<float16 *>(
        graph->allocMemory(graph->inputSize[2], MemType::SHARED_MEMORY));
    float16 *newOutput1 = reinterpret_cast<float16 *>(
        graph->allocMemory(graph->outputSize[1], MemType::SHARED_MEMORY));

    std::iota(newInput1, newInput1 + graph->inputSize[1] / sizeof(float), -2.0f);
    std::iota(newInput2, newInput2 + graph->inputSize[2] / sizeof(float), -3.0f);

    ASSERT_NE(0, memcmp(input1, newInput1, graph->inputSize[1]));
    ASSERT_NE(0, memcmp(input2, newInput2, graph->inputSize[2]));

    // update graph's arguments
    ze_mutable_graph_argument_exp_desc_t secondInputDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
        .pNext = nullptr,
        .commandId = commandId,
        .argIndex = 1,
        .pArgValue = newInput1,
    };
    ze_mutable_graph_argument_exp_desc_t thirdInputDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
        .pNext = &secondInputDesc,
        .commandId = commandId,
        .argIndex = 2,
        .pArgValue = newInput2,
    };
    ze_mutable_graph_argument_exp_desc_t secondOutputDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
        .pNext = &thirdInputDesc,
        .commandId = commandId,
        .argIndex = 4,
        .pArgValue = newOutput1,
    };
    ze_mutable_commands_exp_desc_t mutableCommandsDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMANDS_EXP_DESC,
        .pNext = &secondOutputDesc,
        .flags = 0,
    };
    result = zeCommandListUpdateMutableCommandsExp(commandList, &mutableCommandsDesc);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandListClose(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueExecuteCommandLists(queue, 1, &commandList, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueSynchronize(queue, graphSyncTimeout);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    verifyOutputs({graph->inArgs[0], newInput1, newInput2},
                  {graph->outArgs[0], newOutput1},
                  graph->outputSize[0]);

    result = zeCommandListDestroy(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);
}

// Test does:
// - create two graphs in the same command list
// - execute command list and check results
// - update second input in the first graph
// - update third input in the second graph
// - execute command list again and check results
TEST_F(MutableCmdList, UpdateTwoGraphsInSingleCmdList) {
    auto graph1 = Graph::create(zeContext,
                                zeDevice,
                                zeGraphDDITableExt,
                                modelSetup.modelPath,
                                modelSetup.buildFlags);
    ASSERT_NE(graph1, nullptr);

    graph1->allocateArguments(MemType::SHARED_MEMORY);
    {
        float16 *input0 = reinterpret_cast<float16 *>(graph1->inArgs[0]);
        float16 *input1 = reinterpret_cast<float16 *>(graph1->inArgs[1]);
        float16 *input2 = reinterpret_cast<float16 *>(graph1->inArgs[2]);

        std::iota(input0, input0 + graph1->inputSize[0] / sizeof(float16), 0);
        std::iota(input1, input1 + graph1->inputSize[1] / sizeof(float16), 1);
        std::iota(input2, input2 + graph1->inputSize[2] / sizeof(float16), 2);
    }

    auto graph2 = Graph::create(zeContext,
                                zeDevice,
                                zeGraphDDITableExt,
                                modelSetup.modelPath,
                                modelSetup.buildFlags);
    ASSERT_NE(graph2, nullptr);

    graph2->allocateArguments(MemType::SHARED_MEMORY);
    {
        float16 *input0 = reinterpret_cast<float16 *>(graph2->inArgs[0]);
        float16 *input1 = reinterpret_cast<float16 *>(graph2->inArgs[1]);
        float16 *input2 = reinterpret_cast<float16 *>(graph2->inArgs[2]);

        std::iota(input0, input0 + graph2->inputSize[0] / sizeof(float16), 3);
        std::iota(input1, input1 + graph2->inputSize[1] / sizeof(float16), 4);
        std::iota(input2, input2 + graph2->inputSize[2] / sizeof(float16), 5);
    }

    ze_command_list_handle_t commandList = createMutableCmdList();
    ASSERT_NE(nullptr, commandList);

    ze_mutable_command_id_exp_desc_t mutableCmdIdDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_ID_EXP_DESC,
        .pNext = nullptr,
        .flags = ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS,
    };

    // get the id of first graph execute command
    uint64_t graph1CommandId;
    ze_result_t result =
        zeCommandListGetNextCommandIdExp(commandList, &mutableCmdIdDesc, &graph1CommandId);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeGraphDDITableExt
                 ->pfnAppendGraphExecute(commandList, graph1->handle, nullptr, nullptr, 0, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    // get the id of second graph execute command
    uint64_t graph2CommandId;
    result = zeCommandListGetNextCommandIdExp(commandList, &mutableCmdIdDesc, &graph2CommandId);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeGraphDDITableExt
                 ->pfnAppendGraphExecute(commandList, graph2->handle, nullptr, nullptr, 0, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandListClose(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueExecuteCommandLists(queue, 1, &commandList, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueSynchronize(queue, graphSyncTimeout);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    verifyOutputs(graph1->inArgs, graph1->outArgs, graph1->outputSize[0]);
    verifyOutputs(graph2->inArgs, graph2->outArgs, graph2->outputSize[0]);

    // create new input buffer for second input for first graph
    float16 *newInput1Graph1 = reinterpret_cast<float16 *>(
        graph1->allocMemory(graph1->inputSize[1], MemType::SHARED_MEMORY));
    std::iota(newInput1Graph1, newInput1Graph1 + graph1->inputSize[1] / sizeof(float16), -2.0f);
    ASSERT_NE(0, memcmp(graph1->inArgs[1], newInput1Graph1, graph1->inputSize[1]));

    // create new input buffer for third input for second graph
    float16 *newInput2Graph2 = reinterpret_cast<float16 *>(
        graph2->allocMemory(graph2->inputSize[2], MemType::SHARED_MEMORY));
    std::iota(newInput2Graph2, newInput2Graph2 + graph2->inputSize[2] / sizeof(float16), -3.0f);
    ASSERT_NE(0, memcmp(graph2->inArgs[2], newInput2Graph2, graph2->inputSize[2]));

    // update second input buffer for first graph
    ze_mutable_graph_argument_exp_desc_t graph1KernelArgDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
        .pNext = nullptr,
        .commandId = graph1CommandId,
        .argIndex = 1,
        .pArgValue = newInput1Graph1,
    };
    // update third input buffer for second graph
    ze_mutable_graph_argument_exp_desc_t graph2KernelArgDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
        .pNext = &graph1KernelArgDesc,
        .commandId = graph2CommandId,
        .argIndex = 2,
        .pArgValue = newInput2Graph2,
    };
    ze_mutable_commands_exp_desc_t mutableCommandsDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMANDS_EXP_DESC,
        .pNext = &graph2KernelArgDesc,
        .flags = 0,
    };
    result = zeCommandListUpdateMutableCommandsExp(commandList, &mutableCommandsDesc);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandListClose(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueExecuteCommandLists(queue, 1, &commandList, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueSynchronize(queue, graphSyncTimeout);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    verifyOutputs({graph1->inArgs[0], newInput1Graph1, graph1->inArgs[2]},
                  graph1->outArgs,
                  graph1->outputSize[0]);
    verifyOutputs({graph2->inArgs[0], graph2->inArgs[1], newInput2Graph2},
                  graph2->outArgs,
                  graph2->outputSize[0]);

    result = zeCommandListDestroy(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);
}

// Test does:
// - create a graph
// - execute command list and check results
// - update first input in the graph
// - execute command list again and check results
// - update second input in the graph (so first and second inputs are updated now)
// - execute command list again and check results
TEST_F(MutableCmdList, UpdateGraphTwice) {
    auto graph = Graph::create(zeContext,
                               zeDevice,
                               zeGraphDDITableExt,
                               modelSetup.modelPath,
                               modelSetup.buildFlags);
    ASSERT_NE(graph, nullptr);

    graph->allocateArguments(MemType::SHARED_MEMORY);

    float16 *input0 = reinterpret_cast<float16 *>(graph->inArgs[0]);
    float16 *input1 = reinterpret_cast<float16 *>(graph->inArgs[1]);
    float16 *input2 = reinterpret_cast<float16 *>(graph->inArgs[2]);

    std::iota(input0, input0 + graph->inputSize[0] / sizeof(float16), 0);
    std::iota(input1, input1 + graph->inputSize[1] / sizeof(float16), 1);
    std::iota(input2, input2 + graph->inputSize[2] / sizeof(float16), 2);

    ze_command_list_handle_t commandList = createMutableCmdList();
    ASSERT_NE(nullptr, commandList);

    // get the id of graph execute command
    ze_mutable_command_id_exp_desc_t mutableCmdIdDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_ID_EXP_DESC,
        .pNext = nullptr,
        .flags = ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS,
    };
    uint64_t commandId;
    ze_result_t result =
        zeCommandListGetNextCommandIdExp(commandList, &mutableCmdIdDesc, &commandId);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeGraphDDITableExt
                 ->pfnAppendGraphExecute(commandList, graph->handle, nullptr, nullptr, 0, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandListClose(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueExecuteCommandLists(queue, 1, &commandList, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueSynchronize(queue, graphSyncTimeout);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    verifyOutputs(graph->inArgs, graph->outArgs, graph->outputSize[0]);

    // create new buffer for first input, update the graph's first argument and run the inference

    float16 *newInput0 = reinterpret_cast<float16 *>(
        graph->allocMemory(graph->inputSize[0], MemType::SHARED_MEMORY));
    std::iota(newInput0, newInput0 + graph->inputSize[0] / sizeof(float16), -2.0f);
    ASSERT_NE(0, memcmp(input0, newInput0, graph->inputSize[0]));

    ze_mutable_graph_argument_exp_desc_t graphArgument0Desc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
        .pNext = nullptr,
        .commandId = commandId,
        .argIndex = 0,
        .pArgValue = newInput0,
    };
    ze_mutable_commands_exp_desc_t mutableCommandsDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMANDS_EXP_DESC,
        .pNext = &graphArgument0Desc,
        .flags = 0,
    };
    result = zeCommandListUpdateMutableCommandsExp(commandList, &mutableCommandsDesc);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandListClose(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueExecuteCommandLists(queue, 1, &commandList, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueSynchronize(queue, graphSyncTimeout);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    verifyOutputs({newInput0, graph->inArgs[1], graph->inArgs[2]},
                  graph->outArgs,
                  graph->outputSize[0]);

    // create new buffer for second input, update the graph's second argument and run the inference

    float16 *newInput1 = reinterpret_cast<float16 *>(
        graph->allocMemory(graph->inputSize[1], MemType::SHARED_MEMORY));
    std::iota(newInput1, newInput1 + graph->inputSize[1] / sizeof(float), -3.0f);
    ASSERT_NE(0, memcmp(input1, newInput1, graph->inputSize[1]));

    ze_mutable_graph_argument_exp_desc_t graphArgument1Desc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
        .pNext = nullptr,
        .commandId = commandId,
        .argIndex = 1,
        .pArgValue = newInput1,
    };
    ze_mutable_commands_exp_desc_t mutableCommandsDesc2{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMANDS_EXP_DESC,
        .pNext = &graphArgument1Desc,
        .flags = 0,
    };
    result = zeCommandListUpdateMutableCommandsExp(commandList, &mutableCommandsDesc2);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandListClose(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueExecuteCommandLists(queue, 1, &commandList, nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandQueueSynchronize(queue, graphSyncTimeout);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    verifyOutputs({newInput0, newInput1, graph->inArgs[2]}, graph->outArgs, graph->outputSize[0]);

    result = zeCommandListDestroy(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);
}

// Negative tests for zeCommandListUpdateMutableCommandsExp

// Call zeCommandListUpdateMutableCommandsExp with invalid commandId
TEST_F(MutableCmdList, UpdateMutableCommandsInvalidCommandId) {
    auto graph = Graph::create(zeContext,
                               zeDevice,
                               zeGraphDDITableExt,
                               modelSetup.modelPath,
                               modelSetup.buildFlags);
    ASSERT_NE(graph, nullptr);

    graph->allocateArguments(MemType::SHARED_MEMORY);

    ze_command_list_handle_t commandList = createMutableCmdList();
    ASSERT_NE(nullptr, commandList);

    ze_result_t result = zeGraphDDITableExt->pfnAppendGraphExecute(commandList,
                                                                   graph->handle,
                                                                   nullptr,
                                                                   nullptr,
                                                                   0,
                                                                   nullptr);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    result = zeCommandListClose(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);

    void *newInput0 = graph->allocMemory(graph->inputSize[0], MemType::SHARED_MEMORY);

    ze_mutable_graph_argument_exp_desc_t graphArgumentDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
        .pNext = nullptr,
        .commandId = 10000,
        .argIndex = 0,
        .pArgValue = newInput0,
    };
    ze_mutable_commands_exp_desc_t mutableCommandsDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMANDS_EXP_DESC,
        .pNext = &graphArgumentDesc,
        .flags = 0,
    };
    result = zeCommandListUpdateMutableCommandsExp(commandList, &mutableCommandsDesc);
    ASSERT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT, result);

    result = zeCommandListDestroy(commandList);
    ASSERT_EQ(ZE_RESULT_SUCCESS, result);
}

// Negative tests for zeCommandListGetNextCommandIdExp

// Call zeCommandListGetNextCommandIdExp on not mutable command list
TEST_F(MutableCmdList, GetNextCommandIdNotMutableCmdList) {
    ze_command_list_desc_t commandListDesc{
        .stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
        .pNext = nullptr,
        .commandQueueGroupOrdinal = 0u,
        .flags = 0,
    };
    ze_command_list_handle_t commandList;
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListCreate(zeContext, zeDevice, &commandListDesc, &commandList));

    ze_mutable_command_id_exp_desc_t mutableCmdIdDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_ID_EXP_DESC,
        .pNext = nullptr,
        .flags = ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS,
    };
    uint64_t commandId;
    ASSERT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT,
              zeCommandListGetNextCommandIdExp(commandList, &mutableCmdIdDesc, &commandId));

    ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandListDestroy(commandList));
}

// Call zeCommandListGetNextCommandIdExp on closed command list
TEST_F(MutableCmdList, GetNextCommandIdClosedCmdList) {
    ze_mutable_command_list_exp_desc_t mutableCmdListDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_LIST_EXP_DESC,
        .pNext = nullptr,
        .flags = 0,
    };
    ze_command_list_desc_t commandListDesc{
        .stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
        .pNext = &mutableCmdListDesc,
        .commandQueueGroupOrdinal = 0u,
        .flags = 0,
    };
    ze_command_list_handle_t commandList;

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeCommandListCreate(zeContext, zeDevice, &commandListDesc, &commandList));
    ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandListClose(commandList));

    ze_mutable_command_id_exp_desc_t mutableCmdIdDesc{
        .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_ID_EXP_DESC,
        .pNext = nullptr,
        .flags = ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS,
    };
    uint64_t commandId;
    ASSERT_EQ(ZE_RESULT_ERROR_INVALID_ARGUMENT,
              zeCommandListGetNextCommandIdExp(commandList, &mutableCmdIdDesc, &commandId));

    ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandListDestroy(commandList));
}

// The test does:
// - create a graph
// - create multiple mutable command lists
// - execute each command list and check results
// - deallocate all previously used graph arguments
// - allocate new arguments
// - mutate command list with new arguments
// - execute command list again and check results
TEST_F(MutableCmdList, MutateGraphExecuteInMultipleCommandList) {
    auto graph = Graph::create(zeContext,
                               zeDevice,
                               zeGraphDDITableExt,
                               modelSetup.modelPath,
                               modelSetup.buildFlags);
    ASSERT_NE(graph, nullptr);

    const size_t cmdListCount = 4;

    ze_result_t result;
    std::vector<uint64_t> commandIds(cmdListCount, 0);
    std::vector<ze_command_list_handle_t> cmdLists;
    for (size_t i = 0; i < cmdListCount; i++) {
        cmdLists.push_back(createMutableCmdList());
    }

    for (size_t i = 0; i < cmdListCount; i++) {
        auto &cmdList = cmdLists[i];

        graph->allocateArguments(MemType::SHARED_MEMORY);

        float16 *input0 = reinterpret_cast<float16 *>(graph->inArgs[0]);
        float16 *input1 = reinterpret_cast<float16 *>(graph->inArgs[1]);
        float16 *input2 = reinterpret_cast<float16 *>(graph->inArgs[2]);

        std::iota(input0, input0 + graph->inputSize[0] / sizeof(float16), i);
        std::iota(input1, input1 + graph->inputSize[1] / sizeof(float16), 1 + i);
        std::iota(input2, input2 + graph->inputSize[2] / sizeof(float16), 2 + i);

        ze_mutable_command_id_exp_desc_t mutableCmdIdDesc{
            .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMAND_ID_EXP_DESC,
            .pNext = nullptr,
            .flags = ZE_MUTABLE_COMMAND_EXP_FLAG_GRAPH_ARGUMENTS,
        };

        result = zeCommandListGetNextCommandIdExp(cmdList, &mutableCmdIdDesc, &commandIds[i]);
        ASSERT_EQ(ZE_RESULT_SUCCESS, result);

        result = zeGraphDDITableExt
                     ->pfnAppendGraphExecute(cmdList, graph->handle, nullptr, nullptr, 0, nullptr);
        ASSERT_EQ(ZE_RESULT_SUCCESS, result);

        result = zeCommandListClose(cmdList);
        ASSERT_EQ(ZE_RESULT_SUCCESS, result);

        result = zeCommandQueueExecuteCommandLists(queue, 1, &cmdList, nullptr);
        ASSERT_EQ(ZE_RESULT_SUCCESS, result);

        result = zeCommandQueueSynchronize(queue, graphSyncTimeout);
        ASSERT_EQ(ZE_RESULT_SUCCESS, result);

        verifyOutputs(graph->inArgs, graph->outArgs, graph->outputSize[0]);
    }

    graph->deallocateAllArguments();

    // update graph's arguments
    for (size_t i = 0; i < cmdListCount; i++) {
        auto &cmdList = cmdLists[i];
        auto &commandId = commandIds[i];

        graph->allocateArguments(MemType::SHARED_MEMORY);

        float16 *input0 = reinterpret_cast<float16 *>(graph->inArgs[0]);
        float16 *input1 = reinterpret_cast<float16 *>(graph->inArgs[1]);
        float16 *input2 = reinterpret_cast<float16 *>(graph->inArgs[2]);

        std::iota(input0, input0 + graph->inputSize[0] / sizeof(float16), cmdListCount + i);
        std::iota(input1, input1 + graph->inputSize[1] / sizeof(float16), cmdListCount + 1 + i);
        std::iota(input2, input2 + graph->inputSize[2] / sizeof(float16), cmdListCount + 2 + i);

        ze_mutable_graph_argument_exp_desc_t input0Desc{
            .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
            .pNext = nullptr,
            .commandId = commandId,
            .argIndex = 0,
            .pArgValue = graph->inArgs[0],
        };
        ze_mutable_graph_argument_exp_desc_t input1Desc{
            .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
            .pNext = &input0Desc,
            .commandId = commandId,
            .argIndex = 1,
            .pArgValue = graph->inArgs[1],
        };
        ze_mutable_graph_argument_exp_desc_t input2Desc{
            .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
            .pNext = &input1Desc,
            .commandId = commandId,
            .argIndex = 2,
            .pArgValue = graph->inArgs[2],
        };
        ze_mutable_graph_argument_exp_desc_t output0Desc{
            .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
            .pNext = &input2Desc,
            .commandId = commandId,
            .argIndex = 3,
            .pArgValue = graph->outArgs[0],
        };
        ze_mutable_graph_argument_exp_desc_t output1Desc{
            .stype = ZE_STRUCTURE_TYPE_MUTABLE_GRAPH_ARGUMENT_EXP_DESC,
            .pNext = &output0Desc,
            .commandId = commandId,
            .argIndex = 4,
            .pArgValue = graph->outArgs[1],
        };
        ze_mutable_commands_exp_desc_t mutableCommandsDesc{
            .stype = ZE_STRUCTURE_TYPE_MUTABLE_COMMANDS_EXP_DESC,
            .pNext = &output1Desc,
            .flags = 0,
        };
        result = zeCommandListUpdateMutableCommandsExp(cmdList, &mutableCommandsDesc);
        ASSERT_EQ(ZE_RESULT_SUCCESS, result);

        result = zeCommandListClose(cmdList);
        ASSERT_EQ(ZE_RESULT_SUCCESS, result);

        result = zeCommandQueueExecuteCommandLists(queue, 1, &cmdList, nullptr);
        ASSERT_EQ(ZE_RESULT_SUCCESS, result) << "Failed to execute using cmdlist " << i;

        result = zeCommandQueueSynchronize(queue, graphSyncTimeout);
        ASSERT_EQ(ZE_RESULT_SUCCESS, result) << "Failed to synchronize using cmdlist " << i;

        verifyOutputs(graph->inArgs, graph->outArgs, graph->outputSize[0]);
    }

    for (size_t i = 0; i < cmdListCount; i++) {
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeCommandListDestroy(cmdLists[i]));
    }
}
