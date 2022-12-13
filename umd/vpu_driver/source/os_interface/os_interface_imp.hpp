/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/os_interface/os_interface.hpp"

namespace VPU {

class OsInterfaceImp : public OsInterface {
  public:
    OsInterfaceImp() = default;

    OsInterfaceImp(const OsInterfaceImp &) = delete;
    OsInterfaceImp &operator=(const OsInterfaceImp &) = delete;
    OsInterfaceImp(OsInterfaceImp &&) = delete;
    OsInterfaceImp &&operator=(OsInterfaceImp &&) = delete;

    static OsInterfaceImp &getInstance();

    int osiOpen(const char *pathname, int flags, mode_t mode) override;
    int osiClose(int fildes) override;
    int osiFcntl(int fd, int cmd) override;
    int osiIoctl(int fd, unsigned long request, void *arg) override;

    void *osiAlloc(size_t size) override;
    int osiFree(void *ptr) override;
    size_t osiGetSystemPageSize() override;

    void *osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) override;
    int osiMunmap(void *addr, size_t size) override;

    bool fileExists(std::string &p) override;

  private:
    /**
     * Does extra check for secure file open to avoid TOCTOU.
     * Non-regular files will be failed to be opened.
     */
    static int secureOpen(const char *pathname, int flags, mode_t mode = 0);
};

} // namespace VPU
