/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "blob_params.hpp"
#include "model_params.hpp"
#include "test_app.h"
#include "umd_extensions.h"
#include "ze_scope.hpp"
#include "testenv.hpp"

#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <thread>

// Custom printer to dump ze_result_t as hex string
void PrintTo(const ze_result_t &ze_result, std::ostream *os);

#define SKIP_(msg)                      \
    if (!test_app::run_skipped_tests) { \
        GTEST_SKIP_(msg);               \
    }

#define KB (1024llu)
#define MB (1024llu * 1024)

inline std::string memSizeToStr(uint64_t size) {
    std::string str;
    if (size >= MB)
        return std::to_string(size / MB) + "MB";
    if (size >= KB)
        return std::to_string(size / KB) + "KB";
    return std::to_string(size) + "B";
}

inline std::string replacePathSigns(std::string name) {
    std::replace(name.begin(), name.end(), '.', '_');
    std::replace(name.begin(), name.end(), ' ', '_');
    std::replace(name.begin(), name.end(), '-', '_');
    return name;
}

inline std::string generateTestNameFromNode(const YAML::Node &node) {
    std::string testName("Unknown_test_case_name");

    try {
        if (node["name"].IsDefined())
            testName = node["name"].as<std::string>();
        else if (node["path"].IsDefined()) {
            std::filesystem::path testPath = node["path"].as<std::string>();
            testName = testPath.stem();
        }
        testName = replacePathSigns(testName);
    } catch (YAML::Exception &e) {
        std::cerr << "Error in configuration: " << node << std::endl;
    }
    return testName;
}

class UmdTest : public ::testing::Test {
  public:
    /**
     * @brief Return string representing device type
     * @param devType[in]: ZE device type
     * @return : The name of the ZE type or "Unknown"
     */
    static const char *zeDevTypeStr(ze_device_type_t devType);

    /**
     * @brief Load a binary file into the program
     * @param filePath[in]: Path to the file to be loaded.
     * @param dataOut[out]: Reference to vector to store raw data.
     * @return : true indicating success, false if otherwise
     */
    static bool loadFile(const std::string &filePath, std::vector<char> &dataOut);

    /**
     * @brief Write data out to a file
     * @param filePath[in]: Path to the file to written.
     * @param dataIn[in]: Reference to vector to obtain data for storing.
     * @param inputSize[in]: size of data ro be written from void pointer
     * @return : int 0 indicating success, -1 if otherwise
     */
    static int saveFile(const std::string &filePath, void *dataIn, size_t inputSize);

    static constexpr int PAGE_SIZE = 4096;

  protected:
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<void> AllocSharedMemory(size_t size, ze_host_mem_alloc_flags_t flagsHost = 0);
    std::shared_ptr<void> AllocDeviceMemory(size_t size);
    std::shared_ptr<void> AllocHostMemory(size_t size, ze_host_mem_alloc_flags_t flagsHost = 0);

    bool isSilicon();

    /** @brief Handle to the Level Zero API driver object */
    ze_driver_handle_t zeDriver = nullptr;
    /** @brief Handle to the Level Zero API device object */
    ze_device_handle_t zeDevice = nullptr;
    /** @brief Handle to the Level Zero API context object */
    ze_context_handle_t zeContext = nullptr;
    /** @brief Pointer to the Level Zero API graph extension DDI table */
    graph_dditable_ext_t *zeGraphDDITableExt = nullptr;
    /** @brief Pointer to the Level Zero API graph extension profiling DDI table */
    ze_graph_profiling_dditable_ext_t *zeGraphProfilingDDITableExt = nullptr;
    uint64_t maxMemAllocSize = 0;

    /** @brief Retrieve command group ordinals for compute and copy engine usage */
    void CommandQueueGroupSetUp();
    uint32_t computeGrpOrdinal = std::numeric_limits<uint32_t>::max();
    uint32_t copyGrpOrdinal = std::numeric_limits<uint32_t>::max();

    uint16_t pciDevId = 0u;
    uint32_t platformType = 0u;

    uint64_t syncTimeout = 2'000'000'000;    // 2 seconds
    uint64_t graphSyncTimeout = syncTimeout; // 2 seconds

  private:
    zeScope::SharedPtr<ze_context_handle_t> scopedContext;
};
