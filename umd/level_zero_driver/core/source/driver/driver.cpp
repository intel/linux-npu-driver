/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"
#include "level_zero_driver/ext/source/graph/compiler_common.hpp"

#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/driver/driver.hpp"

#include "version.h"
#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/os_interface/vpu_device_factory.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"
#include <memory>

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
}

void Driver::initializeLogging() {
    const char *env = getenv("ZE_INTEL_NPU_LOGLEVEL");
    std::string_view umdLogLevel = env == nullptr ? "" : env;

    env = getenv("ZE_INTEL_NPU_LOGMASK");
    std::string_view umdLogMask = env == nullptr ? "" : env;

    env = getenv("ZE_INTEL_NPU_COMPILER_LOGLEVEL");
    std::string_view cidLogLevel = env == nullptr ? "" : env;

    VPU::setLogLevel(umdLogLevel);
    VPU::setLogMask(umdLogMask);
    setCidLogLevel(cidLogLevel);
}

ze_result_t Driver::getInitStatus() {
    LOG(DRIVER, "Current driver init status is %u", initStatus);
    return initStatus;
}

void Driver::driverInit(ze_init_flags_t flags) {
    std::call_once(this->initDriverOnce, [&]() {
        initializeEnvVariables();
        if (osInfc == nullptr) {
            LOG(DRIVER, "OS interface updated");
            osInfc = &VPU::getOsInstance();

            if (osInfc == nullptr) {
                LOG_E("Failed to initialize (OS interface is null)");
                return;
            }
        }

        diskCache = std::make_unique<DiskCache>(*osInfc);
        auto vpuDevices = VPU::DeviceFactory::createDevices(osInfc, envVariables.metrics);
        LOG(DRIVER, "%zu VPU device(s) found.", vpuDevices.size());
        if (!vpuDevices.empty()) {
            pGlobalDriverHandle = DriverHandle::create(std::move(vpuDevices));
            if (pGlobalDriverHandle == nullptr) {
                LOG_W("Failed to initialize (GlobalDriverHandle is null)");
                return;
            }
            initStatus = ZE_RESULT_SUCCESS;
        }
    });
    LOG(DRIVER, "Driver init status to %u", initStatus);
    LOG(DRIVER, "Driver version: %s", vpu_drv_version_str);

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
        LOG_E("Invalid driver handle count pointer");
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
        LOG(DRIVER, "Input driver handle pointer is NULL");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t init(ze_init_flags_t flags) {
    if (flags != 0 && ((flags & ZE_INIT_FLAG_VPU_ONLY) == 0)) {
        LOG_E("Invalid init flag: %x", flags);
        return ZE_RESULT_ERROR_UNINITIALIZED;
    }

    Driver *pDriver = Driver::getInstance();
    if (!pDriver)
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;

    pDriver->driverInit(flags);
    LOG(DRIVER, "Updating driver init status to %u", pDriver->getInitStatus());

    return pDriver->getInitStatus();
}

} // namespace L0
