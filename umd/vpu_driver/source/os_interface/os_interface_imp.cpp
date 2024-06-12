/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "vpu_driver/source/os_interface/os_interface_imp.hpp"

#include "umd_common.hpp"

#include <dirent.h>
#include <fcntl.h>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace VPU {

OsInterface &OsInterfaceImp::getInstance() {
    static OsInterfaceImp instance;
    return instance;
}

int OsInterfaceImp::osiOpen(const char *pathname, int flags, mode_t mode) {
    struct stat fstatInfo = {};
    int fd;

    LOG(FSYS, "Trying to open file '%s'.", pathname);
    if ((fd = open(pathname, flags, mode)) == -1) {
        LOG(FSYS, "Failed to open file '%s'.", pathname);
        return -1;
    }

    if (fstat(fd, &fstatInfo) != 0) {
        LOG_E("Failed to get file information. Closing");
        close(fd);
        return -1;
    }

    if (!S_ISCHR(fstatInfo.st_mode) || S_ISLNK(fstatInfo.st_mode)) {
        LOG_E("Open file is not the expected device file. Closing");
        close(fd);
        return -1;
    }

    LOG(FSYS, "Returning file descriptor %d", fd);
    return fd;
}

int OsInterfaceImp::osiClose(int fd) {
    return close(fd);
}

int OsInterfaceImp::osiFcntl(int fd, int cmd) {
    return fcntl(fd, cmd);
}

int OsInterfaceImp::osiIoctl(int fd, unsigned long request, void *args) {
    return ioctl(fd, request, args);
}

size_t OsInterfaceImp::osiGetSystemPageSize() {
    return safe_cast<size_t>(sysconf(_SC_PAGESIZE));
}

void *OsInterfaceImp::osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) {
    return mmap(addr, size, prot, flags, fd, offset);
}

int OsInterfaceImp::osiMunmap(void *addr, size_t size) {
    return munmap(addr, size);
}

bool OsInterfaceImp::osiCreateDirectories(const std::filesystem::path &path) {
    std::error_code ec;
    if (!std::filesystem::create_directories(path, ec) && ec) {
        LOG_W("Failed to create directories, path: %s, ec: %d (%s)",
              path.c_str(),
              ec.value(),
              ec.message().c_str());
        return false;
    }

    return true;
}

class OsFileImp : public OsFile {
  public:
    OsFileImp(const std::filesystem::path &path, bool writeAccess)
        : path(path) {
        int flags = O_RDONLY;
        if (writeAccess) {
            flags = O_CREAT | O_RDWR;
        }

        fd = ::open(path.c_str(), flags | O_CLOEXEC, S_IRUSR | S_IRGRP);
        if (fd == -1) {
            LOG(FSYS, "Failed to open file, errno: %u (%s)", errno, strerror(errno));
            return;
        }

        struct stat fstatInfo = {};
        if (fstat(fd, &fstatInfo) != 0 || !S_ISREG(fstatInfo.st_mode)) {
            LOG_E("Invalid file");
            close(fd);
            fd = -1;
            return;
        }
    }

    ~OsFileImp() override {
        if (isOpen())
            ::close(fd);
    }

    OsFileImp(const OsFileImp &) = delete;
    OsFileImp(OsFileImp &&) = delete;
    OsFileImp &operator=(const OsFileImp &) = delete;
    OsFileImp &operator=(OsFileImp &&) = delete;

    bool isOpen() const { return fd != -1; }
    bool exclusiveLock() const {
        int ret = ::flock(fd, LOCK_EX | LOCK_NB);
        if (ret == 0)
            return true;

        LOG(FSYS, "Failed get exclusive lock, errno: %u (%s)", errno, strerror(errno));
        return false;
    }

    bool sharedLock() const {
        int ret = ::flock(fd, LOCK_SH | LOCK_NB);
        if (ret == 0)
            return true;

        LOG(FSYS, "Failed to get shared lock, errno: %u, (%s)", errno, strerror(errno));
        return false;
    }

    bool read(void *out, size_t size) override {
        if (out == nullptr || size == 0) {
            LOG_E("Invalid pointer or size");
            return false;
        }

        if (!setOffsetAtZero())
            return false;

        size_t at = 0;
        while (at < size) {
            off_t off = ::read(fd, static_cast<uint8_t *>(out) + at, size - at);
            if (off == -1 || off == 0) {
                LOG_E("Failed to read, errno: %u (%s)", errno, strerror(errno));
                return false;
            }

            at += static_cast<size_t>(off);
        }

        if (at != size)
            return false;
        return true;
    }

    bool write(const void *in, size_t size) override {
        if (in == nullptr || size == 0) {
            LOG_E("Invalid pointer or size");
            return false;
        }

        if (!setOffsetAtZero())
            return false;

        size_t at = 0;
        while (at < size) {
            off_t off = ::write(fd, static_cast<const uint8_t *>(in) + at, size - at);
            if (off == -1) {
                LOG_E("Failed to write, errno: %u (%s)", errno, strerror(errno));
                return false;
            }

            at += static_cast<size_t>(off);
        }

        if (at != size)
            return false;
        return true;
    }

    bool remove() override {
        std::error_code ec;
        std::filesystem::remove(path, ec);
        if (ec) {
            LOG_E("Failed to remove file, ec: %i (%s)", ec.value(), ec.message().c_str());
            return false;
        }
        return true;
    }

    size_t size() override { return std::filesystem::file_size(path); }

  private:
    bool setOffsetAtZero() {
        off_t off = lseek(fd, 0, SEEK_SET);
        if (off != 0) {
            LOG_E("Failed to set file offset, errno: %u (%s)", errno, strerror(errno));
            return false;
        }

        return true;
    }

    std::filesystem::path path;
    int fd;
};

std::unique_ptr<OsFile> OsInterfaceImp::osiOpenWithExclusiveLock(const std::filesystem::path &path,
                                                                 bool writeAccess) {
    auto file = std::make_unique<OsFileImp>(path, writeAccess);
    if (!file->isOpen() || !file->exclusiveLock()) {
        return nullptr;
    }
    return file;
}

std::unique_ptr<OsFile> OsInterfaceImp::osiOpenWithSharedLock(const std::filesystem::path &path,
                                                              bool writeAccess) {
    auto file = std::make_unique<OsFileImp>(path, writeAccess);
    if (!file->isOpen() || !file->sharedLock()) {
        return nullptr;
    }
    return file;
}

void OsInterfaceImp::osiScanDir(const std::filesystem::path &path,
                                std::function<void(const char *name, struct stat &stat)> f) {
    DIR *dir = opendir(path.c_str());
    if (dir == nullptr) {
        LOG_E("Failed to open %s, errno: %u (%s)", path.c_str(), errno, strerror(errno));
        return;
    }

    const int dirFd = dirfd(dir);
    for (struct dirent *dirent = readdir(dir); dirent != nullptr; dirent = readdir(dir)) {
        if (dirent->d_type != DT_REG)
            continue;

        /* Posix struct stat is used because std::filesystem only show timestamp of last write */
        struct stat sb;
        if (fstatat(dirFd, dirent->d_name, &sb, 0) != 0)
            continue;

        f(dirent->d_name, sb);
    }

    closedir(dir);
}

} // namespace VPU
