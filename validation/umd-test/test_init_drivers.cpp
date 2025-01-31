/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "testenv.hpp"
#include "ze_memory.hpp"

#include <gtest/gtest.h>

static void assertNotInitialized() {
    uint32_t numDrivers = 1;
    ze_driver_handle_t driver;
    ASSERT_EQ(zeDriverGet(&numDrivers, &driver), ZE_RESULT_ERROR_UNINITIALIZED)
        << "The driver is already initialized. It means that zeInit/zeInitDrivers was called prior "
           "and/or multiple test cases are executed in a single process. This is not supported - "
           "each case needs to be run separetely.";
}

TEST(zeInitDrivers, FailWithoutInit) {
    if (!test_vars::initialization_tests)
        SKIP_("The test is skipped because --initialization_tests flag is missing");
    assertNotInitialized();
}

static void executeCopyCommand(ze_driver_handle_t driver) {
    uint32_t numDevices = 1;
    ze_device_handle_t device;
    ASSERT_EQ(zeDeviceGet(driver, &numDevices, &device), ZE_RESULT_SUCCESS);

    ze_context_desc_t contextDesc{.stype = ZE_STRUCTURE_TYPE_CONTEXT_DESC,
                                  .pNext = nullptr,
                                  .flags = 0};
    ze_context_handle_t context;
    ASSERT_EQ(zeContextCreate(driver, &contextDesc, &context), ZE_RESULT_SUCCESS);

    ze_command_queue_desc_t commandQueueDesc{
        .stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
        .pNext = nullptr,
        .ordinal = 0,
        .index = 0,
        .flags = ZE_COMMAND_QUEUE_FLAG_EXPLICIT_ONLY,
        .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
        .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL,
    };
    ze_command_queue_handle_t commandQueue;
    ASSERT_EQ(zeCommandQueueCreate(context, device, &commandQueueDesc, &commandQueue),
              ZE_RESULT_SUCCESS);

    ze_command_list_desc_t commandListDesc{
        .stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
        .pNext = nullptr,
        .commandQueueGroupOrdinal = 0,
        .flags = ZE_COMMAND_LIST_FLAG_IN_ORDER,
    };
    ze_command_list_handle_t commandList;
    ASSERT_EQ(zeCommandListCreate(context, device, &commandListDesc, &commandList),
              ZE_RESULT_SUCCESS);

    size_t size = 10;
    std::shared_ptr<void> input = zeMemory::allocShared(context, device, size);
    std::shared_ptr<void> output = zeMemory::allocShared(context, device, size);
    uint8_t *inputPtr = reinterpret_cast<uint8_t *>(input.get());
    uint8_t *outputPtr = reinterpret_cast<uint8_t *>(output.get());

    for (size_t i = 0; i < size; i++)
        inputPtr[i] = i + 1;

    ASSERT_EQ(
        zeCommandListAppendMemoryCopy(commandList, outputPtr, inputPtr, size, nullptr, 0, nullptr),
        ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(commandList), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(commandQueue, 1, &commandList, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(commandQueue, 10e9), ZE_RESULT_SUCCESS);

    ASSERT_EQ(memcmp(input.get(), output.get(), size), 0);
}

TEST(zeInitDrivers, InitializeAndExecuteCopyCommand) {
    if (!test_vars::initialization_tests)
        SKIP_("The test is skipped because --initialization_tests flag is missing");

    assertNotInitialized();

    ze_init_driver_type_desc_t initDriverDesc{
        .stype = ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC,
        .pNext = nullptr,
        .flags = ZE_INIT_DRIVER_TYPE_FLAG_NPU,
    };
    uint32_t numDrivers = 0;
    ASSERT_EQ(zeInitDrivers(&numDrivers, nullptr, &initDriverDesc), ZE_RESULT_SUCCESS);
    ASSERT_GT(numDrivers, 0);

    std::vector<ze_driver_handle_t> drivers(numDrivers);
    ASSERT_EQ(zeInitDrivers(&numDrivers, &drivers[0], &initDriverDesc), ZE_RESULT_SUCCESS);

    executeCopyCommand(drivers[0]);
}

class InitFlagsTest : public testing::TestWithParam<int> {};

TEST_P(InitFlagsTest, CallzeInitThenzeInitDriversThenExecuteCopyCommand) {
    if (!test_vars::initialization_tests)
        SKIP_("The test is skipped because --initialization_tests flag is missing");

    assertNotInitialized();

    ASSERT_EQ(zeInit(GetParam()), ZE_RESULT_SUCCESS);

    ze_init_driver_type_desc_t initDriverDesc{
        .stype = ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC,
        .pNext = nullptr,
        .flags = ZE_INIT_DRIVER_TYPE_FLAG_NPU,
    };
    uint32_t numDrivers = 0;
    ASSERT_EQ(zeInitDrivers(&numDrivers, nullptr, &initDriverDesc), ZE_RESULT_SUCCESS);
    ASSERT_GT(numDrivers, 0);

    std::vector<ze_driver_handle_t> drivers(numDrivers);
    ASSERT_EQ(zeInitDrivers(&numDrivers, &drivers[0], &initDriverDesc), ZE_RESULT_SUCCESS);

    executeCopyCommand(drivers[0]);
}

INSTANTIATE_TEST_SUITE_P(zeInitDrivers, InitFlagsTest, testing::Values(0, ZE_INIT_FLAG_VPU_ONLY));
