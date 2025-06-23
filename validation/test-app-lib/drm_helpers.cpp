/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */
#include <chrono>
#include <fcntl.h>
#include <filesystem>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

#include <uapi/drm/ivpu_accel.h>
#include "drm_helpers.h"

#define ACCEL_MAJOR 261
#define ACCEL_MAX_MINORS 64

#define DRM_MAJOR 226
#define DRM_RENDER_MINOR_BASE 128
#define DRM_RENDER_MINOR_MAX (DRM_RENDER_MINOR_BASE + 63)

#define VPU_DRIVER_NAME1 "intel_vpu"
#define VPU_DRIVER_NAME2 "intel_npu"

namespace drm {

std::string module_name = "";

bool read_version(int fd, int &major, int &minor) {
    drm_version_t version = {};
    bool ret = false;

    if (ioctl(fd, DRM_IOCTL_VERSION, &version))
        return false;

    major = version.version_major;
    minor = version.version_minor;

    if (!version.name_len)
        return false;

    version.name = (char *)calloc(version.name_len + 1, 1);
    if (ioctl(fd, DRM_IOCTL_VERSION, &version)) {
        ret = false;
        goto free;
    }

    version.name[version.name_len] = '\0';
    ret = !(strcmp(version.name, VPU_DRIVER_NAME1) && strcmp(version.name, VPU_DRIVER_NAME2));

free:
    free(version.name);
    return ret;
}

int secureOpen(const char *filePath, int opMode) {
    struct stat fstatInfo = {};
    int fd;

    if ((fd = open(filePath, opMode, (S_IRUSR | S_IWUSR))) < 0)
        return -errno;

    if (fstat(fd, &fstatInfo) != 0) {
        close(fd);
        return -1;
    }

    if (S_ISCHR(fstatInfo.st_mode) || S_ISREG(fstatInfo.st_mode)) {
        return fd;
    }

    close(fd);
    return -1;
}

static int open_minor(const char *prefix, int minor) {
    char buf[64];

    snprintf(buf, sizeof(buf), "%s%d", prefix, minor);
    return secureOpen(buf, (O_RDWR | O_CLOEXEC));
}

static drm_device_desc find_minor(int instance, const char *prefix, int min_minor, int max_minor) {
    drm_device_desc ret = {};

    for (int i = min_minor; i <= max_minor; i++) {
        ret.fd = open_minor(prefix, i);
        if (ret.fd >= 0) {
            if (read_version(ret.fd, ret.version_major, ret.version_minor)) {
                if (instance == 0) {
                    ret.minor_id = i;
                    return ret;
                } else {
                    close(ret.fd);
                    instance--;
                }
            } else {
                close(ret.fd);
            }
        }
    }

    ret.fd = -1;
    return ret;
}

// Opens Nth instance of intel_vpu device
drm_device_desc open_intel_vpu(int instance) {
    drm_device_desc ret = {};

    // For about 100ms timeout, attempt to check if a device file exists.
    for (int i = 0; i < 100; i++) {
        ret = find_minor(instance, "/dev/accel/accel", 0, ACCEL_MAX_MINORS);
        if (ret.fd >= 0) {
            ret.major_id = ACCEL_MAJOR;
            return ret;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    ret.fd = -1;
    return ret;
}

std::string get_sysfs_device_path(int major_id, int minor_id) {
    // Device tree can be found under /sys/dev/char/<major>:<minor>/device
    return "/sys/dev/char/" + std::to_string(major_id) + ":" + std::to_string(minor_id) + "/device";
}

std::string get_module_name() {
    if (!module_name.empty()) {
        return module_name;
    }

    module_name = VPU_DRIVER_NAME1;
    if (std::filesystem::exists(std::string("/sys/module/") + module_name))
        return module_name;

    module_name = VPU_DRIVER_NAME2;
    if (std::filesystem::exists(std::string("/sys/module/") + module_name))
        return module_name;

    module_name = "";
    return module_name;
}

std::string get_sysfs_module_path() {
    return std::string("/sys/module/") + get_module_name();
}

std::string get_sysfs_driver_path() {
    return std::string("/sys/bus/pci/drivers/") + get_module_name();
}

std::string get_vpu_bus_id(int major_id, int minor_id) {
    auto device_path = get_sysfs_device_path(major_id, minor_id);

    if (std::filesystem::is_symlink(device_path))
        return std::filesystem::read_symlink(device_path).filename();

    return "";
}

std::string get_debugfs_path(int major_id, int minor_id) {
    std::string path = std::string("/sys/kernel/debug/accel/") + get_vpu_bus_id(major_id, minor_id);
    if (std::filesystem::exists(path))
        return path;

    if (major_id == ACCEL_MAJOR)
        return "/sys/kernel/debug/accel/" + std::to_string(minor_id);

    return "/sys/kernel/debug/dri/" + std::to_string(minor_id);
}

} // namespace drm
