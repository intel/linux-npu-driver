/*
 * Copyright (C) 2024-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "testenv.hpp"
#include "ze_memory.hpp"

#include <cstdlib>
#include <gtest/gtest.h>
#include <unistd.h>
#include <ze_intel_npu_uuid.h>

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

    static ze_init_driver_app_version_ext_desc_t
    makeAppVersionDesc(ze_api_version_t apiVersionHint) {
        return ze_init_driver_app_version_ext_desc_t{
            .stype = ZE_STRUCTURE_TYPE_INIT_DRIVER_APP_VERSION_EXT_DESC,
            .pNext = nullptr,
            .apiVersionHint = apiVersionHint,
        };
    }

    static ze_init_driver_type_desc_t makeInitDriverDesc(const void *pNext = nullptr) {
        return ze_init_driver_type_desc_t{
            .stype = ZE_STRUCTURE_TYPE_INIT_DRIVER_TYPE_DESC,
            .pNext = pNext,
            .flags = ZE_INIT_DRIVER_TYPE_FLAG_NPU,
        };
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

        ze_init_driver_type_desc_t initDriverDesc = makeInitDriverDesc();
        uint32_t numDrivers = 0;
        ASSERT_EQ(zeInitDrivers(&numDrivers, nullptr, &initDriverDesc), ZE_RESULT_SUCCESS);
        ASSERT_GT(numDrivers, 0);

        std::vector<ze_driver_handle_t> drivers(numDrivers);
        ASSERT_EQ(zeInitDrivers(&numDrivers, &drivers[0], &initDriverDesc), ZE_RESULT_SUCCESS);

        executeCopyCommand(drivers[0]);
    });
}

static void checkImportedSystemMemoryType(ze_driver_handle_t driver,
                                          ze_memory_type_t expectedType) {
    uint32_t numDevices = 1;
    ze_device_handle_t device;
    ASSERT_EQ(zeDeviceGet(driver, &numDevices, &device), ZE_RESULT_SUCCESS);

    ze_device_external_memory_properties_t extMemProps = {};
    extMemProps.stype = ZE_STRUCTURE_TYPE_DEVICE_EXTERNAL_MEMORY_PROPERTIES;
    ASSERT_EQ(zeDeviceGetExternalMemoryProperties(device, &extMemProps), ZE_RESULT_SUCCESS);
    if (!(extMemProps.memoryAllocationImportTypes &
          ZE_EXTERNAL_MEMORY_TYPE_FLAG_STANDARD_ALLOCATION)) // NOLINT
        SKIP_("System memory import type is not supported, skipping the test.");

    ze_context_desc_t contextDesc{.stype = ZE_STRUCTURE_TYPE_CONTEXT_DESC,
                                  .pNext = nullptr,
                                  .flags = 0};
    ze_result_t ret;
    auto sContext = zeScope::contextCreate(driver, contextDesc, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to create zeContext";
    auto context = sContext.get();

    size_t pageSize = getpagesize();
    void *mallocPtr = aligned_alloc(pageSize, pageSize);
    ASSERT_NE(mallocPtr, nullptr) << "Failed to allocate memory";
    std::shared_ptr<void> scopedMallocPtr(mallocPtr, [](void *p) { free(p); });

    ze_external_memmap_sysmem_ext_desc_t importDesc =
        {ZE_STRUCTURE_TYPE_EXTERNAL_MEMMAP_SYSMEM_EXT_DESC, nullptr, mallocPtr, pageSize};
    ze_host_mem_alloc_desc_t hostMemAllocDesc = {ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC,
                                                 &importDesc,
                                                 0};
    auto sImportedMem = zeScope::memAllocHost(context, hostMemAllocDesc, pageSize, 0, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS) << "Failed to import system memory";

    ze_memory_allocation_properties_t props = {};
    ASSERT_EQ(zeMemGetAllocProperties(context, sImportedMem.get(), &props, nullptr),
              ZE_RESULT_SUCCESS);
    EXPECT_EQ(props.type, expectedType);
}

TEST_F(ZeInitDriversTest, ImportedSystemMemoryReportsHostWithoutAppVersionHint) {
    RunInFork([]() {
        assertNotInitialized();

        ze_init_driver_type_desc_t initDriverDesc = makeInitDriverDesc();
        uint32_t numDrivers = 0;
        ASSERT_EQ(zeInitDrivers(&numDrivers, nullptr, &initDriverDesc), ZE_RESULT_SUCCESS);
        ASSERT_GT(numDrivers, 0);

        std::vector<ze_driver_handle_t> drivers(numDrivers);
        ASSERT_EQ(zeInitDrivers(&numDrivers, &drivers[0], &initDriverDesc), ZE_RESULT_SUCCESS);

        checkImportedSystemMemoryType(drivers[0], ZE_MEMORY_TYPE_HOST);
    });
}

TEST_F(ZeInitDriversTest, ImportedSystemMemoryReportsHostWithAppVersionHint1_17) {
    RunInFork([]() {
        assertNotInitialized();

        ze_init_driver_app_version_ext_desc_t appVersionDesc =
            makeAppVersionDesc(ZE_API_VERSION_1_17);
        ze_init_driver_type_desc_t initDriverDesc = makeInitDriverDesc(&appVersionDesc);
        uint32_t numDrivers = 0;
        ASSERT_EQ(zeInitDrivers(&numDrivers, nullptr, &initDriverDesc), ZE_RESULT_SUCCESS);
        ASSERT_GT(numDrivers, 0);

        std::vector<ze_driver_handle_t> drivers(numDrivers);
        ASSERT_EQ(zeInitDrivers(&numDrivers, &drivers[0], &initDriverDesc), ZE_RESULT_SUCCESS);

        checkImportedSystemMemoryType(drivers[0], ZE_MEMORY_TYPE_HOST);
    });
}

TEST_F(ZeInitDriversTest, ImportedSystemMemoryReportsHostImportedWithCurrentAppVersionHint) {
    RunInFork([]() {
        assertNotInitialized();

        ze_init_driver_app_version_ext_desc_t appVersionDesc =
            makeAppVersionDesc(ZE_API_VERSION_CURRENT);
        ze_init_driver_type_desc_t initDriverDesc = makeInitDriverDesc(&appVersionDesc);
        uint32_t numDrivers = 0;
        ASSERT_EQ(zeInitDrivers(&numDrivers, nullptr, &initDriverDesc), ZE_RESULT_SUCCESS);
        ASSERT_GT(numDrivers, 0);

        std::vector<ze_driver_handle_t> drivers(numDrivers);
        ASSERT_EQ(zeInitDrivers(&numDrivers, &drivers[0], &initDriverDesc), ZE_RESULT_SUCCESS);

        checkImportedSystemMemoryType(drivers[0], ZE_MEMORY_TYPE_HOST_IMPORTED);
    });
}

class ZeInitTest : public ZeInitDriversTest, public ::testing::WithParamInterface<int> {};

TEST_P(ZeInitTest, CallzeInitThenzeInitDriversThenExecuteCopyCommand) {
    RunInFork([]() {
        assertNotInitialized();

        ASSERT_EQ(zeInit(GetParam()), ZE_RESULT_SUCCESS);

        ze_init_driver_type_desc_t initDriverDesc = makeInitDriverDesc();
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
