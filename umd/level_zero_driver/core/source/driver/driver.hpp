/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"

#include <level_zero/ze_api.h>

#include <mutex>

namespace L0 {

class Driver {
  public:
    struct L0EnvVariables {
        std::string_view affinityMask;
        bool metrics;
        bool pciIdDeviceOrder;
        bool sharedForceDeviceAlloc;

        std::string_view umdLogLevel;
    };

    Driver() { pDriver = this; }
    virtual ~Driver() {}
    void operator=(const Driver &) = delete;

    static Driver *getInstance() { return pDriver; }
    uint32_t getDriverCount() { return driverCount; }
    virtual void driverInit(ze_init_flags_t flags);
    virtual ze_result_t getInitStatus();
    virtual const L0EnvVariables &getEnvVariables() { return envVariables; }
    virtual DriverHandle *getDriverHandle() { return pGlobalDriverHandle; }

  protected:
    static Driver *pDriver;
    L0EnvVariables envVariables = {};
    void initializeEnvVariables();

  private:
    const uint32_t driverCount = 1;
    DriverHandle *pGlobalDriverHandle = nullptr;
    VPU::OsInterface *osInfc = nullptr;
    ze_result_t initStatus = ZE_RESULT_ERROR_UNINITIALIZED;
    std::once_flag initDriverOnce;
};

ze_result_t init(ze_init_flags_t);
ze_result_t driverHandleGet(uint32_t *pCount, ze_driver_handle_t *phDrivers);

} // namespace L0
