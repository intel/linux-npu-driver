/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstdint>

#include "umd_common.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <errno.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <uapi/drm/ivpu_accel.h>

namespace VPU {

class OsInterface;

/**
 * VPUDriverApi
 * Interface between upper layer L0 calls and KMD.
 */
class VPUDriverApi final {
  public:
    VPUDriverApi(std::string devPath, OsInterface &osInfc);
    VPUDriverApi(VPUDriverApi &&v);
    ~VPUDriverApi();

    VPUDriverApi(const VPUDriverApi &) = delete;
    VPUDriverApi &operator=(const VPUDriverApi &) = delete;
    VPUDriverApi &&operator=(VPUDriverApi &&v) = delete;

    static std::unique_ptr<VPUDriverApi> openDriverApi(std::string devPath, OsInterface &osInfc);

    int getFd() const { return vpuFd; }
    bool isVpuDevice() const;
    int submitCommandBuffer(drm_ivpu_submit *arg) const;
    bool checkDeviceCapability(uint32_t index) const;
    bool checkPrimeBuffersCapability() const;
    size_t getPageSize() const;
    std::string getDeviceLink();

    int wait(void *args) const;
    int closeBuffer(uint32_t handle) const;

    int createBuffer(size_t size, uint32_t flags, uint32_t &handle, uint64_t &vpuAddr) const;
    int getBufferInfo(uint32_t handle, uint64_t &mmap_offset) const;
    int getExtBufferInfo(uint32_t handle,
                         uint32_t &flags,
                         uint64_t &vpu_address,
                         uint64_t &size,
                         uint64_t &mmap_offset) const;
    std::string getFWComponentVersion(uint32_t componentVerIndex);
    int exportBuffer(uint32_t handle, uint32_t flags, int32_t &fd) const;
    int importBuffer(int32_t fd, uint32_t flags, uint32_t &handle) const;
    void *mmap(size_t size, off_t offset) const;
    int unmap(void *ptr, size_t size) const;

    int metricStreamerStart(drm_ivpu_metric_streamer_start *startData) const;
    int metricStreamerStop(drm_ivpu_metric_streamer_stop *stopData) const;
    int metricStreamerGetData(drm_ivpu_metric_streamer_get_data *data) const;
    int metricStreamerGetInfo(drm_ivpu_metric_streamer_get_data *data) const;

    template <typename T = uint64_t>
    T getDeviceParam(uint32_t param, uint32_t index = 0) const {
        struct drm_ivpu_param arg = {};
        arg.param = param;
        arg.index = index;
        if (doIoctl(DRM_IOCTL_IVPU_GET_PARAM, &arg)) {
            LOG_E("Failed to read device param, param: %#x, errno: %d", param, errno);
            throw std::runtime_error("Failed to get device param");
        }
        if constexpr (std::is_same_v<T, uint64_t>)
            return arg.value;
        return safe_cast<T>(arg.value);
    }

  private:
    bool openDevice();
    bool closeDevice();
    int doIoctl(unsigned long request, void *arg) const;

    const static int32_t umdIoctlVersionMajor = DRM_IVPU_DRIVER_MAJOR;
    const static int32_t umdIoctlVersionMinor = DRM_IVPU_DRIVER_MINOR;
    constexpr static char const *umdIoctlDeviceName1 = "intel_vpu";
    constexpr static char const *umdIoctlDeviceName2 = "intel_npu";

    std::string devPath;
    OsInterface &osInfc;
    int vpuFd;
};
} // namespace VPU
