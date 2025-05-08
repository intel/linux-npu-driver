/*
 * Copyright (C) 2024-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "testenv.hpp"
#include "ze_memory.hpp"

#include <gtest/gtest.h>
#include <level_zero/ze_intel_npu_uuid.h>

static void assertNotInitialized() {
    uint32_t numDrivers = 1;
    ze_driver_handle_t driver;
    ASSERT_EQ(zeDriverGet(&numDrivers, &driver), ZE_RESULT_ERROR_UNINITIALIZED)
        << "The driver is already initialized. It means that zeInit/zeInitDrivers was called prior "
           "and/or multiple test cases are executed in a single process. This is not supported - "
           "each case needs to be run separetely.";
}

static void RunInFork(const std::function<void()> &testFunction) {
    pid_t pid = fork();
    if (pid == 0) {
        testFunction();
        exit(::testing::Test::HasFailure());
    } else if (pid > 0) {
        int status;
        auto ret = waitpid(pid, &status, 0);
        ASSERT_EQ(ret, pid) << "Failed to wait for child process";
        ASSERT_EQ(status, 0) << "Test failed or interrupted by signal in child process";
    } else {
        FAIL() << "Failed to fork process";
    }
}

class ZeInitDriversTest : public testing::Test {
  protected:
    void SetUp() override {
        if (!test_vars::forceZeInitTests)
            SKIP_("The test is skipped because --ze-init-tests flag is missing");
    }
};

TEST_F(ZeInitDriversTest, FailWithoutInit) {
    RunInFork([]() { assertNotInitialized(); });
}

static void executeCopyCommand(ze_driver_handle_t driver) {
    uint32_t numDevices = 1;
    ze_device_handle_t device;
    ASSERT_EQ(zeDeviceGet(driver, &numDevices, &device), ZE_RESULT_SUCCESS);

    ze_context_desc_t contextDesc{.stype = ZE_STRUCTURE_TYPE_CONTEXT_DESC,
                                  .pNext = nullptr,
                                  .flags = 0};
    ze_result_t ret;
    auto sContext = zeScope::contextCreate(driver, contextDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create zeContext";
    auto context = sContext.get();

    ze_command_queue_desc_t commandQueueDesc{
        .stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
        .pNext = nullptr,
        .ordinal = 0,
        .index = 0,
        .flags = ZE_COMMAND_QUEUE_FLAG_EXPLICIT_ONLY,
        .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
        .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL,
    };
    auto sCommandQueue = zeScope::commandQueueCreate(context, device, commandQueueDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create zeCommandQueue";
    ze_command_queue_handle_t commandQueue = sCommandQueue.get();

    ze_command_list_desc_t commandListDesc{
        .stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
        .pNext = nullptr,
        .commandQueueGroupOrdinal = 0,
        .flags = ZE_COMMAND_LIST_FLAG_IN_ORDER,
    };
    auto sCommandList = zeScope::commandListCreate(context, device, commandListDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create zeCommandList";
    ze_command_list_handle_t commandList = sCommandList.get();

    size_t size = 10;
    std::shared_ptr<void> input = zeMemory::allocShared(context, device, size);
    std::shared_ptr<void> output = zeMemory::allocShared(context, device, size);
    uint8_t *inputPtr = static_cast<uint8_t *>(input.get());
    uint8_t *outputPtr = static_cast<uint8_t *>(output.get());

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

TEST_F(ZeInitDriversTest, InitializeAndExecuteCopyCommand) {
    RunInFork([]() {
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
    });
}

class ZeInitTest : public ZeInitDriversTest, public ::testing::WithParamInterface<int> {};

TEST_P(ZeInitTest, CallzeInitThenzeInitDriversThenExecuteCopyCommand) {
    RunInFork([]() {
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
    });
}

TEST_P(ZeInitTest, CallzeInitThenExecuteCopyCommand) {
    RunInFork([]() {
        assertNotInitialized();

        ASSERT_EQ(zeInit(GetParam()), ZE_RESULT_SUCCESS);

        uint32_t numDrivers = 0;
        ASSERT_EQ(zeDriverGet(&numDrivers, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_GT(numDrivers, 0);

        std::vector<ze_driver_handle_t> drivers(numDrivers);
        ASSERT_EQ(zeDriverGet(&numDrivers, &drivers[0]), ZE_RESULT_SUCCESS);

        ze_driver_handle_t driver = nullptr;
        ze_driver_uuid_t uuid = ze_intel_npu_driver_uuid;
        for (uint32_t i = 0; i < numDrivers; i++) {
            ze_driver_properties_t properties = {
                .stype = ZE_STRUCTURE_TYPE_DRIVER_PROPERTIES,
                .pNext = nullptr,
                .uuid = {},
                .driverVersion = 0,
            };
            ASSERT_EQ(zeDriverGetProperties(drivers[i], &properties), ZE_RESULT_SUCCESS);
            if (memcmp(&properties.uuid, &uuid, sizeof(uuid)) == 0) {
                driver = drivers[0];
                break;
            }
        }

        ASSERT_NE(driver, nullptr);

        executeCopyCommand(driver);
    });
}

INSTANTIATE_TEST_SUITE_P(, ZeInitTest, testing::Values(0, ZE_INIT_FLAG_VPU_ONLY));
