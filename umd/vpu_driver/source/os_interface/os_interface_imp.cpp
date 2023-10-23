/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "vpu_driver/source/os_interface/os_interface_imp.hpp"
#include "umd_common.hpp"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <memory>
#include <string>

namespace VPU {

OsInterfaceImp &OsInterfaceImp::getInstance() {
    static OsInterfaceImp instance;
    return instance;
}

int OsInterfaceImp::osiOpen(const char *pathname, int flags, mode_t mode) {
    struct stat fstatInfo = {};
    int fd;

    LOG_V("Trying to open file '%s'.", pathname);
    if ((fd = open(pathname, flags, mode)) == -1) {
        LOG_V("Failed to open file '%s'.", pathname);
        return -1;
    }

    if (fstat(fd, &fstatInfo) != 0) {
        LOG_E("Failed to get file information. Closing.");
        close(fd);
        return -1;
    }

    if (!S_ISCHR(fstatInfo.st_mode)) {
        LOG_E("Open file is not the expected device file. Closing.");
        close(fd);
        return -1;
    }

    LOG_I("Returning file descriptor %d", fd);
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
    return safe_cast<size_t>(sysconf(_SC_PAGESIZE));
}

void *OsInterfaceImp::osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) {
    return mmap(addr, size, prot, flags, fd, offset);
}

int OsInterfaceImp::osiMunmap(void *addr, size_t size) {
    return munmap(addr, size);
}

} // namespace VPU
