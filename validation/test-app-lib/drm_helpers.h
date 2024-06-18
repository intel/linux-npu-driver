/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <string>
#include <cstdint>
#include <time.h>

#define MICRO_TO_NSEC(t) (t * INT64_C(1e3))
#define MILLI_TO_NSEC(t) (t * INT64_C(1e6))
#define SEC_TO_NSEC(t) (t * INT64_C(1e9))

struct drm_device_desc {
    int fd;
    int version_major;
    int version_minor;
    int major_id;
    int minor_id;
};

namespace drm {

struct drm_device_desc open_intel_vpu(int instance = 0);
int secureOpen(const char *filePath, int opMode);
std::string get_vpu_bus_id(int major_id, int minor_id);
std::string get_sysfs_device_path(int major_id, int minor_id);
std::string get_sysfs_module_path();
std::string get_sysfs_driver_path();
std::string get_debugfs_path(int major_id, int minor_id);

/* returns current kernel time in nanoseconds */
inline int64_t time_ns(void) {
    struct timespec tv;

    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_nsec + SEC_TO_NSEC(tv.tv_sec);
}

} // namespace drm
