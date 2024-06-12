/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/os_interface/null_interface_imp.hpp"

#include "umd_common.hpp"

#include <cstring>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <memory>
#include <string>
#include <uapi/drm/ivpu_accel.h>

namespace VPU {

OsInterface &NullOsInterfaceImp::getInstance() {
    static NullOsInterfaceImp instance;
    return instance;
}

bool NullOsInterfaceImp::isNullDeviceRequested() {
    char *env = getenv("ZE_INTEL_NPU_PLATFORM_OVERRIDE");
    if (!env)
        return false;
    NullOsInterfaceImp *dev =
        reinterpret_cast<NullOsInterfaceImp *>(&NullOsInterfaceImp::getInstance());
    if (std::string("INPU_MTL") == env) {
        dev->nullHwInfo = getHwInfoByDeviceId(0x7d1d);
        dev->nullHwInfo.deviceId = 0x7d1d;
        LOG_W("MTL(%x) null device is set.", dev->nullHwInfo.deviceId);
    } else if (std::string("INPU_LNL") == env) {
        dev->nullHwInfo = getHwInfoByDeviceId(0x643e);
        dev->nullHwInfo.deviceId = 0x643e;
        LOG_W("LNL(%x) null device is set.", dev->nullHwInfo.deviceId);
    } else {
        LOG_E("Null device(%s) requested but configured device is not supported.", env);
        return false;
    }

    dev->nullHwInfo.baseLowAddress = 0xc000'0000;
    dev->deviceAddress = dev->nullHwInfo.baseLowAddress;

    try {
        env = getenv("ZE_INTEL_NPU_REVISION_OVERRIDE");
        if (env) {
            std::string rev(env);
            dev->nullHwInfo.deviceRevision = static_cast<uint32_t>(std::stoul(rev));
        }
        env = getenv("ZE_INTEL_NPU_DISABLED_TILE_OVERRIDE");
        if (env) {
            std::string tilesMask(env);
            /* In null device each bit in tile config is interpreted
             * as disabled tile. This value is returned to the driver
             * as a value of kernel parameter and from this value is
             * calculated driver tileConfig, where each bit means
             * enabled tile*/
            dev->nullHwInfo.tileConfig = static_cast<uint32_t>(std::stoul(tilesMask));
        }
    } catch (std::exception &e) {
        LOG_E("Null device configuration failed: %s", e.what());
        return false;
    }
    LOG(DEVICE, "Device revision is %d", dev->nullHwInfo.deviceRevision);
    LOG(DEVICE, "Device disabled tiles bits are 0x%x", dev->nullHwInfo.tileConfig);
    return true;
}

int NullOsInterfaceImp::osiOpen(const char *pathname, int flags, mode_t mode) {
    int fd;

    if (strcmp("/dev/accel/accel0", pathname))
        return -1;

    if ((fd = open("/dev/null", O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
        LOG(FSYS, "Failed to open file dev/null.");
        return -1;
    }
    LOG(FSYS, "Returning null device file descriptor %d", fd);
    return fd;
};

int NullOsInterfaceImp::osiClose(int fd) {
    return close(fd);
}

int NullOsInterfaceImp::osiFcntl(int fd, int cmd) {
    return 0;
};

int NullOsInterfaceImp::getParamValue(drm_ivpu_param &p) {
    switch (p.param) {
    case DRM_IVPU_PARAM_DEVICE_ID:
        p.value = nullHwInfo.deviceId;
        break;
    case DRM_IVPU_PARAM_DEVICE_REVISION:
        p.value = nullHwInfo.deviceRevision;
        break;
    case DRM_IVPU_PARAM_CORE_CLOCK_RATE:
        p.value = 0ULL;
        break;
    case DRM_IVPU_PARAM_NUM_CONTEXTS:
        p.value = 64ULL;
        break;
    case DRM_IVPU_PARAM_CONTEXT_BASE_ADDRESS:
        p.value = nullHwInfo.baseLowAddress;
        break;
    case DRM_IVPU_PARAM_CAPABILITIES:
        if (p.index == DRM_IVPU_CAP_METRIC_STREAMER) {
            p.value = 0ULL;
        } else if (p.index == DRM_IVPU_CAP_DMA_MEMORY_RANGE) {
            p.value = 1ULL;
        } else {
            p.value = 0ULL;
        }
        break;
    case DRM_IVPU_PARAM_UNIQUE_INFERENCE_ID:
        p.value = unique_id++;
        break;
    case DRM_IVPU_PARAM_FW_API_VERSION:
        if (p.index == nullHwInfo.fwMappedInferenceIndex)
            p.value = nullHwInfo.fwMappedInferenceVersion;
        break;
    case DRM_IVPU_PARAM_PLATFORM_TYPE:
        p.value = 0; /* silicon */
        break;
    case DRM_IVPU_PARAM_TILE_CONFIG:
        p.value = nullHwInfo.tileConfig;
        break;
    default:
        errno = EINVAL;
        return -1;
    }
    return 0;
}

int NullOsInterfaceImp::osiIoctl(int fd, unsigned long request, void *arg) {
    if (arg == nullptr) {
        errno = EINVAL;
        return -1;
    }

    switch (request) {
    case DRM_IOCTL_VERSION: {
        auto *version = reinterpret_cast<drm_version_t *>(arg);
        version->version_major = DRM_IVPU_DRIVER_MAJOR;
        version->version_minor = DRM_IVPU_DRIVER_MINOR;
        if (version->name_len < strlen("intel_npu")) {
            version->name_len = strlen("intel_npu");
            break;
        }
        strncpy(version->name, "intel_npu", version->name_len);
        break;
    }
    case DRM_IOCTL_IVPU_GET_PARAM: {
        auto *param = reinterpret_cast<drm_ivpu_param *>(arg);
        return getParamValue(*param);
        break;
    }
    case DRM_IOCTL_IVPU_BO_CREATE: {
        auto *boCreate = reinterpret_cast<struct drm_ivpu_bo_create *>(arg);
        boCreate->vpu_addr = deviceAddress;
        deviceAddress += ALIGN(boCreate->size, osiGetSystemPageSize());
        break;
    }
    case DRM_IOCTL_IVPU_BO_INFO: {
        auto *boInfo = reinterpret_cast<struct drm_ivpu_bo_info *>(arg);
        boInfo->mmap_offset = 100u;
        break;
    }
    case DRM_IOCTL_IVPU_BO_WAIT: {
        auto *boWait = reinterpret_cast<struct drm_ivpu_bo_wait *>(arg);
        boWait->job_status = DRM_IVPU_JOB_STATUS_SUCCESS;
        break;
    }
    case DRM_IOCTL_IVPU_SET_PARAM:
    case DRM_IOCTL_IVPU_SUBMIT:
    case DRM_IOCTL_IVPU_METRIC_STREAMER_START:
    case DRM_IOCTL_IVPU_METRIC_STREAMER_STOP:
    case DRM_IOCTL_IVPU_METRIC_STREAMER_GET_DATA:
    case DRM_IOCTL_IVPU_METRIC_STREAMER_GET_INFO:
    case DRM_IOCTL_GEM_CLOSE:
    case DRM_IOCTL_PRIME_HANDLE_TO_FD:
    case DRM_IOCTL_PRIME_FD_TO_HANDLE:
        break;
    default:
        errno = EINVAL;
        return -1;
    }
    return 0;
}

size_t NullOsInterfaceImp::osiGetSystemPageSize() {
    return 4096;
}

void *
NullOsInterfaceImp::osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) {
    void *ptr;
    if (posix_memalign(&ptr, osiGetSystemPageSize(), size))
        return nullptr;

    return ptr;
}

int NullOsInterfaceImp::osiMunmap(void *addr, size_t size) {
    free(addr);
    return 0;
}

bool NullOsInterfaceImp::osiCreateDirectories(const std::filesystem::path &path) {
    return false;
}

std::unique_ptr<OsFile>
NullOsInterfaceImp::osiOpenWithExclusiveLock(const std::filesystem::path &path, bool writeAccess) {
    return nullptr;
}

std::unique_ptr<OsFile> NullOsInterfaceImp::osiOpenWithSharedLock(const std::filesystem::path &path,
                                                                  bool writeAccess) {
    return nullptr;
}

void NullOsInterfaceImp::osiScanDir(const std::filesystem::path &path,
                                    std::function<void(const char *name, struct stat &stat)> f) {
    return;
}

} // namespace VPU
