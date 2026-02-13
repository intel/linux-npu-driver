/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "drm/ivpu_accel.h"
#include "graph_utilities.hpp"
#include "ze_stringify.hpp"
#include "zex_driver.hpp"

#include <chrono>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ze_api.h>
#include <ze_context_npu_ext.h>

using namespace std::literals;

class IdleOptimizations : public UmdTest,
                          public ::testing::WithParamInterface<std::tuple<YAML::Node, bool>> {
  protected:
    void SetUp() override {
        UmdTest::SetUp();

        if (isVPU37xx()) {
            SKIP_("IdleOptimizations test is not supported on NPU 37xx");
        }

        ASSERT_EQ(zeDriverGetExtensionFunctionAddress(
                      zeDriver,
                      "zexContextSetIdlePruningTimeout",
                      reinterpret_cast<void **>(&contextSetIdlePruningTimeout)),
                  ZE_RESULT_SUCCESS);
    }

    std::shared_ptr<Graph> createGraph(const YAML::Node &node) {
        auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);
        graph->allocateArguments(MemType::SHARED_MEMORY);
        graph->copyInputData();
        return graph;
    }

    std::unique_ptr<InferenceRequest> createInferRequest(const std::shared_ptr<Graph> &graph,
                                                         bool useFence = false) {
        auto inference = std::make_unique<InferenceRequest>(*graph.get());
        inference->setUpCommandQueue(zeContext, zeDevice);
        inference->setUpCommandList(zeContext, zeDevice, graph->handle, zeGraphDDITableExt);
        if (useFence) {
            inference->setUpFence(zeContext, zeDevice);
        }
        return inference;
    }

    void runInference(InferenceRequest &inference) {
        ASSERT_EQ(inference.runAsync(), ZE_RESULT_SUCCESS);
        ASSERT_EQ(inference.wait(graphSyncTimeout), ZE_RESULT_SUCCESS);
        ASSERT_TRUE(inference.validateOutput());
    }

    uint64_t getContextMemory() {
        ze_graph_memory_query_t query = {};
        ze_result_t ret =
            zeGraphDDITableExt->pfnQueryContextMemory(zeContext, ZE_GRAPH_QUERY_MEMORY_DDR, &query);
        if (ret != ZE_RESULT_SUCCESS) {
            throw std::runtime_error(std::string("QueryContextMemory failed with error ") +
                                     ze_result_to_str(ret));
        }
        return query.allocated;
    }

    void checkIfMemoryIsReleased(const std::chrono::milliseconds &duration) {
        uint64_t beforeMem = getContextMemory();
        PRINTF("NPU memory after inference: %zu\n", beforeMem);
        std::this_thread::sleep_for(duration);
        uint64_t afterMem = getContextMemory();
        PRINTF("NPU memory after sleep: %zu\n\n", afterMem);
        ASSERT_GT(beforeMem, afterMem);
    }

    void checkIfMemoryIsNotReleased(const std::chrono::milliseconds &duration) {
        uint64_t beforeMem = getContextMemory();
        PRINTF("NPU memory after inference: %zu\n", beforeMem);
        std::this_thread::sleep_for(duration);
        uint64_t afterMem = getContextMemory();
        PRINTF("NPU memory after sleep: %zu\n\n", afterMem);
        ASSERT_EQ(beforeMem, afterMem);
    }

    decltype(zexContextSetIdlePruningTimeout) *contextSetIdlePruningTimeout;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(IdleOptimizations);

std::vector<std::tuple<YAML::Node, bool>> getTestParams() {
    std::vector<std::tuple<YAML::Node, bool>> ret;
    auto nodes = Environment::getConfiguration("idle_optimizations");
    for (const auto &node : nodes) {
        ret.emplace_back(node, false);
        ret.emplace_back(node, true);
    }
    return ret;
}

INSTANTIATE_TEST_SUITE_P(,
                         IdleOptimizations,
                         testing::ValuesIn(getTestParams()),
                         [](const testing::TestParamInfo<std::tuple<YAML::Node, bool>> &p) {
                             return generateTestNameFromNode(std::get<0>(p.param)) + "_use_fence_" +
                                    std::to_string(std::get<1>(p.param));
                         });

TEST_P(IdleOptimizations, ContextSetProperties) {
    const auto [node, useFence] = GetParam();

    auto graph = createGraph(node);
    ASSERT_NE(graph, nullptr);
    auto inference = createInferRequest(graph, useFence);

    contextSetIdlePruningTimeout(zeContext, 300);

    PRINTF("Run the inference, sleep for 500ms and check if memory is freed\n");
    runInference(*inference);
    checkIfMemoryIsReleased(500ms);

    PRINTF("Run the inference, sleep for 100ms and check if memory is NOT freed\n");
    runInference(*inference);
    checkIfMemoryIsNotReleased(100ms);

    PRINTF(
        "Unset idle optimizations, run the inference, sleep for 500ms and check if memory is NOT "
        "freed\n");
    ze_context_properties_npu_ext_t contextProperties = {};
    contextProperties.stype = ZE_STRUCTURE_TYPE_CONTEXT_PROPERTIES_NPU_EXT;
    contextProperties.options = 0;
    ASSERT_EQ(zeContextDDITableExt->pfnSetProperties(zeContext, &contextProperties),
              ZE_RESULT_SUCCESS);
    runInference(*inference);
    checkIfMemoryIsNotReleased(500ms);

    PRINTF("Set idle optimizations, run the inference, sleep for 500ms and check if memory is "
           "freed\n");
    contextProperties.options = ZE_NPU_CONTEXT_OPTION_IDLE_OPTIMIZATIONS;
    ASSERT_EQ(zeContextDDITableExt->pfnSetProperties(zeContext, &contextProperties),
              ZE_RESULT_SUCCESS);
    runInference(*inference);
    checkIfMemoryIsReleased(500ms);
}

TEST_P(IdleOptimizations, ReleaseMemory) {
    const auto [node, useFence] = GetParam();

    auto graph = createGraph(node);
    ASSERT_NE(graph, nullptr);
    auto inference = createInferRequest(graph, useFence);

    ze_context_properties_npu_ext_t contextProperties = {};
    contextProperties.stype = ZE_STRUCTURE_TYPE_CONTEXT_PROPERTIES_NPU_EXT;
    contextProperties.options = 0;
    ASSERT_EQ(zeContextDDITableExt->pfnSetProperties(zeContext, &contextProperties),
              ZE_RESULT_SUCCESS);

    PRINTF("Run the inference, call ReleaseMemory and check if memory is freed\n");
    runInference(*inference);
    uint64_t beforeMem = getContextMemory();
    PRINTF("NPU memory after inference: %zu\n", beforeMem);
    ASSERT_EQ(zeContextDDITableExt->pfnReleaseMemory(zeContext), ZE_RESULT_SUCCESS);
    uint64_t afterMem = getContextMemory();
    PRINTF("NPU memory after release: %zu\n\n", afterMem);
    ASSERT_GT(beforeMem, afterMem);
}

TEST_P(IdleOptimizations, MultiThreadedInferenceAndReleaseMemory) {
    const auto [node, useFence] = GetParam();

    auto graph = createGraph(node);
    ASSERT_NE(graph, nullptr);
    auto inference = createInferRequest(graph, useFence);

    ze_context_properties_npu_ext_t contextProperties = {};
    contextProperties.stype = ZE_STRUCTURE_TYPE_CONTEXT_PROPERTIES_NPU_EXT;
    contextProperties.options = 0;
    ASSERT_EQ(zeContextDDITableExt->pfnSetProperties(zeContext, &contextProperties),
              ZE_RESULT_SUCCESS);

    bool stop = false;
    std::thread inferenceThread([&]() {
        while (!stop) {
            runInference(*inference);
        }
    });
    std::thread releaseThread([&]() {
        while (!stop) {
            ASSERT_EQ(zeContextDDITableExt->pfnReleaseMemory(zeContext), ZE_RESULT_SUCCESS);
        }
    });
    std::this_thread::sleep_for(1s);
    stop = true;
    inferenceThread.join();
    releaseThread.join();
}

TEST_P(IdleOptimizations, MultiThreadedInferenceAndIdleOptimizationsOnOff) {
    const auto [node, useFence] = GetParam();

    auto graph = createGraph(node);
    ASSERT_NE(graph, nullptr);
    auto inference = createInferRequest(graph, useFence);

    bool stop = false;
    std::thread inferenceThread([&]() {
        while (!stop) {
            runInference(*inference);
        }
    });
    std::thread idleOptimizationsOnOffThread([&]() {
        ze_context_properties_npu_ext_t contextProperties = {};
        contextProperties.stype = ZE_STRUCTURE_TYPE_CONTEXT_PROPERTIES_NPU_EXT;
        while (!stop) {
            contextProperties.options = ZE_NPU_CONTEXT_OPTION_IDLE_OPTIMIZATIONS;
            ASSERT_EQ(zeContextDDITableExt->pfnSetProperties(zeContext, &contextProperties),
                      ZE_RESULT_SUCCESS);
            contextProperties.options = 0;
            ASSERT_EQ(zeContextDDITableExt->pfnSetProperties(zeContext, &contextProperties),
                      ZE_RESULT_SUCCESS);
        }
    });
    std::this_thread::sleep_for(1s);
    stop = true;
    inferenceThread.join();
    idleOptimizationsOnOffThread.join();
}
