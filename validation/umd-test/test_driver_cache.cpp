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

    bool addConfigOption(std::string &flags, std::string option, std::string value) {
        if (flags.find("--config") == std::string::npos)
            flags.append(" --config");
        if (flags.find(option) != std::string::npos)
            return false;
        flags += " " + option + "=" + "\"" + value + "\"";
        return true;
    }

    void graphInputHashTestCase(void (*graphCreate)(ze_context_handle_t context,
                                                    ze_device_handle_t device,
                                                    ze_graph_dditable_ext_t *ddi,
                                                    const std::shared_ptr<GraphBuffer> &graphBuffer,
                                                    const std::string &buildFlags,
                                                    uint64_t hash,
                                                    ze_graph_handle_t *graph));

    std::shared_ptr<Graph> graph;
    char driverCacheDirectory[PATH_MAX];
    std::vector<YAML::Node> modelDataNodes;
    size_t defaultCacheSize = 0;
    ze_graph_properties_flags_t graphPropsFlagCompileMask =
        ZE_GRAPH_PROPERTIES_FLAG_LOADED_FROM_CACHE | ZE_GRAPH_PROPERTIES_FLAG_COMPILED |
        ZE_GRAPH_PROPERTIES_FLAG_PRE_COMPILED;

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

    for (auto &modelNode : modelDataNodes) {
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graph, nullptr);

        ze_graph_properties_3_t graphProperties;
        ASSERT_EQ(graph->getGraphProperties(&graphProperties), ZE_RESULT_SUCCESS);
        ASSERT_EQ(graphProperties.flags & graphPropsFlagCompileMask,
                  ZE_GRAPH_PROPERTIES_FLAG_COMPILED);
    }

    auto cachedBlobsFirstIteration = getListOfCachedFiles();
    auto cachedBlobsTimeFirstIteration = getCacheFilesLastWriteTime();

    for (auto &modelNode : modelDataNodes) {
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graph, nullptr);

        ze_graph_properties_3_t graphProperties;
        ASSERT_EQ(graph->getGraphProperties(&graphProperties), ZE_RESULT_SUCCESS);
        ASSERT_EQ(graphProperties.flags & graphPropsFlagCompileMask,
                  ZE_GRAPH_PROPERTIES_FLAG_LOADED_FROM_CACHE);
    }

    /* second compilation should not modify cache files */
    auto cachedBlobsSecondIteration = getListOfCachedFiles();
    ASSERT_EQ(cachedBlobsFirstIteration, cachedBlobsSecondIteration);

    auto cachedBlobsTimeSecondIteration = getCacheFilesLastWriteTime();
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

TEST_F(DriverCache, CheckIfCorruptedCachedIsOverriden) {
    for (auto &modelNode : modelDataNodes) {
        clearCacheDirectory();

        /* generate cached file */
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graph, nullptr);

        auto cachedBlobs = getListOfCachedFiles();
        ASSERT_EQ(cachedBlobs.size(), 1);

        /* modify cached file */
        std::vector<char> cachedData;
        auto cachedFilePath = std::filesystem::path(driverCacheDirectory) / cachedBlobs[0];
        ASSERT_EQ(DataHandle::loadFile(cachedFilePath, cachedData), 0);

        /* Set random byte to random position multiple times to ensure file is corrupted */
        for (auto i = 0; i < 16; i++) {
            auto *data =
                cachedData.data() + DataHandle::getRandomNumber<uint64_t>(0, cachedData.size() - 1);
            *data = DataHandle::getRandomNumber<uint8_t>();
        }

        /* cached files has only read permission */
        std::filesystem::permissions(cachedFilePath,
                                     std::filesystem::perms::owner_write,
                                     std::filesystem::perm_options::add);
        ASSERT_EQ(DataHandle::writeFile(cachedFilePath, cachedData.data(), cachedData.size()), 0);
        std::filesystem::permissions(cachedFilePath,
                                     std::filesystem::perms::owner_write,
                                     std::filesystem::perm_options::remove);

        /* regenerate cached file by compiling a new model */
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graph, nullptr);

        ze_graph_properties_3_t graphProperties;
        ASSERT_EQ(graph->getGraphProperties(&graphProperties), ZE_RESULT_SUCCESS);
        ASSERT_EQ(graphProperties.flags & graphPropsFlagCompileMask,
                  ZE_GRAPH_PROPERTIES_FLAG_COMPILED);
        /* check if cached file has changed */
        std::vector<char> cachedDataAfter;
        ASSERT_EQ(DataHandle::loadFile(cachedFilePath, cachedDataAfter), 0);
        ASSERT_NE(memcmp(cachedData.data(), cachedDataAfter.data(), cachedDataAfter.size()), 0);

        /* check if driver load the blob from cache */
        graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graph, nullptr);

        ASSERT_EQ(graph->getGraphProperties(&graphProperties), ZE_RESULT_SUCCESS);
        ASSERT_EQ(graphProperties.flags & graphPropsFlagCompileMask,
                  ZE_GRAPH_PROPERTIES_FLAG_LOADED_FROM_CACHE);
    }
}

TEST_F(DriverCache, CheckCacheUsingMultipleThreads) {
    /* Compile at least one model in two threads simultaneously */
    const size_t numThreads = std::max(8lu, modelDataNodes.size() + 2);
    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    clearCacheDirectory();
    ASSERT_EQ(getUsedCacheSpace(), 0);

    auto compileModel = [&](size_t idx, bool cached) {
        auto &model = modelDataNodes.at(idx % modelDataNodes.size());

        auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, model);
        ASSERT_NE(graph, nullptr);

        ze_graph_properties_3_t graphProperties;
        ASSERT_EQ(graph->getGraphProperties(&graphProperties), ZE_RESULT_SUCCESS);
        ASSERT_TRUE(graphProperties.flags & graphPropsFlagCompileMask);
        if (cached) {
            ASSERT_TRUE(graphProperties.flags & ZE_GRAPH_PROPERTIES_FLAG_LOADED_FROM_CACHE);
        }
    };

    /* Expect to compile all models with or without caching it */
    for (size_t i = 0; i < numThreads; ++i)
        threads.emplace_back(compileModel, i, false);
    for (auto &t : threads)
        t.join();
    threads.clear();

    /* Expect that all models from previous compilation are cached */
    auto cachedBlobs = getListOfCachedFiles();
    ASSERT_EQ(cachedBlobs.size(), modelDataNodes.size());
    for (size_t i = 0; i < numThreads; ++i)
        threads.emplace_back(compileModel, i, true);
    for (auto &t : threads)
        t.join();

    cachedBlobs = getListOfCachedFiles();
    ASSERT_EQ(cachedBlobs.size(), modelDataNodes.size());
}

void DriverCache::graphInputHashTestCase(
    void (*graphCreate)(ze_context_handle_t context,
                        ze_device_handle_t device,
                        ze_graph_dditable_ext_t *ddi,
                        const std::shared_ptr<GraphBuffer> &graphBuffer,
                        const std::string &buildFlags,
                        uint64_t hash,
                        ze_graph_handle_t *graph)) {
    clearCacheDirectory();
    ASSERT_EQ(getUsedCacheSpace(), 0);

    std::vector<std::shared_ptr<GraphBuffer>> graphBuffers;
    for (size_t i = 0; i < modelDataNodes.size(); i++) {
        const auto &modelNode = modelDataNodes[i];
        auto graphBuffer = GraphBuffer::get(zeDevice, zeGraphDDITableExt, globalConfig, modelNode);
        ASSERT_NE(graphBuffer, nullptr);
        graphBuffers.push_back(graphBuffer);

        uint64_t hash = 0x112233 + i;
        ze_graph_handle_t graph;
        graphCreate(zeContext,
                    zeDevice,
                    zeGraphDDITableExt,
                    graphBuffer,
                    graphBuffer->buildFlags,
                    hash,
                    &graph);

        ze_graph_properties_3_t graphProperties = {};
        ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties3(graph, &graphProperties),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(graphProperties.flags & graphPropsFlagCompileMask,
                  ZE_GRAPH_PROPERTIES_FLAG_COMPILED);
    }

    auto cachedBlobsFirstIteration = getListOfCachedFiles();
    ASSERT_EQ(cachedBlobsFirstIteration.size(), modelDataNodes.size());
    auto cachedBlobsTimeFirstIteration = getCacheFilesLastWriteTime();

    // check if we have a cache hit
    for (size_t i = 0; i < graphBuffers.size(); i++) {
        const auto &graphBuffer = graphBuffers[i];

        uint64_t hash = 0x112233 + i;
        ze_graph_handle_t graph;
        graphCreate(zeContext,
                    zeDevice,
                    zeGraphDDITableExt,
                    graphBuffer,
                    graphBuffer->buildFlags,
                    hash,
                    &graph);

        ze_graph_properties_3_t graphProperties = {};
        ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties3(graph, &graphProperties),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(graphProperties.flags & graphPropsFlagCompileMask,
                  ZE_GRAPH_PROPERTIES_FLAG_LOADED_FROM_CACHE);
    }

    auto cachedBlobsSecondIteration = getListOfCachedFiles();
    ASSERT_EQ(cachedBlobsFirstIteration, cachedBlobsSecondIteration);

    auto cachedBlobsTimeSecondIteration = getCacheFilesLastWriteTime();
    ASSERT_EQ(cachedBlobsTimeFirstIteration, cachedBlobsTimeSecondIteration);

    // change model name and check if we still have a cache hit
    for (size_t i = 0; i < graphBuffers.size(); i++) {
        const auto &graphBuffer = graphBuffers[i];
        const char *pattern = "<net name=\"";
        char *netName = strstr(graphBuffer->buffer.data() + graphBuffer->xmlOffset, pattern);
        ASSERT_NE(netName, nullptr);
        netName += strlen(pattern);
        *netName = *netName == '0' ? '1' : '0';

        uint64_t hash = 0x112233 + i;
        ze_graph_handle_t graph;
        graphCreate(zeContext,
                    zeDevice,
                    zeGraphDDITableExt,
                    graphBuffer,
                    graphBuffer->buildFlags,
                    hash,
                    &graph);

        ze_graph_properties_3_t graphProperties = {};
        ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties3(graph, &graphProperties),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(graphProperties.flags & graphPropsFlagCompileMask,
                  ZE_GRAPH_PROPERTIES_FLAG_LOADED_FROM_CACHE);
    }

    auto cachedBlobsThirdIteration = getListOfCachedFiles();
    ASSERT_EQ(cachedBlobsSecondIteration, cachedBlobsThirdIteration);

    auto cachedBlobsTimeThirdIteration = getCacheFilesLastWriteTime();
    ASSERT_EQ(cachedBlobsTimeSecondIteration, cachedBlobsTimeThirdIteration);

    // change build flags and check if we have a cache miss
    for (size_t i = 0; i < graphBuffers.size(); i++) {
        const auto &graphBuffer = graphBuffers[i];

        ze_graph_handle_t graph;
        uint64_t hash = 0x112233 + i;
        std::string buildFlags = graphBuffer->buildFlags;
        ASSERT_TRUE(addConfigOption(buildFlags, "PERF_COUNT", "NO"));
        graphCreate(zeContext, zeDevice, zeGraphDDITableExt, graphBuffer, buildFlags, hash, &graph);

        ze_graph_properties_3_t graphProperties = {};
        ASSERT_EQ(zeGraphDDITableExt->pfnGetProperties3(graph, &graphProperties),
                  ZE_RESULT_SUCCESS);
        ASSERT_EQ(graphProperties.flags & graphPropsFlagCompileMask,
                  ZE_GRAPH_PROPERTIES_FLAG_COMPILED);
    }

    auto cachedBlobsFourthIteration = getListOfCachedFiles();
    ASSERT_NE(cachedBlobsFirstIteration, cachedBlobsFourthIteration);
    ASSERT_EQ(cachedBlobsFirstIteration.size() * 2, cachedBlobsFourthIteration.size());
}

static void graphCreateWithInputHash(ze_context_handle_t context,
                                     ze_device_handle_t device,
                                     ze_graph_dditable_ext_t *ddi,
                                     const std::shared_ptr<GraphBuffer> &graphBuffer,
                                     const std::string &buildFlags,
                                     uint64_t hash,
                                     ze_graph_handle_t *graph) {
    ze_graph_input_hash_t graphInputHash = {};
    graphInputHash.stype = ZE_STRUCTURE_TYPE_GRAPH_INPUT_HASH;
    graphInputHash.hash = hash;
    ze_graph_desc_t desc = {};
    desc.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES;
    desc.pNext = &graphInputHash;
    desc.format = ZE_GRAPH_FORMAT_NGRAPH_LITE;
    desc.inputSize = graphBuffer->buffer.size();
    desc.pInput = reinterpret_cast<uint8_t *>(graphBuffer->buffer.data());
    desc.pBuildFlags = buildFlags.c_str();
    ASSERT_EQ(ddi->pfnCreate(context, device, &desc, graph), ZE_RESULT_SUCCESS);
}

TEST_F(DriverCache, GraphInputHashWithGraphCreate) {
    graphInputHashTestCase(graphCreateWithInputHash);
}

static void graphCreate2WithInputHash(ze_context_handle_t context,
                                      ze_device_handle_t device,
                                      ze_graph_dditable_ext_t *ddi,
                                      const std::shared_ptr<GraphBuffer> &graphBuffer,
                                      const std::string &buildFlags,
                                      uint64_t hash,
                                      ze_graph_handle_t *graph) {
    ze_graph_input_hash_t graphInputHash = {};
    graphInputHash.stype = ZE_STRUCTURE_TYPE_GRAPH_INPUT_HASH;
    graphInputHash.hash = hash;
    ze_graph_desc_2_t desc = {};
    desc.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES;
    desc.pNext = &graphInputHash;
    desc.format = ZE_GRAPH_FORMAT_NGRAPH_LITE;
    desc.inputSize = graphBuffer->buffer.size();
    desc.pInput = reinterpret_cast<uint8_t *>(graphBuffer->buffer.data());
    desc.pBuildFlags = buildFlags.c_str();
    ASSERT_EQ(ddi->pfnCreate2(context, device, &desc, graph), ZE_RESULT_SUCCESS);
}

TEST_F(DriverCache, GraphInputHashWithGraphCreate2) {
    graphInputHashTestCase(graphCreate2WithInputHash);
}

static void graphCreate3WithInputHash(ze_context_handle_t context,
                                      ze_device_handle_t device,
                                      ze_graph_dditable_ext_t *ddi,
                                      const std::shared_ptr<GraphBuffer> &graphBuffer,
                                      const std::string &buildFlags,
                                      uint64_t hash,
                                      ze_graph_handle_t *graph) {
    ze_graph_input_hash_t graphInputHash = {};
    graphInputHash.stype = ZE_STRUCTURE_TYPE_GRAPH_INPUT_HASH;
    graphInputHash.hash = hash;
    ze_graph_desc_2_t desc = {};
    desc.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES;
    desc.pNext = &graphInputHash;
    desc.format = ZE_GRAPH_FORMAT_NGRAPH_LITE;
    desc.inputSize = graphBuffer->buffer.size();
    desc.pInput = reinterpret_cast<uint8_t *>(graphBuffer->buffer.data());
    desc.pBuildFlags = buildFlags.c_str();
    ASSERT_EQ(ddi->pfnCreate3(context, device, &desc, graph, nullptr), ZE_RESULT_SUCCESS);
}

TEST_F(DriverCache, GraphInputHashWithGraphCreate3) {
    graphInputHashTestCase(graphCreate3WithInputHash);
}

class CompilationLog : public DriverCache {
  public:
    void SetUp() override { DriverCache::SetUp(); }

    ze_result_t getBuildLog(ze_graph_build_log_handle_t buildLogHandle, std::vector<char> &log) {
        uint32_t logSize;
        ze_result_t ret;
        ret = zeGraphDDITableExt->pfnBuildLogGetString2(buildLogHandle, &logSize, nullptr);
        if (ret != ZE_RESULT_SUCCESS)
            return ret;

        log.resize(logSize);
        ret = zeGraphDDITableExt->pfnBuildLogGetString2(buildLogHandle, &logSize, log.data());

        return ret;
    }

    ze_result_t getErrorLog(std::vector<char> &log) {
        uint32_t logSize = 0;
        ze_result_t ret = zeGraphDDITableExt->pfnBuildLogGetString(nullptr, &logSize, nullptr);
        if (ret != ZE_RESULT_SUCCESS)
            return ret;

        log.resize(logSize);
        ret = zeGraphDDITableExt->pfnBuildLogGetString(nullptr, &logSize, log.data());

        return ret;
    }

    ze_result_t destroyBuildLog(ze_graph_build_log_handle_t buildLogHandle) {
        return zeGraphDDITableExt->pfnBuildLogDestroy(buildLogHandle);
    }
};

TEST_F(CompilationLog, CompileModelsAndCheckLog) {
    clearCacheDirectory();
    ASSERT_EQ(getUsedCacheSpace(), 0);
    std::vector<ze_graph_build_log_handle_t> logHandles;

    for (auto &modelNode : modelDataNodes) {
        ze_graph_build_log_handle_t graphBuildLogHandle = nullptr;
        std::string buildFlags;
        std::filesystem::path modelPath;

        buildFlags = modelNode["flags"].as<std::string>("");
        modelPath = modelNode["path"].as<std::string>();
        addConfigOption(buildFlags, "LOG_LEVEL", "LOG_WARNING");

        if (modelPath.extension() == ".xml") {
            modelPath = globalConfig.modelDir / modelPath;
        } else {
            modelPath = globalConfig.blobDir / modelPath;
        }

        graph = Graph::create(zeContext,
                              zeDevice,
                              zeGraphDDITableExt,
                              modelPath,
                              buildFlags,
                              &graphBuildLogHandle);

        ASSERT_NE(graphBuildLogHandle, nullptr);
        logHandles.push_back(graphBuildLogHandle);
        ASSERT_NE(graph, nullptr);
        ze_graph_properties_3_t graphProperties;
        ASSERT_EQ(graph->getGraphProperties(&graphProperties), ZE_RESULT_SUCCESS);
        ASSERT_EQ(graphProperties.flags & graphPropsFlagCompileMask,
                  ZE_GRAPH_PROPERTIES_FLAG_COMPILED);
    }
    /* optionally if set of handles size is greater than 3 check removing single handle from the
     * middle*/
    if (logHandles.size() > 3) {
        auto removedHandle = logHandles[2];
        logHandles.erase(logHandles.begin() + 2);
        std::vector<char> log;
        EXPECT_EQ(getBuildLog(removedHandle, log), ZE_RESULT_SUCCESS);
        EXPECT_NE(log.size(), 0);
        EXPECT_EQ(destroyBuildLog(removedHandle), ZE_RESULT_SUCCESS);
    }

    for (auto const &logHandle : logHandles) {
        std::vector<char> log;
        EXPECT_EQ(getBuildLog(logHandle, log), ZE_RESULT_SUCCESS);
        EXPECT_NE(log.size(), 0);
        TRACE("Log size: %lu\n%s \n", log.size(), log.data());
        EXPECT_EQ(destroyBuildLog(logHandle), ZE_RESULT_SUCCESS);
    }
}

TEST_F(CompilationLog, ForceCompilationErrorAndCheckLogs) {
    clearCacheDirectory();
    ASSERT_EQ(getUsedCacheSpace(), 0);

    if (modelDataNodes.empty())
        return;

    auto &modelNode = modelDataNodes[0];
    ze_graph_build_log_handle_t graphBuildLogHandle = nullptr;
    std::string buildFlags;
    std::filesystem::path modelPath;

    buildFlags = modelNode["flags"].as<std::string>("");
    modelPath = modelNode["path"].as<std::string>();
    addConfigOption(buildFlags, "OPTION", "UNSUPPORTED");

    if (modelPath.extension() == ".xml") {
        modelPath = globalConfig.modelDir / modelPath;
    } else {
        modelPath = globalConfig.blobDir / modelPath;
    }

    graph = Graph::create(zeContext,
                          zeDevice,
                          zeGraphDDITableExt,
                          modelPath,
                          buildFlags,
                          &graphBuildLogHandle);

    ASSERT_NE(graphBuildLogHandle, nullptr);
    EXPECT_EQ(graph, nullptr);

    std::vector<char> logA, logB;
    EXPECT_EQ(getBuildLog(graphBuildLogHandle, logA), ZE_RESULT_SUCCESS);
    EXPECT_GT(logA.size(), 0);
    TRACE("Log from pfnBuildLogGetString2 size: %lu\n%s \n", logA.size(), logA.data());
    EXPECT_EQ(getErrorLog(logB), ZE_RESULT_SUCCESS);
    EXPECT_GT(logB.size(), 0);
    TRACE("Log from >pfnBuildLogGetString size: %lu\n%s \n", logB.size(), logB.data());
    EXPECT_TRUE(logA == logB);
    EXPECT_EQ(destroyBuildLog(graphBuildLogHandle), ZE_RESULT_SUCCESS);
}

TEST_F(CompilationLog, CheckIfModelIsCachedUsingBuildLog) {
    clearCacheDirectory();
    ASSERT_EQ(getUsedCacheSpace(), 0);

    if (modelDataNodes.empty())
        return;

    auto &model = modelDataNodes[0];
    std::vector<char> log;

    auto graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, model);
    ASSERT_EQ(getErrorLog(log), ZE_RESULT_SUCCESS);
    /* Compare without \0 sign */
    ASSERT_EQ(std::string(log.data(), log.size() - 1),
              "ZE DynamicCaching cache_status_t: cache_status_t::stored\n");

    auto cachedBlobs = getListOfCachedFiles();
    ASSERT_EQ(cachedBlobs.size(), 1);

    graph = Graph::create(zeContext, zeDevice, zeGraphDDITableExt, globalConfig, model);
    ASSERT_EQ(getErrorLog(log), ZE_RESULT_SUCCESS);
    /* Compare without \0 sign */
    ASSERT_EQ(std::string(log.data(), log.size() - 1),
              "ZE DynamicCaching cache_status_t: cache_status_t::found\n");

    cachedBlobs = getListOfCachedFiles();
    ASSERT_EQ(cachedBlobs.size(), 1);
}
