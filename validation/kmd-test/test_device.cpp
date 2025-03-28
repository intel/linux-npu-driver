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
};

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

TEST_F(Device, ResetComputeEngine) {
    bool hws = is_hws_enabled();
    SKIP_NO_DEBUGFS("reset_engine");

    if (hws) {
        SKIP_NO_DEBUGFS("resume_engine");
    }

    ASSERT_EQ(write_debugfs_file("reset_engine", ENGINE_COMPUTE), 0);

    if (hws) {
        ASSERT_EQ(write_debugfs_file("resume_engine", ENGINE_COMPUTE), 0);
    }
}

// Failed with patchset KMD. Requires:
// c613ba134 accel/ivpu: Fix reset_engine debugfs file logic
TEST_F(Device, DISABLED_ResetInvalidEngine) {
    SKIP_NO_DEBUGFS("reset_engine");

    ASSERT_EQ(write_debugfs_file("reset_engine", 3), -1);
    ASSERT_EQ(errno, EINVAL);
}

TEST_F(Device, Heartbeat_ComputeEngine) {
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
