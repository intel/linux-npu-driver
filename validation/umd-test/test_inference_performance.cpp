/*
 * Copyright (C) 2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"
#include "blob_params.hpp"

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

        /*Get base configuration from config file*/
        YAML::Node &configuration = Environment::getConfiguration();
        if (configuration["blob_dir"].IsDefined())
            blobDir = configuration["blob_dir"].as<std::string>();

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
    std::string blobDir = "";

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
    std::vector<std::vector<char>> inputBin, outputBin;
    std::vector<char> vpuBlob, vpuBin;

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

    std::chrono::steady_clock::time_point start;
    size_t execIter = 3;

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

    ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphInitialize(list, graphHandle, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(
        zeGraphDDITableExt->pfnAppendGraphExecute(list, graphHandle, nullptr, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    for (size_t i = 0; i < execIter; i++) {
        start = std::chrono::steady_clock::now();
        ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);
        inferenceDuration.push_back(sectionDuration(start));

        for (size_t i = 0; i < graphOutput.size(); i++)
            EXPECT_EQ(memcmp(graphOutput[i], outputBin[i].data(), outputBin[i].size()), 0);

        PRINTF("Inference #%zu took: %f ms\n", i, inferenceDuration[i].count());
    }

    InferenceDuration inferenceTime =
        std::reduce(inferenceDuration.begin(), inferenceDuration.end());

    PRINTF("\nThe duration of the VPU wakeup to be able to run the inference: %f ms\n",
           (inferenceDuration[0].count() - inferenceDuration[1].count()));

    TRACE("\nThroughput: %f FPS\n\n", static_cast<float>(execIter) * 1000 / inferenceTime.count());
}
