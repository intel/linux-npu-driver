/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>

#include "driver_handle.hpp"
#include "ext/disk_cache.hpp"

#include <memory>
#include <mutex>
#include <string_view>
#include <vector>
#include <ze_api.h>

namespace VPU {
class OsInterface;
} // namespace VPU

namespace L0 {

class Driver {
  public:
    struct L0EnvVariables {
        std::string_view affinityMask;
        bool metrics;
        bool pciIdDeviceOrder;
        bool sharedForceDeviceAlloc;
        bool extensionValidation;
    };

    Driver() { initializeLogging(); }
    virtual ~Driver() = default;

    static Driver *getInstance() { return pDriver; }

    virtual ze_result_t driverInit(ze_init_flags_t flags);
    virtual ze_result_t getInitStatus();
    virtual const L0EnvVariables &getEnvVariables() { return envVariables; }
    virtual std::vector<std::unique_ptr<DriverHandle>> &getDriverHandles() { return driverHandles; }

    ze_api_version_t getAppApiVersionHint() const { return appApiVersionHint; }
    void setAppApiVersionHint(ze_api_version_t version) {
        std::call_once(appApiVersionHintOnce, [this, version]() { appApiVersionHint = version; });
    }

    DiskCache &getDiskCache() { return *diskCache; }

    std::unique_ptr<DiskCache> diskCache;

  protected:
    void displayComponentVersions();
    void initializeEnvVariables();
    void initializeLogging();
    static void setInstanceForTesting(Driver *driver) { pDriver = driver; }

    L0EnvVariables envVariables = {};
    VPU::OsInterface *osInfc = nullptr;

  private:
    static Driver *pDriver;
    std::vector<std::unique_ptr<DriverHandle>> driverHandles;
    ze_result_t initStatus = ZE_RESULT_ERROR_UNINITIALIZED;
    std::once_flag initDriverOnce;
    std::once_flag appApiVersionHintOnce;
    ze_api_version_t appApiVersionHint = ZE_API_VERSION_1_0;
};

ze_result_t init(ze_init_flags_t);
ze_result_t driverHandleGet(uint32_t *pCount, ze_driver_handle_t *phDrivers);
ze_result_t
initDrivers(uint32_t *pCount, ze_driver_handle_t *phDrivers, ze_init_driver_type_desc_t *desc);

} // namespace L0
