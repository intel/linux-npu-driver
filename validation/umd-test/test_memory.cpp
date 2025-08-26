/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "frame_counter.hpp"
#include "umd_test.h"
#include "utilities/data_handle.h"

#include <algorithm>
#include <memory>
#include <thread>
#include <vector>

class MemoryAllocation : public UmdTest, public ::testing::WithParamInterface<YAML::Node> {
  public:
    void SetUp() override {
        UmdTest::SetUp();
        pMemAllocProperties.stype = ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES;
    }

    void TearDown() override { UmdTest::TearDown(); }

    static uint64_t decodeSize(const YAML::Node &node) {
        if (node["size_in_bytes"].IsDefined())
            return node["size_in_bytes"].as<uint64_t>();

        throw std::runtime_error("Missing size_in_bytes field in config");
    }

    static std::string decodeType(const YAML::Node &node) {
        return node["type"].as<std::string>("host_to_host");
    }

    void allocMemoryBasedOnType(const std::string &type,
                                size_t size,
                                std::shared_ptr<void> &dst,
                                std::shared_ptr<void> &src) {
        try {
            const std::string_view sep = "_to_";
            const auto sepPos = type.find(sep);
            if (sepPos == std::string::npos)
                FAIL() << "Unkown type: " << type << " passed in configuration";

            const auto srcName = type.substr(0, sepPos);
            if (srcName == "device") {
                src = AllocDeviceMemory(size);
            } else if (srcName == "shared") {
                src = AllocSharedMemory(size);
            } else if (srcName == "host") {
                src = AllocHostMemory(size);
            } else {
                FAIL() << "Unkown type: " << type << " passed in configuration";
            }

            const auto dstName = type.substr(sepPos + sep.size());
            if (dstName == "device") {
                dst = AllocDeviceMemory(size);
            } else if (dstName == "shared") {
                dst = AllocSharedMemory(size);
            } else if (dstName == "host") {
                dst = AllocHostMemory(size);
            } else {
                FAIL() << "Unkown type: " << type << " passed in configuration";
            }
        } catch (const std::out_of_range &err) {
            FAIL() << "Unkown type: " << type << " passed in configuration";
        }
    }

    ze_memory_allocation_properties_t pMemAllocProperties = {};
};

static std::vector<YAML::Node> getMemoryAllocationParameters() {
    if (Environment::getConfiguration()["alloc"].IsDefined())
        return Environment::getConfiguration("alloc");

    return {
        YAML::Load("size_in_bytes: 2048"),
        YAML::Load("size_in_bytes: " + std::to_string(16 * MB)),
        YAML::Load("size_in_bytes: " + std::to_string(128 * MB)),
    };
}

INSTANTIATE_TEST_SUITE_P(,
                         MemoryAllocation,
                         ::testing::ValuesIn(getMemoryAllocationParameters()),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return memSizeToStr(MemoryAllocation::decodeSize(p.param));
                         });

TEST_P(MemoryAllocation, AllocDeviceMemory) {
    uint64_t size = decodeSize(GetParam());
    auto mem = AllocDeviceMemory(size);
    ASSERT_TRUE(mem.get()) << "Failed to allocate device memory using size " << size;
}

TEST_P(MemoryAllocation, AllocHostMemory) {
    uint64_t size = decodeSize(GetParam());
    auto mem = AllocHostMemory(size);
    ASSERT_TRUE(mem.get()) << "Failed to allocate host memory using size " << size;
}

TEST_F(MemoryAllocation, QueryContextMemory) {
    uint64_t size = 48 * KB;
    auto mem = AllocHostMemory(size);
    ASSERT_TRUE(mem.get());

    ze_graph_memory_query_t memQuery = {};
    EXPECT_EQ(
        zeGraphDDITableExt->pfnQueryContextMemory(zeContext, ZE_GRAPH_QUERY_MEMORY_DDR, &memQuery),
        ZE_RESULT_SUCCESS);
    EXPECT_EQ(memQuery.allocated, size);

    TRACE("MemoryQuery.allocated: %lu\n", memQuery.allocated);
    TRACE("MemoryQuery.total: %lu\n", memQuery.total);
}

TEST_F(MemoryAllocation, GetMemAddressRangeWithoutMemInputExpectSuccess) {
    size_t size = 128;

    auto mem = AllocHostMemory(size);
    ASSERT_NE(nullptr, mem.get());
    void *ptr = mem.get();

    ASSERT_EQ(
        ZE_RESULT_SUCCESS,
        zeMemGetAddressRange(zeContext, static_cast<char *>(ptr) + size - 1, nullptr, nullptr));
}

TEST_F(MemoryAllocation, GetMemAddressRangeWithMemInputExpectSuccess) {
    size_t size = 10;

    auto mem = AllocHostMemory(size);
    EXPECT_NE(nullptr, mem.get());
    void *ptr = mem.get();

    size_t pSize = 0;
    void *basePtr = nullptr;

    ASSERT_EQ(
        ZE_RESULT_SUCCESS,
        zeMemGetAddressRange(zeContext, static_cast<char *>(ptr) + size - 1, &basePtr, &pSize));
    EXPECT_NE(basePtr, nullptr);
    EXPECT_EQ(size, pSize);
}

TEST_F(MemoryAllocation, GetMemPropertiesWithAllocHostMemExpectSuccess) {
    size_t size = 10;
    void *ptr = nullptr;

    auto mem = AllocHostMemory(size);
    ASSERT_NE(nullptr, ptr = mem.get());

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeMemGetAllocProperties(zeContext,
                                      static_cast<char *>(ptr) + size - 1,
                                      &pMemAllocProperties,
                                      nullptr));
    EXPECT_EQ(pMemAllocProperties.stype, ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES);
    EXPECT_EQ(pMemAllocProperties.pNext, nullptr);
    EXPECT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_HOST);
    EXPECT_GT(pMemAllocProperties.id, 0u);
    EXPECT_EQ(pMemAllocProperties.pageSize, size);
}

TEST_F(MemoryAllocation, GetMemPropertiesWithAllocDeviceMemExpectSuccess) {
    size_t size = 10;
    void *ptr = nullptr;

    auto mem = AllocDeviceMemory(size);
    ASSERT_NE(nullptr, ptr = mem.get());

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeMemGetAllocProperties(zeContext,
                                      static_cast<char *>(ptr) + size - 1,
                                      &pMemAllocProperties,
                                      nullptr));
    EXPECT_EQ(pMemAllocProperties.stype, ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES);
    EXPECT_EQ(pMemAllocProperties.pNext, nullptr);
    EXPECT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_DEVICE);
    EXPECT_GT(pMemAllocProperties.id, 0u);
    EXPECT_EQ(pMemAllocProperties.pageSize, size);
}

class MemoryExecution : public MemoryAllocation {
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

    void SetUp() override {
        MemoryAllocation::SetUp();

        ze_result_t ret;
        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();
    }

    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
};

static std::vector<YAML::Node> getMemoryExecutionParameters() {
    if (Environment::getConfiguration()["copy"].IsDefined())
        return Environment::getConfiguration("copy");

    return {
        YAML::Load("{ size_in_bytes: 2048, type: host_to_host }"),
        YAML::Load("{ size_in_bytes: 8096, type: shared_to_shared }"),
        YAML::Load("{ size_in_bytes: 65536, type: host_to_device }"),
        YAML::Load("{ size_in_bytes: " + std::to_string(4 * MB) + ", type: shared_to_host}"),
        YAML::Load("size_in_bytes: " + std::to_string(16 * MB - 1)),
        YAML::Load("size_in_bytes: " + std::to_string(16 * MB)),
    };
}

INSTANTIATE_TEST_SUITE_P(,
                         MemoryExecution,
                         ::testing::ValuesIn(getMemoryExecutionParameters()),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return MemoryExecution::decodeType(p.param) + "_" +
                                    memSizeToStr(p.param["size_in_bytes"].as<uint64_t>());
                         });

TEST_P(MemoryExecution, AllocMemoryExecuteCopyCommand) {
    size_t size = decodeSize(GetParam());

    std::shared_ptr<void> dst;
    std::shared_ptr<void> src;
    allocMemoryBasedOnType(decodeType(GetParam()), size, dst, src);
    ASSERT_TRUE(dst.get()) << "Failed to allocate destination memory";
    ASSERT_TRUE(src.get()) << "Failed to allocate source memory";

    DataHandle::generateRandomData(src.get(), size);

    ASSERT_EQ(zeCommandListAppendMemoryCopy(list, dst.get(), src.get(), size, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    ASSERT_EQ(memcmp(dst.get(), src.get(), size), 0);
}

// TODO: Allow copy from user pointer, EISW-19284
TEST_F(MemoryExecution, DISABLED_CopyingFromUnpinnedHostMemoryShouldBeAllowed) {
    size_t size = 4 * 1024;

    // Unpinned host memory.
    std::vector<uint8_t> hostMem(size);
    auto destDevMem = AllocSharedMemory(size);
    memset(hostMem.data(), 0xCD, size);

    EXPECT_EQ(zeCommandListAppendMemoryCopy(list,
                                            destDevMem.get(),
                                            hostMem.data(),
                                            size,
                                            nullptr,
                                            0,
                                            nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);

    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS)
        << "Failed to pin memory using size " << size;
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);
    EXPECT_EQ(memcmp(destDevMem.get(), hostMem.data(), size), 0);
}

TEST_F(MemoryExecution, CheckQueryContextMemory) {
    ze_graph_memory_query_t memAllocated = {};
    size_t testAllocSize = 20000;

    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeGraphDDITableExt->pfnQueryContextMemory(zeContext,
                                                        ZE_GRAPH_QUERY_MEMORY_DDR,
                                                        &memAllocated));
    size_t initialMem = memAllocated.allocated;

    auto mem = AllocHostMemory(testAllocSize);
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeGraphDDITableExt->pfnQueryContextMemory(zeContext,
                                                        ZE_GRAPH_QUERY_MEMORY_DDR,
                                                        &memAllocated));
    size_t userAllocatedMem = memAllocated.allocated;
    EXPECT_EQ(userAllocatedMem, initialMem + testAllocSize);

    ASSERT_EQ(zeCommandListAppendBarrier(list, nullptr, 0, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(ZE_RESULT_SUCCESS,
              zeGraphDDITableExt->pfnQueryContextMemory(zeContext,
                                                        ZE_GRAPH_QUERY_MEMORY_DDR,
                                                        &memAllocated));

    /* Expected some internal allocations for command buffer*/
    EXPECT_GT(memAllocated.allocated, userAllocatedMem);
}

enum CopyDimension { one_dimension, two_dimension };

class MemoryCopyExecution : public MemoryAllocation {
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

    void SetUp() override {
        MemoryAllocation::SetUp();

        ze_result_t ret;
        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();

        const YAML::Node &node = GetParam();

        if (node["dimension"].IsDefined() && node["dimension"].as<std::string>().size()) {
            std::string dim = node["dimension"].as<std::string>();

            if (dim == "one_dimension") {
                copyDimension = CopyDimension::one_dimension;
                srcNumElements = node["src_num_elements"].as<uint32_t>();
                dstNumElements = node["dst_num_elements"].as<uint32_t>();
            } else if (dim == "two_dimension") {
                copyDimension = CopyDimension::two_dimension;
                srcRows = node["src_rows"].as<uint32_t>();
                srcCols = node["src_cols"].as<uint32_t>();
                dstRows = node["dst_rows"].as<uint32_t>();
                dstCols = node["dst_cols"].as<uint32_t>();
            }
        }
    }

    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;

    CopyDimension copyDimension = CopyDimension::one_dimension;
    // 1D region
    uint32_t srcNumElements = 0u;
    uint32_t dstNumElements = 0u;
    // 2D region
    uint32_t srcRows = 0u;
    uint32_t srcCols = 0u;
    uint32_t dstRows = 0u;
    uint32_t dstCols = 0u;

    size_t srcSizeInBytes = 0u;
    size_t dstSizeInBytes = 0u;

    std::shared_ptr<void> srcMem = nullptr;
    std::shared_ptr<void> dstMem = nullptr;

    uint64_t *srcArray = nullptr;
    uint64_t *dstArray = nullptr;

    ze_copy_region_t srcRegion = {};
    ze_copy_region_t dstRegion = {};

    uint32_t srcPitchInBytes = 0u;
    uint32_t dstPitchInBytes = 0u;
    // only used for 3D case
    uint32_t slicePitchInBytes = 0u;

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
};

static std::vector<YAML::Node> getMemoryCopyExecutionParameters() {
    if (Environment::getConfiguration()["copy_region"].IsDefined())
        return Environment::getConfiguration("copy_region");

    return {
        YAML::Load("{ dimension: one_dimension, src_num_elements: 10, dst_num_elements: 7 }"),
        YAML::Load(
            "{ dimension: two_dimension, src_rows: 6, src_cols: 6, dst_rows: 4, dst_cols: 6 }"),
    };
}

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(MemoryCopyExecution);

INSTANTIATE_TEST_SUITE_P(,
                         MemoryCopyExecution,
                         ::testing::ValuesIn(getMemoryCopyExecutionParameters()),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return p.param["dimension"].as<std::string>();
                             ;
                         });

TEST_P(MemoryCopyExecution, AppendMemoryCopyRegion) {
    switch (copyDimension) {
    case CopyDimension::one_dimension: {
        srcSizeInBytes = srcNumElements * sizeof(uint64_t);
        dstSizeInBytes = dstNumElements * sizeof(uint64_t);

        srcMem = AllocSharedMemory(srcSizeInBytes);
        dstMem = AllocSharedMemory(dstSizeInBytes);

        srcArray = reinterpret_cast<uint64_t *>(srcMem.get());
        dstArray = reinterpret_cast<uint64_t *>(dstMem.get());

        DataHandle::generateRandomData(srcArray, srcSizeInBytes);

        memset(dstArray, 0, dstSizeInBytes);

        // Define the source region:
        // Copy 4 elements starting at index 2 in the source
        srcRegion.originX = 2 * sizeof(uint64_t);
        srcRegion.originY = 0;
        srcRegion.originZ = 0;
        srcRegion.width = 4 * sizeof(uint64_t);
        srcRegion.height = 1;
        srcRegion.depth = 1;

        // Define the destination region:
        // Copy into the beginning of the buffer
        dstRegion.originX = 0;
        dstRegion.originY = 0;
        dstRegion.originZ = 0;
        dstRegion.width = 4 * sizeof(uint64_t);
        dstRegion.height = 1;
        dstRegion.depth = 1;

        // For a linear array, the pitch is the full array size
        srcPitchInBytes = srcNumElements * sizeof(uint64_t);
        dstPitchInBytes = dstNumElements * sizeof(uint64_t);

        TRACE("Source buffer:\n");
        for (uint32_t i = 0; i < srcNumElements; i++) {
            TRACE("0x%020lx ", srcArray[i]);
        }
        TRACE("\n");

        TRACE("Destination buffer before copy:\n");
        for (uint32_t i = 0; i < dstNumElements; i++) {
            TRACE("0x%020lx ", dstArray[i]);
        }
        TRACE("\n");
    } break;
    case CopyDimension::two_dimension: {
        // Allocate a 6x6 matrix as source buffer and 4x6 matrix as destination buffer
        srcSizeInBytes = srcRows * srcCols * sizeof(uint64_t);
        dstSizeInBytes = dstRows * dstCols * sizeof(uint64_t);

        srcMem = AllocSharedMemory(srcSizeInBytes);
        dstMem = AllocSharedMemory(dstSizeInBytes);

        srcArray = reinterpret_cast<uint64_t *>(srcMem.get());
        dstArray = reinterpret_cast<uint64_t *>(dstMem.get());

        DataHandle::generateRandomData(srcArray, srcSizeInBytes);
        memset(dstArray, 0, dstSizeInBytes);

        // Define the source region: 3x3 block
        // Source region starts at row 1, column 2
        srcRegion.originX = 2 * sizeof(uint64_t);
        srcRegion.originY = 1;
        srcRegion.originZ = 0;
        srcRegion.width = 3 * sizeof(uint64_t);
        srcRegion.height = 3;
        srcRegion.depth = 1;

        // Destination region starts at row 1, column 0
        dstRegion.originX = 0;
        dstRegion.originY = 1;
        dstRegion.originZ = 0;
        dstRegion.width = 3 * sizeof(uint64_t);
        dstRegion.height = 3;
        dstRegion.depth = 1;

        // The pitch is the full row size
        srcPitchInBytes = srcCols * sizeof(uint64_t);
        dstPitchInBytes = dstCols * sizeof(uint64_t);

        TRACE("Source buffer:\n");
        for (uint32_t r = 0; r < srcRows; r++) {
            for (uint32_t c = 0; c < srcCols; c++) {
                TRACE("0x%020lx ", srcArray[r * srcCols + c]);
            }
            TRACE("\n");
        }
        TRACE("\n");

        TRACE("Destination buffer before copy:\n");
        for (uint32_t r = 0; r < dstRows; r++) {
            for (uint32_t c = 0; c < dstCols; c++) {
                TRACE("0x%020lx ", dstArray[r * dstCols + c]);
            }
            TRACE("\n");
        }
        TRACE("\n");
    } break;
    default:
        break;
    }
    ASSERT_EQ(zeCommandListAppendMemoryCopyRegion(list,
                                                  dstMem.get(),
                                                  &dstRegion,
                                                  dstPitchInBytes,
                                                  slicePitchInBytes,
                                                  srcMem.get(),
                                                  &srcRegion,
                                                  srcPitchInBytes,
                                                  slicePitchInBytes,
                                                  nullptr,
                                                  0,
                                                  nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, syncTimeout), ZE_RESULT_SUCCESS);

    // Displaying the destination buffer after the copy operation
    switch (copyDimension) {
    case CopyDimension::one_dimension: {
        TRACE("Destination buffer after copy:\n");
        for (uint32_t i = 0; i < dstNumElements; i++) {
            TRACE("0x%020lx ", dstArray[i]);
        }
        TRACE("\n");

        // Verification of the copied region
        uint32_t srcOffset = srcRegion.originX + srcPitchInBytes;
        uint32_t dstOffset = dstRegion.originX + dstPitchInBytes;
        EXPECT_EQ(memcmp(srcArray + srcOffset, dstArray + dstOffset, dstRegion.width), 0);
    } break;
    case CopyDimension::two_dimension: {
        TRACE("Destination buffer after copy:\n");
        for (uint32_t r = 0; r < dstRows; r++) {
            for (uint32_t c = 0; c < dstCols; c++) {
                TRACE("0x%020lx ", dstArray[r * dstCols + c]);
            }
            TRACE("\n");
        }
        TRACE("\n");

        // Verification of the copied region
        for (uint32_t y = 0; y < dstRegion.height; y++) {
            uint32_t srcOffset = srcRegion.originX + (srcRegion.originY + y) * srcPitchInBytes;
            uint32_t dstOffset = dstRegion.originX + (dstRegion.originY + y) * dstPitchInBytes;
            EXPECT_EQ(memcmp(srcArray + srcOffset, dstArray + dstOffset, dstRegion.width), 0);
        }
    } break;
    default:
        break;
    }
}

class MemoryAllocationThreaded : public UmdTest {
  public:
    void SetUp() override {
        mem.resize(numOfThreadedTasks);
        UmdTest::SetUp();
    }
    void TearDown() override { UmdTest::TearDown(); }

    ze_host_mem_alloc_desc_t hostMemAllocDesc = {.stype = ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC,
                                                 .pNext = nullptr,
                                                 .flags = 0};

    ze_device_mem_alloc_desc_t deviceMemAllocDesc = {.stype =
                                                         ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
                                                     .pNext = nullptr,
                                                     .flags = 0,
                                                     .ordinal = 0};

    void allocHostMem(void **ptr) {
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeMemAllocHost(zeContext, &hostMemAllocDesc, size, 0, ptr));
    }

    void allocDeviceMem(void **ptr) {
        ASSERT_EQ(ZE_RESULT_SUCCESS,
                  zeMemAllocDevice(zeContext, &deviceMemAllocDesc, size, 0, zeDevice, ptr));
    }

    void freeMem(void *ptr) { ASSERT_EQ(ZE_RESULT_SUCCESS, zeMemFree(zeContext, ptr)); }

    uint16_t numOfThreadedTasks = 32;
    std::vector<std::unique_ptr<std::thread>> tasks;
    std::vector<void *> mem;
    uint64_t size = 8192;
};

TEST_F(MemoryAllocationThreaded, HostMem) {
    for (auto &m : mem) {
        tasks.push_back(
            std::make_unique<std::thread>(&MemoryAllocationThreaded::allocHostMem, this, &m));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
    for (const auto &m : mem) {
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeMemFree(zeContext, m));
    }
}

TEST_F(MemoryAllocationThreaded, DeviceMem) {
    for (auto &m : mem) {
        tasks.push_back(
            std::make_unique<std::thread>(&MemoryAllocationThreaded::allocDeviceMem, this, &m));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
    for (const auto &m : mem) {
        ASSERT_EQ(ZE_RESULT_SUCCESS, zeMemFree(zeContext, m));
    }
}

TEST_F(MemoryAllocationThreaded, FreeMem) {
    for (auto &m : mem) {
        allocDeviceMem(&m);
    }
    for (auto &m : mem) {
        tasks.push_back(std::make_unique<std::thread>(&MemoryAllocationThreaded::freeMem, this, m));
    }
    for (const auto &t : tasks) {
        t.get()->join();
    }
}

class MultiMemoryExecution : public MemoryAllocation {
  public:
    struct MultiCopyParam {
        // Input
        uint64_t size;
        std::string type;
        uint64_t iterationCount;
        uint64_t targetFps;
        uint64_t delayUs;

        // Results
        ze_result_t result = ZE_RESULT_SUCCESS;
        FrameCounter counter;
    };

    static std::pair<std::string, std::vector<MultiCopyParam>>
    decodeMultiCopy(const YAML::Node &node) {
        auto name = node["name"].as<std::string>();

        std::vector<MultiCopyParam> params;
        for (const auto &pipeline : node["pipeline"].as<std::vector<YAML::Node>>()) {
            MultiCopyParam param;
            param.size = decodeSize(pipeline);
            param.type = decodeType(pipeline);
            param.iterationCount = pipeline["iteration_count"].as<uint64_t>(10);
            param.targetFps = pipeline["target_fps"].as<uint64_t>(UINT64_MAX);
            param.delayUs = pipeline["delay_in_us"].as<uint64_t>(0);
            params.push_back(std::move(param));
        }

        return {name, params};
    }

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
};

static std::vector<YAML::Node> getMultiMemoryExecutionParameters() {
    if (Environment::getConfiguration()["multi_copy"].IsDefined())
        return Environment::getConfiguration("multi_copy");

    return {
        YAML::Load("{name: 3CopyStreams, pipeline: ["
                   "{size_in_bytes: 4096, target_fps: 100, iteration_count: 10},"
                   "{size_in_bytes: 4096, type: shared_to_host, iteration_count: 200},"
                   "{size_in_bytes: 8096, iteration_count: 100}]}"),
    };
}

INSTANTIATE_TEST_SUITE_P(,
                         MultiMemoryExecution,
                         ::testing::ValuesIn(getMultiMemoryExecutionParameters()),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return MultiMemoryExecution::decodeMultiCopy(p.param).first;
                         });

#define BREAK_ON_FAIL(ret_)                 \
    if (ret_ != ZE_RESULT_SUCCESS) {        \
        EXPECT_EQ(ret_, ZE_RESULT_SUCCESS); \
        param.result = ret_;                \
        break;                              \
    }

TEST_P(MultiMemoryExecution, Pipeline) {
    auto [name, params] = decodeMultiCopy(GetParam());
    std::vector<std::thread> threads;
    for (auto &param : params) {
        threads.emplace_back([this, &param]() {
            do {
                ze_result_t ret;
                auto scopedQueue =
                    zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
                BREAK_ON_FAIL(ret);
                auto queue = scopedQueue.get();

                auto scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
                BREAK_ON_FAIL(ret);
                auto list = scopedList.get();

                std::shared_ptr<void> dst;
                std::shared_ptr<void> src;
                allocMemoryBasedOnType(param.type, param.size, dst, src);
                ret = dst == nullptr || src == nullptr ? ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY
                                                       : ZE_RESULT_SUCCESS;
                BREAK_ON_FAIL(ret);

                DataHandle::generateRandomData(src.get(), param.size);

                BREAK_ON_FAIL(zeCommandListAppendMemoryCopy(list,
                                                            dst.get(),
                                                            src.get(),
                                                            param.size,
                                                            nullptr,
                                                            0,
                                                            nullptr));
                BREAK_ON_FAIL(zeCommandListClose(list));
                // Warm up
                BREAK_ON_FAIL(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr));
                BREAK_ON_FAIL(zeCommandQueueSynchronize(queue, syncTimeout));

                std::this_thread::sleep_for(std::chrono::microseconds(param.delayUs));

                param.counter.startTimer(0, param.targetFps);
                for (size_t i = 0; i < param.iterationCount; i++) {
                    param.counter.delayNextFrame();

                    BREAK_ON_FAIL(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr));
                    BREAK_ON_FAIL(zeCommandQueueSynchronize(queue, syncTimeout));

                    param.counter.recordFrame();
                }
                param.counter.stopTimer();
            } while (false);
        });
    }

    for (auto &thread : threads)
        thread.join();

    for (const auto &param : params) {
        PRINTF("------------------------------------------------------\n");
        PRINTF("Size/Type:            %lu/%s\n", param.size, param.type.c_str());
        if (param.result == ZE_RESULT_SUCCESS) {
            PRINTF("Status:               SUCCESS \n");
        } else {
            PRINTF("Status:               FAIL (%#x) \n", param.result);
        }
        PRINTF("FramesExecuted:       %lu\n", param.counter.frameCount);
        PRINTF("CalculatedFPS:        %f\n", param.counter.fps);
        PRINTF("Bandwidth[MB/s]:      %f\n", param.counter.getBytesPerSec(param.size) / (MB));
        PRINTF("ExecutionTime[ms]:    %f\n", param.counter.totalTimeMs);
        PRINTF("MinFrameExecTime[ms]: %f\n", param.counter.frameMinMs);
        PRINTF("AvgFrameExecTime[ms]: %f\n", param.counter.frameAvgMs);
        PRINTF("MaxFrameExecTime[ms]: %f\n", param.counter.frameMaxMs);
    }
}
