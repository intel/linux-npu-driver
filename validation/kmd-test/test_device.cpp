/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "drm_helpers.h"
#include "file_helpers.h"
#include "kmd_test.h"

#include <grp.h>
#include <string.h>
#include <thread>

class Device : public KmdTest {
  public:
    void Heartbeat(int thread_id);
    void ResetEngine();
};

void Device::ResetEngine() {
    SKIP_NO_DEBUGFS("reset_engine");
    SKIP_NO_DEBUGFS("resume_engine");

    ASSERT_EQ(write_debugfs_file("reset_engine", ENGINE_COMPUTE), 0);
    if (is_hws_enabled()) {
        ASSERT_EQ(write_debugfs_file("resume_engine", ENGINE_COMPUTE), 0);
    }

    ASSERT_TRUE(wait_for_suspend());
}

TEST_F(Device, Open) {
    static const int COUNT = 128;
    int fds[COUNT] = {};
    int i;

    for (i = 0; i < COUNT; i++) {
        fds[i] = open();
        EXPECT_GE(fds[i], 0) << "open failed at " << i << std::endl;
        if (fds[i] < 0)
            break;
    }
    for (int j = i - 1; j >= 0; --j) {
        close(fds[j]);
    }
}

static bool isChromeOs() {
    std::string os_release;

    read_file("/etc/os-release", os_release);
    return os_release.find("chromiumos") != std::string::npos;
}

TEST_F(Device, GroupOwnership) {
    struct stat fileInfo;
    std::string expectedGroupName;
    if (isChromeOs()) {
        expectedGroupName = "ml-core";
    } else {
        expectedGroupName = "render";
    }
#ifdef ANDROID
    expectedGroupName = "system";
#endif
    ASSERT_EQ(0, fstat(context.get_fd(), &fileInfo));
    struct group *fileGroup = getgrgid(fileInfo.st_gid);
    ASSERT_NE(nullptr, fileGroup);
    ASSERT_TRUE(expectedGroupName == fileGroup->gr_name)
        << "Group name should be " << expectedGroupName << " but is " << fileGroup->gr_name;
}

TEST_F(Device, FwApiVersion) {
    vpu_fw_api_version apiver;

    apiver = get_fw_api_version(VPU_JSM_JOB_CMD_API_VER_INDEX);

    TRACE("JSM JOB CMD API VERSION, FW: %d.%d, UMD: %d.%d\n",
          apiver.major,
          apiver.minor,
          VPU_JSM_JOB_CMD_API_VER_MAJOR,
          VPU_JSM_JOB_CMD_API_VER_MINOR);

    // Only enforce major version
    // Minor version may be different between UMD and FW
    EXPECT_EQ(apiver.major, VPU_JSM_JOB_CMD_API_VER_MAJOR);
}

TEST_F(Device, FwApiVersionList) {
    vpu_fw_api_version_value apiver;
    int api_num = 0;

    apiver.raw = 0;
    for (int i = 0; i < 16; i++) {
        apiver.data = get_fw_api_version(i);
        if (apiver.raw) {
            TRACE("API[%d]= %d.%d\n", i, apiver.data.major, apiver.data.minor);
            api_num++;
        }
    }

    ASSERT_GE(api_num, 4);
}

TEST_F(Device, GetDeviceParams) {
    uint64_t value;

    EXPECT_EQ(get_param(DRM_IVPU_PARAM_DEVICE_ID, &value), 0);
    EXPECT_TRUE(test_app::is_vpu(value));

    EXPECT_EQ(get_param(DRM_IVPU_PARAM_PLATFORM_TYPE, &value), 0);
    EXPECT_LT(value, 8); // see ivpu_drv.h

    EXPECT_EQ(get_param(DRM_IVPU_PARAM_NUM_CONTEXTS, &value), 0);
    EXPECT_GT(value, 0);

    EXPECT_EQ(get_param(DRM_IVPU_PARAM_DEVICE_REVISION, &value), 0);
    EXPECT_EQ(get_param(DRM_IVPU_PARAM_CORE_CLOCK_RATE, &value), 0);
    EXPECT_EQ(get_param(DRM_IVPU_PARAM_CONTEXT_BASE_ADDRESS, &value), 0);
    EXPECT_EQ(get_param(DRM_IVPU_PARAM_CONTEXT_ID, &value), 0);
    EXPECT_EQ(get_param(DRM_IVPU_PARAM_FW_API_VERSION, &value), 0);
}

TEST_F(Device, ResetEngine) {
    ResetEngine();
}

TEST_F(Device, ResetEngineAfterCmd) {
    SendCheckTimestamp();
    ResetEngine();
}

TEST_F(Device, ResetEngineAfterHB) {
    uint64_t hb;

    ASSERT_EQ(get_param(DRM_IVPU_PARAM_ENGINE_HEARTBEAT, &hb, ENGINE_COMPUTE), 0);
    ResetEngine();
}

TEST_F(Device, Suspend) {
    ASSERT_TRUE(wait_for_suspend());
}

TEST_F(Device, ResetInvalidEngine) {
    SKIP_PATCHSET();
    SKIP_NO_DEBUGFS("reset_engine");
    SKIP_NO_DEBUGFS("resume_engine");

    EXPECT_EQ(write_debugfs_file("reset_engine", 3), EINVAL);
    EXPECT_EQ(write_debugfs_file("resume_engine", 3), EINVAL);
}

TEST_F(Device, Heartbeat) {
    uint64_t hb;

    ASSERT_EQ(get_param(DRM_IVPU_PARAM_ENGINE_HEARTBEAT, &hb, ENGINE_COMPUTE), 0);
    TRACE_P64(hb);
}

void Device::Heartbeat(int thread_id) {
    PerfCounter counter;
    uint64_t hb;
    int engine = ENGINE_COMPUTE;
    int ret;

    // First get_param() can be slow as it may nee to wait for VPU boot
    EXPECT_EQ(ret = get_param(DRM_IVPU_PARAM_ENGINE_HEARTBEAT, &hb, engine), 0);
    if (ret)
        return;

    counter.setTimeout(is_silicon() ? 100 : 10);
    counter.start();
    do {
        EXPECT_EQ(ret = get_param(DRM_IVPU_PARAM_ENGINE_HEARTBEAT, &hb, engine), 0);
        if (ret)
            break;
        counter.countFrame();
    } while (!counter.isTimedOut());
    counter.stop();
}

TEST_F(Device, HeartbeatPerf) {
    Heartbeat(0);
}

TEST_F(Device, HeartbeatPerf4Threads) {
    std::vector<std::unique_ptr<test_app::thread>> threads;
    static const int THREAD_COUNT = 4;
    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.push_back(std::make_unique<test_app::thread>(&Device::Heartbeat, this, i));
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        threads[i]->join();
    }
}

TEST_F(Device, Utilization) {
    SKIP_NO_SYSFS("npu_busy_time_us");

    const std::string busy_time_file = "npu_busy_time_us";
    const size_t buf_size = 4096 * 2;
    int64_t sample_1 = -1, sample_2 = -1;
    std::chrono::steady_clock::time_point job_start, job_end;

    ASSERT_EQ(read_sysfs_file(busy_time_file, sample_1), 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ASSERT_EQ(read_sysfs_file(busy_time_file, sample_2), 0);
    ASSERT_EQ(sample_2, sample_1);

    MemoryBuffer ts_buf(context, buf_size, VPU_BUF_USAGE_TIMESTAMP_HEAP);
    ASSERT_EQ(ts_buf.create(), 0);

    CmdBuffer cmd_buf(context, buf_size, VPU_BUF_USAGE_BATCHBUFFER);
    ASSERT_EQ(cmd_buf.create(), 0);
    cmd_buf.add_ts_cmd(ts_buf, 0);

    job_start = std::chrono::steady_clock::now();
    ASSERT_EQ(cmd_buf.submit(ENGINE_COMPUTE), 0);
    EXPECT_EQ(cmd_buf.wait(), 0);
    job_end = std::chrono::steady_clock::now();

    ts_buf.destroy();
    cmd_buf.destroy();
    context.close();

    ASSERT_EQ(read_sysfs_file(busy_time_file, sample_2), 0);
    ASSERT_LT(std::chrono::microseconds(sample_2 - sample_1),
              std::chrono::duration_cast<std::chrono::microseconds>(job_end - job_start));
}

struct FdInfo {
    uint64_t drm_total_memory = 0;
    uint64_t drm_shared_memory = 0;
    uint64_t drm_resident_memory = 0;
    uint64_t drm_active_memory = 0;
    uint64_t drm_purgeable_memory = 0;

    void trace() const {
        TRACE("Parsed:\n");
        TRACE("  %s:\t%lu\n", "drm-total-memory", drm_total_memory);
        TRACE("  %s:\t%lu\n", "drm-shared-memory", drm_shared_memory);
        TRACE("  %s:\t%lu\n", "drm-resident-memory", drm_resident_memory);
        TRACE("  %s:\t%lu\n", "drm-active-memory", drm_active_memory);
        TRACE("  %s:\t%lu\n", "drm-purgeable-memory", drm_purgeable_memory);
        TRACE("\n");
    }
};

void parse_fdinfo(int fd, FdInfo &info) {
    std::string fdinfo_path = "/proc/" + std::to_string(getpid()) + "/fdinfo/" + std::to_string(fd);
    std::ifstream fdinfo_file(fdinfo_path);

    // Reset all values
    info = {};
    std::string line;

    TRACE("Source:\n");
    while (std::getline(fdinfo_file, line)) {
        auto parse_memory_line = [&](const std::string &prefix) -> uint64_t {
            TRACE("  %s\n", line.c_str());

            if (line.find(prefix) != std::string::npos) {
                size_t colon_pos = line.find(':');

                if (colon_pos != std::string::npos) {
                    std::string value_str = line.substr(colon_pos + 1);
                    value_str.erase(0, value_str.find_first_not_of(" \t"));

                    // Extract the integer part and detect suffix
                    std::stringstream ss(value_str);
                    uint64_t value;
                    ss >> value;

                    // Prevent overflow when converting MB to bytes,
                    // if value is too large, return max value
                    if (value > std::numeric_limits<decltype(value)>::max() / MB)
                        return std::numeric_limits<decltype(value)>::max();

                    // Check for suffix and apply appropriate multiplier
                    if (value_str.find("MiB") != std::string::npos) {
                        return value * MB;
                    } else if (value_str.find("KiB") != std::string::npos) {
                        return value * KB;
                    } else {
                        return value;
                    }
                }
            }
            return 0;
        };

        // Parse different memory types
        if (line.find("drm-total-memory:") != std::string::npos) {
            info.drm_total_memory = parse_memory_line("drm-total-memory:");
        } else if (line.find("drm-shared-memory:") != std::string::npos) {
            info.drm_shared_memory = parse_memory_line("drm-shared-memory:");
        } else if (line.find("drm-resident-memory:") != std::string::npos) {
            info.drm_resident_memory = parse_memory_line("drm-resident-memory:");
        } else if (line.find("drm-active-memory:") != std::string::npos) {
            info.drm_active_memory = parse_memory_line("drm-active-memory:");
        } else if (line.find("drm-purgeable-memory:") != std::string::npos) {
            info.drm_purgeable_memory = parse_memory_line("drm-purgeable-memory:");
        }
    }

    info.trace();
    fdinfo_file.close();
}

void parse_and_validate_fdinfo(int fd,
                               uint64_t expected_total,
                               uint64_t expected_shared,
                               uint64_t expected_resident,
                               uint64_t expected_active,
                               uint64_t expected_purgeable) {
    FdInfo info;

    parse_fdinfo(fd, info);
    ASSERT_EQ(info.drm_total_memory, expected_total);
    ASSERT_EQ(info.drm_shared_memory, expected_shared);
    ASSERT_EQ(info.drm_resident_memory, expected_resident);
    ASSERT_EQ(info.drm_active_memory, expected_active);
    ASSERT_EQ(info.drm_purgeable_memory, expected_purgeable);
}

TEST_F(Device, FdInfo) {
    SKIP_PATCHSET(); // Restore when fdinfo will be supported in patchset

    struct FdInfoExpected {
        uint64_t total_size = 0;
        uint64_t shared_size = 0;
        uint64_t resident_size = 0;
        uint64_t active_size = 0;
        uint64_t purgeable_size = 0;

        void validate(int fd) const {
            parse_and_validate_fdinfo(fd,
                                      total_size,
                                      shared_size,
                                      resident_size,
                                      active_size,
                                      purgeable_size);
        }
    };

    KmdContext ctx;
    auto fd = ctx.open();
    ASSERT_GT(fd, -1) << "open() failed with error " << errno << " - " << strerror(errno);

    FdInfoExpected expected;

    // Initial state - no buffers allocated
    expected.validate(fd);

    // Create non-mappable buffer - no resident memory expected
    MemoryBuffer buf1(ctx, 4096, VPU_BUF_USAGE_PREEMPT_LOW);
    ASSERT_EQ(buf1.create(), 0);
    expected.total_size += buf1.size();
    expected.validate(fd);

    // Create MB-aligned buffer to validate MB suffix parsing
    MemoryBuffer buf2(ctx, (2 * MB) - expected.total_size, VPU_BUF_USAGE_PREEMPT_LOW);
    ASSERT_EQ(buf2.create(), 0);
    expected.total_size += buf2.size();
    expected.validate(fd);

    // Add another non-mappable buffer
    MemoryBuffer buf3(ctx, 8192, VPU_BUF_USAGE_PREEMPT_LOW);
    ASSERT_EQ(buf3.create(), 0);
    expected.total_size += buf3.size();
    expected.validate(fd);

    // Add mappable buffer - increases resident memory
    MemoryBuffer buf4(ctx, 4096);
    ASSERT_EQ(buf4.create(), 0);
    expected.total_size += buf4.size();
    expected.resident_size += buf4.size();
    expected.validate(fd);

    // Add userptr buffer - no resident memory increase
    auto userptr = mmap(nullptr, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    ASSERT_NE(userptr, MAP_FAILED);
    MemoryBuffer user_buf(ctx, 4096, 0);
    ASSERT_EQ(user_buf.create_from_userptr(userptr), 0);
    expected.total_size += user_buf.size();
    expected.validate(fd);

    // Submit command buffer with fence wait - increases resident memory for all added buffers
    // except userptr, and active memory for command buffer
    CmdBuffer cmd_wait(ctx, 4096);
    ASSERT_EQ(cmd_wait.create(), 0);
    cmd_wait.add_fence_wait_cmd(buf1, 0);
    cmd_wait.add_handle(buf2);
    cmd_wait.add_handle(user_buf);
    ASSERT_EQ(cmd_wait.submit(), 0);

    expected.total_size += cmd_wait.size(); // other buffers are already counted
    expected.resident_size += cmd_wait.size() + buf1.size() + buf2.size(); // userptr not resident
    expected.active_size += cmd_wait.size(); // only submitted cmd buffer impacts active memory
    expected.validate(fd);

    // Signal fence to avoid TDR
    CmdBuffer cmd_signal(ctx, 4096);
    ASSERT_EQ(cmd_signal.create(), 0);
    cmd_signal.add_fence_signal_cmd(buf1, 0);
    ASSERT_EQ(cmd_signal.submit(), 0);
    ASSERT_EQ(cmd_signal.wait(), 0);
}
