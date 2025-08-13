/*
 * Copyright (C) 2024-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/os_interface/null_interface_imp.hpp"

#include "umd_common.hpp"
#include "vpu_driver/source/device/vpu_37xx/vpu_hw_37xx.hpp"
#include "vpu_driver/source/device/vpu_40xx/vpu_hw_40xx.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <bitset>
#include <cstring>
#include <errno.h>
#include <exception>
#include <fcntl.h>
#include <memory>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <uapi/drm/drm.h>
#include <uapi/drm/ivpu_accel.h>
#include <unistd.h>

namespace VPU {

OsInterface *NullOsInterfaceImp::getInstance() {
    static NullOsInterfaceImp instance;

    return instance.configureNullDevice() ? &instance : nullptr;
}

bool NullOsInterfaceImp::configureNullDevice() {
    char *env = getenv("ZE_INTEL_NPU_PLATFORM_OVERRIDE");
    if (!env)
        return false;

    if (std::string("INPU_MTL") == env || std::string("METEORLAKE") == env) {
        nullHwInfo = getHwInfoByDeviceId(PCI_DEVICE_ID_MTL);
        nullHwInfo.deviceId = PCI_DEVICE_ID_MTL;
        LOG_W("MTL(%#x) null device is set.", nullHwInfo.deviceId);
    } else if (std::string("INPU_LNL") == env || std::string("LUNARLAKE") == env) {
        nullHwInfo = getHwInfoByDeviceId(PCI_DEVICE_ID_LNL);
        nullHwInfo.deviceId = PCI_DEVICE_ID_LNL;
        LOG_W("LNL(%#x) null device is set.", nullHwInfo.deviceId);
    } else if (std::string("ARROWLAKE") == env) {
        nullHwInfo = getHwInfoByDeviceId(PCI_DEVICE_ID_ARL);
        nullHwInfo.deviceId = PCI_DEVICE_ID_ARL;
        LOG_W("ARL(%#x) null device is set.", nullHwInfo.deviceId);
    } else {
        LOG_E("Null device(%s) requested but configured device is not supported.", env);
        return false;
    }

    deviceAddress = 0xc000'0000;

    /* Revision can be provided in formats: oct, dec, hex */
    env = getenv("ZE_INTEL_NPU_REVISION_OVERRIDE");
    if (env) {
        uint16_t npuRevisionId;
        try {
            std::string revString(env);
            size_t charsParsed;

            npuRevisionId = static_cast<uint16_t>(std::stoul(revString, &charsParsed, 0));

            if (charsParsed != revString.length())
                throw std::invalid_argument(revString);
        } catch (std::exception &e) {
            LOG_E("Null device revision can not be parsed: %s", e.what());
            return false;
        }

        nullHwInfo.deviceRevision = npuRevisionId;
    }

    /* Disabled tile mask can be provided in formats: bin, dec, oct, hex */
    env = getenv("ZE_INTEL_NPU_DISABLED_TILE_OVERRIDE");
    if (env) {
        try {
            std::string tilesMask(env);
            size_t charsParsed;
            /* In null device each bit in tile config is interpreted
             * as disabled tile. This value is returned to the driver
             * as a value of kernel parameter and from this value is
             * calculated driver tileConfig, where each bit means
             * enabled tile, no additional verification against device
             * is intentionally for test purposes
             */
            if (tilesMask.front() == 'b') {
                tilesMask = tilesMask.substr(1);
                nullHwInfo.tileConfig =
                    static_cast<uint32_t>(std::stoul(tilesMask, &charsParsed, 2));
            } else {
                nullHwInfo.tileConfig =
                    static_cast<uint32_t>(std::stoul(tilesMask, &charsParsed, 0));
            }
            if (charsParsed != tilesMask.length())
                throw std::invalid_argument(tilesMask);
        } catch (std::exception &e) {
            LOG_E("Null device tile configuration can not be parsed: %s", e.what());
            return false;
        }
    }

    /* Tile count can be provided in formats: oct, dec, hex */
    env = getenv("ZE_INTEL_NPU_TILE_COUNT_OVERRIDE");
    if (env) {
        size_t enabledTiles;
        try {
            std::string tilesCount(env);
            size_t charsParsed;

            enabledTiles = static_cast<size_t>(std::stoul(tilesCount, &charsParsed, 0));
            if (charsParsed != tilesCount.length())
                throw std::invalid_argument(tilesCount);
        } catch (std::exception &e) {
            LOG_E("Null device tile configuration can not be parsed: %s", e.what());
            return false;
        }

        if (!enabledTiles) {
            LOG_E("Null device configuration disables all tiles.");
            return false;
        }
        std::bitset<32> maxTiles(nullHwInfo.tileFuseMask);
        if (enabledTiles > maxTiles.count()) {
            LOG_E("Null device tile count above limit, set: %zd supported: %zd",
                  enabledTiles,
                  maxTiles.count());
            return false;
        }
        size_t disabledTiles = maxTiles.count() - enabledTiles;

        /* tilesConfig represents disabled tiles */
        if (disabledTiles)
            nullHwInfo.tileConfig = (1 << disabledTiles) - 1;
    }

    LOG(DEVICE, "Device PCI ID is %x", nullHwInfo.deviceId);
    LOG(DEVICE, "Device revision is %d", nullHwInfo.deviceRevision);
    LOG(DEVICE, "Device disabled tiles bits are 0x%x", nullHwInfo.tileConfig);
    return true;
}

int NullOsInterfaceImp::osiOpen(const char *pathname, int flags, mode_t mode) {
    int fd;

    if (strcmp("/dev/accel/accel0", pathname))
        return -1;

    if ((fd = open("/dev/null", O_RDWR)) == -1) {
        LOG(FSYS, "Failed to open file /dev/null.");
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

int NullOsInterfaceImp::osiIoctl(int fd, unsigned int request, void *arg) {
    if (arg == nullptr) {
        errno = EINVAL;
        return -1;
    }

    switch (request) {
    case DRM_IOCTL_VERSION: {
        auto *uapi_version = reinterpret_cast<drm_version_t *>(arg);
        uapi_version->version_major = 1;
        uapi_version->version_minor = 0;
        if (uapi_version->name_len < strlen("intel_npu")) {
            uapi_version->name_len = strlen("intel_npu");
            break;
        }
        strncpy(uapi_version->name, "intel_npu", uapi_version->name_len);
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

std::string NullOsInterfaceImp::osiReadFile(const std::filesystem::path &path, size_t maxReadSize) {
    return std::string("");
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

bool NullOsInterfaceImp::osiFileRemove(const std::filesystem::path &path) {
    return true;
}
} // namespace VPU
