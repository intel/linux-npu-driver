/*
 * Copyright (C) 2024-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/os_interface/os_interface.hpp"

#include "vpu_driver/source/os_interface/os_interface_imp.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <stdlib.h>

#ifdef NO_NULL_INTERFACE
struct NullOsInterfaceImp {
    static VPU::OsInterface *getInstance() {
        LOG_E("Null device is not supported in this driver.");
        return nullptr;
    }
};
#else
#include "vpu_driver/source/os_interface/null_interface_imp.hpp"
#endif

namespace VPU {

static bool isNullDeviceRequested() {
    char *env = getenv("ZE_INTEL_NPU_PLATFORM_OVERRIDE");
    if (env)
        return true;
    return false;
}

OsInterface *getOsInstance() {
    if (isNullDeviceRequested()) {
        LOG(DEVICE, "NULL OS interface set.");
        return NullOsInterfaceImp::getInstance();
    } else {
        LOG(DEVICE, "OS interface set.");
        return OsInterfaceImp::getInstance();
    }
}

} // namespace VPU