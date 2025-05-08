/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/os_interface/os_interface_imp.hpp"

#include <stdint.h>

#include "umd_common.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <memory>
#include <string.h>
#include <string>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

namespace VPU {

OsInterface *OsInterfaceImp::getInstance() {
    static OsInterfaceImp instance;
    return &instance;
}

int OsInterfaceImp::osiOpen(const char *pathname, int flags, mode_t mode) {
    struct stat fstatInfo = {};
    int fd;

    LOG(FSYS, "Trying to open file '%s'.", pathname);
    if ((fd = open(pathname, flags, mode)) == -1) {
        LOG(FSYS, "Failed to open %s, errno: %u (%s)", pathname, errno, strerror(errno));
        return -1;
    }

    if (fstat(fd, &fstatInfo) != 0) {
        LOG_E("Failed to fstat on file %s", pathname);
        close(fd);
        return -1;
    }

    if (!S_ISCHR(fstatInfo.st_mode)) {
        LOG_E("Failed to use %s because file is not a device file", pathname);
        close(fd);
        return -1;
    }

    if (fstatInfo.st_nlink > 1) {
        LOG_E("Failed to use %s because file might be a hard link", pathname);
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

int OsInterfaceImp::osiIoctl(int fd, unsigned int request, void *args) {
    return ioctl(fd, request, args);
}

size_t OsInterfaceImp::osiGetSystemPageSize() {
    return safe_cast<size_t>(sysconf(_SC_PAGESIZE));
}

std::string OsInterfaceImp::osiReadFile(const std::filesystem::path &path, size_t maxReadSize) {
    std::string out(maxReadSize + 1, 0);
    int fd = ::open(path.c_str(), O_RDONLY | O_CLOEXEC | O_NOFOLLOW);
    if (fd == -1) {
        LOG_E("Failed to open %s, errno: %u (%s)", path.c_str(), errno, strerror(errno));
        return "";
    }
    off_t off = ::read(fd, out.data(), maxReadSize);
    ::close(fd);
    if (off == -1) {
        LOG_E("Failed to read, errno: %u (%s)", errno, strerror(errno));
        return "";
    }
    return out;
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

bool OsInterfaceImp::osiFileRemove(const std::filesystem::path &path) {
    std::error_code ec;
    if (!std::filesystem::remove(path, ec)) {
        LOG_E("Failed to remove file, ec: %i (%s)", ec.value(), ec.message().c_str());
        return false;
    }
    return true;
}

class OsFileImp : public OsFile {
  public:
    OsFileImp(const std::filesystem::path &path, bool writeAccess)
        : writeAccess(writeAccess) {
        int flags = O_RDONLY;
        if (writeAccess) {
            flags = O_CREAT | O_RDWR | O_TRUNC;
        }

        fd = ::open(path.c_str(), flags | O_CLOEXEC | O_NOFOLLOW, S_IRUSR | S_IRGRP);
        if (fd == -1) {
            LOG(FSYS,
                "Failed to open file %s, errno: %u (%s)",
                path.c_str(),
                errno,
                strerror(errno));
            return;
        }

        struct stat fstatInfo = {};
        if (fstat(fd, &fstatInfo) != 0 || !S_ISREG(fstatInfo.st_mode) || fstatInfo.st_nlink > 1) {
            LOG_E("Invalid file %s", path.c_str());
            close(fd);
            fd = -1;
            return;
        }

        fileSize = safe_cast<size_t>(fstatInfo.st_size);
        LOG(FSYS, "OsFileImp - path: %p, fd: %i, fileSize: %lu", path.c_str(), fd, fileSize);
    }

    ~OsFileImp() override {
        if (mmapPtr != MAP_FAILED)
            ::munmap(mmapPtr, fileSize);

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

    bool write(const void *in, size_t size) override {
        if (in == nullptr || size == 0) {
            LOG_E("Invalid pointer or size");
            return false;
        }

        size_t offset = 0;
        size_t remaining = size;
        while (remaining > 0) {
            ssize_t written = ::write(fd, static_cast<const uint8_t *>(in) + offset, remaining);
            if (written == -1) {
                LOG_E("Failed to write, errno: %u (%s)", errno, strerror(errno));
                return false;
            }

            offset += static_cast<size_t>(written);
            remaining -= static_cast<size_t>(written);
        }

        fileSize += offset;
        return remaining == 0;
    }

    void *mmap() override {
        if (writeAccess) {
            LOG(FSYS, "File %d cannot be mapped in write access", fd);
            return nullptr;
        }

        if (mmapPtr != MAP_FAILED) {
            LOG(FSYS, "File %d already mapped, mmapPtr: %p", fd, mmapPtr);
            return mmapPtr;
        }

        mmapPtr = ::mmap(NULL, fileSize, PROT_READ, MAP_SHARED, fd, 0);
        if (mmapPtr == MAP_FAILED) {
            LOG_E("Failed to map the file, errno: %u (%s)", errno, strerror(errno));
            return nullptr;
        }

        LOG(FSYS, "File %d is mapped, mmapPtr: %p", fd, mmapPtr);
        return mmapPtr;
    }

    size_t size() override { return fileSize; }

  private:
    bool writeAccess;
    int fd;
    void *mmapPtr = MAP_FAILED;
    size_t fileSize = 0;
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
