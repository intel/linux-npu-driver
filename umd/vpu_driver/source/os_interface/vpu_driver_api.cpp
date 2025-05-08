/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include "perfetto.h"

#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"

#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_ioctl_trace.hpp"
#include "vpu_driver/source/utilities/trace_perfetto.hpp" // IWYU pragma: keep

#include <exception>
#include <fcntl.h>
#include <memory>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <uapi/drm/drm.h>
#include <uapi/drm/ivpu_accel.h>
#include <unistd.h>
#include <utility>

namespace VPU {

VPUDriverApi::VPUDriverApi(std::string devPath, OsInterface &osInfc)
    : devPath(std::move(devPath))
    , osInfc(osInfc)
    , vpuFd(-1) {}

VPUDriverApi::~VPUDriverApi() {
    if (vpuFd > 0)
        closeDevice();
}

VPUDriverApi::VPUDriverApi(VPUDriverApi &&v)
    : devPath(v.devPath)
    , osInfc(v.osInfc)
    , vpuFd(v.vpuFd) {
    v.vpuFd = -1;
}

int VPUDriverApi::doIoctl(unsigned int request, void *arg) const {
    if (vpuFd < 0 || arg == nullptr) {
        LOG_E("Invalid arguments (vpuFd:%d, arg:%p)", vpuFd, arg);
        return -EINVAL;
    }

    TRACE_EVENT("SYS", perfetto::StaticString{driver_ioctl_request_str(request)});
    LOG(IOCTL, "ioctl(%s)..", driver_ioctl_trace(vpuFd, request, arg).c_str());
    int ret;
    do {
        ret = osInfc.osiIoctl(vpuFd, request, arg);
    } while (ret == -1 && (errno == -EAGAIN || errno == -EINTR));

    LOG(IOCTL, "ioctl(%s) = %i", driver_ioctl_trace(vpuFd, request, arg).c_str(), ret);
    if (ret != 0)
        LOG(IOCTL, "ioctl -> errno:%d, strerror:\"%s\"", errno, strerror(errno));

    return ret;
}

std::unique_ptr<VPUDriverApi> VPUDriverApi::openDriverApi(std::string devPath,
                                                          OsInterface &osInfc) {
    auto driverApi = std::make_unique<VPUDriverApi>(devPath, osInfc);

    if (!driverApi->openDevice())
        return nullptr;

    return driverApi;
}

bool VPUDriverApi::openDevice() {
    TRACE_EVENT("SYS", "open");
    vpuFd = osInfc.osiOpen(devPath.c_str(), (O_RDWR | O_CLOEXEC | O_NOFOLLOW), 0);
    if (vpuFd < 0)
        LOG(FSYS, "Failed to open '%s'", devPath.c_str());
    return vpuFd >= 0;
}

bool VPUDriverApi::closeDevice() {
    int ret = 0;
    TRACE_EVENT("SYS", "close");

    if (vpuFd > 0)
        ret = osInfc.osiClose(vpuFd);
    if (ret)
        LOG_E("Failed to close '%i' fd", vpuFd);

    vpuFd = -1;
    return ret == 0;
}

bool VPUDriverApi::isVpuDevice() const {
    drm_version_t version = {};
    int ret = doIoctl(DRM_IOCTL_VERSION, &version);
    if (ret < 0) {
        LOG(MISC, "Failed to get API version ioctl(ret: %d).", ret);
        return false;
    }

    if (umdIoctlVersionMajor != version.version_major) {
        LOG(MISC,
            "IOCTL version doesn't match! (UMD: %d.%d, KMD: %d.%d)",
            umdIoctlVersionMajor,
            umdIoctlVersionMinor,
            version.version_major,
            version.version_minor);
        return false;
    }

    std::string kmdVersion(version.name_len, '\0');
    version.name = kmdVersion.data();
    version.date_len = 0;
    version.desc_len = 0;

    ret = doIoctl(DRM_IOCTL_VERSION, &version);
    if (ret < 0) {
        LOG(MISC, "Failed to get API version ioctl(ret: %d).", ret);
        return false;
    }

    if (kmdVersion != umdIoctlDeviceName1 && kmdVersion != umdIoctlDeviceName2) {
        LOG_E("IOCTL device name doesn't match(UMD: %s or %s, KMD: %s)",
              umdIoctlDeviceName1,
              umdIoctlDeviceName2,
              kmdVersion.c_str());
        return false;
    }

    return true;
}

int VPUDriverApi::commandQueueCreate(uint32_t priority, uint32_t &queueId) {
    drm_ivpu_cmdq_create createArgs = {};

    createArgs.priority = priority;
    int ret = doIoctl(DRM_IOCTL_IVPU_CMDQ_CREATE, &createArgs);
    if (ret) {
        LOG_E("DRM_IOCTL_IVPU_CMDQ_CREATE failed, error %d", ret);
        return ret;
    }
    queueId = createArgs.cmdq_id;
    return 0;
}

int VPUDriverApi::commandQueueSubmit(const void *buffers, uint32_t bufCnt, uint32_t queueId) const {
    drm_ivpu_cmdq_submit submitArgs = {};
    submitArgs.buffers_ptr = reinterpret_cast<uint64_t>(buffers);
    submitArgs.buffer_count = bufCnt;
    submitArgs.cmdq_id = queueId;

    int ret = doIoctl(DRM_IOCTL_IVPU_CMDQ_SUBMIT, &submitArgs);
    if (ret)
        LOG_E("DRM_IOCTL_IVPU_CMDQ_SUBMIT failed, error %d", ret);
    return ret;
}

int VPUDriverApi::commandQueueDestroy(uint32_t queueId) const {
    drm_ivpu_cmdq_destroy destroyArgs = {queueId};

    int ret = doIoctl(DRM_IOCTL_IVPU_CMDQ_DESTROY, &destroyArgs);
    if (ret)
        LOG_E("DRM_IOCTL_IVPU_CMDQ_DESTROY failed, error %d", ret);
    return ret;
}

int VPUDriverApi::submitCommandBuffer(drm_ivpu_submit *arg) const {
    return doIoctl(DRM_IOCTL_IVPU_SUBMIT, arg);
}

bool VPUDriverApi::checkDeviceCapability(uint32_t index) const {
    struct drm_ivpu_param arg = {};
    arg.param = DRM_IVPU_PARAM_CAPABILITIES;
    arg.index = index;
    if (doIoctl(DRM_IOCTL_IVPU_GET_PARAM, &arg)) {
        LOG(MISC,
            "Capability does not exist, index: %s (%#x), errno: %d",
            driver_struct_param_cap_index_str(index),
            index,
            errno);
        return false;
    }

    if (arg.value == 0) {
        LOG(MISC,
            "Capability from index: %s (%#x) is not set",
            driver_struct_param_cap_index_str(index),
            index);
        return false;
    }

    LOG(MISC,
        "Capability from index: %s (%#x) is set",
        driver_struct_param_cap_index_str(index),
        index);
    return true;
}

bool VPUDriverApi::checkPrimeBuffersCapability() const {
    drm_get_cap args = {.capability = DRM_CAP_PRIME, .value = 0ULL};

    int ret = doIoctl(DRM_IOCTL_GET_CAP, &args);
    if (ret) {
        LOG_E("Failed to call DRM_IOCTL_GET_CAP");
        return false;
    }
    const uint64_t primeMask = DRM_PRIME_CAP_IMPORT | DRM_PRIME_CAP_EXPORT;
    return (args.value & primeMask) == primeMask ? true : false;
}

size_t VPUDriverApi::getPageSize() const {
    return osInfc.osiGetSystemPageSize();
}

int VPUDriverApi::wait(void *args) const {
    return doIoctl(DRM_IOCTL_IVPU_BO_WAIT, args);
}

int VPUDriverApi::closeBuffer(uint32_t handle) const {
    struct drm_gem_close args = {.handle = handle, .pad = 0};
    return doIoctl(DRM_IOCTL_GEM_CLOSE, &args);
}

int VPUDriverApi::createBuffer(size_t size,
                               uint32_t flags,
                               uint32_t &handle,
                               uint64_t &vpuAddr) const {
    drm_ivpu_bo_create args = {};
    args.size = size;
    args.flags = flags;

    int ret = doIoctl(DRM_IOCTL_IVPU_BO_CREATE, &args);
    if (ret) {
        if (errno == ENOSPC) {
            LOG_E("Buffer size is too big");
        }

        LOG_E("Failed to call DRM_IOCTL_IVPU_BO_CREATE");
        return ret;
    }

    handle = args.handle;
    vpuAddr = args.vpu_addr;
    return ret;
}

int VPUDriverApi::getBufferInfo(uint32_t handle, uint64_t &mmap_offset) const {
    drm_ivpu_bo_info args = {};
    args.handle = handle;

    int ret = doIoctl(DRM_IOCTL_IVPU_BO_INFO, &args);
    if (ret) {
        LOG_E("Failed to call DRM_IOCTL_IVPU_BO_INFO");
        return ret;
    }

    mmap_offset = args.mmap_offset;
    return ret;
}

int VPUDriverApi::getExtBufferInfo(uint32_t handle,
                                   uint32_t &flags,
                                   uint64_t &vpu_address,
                                   uint64_t &size,
                                   uint64_t &mmap_offset) const {
    drm_ivpu_bo_info args = {};
    args.handle = handle;

    int ret = doIoctl(DRM_IOCTL_IVPU_BO_INFO, &args);
    if (ret) {
        LOG_E("Failed to call DRM_IOCTL_IVPU_BO_INFO");
        return ret;
    }

    flags = args.flags;
    vpu_address = args.vpu_addr;
    size = args.size;
    mmap_offset = args.mmap_offset;
    return ret;
}

std::string VPUDriverApi::getFWComponentVersion(uint32_t componentVerIndex) {
    std::string version;

    try {
        uint32_t rev = getDeviceParam<uint32_t>(DRM_IVPU_PARAM_FW_API_VERSION, componentVerIndex);
        version += std::to_string(rev >> 16) + "." + std::to_string(rev & 0xFFFF);
    } catch (std::exception &e) {
        version = "not available";
    }
    return version;
}

int VPUDriverApi::exportBuffer(uint32_t handle, uint32_t flags, int32_t &fd) const {
    drm_prime_handle args = {.handle = handle, .flags = flags, .fd = -1};

    int ret = doIoctl(DRM_IOCTL_PRIME_HANDLE_TO_FD, &args);
    if (ret) {
        LOG_E("Failed to call DRM_IOCTL_PRIME_HANDLE_TO_FD");
        return ret;
    }

    fd = args.fd;
    return ret;
}

int VPUDriverApi::importBuffer(int32_t fd, uint32_t flags, uint32_t &handle) const {
    drm_prime_handle args = {.handle = 0, .flags = flags, .fd = fd};

    int ret = doIoctl(DRM_IOCTL_PRIME_FD_TO_HANDLE, &args);
    if (ret) {
        LOG_E("Failed to call DRM_IOCTL_PRIME_FD_TO_HANDLE");
        return ret;
    }

    handle = args.handle;
    return ret;
}

void *VPUDriverApi::mmap(size_t size, off_t offset) const {
    TRACE_EVENT("SYS", "mmap");
    void *ptr = osInfc.osiMmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, vpuFd, offset);
    if (ptr == MAP_FAILED) {
        LOG_E("Failed to mmap the memory using offset received from KMD");
        return nullptr;
    }

    return ptr;
}

int VPUDriverApi::unmap(void *ptr, size_t size) const {
    TRACE_EVENT("SYS", "munmap");
    return osInfc.osiMunmap(ptr, size);
}

int VPUDriverApi::metricStreamerStart(drm_ivpu_metric_streamer_start *startData) const {
    return doIoctl(DRM_IOCTL_IVPU_METRIC_STREAMER_START, startData);
}

int VPUDriverApi::metricStreamerStop(drm_ivpu_metric_streamer_stop *stopData) const {
    return doIoctl(DRM_IOCTL_IVPU_METRIC_STREAMER_STOP, stopData);
}

int VPUDriverApi::metricStreamerGetData(drm_ivpu_metric_streamer_get_data *data) const {
    return doIoctl(DRM_IOCTL_IVPU_METRIC_STREAMER_GET_DATA, data);
}

int VPUDriverApi::metricStreamerGetInfo(drm_ivpu_metric_streamer_get_data *data) const {
    return doIoctl(DRM_IOCTL_IVPU_METRIC_STREAMER_GET_INFO, data);
}

std::string VPUDriverApi::getDeviceLink() {
    constexpr size_t len = 256;
    char devChar[len] = {0};
    char devLink[len] = {0};
    struct stat st;

    if (::fstat(vpuFd, &st)) {
        LOG_E("Failed to ::fstat");
        return {};
    }

    snprintf(devChar, len, "/sys/dev/char/%d:%d", major(st.st_rdev), minor(st.st_rdev));

    ssize_t readLen = ::readlink(devChar, devLink, len);
    if (readLen < 0) {
        LOG_E("Failed to ::readlink");
        return {};
    }
    devLink[len - 1] = '\0';

    LOG(DEVICE, "Device path: %s", devChar);
    LOG(DEVICE, "Device path link: %s", devLink);

    return {devLink};
}

std::string VPUDriverApi::getSysDeviceAbsolutePath() {
    std::string path("/sys/dev/char/");
    return path + getDeviceLink() + "/../../";
}

} // namespace VPU
