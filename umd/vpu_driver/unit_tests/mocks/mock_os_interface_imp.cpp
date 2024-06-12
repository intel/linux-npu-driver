/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <api/vpu_jsm_api.h>
#include <uapi/drm/ivpu_accel.h>

#include "api/vpu_nnrt_api_37xx.h"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"

namespace VPU {

MockOsInterfaceImp::MockOsInterfaceImp(uint32_t pciDevId)
    : pciDevId(pciDevId) {}

int MockOsInterfaceImp::osiOpen(const char *pathname, int flags, mode_t mode) {
    if (openSuccessful) {
        int vpuFd = fd;
        fd++;
        LOG(UTEST, "Returning file descriptor %d", vpuFd);
        return vpuFd;
    }

    return -EACCES;
}

int MockOsInterfaceImp::osiClose(int fildes) {
    return 0;
}

int MockOsInterfaceImp::osiFcntl(int fd, int cmd) {
    if (deviceConnected)
        return 0;

    return -1;
}

int MockOsInterfaceImp::osiIoctl(int fd, unsigned long request, void *data) {
    LOG(UTEST, "Cnt = %i, IOCTL = %#lx", callCntIoctl, request);
    // Increase call count.
    callCntIoctl++;

    // Remember the last request.
    ioctlLastCommand = request;

    if (!deviceConnected) {
        errno = EIO;
        return -1;
    }

    if (request == DRM_IOCTL_VERSION) {
        drm_version_t *arg = static_cast<drm_version_t *>(data);
        arg->version_major = kmdApiVersionMajor;
        arg->version_minor = kmdApiVersionMinor;
        arg->version_patchlevel = 0;
        if (arg->name_len != 0) {
            memcpy(arg->name, kmdApiDeviceName.data(), kmdApiDeviceName.size());
        }
        arg->name_len = kmdApiDeviceName.size();
        arg->date_len = 0;
        arg->desc_len = 0;
    } else if (request == DRM_IOCTL_IVPU_GET_PARAM) {
        struct drm_ivpu_param *args = static_cast<struct drm_ivpu_param *>(data);

        switch (args->param) {
        case DRM_IVPU_PARAM_DEVICE_ID:
            args->value = pciDevId;
            break;
        case DRM_IVPU_PARAM_DEVICE_REVISION:
        case DRM_IVPU_PARAM_CORE_CLOCK_RATE:
            args->value = 0ULL;
            break;
        case DRM_IVPU_PARAM_NUM_CONTEXTS:
            args->value = 64ULL;
            break;
        case DRM_IVPU_PARAM_CONTEXT_BASE_ADDRESS:
            args->value = deviceLowBaseAddress;
            break;
        case DRM_IVPU_PARAM_CAPABILITIES:
            if (args->index == DRM_IVPU_CAP_METRIC_STREAMER) {
                args->value = 1ULL;
            }
            break;
        case DRM_IVPU_PARAM_UNIQUE_INFERENCE_ID:
            args->value = unique_id++;
            break;
        case DRM_IVPU_PARAM_FW_API_VERSION:
            if (args->index == VPU_NNRT_37XX_API_VER_INDEX)
                args->value = VPU_NNRT_37XX_API_VER;
            break;
        case DRM_IVPU_PARAM_ENGINE_HEARTBEAT:
            args->value = callCntIoctl;
            break;
        default:
            break;
        }
    } else if (request == DRM_IOCTL_IVPU_BO_CREATE) {
        if (failNextAlloc) {
            failNextAlloc = false;
            errno = ENOMEM;
            return -1;
        }

        auto *args = static_cast<struct drm_ivpu_bo_create *>(data);
        args->vpu_addr = deviceAddress;
        deviceAddress += ALIGN(args->size, osiGetSystemPageSize());
    } else if (request == DRM_IOCTL_IVPU_BO_INFO) {
        auto *args = static_cast<struct drm_ivpu_bo_info *>(data);
        args->mmap_offset = 100u;
    } else if (request == DRM_IOCTL_IVPU_BO_WAIT) {
        bool timeout = waitFailed.test(0);
        waitFailed >>= 1;
        if (timeout) {
            errno = ETIMEDOUT;
            return -1;
        }

        auto *args = static_cast<struct drm_ivpu_bo_wait *>(data);
        if (jobFailed.test(0)) {
            args->job_status = VPU_JSM_STATUS_PARSING_ERR;
        } else {
            args->job_status = DRM_IVPU_JOB_STATUS_SUCCESS;
        }
        jobFailed >>= 1;
    } else if (request == DRM_IOCTL_IVPU_METRIC_STREAMER_GET_INFO) {
        drm_ivpu_metric_streamer_get_data *args =
            static_cast<struct drm_ivpu_metric_streamer_get_data *>(data);
        if (args->buffer_size == 0) {
            /*
            size = sizeof(vpu_jsm_metric_group_descriptor) +
                   group_desc->name_string_size +
                   group_desc->description_string_size +
                   sizeof(vpu_jsm_metric_counter_descriptor) +
                   counter_desc->name_string_size +
                   counter_desc->description_string_size +
                   counter_desc->component_string_size +
                   counter_desc->units_string_size
            */
            args->data_size = sizeof(vpu_jsm_metric_group_descriptor) + 80 +
                              sizeof(vpu_jsm_metric_counter_descriptor) + 80;
        } else {
            vpu_jsm_metric_counter_descriptor *counter_desc = nullptr;

            vpu_jsm_metric_group_descriptor *group_desc =
                reinterpret_cast<vpu_jsm_metric_group_descriptor *>(args->buffer_ptr);
            group_desc->next_metric_group_info_offset = 0;
            group_desc->next_metric_counter_info_offset = 120;
            group_desc->group_id = 3;
            group_desc->num_counters = 1;
            group_desc->metric_group_data_size = 128;
            group_desc->domain = 1;
            group_desc->name_string_size = 16;
            group_desc->description_string_size = 64;

            strncpy(reinterpret_cast<char *>(reinterpret_cast<uint64_t>(group_desc) +
                                             sizeof(vpu_jsm_metric_group_descriptor)),
                    "NOC",
                    group_desc->name_string_size);

            strncpy(reinterpret_cast<char *>(reinterpret_cast<uint64_t>(group_desc) +
                                             sizeof(vpu_jsm_metric_group_descriptor) +
                                             group_desc->name_string_size),
                    "NOC",
                    group_desc->description_string_size);

            counter_desc = reinterpret_cast<vpu_jsm_metric_counter_descriptor *>(
                reinterpret_cast<uint64_t>(group_desc) +
                group_desc->next_metric_counter_info_offset);

            counter_desc->next_metric_counter_info_offset = 0;
            counter_desc->metric_data_offset = 21696;
            counter_desc->metric_data_size = 128;
            counter_desc->tier = 0;
            counter_desc->metric_type = 1;
            counter_desc->metric_value_type = 1;
            counter_desc->name_string_size = 16;
            counter_desc->description_string_size = 64;
            counter_desc->component_string_size = 0;
            counter_desc->units_string_size = 0;

            strncpy(reinterpret_cast<char *>(reinterpret_cast<uint64_t>(counter_desc) +
                                             sizeof(vpu_jsm_metric_counter_descriptor)),
                    "noc",
                    counter_desc->name_string_size);

            strncpy(reinterpret_cast<char *>(reinterpret_cast<uint64_t>(counter_desc) +
                                             sizeof(vpu_jsm_metric_counter_descriptor) +
                                             counter_desc->name_string_size),
                    "noc",
                    counter_desc->description_string_size);
        }
    }

    if (!kmdIoctlRetCode)
        return 0;

    errno = kmdIoctlRetCode;
    return -1;
}

void *
MockOsInterfaceImp::osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) {
    if (offset == 0)
        return nullptr;

    if (failNextAlloc) {
        failNextAlloc = false;
        return nullptr;
    }

    void *ptr;
    if (posix_memalign(&ptr, osiGetSystemPageSize(), size))
        return nullptr;

    callCntAlloc++;
    return ptr;
}

int MockOsInterfaceImp::osiMunmap(void *addr, size_t size) {
    callCntFree++;
    free(addr);
    return 0;
}

bool MockOsInterfaceImp::osiCreateDirectories(const std::filesystem::path &path) {
    return true;
}

std::unique_ptr<OsFile>
MockOsInterfaceImp::osiOpenWithExclusiveLock(const std::filesystem::path &path, bool writeAccess) {
    return nullptr;
}

std::unique_ptr<OsFile> MockOsInterfaceImp::osiOpenWithSharedLock(const std::filesystem::path &path,
                                                                  bool writeAccess) {
    return nullptr;
}

void MockOsInterfaceImp::osiScanDir(const std::filesystem::path &path,
                                    std::function<void(const char *name, struct stat &stat)> f) {}

size_t MockOsInterfaceImp::osiGetSystemPageSize() {
    return 4u * 1024u;
}

// Mock manipulators.
void MockOsInterfaceImp::mockFailNextAlloc() {
    failNextAlloc = true;
}

void MockOsInterfaceImp::mockFailNextJobWait() {
    waitFailed <<= 1;
    waitFailed.set(0);
}

void MockOsInterfaceImp::mockSuccessNextJobWait() {
    waitFailed <<= 1;
}

void MockOsInterfaceImp::mockFailNextJobStatus() {
    jobFailed <<= 1;
    jobFailed.set(0);
}

void MockOsInterfaceImp::mockSuccessNextJobStatus() {
    jobFailed <<= 1;
}

} // namespace VPU
