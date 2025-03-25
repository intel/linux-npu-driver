/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <sys/types.h>

struct stat;

namespace VPU {

class OsFile {
  public:
    virtual ~OsFile() = default;

    virtual bool write(const void *in, size_t size) = 0;
    virtual void *mmap() = 0;
    virtual size_t size() = 0;
};

class OsInterface {
  public:
    virtual ~OsInterface() = default;

    virtual int osiOpen(const char *pathname, int flags, mode_t mode) = 0;
    virtual int osiClose(int fd) = 0;
    virtual int osiFcntl(int fd, int cmd) = 0;
    virtual int osiIoctl(int fd, unsigned int request, void *arg) = 0;

    virtual size_t osiGetSystemPageSize() = 0;

    virtual void *osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) = 0;
    virtual int osiMunmap(void *addr, size_t size) = 0;

    virtual std::string osiReadFile(const std::filesystem::path &path,
                                    size_t maxReadSize = 255) = 0;
    virtual bool osiCreateDirectories(const std::filesystem::path &path) = 0;
    virtual std::unique_ptr<OsFile> osiOpenWithExclusiveLock(const std::filesystem::path &path,
                                                             bool writeAccess) = 0;
    virtual std::unique_ptr<OsFile> osiOpenWithSharedLock(const std::filesystem::path &path,
                                                          bool writeAccess) = 0;
    virtual void osiScanDir(const std::filesystem::path &path,
                            std::function<void(const char *name, struct ::stat &stat)> f) = 0;
    virtual bool osiFileRemove(const std::filesystem::path &path) = 0;
};

OsInterface *getOsInstance();

} // namespace VPU
