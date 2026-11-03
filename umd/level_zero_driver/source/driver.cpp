/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "driver.hpp"

#include "device.hpp"
#include "driver_handle.hpp"
#include "ext/compiler.hpp"
#include "ext/disk_cache.hpp"
#include "level_zero_driver/include/nested_structs_handler.hpp"
#include "umd_common.hpp"
#include "version.h"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "vpu_driver/source/os_interface/vpu_device_factory.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <algorithm>
#include <memory>
#include <optional>
#include <stdlib.h>
#include <utility>
#include <vector>
#include <ze_api.h>

namespace L0 {

static Driver driver;
Driver *Driver::pDriver = &driver;

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

    env = getenv("ZE_ENABLE_VALIDATION_LAYER");
    bool validationLayerEnabled = env == nullptr || env[0] == '0' || env[0] == '\0' ? false : true;

    if (validationLayerEnabled) {
        env = getenv("ZE_ENABLE_PARAMETER_VALIDATION");
        bool paramValidationEnabled =
            env == nullptr || env[0] == '0' || env[0] == '\0' ? false : true;
        envVariables.extensionValidation = paramValidationEnabled;
    }
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
    L0::Compiler::setLogLevel(cidLogLevel);
}

ze_result_t Driver::getInitStatus() {
    return initStatus;
}

void Driver::displayComponentVersions() {
    LOG(MISC, "Driver version: %s", vpu_drv_version_str);
    for (auto &driverHandle : driverHandles) {
        auto device = driverHandle->getPrimaryDevice();
        if (device == nullptr) {
            LOG_E("Failed to retrieve primary device");
            continue;
        }

        auto vpuDevice = device->getVPUDevice();
        if (vpuDevice == nullptr) {
            LOG_E("Failed to retrieve VPU device");
            continue;
        }

        LOG(MISC,
            "Driver[%p]: Device JSM Command Job API version: %s",
            driverHandle.get(),
            vpuDevice->jsmCmdApiVersion.c_str());
        LOG(MISC,
            "Driver[%p]: Device NNRT version: %s",
            driverHandle.get(),
            vpuDevice->mappedInferenceVersion.c_str());
        LOG(MISC,
            "Driver[%p]: Device JSM API version: %s",
            driverHandle.get(),
            vpuDevice->jsmApiVersion.c_str());
    }
}

ze_result_t Driver::driverInit(ze_init_flags_t flags) {
    std::call_once(this->initDriverOnce, [&]() {
        initializeEnvVariables();
        if (osInfc == nullptr) {
            osInfc = VPU::getOsInstance();
            if (osInfc == nullptr) {
                LOG_E("Failed to initialize OS interface");
                return;
            }
        }

        diskCache = std::make_unique<DiskCache>(*osInfc);
        auto vpuDevices = VPU::DeviceFactory::createDevices(osInfc);
        LOG(DRIVER, "%zu VPU device(s) found.", vpuDevices.size());
        for (auto &vpuDevice : vpuDevices) {
            auto driverHandle = std::make_unique<DriverHandle>(std::move(vpuDevice));
            if (driverHandle == nullptr) {
                LOG_W("Failed to initialize driver handle");
                driverHandles.clear();
                return;
            }
            driverHandles.push_back(std::move(driverHandle));
        }

        initStatus = driverHandles.empty() ? ZE_RESULT_ERROR_UNINITIALIZED : ZE_RESULT_SUCCESS;
        displayComponentVersions();
    });
    return initStatus;
}

ze_result_t driverHandleGet(uint32_t *pCount, ze_driver_handle_t *phDriverHandles) {
    Driver *pDriver = Driver::getInstance();
    if (!pDriver)
        return ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY;

    ze_result_t initStatus = pDriver->getInitStatus();
    if (initStatus != ZE_RESULT_SUCCESS)
        return initStatus;

    if (pCount == nullptr) {
        LOG_E("Invalid driver handle count pointer");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    uint32_t drvCount = safe_cast<uint32_t>(pDriver->getDriverHandles().size());
    if (*pCount == 0) {
        *pCount = drvCount;
        return ZE_RESULT_SUCCESS;
    }

    if (phDriverHandles != nullptr) {
        *pCount = std::min(drvCount, *pCount);
        for (auto i = 0u; i < *pCount; i++)
            phDriverHandles[i] = pDriver->getDriverHandles()[i].get();
    } else {
        LOG(DRIVER, "Input driver handle pointer is NULL");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    return ZE_RESULT_SUCCESS;
}

ze_result_t init(ze_init_flags_t flags) {
    if (flags != 0 && ((flags & ZE_INIT_FLAG_VPU_ONLY) == 0))
        return ZE_RESULT_ERROR_UNINITIALIZED;

    Driver *pDriver = Driver::getInstance();
    if (!pDriver)
        return ZE_RESULT_ERROR_UNKNOWN;

    return pDriver->driverInit(flags);
}

ze_result_t
initDrivers(uint32_t *pCount, ze_driver_handle_t *phDrivers, ze_init_driver_type_desc_t *desc) {
    if (desc == nullptr) {
        LOG_E("desc pointer is null");
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    }

    auto processInitDriverTypeExt = [](const void *pNext) -> std::optional<const void *> {
        const auto *base = reinterpret_cast<const ze_base_desc_t *>(pNext);
        if (base->stype == ZE_STRUCTURE_TYPE_INIT_DRIVER_APP_VERSION_EXT_DESC) {
            const auto *appVersionDesc =
                reinterpret_cast<const ze_init_driver_app_version_ext_desc_t *>(pNext);
            LOG(DRIVER,
                "Application declared max supported API version: 0x%x",
                appVersionDesc->apiVersionHint);
            Driver::getInstance()->setAppApiVersionHint(appVersionDesc->apiVersionHint);
        }
        return base->pNext;
    };
    handleNestedStructs(desc->pNext, processInitDriverTypeExt);

    ze_result_t result = init(desc->flags);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    result = driverHandleGet(pCount, phDrivers);
    if (result != ZE_RESULT_SUCCESS) {
        return result;
    }

    return ZE_RESULT_SUCCESS;
}

} // namespace L0
