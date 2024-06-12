/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/os_interface/os_interface.hpp"

namespace VPU {

class OsInterfaceImp : public OsInterface {
  private:
    OsInterfaceImp() = default;

  public:
    static OsInterface &getInstance();

    int osiOpen(const char *pathname, int flags, mode_t mode) override;
    int osiClose(int fd) override;
    int osiFcntl(int fd, int cmd) override;
    int osiIoctl(int fd, unsigned long request, void *arg) override;

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
};

} // namespace VPU
