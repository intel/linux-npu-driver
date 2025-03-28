/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "blob_params.hpp"
#include "drm_helpers.h"
#include "model_params.hpp"
#include "test_app.h"
#include "testenv.hpp"
#include "umd_extensions.h"
#include "ze_memory.hpp"
#include "ze_scope.hpp"

#include <filesystem>
#include <linux/kernel.h>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <yaml-cpp/yaml.h>

// Custom printer to dump ze_result_t as hex string
void PrintTo(const ze_result_t &result, std::ostream *os);

#define SKIP_(msg)                      \
    if (!test_app::run_skipped_tests) { \
        GTEST_SKIP_(msg);               \
    }

#define SKIP_VPU37XX(msg) \
    if (isVPU37xx()) {    \
        SKIP_(msg);       \
    }

#define SKIP_VPU40XX(msg) \
    if (isVPU40xx()) {    \
        SKIP_(msg);       \
    }

#define SKIP_NO_HWS(msg)       \
    if (!isHwsModeEnabled()) { \
        SKIP_(msg);            \
    }

#define SKIP_NEEDS_ROOT()               \
    if (!test_app::has_root_access()) { \
        SKIP_("Needs root privileges")  \
    }

#define SKIP_CHROMEOS()                                    \
    if (std::filesystem::exists("/etc/chrome_dev.conf")) { \
        SKIP_("Test is not supported in ChromeOS")         \
    }

#define SKIP_NEEDS_SYSFS_FILE(x)                                          \
    if (!isFileAvailableInSysFs(x)) {                                     \
        SKIP_("Test is not supported because " x " is missing in SysFs"); \
    }

#define SKIP_HARDENING(msg)                \
    if (test_app::is_hardening_kernel()) { \
        SKIP_(msg);                        \
    }

#define KB (1024llu)
#define MB (1024llu * 1024)

#define ALIGN_TO_PAGE(x) __ALIGN_KERNEL((x), UmdTest::pageSize)

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

inline std::string getDeviceSysFsDirectory() {
    drm_device_desc desc = drm::open_intel_vpu();
    close(desc.fd);
    std::string path = "/sys/dev/char/" + std::to_string(desc.major_id) + ":" +
                       std::to_string(desc.minor_id) + "/device";
    return path;
}

inline bool isFileAvailableInSysFs(const std::string &filename) {
    std::filesystem::path deviceSysFs = getDeviceSysFsDirectory();
    if (deviceSysFs.empty()) {
        TRACE("WARNING: No SysFs available in system\n");
        return false;
    }

    std::error_code ec;
    return std::filesystem::exists(deviceSysFs / filename, ec);
}

class UmdTest : public ::testing::Test {
  public:
    /**
     * @brief Write data out to a file
     * @param filePath[in]: Path to the file to written.
     * @param dataIn[in]: Reference to vector to obtain data for storing.
     * @param inputSize[in]: size of data ro be written from void pointer
     * @return : int 0 indicating success, -1 if otherwise
     */
    static int saveFile(const std::string &filePath, void *dataIn, size_t inputSize);

    static constexpr int pageSize = 4096;

    uint32_t computeGrpOrdinalGpu = std::numeric_limits<uint32_t>::max();
    uint32_t copyGrpOrdinalGpu = std::numeric_limits<uint32_t>::max();

    struct GlobalConfig {
        std::string blobDir = "";
        std::string imageDir = "";
        std::string modelDir = "";
    } globalConfig;

  protected:
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<void> AllocSharedMemory(size_t size, ze_host_mem_alloc_flags_t flagsHost = 0);
    std::shared_ptr<void> AllocDeviceMemory(size_t size);
    std::shared_ptr<void> AllocHostMemory(size_t size, ze_host_mem_alloc_flags_t flagsHost = 0);

    bool isSilicon();
    bool isHwsModeEnabled();
    bool isVPU37xx() { return test_app::is_vpu37xx(pciDevId); }
    bool isVPU40xx() { return test_app::is_vpu40xx(pciDevId); }

    ze_driver_handle_t zeDriver = nullptr;
    ze_device_handle_t zeDevice = nullptr;
    ze_context_handle_t zeContext = nullptr;
    graph_dditable_ext_t *zeGraphDDITableExt = nullptr;
    ze_graph_profiling_dditable_ext_t *zeGraphProfilingDDITableExt = nullptr;
    command_queue_dditable_t *zeCommandQueueDDITableExt = nullptr;
    uint64_t maxMemAllocSize = 0;

    ze_driver_handle_t zeDriverGpu = nullptr;
    ze_device_handle_t zeDeviceGpu = nullptr;
    ze_context_handle_t zeContextGpu = nullptr;

    void CommandQueueGroupSetUpNpu(ze_device_handle_t dev);
    void
    CommandQueueGroupSetUpGpu(ze_device_handle_t dev, uint32_t &compOrdinal, uint32_t &copyOrdinal);

    uint16_t pciDevId = 0u;
    uint32_t platformType = 0u;

    uint64_t syncTimeout = 0ULL;
    uint64_t graphSyncTimeout = 0ULL;

  private:
    zeScope::SharedPtr<ze_context_handle_t> scopedContext;
    zeScope::SharedPtr<ze_context_handle_t> scopedContextGpu;
};
