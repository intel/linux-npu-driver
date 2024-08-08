/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/os_interface/os_interface.hpp"

#include "vpu_driver/source/os_interface/null_interface_imp.hpp"
#include "vpu_driver/source/os_interface/os_interface_imp.hpp"
#include "vpu_driver/source/utilities/log.hpp"

namespace VPU {

OsInterface &getOsInstance() {
    if (VPU::NullOsInterfaceImp::isNullDeviceRequested()) {
        LOG(DEVICE, "NULL OS interface set.");
        return NullOsInterfaceImp::getInstance();
    } else {
        LOG(DEVICE, "OS interface set.");
        return OsInterfaceImp::getInstance();
    }
}

} // namespace VPU