/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "kmd_test.h"

#include <chrono>
#include <climits>
#include <cstdio>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <linux/netlink.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <thread>

#define IVPU_PLATFORM_SILICON 0
#define IVPU_PLATFORM_SIMICS 2
#define IVPU_PLATFORM_FPGA 3
#define IVPU_PLATFORM_INVALID 8

KmdContext::KmdContext()
    : fd(-1)
    , ssid(0)
    , pci_id(0) {}

KmdContext::~KmdContext() {
    close();
}

int KmdContext::open() {
    if (fd < 0) {
        drm_device_desc desc = drm::open_intel_vpu();
        fd = desc.fd;
        major_id = desc.major_id;
        minor_id = desc.minor_id;
        drv_version_major = desc.version_major;
        drv_version_minor = desc.version_minor;
    }
    return fd;
}

int KmdContext::get_fd() const {
    return fd;
}

int KmdContext::get_major_version() const {
    return drv_version_major;
}

int KmdContext::get_minor_version() const {
    return drv_version_minor;
}

int KmdContext::get_major_id() const {
    return major_id;
}

int KmdContext::get_minor_id() const {
    return minor_id;
}

void KmdContext::close() {
    if (fd != -1) {
        ::close(fd);
        fd = -1;
        ssid = 0;
    }
}

int KmdContext::reopen() {
    close();
    return open();
}

int KmdContext::ioctl(unsigned long req, void *data) {
    if (::ioctl(fd, req, data) == -1)
        return errno;
    else
        return 0;
}

int KmdContext::get_param(uint32_t param, uint64_t *value, uint32_t index) {
    struct drm_ivpu_param args = {};
    int ret;
    args.index = index;
    args.param = param;
    ret = ioctl(DRM_IOCTL_IVPU_GET_PARAM, &args);
    if (!ret)
        *value = args.value;
    return ret;
}

int KmdContext::set_param(uint32_t param, uint64_t value, uint32_t index) {
    struct drm_ivpu_param args = {};
    int ret;
    args.index = index;
    args.param = param;
    args.value = value;
    ret = ioctl(DRM_IOCTL_IVPU_SET_PARAM, &args);
    return ret;
}

int KmdContext::bo_create(uint64_t size, uint32_t flags, uint32_t *handle, uint64_t *vpu_addr) {
    drm_ivpu_bo_create args = {};
    int ret;

    args.size = size;
    args.flags = flags;

    ret = ioctl(DRM_IOCTL_IVPU_BO_CREATE, &args);
    if (ret) {
        EXPECT_EQ(args.handle, 0u);
        EXPECT_EQ(args.vpu_addr, 0u);
    }

    *handle = args.handle;
    *vpu_addr = args.vpu_addr;
    return ret;
}

int KmdContext::bo_create_from_userptr(void *ptr,
                                       uint64_t size,
                                       uint32_t flags,
                                       uint32_t *handle,
                                       uint64_t *vpu_addr) {
    drm_ivpu_bo_create_from_userptr args = {};
    int ret;

    args.user_ptr = (uint64_t)ptr;
    args.size = size;
    args.flags = flags;

    ret = ioctl(DRM_IOCTL_IVPU_BO_CREATE_FROM_USERPTR, &args);
    if (ret) {
        EXPECT_EQ(args.handle, 0u);
        EXPECT_EQ(args.vpu_addr, 0u);
    }

    *handle = args.handle;
    *vpu_addr = args.vpu_addr;
    return ret;
}

int KmdContext::bo_info(drm_ivpu_bo_info *args) {
    int ret = ioctl(DRM_IOCTL_IVPU_BO_INFO, args);
    if (ret) {
        EXPECT_EQ(args->flags, 0u);
        EXPECT_EQ(args->vpu_addr, 0u);
        EXPECT_EQ(args->mmap_offset, 0u);
        EXPECT_EQ(args->size, 0u);
    } else {
        EXPECT_NE(args->vpu_addr, 0u);
        EXPECT_NE(args->size, 0u);
    }
    return ret;
}

int KmdContext::bo_close(uint32_t handle) {
    struct drm_gem_close args = {.handle = handle, .pad = 0};

    return ioctl(DRM_IOCTL_GEM_CLOSE, &args);
}

int KmdContext::bo_open(uint32_t name, uint32_t *handle, uint64_t *size) {
    struct drm_gem_open args = {.name = name, .handle = 0, .size = 0};

    int ret = ioctl(DRM_IOCTL_GEM_OPEN, &args);
    if (ret) {
        EXPECT_EQ(args.handle, 0u);
        EXPECT_EQ(args.size, 0u);
    } else {
        EXPECT_NE(args.handle, 0u);
        EXPECT_NE(args.size, 0u);
    }

    *handle = args.handle;
    *size = args.size;
    return ret;
}

int KmdContext::prime_handle_to_fd(uint32_t handle, uint32_t flags, int32_t *fd) {
    struct drm_prime_handle args = {.handle = handle, .flags = flags, .fd = 0};

    int ret = ioctl(DRM_IOCTL_PRIME_HANDLE_TO_FD, &args);
    if (ret)
        EXPECT_EQ(args.fd, 0u);
    else
        EXPECT_GT(args.fd, 0u);

    *fd = args.fd;
    return ret;
}

int KmdContext::prime_fd_to_handle(int32_t fd, uint32_t flags, uint32_t *handle) {
    struct drm_prime_handle args = {.handle = 0, .flags = flags, .fd = fd};

    int ret = ioctl(DRM_IOCTL_PRIME_FD_TO_HANDLE, &args);
    if (ret)
        EXPECT_EQ(args.handle, 0u);
    else
        EXPECT_NE(args.handle, 0u);

    *handle = args.handle;
    return ret;
}

int KmdContext::create_cmdq(uint32_t *cmdq_id, int priority, uint32_t flags) {
    struct drm_ivpu_cmdq_create args = {};

    args.priority = priority;
    args.flags = flags;

    int ret = ioctl(DRM_IOCTL_IVPU_CMDQ_CREATE, &args);
    if (ret)
        EXPECT_EQ(args.cmdq_id, 0u);
    else
        EXPECT_NE(args.cmdq_id, 0u);

    *cmdq_id = args.cmdq_id;
    return ret;
}

int KmdContext::destroy_cmdq(uint32_t cmdq_id) {
    struct drm_ivpu_cmdq_create args;

    args.cmdq_id = cmdq_id;

    return ioctl(DRM_IOCTL_IVPU_CMDQ_DESTROY, &args);
}

void *KmdContext::bo_mmap(size_t size, int prot, uint64_t mmap_offset) {
    void *ptr = ::mmap(0, size, prot, MAP_SHARED, fd, mmap_offset);
    if (ptr == MAP_FAILED)
        return NULL;
    return ptr;
}

uint32_t KmdContext::get_id() {
    if (ssid == 0) {
        uint64_t val = 0;
        int ret;

        ret = get_param(DRM_IVPU_PARAM_CONTEXT_ID, &val);
        if (!ret)
            ssid = val;
    }
    return ssid;
}

bool KmdContext::valid() {
    return get_id() != 0;
}

bool KmdContext::is_vpu37xx() {
    if (pci_id == 0) {
        uint64_t val = 0;
        get_param(DRM_IVPU_PARAM_DEVICE_ID, &val);
        pci_id = val;
    }
    return test_app::is_vpu37xx(pci_id);
}

size_t KmdContext::copy_desc_size() {
    if (is_vpu37xx())
        return sizeof(copy_descriptor::vpu37xx);

    return sizeof(copy_descriptor::vpu40xx);
}

KmdTest::KmdTest()
    : pci_id(0)
    , num_contexts(0) {
    context.close();
}

KmdTest::~KmdTest() {}

void KmdTest::SetUp() {
    int fd = context.open();
    ASSERT_GT(fd, -1) << "open() failed with error " << errno << " - " << strerror(errno);

    has_debugfs = debugfs_is_available();

    check_api_version();
    get_context_num();
    get_vpu_bus_id();
    get_param(DRM_IVPU_PARAM_DEVICE_ID, &pci_id);
    get_param(DRM_IVPU_PARAM_PLATFORM_TYPE, &platform_type);
    if (has_debugfs) {
        read_debugfs_file("reset_counter", initial_reset_counter);
    }
    ASSERT_EQ(sched_getaffinity(getpid(), sizeof(original_affinity), &original_affinity), 0);
}

void KmdTest::TearDown() {
    int current_reset_counter = 0;
    int reset_pending = 0;

    if (custom_affinity) {
        ASSERT_EQ(sched_setaffinity(getpid(), sizeof(original_affinity), &original_affinity), 0);
    }

    context.close();

    if (has_debugfs && !read_debugfs_file("reset_pending", reset_pending)) {
        EXPECT_EQ(reset_pending, 0) << "Unexpected VPU reset/recovery after test execution";
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(test_app::pause_after_test_ms));

    if (has_debugfs && !read_debugfs_file("reset_counter", current_reset_counter)) {
        int actual_resets = current_reset_counter - initial_reset_counter;
        EXPECT_EQ(expected_resets, actual_resets)
            << "The test failed because it caused " << actual_resets
            << " VPU resets/recoveries but " << expected_resets << " were expected";
    }
}

int KmdTest::open() {
    return context.open();
}

FILE *KmdTest::securefopen(const char *path, const char *mode) {
    struct stat pstat;

    FILE *file = fopen(path, mode);
    if (!file)
        return nullptr;

    if (fstat(fileno(file), &pstat) == -1) {
        fclose(file);
        return nullptr;
    }

    if (S_ISDIR(pstat.st_mode)) {
        fclose(file);
        return nullptr;
    }

    return file;
}

int KmdTest::get_param(uint32_t param, uint64_t *value, uint32_t index) {
    struct drm_ivpu_param args = {};
    int ret;
    args.index = index;
    args.param = param;
    ret = ioctl(DRM_IOCTL_IVPU_GET_PARAM, &args);

    *value = args.value;

    return ret;
}

bool KmdTest::is_silicon() {
    return (platform_type == IVPU_PLATFORM_SILICON);
}

bool KmdTest::is_simics() {
    return (platform_type == IVPU_PLATFORM_SIMICS);
}

bool KmdTest::is_fpga() {
    return (platform_type == IVPU_PLATFORM_FPGA);
}

bool KmdTest::api_version_lt(int major, int minor) {
    if (context.get_major_version() < major)
        return true;
    else if (context.get_major_version() == major)
        return context.get_minor_version() < minor;
    else
        return false;
}

bool KmdTest::is_patchset() {
    return api_version_lt(1, 3);
}

void KmdTest::get_context_num() {
    uint64_t param_value;
    ASSERT_EQ(0, get_param(DRM_IVPU_PARAM_NUM_CONTEXTS, &param_value));
    ASSERT_GT(param_value, 0ull);
    num_contexts = param_value;
}

void KmdTest::get_vpu_bus_id() {
    EXPECT_GT(context.get_major_id(), 0);
    EXPECT_GE(context.get_minor_id(), 0);
    vpu_bus_id = drm::get_vpu_bus_id(context.get_major_id(), context.get_minor_id());
    ASSERT_GT(vpu_bus_id.length(), 0u);
}

vpu_fw_api_version KmdTest::get_fw_api_version(int index) {
    vpu_fw_api_version_value val;
    uint64_t val64;
    int ret;
    ret = get_param(DRM_IVPU_PARAM_FW_API_VERSION, &val64, index);
    val.raw = val64;
    if (ret)
        val.raw = 0;
    return val.data;
}

bool KmdTest::is_fw_api_version_supported(int index, int major, int minor) {
    vpu_fw_api_version_value apiver = {};
    vpu_fw_api_version_value minver = {};

    apiver.data = get_fw_api_version(index);

    TRACE("API Version[%d]= %d.%d\n", index, apiver.data.major, apiver.data.minor);

    minver.data.major = major;
    minver.data.minor = minor;

    if (apiver.raw >= minver.raw)
        return true;
    return false;
}

void KmdTest::check_api_version() {
    ASSERT_EQ(context.get_major_version(), 1);
}

bool KmdTest::wait_for_resume(int timeout_ms) {
    test_app::overwrite_timeout(timeout_ms);
    auto timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    std::string state;

    do {
        int err = read_sysfs_file("power_state", state);
        if (!err && state == PM_STATE_D0)
            return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } while (std::chrono::steady_clock::now() < timeout);

    return false;
}

bool KmdTest::resume() {
    uint64_t hb;
    int ret;

    /* Always send HB so to update autosuspend timeout */
    ret = get_param(DRM_IVPU_PARAM_ENGINE_HEARTBEAT, &hb, ENGINE_COMPUTE);
    if (ret) {
        ADD_FAILURE() << "Failed to send IOCTL Heartbeat: " << ret << "\n";
        return false;
    }

    return true;
}

bool KmdTest::wait_for_suspend(int timeout_ms, bool expect_timeout) {
    test_app::overwrite_timeout(timeout_ms);
    auto timeout = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::string power_state;

    do {
        auto elapsed = std::chrono::steady_clock::now() - start;
        int err = read_sysfs_file("power_state", power_state);
        if (!err && power_state == PM_STATE_D3) {
            TRACE("Wait for %s done after %lld ms\n",
                  power_state.c_str(),
                  static_cast<long long>(
                      std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()));
            if (expect_timeout) {
                ADD_FAILURE() << "Expected to timeout waiting for suspend, but suspend occurred";
            }
            return true;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    } while (std::chrono::steady_clock::now() < timeout);

    if (!expect_timeout) {
        std::string usage, status;

        read_sysfs_file("power/runtime_usage", usage);
        read_sysfs_file("power/runtime_status", status);

        ADD_FAILURE() << "Failed to wait for suspend: " << "power_state=" << power_state
                      << ", runtime_usage=" << usage << ", runtime_status=" << status;
    }

    return false;
}

bool KmdTest::wait_for_recovery_event(int timeout_ms) {
    test_app::overwrite_timeout(timeout_ms);

    if (pm_mon.wait_for_recovery_event(timeout_ms))
        return true;

    return false;
}

int KmdTest::force_recovery() {
    expected_resets++;
    return write_debugfs_file("force_recovery", 1);
}

int KmdTest::get_autosuspend_delay(int &delay) {
    return read_sysfs_file("power/autosuspend_delay_ms", delay);
}

int KmdTest::set_autosuspend_delay(int delay) {
    return write_sysfs_file("power/autosuspend_delay_ms", delay);
}

int KmdTest::ioctl(unsigned long req, void *data) {
    return context.ioctl(req, data);
}

std::string KmdTest::get_sysfs_device_path() const {
    EXPECT_GT(context.get_major_id(), 0);
    EXPECT_GE(context.get_minor_id(), 0);
    return drm::get_sysfs_device_path(context.get_major_id(), context.get_minor_id());
}

std::string KmdTest::get_debugfs_path() const {
    EXPECT_GT(context.get_major_id(), 0);
    EXPECT_GE(context.get_minor_id(), 0);
    return drm::get_debugfs_path(context.get_major_id(), context.get_minor_id());
}

bool KmdTest::get_TDR_timeout(int &tdr) {
    int timeout = -1;

    /* In case tdr timeout is not default(module param > 0) it can be read from module parameter */
    if (read_module_param("tdr_timeout_ms", timeout) == 0 && timeout > 0) {
        tdr = timeout;
        return true;
    }

    /*Default tdr timeout is calculated based on ivpu_hw_timeouts_init() implementation in KMD */
    if (is_fpga())
        tdr = 30000;
    else if (is_simics())
        tdr = 10000;
    else
        tdr = 2000;

    return true;
}

bool KmdTest::is_hws_enabled() {
    std::string sched_mode;
    int err;

    err = read_sysfs_file("sched_mode", sched_mode);
    if (err)
        return false;

    return sched_mode == "HW";
}

bool KmdTest::is_userptr_supported() {
    uint64_t value;
    int ret = get_param(DRM_IVPU_PARAM_CAPABILITIES, &value, DRM_IVPU_CAP_BO_CREATE_FROM_USERPTR);
    return (ret == 0 && value == 1);
}

void KmdTest::SendCheckTimestamp(int engine, KmdContext &ctx) {
    const size_t map_len = 4096;

    MemoryBuffer ts_buf(ctx, map_len, VPU_BUF_USAGE_TIMESTAMP_HEAP);
    ASSERT_EQ(ts_buf.create(), 0);

    CmdBuffer cmd_buf(ctx, map_len, VPU_BUF_USAGE_BATCHBUFFER);
    ASSERT_EQ(cmd_buf.create(), 0);

    cmd_buf.add_ts_cmd(ts_buf, 0);

    ASSERT_EQ(cmd_buf.submit(engine), 0);
    ASSERT_EQ(cmd_buf.wait(), 0);

    uint32_t timestamp1 = *(uint32_t *)(ts_buf.ptr());
    EXPECT_GT(timestamp1, 0UL);
}

void KmdTest::SendCheckTimestamp(int engine) {
    SendCheckTimestamp(engine, context);
}

void KmdTest::SendFence(int buf_size, int write_offset, int read_offset) {
    uint32_t cmd_buf_size = 4096;

    CmdBuffer cmd_buf(context, cmd_buf_size, VPU_BUF_USAGE_BATCHBUFFER);
    ASSERT_EQ(cmd_buf.create(), 0);

    MemoryBuffer fence_buf(*this, buf_size, VPU_BUF_USAGE_FENCE_HEAP);
    ASSERT_EQ(fence_buf.create(), 0);

    cmd_buf.add_fence_signal_cmd(fence_buf, write_offset);
    cmd_buf.add_fence_wait_cmd(fence_buf, read_offset);

    ASSERT_EQ(cmd_buf.submit(ENGINE_COMPUTE), 0);
}

int KmdTest::write_bind_unbind_sysfs(std::string param) const {
    std::string path = drm::get_sysfs_driver_path();
    path += "/" + param;

    std::ofstream sysfs_file(path, std::ios::binary);
    if (sysfs_file.is_open()) {
        sysfs_file << vpu_bus_id;
        return 0;
    }

    return -EIO;
}

int KmdTest::bind_module() const {
    if (test_app::disable_unbind)
        return 0;

    return write_bind_unbind_sysfs("bind");
}

int KmdTest::unbind_module() {
    initial_reset_counter = 0;
    expected_resets = 0;

    if (test_app::disable_unbind)
        return 0;

    return write_bind_unbind_sysfs("unbind");
}

int KmdTest::rebind_module() {
    if (auto ret = unbind_module())
        return ret;

    return bind_module();
}

void KmdTest::fw_store() {
    SKIP_NO_DEBUGFS("fw_name");

    ASSERT_TRUE(debugfs_file_exists("fw_name"));
    ASSERT_EQ(read_debugfs_file("fw_name", fw_name), 0);
    TRACE("Store firmware path: %s\n", fw_name.c_str());
}

void KmdTest::fw_restore() {
    std::string param;
    int ret;

    // If the `firmware` module param does not exist then the firmware cannot
    // be changed and does not need to be restored
    ret = read_module_param("firmware", param);
    if (!ret && param != "(null)" && !fw_name.empty() && param != fw_name) {
        TRACE("Restore firmware path: %s\n", fw_name.c_str());

        context.close();

        EXPECT_EQ(unbind_module(), 0);
        EXPECT_EQ(write_module_param("firmware", fw_name), 0);
        EXPECT_EQ(bind_module(), 0);
    } else {
        TRACE("Restore firmware not needed. Firmware path: %s\n", fw_name.c_str());
        bind_module();
    }

    context.open();
}

void KmdTest::set_pcores_affinity() {
    // Ensure this is called only once
    ASSERT_EQ(custom_affinity, false);

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    CPU_SET(1, &mask);

    custom_affinity = true;

    // Set affinity to performance cores only to ensure stable unbind/bind time
    // which could get impacted by much slower TLB flush on efficient cores
    ASSERT_EQ(sched_setaffinity(getpid(), sizeof(mask), &mask), 0);
}

int KmdTest::WaitPid(pid_t pid, int secTimeout) {
    constexpr std::chrono::milliseconds sleep_time_ms = std::chrono::milliseconds(10);
    std::chrono::steady_clock::time_point timeout =
        std::chrono::steady_clock::now() + std::chrono::seconds(secTimeout);
    pid_t wait_pid;
    int status;

    do {
        std::this_thread::sleep_for(sleep_time_ms);
        if (std::chrono::steady_clock::now() > timeout) {
            kill(pid, SIGKILL);
            return -ETIMEDOUT;
        }
        wait_pid = waitpid(pid, &status, WNOHANG);

    } while (wait_pid != pid);

    if (WIFEXITED(status) != 0)
        return WEXITSTATUS(status);
    else
        return -ENOEXEC;
}

int KmdTest::RunCommand(char *const commandLine[], int secTimeout) {
    pid_t pid;

    pid = fork();
    if (pid == 0) {
        execve(commandLine[0], commandLine, nullptr);
        exit(EXIT_FAILURE);
    }
    EXPECT_GT(pid, 0);

    return pid <= 0 ? (int)pid : WaitPid(pid, secTimeout);
}

void KmdTest::CopyTest(KmdContext &ctx, MemoryBuffer &src_buf, uint64_t size, uint8_t pattern) {
    uint64_t cmd_size = 4 * KB;

    MemoryBuffer dst_buf(ctx, size);
    ASSERT_EQ(dst_buf.create(), 0);

    MemoryBuffer desc_buf(ctx, cmd_size, VPU_BUF_USAGE_DESCRIPTOR_HEAP);
    ASSERT_EQ(desc_buf.create(), 0);

    CmdBuffer cmd_buf(ctx, cmd_size);
    ASSERT_EQ(cmd_buf.create(), 0);

    cmd_buf.add_copy_cmd(desc_buf, 0, src_buf, 0, dst_buf, 0, size);

    ASSERT_EQ(cmd_buf.submit(), 0);
    ASSERT_EQ(cmd_buf.wait(), 0);
    EXPECT_BYTE_ARR_EQ(dst_buf.ptr(), size, pattern);
}

static uint32_t buf_usage_to_flags(VPU_BUF_USAGE usage) {
    switch (usage) {
    case VPU_BUF_USAGE_INPUT_LOW:
    case VPU_BUF_USAGE_KERNEL_HEAP:
    case VPU_BUF_USAGE_DESCRIPTOR_HEAP:
        return DRM_IVPU_BO_WC | DRM_IVPU_BO_MAPPABLE;
    case VPU_BUF_USAGE_INPUT_HIGH:
        return DRM_IVPU_BO_WC | DRM_IVPU_BO_SHAVE_MEM | DRM_IVPU_BO_MAPPABLE;
    case VPU_BUF_USAGE_OUTPUT_LOW:
    case VPU_BUF_USAGE_INPUT_OUTPUT_LOW:
    case VPU_BUF_USAGE_TIMESTAMP_HEAP:
    case VPU_BUF_USAGE_FENCE_HEAP:
    case VPU_BUF_USAGE_BATCHBUFFER:
        return DRM_IVPU_BO_CACHED | DRM_IVPU_BO_MAPPABLE;
    case VPU_BUF_USAGE_OUTPUT_HIGH:
    case VPU_BUF_USAGE_INPUT_OUTPUT_HIGH:
        return DRM_IVPU_BO_CACHED | DRM_IVPU_BO_SHAVE_MEM | DRM_IVPU_BO_MAPPABLE;
    case VPU_BUF_USAGE_SCRATCH:
        return DRM_IVPU_BO_WC | DRM_IVPU_BO_SHAVE_MEM | DRM_IVPU_BO_MAPPABLE;
    case VPU_BUF_USAGE_INPUT_DMA:
        return DRM_IVPU_BO_WC | DRM_IVPU_BO_DMA_MEM | DRM_IVPU_BO_MAPPABLE;
    case VPU_BUF_USAGE_OUTPUT_DMA:
    case VPU_BUF_USAGE_INPUT_OUTPUT_DMA:
        return DRM_IVPU_BO_CACHED | DRM_IVPU_BO_DMA_MEM | DRM_IVPU_BO_MAPPABLE;
    case VPU_BUF_USAGE_PREEMPT_LOW:
        return DRM_IVPU_BO_WC;
    case VPU_BUF_USAGE_PREEMPT_HIGH:
        return DRM_IVPU_BO_WC | DRM_IVPU_BO_DMA_MEM;

    default:
        ADD_FAILURE();
        return UINT32_MAX;
    }
}

MemoryBuffer::MemoryBuffer(KmdContext &context, size_t size, VPU_BUF_USAGE usage)
    : _context(context)
    , _buffer(nullptr)
    , _size(size)
    , _handle(0)
    , _vpu_addr(0)
    , _mmap_offset(0)
    , _user_ptr(nullptr) {
    _flags = buf_usage_to_flags(usage);
}

MemoryBuffer::MemoryBuffer(KmdContext &context)
    : MemoryBuffer(context, 0, VPU_BUF_USAGE_INPUT_LOW) {}

MemoryBuffer::MemoryBuffer(KmdTest &owner, size_t size, VPU_BUF_USAGE usage)
    : MemoryBuffer(owner.context, size, usage) {}

// Userptr constructor - simplified, only sets _user_ptr to non-null to indicate userptr usage
MemoryBuffer::MemoryBuffer(KmdContext &context, size_t size, int flags)
    : _context(context)
    , _buffer(nullptr)
    , _size(size)
    , _flags(flags)
    , _handle(0)
    , _vpu_addr(0)
    , _mmap_offset(0)
    , _user_ptr(nullptr) {}

void MemoryBuffer::set_flags(uint32_t flags) {
    _flags = flags;
}

int MemoryBuffer::create() {
    int ret;

    EXPECT_EQ(_buffer, nullptr);
    EXPECT_EQ(_user_ptr, nullptr);
    EXPECT_NE(_size, 0u);
    EXPECT_EQ(_handle, 0u);
    EXPECT_EQ(_vpu_addr, 0u);
    EXPECT_EQ(_mmap_offset, 0u);

    ret = _context.bo_create(_size, _flags, &_handle, &_vpu_addr);
    if (ret)
        return ret;

    ret = init_info();
    if (ret) {
        destroy();
        return ret;
    }

    if (_flags & DRM_IVPU_BO_MAPPABLE) {
        ret = mmap();
        if (ret)
            destroy();
    }

    return ret;
}

int MemoryBuffer::create_from_userptr(void *userptr) {
    int ret;

    EXPECT_EQ(_buffer, nullptr);
    EXPECT_EQ(_user_ptr, nullptr);
    EXPECT_NE(_size, 0u);
    EXPECT_EQ(_handle, 0u);
    EXPECT_EQ(_vpu_addr, 0u);
    EXPECT_EQ(_mmap_offset, 0u);

    // userptr is now mandatory
    if (!userptr) {
        ADD_FAILURE() << "Userptr cannot be null";
        return -EINVAL;
    }

    _user_ptr = userptr;

    ret = _context.bo_create_from_userptr(_user_ptr, _size, _flags, &_handle, &_vpu_addr);
    if (ret)
        return ret;

    EXPECT_NE(_handle, 0u);
    EXPECT_NE(_vpu_addr, 0u);

    ret = init_info();
    if (ret) {
        destroy();
        return ret;
    }

    return ret;
}

int MemoryBuffer::prime_fd_to_handle(int32_t fd) {
    int ret;

    EXPECT_EQ(_buffer, nullptr);
    EXPECT_EQ(_size, 0u);
    EXPECT_EQ(_handle, 0u);
    EXPECT_EQ(_vpu_addr, 0u);
    EXPECT_EQ(_mmap_offset, 0u);

    ret = _context.prime_fd_to_handle(fd, 0, &_handle);
    if (ret)
        return ret;

    ret = init_info();
    if (ret) {
        destroy();
        return ret;
    }

    EXPECT_EQ(lseek(fd, 0, SEEK_END), _size);
    lseek(fd, 0, SEEK_CUR);
    return ret;
}

int MemoryBuffer::prime_handle_to_fd(int32_t *fd) {
    return _context.prime_handle_to_fd(_handle, O_RDWR, fd);
}

void MemoryBuffer::destroy() {
    if (_buffer) {
        EXPECT_EQ(munmap(), 0);
    }

    if (_handle)
        close();

    _user_ptr = nullptr;

    EXPECT_EQ(_handle, 0u);
}

int MemoryBuffer::init_info() {
    int ret;

    drm_ivpu_bo_info info_args = {};
    info_args.handle = _handle;

    ret = _context.bo_info(&info_args);
    if (ret)
        return ret;

    EXPECT_LE(_size, info_args.size);

    _vpu_addr = info_args.vpu_addr;
    _mmap_offset = info_args.mmap_offset;
    _size = info_args.size;
    return 0;
}

int MemoryBuffer::close() {
    EXPECT_NE(_handle, 0);

    int ret = _context.bo_close(_handle);
    EXPECT_TRUE(ret == 0 || ret == ENODEV) << "Incorrect error when closing buffer object: " << ret;

    _vpu_addr = 0;
    _handle = 0;

    // For userptr buffers, we don't want to clear mmap_offset since it's not used
    if (!_user_ptr) {
        _mmap_offset = 0;
    }
    return 0;
}

int MemoryBuffer::mmap() {
    if (_buffer)
        return EALREADY;

    if (!(_flags & DRM_IVPU_BO_MAPPABLE))
        return EINVAL;

    if (_mmap_offset == 0)
        return EINVAL;

    _buffer = _context.bo_mmap(_size, PROT_READ | PROT_WRITE, _mmap_offset);
    if (!_buffer)
        return ENOMEM;

    return 0;
}

int MemoryBuffer::munmap() {
    int ret = 0;

    if (!_buffer)
        return EINVAL;

    if (!_mmap_offset)
        return EINVAL;

    ret = ::munmap(_buffer, _size);
    _mmap_offset = 0;
    _buffer = nullptr;
    return ret;
}

uint8_t *MemoryBuffer::ptr(int offset) {
    if (_user_ptr) {
        return (uint8_t *)((uintptr_t)_user_ptr + offset);
    }
    return (uint8_t *)((uintptr_t)_buffer + offset);
}

uint32_t *MemoryBuffer::ptr32(int offset) {
    return (uint32_t *)ptr(offset);
};

uint64_t *MemoryBuffer::ptr64(int offset) {
    return (uint64_t *)ptr(offset);
};

uint32_t MemoryBuffer::handle() {
    return _handle;
}

uint64_t MemoryBuffer::vpu_addr() {
    return _vpu_addr;
}

void MemoryBuffer::fill(uint8_t pattern, size_t offset, size_t len) {
    if (len == 0 || (len > (_size - offset)))
        len = _size - offset;
    memset(ptr(offset), pattern, len);
}

void MemoryBuffer::clear(size_t offset, size_t len) {
    fill(0, offset, len);
}

bool MemoryBuffer::verify_pattern(uint8_t pattern) {
    uint8_t *ptr = this->ptr();
    for (size_t i = 0; i < ALIGN_PAGE(_size); i++) {
        if (ptr[i] != pattern) {
            return false;
        }
    }
    return true;
}

void MemoryBuffer::verify_bo_info(uint32_t expected_flags) {
    struct drm_ivpu_bo_info info;
    memset(&info, 0, sizeof(info));
    info.handle = handle();
    ASSERT_EQ(_context.bo_info(&info), 0);
    ASSERT_EQ(info.handle, handle());
    ASSERT_EQ(info.vpu_addr, vpu_addr());
    ASSERT_EQ(info.size, ALIGN_PAGE(_size));

    if (expected_flags != 0) {
        ASSERT_EQ(info.flags & expected_flags, expected_flags);
    }
}

MemoryBuffer::~MemoryBuffer() {
    destroy();
}

CmdBuffer::CmdBuffer(KmdContext &context, size_t size, VPU_BUF_USAGE usage)
    : MemoryBuffer(context, size, usage)
    , _start(0)
    , _end(sizeof(vpu_cmd_buffer_header_t))
    , _preempt_buffer_index(0) {}

int CmdBuffer::create() {
    int ret = MemoryBuffer::create();
    if (ret)
        return ret;

    // Command buffer must be cleared by default, otherwise header
    // may contain random values that can confuse VPU
    clear();
    start(0);
    return 0;
}

int CmdBuffer::create_from_fd(int fd) {
    int ret = MemoryBuffer::prime_fd_to_handle(fd);
    if (ret)
        return ret;

    ret = mmap();
    if (ret)
        return ret;

    clear();
    start(0);
    return 0;
}

void CmdBuffer::start(int offset, int cmds_offset) {
    _start = offset;
    clear(_start, sizeof(vpu_cmd_buffer_header_t));

    vpu_cmd_buffer_header_t *bb_hdr = hdr();

    if (cmds_offset) {
        bb_hdr->cmd_offset = cmds_offset;
    } else {
        bb_hdr->cmd_offset = sizeof(vpu_cmd_buffer_header_t);
    }

    _end = _start + bb_hdr->cmd_offset;
    referenced_handles.clear();
    add_handle(*this);

    _preempt_buffer_index = 0;
}

// Resize the command buffer to a new size.
// If the new size is larger than the current size, it will add a NOP command
// to fill the gap. If the new size is smaller, it will truncate the command buffer.
void CmdBuffer::resize(uint32_t bb_size) {
    uint32_t old_bb_size = _end - _start;

    if (bb_size > old_bb_size) {
        add_nop_cmd(bb_size - old_bb_size);
    } else {
        _end = _start + bb_size;
    }
}

void *CmdBuffer::add_cmd(int type, int size) {
    if (get_free_space() < static_cast<ssize_t>(size)) {
        ADD_FAILURE() << "Command buffer overflow";
        return NULL;
    }

    vpu_cmd_header_t *header = (vpu_cmd_header_t *)ptr(_end);
    header->type = type;
    header->size = size;

    _end += size;

    return header;
}

vpu_cmd_buffer_header_t *CmdBuffer::hdr() {
    return (vpu_cmd_buffer_header_t *)ptr(_start);
}

uint32_t CmdBuffer::add_handle(MemoryBuffer &buf) {
    auto it = std::find(referenced_handles.begin(), referenced_handles.end(), buf.handle());
    if (it == referenced_handles.end()) {
        referenced_handles.push_back(buf.handle());
        return referenced_handles.size() - 1;
    }

    return std::distance(referenced_handles.begin(), it);
}

ssize_t CmdBuffer::get_free_space() {
    // Also account for space reserved for context save area buffer
    return _size - VPU_CONTEXT_SAVE_AREA_SIZE - _end;
}

void CmdBuffer::add_nop_cmd(int size) {
    ASSERT_TRUE(add_cmd(VPU_CMD_NOP, size));
}

void CmdBuffer::add_barrier_cmd() {
    ASSERT_TRUE(add_cmd<vpu_cmd_barrier_t>(VPU_CMD_BARRIER));
}

void CmdBuffer::add_ts_cmd(MemoryBuffer &ts_buf, uint32_t ts_offset, enum vpu_time_type type) {
    auto cmd = add_cmd<vpu_cmd_timestamp_t>(VPU_CMD_TIMESTAMP);
    ASSERT_TRUE(cmd);
    cmd->timestamp_address = ts_buf.vpu_addr() + ts_offset;
    cmd->type = type;
    add_handle(ts_buf);
}

void CmdBuffer::add_fence_cmd(uint64_t fence_address,
                              uint64_t fence_value,
                              enum vpu_cmd_type type) {
    auto cmd = add_cmd<vpu_cmd_fence_t>(type);
    ASSERT_TRUE(cmd);
    cmd->offset = fence_address;
    cmd->value = fence_value;
}

void CmdBuffer::add_fence_cmd(MemoryBuffer &fence_buf,
                              uint32_t fence_offset,
                              uint64_t fence_value,
                              enum vpu_cmd_type type) {
    add_fence_cmd(fence_buf.vpu_addr() + fence_offset, fence_value, type);
    add_handle(fence_buf);
}

void CmdBuffer::add_fence_reset_cmd(MemoryBuffer &fence_buf,
                                    uint32_t fence_offset,
                                    uint64_t fence_value) {
    add_fence_cmd(fence_buf, fence_offset, fence_value, VPU_CMD_FENCE_SIGNAL);
    add_handle(fence_buf);
}

void CmdBuffer::add_fence_signal_cmd(MemoryBuffer &fence_buf,
                                     uint32_t fence_offset,
                                     uint64_t fence_value) {
    add_fence_cmd(fence_buf, fence_offset, fence_value, VPU_CMD_FENCE_SIGNAL);
}

void CmdBuffer::add_fence_wait_cmd(MemoryBuffer &fence_buf,
                                   uint32_t fence_offset,
                                   uint64_t fence_value) {
    add_fence_cmd(fence_buf, fence_offset, fence_value, VPU_CMD_FENCE_WAIT);
}

// Use arbitrary address to write at (for negative testing)
void CmdBuffer::add_write_cmd(uint64_t addr, uint64_t value) {
    auto cmd = add_cmd<vpu_cmd_fence_t>(VPU_CMD_FENCE_SIGNAL);
    ASSERT_TRUE(cmd);
    cmd->offset = addr;
    cmd->value = value;
}

void CmdBuffer::add_memory_fill_cmd(MemoryBuffer &buf,
                                    uint64_t offset,
                                    uint64_t size,
                                    uint32_t fill_pattern) {
    auto cmd = add_cmd<vpu_cmd_memory_fill_t>(VPU_CMD_MEMORY_FILL);
    ASSERT_TRUE(cmd);
    cmd->start_address = buf.vpu_addr() + offset;
    cmd->size = size;
    cmd->fill_pattern = fill_pattern;

    add_handle(buf);
}

void CmdBuffer::copy_desc_fill(copy_descriptor_t *copy_descriptor,
                               uint64_t src_vpu_address,
                               uint64_t drc_vpu_address,
                               size_t size) {
    if (_context.is_vpu37xx()) {
        copy_descriptor->vpu37xx.src_address = src_vpu_address;
        copy_descriptor->vpu37xx.dst_address = drc_vpu_address;
        copy_descriptor->vpu37xx.size = size;
    } else {
        copy_descriptor->vpu40xx.src_address = src_vpu_address;
        copy_descriptor->vpu40xx.dst_address = drc_vpu_address;
        copy_descriptor->vpu40xx.size = size;
    }
}

void CmdBuffer::add_copy_cmd(MemoryBuffer &desc_buf,
                             uint32_t desc_start_offset,
                             MemoryBuffer &src_buf,
                             uint32_t src_offset,
                             MemoryBuffer &dst_buf,
                             uint32_t dst_offset,
                             size_t length,
                             uint16_t copy_cmd) {
#if HAS_COPY_ENGINE
    auto cmd = add_cmd<vpu_cmd_copy_buffer_t>(copy_cmd);
#else
    auto cmd = add_cmd<vpu_cmd_copy_buffer_t>(VPU_CMD_COPY_LOCAL_TO_LOCAL);
#endif
    ASSERT_TRUE(cmd);
    cmd->desc_start_offset = desc_buf.vpu_addr() + desc_start_offset;
    cmd->desc_count = 1;

    auto cmd_copy_desc = (copy_descriptor_t *)desc_buf.ptr(desc_start_offset);
    copy_desc_fill(cmd_copy_desc,
                   src_buf.vpu_addr() + (src_offset),
                   dst_buf.vpu_addr() + (dst_offset),
                   length);

    add_handle(desc_buf);
    add_handle(src_buf);
    add_handle(dst_buf);
}

void CmdBuffer::prepare_bb_hdr(void) {
    vpu_cmd_buffer_header_t *bb_hdr = hdr();

    bb_hdr->cmd_buffer_size = _end - _start;

    // Don't set API version yet, it may cause compatibility issues with older FW versions
    // bb_hdr->api_version = API_VER(VPU_JSM_JOB_CMD);

    bb_hdr->context_save_area_address = vpu_addr() + ALIGN(_end, 64);

    TRACE("Submit: ssid %d, addr 0x%lx, bb size %d, cmds offset %d, apiver 0x%08x, bos %lu, "
          "preempt %u\n",
          _context.get_id(),
          vpu_addr() + _start,
          bb_hdr->cmd_buffer_size,
          bb_hdr->cmd_offset,
          bb_hdr->api_version,
          referenced_handles.size(),
          _preempt_buffer_index);
}

void CmdBuffer::prepare_params(int engine, int priority, drm_ivpu_submit *params) {
    params->engine = engine;
    // Force compute engine if Copy Engine not supported
    if (!HAS_COPY_ENGINE)
        params->engine = ENGINE_COMPUTE;
    params->buffer_count = referenced_handles.size();
    params->buffers_ptr = (__u64)referenced_handles.data();
    params->commands_offset = _start;
    params->priority = priority;
}

void CmdBuffer::set_preempt_buffer(MemoryBuffer &buf) {
    _preempt_buffer_index = add_handle(buf);
}

int CmdBuffer::submit(int engine, int priority, uint32_t submit_timeout_ms) {
    test_app::overwrite_timeout(submit_timeout_ms);
    drm_ivpu_submit params = {};

    if (ALIGN(_end, 64) + VPU_CONTEXT_SAVE_AREA_SIZE > _size)
        return -ENOSPC;

    prepare_bb_hdr();
    prepare_params(engine, priority, &params);

    return submit_retry(&params, submit_timeout_ms);
}

// Retry submit if VPU is BUSY
int CmdBuffer::submit_retry(drm_ivpu_submit *params, uint32_t submit_timeout_ms) {
    test_app::overwrite_timeout(submit_timeout_ms);
    std::chrono::steady_clock::time_point timeOut =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(submit_timeout_ms);
    constexpr std::chrono::milliseconds sleep_time_ms = std::chrono::milliseconds(100);
    int ret = 0;

    do {
        ret = _context.ioctl(DRM_IOCTL_IVPU_SUBMIT, params);
        if (ret == 0 || ret == EBUSY)
            break;

        std::this_thread::sleep_for(sleep_time_ms);
    } while (std::chrono::steady_clock::now() < timeOut);

    return ret;
}

int CmdBuffer::cmdq_submit(uint32_t cmdq_id) {
    drm_ivpu_cmdq_submit args = {};

    if (ALIGN(_end, 64) + VPU_CONTEXT_SAVE_AREA_SIZE > _size)
        return -ENOSPC;

    args.buffers_ptr = (__u64)referenced_handles.data();
    args.buffer_count = (__u32)referenced_handles.size();
    args.cmdq_id = cmdq_id;
    args.commands_offset = _start;
    args.preempt_buffer_index = _preempt_buffer_index;

    prepare_bb_hdr();

    return _context.ioctl(DRM_IOCTL_IVPU_CMDQ_SUBMIT, &args);
}

int CmdBuffer::wait(uint32_t timeout_ms) {
    test_app::overwrite_timeout(timeout_ms);
    int64_t timeout_abs_ns = drm::time_ns() + MILLI_TO_NSEC(timeout_ms);
    struct drm_ivpu_bo_wait args = {.handle = _handle,
                                    .flags = 0,
                                    .timeout_ns = timeout_abs_ns,
                                    .job_status = std::numeric_limits<uint32_t>::max(),
                                    .pad = 0};

    int ret = _context.ioctl(DRM_IOCTL_IVPU_BO_WAIT, &args);

    if (ret == 0)
        return args.job_status;

    return ret;
}

PmMonitor::PmMonitor() {
    uevent_sock =
        socket(PF_NETLINK, SOCK_RAW | SOCK_CLOEXEC | SOCK_NONBLOCK, NETLINK_KOBJECT_UEVENT);
    if (uevent_sock == -1)
        throw std::runtime_error("Failed to create NETLINK_KOBJECT_UEVENT socket");

    int enable = 1;
    int ret = setsockopt(uevent_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    if (ret == -1)
        throw std::runtime_error("Failed to set uevent socket options");

    sockaddr_nl nladdr = {};
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = 0;
    nladdr.nl_groups = 1;
    ret = bind(uevent_sock, (struct sockaddr *)&nladdr, sizeof(nladdr));
    if (ret == -1)
        throw std::runtime_error("Failed to bind uevent socket");
}

PmMonitor::~PmMonitor() {
    close(uevent_sock);
}

bool PmMonitor::wait_for_recovery_event(unsigned timeout_ms) {
    const char event[] = "IVPU_PM_EVENT=IVPU_RECOVER";
    constexpr int response_size = 4096;
    char response[response_size] = {};

    test_app::overwrite_timeout(timeout_ms);
    std::chrono::steady_clock::time_point timeout =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(timeout_ms);

    do {
        ssize_t len = recv(uevent_sock, response, response_size - 1, 0);
        if (len > 0) {
            response[len] = 0;
            char *p = response;
            char *end = response + len;
            do {
                if (p < end - sizeof(event) && strstr(p, event)) {
                    return true;
                }
                p = strchr(p, 0);
            } while (p && ++p < end);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } while (std::chrono::steady_clock::now() < timeout);

    return false;
}

void DmaBuffer::create() {
    ASSERT_EQ(_heap_fd, -1);
    ASSERT_EQ(_dmabuf_fd, -1);
    ASSERT_EQ(_buf_ptr, nullptr);
    ASSERT_GT(_size, 0);
    ASSERT_TRUE(_size % 4096 == 0) << "Size not aligned to page: " << _size;

    _heap_fd = ::open("/dev/dma_heap/system", O_RDWR);
    ASSERT_GE(_heap_fd, 0);

    struct dma_heap_allocation_data heap_alloc = {
        .len = _size,
        .fd = 0,
        .fd_flags = O_RDWR | O_CLOEXEC,
        .heap_flags = 0,
    };

    int ret = ::ioctl(_heap_fd, DMA_HEAP_IOCTL_ALLOC, &heap_alloc);
    ASSERT_GE(ret, 0);
    ASSERT_GE(heap_alloc.fd, 0);

    _dmabuf_fd = heap_alloc.fd;
}

void DmaBuffer::destroy() {
    if (_buf_ptr)
        munmap();
    if (_dmabuf_fd >= 0)
        ::close(_dmabuf_fd);
    if (_heap_fd >= 0)
        ::close(_heap_fd);

    _dmabuf_fd = _heap_fd = -1;
}

void DmaBuffer::mmap() {
    ASSERT_EQ(_buf_ptr, nullptr);

    _buf_ptr = ::mmap(NULL, _size, PROT_WRITE | PROT_READ, MAP_SHARED, _dmabuf_fd, 0);
    ASSERT_NE(_buf_ptr, MAP_FAILED) << "error " << errno;
}

void DmaBuffer::munmap() {
    ASSERT_NE(_buf_ptr, nullptr);
    ASSERT_GT(_size, 0);

    EXPECT_EQ(::munmap(_buf_ptr, _size), 0);
    _buf_ptr = NULL;
}

TEST_F(KmdTest, Init) {
    TRACE_IN();
}
