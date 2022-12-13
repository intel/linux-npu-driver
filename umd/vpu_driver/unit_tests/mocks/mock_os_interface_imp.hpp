/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "umd_common.hpp"

#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "vpu_driver/source/device/vpu_device.hpp"
#include <bitset>
#include <cstdint>
#include <memory.h>
#include <string>
#include <uapi/drm/ivpu_drm.h>

namespace VPU {
class MockOsInterfaceImp : public OsInterface {
  public:
    // Operation flags.
    bool openSuccessful = true;

    // Device is connected.
    bool deviceConnected = true;

    // Device ID
    uint16_t pciDevId = 0u;

    uint32_t callCntAlloc = 0;
    uint32_t callCntFree = 0;
    uint32_t callCntIoctl = 0;

    unsigned long ioctlLastCommand = 0;
    uint32_t host_ssid = 3u;
    const uint64_t deviceLowBaseAddress = 0xc000'0000;
    uint64_t deviceAddress = deviceLowBaseAddress;
    uint64_t unique_id = 0;

    int32_t kmdApiVersionMajor = DRM_IVPU_DRIVER_MAJOR;
    int32_t kmdApiVersionMinor = DRM_IVPU_DRIVER_MINOR;
    std::string kmdApiDeviceName = "intel_vpu";

    int kmdIoctlRetCode = 0;

    MockOsInterfaceImp(uint16_t pciDevId = MTL_VPU_PCI_DEVICE_ID);
    MockOsInterfaceImp(const MockOsInterfaceImp &) = delete;
    MockOsInterfaceImp &operator=(const MockOsInterfaceImp &) = delete;
    MockOsInterfaceImp(MockOsInterfaceImp &&) = delete;
    MockOsInterfaceImp &operator=(MockOsInterfaceImp &&) = delete;

    // File operations.
    int osiOpen(const char *pathname, int flags, mode_t mode) override;
    int osiClose(int fildes) override;
    int osiFcntl(int fd, int cmd) override;
    int osiIoctl(int fd, unsigned long request, void *args) override;

    void *osiAlloc(size_t size) override;
    int osiFree(void *ptr) override;
    size_t osiGetSystemPageSize() override;

    void *osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) override;
    int osiMunmap(void *addr, size_t size) override;

    bool fileExists(std::string &p) override;

    void mockFailNextAlloc(); // Fails next call to osiAlloc
    void mockFailNextJobWait();
    void mockSuccessNextJobWait();
    void mockFailNextJobStatus();
    void mockSuccessNextJobStatus();

  private:
    bool failNextAlloc = false;
    std::bitset<8> waitFailed = {};
    std::bitset<8> jobFailed = {};
};

} // namespace VPU
