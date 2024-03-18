/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"

#include <cerrno>
#include <string.h>
#include <fcntl.h>
#include <memory>
#include <sys/mman.h>
#include <uapi/drm/ivpu_accel.h>

namespace VPU {

VPUDriverApi::VPUDriverApi(std::string devnode, OsInterface &osInfc)
    : devnode(std::move(devnode))
    , osInfc(osInfc)
    , vpuFd(-1) {}

VPUDriverApi::~VPUDriverApi() {
    if (vpuFd > 0)
        closeDevice();
}

VPUDriverApi::VPUDriverApi(VPUDriverApi &&v)
    : devnode(v.devnode)
    , osInfc(v.osInfc)
    , vpuFd(v.vpuFd) {
    v.vpuFd = -1;
}

int VPUDriverApi::doIoctl(unsigned long request, void *arg) const {
    if (vpuFd < 0 || arg == nullptr) {
        LOG_E("Invalid arguments (vpuFd:%d, arg:%p)", vpuFd, arg);
        return -EINVAL;
    }

    LOG_V("Start IOCTL request %#lx", request);
    int ret;
    do {
        ret = osInfc.osiIoctl(vpuFd, request, arg);
    } while (ret == -1 && (errno == -EAGAIN || errno == -EINTR));

    LOG_V("End IOCTL request %#lx: ret=%d", request, ret);
    if (ret != 0)
        LOG_V("IOCTL ERRNO=%d, STRERROR=\"%s\"", errno, strerror(errno));

    return ret;
}

std::unique_ptr<VPUDriverApi> VPUDriverApi::openDriverApi(std::string devnode,
                                                          OsInterface &osInfc) {
    auto driverApi = std::make_unique<VPUDriverApi>(std::move(devnode), osInfc);

    if (!driverApi->openDevice())
        return nullptr;

    return driverApi;
}

bool VPUDriverApi::openDevice() {
    vpuFd = osInfc.osiOpen(devnode.c_str(), (O_RDWR | O_CLOEXEC), S_IRUSR | S_IWUSR);
    if (vpuFd < 0)
        LOG_V("Failed to open '%s'", devnode.c_str());
    return vpuFd >= 0;
}

bool VPUDriverApi::closeDevice() {
    int ret = 0;

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
        LOG_I("Failed to get API version ioctl(ret: %d).", ret);
        return false;
    }

    if (umdIoctlVersionMajor != version.version_major) {
        LOG_I("IOCTL version doesn't match! (UMD: %d.%d, KMD: %d.%d)",
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
        LOG_I("Failed to get API version ioctl(ret: %d).", ret);
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

int VPUDriverApi::submitCommandBuffer(drm_ivpu_submit *arg) const {
    return doIoctl(DRM_IOCTL_IVPU_SUBMIT, arg);
}

bool VPUDriverApi::checkDeviceCapability(uint32_t index) const {
    struct drm_ivpu_param arg = {};
    arg.param = DRM_IVPU_PARAM_CAPABILITIES;
    arg.index = index;
    if (doIoctl(DRM_IOCTL_IVPU_GET_PARAM, &arg)) {
        LOG_W("Capability does not exist, index: %#x, errno: %d", index, errno);
        return false;
    }

    if (arg.value == 0) {
        LOG_W("Capability from index: %#x is not set", index);
        return false;
    }

    LOG_I("Capability from index: %#x is set", index);
    return true;
}

bool VPUDriverApi::checkDeviceStatus() const {
    return osInfc.osiFcntl(vpuFd, F_GETFL) >= 0;
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
            LOG_E("Buffer size is too big.");
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
    void *ptr = osInfc.osiMmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, vpuFd, offset);
    if (ptr == MAP_FAILED) {
        LOG_E("Failed to mmap the memory using offset received from KMD");
        return nullptr;
    }

    return ptr;
}

int VPUDriverApi::unmap(void *ptr, size_t size) const {
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

} // namespace VPU
