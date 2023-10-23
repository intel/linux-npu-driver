/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/os_interface/os_interface_imp.hpp"

#include <chrono>
#include <drm/drm.h>
#include <string>
#include <memory>
#include <cstdint>
#include <uapi/drm/ivpu_accel.h>

namespace VPU {

class OsInterface;

/**
 * VPUDriverApi
 * Interface between upper layer L0 calls and KMD.
 */
class VPUDriverApi final {
  public:
    VPUDriverApi(std::string devnode, OsInterface &osInfc);
    VPUDriverApi(VPUDriverApi &&v);
    ~VPUDriverApi();

    VPUDriverApi(const VPUDriverApi &) = delete;
    VPUDriverApi &operator=(const VPUDriverApi &) = delete;
    VPUDriverApi &&operator=(VPUDriverApi &&v) = delete;

    static std::unique_ptr<VPUDriverApi> openDriverApi(std::string devnode, OsInterface &osInfc);

    int getFd() const { return vpuFd; }
    bool isVpuDevice() const;
    int submitCommandBuffer(drm_ivpu_submit *arg) const;
    int getDeviceParam(drm_ivpu_param *arg) const;
    bool checkDeviceStatus() const;
    size_t getPageSize() const;

    void *alloc(size_t size) const;
    int free(void *ptr) const;

    int wait(void *args) const;
    int closeBuffer(uint32_t handle) const;

    int createBuffer(size_t size, uint32_t flags, uint32_t &handle, uint64_t &vpuAddr) const;
    int getBufferInfo(uint32_t handle, uint64_t &mmap_offset) const;
    void *mmap(size_t size, off_t offset) const;
    int unmap(void *ptr, size_t size) const;

    int metricStreamerStart(drm_ivpu_metric_streamer_start *startData) const;
    int metricStreamerStop(drm_ivpu_metric_streamer_stop *stopData) const;
    int metricStreamerGetData(drm_ivpu_metric_streamer_get_data *data) const;
    int metricStreamerGetInfo(drm_ivpu_metric_streamer_get_data *data) const;

  private:
    bool openDevice();
    bool closeDevice();
    int doIoctl(unsigned long request, void *arg) const;

    const static int32_t umdIoctlVersionMajor = DRM_IVPU_DRIVER_MAJOR;
    const static int32_t umdIoctlVersionMinor = DRM_IVPU_DRIVER_MINOR;
    constexpr static char const *umdIoctlDeviceName1 = "intel_vpu";
    constexpr static char const *umdIoctlDeviceName2 = "intel_npu";

    std::string devnode;
    OsInterface &osInfc;
    int vpuFd;
};
} // namespace VPU
