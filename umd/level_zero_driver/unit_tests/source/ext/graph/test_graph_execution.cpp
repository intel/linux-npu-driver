/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <cstddef>
#include <cstdint>

#include "gtest/gtest.h"
#include "level_zero_driver/source/cmdlist.hpp"
#include "level_zero_driver/source/cmdqueue.hpp"
#include "level_zero_driver/source/context.hpp"
#include "level_zero_driver/source/device.hpp"
#include "level_zero_driver/source/ext/graph.hpp"
#include "level_zero_driver/unit_tests/fixtures/graph_fixture.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <limits>
#include <memory>
#include <vector>
#include <ze_api.h>
#include <ze_graph_ext.h>

namespace L0 {
namespace ult {

class GraphExecutionFixture : public GraphNGraphLiteFixture {
  public:
    void SetUp() override {
        GraphNGraphLiteFixture::SetUp();
        if (::testing::Test::IsSkipped())
            return;

        auto res = L0::Graph::create(context, device, &graphDesc, &hGraph);
        ASSERT_EQ(ZE_RESULT_SUCCESS, res);
        ASSERT_NE(nullptr, hGraph);
        graph = L0::Graph::fromHandle(hGraph);

        ASSERT_TRUE(setGraphArguments());

        ze_command_list_handle_t hCmdList = createCommandList();
        ASSERT_NE(nullptr, hCmdList);
        cmdList = L0::CommandList::fromHandle(hCmdList);

        ze_command_queue_handle_t hCmdQueue = createCommandQueue();
        ASSERT_NE(nullptr, hCmdQueue);
        cmdQueue = L0::CommandQueue::fromHandle(hCmdQueue);
    }

    void TearDown() override {
        buffers.clear();
        if (cmdList != nullptr) {
            cmdList->destroy();
        }
        if (cmdQueue != nullptr) {
            cmdQueue->destroy();
        }
        GraphNGraphLiteFixture::TearDown();
    }

    bool setGraphArguments() {
        if (graph == nullptr) {
            return false;
        }

        ze_graph_properties_3_t graphProps = {};
        graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES_3;
        EXPECT_EQ(graph->getProperties3(&graphProps), ZE_RESULT_SUCCESS);
        RETURN_ON_TEST_FAILURE(false);

        for (auto i = 0u; i < graphProps.numGraphArgs; i++) {
            ze_graph_argument_properties_3_t argProps = {};
            argProps.stype = ZE_STRUCTURE_TYPE_GRAPH_ARGUMENT_PROPERTIES_3;
            EXPECT_EQ(graph->getArgumentProperties3(i, &argProps), ZE_RESULT_SUCCESS);
            BREAK_ON_TEST_FAILURE();

            uint64_t size = 1u;
            for (auto dim = 0u; dim < argProps.dims_count; dim++) {
                size *= argProps.dims[dim];
            }
            size *= graphPrecisionToByteSize(argProps.devicePrecision);

            void *buffer = nullptr;
            EXPECT_EQ(context->allocMemory(size,
                                           1u,
                                           &buffer,
                                           VPU::VPUBufferObject::Location::Host,
                                           VPU::VPUBufferObject::Type::CachedDma),
                      ZE_RESULT_SUCCESS);
            BREAK_ON_TEST_FAILURE();

            EXPECT_EQ(graph->setArgumentValue(i, buffer), ZE_RESULT_SUCCESS);
            BREAK_ON_TEST_FAILURE();
            buffers.push_back(
                std::shared_ptr<void>(buffer, [this](void *ptr) { context->freeMem(ptr); }));
        }

        RETURN_ON_TEST_FAILURE(false);
        return true;
    }

    size_t graphPrecisionToByteSize(ze_graph_argument_precision_t arg) {
        switch (arg) {
        case ZE_GRAPH_ARGUMENT_PRECISION_FP64:
        case ZE_GRAPH_ARGUMENT_PRECISION_INT64:
        case ZE_GRAPH_ARGUMENT_PRECISION_UINT64:
            return sizeof(uint64_t);

        case ZE_GRAPH_ARGUMENT_PRECISION_FP32:
        case ZE_GRAPH_ARGUMENT_PRECISION_INT32:
        case ZE_GRAPH_ARGUMENT_PRECISION_UINT32:
            return sizeof(uint32_t);

        case ZE_GRAPH_ARGUMENT_PRECISION_BF16:
        case ZE_GRAPH_ARGUMENT_PRECISION_FP16:
        case ZE_GRAPH_ARGUMENT_PRECISION_INT16:
        case ZE_GRAPH_ARGUMENT_PRECISION_UINT16:
            return sizeof(uint16_t);

        default:
            return sizeof(uint8_t);
        }
    }

    L0::Graph *graph = nullptr;
    L0::CommandQueue *cmdQueue = nullptr;
    L0::CommandList *cmdList = nullptr;
    std::vector<std::shared_ptr<void>> buffers;
};

using GraphExecution = Test<GraphExecutionFixture>;

TEST_F(GraphExecution, submitGraphInitAndGraphExecute) {
    ze_command_list_handle_t hCmdList = cmdList->toHandle();

    ASSERT_EQ(cmdList->appendGraphInitialize(hGraph, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->appendGraphExecute(hGraph, nullptr, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->close(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->executeCommandLists(1, &hCmdList, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->synchronize(std::numeric_limits<uint64_t>::max()), ZE_RESULT_SUCCESS);

    ASSERT_EQ(cmdList->reset(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->appendGraphInitialize(hGraph, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->appendGraphExecute(hGraph, nullptr, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->close(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->executeCommandLists(1, &hCmdList, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->synchronize(std::numeric_limits<uint64_t>::max()), ZE_RESULT_SUCCESS);
}

TEST_F(GraphExecution, submitGraphExecute) {
    ze_command_list_handle_t hCmdList = cmdList->toHandle();

    ASSERT_EQ(cmdList->appendGraphExecute(hGraph, nullptr, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->close(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->executeCommandLists(1, &hCmdList, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->synchronize(std::numeric_limits<uint64_t>::max()), ZE_RESULT_SUCCESS);

    ASSERT_EQ(cmdList->reset(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->appendGraphExecute(hGraph, nullptr, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->close(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->executeCommandLists(1, &hCmdList, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->synchronize(std::numeric_limits<uint64_t>::max()), ZE_RESULT_SUCCESS);
}

TEST_F(GraphExecution, graphInitThensubmitGraphExecute) {
    ze_command_list_handle_t hCmdList = cmdList->toHandle();

    ASSERT_EQ(graph->parserInitialize(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->appendGraphExecute(hGraph, nullptr, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->close(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->executeCommandLists(1, &hCmdList, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->synchronize(std::numeric_limits<uint64_t>::max()), ZE_RESULT_SUCCESS);

    ASSERT_EQ(cmdList->reset(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(graph->parserInitialize(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->appendGraphExecute(hGraph, nullptr, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->close(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->executeCommandLists(1, &hCmdList, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->synchronize(std::numeric_limits<uint64_t>::max()), ZE_RESULT_SUCCESS);
}

TEST_F(GraphExecution, multipleGraphExecuteInCmdList) {
    ze_command_list_handle_t hCmdList = cmdList->toHandle();

    ASSERT_EQ(cmdList->appendGraphExecute(hGraph, nullptr, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_TRUE(setGraphArguments());
    ASSERT_EQ(cmdList->appendGraphExecute(hGraph, nullptr, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_TRUE(setGraphArguments());
    ASSERT_EQ(cmdList->appendGraphExecute(hGraph, nullptr, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->close(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->executeCommandLists(1, &hCmdList, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->synchronize(std::numeric_limits<uint64_t>::max()), ZE_RESULT_SUCCESS);

    ASSERT_EQ(cmdList->reset(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->appendGraphExecute(hGraph, nullptr, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_TRUE(setGraphArguments());
    ASSERT_EQ(cmdList->appendGraphExecute(hGraph, nullptr, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdList->close(), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->executeCommandLists(1, &hCmdList, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(cmdQueue->synchronize(std::numeric_limits<uint64_t>::max()), ZE_RESULT_SUCCESS);
}

} // namespace ult
} // namespace L0
