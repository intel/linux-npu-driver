/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"

#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/driver/driver.hpp"

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/os_interface/vpu_device_factory.hpp"
#include "vpu_driver/source/os_interface/os_interface_imp.hpp"

namespace L0 {

static Driver driver;
Driver *Driver::pDriver;

void Driver::initializeEnvVariables() {
    const char *env = getenv("ZE_AFFINITY_MASK");
    envVariables.affinityMask = env == nullptr ? "" : env;

    env = getenv("ZET_ENABLE_METRICS");
    envVariables.metrics = env == nullptr || env[0] == '0' || env[0] == '\0' ? false : true;

    env = getenv("ZE_ENABLE_PCI_ID_DEVICE_ORDER");
    envVariables.pciIdDeviceOrder =
        env == nullptr || env[0] == '0' || env[0] == '\0' ? false : true;

    env = getenv("ZE_SHARED_FORCE_DEVICE_ALLOC");
    envVariables.sharedForceDeviceAlloc =
        env == nullptr || env[0] == '0' || env[0] == '\0' ? false : true;

    env = getenv("VPU_DRV_UMD_LOGLEVEL");
    envVariables.umdLogLevel = env == nullptr ? "" : env;
}

ze_result_t Driver::getInitStatus() {
    LOG_V("Current driver init status is %u", initStatus);
    return initStatus;
}

void Driver::driverInit(ze_init_flags_t flags) {
    std::call_once(this->initDriverOnce, [&]() {
        initializeEnvVariables();
        VPU::setLogLevel(envVariables.umdLogLevel);

        if (osInfc == nullptr) {
            LOG_V("OS interface updated.");
            osInfc = &VPU::OsInterfaceImp::getInstance();

            if (osInfc == nullptr) {
                LOG_E("Failed to initialize (OS interface is null).");
                return;
            }
        }

        auto vpuDevices = VPU::DeviceFactory::createDevices(osInfc);
        LOG_W("%zu VPU device(s) found.", vpuDevices.size());
        if (!vpuDevices.empty()) {
            pGlobalDriverHandle = DriverHandle::create(std::move(vpuDevices));
            if (pGlobalDriverHandle == nullptr) {
                LOG_W("Failed to initialize (GlobalDriverHandle is null).");
                return;
            }
        }

        initStatus = ZE_RESULT_SUCCESS;
    });
    LOG_V("Driver init status to %u", safe_cast<uint32_t>(initStatus));

    return;
}

ze_result_t driverHandleGet(uint32_t *pCount, ze_driver_handle_t *phDriverHandles) {
    Driver *pDriver = Driver::getInstance();
    if (!pDriver)
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;

    if (pDriver->getInitStatus() != ZE_RESULT_SUCCESS) {
        LOG_E("Invalid init status: ZE_RESULT_ERROR_UNINITIALIZED");
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    if (nullptr == pCount) {
        LOG_E("Invalid driver handle count pointer.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    if (*pCount == 0) {
        *pCount = pDriver->getDriverCount();
        return ZE_RESULT_SUCCESS;
    }

    if (phDriverHandles != nullptr) {
        phDriverHandles[0] = static_cast<_ze_driver_handle_t *>(pDriver->getDriverHandle());
        *pCount = pDriver->getDriverCount();
    } else {
        LOG_I("Input driver handle pointer is NULL.");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t init(ze_init_flags_t flags) {
    if (flags != 0 && ((flags & ZE_INIT_FLAG_VPU_ONLY) == 0)) {
        LOG_E("Invalid init flag: %x", flags);
        return ZE_RESULT_ERROR_INVALID_ENUMERATION;
    }

    Driver *pDriver = Driver::getInstance();
    if (!pDriver)
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;

    pDriver->driverInit(flags);
    LOG_V("Updating driver init status to %u", safe_cast<uint32_t>(pDriver->getInitStatus()));

    return pDriver->getInitStatus();
}

} // namespace L0
