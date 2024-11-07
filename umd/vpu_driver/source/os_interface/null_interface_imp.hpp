/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>

#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <sys/types.h>

struct drm_ivpu_param;

namespace VPU {

class NullOsInterfaceImp : public OsInterface {
  public:
    static OsInterface &getInstance();
    static bool isNullDeviceRequested();

    int osiOpen(const char *pathname, int flags, mode_t mode) override;
    int osiClose(int fd) override;
    int osiFcntl(int fd, int cmd) override;
    int osiIoctl(int fd, unsigned long request, void *arg) override;

    size_t osiGetSystemPageSize() override;
    void *osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) override;
    int osiMunmap(void *addr, size_t size) override;

    std::string osiReadFile(const std::filesystem::path &path, size_t maxReadSize = 255) override;
    bool osiCreateDirectories(const std::filesystem::path &path) override;

    std::unique_ptr<OsFile> osiOpenWithExclusiveLock(const std::filesystem::path &path,
                                                     bool writeAccess) override;
    std::unique_ptr<OsFile> osiOpenWithSharedLock(const std::filesystem::path &path,
                                                  bool writeAccess) override;
    void osiScanDir(const std::filesystem::path &path,
                    std::function<void(const char *name, struct stat &stat)> f) override;
    bool osiFileRemove(const std::filesystem::path &path) override;

  private:
    VPUHwInfo nullHwInfo;
    NullOsInterfaceImp() = default;

    int getParamValue(drm_ivpu_param &p);

    uint32_t unique_id = 0;
    uint64_t deviceAddress = 0;
};

} // namespace VPU
