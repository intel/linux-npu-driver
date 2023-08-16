/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <sys/mman.h>
#include <thread>
#include <uapi/drm/ivpu_accel.h>
#include <vector>

#include <boost/numeric/conversion/cast.hpp>

namespace VPU {

VPUDriverApi::VPUDriverApi(std::string devnode, OsInterface &osInfc)
    : devnode(devnode)
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
    auto driverApi = std::make_unique<VPUDriverApi>(devnode, osInfc);

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

    if (kmdVersion != umdIoctlDeviceName) {
        LOG_E("IOCTL device name doesn't match(UMD: %s, KMD: %s)",
              umdIoctlDeviceName,
              kmdVersion.c_str());
        return false;
    }

    return true;
}

int VPUDriverApi::submitCommandBuffer(drm_ivpu_submit *arg) const {
    return doIoctl(DRM_IOCTL_IVPU_SUBMIT, arg);
}

int VPUDriverApi::getDeviceParam(drm_ivpu_param *arg) const {
    return doIoctl(DRM_IOCTL_IVPU_GET_PARAM, arg);
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

void *VPUDriverApi::alloc(size_t size) const {
    return osInfc.osiAlloc(size);
}

int VPUDriverApi::free(void *ptr) const {
    return osInfc.osiFree(ptr);
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
        closeBuffer(handle);
        return ret;
    }

    mmap_offset = args.mmap_offset;
    return ret;
}

void *VPUDriverApi::mmap(size_t size, uint64_t offset) const {
    void *ptr = osInfc.osiMmap(nullptr,
                               size,
                               PROT_READ | PROT_WRITE,
                               MAP_SHARED,
                               vpuFd,
                               boost::numeric_cast<off_t>(offset));
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
