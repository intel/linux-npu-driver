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
  private:
    OsInterfaceImp() = default;

  public:
    static OsInterfaceImp &getInstance();

    int osiOpen(const char *pathname, int flags, mode_t mode) override;
    int osiClose(int fd) override;
    int osiFcntl(int fd, int cmd) override;
    int osiIoctl(int fd, unsigned long request, void *arg) override;

    size_t osiGetSystemPageSize() override;

    void *osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) override;
    int osiMunmap(void *addr, size_t size) override;
};

} // namespace VPU
