/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "vpu_driver/source/os_interface/os_interface_imp.hpp"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <memory>
#include <filesystem>
#include <string>

#include <boost/numeric/conversion/cast.hpp>

namespace VPU {

OsInterfaceImp &OsInterfaceImp::getInstance() {
    static OsInterfaceImp instance;
    return instance;
}

int OsInterfaceImp::secureOpen(const char *pathname, int flags, mode_t mode) {
    struct stat lstatInfo = {};
    struct stat fstatInfo = {};
    int fd;

    LOG_V("Trying to open file '%s'.", pathname);
    if (lstat(pathname, &lstatInfo) == -1) {
        LOG_V("File '%s' does not exits", pathname);
        return -1;
    }

    if ((fd = open(pathname, flags, mode)) == -1) {
        LOG_E("Failed to open file '%s'.", pathname);
        return -1;
    }

    if (fstat(fd, &fstatInfo) == -1) {
        LOG_E("Failed to get file information. Closing.");
        close(fd);
        return -1;
    }

    if (lstatInfo.st_ino != fstatInfo.st_ino || lstatInfo.st_dev != fstatInfo.st_dev ||
        lstatInfo.st_mode != fstatInfo.st_mode) {
        LOG_E("Open file is not the expected regular file. Closing.");
        close(fd);
        return -1;
    }

    LOG_I("Returning file descriptor %d", fd);
    return fd;
}

int OsInterfaceImp::osiOpen(const char *pathname, int flags, mode_t mode) {
    return secureOpen(pathname, flags, mode);
}

int OsInterfaceImp::osiClose(int fildes) {
    return close(fildes);
}

int OsInterfaceImp::osiFcntl(int fd, int cmd) {
    return fcntl(fd, cmd);
}

int OsInterfaceImp::osiIoctl(int fd, unsigned long request, void *args) {
    return ioctl(fd, request, args);
}

void *OsInterfaceImp::osiAlloc(size_t size) {
    void *ptr;

    if (posix_memalign(&ptr, osiGetSystemPageSize(), size))
        return nullptr;

    return ptr;
}

int OsInterfaceImp::osiFree(void *ptr) {
    free(ptr);
    return 0;
}

size_t OsInterfaceImp::osiGetSystemPageSize() {
    return boost::numeric_cast<size_t>(sysconf(_SC_PAGESIZE));
}

void *OsInterfaceImp::osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) {
    return mmap(addr, size, prot, flags, fd, offset);
}

int OsInterfaceImp::osiMunmap(void *addr, size_t size) {
    return munmap(addr, size);
}

bool OsInterfaceImp::fileExists(std::string &p) {
    return std::filesystem::exists(p);
}

} // namespace VPU
