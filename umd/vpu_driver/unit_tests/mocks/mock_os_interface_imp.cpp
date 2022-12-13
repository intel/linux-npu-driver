/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <uapi/drm/ivpu_drm.h>

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/include/firmware/vpu_jsm_api.h"

namespace VPU {

MockOsInterfaceImp::MockOsInterfaceImp(uint16_t pciDevId)
    : pciDevId(pciDevId) {}

int MockOsInterfaceImp::osiOpen(const char *pathname, int flags, mode_t mode) {
    if (openSuccessful)
        return 0;

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
    LOG_I("Cnt = %i, IOCTL = %#lx", callCntIoctl, request);
    // Increase call count.
    callCntIoctl++;

    // Remember the last request.
    ioctlLastCommand = request;

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
        case DRM_IVPU_PARAM_CONTEXT_ID:
            args->value = host_ssid++;
            break;
        case DRM_IVPU_PARAM_CONTEXT_BASE_ADDRESS:
            args->value = deviceLowBaseAddress;
            break;
        case DRM_IVPU_PARAM_UNIQUE_INFERENCE_ID:
            args->value = unique_id++;
        default:
            break;
        }
    } else if (request == DRM_IOCTL_IVPU_BO_USERPTR) {
        struct drm_ivpu_bo_userptr *args = static_cast<struct drm_ivpu_bo_userptr *>(data);
        args->vpu_addr = deviceAddress;
        deviceAddress += ALIGN(args->user_size, osiGetSystemPageSize());
    } else if (request == DRM_IOCTL_IVPU_BO_CREATE) {
        if (failNextAlloc) {
            failNextAlloc = false;
            errno = -ENOMEM;
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
            errno = -ETIMEDOUT;
            return -1;
        }

        auto *args = static_cast<struct drm_ivpu_bo_wait *>(data);
        if (jobFailed.test(0)) {
            args->job_status = VPU_JSM_STATUS_PARSING_ERR;
        } else {
            args->job_status = DRM_IVPU_JOB_STATUS_SUCCESS;
        }
        jobFailed >>= 1;
    }

    if (!kmdIoctlRetCode)
        return 0;

    errno = kmdIoctlRetCode;
    return -1;
}

void *MockOsInterfaceImp::osiAlloc(size_t size) {
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

int MockOsInterfaceImp::osiFree(void *ptr) {
    callCntFree++;
    free(ptr);
    return 0;
}

void *
MockOsInterfaceImp::osiMmap(void *addr, size_t size, int prot, int flags, int fd, off_t offset) {
    if (offset == 0)
        return nullptr;

    return osiAlloc(size);
}

int MockOsInterfaceImp::osiMunmap(void *addr, size_t size) {
    return osiFree(addr);
}

bool MockOsInterfaceImp::fileExists(std::string &p) {
    return true;
}

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
