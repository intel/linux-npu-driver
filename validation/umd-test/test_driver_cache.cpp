/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"
#include "umd_dma_heap_system.hpp"
#include "zex_driver.hpp"

#include <chrono>
#include <future>
#include <level_zero/ze_api.h>
#include <linux/limits.h>
#include <stdexcept>

class DriverCache : public UmdTest {
  public:
    void SetUp() override {
        UmdTest::SetUp();
        ASSERT_EQ(zeDriverGetExtensionFunctionAddress(zeDriver,
                                                      "zexDiskCacheSetSize",
                                                      reinterpret_cast<void **>(&diskCacheSetSize)),
                  ZE_RESULT_SUCCESS);

        ASSERT_EQ(zeDriverGetExtensionFunctionAddress(zeDriver,
                                                      "zexDiskCacheGetSize",
                                                      reinterpret_cast<void **>(&diskCacheGetSize)),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(
            zeDriverGetExtensionFunctionAddress(zeDriver,
                                                "zexDiskCacheGetDirectory",
                                                reinterpret_cast<void **>(&diskCacheGetDirectory)),
            ZE_RESULT_SUCCESS);

        size_t driverCacheDirLength = sizeof(driverCacheDirectory);
        ASSERT_EQ(diskCacheGetDirectory(driverCacheDirectory, &driverCacheDirLength), 0);

        ASSERT_EQ(diskCacheGetSize(&defaultCacheSize), 0);

        for (auto &modelNode : Environment::getConfiguration("driver_cache"))
            modelDataNodes.push_back(modelNode);
        if (modelDataNodes.empty()) {
            SKIP_("No models found in driver_cache section of configuration file");
        }
    }

    void clearCacheDirectory() {
        std::filesystem::path cacheDir(driverCacheDirectory);
        for (auto const &cacheFile : std::filesystem::directory_iterator{cacheDir}) {
            std::filesystem::remove(cacheFile);
        }
    }

    std::vector<std::string> getListOfCachedFiles() {
        std::filesystem::path cacheDir(driverCacheDirectory);
        std::vector<std::string> fileNames;
        for (auto const &cacheFile : std::filesystem::directory_iterator{cacheDir})
            fileNames.push_back(cacheFile.path().string());

        return fileNames;
    }

    size_t getUsedCacheSpace() {
        std::filesystem::path cacheDir(driverCacheDirectory);
        size_t usedSpace = 0;

        for (auto const &cacheFile : std::filesystem::directory_iterator{cacheDir})
            usedSpace += std::filesystem::file_size(cacheFile);
        return usedSpace;
    }

    std::vector<std::filesystem::file_time_type> getCacheFilesLastWriteTime() {
        std::filesystem::path cacheDir(driverCacheDirectory);
        std::vector<std::filesystem::file_time_type> filesWriteTime;
        for (auto const &cacheFile : std::filesystem::directory_iterator{cacheDir})
            filesWriteTime.push_back(std::filesystem::last_write_time(cacheFile));

        return filesWriteTime;
    }

    void compareWithGraphQueryMemoryCache(size_t expUsedSize, size_t maxExpSize) {
        ze_graph_memory_query_t query = {};
        EXPECT_EQ(zeGraphDDITableExt->pfnQueryContextMemory(zeContext,
                                                            ZE_GRAPH_QUERY_MEMORY_DRIVER_CACHE,
                                                            &query),
                  ZE_RESULT_SUCCESS);
        EXPECT_EQ(expUsedSize, query.allocated);
        if (maxExpSize != 0) {
            EXPECT_EQ(maxExpSize, query.total);
        }

        query = {};
        EXPECT_EQ(zeGraphDDITableExt->pfnQueryContextMemory(zeContext,
                                                            ZE_GRAPH_QUERY_MEMORY_PROGRAM_CACHE,
                                                            &query),
                  ZE_RESULT_SUCCESS);

        EXPECT_EQ(expUsedSize, query.allocated);
        if (maxExpSize != 0) {
            EXPECT_EQ(maxExpSize, query.total);
        }
    }

    std::shared_ptr<Graph> graph;
    char driverCacheDirectory[PATH_MAX];
    std::vector<YAML::Node> modelDataNodes;
    size_t defaultCacheSize = 0;

    decltype(zexDiskCacheSetSize) *diskCacheSetSize;
    decltype(zexDiskCacheGetSize) *diskCacheGetSize;
    decltype(zexDiskCacheGetDirectory) *diskCacheGetDirectory;
};

TEST_F(DriverCache, CheckIfCacheSetSizeChangeTotalSize) {
    size_t usedCacheSize = getUsedCacheSpace();
    compareWithGraphQueryMemoryCache(usedCacheSize, defaultCacheSize);

    size_t testCacheSize = 512 * MB;
    EXPECT_EQ(diskCacheSetSize(testCacheSize), ZE_RESULT_SUCCESS);
    compareWithGraphQueryMemoryCache(usedCacheSize, testCacheSize);

    EXPECT_EQ(diskCacheSetSize(defaultCacheSize), ZE_RESULT_SUCCESS);
    compareWithGraphQueryMemoryCache(usedCacheSize, defaultCacheSize);
}

TEST_F(DriverCache, CheckIfAllCompiledModelsCached) {
    clearCacheDirectory();
    ASSERT_EQ(getUsedCacheSpace(), 0);
    compareWithGraphQueryMemoryCache(0, defaultCacheSize);

    for (auto &modelNode : modelDataNodes) {
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graph, nullptr);
    }

    auto cachedBlobs = getListOfCachedFiles();
    ASSERT_EQ(cachedBlobs.size(), modelDataNodes.size());
    compareWithGraphQueryMemoryCache(getUsedCacheSpace(), defaultCacheSize);
}

TEST_F(DriverCache, CompileModelsTwiceAndCheckNumberCachedFiles) {
    clearCacheDirectory();
    ASSERT_EQ(getUsedCacheSpace(), 0);

    for (auto &modelNode : modelDataNodes) {
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graph, nullptr);
    }

    auto cachedBlobsFirstIteration = getListOfCachedFiles();
    for (auto &modelNode : modelDataNodes) {
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graph, nullptr);
    }

    auto cachedBlobsSecondIteration = getListOfCachedFiles();
    ASSERT_EQ(cachedBlobsFirstIteration, cachedBlobsSecondIteration);
}

TEST_F(DriverCache, CompileModelsTwiceAndCheckWriteTime) {
    clearCacheDirectory();
    ASSERT_EQ(getUsedCacheSpace(), 0);

    for (auto &modelNode : modelDataNodes) {
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graph, nullptr);
    }

    auto cachedBlobsTimeFirstIteration = getCacheFilesLastWriteTime();
    for (auto &modelNode : modelDataNodes) {
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graph, nullptr);
    }

    auto cachedBlobsTimeSecondIteration = getCacheFilesLastWriteTime();
    /* second compilation should not modify cache files */
    ASSERT_EQ(cachedBlobsTimeFirstIteration, cachedBlobsTimeSecondIteration);
}

TEST_F(DriverCache, CheckTooSmallCache) {
    const size_t testCacheSize = 0x100;

    clearCacheDirectory();
    ASSERT_EQ(getUsedCacheSpace(), 0);
    ASSERT_EQ(diskCacheSetSize(testCacheSize), 0);

    for (auto &modelNode : modelDataNodes) {
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        EXPECT_NE(graph, nullptr);
    }
    auto cachedBlobs = getListOfCachedFiles();
    EXPECT_EQ(cachedBlobs.size(), 0);

    EXPECT_EQ(diskCacheSetSize(defaultCacheSize), 0);
}

TEST_F(DriverCache, CheckWhenSpaceLessThanAllBlobs) {
    clearCacheDirectory();
    ASSERT_EQ(getUsedCacheSpace(), 0);

    for (auto &modelNode : modelDataNodes) {
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graph, nullptr);
    }
    auto cachedBlobs = getListOfCachedFiles();
    ASSERT_EQ(cachedBlobs.size(), modelDataNodes.size());

    size_t reduceUsedSpace = getUsedCacheSpace() - 1;

    clearCacheDirectory();
    EXPECT_EQ(getUsedCacheSpace(), 0);

    /* set cache size one byte less than reqired */
    EXPECT_EQ(diskCacheSetSize(reduceUsedSpace), 0);

    for (auto &modelNode : modelDataNodes) {
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        EXPECT_NE(graph, nullptr);
    }

    cachedBlobs = getListOfCachedFiles();
    /* epected that one blob will be skipped due space limitation */
    EXPECT_EQ(cachedBlobs.size() + 1, modelDataNodes.size());

    EXPECT_EQ(diskCacheSetSize(defaultCacheSize), 0);
}

TEST_F(DriverCache, CheckWhenSpaceEqualExpectedBlobsSize) {
    clearCacheDirectory();
    ASSERT_EQ(getUsedCacheSpace(), 0);

    for (auto &modelNode : modelDataNodes) {
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graph, nullptr);
    }
    auto cachedBlobs = getListOfCachedFiles();
    ASSERT_EQ(cachedBlobs.size(), modelDataNodes.size());

    size_t usedCacheSpace = getUsedCacheSpace();

    clearCacheDirectory();
    EXPECT_EQ(getUsedCacheSpace(), 0);

    /* set cache to value exactly required by all blobs */
    EXPECT_EQ(diskCacheSetSize(usedCacheSpace), 0);

    for (auto &modelNode : modelDataNodes) {
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        EXPECT_NE(graph, nullptr);
    }

    cachedBlobs = getListOfCachedFiles();
    /* expected that all blobs will be stored in cache */
    EXPECT_EQ(cachedBlobs.size(), modelDataNodes.size());
    EXPECT_EQ(diskCacheSetSize(defaultCacheSize), 0);
}
