/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

namespace VPU {

/**
 * OS file system interface.
 *
 * File system interfacing methods will get concrete class of this interface and also
 * useful for Mocking OS layer.
 */
class OsInterface {
  public:
    virtual ~OsInterface() {}

    virtual int osiOpen(const char *pathname, int flags, mode_t mode) = 0;
    virtual int osiClose(int fildes) = 0;
    virtual int osiFcntl(int fd, int cmd) = 0;
    virtual int osiIoctl(int fd, unsigned long request, void *arg) = 0;

    virtual void *osiAlloc(size_t size) = 0;
    virtual int osiFree(void *ptr) = 0;
    virtual size_t osiGetSystemPageSize() = 0;

    virtual void *osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) = 0;
    virtual int osiMunmap(void *addr, size_t size) = 0;

    virtual bool fileExists(std::string &p) = 0;
};

} // namespace VPU
