/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <ze_api.h>
#include <zes_ddi.h>

namespace L0 {
extern zes_device_dditable_t zesDeviceDdiTable;
extern zes_global_dditable_t zesGlobalDdiTable;
extern zes_driver_dditable_t zesDriverDdiTable;
extern zes_engine_dditable_t zesEngineDdiTable;
} // namespace L0

zes_dditable_driver_t sysmanDdiTable = {.version = ZE_API_VERSION_CURRENT,
                                        .isValidFlag = 1,
                                        .Global = &L0::zesGlobalDdiTable,
                                        .Device = &L0::zesDeviceDdiTable,
                                        .DeviceExp = nullptr,
                                        .Driver = &L0::zesDriverDdiTable,
                                        .DriverExp = nullptr,
                                        .Overclock = nullptr,
                                        .Scheduler = nullptr,
                                        .PerformanceFactor = nullptr,
                                        .Power = nullptr,
                                        .Frequency = nullptr,
                                        .Engine = &L0::zesEngineDdiTable,
                                        .Standby = nullptr,
                                        .Firmware = nullptr,
                                        .FirmwareExp = nullptr,
                                        .Memory = nullptr,
                                        .FabricPort = nullptr,
                                        .Temperature = nullptr,
                                        .Psu = nullptr,
                                        .Fan = nullptr,
                                        .Led = nullptr,
                                        .Ras = nullptr,
                                        .RasExp = nullptr,
                                        .Diagnostics = nullptr,
                                        .VFManagementExp = nullptr};
