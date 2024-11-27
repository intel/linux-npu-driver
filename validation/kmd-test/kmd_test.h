/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>

#include "api/vpu_jsm_job_cmd_api.h"
#include "drm/ivpu_accel.h"
#include "drm_helpers.h"
#include "file_helper.h"
#include "linux/dma-heap.h"
#include "perf_counter.h"
#include "test_app.h"

#include <algorithm>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <libudev.h>
#include <linux/kernel.h>
#include <linux/magic.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <sys/vfs.h>

#define SKIP_(msg)                    \
    if (!test_app::run_skipped_tests) \
    GTEST_SKIP_(msg)

#define SKIP                          \
    if (!test_app::run_skipped_tests) \
    GTEST_SKIP

#define SKIP_SIMICS(msg) \
    if (is_simics())     \
    SKIP_(msg)

#define SKIP_PRESILICON(msg) \
    if (!is_silicon())       \
    SKIP_(msg)

#define SKIP_VPU37XX(msg) \
    if (is_vpu37xx())     \
    SKIP_(msg)

#define SKIP_VPU40XX(msg) \
    if (is_vpu40xx())     \
    SKIP_(msg)

#define SKIP_NO_DEBUGFS(fname)                            \
    if (!debugfs_is_available())                          \
        SKIP_("Debugfs not supported or mounted");        \
    if (!test_app::has_root_access())                     \
        SKIP_("Needs root privileges to access debugfs"); \
    if (!debugfs_file_exists(fname))                      \
        SKIP_("No " fname " debugfs file");

#define SKIP_NO_SYSFS(fname)       \
    if (!sysfs_file_exists(fname)) \
        SKIP_("No " fname " sysfs file");

#define SKIP_NEEDS_ROOT()             \
    if (!test_app::has_root_access()) \
    SKIP_("Needs root privileges")

#define SKIP_HARDENING(msg)    \
    if (is_hardening_kernel()) \
    SKIP_(msg)

#define SKIP_NO_HWS(msg)   \
    if (!is_hws_enabled()) \
    SKIP_(msg)

#define SKIP_NO_DMA_HEAP()        \
    if (!dma_heap_is_available()) \
    SKIP_("DMA heaps are not available")

#define EXPECT_BYTE_ARR_EQ(arr, size, value) EXPECT_PRED3(byte_array_eq, arr, size, value)

#define ALIGN(s, alignment) __ALIGN_KERNEL((s), (alignment))
#define ALIGN_PAGE(s) (ALIGN(s, 4096))
#define KB (1024llu)
#define MB (1024llu * 1024)
#define GB (1024llu * 1024 * 1024)

#define VPU_CMD_DATA_ALIGNMENT 64

#define JOB_SYNC_TIMEOUT_MS (60 * 1000)

#define PM_STATE_D0 "D0"
#define PM_STATE_D3 "D3hot"
#define PM_STATE_TIMEOUT_MS (6 * 1000)

#define ENGINE_COMPUTE DRM_IVPU_ENGINE_COMPUTE
#define ENGINE_COPY DRM_IVPU_ENGINE_COPY

#define HAS_COPY_ENGINE 0

typedef struct copy_descriptor {
    union {
        vpu_cmd_copy_descriptor_37xx_t vpu37xx;
        vpu_cmd_copy_descriptor_40xx_t vpu40xx;
    };
} copy_descriptor_t;

struct vpu_fw_api_version {
    __u16 minor;
    __u16 major;
};

union vpu_fw_api_version_value {
    __u64 raw;
    struct vpu_fw_api_version data;
};

enum VPU_BUF_USAGE {
    VPU_BUF_USAGE_BATCHBUFFER,
    VPU_BUF_USAGE_KERNEL_HEAP,
    VPU_BUF_USAGE_DESCRIPTOR_HEAP,
    VPU_BUF_USAGE_TIMESTAMP_HEAP,
    VPU_BUF_USAGE_FENCE_HEAP,
    VPU_BUF_USAGE_SCRATCH,

    // For input and output buffers
    VPU_BUF_USAGE_INPUT_HIGH,
    VPU_BUF_USAGE_OUTPUT_HIGH,
    VPU_BUF_USAGE_INPUT_OUTPUT_HIGH,

    // For input and output buffers that are used for
    // VPU_CMD_COPY_LOCAL_TO_LOCAL - this command seems to
    // be executed in LeonRT rather than the DMA engine
    // and it warks only with aliased memory range.
    VPU_BUF_USAGE_INPUT_LOW,
    VPU_BUF_USAGE_OUTPUT_LOW,
    VPU_BUF_USAGE_INPUT_OUTPUT_LOW,

    VPU_BUF_USAGE_INPUT_DMA,
    VPU_BUF_USAGE_OUTPUT_DMA,
    VPU_BUF_USAGE_INPUT_OUTPUT_DMA,

    VPU_BUF_USAGE_COUNT
};

enum FENCE_VAL : uint64_t {
    FENCE_RESET_VAL = 1,
    FENCE_WAIT_VAL,
    FENCE_SIGNAL_VAL,
};

class KmdContext {
  public:
    KmdContext();
    virtual ~KmdContext();

    int open();
    void close();

    int get_fd() const;
    int get_major_id() const;
    int get_minor_id() const;
    int get_major_version() const;
    int get_minor_version() const;
    int ioctl(unsigned long req, void *data);

    int get_param(uint32_t param, uint64_t *value, uint32_t index = 0);
    int set_param(uint32_t param, uint64_t value, uint32_t index);
    int bo_create(uint64_t size, uint32_t flags, uint32_t *handle, uint64_t *vpu_addr);

    int bo_info(drm_ivpu_bo_info *args);
    int bo_close(uint32_t handle);
    int bo_open(uint32_t name, uint32_t *handle, uint64_t *size);
    void *bo_mmap(size_t size, int prot, uint64_t mmap_offset);

    int prime_handle_to_fd(uint32_t handle, uint32_t flags, int32_t *fd);
    int prime_fd_to_handle(int32_t fd, uint32_t flags, uint32_t *handle);

    int get_unique_id();

    bool valid();
    bool is_vpu37xx();
    size_t copy_desc_size();

  protected:
    int fd;
    int major_id = -1;
    int minor_id = -1;
    uint32_t ssid;
    uint16_t pci_id;
    int drv_version_major = -1;
    int drv_version_minor = -1;
};

class PmMonitor {
  public:
    static constexpr const char *vpu_path = "/sys/devices/pci0000:00/0000:00:0b.0";
    udev_monitor *ud_mon{nullptr};
    udev_device *ud_dev{nullptr};
    udev *ud{nullptr};
    fd_set ud_fds;
    int ud_fd = 0;

    PmMonitor();
    bool wait_for_recovery_event(unsigned timeout_ms = PM_STATE_TIMEOUT_MS);
    ~PmMonitor();
};

class KmdTest : public ::testing::Test {
  public:
    KmdTest();
    virtual ~KmdTest();

    void SetUp(void) override;
    void TearDown(void) override;

    int open(void);

    FILE *securefopen(const char *path, const char *mode);

    int ioctl(unsigned long req, void *data);

    bool is_vpu37xx() { return test_app::is_vpu37xx(pci_id); }
    bool is_vpu40xx() { return test_app::is_vpu40xx(pci_id); }
    bool is_silicon();
    bool is_simics();
    bool is_fpga();

    int get_param(uint32_t param, uint64_t *value, uint32_t index = 0);

    vpu_fw_api_version get_fw_api_version(int index);
    bool is_fw_api_version_supported(int index, int major, int minor);

    void get_context_num();
    void get_vpu_bus_id();

    void check_api_version();
    bool api_version_lt(int major, int minor);

    bool resume();
    bool wait_for_resume(int timeout_ms = PM_STATE_TIMEOUT_MS);
    bool wait_for_suspend(int timeout_ms = PM_STATE_TIMEOUT_MS);
    bool wait_for_recovery_event(int timeout_ms = PM_STATE_TIMEOUT_MS);
    int force_recovery();
    int get_autosuspend_delay(int &delay);
    int set_autosuspend_delay(int delay);

    bool get_TDR_timeout(int &tdr);
    bool is_hws_enabled();
    int write_bind_unbind_sysfs(std::string param) const;
    int bind_module() const;
    int unbind_module();
    int rebind_module();

    void fw_store();
    void fw_restore();

    int RunCommand(char *const commandLine[], int secTimeout);

    void SendCheckTimestamp(int engine = ENGINE_COMPUTE);
    void SendCheckTimestamp(int engine, KmdContext &ctx);
    void SendFence(int buf_size, int write_offset, int read_offset);

    bool is_hardening_kernel() {
        struct utsname uts;

        uname(&uts);
        std::string str(uts.release);
        return str.find("hardening") != std::string::npos;
    }

    bool file_exists(const std::filesystem::path &path) {
        std::error_code ec;
        return std::filesystem::exists(path, ec);
    }

    bool debugfs_file_exists(const std::string &file) {
        return file_exists(get_debugfs_path() + "/" + file);
    }

    bool debugfs_is_available() {
        struct statfs sfs = {};

        statfs("/sys/kernel/debug", &sfs);
        return sfs.f_type == DEBUGFS_MAGIC;
    }

    bool dma_heap_is_available() { return file_exists("/dev/dma_heap/system"); }

    std::string get_sysfs_device_path() const;
    std::string get_debugfs_path() const;

    bool sysfs_file_exists(const std::string &file) {
        return file_exists(get_sysfs_device_path() + "/" + file);
    }

    template <class T>
    int read_sysfs_file(const std::string &fileName, T &val) {
        return read_file(get_sysfs_device_path() + "/" + fileName, val);
    }

    template <class T>
    int write_sysfs_file(const std::string &fileName, const T &val) {
        return write_existing_file(get_sysfs_device_path() + "/" + fileName, val);
    }

    template <typename T>
    int read_module_param(const std::string &param, T &val) {
        return read_file(drm::get_sysfs_module_path() + "/parameters/" + param, val);
    }

    template <typename T>
    int write_module_param(const std::string &param, const T &val) {
        return write_existing_file(drm::get_sysfs_module_path() + "/parameters/" + param, val);
    }

    template <class T>
    int read_debugfs_file(const std::string &file, T &val) {
        return read_file(get_debugfs_path() + "/" + file, val);
    }

    template <class T>
    int write_debugfs_file(const std::string &file, const T &val) {
        return write_existing_file(get_debugfs_path() + "/" + file, val);
    }

    template <class T>
    int write_existing_file(const std::string &path, const T &val) {
        if (!file_exists(path))
            return ENOENT;
        return write_file(path, val);
    }

    template <class T>
    int write_file(const std::string &path, const T &val) {
        FILE *file = fopen(path.c_str(), "wb");
        if (!file)
            return errno;

        std::ostringstream ss;
        ss << val;

        std::string str = ss.str();
        if (fwrite(str.c_str(), 1, str.size(), file) != str.size()) {
            fclose(file);
            return errno;
        }

        return fclose(file);
    }

    int read_file_to_ss(const std::string &path, std::stringstream &ss) {
        FILE *file = fopen(path.c_str(), "rb");
        if (!file)
            return errno;

        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file))
            ss << buffer;

        if (ferror(file)) {
            fclose(file);
            return errno;
        }

        return fclose(file);
    }

    template <class T>
    int read_file(const std::string &path, T &val) {
        std::stringstream ss;

        int ret = read_file_to_ss(path, ss);
        if (ret)
            return ret;

        if (std::is_integral_v<T> && ss.str().substr(0, 2) == "0x")
            ss >> std::hex >> val;
        else
            ss >> val;

        return ss.fail() ? EINVAL : 0;
    }

    int read_file(const std::string &path, std::string &str) {
        std::stringstream ss;

        int ret = read_file_to_ss(path, ss);
        if (ret)
            return ret;

        str = ss.str();
        int lines = std::count(str.begin(), str.end(), '\n');
        if (lines == 1 && str.back() == '\n')
            str.pop_back(); // remove newlines from single line files

        return 0;
    }

    std::string vpu_bus_id;
    uint64_t pci_id;
    uint64_t platform_type = 0;
    uint32_t num_contexts;
    KmdContext context;
    PmMonitor pm_mon;
    std::string fw_name;
    int initial_reset_counter = 0;
    int expected_resets = 0;
};

struct MemoryBuffer {
    KmdContext &_context;
    void *_buffer;
    size_t _size;
    uint32_t _flags;
    uint32_t _handle;
    uint64_t _vpu_addr;
    uint64_t _mmap_offset;

    MemoryBuffer(KmdContext &context, size_t size, VPU_BUF_USAGE usage = VPU_BUF_USAGE_BATCHBUFFER);
    MemoryBuffer(KmdContext &context);
    MemoryBuffer(KmdTest &owner, size_t size, VPU_BUF_USAGE usage); // Deprecated

    MemoryBuffer(const MemoryBuffer &) = delete;
    MemoryBuffer &operator=(MemoryBuffer const &) = delete;

    virtual ~MemoryBuffer();

    void set_flags(uint32_t flags);
    virtual int create();
    int close();
    int mmap();
    int munmap();
    int prime_fd_to_handle(int32_t fd);
    int prime_handle_to_fd(int32_t *fd);
    void destroy();
    uint8_t *ptr(int offset = 0);
    uint32_t *ptr32(int offset = 0);
    uint64_t *ptr64(int offset = 0);
    uint32_t handle();
    uint64_t vpu_addr();
    void fill(uint8_t pattern = 0, size_t offset = 0, size_t len = -1);
    void clear(size_t offset = 0, size_t len = -1);
    size_t size() { return _size; }

  private:
    int create_shmem();
    int init_info();
};

struct CmdBuffer : MemoryBuffer {
    CmdBuffer(KmdContext &context, size_t size, VPU_BUF_USAGE usage = VPU_BUF_USAGE_BATCHBUFFER);

    int create();
    void start(int offset);

    template <typename T>
    T *add_cmd(int type) {
        if (get_free_space() < static_cast<ssize_t>(sizeof(T))) {
            ADD_FAILURE() << "Command buffer overflow";
            return NULL;
        }

        vpu_cmd_header_t *header = (vpu_cmd_header_t *)ptr(_end);
        header->type = type;
        header->size = sizeof(T);

        _end += sizeof(T);

        return (T *)header;
    }

    vpu_cmd_buffer_header_t *hdr();
    void add_handle(MemoryBuffer &buf);
    ssize_t get_free_space();
    void add_barrier_cmd();
    void
    add_ts_cmd(MemoryBuffer &ts_buf, uint32_t ts_offset, enum vpu_time_type type = VPU_TIME_RAW);
    void add_fence_reset_cmd(MemoryBuffer &fence_buf,
                             uint32_t fence_offset,
                             uint64_t fence_val = FENCE_RESET_VAL);
    void add_fence_signal_cmd(MemoryBuffer &fence_buf,
                              uint32_t fence_offset,
                              uint64_t fence_val = FENCE_SIGNAL_VAL);
    void add_fence_wait_cmd(MemoryBuffer &fence_buf,
                            uint32_t fence_offset,
                            uint64_t fence_val = FENCE_WAIT_VAL);
    void add_memory_fill_cmd(MemoryBuffer &buf,
                             uint64_t start_address,
                             uint64_t size,
                             uint32_t fill_pattern);
    void copy_desc_fill(copy_descriptor_t *copy_descriptor,
                        uint64_t src_vpu_address,
                        uint64_t drc_vpu_address,
                        size_t size);
    void add_copy_cmd(MemoryBuffer &desc_buf,
                      uint32_t desc_start_offset,
                      MemoryBuffer &src_buf,
                      uint32_t src_offset,
                      MemoryBuffer &dst_buf,
                      uint32_t dst_offset,
                      size_t length,
                      uint16_t copy_cmd = VPU_CMD_COPY_LOCAL_TO_LOCAL);
    int submit(int engine = ENGINE_COMPUTE, int priority = 0, uint32_t timeout_ms = 0);
    void prepare_bb_hdr(void);
    void prepare_params(int engine, int priority, drm_ivpu_submit *params);
    int wait(uint32_t timeout_ms = JOB_SYNC_TIMEOUT_MS);

    std::vector<uint32_t> referenced_handles;
    void add_fence_cmd(MemoryBuffer &fence_buffer,
                       uint32_t fence_offset,
                       uint64_t fence_value,
                       enum vpu_cmd_type type);
    int submit_retry(drm_ivpu_submit *params, uint32_t timeout_ms = 0);

    uint32_t _start;
    uint32_t _end;
};

struct DmaBuffer {
    size_t _size;
    int _heap_fd;
    int _dmabuf_fd;
    void *_buf_ptr;

    DmaBuffer(size_t size)
        : _size(size)
        , _heap_fd(-1)
        , _dmabuf_fd(-1)
        , _buf_ptr(NULL) {}

    virtual ~DmaBuffer() { destroy(); }

    DmaBuffer(const DmaBuffer &) = delete;
    DmaBuffer &operator=(const DmaBuffer &) = delete;

    void create();
    void destroy();
    void mmap();
    void munmap();

    size_t size() { return _size; }
    size_t buffer_fd() { return _dmabuf_fd; }
    void *buffer_ptr(int offset = 0) { return ((uint8_t *)_buf_ptr + offset); }
    uint64_t *buffer_ptr64(int offset = 0) { return (uint64_t *)buffer_ptr(offset); }
};

bool byte_array_eq(uint8_t *arr, size_t size, uint8_t value);
