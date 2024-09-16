/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstdint>

#include "vpu_driver/source/os_interface/os_interface.hpp"

#include <bitset>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <sys/types.h>

namespace VPU {
class MockOsInterfaceImp : public OsInterface {
  public:
    // Operation flags.
    bool openSuccessful = true;

    // Device is connected.
    bool deviceConnected = true;

    // Device ID
    uint32_t pciDevId = 0;

    uint32_t callCntAlloc = 0;
    uint32_t callCntFree = 0;
    uint32_t callCntIoctl = 0;
    uint32_t callCntSubmit = 0;

    unsigned long ioctlLastCommand = 0;
    int fd = 3;
    const uint64_t deviceLowBaseAddress = 0xc000'0000;
    uint64_t deviceAddress = deviceLowBaseAddress;
    uint64_t unique_id = 0;

    int32_t kmdApiVersionMajor = 1;
    int32_t kmdApiVersionMinor = 0;
    std::string kmdApiDeviceName = "intel_vpu";

    int kmdIoctlRetCode = 0;

    MockOsInterfaceImp(uint32_t pciDevId = 0x7d1d);
    MockOsInterfaceImp(const MockOsInterfaceImp &) = delete;
    MockOsInterfaceImp &operator=(const MockOsInterfaceImp &) = delete;
    MockOsInterfaceImp(MockOsInterfaceImp &&) = delete;
    MockOsInterfaceImp &operator=(MockOsInterfaceImp &&) = delete;

    // File operations.
    int osiOpen(const char *pathname, int flags, mode_t mode) override;
    int osiClose(int fildes) override;
    int osiFcntl(int fd, int cmd) override;
    int osiIoctl(int fd, unsigned long request, void *args) override;

    size_t osiGetSystemPageSize() override;

    void *osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) override;
    int osiMunmap(void *addr, size_t size) override;

    bool osiCreateDirectories(const std::filesystem::path &path) override;
    std::unique_ptr<OsFile> osiOpenWithExclusiveLock(const std::filesystem::path &path,
                                                     bool writeAccess) override;
    std::unique_ptr<OsFile> osiOpenWithSharedLock(const std::filesystem::path &path,
                                                  bool writeAccess) override;
    void osiScanDir(const std::filesystem::path &path,
                    std::function<void(const char *name, struct stat &stat)> f) override;

    void mockFailNextAlloc(); // Fails next call to osiMmap
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
