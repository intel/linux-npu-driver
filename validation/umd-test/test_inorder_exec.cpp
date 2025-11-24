/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

#include <future>

class InOrderExecution : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();

        if (!Environment::getInstance()->isDriverExtensionSupported(
                COMMAND_QUEUE_EXT_NAME,
                ZE_COMMAND_QUEUE_NPU_EXT_VERSION_1_1)) {
            GTEST_SKIP() << "Command queue extension doesn't support in order execution";
        }

        cmdQueueDesc.pNext = &inOrderQueueDescriptorExt;
        scopedInOrderQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        inOrderQueue = scopedInOrderQueue.get();
        cmdQueueDesc.pNext = nullptr;
    }

    void TearDown() override { UmdTest::TearDown(); }
    ze_command_queue_desc_npu_ext_2_t inOrderQueueDescriptorExt = {
        ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC_NPU_EXT_2,
        nullptr,
        ZE_NPU_COMMAND_QUEUE_OPTION_DEVICE_SYNC};

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

    std::vector<zeScope::SharedPtr<ze_command_list_handle_t>> scopedCmdLists;
    std::vector<ze_command_list_handle_t> cmdListHandlers;
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedInOrderQueue;
    ze_command_queue_handle_t inOrderQueue = nullptr;
    ze_result_t ret;
};

/* Test submits the same command buffer many times without append any synchronization,
 * command queue created in InOrder mode should provide synchronization.
 */
TEST_F(InOrderExecution, SubmitTheSameBuffer) {
    const size_t iterations = 1000;

    auto mem = AllocSharedMemory(sizeof(uint64_t) * 2);

    uint64_t *mem1 = static_cast<uint64_t *>(mem.get());
    uint64_t *mem2 = mem1++;
    auto scopedCmdList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListAppendMemoryCopy(scopedCmdList.get(),
                                            mem1,
                                            mem2,
                                            sizeof(uint64_t),
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(scopedCmdList.get()), ZE_RESULT_SUCCESS);

    for (size_t i = 0; i < iterations; i++) {
        cmdListHandlers.push_back(scopedCmdList.get());
    }

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(inOrderQueue,
                                                cmdListHandlers.size(),
                                                cmdListHandlers.data(),
                                                nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueSynchronize(inOrderQueue, graphSyncTimeout), ZE_RESULT_SUCCESS);
}

/* Test uses InOrder mode to copy pattern between two memory locations, series of copy operations
 * move pattern from beginning of first buffer to the end of second buffer, each copy operation
 * moves pattern one position later.
 */
TEST_F(InOrderExecution, RunCopyUsingCmdQueueInOrderMode) {
    const size_t iterations = 2048;
    uint32_t pattern = 0x12345678;
    const size_t allocSize = (iterations + 1) * sizeof(pattern);

    auto mem1 = AllocSharedMemory(allocSize);
    auto mem2 = AllocSharedMemory(allocSize);

    ASSERT_NE(mem1.get(), nullptr);
    ASSERT_NE(mem2.get(), nullptr);
    memset(mem1.get(), 0, allocSize);
    memset(mem2.get(), 0, allocSize);
    memcpy(mem1.get(), &pattern, sizeof(pattern));

    decltype(pattern) *cpyPtr1 = static_cast<decltype(pattern) *>(mem1.get());
    decltype(pattern) *cpyPtr2 = static_cast<decltype(pattern) *>(mem2.get());

    for (size_t i = 0; i < (iterations * 2) - 1; i++) {
        auto scopedCmdList1 = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        /*Copy pattern to second buffer one location later*/
        ASSERT_EQ(zeCommandListAppendMemoryCopy(scopedCmdList1.get(),
                                                cpyPtr2,
                                                cpyPtr1,
                                                sizeof(pattern),
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(scopedCmdList1.get()), ZE_RESULT_SUCCESS);
        scopedCmdLists.push_back(scopedCmdList1);
        cmdListHandlers.push_back(scopedCmdList1.get());

        auto scopedCmdList2 = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

        /* Zero pattern in original location, copy value(should be 0) from pos + 1 */
        ASSERT_EQ(zeCommandListAppendMemoryCopy(scopedCmdList2.get(),
                                                cpyPtr1,
                                                (cpyPtr1 + 1),
                                                sizeof(pattern),
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(scopedCmdList2.get()), ZE_RESULT_SUCCESS);
        scopedCmdLists.push_back(scopedCmdList2);
        cmdListHandlers.push_back(scopedCmdList2.get());

        std::swap(cpyPtr1, cpyPtr2);
        cpyPtr2++;
    }
    /*  It is expected that when execution was in order the pattern
     *  will be present at the end of second buffer, *testPtr == pattern
     */
    auto testPtr = static_cast<decltype(pattern) *>(mem2.get()) + (iterations - 1);
    ASSERT_EQ(*testPtr, 0);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(inOrderQueue,
                                                cmdListHandlers.size(),
                                                cmdListHandlers.data(),
                                                nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueSynchronize(inOrderQueue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    std::vector<char> zeroBuf(allocSize, 0);

    /* First buffer should be empty */
    ASSERT_EQ(memcmp(mem1.get(), zeroBuf.data(), allocSize), 0);
    /* At the end of second buffer is expected test pattern*/
    ASSERT_EQ(*testPtr, pattern);
}

/* Test uses two queues first in InOrder mode second created in Default mode. The test pattern
 * is copied between two memory locations by series of copy operations, executed separately on these
 * two command queues. Additional synchronization is appended to keep proper order of execution
 * It is expected that when execution was in order the pattern will be present at the end of first
 * buffer.
 */
TEST_F(InOrderExecution, RunCopyUsingInOrderModeAppendSynchronization) {
    const size_t iterations = 200;
    uint32_t pattern = 0x12345678;
    const size_t allocSize = (iterations + 1) * sizeof(pattern);

    const ze_event_pool_desc_t eventPoolDesc = {ZE_STRUCTURE_TYPE_EVENT_POOL_DESC,
                                                nullptr,
                                                ZE_EVENT_POOL_FLAG_HOST_VISIBLE,
                                                iterations * 2};

    ze_event_desc_t eventDesc = {ZE_STRUCTURE_TYPE_EVENT_DESC,
                                 nullptr,
                                 0,
                                 ZE_EVENT_SCOPE_FLAG_HOST,
                                 ZE_EVENT_SCOPE_FLAG_HOST};

    auto scopedEventPool = zeScope::eventPoolCreate(zeContext, eventPoolDesc, 1, zeDevice, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    std::vector<zeScope::SharedPtr<ze_event_handle_t>> events;

    auto scopedDefaultQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    auto mem1 = AllocSharedMemory(allocSize);
    auto mem2 = AllocSharedMemory(allocSize);

    ASSERT_NE(mem1.get(), nullptr);
    ASSERT_NE(mem2.get(), nullptr);
    memset(mem1.get(), 0, allocSize);
    memset(mem2.get(), 0, allocSize);

    /* Set pattern on first position of mem1 buffer */
    memcpy(mem1.get(), &pattern, sizeof(pattern));

    decltype(pattern) *cpyPtr1 = static_cast<decltype(pattern) *>(mem1.get());
    decltype(pattern) *cpyPtr2 = static_cast<decltype(pattern) *>(mem2.get());
    std::vector<ze_command_list_handle_t> cmdListHandlersForInOrder;
    std::vector<ze_command_list_handle_t> cmdListHandlersForDefault;
    ze_event_handle_t defaultQueueEventHandle = nullptr;
    ze_event_handle_t inOrderQueueEventHandle = nullptr;

    for (size_t i = 0; i < iterations; i++) {
        auto inOrderQueueCmdList =
            zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

        eventDesc.index = i * 2;
        auto scopedInOrderSignalling = zeScope::eventCreate(scopedEventPool.get(), eventDesc, ret);
        EXPECT_EQ(ret, ZE_RESULT_SUCCESS);
        events.push_back(scopedInOrderSignalling);
        inOrderQueueEventHandle = scopedInOrderSignalling.get();

        /* Copy operations pattern to second buffer*/
        if (defaultQueueEventHandle) {
            ASSERT_EQ(zeCommandListAppendWaitOnEvents(inOrderQueueCmdList.get(),
                                                      1,
                                                      &defaultQueueEventHandle),
                      ZE_RESULT_SUCCESS);
        }

        ASSERT_EQ(zeCommandListAppendMemoryCopy(inOrderQueueCmdList.get(),
                                                cpyPtr2,
                                                cpyPtr1,
                                                sizeof(pattern),
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);
        /* Zero pattern in original location, copy value(should be 0) from pos + 1 */
        ASSERT_EQ(zeCommandListAppendMemoryCopy(inOrderQueueCmdList.get(),
                                                cpyPtr1,
                                                (cpyPtr1 + 1),
                                                sizeof(pattern),
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);
        /* Signal event for default queue */
        ASSERT_EQ(
            zeCommandListAppendSignalEvent(inOrderQueueCmdList.get(), inOrderQueueEventHandle),
            ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeCommandListClose(inOrderQueueCmdList.get()), ZE_RESULT_SUCCESS);
        scopedCmdLists.push_back(inOrderQueueCmdList);
        cmdListHandlersForInOrder.push_back(inOrderQueueCmdList.get());

        cpyPtr1++;

        /* Copy operations for DefaultQueue, copy back pattern to first buffer one location later*/
        eventDesc.index = i * 2 + 1;
        auto scopedDefaultSignalling = zeScope::eventCreate(scopedEventPool.get(), eventDesc, ret);
        EXPECT_EQ(ret, ZE_RESULT_SUCCESS);
        events.push_back(scopedDefaultSignalling);
        defaultQueueEventHandle = scopedDefaultSignalling.get();
        auto defaultQueueCmdList =
            zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        ASSERT_EQ(
            zeCommandListAppendWaitOnEvents(defaultQueueCmdList.get(), 1, &inOrderQueueEventHandle),
            ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeCommandListAppendMemoryCopy(defaultQueueCmdList.get(),
                                                cpyPtr1,
                                                cpyPtr2,
                                                sizeof(pattern),
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);
        /* Zero pattern in original location, copy value(should be 0) from pos + 1 */
        ASSERT_EQ(zeCommandListAppendMemoryCopy(defaultQueueCmdList.get(),
                                                cpyPtr2,
                                                (cpyPtr2 + 1),
                                                sizeof(pattern),
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);

        ASSERT_EQ(
            zeCommandListAppendSignalEvent(defaultQueueCmdList.get(), defaultQueueEventHandle),
            ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeCommandListClose(defaultQueueCmdList.get()), ZE_RESULT_SUCCESS);
        scopedCmdLists.push_back(defaultQueueCmdList);
        cmdListHandlersForDefault.push_back(defaultQueueCmdList.get());

        cpyPtr2++;
    }
    /*  It is expected that when execution was in order the pattern
     *  will be present at the end of first buffer
     */
    auto testPtr = static_cast<decltype(pattern) *>(mem1.get()) + iterations;
    /* Verify that there is 0 at expected pattern location before execution*/
    ASSERT_EQ(*testPtr, 0);

    /* Verify that all events are in ZE_RESULT_NOT_READY state */
    for (auto &event : events) {
        ASSERT_EQ(zeEventQueryStatus(event.get()), ZE_RESULT_NOT_READY);
    }

    /* Execute asynchronously, resubmits are expected synchronous execution could block */
    std::future<ze_result_t> inOrderQueueExec = std::async(std::launch::async, [&] {
        return zeCommandQueueExecuteCommandLists(scopedInOrderQueue.get(),
                                                 cmdListHandlersForInOrder.size(),
                                                 cmdListHandlersForInOrder.data(),
                                                 nullptr);
    });
    std::future<ze_result_t> defaultQueueExec = std::async(std::launch::async, [&] {
        return zeCommandQueueExecuteCommandLists(scopedDefaultQueue.get(),
                                                 cmdListHandlersForDefault.size(),
                                                 cmdListHandlersForDefault.data(),
                                                 nullptr);
    });

    ASSERT_EQ(inOrderQueueExec.get(), ZE_RESULT_SUCCESS);

    ASSERT_EQ(defaultQueueExec.get(), ZE_RESULT_SUCCESS);

    /* Wait only for default, synchronization should guarantee all copy should be finished */
    ASSERT_EQ(zeCommandQueueSynchronize(scopedDefaultQueue.get(), graphSyncTimeout),
              ZE_RESULT_SUCCESS);

    /* Verify that all events were triggered */
    for (auto &event : events) {
        ASSERT_EQ(zeEventQueryStatus(event.get()), ZE_RESULT_SUCCESS);
    }

    std::vector<char> zeroBuf(allocSize, 0);

    /* Second buffer should be empty */
    ASSERT_EQ(memcmp(mem2.get(), zeroBuf.data(), allocSize), 0);
    /* At the end of first buffer is expected test pattern*/
    ASSERT_EQ(*testPtr, pattern);
}

TEST_F(InOrderExecution, ResendInOrderCmdLists) {
    const size_t iterations = 2048;
    uint8_t pattern = 0x12;
    const size_t allocSize = iterations * sizeof(uint32_t);

    auto mem1 = AllocSharedMemory(allocSize);
    auto mem2 = AllocSharedMemory(allocSize);

    ASSERT_NE(mem1.get(), nullptr);
    ASSERT_NE(mem2.get(), nullptr);
    memset(mem1.get(), pattern, allocSize);
    memset(mem2.get(), 0, allocSize);

    /* Create series of 4 bytes copy commands to copy all data from mem1 to mem2 */
    uint8_t *cpyPtr1 = static_cast<uint8_t *>(mem1.get());
    uint8_t *cpyPtr2 = static_cast<uint8_t *>(mem2.get());
    for (size_t i = 0; i < iterations; i++) {
        auto scopedCmdList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListAppendMemoryCopy(scopedCmdList.get(),
                                                cpyPtr2,
                                                cpyPtr1,
                                                sizeof(uint32_t),
                                                nullptr,
                                                0,
                                                nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(scopedCmdList.get()), ZE_RESULT_SUCCESS);
        scopedCmdLists.push_back(scopedCmdList);
        cmdListHandlers.push_back(scopedCmdList.get());

        cpyPtr1 += sizeof(uint32_t);
        cpyPtr2 += sizeof(uint32_t);
    }
    /* First subcase - series of copy commands executed on inorder queue */
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(inOrderQueue,
                                                cmdListHandlers.size(),
                                                cmdListHandlers.data(),
                                                nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueSynchronize(inOrderQueue, graphSyncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(memcmp(mem1.get(), mem2.get(), allocSize), 0);

    /* Second subcase -  series of copy commands resubmitted on inorder queue but in reverse order
     */
    memset(mem2.get(), 0, allocSize);
    std::reverse(cmdListHandlers.begin(), cmdListHandlers.end());

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(inOrderQueue,
                                                cmdListHandlers.size(),
                                                cmdListHandlers.data(),
                                                nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(inOrderQueue, graphSyncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(memcmp(mem1.get(), mem2.get(), allocSize), 0);

    /* Third subcase -  series of copy commands resubmitted in reverse order but on normal queue
     * without keeping order */
    memset(mem2.get(), 0, allocSize);
    auto scopedDefaultQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(scopedDefaultQueue.get(),
                                                cmdListHandlers.size(),
                                                cmdListHandlers.data(),
                                                nullptr),
              ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueSynchronize(scopedDefaultQueue.get(), graphSyncTimeout),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(memcmp(mem1.get(), mem2.get(), allocSize), 0);
}

class InOrderInference : public InOrderExecution, public ::testing::WithParamInterface<YAML::Node> {
  public:
    void SetUp() override {
        InOrderExecution::SetUp();
        const YAML::Node node = GetParam();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);
        ASSERT_NE(graph, nullptr);

        graph->allocateArguments(MemType::SHARED_MEMORY);
        graph->copyInputData();
    }

    ze_command_list_handle_t list = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
    std::shared_ptr<Graph> graph;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(InOrderInference);

INSTANTIATE_TEST_SUITE_P(,
                         InOrderInference,
                         ::testing::ValuesIn(Environment::getConfiguration("graph_execution")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(InOrderInference, RunGraphInOrderMode) {
    auto mem = AllocSharedMemory(sizeof(uint64_t));
    auto *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_NE(ts, nullptr);
    *ts = 0ULL;
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    auto secondCmdlist = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphExecute(secondCmdlist.get(),
                                                        graph->handle,
                                                        nullptr,
                                                        nullptr,
                                                        0,
                                                        nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(secondCmdlist.get()), ZE_RESULT_SUCCESS);

    std::vector<ze_command_list_handle_t> cmdListHandlers = {list, secondCmdlist.get()};
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(inOrderQueue,
                                                cmdListHandlers.size(),
                                                cmdListHandlers.data(),
                                                nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(inOrderQueue, graphSyncTimeout), ZE_RESULT_SUCCESS);
    graph->checkResults();
    ASSERT_GT(*ts, 0ULL);
}

TEST_P(InOrderInference, RunGraphInOrderModeWithWorkloadType) {
    auto mem = AllocSharedMemory(sizeof(uint64_t));
    auto *ts = static_cast<uint64_t *>(mem.get());
    ASSERT_NE(ts, nullptr);
    *ts = 0ULL;
    ASSERT_EQ(zeCommandListAppendWriteGlobalTimestamp(list, ts, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    auto secondCmdlist = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt->pfnAppendGraphExecute(secondCmdlist.get(),
                                                        graph->handle,
                                                        nullptr,
                                                        nullptr,
                                                        0,
                                                        nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(secondCmdlist.get()), ZE_RESULT_SUCCESS);

    ret = zeCommandQueueDDITableExt->pfnSetWorkloadType(inOrderQueue, ZE_WORKLOAD_TYPE_BACKGROUND);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    std::vector<ze_command_list_handle_t> cmdListHandlers = {list, secondCmdlist.get()};
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(inOrderQueue,
                                                cmdListHandlers.size(),
                                                cmdListHandlers.data(),
                                                nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(inOrderQueue, graphSyncTimeout), ZE_RESULT_SUCCESS);
    graph->checkResults();
    graph->clearOutput();

    ret = zeCommandQueueDDITableExt->pfnSetWorkloadType(inOrderQueue, ZE_WORKLOAD_TYPE_DEFAULT);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(inOrderQueue,
                                                cmdListHandlers.size(),
                                                cmdListHandlers.data(),
                                                nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(inOrderQueue, graphSyncTimeout), ZE_RESULT_SUCCESS);
    graph->checkResults();
    ASSERT_GT(*ts, 0ULL);
}
