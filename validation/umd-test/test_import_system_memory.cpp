/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"
#include "umd_test.h"

#include <cstdlib>
#include <fcntl.h>
#include <level_zero/ze_mem_import_system_memory_ext.h>
#include <sys/mman.h>

class ImportSystemMemoryNoParam : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();
        if (!isImportSystemMemorySupported()) {
            SKIP_("System memory import type is not supported, skipping the test.");
        }
    }

    ze_host_mem_alloc_desc_t hostMemAllocDesc = {.stype = ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC,
                                                 .pNext = nullptr,
                                                 .flags = 0};
};

class ImportSystemMemory : public ImportSystemMemoryNoParam,
                           public ::testing::WithParamInterface<uint64_t> {
  public:
    void SetUp() override { ImportSystemMemoryNoParam::SetUp(); }

    void TearDown() override { ImportSystemMemoryNoParam::TearDown(); }

    void executeCopyCommand(uint8_t *dst, uint8_t *src, uint64_t size, bool skipSrcInit = false) {
        ASSERT_GT(size, 0) << "Size must be greater than zero";
        if (!skipSrcInit)
            DataHandle::generateRandomData(src, size);

        ze_result_t ret = ZE_RESULT_SUCCESS;
        ze_command_list_desc_t cmdListDesc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                              .pNext = nullptr,
                                              .commandQueueGroupOrdinal = 0,
                                              .flags = 0};
        auto cmdListScoped = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        auto hList = cmdListScoped.get();

        ze_command_queue_desc_t cmdQueueDesc{.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                             .pNext = nullptr,
                                             .ordinal = 0,
                                             .index = 0,
                                             .flags = 0,
                                             .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                             .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};
        auto cmdQueueScoped = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        auto hQueue = cmdQueueScoped.get();

        ASSERT_EQ(zeCommandListAppendMemoryCopy(hList, dst, src, size, nullptr, 0, nullptr),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(hList), ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeCommandQueueExecuteCommandLists(hQueue, 1, &hList, nullptr), ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandQueueSynchronize(hQueue, syncTimeout), ZE_RESULT_SUCCESS);
        EXPECT_EQ(memcmp(dst, src, size), 0);
    }

    ze_result_t ret = ZE_RESULT_SUCCESS;
    const char *testFileName = "import_system_memory_test_file";
};

INSTANTIATE_TEST_SUITE_P(,
                         ImportSystemMemory,
                         ::testing::Values(256, 4064, 64 * KB, 4 * MB, 32 * MB),
                         [](const testing::TestParamInfo<uint64_t> &cmd) {
                             return memSizeToStr(cmd.param);
                         });

TEST_F(ImportSystemMemory, MultipleImportSystemMemory) {
    std::vector<uint64_t> testSizes = {256, 4064, 64 * KB, 4 * MB, 32 * MB};

    std::vector<std::shared_ptr<void>> allocPtrs;
    std::vector<std::shared_ptr<void>> importPtrs;

    for (auto testSize : testSizes) {
        void *mallocPtr = aligned_alloc(pageSize, testSize);
        std::shared_ptr<void> scopedPtr(mallocPtr, [](void *p) { free(p); });
        ASSERT_NE(mallocPtr, nullptr) << "Failed to allocate memory";

        auto scopedSystemMemory = UmdTest::importSystemMemory(mallocPtr, testSize);
        ASSERT_NE(scopedSystemMemory.get(), nullptr) << "Failed to import memory";

        allocPtrs.push_back(std::move(scopedPtr));
        importPtrs.push_back(std::move(scopedSystemMemory));
    }

    importPtrs.clear();
    allocPtrs.clear();
}

TEST_P(ImportSystemMemory, AllocMemoryThenExecuteCopy) {
    auto testSize = GetParam();

    // 1st imported allocation uses malloc
    void *mallocPtr = malloc(testSize);
    std::shared_ptr<void> scopedPtr(mallocPtr, [](void *p) { free(p); });
    ASSERT_NE(mallocPtr, nullptr) << "Failed to allocate memory";

    auto scopedSystemMemory = UmdTest::importSystemMemory(mallocPtr, testSize);
    ASSERT_NE(scopedSystemMemory.get(), nullptr) << "Failed to import memory";

    auto scopedHostAlloc = zeScope::memAllocHost(zeContext, hostMemAllocDesc, testSize, 0, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    // First copy from malloc to host alloc
    uint8_t *ptrSrc = static_cast<uint8_t *>(mallocPtr);
    uint8_t *ptrDst = static_cast<uint8_t *>(scopedHostAlloc.get());
    executeCopyCommand(ptrDst, ptrSrc, testSize);

    // 2nd allocation uses aligned_alloc to avoid intersection with 1st allocation within
    // page range. The 2nd allocation has to be in next physical page
    void *alignedAllocPtr = aligned_alloc(pageSize, testSize);
    std::shared_ptr<void> scopedPtrDst(alignedAllocPtr, [](void *p) { free(p); });
    ASSERT_NE(alignedAllocPtr, nullptr) << "Failed to allocate memory";

    auto scopedSystemMemory2nd = UmdTest::importSystemMemory(alignedAllocPtr, testSize);
    ASSERT_NE(scopedSystemMemory2nd.get(), nullptr) << "Failed to import memory";

    // Second copy from host alloc to 2nd system memory import
    ptrSrc = ptrDst;
    ptrDst = static_cast<uint8_t *>(alignedAllocPtr);
    executeCopyCommand(ptrDst, ptrSrc, testSize);

    // Third copy from 2nd system memory import to malloc
    ptrSrc = static_cast<uint8_t *>(alignedAllocPtr);
    ptrDst = static_cast<uint8_t *>(mallocPtr);
    executeCopyCommand(ptrDst, ptrSrc, testSize);
}

TEST_P(ImportSystemMemory, MmapAnonymousThenExecuteCopy) {
    auto testSize = GetParam();

    void *shrPtr =
        mmap(nullptr, testSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    ASSERT_NE(shrPtr, MAP_FAILED) << "Failed to allocate using mmap memory using shared anonymous";
    std::shared_ptr<void> scopedAnonPtr(shrPtr, [size = testSize](void *p) { munmap(p, size); });

    auto scopedSystemMemory = UmdTest::importSystemMemory(shrPtr, testSize);
    ASSERT_NE(scopedSystemMemory.get(), nullptr) << "Failed to import memory mmap shared anonymous";

    auto scopedHostAlloc = zeScope::memAllocHost(zeContext, hostMemAllocDesc, testSize, 0, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    // 1st copy from shared mmap to host alloc
    uint8_t *ptrSrc = static_cast<uint8_t *>(shrPtr);
    uint8_t *ptrDst = static_cast<uint8_t *>(scopedHostAlloc.get());
    executeCopyCommand(ptrDst, ptrSrc, testSize);

    // Test two system memory imports in Context
    void *prvPtr =
        mmap(nullptr, testSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT_NE(prvPtr, MAP_FAILED) << "Failed to allocate using mmap memory using private anonymous";
    std::shared_ptr<void> scopedPrivAnonPtr(prvPtr,
                                            [size = testSize](void *p) { munmap(p, size); });

    auto scopedSystemMemory2nd = UmdTest::importSystemMemory(prvPtr, testSize);
    ASSERT_NE(scopedSystemMemory2nd.get(), nullptr)
        << "Failed to import memory mmap private anonymous";

    // 2nd copy from private mmap to host alloc
    ptrSrc = static_cast<uint8_t *>(prvPtr);
    ptrDst = static_cast<uint8_t *>(scopedHostAlloc.get());
    executeCopyCommand(ptrDst, ptrSrc, testSize);

    // 3rd copy from private mmap to shared mmap
    ptrSrc = static_cast<uint8_t *>(prvPtr);
    ptrDst = static_cast<uint8_t *>(shrPtr);
    executeCopyCommand(ptrDst, ptrSrc, testSize);
}

TEST_P(ImportSystemMemory, OpenFileMmapSharedThenExecuteCopy) {
    auto testSize = GetParam();

    int fd = open(testFileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    ASSERT_GE(fd, 0) << "Failed to open " << testFileName;
    std::shared_ptr<int> scopedFd(&fd, [fd](auto) { close(fd); });
    ASSERT_EQ(ftruncate(fd, testSize), 0) << "Failed to set file testSize";

    void *ptr = mmap(nullptr, testSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ASSERT_NE(ptr, MAP_FAILED) << "Failed to allocate memory";
    std::shared_ptr<void> scopedMmap(ptr, [size = testSize](void *p) { munmap(p, size); });

    // TODO: mmap with MAP_SHARED without read only flag fails to be pinned in kernel
    auto scopedSystemMemory = UmdTest::importSystemMemory(ptr, testSize, true);
    ASSERT_NE(scopedSystemMemory.get(), nullptr) << "Failed to import memory";

    auto scopedHostAlloc = zeScope::memAllocHost(zeContext, hostMemAllocDesc, testSize, 0, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    uint8_t *ptrSrc = static_cast<uint8_t *>(ptr);
    uint8_t *ptrDst = static_cast<uint8_t *>(scopedHostAlloc.get());
    executeCopyCommand(ptrDst, ptrSrc, testSize);

    unlink(testFileName);
}

TEST_P(ImportSystemMemory, OpenFileMmapPrivateFileThenExecuteCopy) {
    auto testSize = GetParam();

    int fd = open(testFileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    ASSERT_GE(fd, 0) << "Failed to open " << testFileName;
    std::shared_ptr<int> scopedFd(&fd, [fd](auto) { close(fd); });
    ASSERT_EQ(ftruncate(fd, testSize), 0) << "Failed to set file testSize";

    void *ptr = mmap(nullptr, testSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    ASSERT_NE(ptr, MAP_FAILED) << "Failed to allocate memory";
    std::shared_ptr<void> scopedMmap(ptr, [size = testSize](void *p) { munmap(p, size); });

    auto scopedSystemMemory = UmdTest::importSystemMemory(ptr, testSize);
    ASSERT_NE(scopedSystemMemory.get(), nullptr) << "Failed to import memory";

    auto scopedHostAlloc = zeScope::memAllocHost(zeContext, hostMemAllocDesc, testSize, 0, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    uint8_t *ptrSrc = static_cast<uint8_t *>(ptr);
    uint8_t *ptrDst = static_cast<uint8_t *>(scopedHostAlloc.get());
    executeCopyCommand(ptrDst, ptrSrc, testSize);
    executeCopyCommand(ptrSrc, ptrDst, testSize);

    unlink(testFileName);
}

TEST_P(ImportSystemMemory, OpenFileInReadOnlyThenExecuteCopy) {
    auto testSize = GetParam();

    int fd = open(testFileName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    ASSERT_GE(fd, 0) << "Failed to open " << testFileName;
    std::shared_ptr<int> scopedFd(&fd, [fd](auto) { close(fd); });
    std::vector<uint8_t> initData(testSize, 0xAB);
    ASSERT_EQ(write(fd, initData.data(), testSize), static_cast<ssize_t>(testSize))
        << "Failed to write initial data to file";
    scopedFd.reset();

    fd = open(testFileName, O_RDONLY);
    ASSERT_GE(fd, 0) << "Failed to open " << testFileName << " in read only mode";
    scopedFd = std::shared_ptr<int>(&fd, [fd](auto) { close(fd); });

    struct stat sb = {};
    fstat(fd, &sb);
    ASSERT_EQ(static_cast<uint64_t>(sb.st_size), testSize) << "File size mismatch";

    void *rdonlyPtr = mmap(nullptr, testSize, PROT_READ, MAP_SHARED, fd, 0);
    ASSERT_NE(rdonlyPtr, MAP_FAILED) << "Failed to mmap private file in read only mode";
    std::shared_ptr<void> scopedMmap(rdonlyPtr, [size = testSize](void *p) { munmap(p, size); });

    auto scopedSystemMemory = UmdTest::importSystemMemory(rdonlyPtr, testSize, true);
    ASSERT_NE(scopedSystemMemory.get(), nullptr) << "Failed to import memory";

    auto scopedHostAlloc = zeScope::memAllocHost(zeContext, hostMemAllocDesc, testSize, 0, ret);
    ASSERT_EQ(ret, ZE_RESULT_SUCCESS);

    uint8_t *ptrSrc = static_cast<uint8_t *>(rdonlyPtr);
    uint8_t *ptrDst = static_cast<uint8_t *>(scopedHostAlloc.get());
    executeCopyCommand(ptrDst, ptrSrc, testSize, true);

    unlink(testFileName);
}

class CommandGraphImportSystemMemory : public ImportSystemMemoryNoParam,
                                       public ::testing::WithParamInterface<YAML::Node> {
  public:
    void SetUp() override {
        ImportSystemMemoryNoParam::SetUp();

        ze_command_queue_desc_t cmdQueueDesc = {};
        cmdQueueDesc.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC;
        scopedQueue = zeScope::commandQueueCreate(zeContext, zeDevice, cmdQueueDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        ze_command_list_desc_t cmdListDesc = {};
        cmdListDesc.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC;
        scopedList = zeScope::commandListCreate(zeContext, zeDevice, cmdListDesc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();

        const YAML::Node node = GetParam();

        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, node);
        ASSERT_NE(graph, nullptr) << "Unable to create Graph object";
    }

    ze_result_t ret = ZE_RESULT_SUCCESS;
    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;
    std::shared_ptr<Graph> graph;

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue = nullptr;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList = nullptr;
};

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(CommandGraphImportSystemMemory);

INSTANTIATE_TEST_SUITE_P(,
                         CommandGraphImportSystemMemory,
                         ::testing::ValuesIn(Environment::getConfiguration("graph_execution")),
                         [](const testing::TestParamInfo<YAML::Node> &p) {
                             return generateTestNameFromNode(p.param);
                         });

TEST_P(CommandGraphImportSystemMemory, GetNativeBinaryThenImportItAndRunInference) {
    auto nativeGraphBuffer = graph->getNativeBinary2AsNewBuffer();
    ASSERT_NE(nativeGraphBuffer, nullptr) << "Unable to get native binary from Graph";

    graph.reset();

    auto mem = UmdTest::importSystemMemory(nativeGraphBuffer->buffer.data(),
                                           nativeGraphBuffer->desc.inputSize,
                                           true);
    ASSERT_NE(mem.get(), nullptr) << "Unable to allocate host memory for native graph binary";

    nativeGraphBuffer->desc.flags |= ZE_GRAPH_FLAG_INPUT_GRAPH_PERSISTENT;
    graph = Graph::create(zeContext,
                          zeDevice,
                          zeGraphDDITableExt,
                          globalConfig,
                          GetParam(),
                          std::move(nativeGraphBuffer));
    ASSERT_NE(graph, nullptr) << "Unable to create new Graph object from native binary";

    ze_graph_properties_3_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;
    ASSERT_EQ(graph->getGraphProperties(&graphProps), ZE_RESULT_SUCCESS);
    ASSERT_EQ(graphProps.flags & ZE_GRAPH_PROPERTIES_FLAG_NO_STANDARD_ALLOCATION, 0);

    graph->allocateArguments(MemType::SHARED_MEMORY);
    graph->copyInputData();

    ASSERT_EQ(zeGraphDDITableExt->pfnGraphInitialize(graph->handle), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();
}

TEST_P(CommandGraphImportSystemMemory, WriteNativeBinaryToFileThenImportItAndRunInference) {
    const char *testFileName = "test_native_graph.blob";
    auto size = 0lu;
    ASSERT_EQ(graph->writeNativeBinaryToFile(testFileName, size), 0)
        << "Unable to get native binary from Graph";
    graph.reset();

    int fd = open(testFileName, O_RDONLY);
    ASSERT_GE(fd, 0) << "Unable to open native graph binary file";
    std::shared_ptr<int> scopedFile(&fd, [fd](auto) { close(fd); });

    auto nativeGraphPtr = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
    ASSERT_NE(nativeGraphPtr, MAP_FAILED) << "Unable to mmap native graph binary file";
    std::shared_ptr<void> scopedPtr(nativeGraphPtr, [size](void *p) { munmap(p, size); });

    auto mem = UmdTest::importSystemMemory(nativeGraphPtr, size, true);
    ASSERT_NE(mem.get(), nullptr) << "Unable to allocate host memory for native graph binary";

    auto nativeGraphBuffer = std::make_shared<GraphBuffer>(nativeGraphPtr, size);
    nativeGraphBuffer->desc.flags |= ZE_GRAPH_FLAG_INPUT_GRAPH_PERSISTENT;
    graph = Graph::create(zeContext,
                          zeDevice,
                          zeGraphDDITableExt,
                          globalConfig,
                          GetParam(),
                          std::move(nativeGraphBuffer));
    ASSERT_NE(graph, nullptr) << "Unable to create new Graph object from native binary";

    ze_graph_properties_3_t graphProps = {};
    graphProps.stype = ZE_STRUCTURE_TYPE_GRAPH_PROPERTIES;
    ASSERT_EQ(graph->getGraphProperties(&graphProps), ZE_RESULT_SUCCESS);
    ASSERT_EQ(graphProps.flags & ZE_GRAPH_PROPERTIES_FLAG_NO_STANDARD_ALLOCATION, 0);

    graph->allocateArguments(MemType::SHARED_MEMORY);
    graph->copyInputData();

    ASSERT_EQ(zeGraphDDITableExt->pfnGraphInitialize(graph->handle), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeGraphDDITableExt
                  ->pfnAppendGraphExecute(list, graph->handle, nullptr, nullptr, 0, nullptr),
              ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr), ZE_RESULT_SUCCESS);
    ASSERT_EQ(zeCommandQueueSynchronize(queue, graphSyncTimeout), ZE_RESULT_SUCCESS);

    graph->checkResults();

    unlink(testFileName);
}
