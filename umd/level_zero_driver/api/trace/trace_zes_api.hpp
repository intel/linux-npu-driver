/*
 * Copyright (C) 2024-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Generated header based on zes_api.h@version v1.11-r1.11.4

#pragma once

#include "trace_ze_api.hpp"

#include <iostream>
#include <sstream>
#include <zes_api.h>

inline std::string _trace_zesInit(zes_init_flags_t flags) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesInit(";
    ss << std::hex << std::showbase;
    ss << "flags: " << flags;
    ss << ")";
    return ss.str();
}
inline void trace_zesInit(zes_init_flags_t flags) {
    TRACE_EVENT_BEGIN("API", "zesInit");
    if (IS_API_TRACE())
        std::cerr << _trace_zesInit(flags) + "..\n";
}
inline void trace_zesInit(ze_result_t ret, zes_init_flags_t flags) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesInit(flags) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDriverGet(uint32_t *pCount, zes_driver_handle_t *phDrivers) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDriverGet(";
    ss << std::hex << std::showbase;
    if (pCount == nullptr) {
        ss << "pCount: nullptr";
    } else {
        ss << "pCount: " << *pCount;
    }
    if (phDrivers == nullptr) {
        ss << ", phDrivers: nullptr";
    } else {
        ss << ", phDrivers: " << *phDrivers;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDriverGet(uint32_t *pCount, zes_driver_handle_t *phDrivers) {
    TRACE_EVENT_BEGIN("API", "zesDriverGet");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDriverGet(pCount, phDrivers) + "..\n";
}
inline void trace_zesDriverGet(ze_result_t ret, uint32_t *pCount, zes_driver_handle_t *phDrivers) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDriverGet(pCount, phDrivers) + trace_ze_result_t(ret);
}

inline std::string
_trace_zesDriverGetExtensionProperties(zes_driver_handle_t hDriver,
                                       uint32_t *pCount,
                                       zes_driver_extension_properties_t *pExtensionProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDriverGetExtensionProperties(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pExtensionProperties == nullptr) {
        ss << ", pExtensionProperties: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pExtensionProperties[";
        if (count > 0) {
            ss << "{";
            ss << "name: "
               << trace_ascii_array(pExtensionProperties[0].name,
                                    sizeof(pExtensionProperties[0].name))
                      .str();
            ss << ", version: " << pExtensionProperties[0].version;
            ss << "}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zesDriverGetExtensionProperties(zes_driver_handle_t hDriver,
                                      uint32_t *pCount,
                                      zes_driver_extension_properties_t *pExtensionProperties) {
    TRACE_EVENT_BEGIN("API", "zesDriverGetExtensionProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDriverGetExtensionProperties(hDriver, pCount, pExtensionProperties) +
                         "..\n";
}
inline void
trace_zesDriverGetExtensionProperties(ze_result_t ret,
                                      zes_driver_handle_t hDriver,
                                      uint32_t *pCount,
                                      zes_driver_extension_properties_t *pExtensionProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDriverGetExtensionProperties(hDriver, pCount, pExtensionProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDriverGetExtensionFunctionAddress(zes_driver_handle_t hDriver,
                                                               const char *name,
                                                               void **ppFunctionAddress) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDriverGetExtensionFunctionAddress(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    if (name == nullptr) {
        ss << ", name: nullptr";
    } else {
        ss << ", name: " << name;
    }
    ss << ", ppFunctionAddress: " << ppFunctionAddress;
    ss << ")";
    return ss.str();
}
inline void trace_zesDriverGetExtensionFunctionAddress(zes_driver_handle_t hDriver,
                                                       const char *name,
                                                       void **ppFunctionAddress) {
    TRACE_EVENT_BEGIN("API", "zesDriverGetExtensionFunctionAddress");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDriverGetExtensionFunctionAddress(hDriver, name, ppFunctionAddress) +
                         "..\n";
}
inline void trace_zesDriverGetExtensionFunctionAddress(ze_result_t ret,
                                                       zes_driver_handle_t hDriver,
                                                       const char *name,
                                                       void **ppFunctionAddress) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDriverGetExtensionFunctionAddress(hDriver, name, ppFunctionAddress) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesDeviceGet(zes_driver_handle_t hDriver, uint32_t *pCount, zes_device_handle_t *phDevices) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceGet(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phDevices == nullptr) {
        ss << ", phDevices: nullptr";
    } else {
        ss << ", phDevices: " << *phDevices;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zesDeviceGet(zes_driver_handle_t hDriver, uint32_t *pCount, zes_device_handle_t *phDevices) {
    TRACE_EVENT_BEGIN("API", "zesDeviceGet");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGet(hDriver, pCount, phDevices) + "..\n";
}
inline void trace_zesDeviceGet(ze_result_t ret,
                               zes_driver_handle_t hDriver,
                               uint32_t *pCount,
                               zes_device_handle_t *phDevices) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGet(hDriver, pCount, phDevices) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceGetProperties(zes_device_handle_t hDevice,
                                                 zes_device_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", core {";
        ss << "stype: " << pProperties->core.stype;
        ss << ", pNext: " << pProperties->core.pNext;
        ss << ", type: " << pProperties->core.type;
        ss << ", vendorId: " << pProperties->core.vendorId;
        ss << ", deviceId: " << pProperties->core.deviceId;
        ss << ", flags: " << pProperties->core.flags;
        ss << ", subdeviceId: " << pProperties->core.subdeviceId;
        ss << ", coreClockRate: " << pProperties->core.coreClockRate;
        ss << ", maxMemAllocSize: " << pProperties->core.maxMemAllocSize;
        ss << ", maxHardwareContexts: " << pProperties->core.maxHardwareContexts;
        ss << ", maxCommandQueuePriority: " << pProperties->core.maxCommandQueuePriority;
        ss << ", numThreadsPerEU: " << pProperties->core.numThreadsPerEU;
        ss << ", physicalEUSimdWidth: " << pProperties->core.physicalEUSimdWidth;
        ss << ", numEUsPerSubslice: " << pProperties->core.numEUsPerSubslice;
        ss << ", numSubslicesPerSlice: " << pProperties->core.numSubslicesPerSlice;
        ss << ", numSlices: " << pProperties->core.numSlices;
        ss << ", timerResolution: " << pProperties->core.timerResolution;
        ss << ", timestampValidBits: " << pProperties->core.timestampValidBits;
        ss << ", kernelTimestampValidBits: " << pProperties->core.kernelTimestampValidBits;
        ss << ", uuid {";
        ss << "id: ";
        ss << trace_u8_array_hex(pProperties->core.uuid.id, sizeof(pProperties->core.uuid.id))
                  .str();
        ss << "}";
        ss << ", name: "
           << trace_ascii_array(pProperties->core.name, sizeof(pProperties->core.name)).str();
        ss << "}";
        ss << ", numSubdevices: " << pProperties->numSubdevices;
        ss << ", serialNumber: "
           << trace_ascii_array(pProperties->serialNumber, sizeof(pProperties->serialNumber)).str();
        ss << ", boardNumber: "
           << trace_ascii_array(pProperties->boardNumber, sizeof(pProperties->boardNumber)).str();
        ss << ", brandName: "
           << trace_ascii_array(pProperties->brandName, sizeof(pProperties->brandName)).str();
        ss << ", modelName: "
           << trace_ascii_array(pProperties->modelName, sizeof(pProperties->modelName)).str();
        ss << ", vendorName: "
           << trace_ascii_array(pProperties->vendorName, sizeof(pProperties->vendorName)).str();
        ss << ", driverVersion: "
           << trace_ascii_array(pProperties->driverVersion, sizeof(pProperties->driverVersion))
                  .str();
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceGetProperties(zes_device_handle_t hDevice,
                                         zes_device_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesDeviceGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetProperties(hDevice, pProperties) + "..\n";
}
inline void trace_zesDeviceGetProperties(ze_result_t ret,
                                         zes_device_handle_t hDevice,
                                         zes_device_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetProperties(hDevice, pProperties) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceGetState(zes_device_handle_t hDevice,
                                            zes_device_state_t *pState) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceGetState(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pState == nullptr) {
        ss << ", pState: nullptr";
    } else {
        ss << ", pState {";
        ss << "stype: " << pState->stype;
        ss << ", pNext: " << pState->pNext;
        ss << ", reset: " << pState->reset;
        ss << ", repaired: " << pState->repaired;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceGetState(zes_device_handle_t hDevice, zes_device_state_t *pState) {
    TRACE_EVENT_BEGIN("API", "zesDeviceGetState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetState(hDevice, pState) + "..\n";
}
inline void
trace_zesDeviceGetState(ze_result_t ret, zes_device_handle_t hDevice, zes_device_state_t *pState) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetState(hDevice, pState) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceReset(zes_device_handle_t hDevice, ze_bool_t force) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceReset(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ", force: " << force;
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceReset(zes_device_handle_t hDevice, ze_bool_t force) {
    TRACE_EVENT_BEGIN("API", "zesDeviceReset");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceReset(hDevice, force) + "..\n";
}
inline void trace_zesDeviceReset(ze_result_t ret, zes_device_handle_t hDevice, ze_bool_t force) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceReset(hDevice, force) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceResetExt(zes_device_handle_t hDevice,
                                            zes_reset_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceResetExt(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", force: " << pProperties->force;
        ss << ", resetType: " << pProperties->resetType;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceResetExt(zes_device_handle_t hDevice,
                                    zes_reset_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesDeviceResetExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceResetExt(hDevice, pProperties) + "..\n";
}
inline void trace_zesDeviceResetExt(ze_result_t ret,
                                    zes_device_handle_t hDevice,
                                    zes_reset_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceResetExt(hDevice, pProperties) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceProcessesGetState(zes_device_handle_t hDevice,
                                                     uint32_t *pCount,
                                                     zes_process_state_t *pProcesses) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceProcessesGetState(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pProcesses == nullptr) {
        ss << ", pProcesses: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pProcesses[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pProcesses[0].stype;
            ss << ", pNext: " << pProcesses[0].pNext;
            ss << ", processId: " << pProcesses[0].processId;
            ss << ", memSize: " << pProcesses[0].memSize;
            ss << ", sharedSize: " << pProcesses[0].sharedSize;
            ss << ", engines: " << pProcesses[0].engines;
            ss << "}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceProcessesGetState(zes_device_handle_t hDevice,
                                             uint32_t *pCount,
                                             zes_process_state_t *pProcesses) {
    TRACE_EVENT_BEGIN("API", "zesDeviceProcessesGetState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceProcessesGetState(hDevice, pCount, pProcesses) + "..\n";
}
inline void trace_zesDeviceProcessesGetState(ze_result_t ret,
                                             zes_device_handle_t hDevice,
                                             uint32_t *pCount,
                                             zes_process_state_t *pProcesses) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceProcessesGetState(hDevice, pCount, pProcesses) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDevicePciGetProperties(zes_device_handle_t hDevice,
                                                    zes_pci_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDevicePciGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", address {";
        ss << "domain: " << pProperties->address.domain;
        ss << ", bus: " << pProperties->address.bus;
        ss << ", device: " << pProperties->address.device;
        ss << ", function: " << pProperties->address.function;
        ss << "}";
        ss << ", maxSpeed {";
        ss << "gen: " << pProperties->maxSpeed.gen;
        ss << ", width: " << pProperties->maxSpeed.width;
        ss << ", maxBandwidth: " << pProperties->maxSpeed.maxBandwidth;
        ss << "}";
        ss << ", haveBandwidthCounters: " << pProperties->haveBandwidthCounters;
        ss << ", havePacketCounters: " << pProperties->havePacketCounters;
        ss << ", haveReplayCounters: " << pProperties->haveReplayCounters;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDevicePciGetProperties(zes_device_handle_t hDevice,
                                            zes_pci_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesDevicePciGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDevicePciGetProperties(hDevice, pProperties) + "..\n";
}
inline void trace_zesDevicePciGetProperties(ze_result_t ret,
                                            zes_device_handle_t hDevice,
                                            zes_pci_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDevicePciGetProperties(hDevice, pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDevicePciGetState(zes_device_handle_t hDevice,
                                               zes_pci_state_t *pState) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDevicePciGetState(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pState == nullptr) {
        ss << ", pState: nullptr";
    } else {
        ss << ", pState {";
        ss << "stype: " << pState->stype;
        ss << ", pNext: " << pState->pNext;
        ss << ", status: " << pState->status;
        ss << ", qualityIssues: " << pState->qualityIssues;
        ss << ", stabilityIssues: " << pState->stabilityIssues;
        ss << ", speed {";
        ss << "gen: " << pState->speed.gen;
        ss << ", width: " << pState->speed.width;
        ss << ", maxBandwidth: " << pState->speed.maxBandwidth;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDevicePciGetState(zes_device_handle_t hDevice, zes_pci_state_t *pState) {
    TRACE_EVENT_BEGIN("API", "zesDevicePciGetState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDevicePciGetState(hDevice, pState) + "..\n";
}
inline void
trace_zesDevicePciGetState(ze_result_t ret, zes_device_handle_t hDevice, zes_pci_state_t *pState) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDevicePciGetState(hDevice, pState) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDevicePciGetBars(zes_device_handle_t hDevice,
                                              uint32_t *pCount,
                                              zes_pci_bar_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDevicePciGetBars(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pProperties[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pProperties[0].stype;
            ss << ", pNext: " << pProperties[0].pNext;
            ss << ", type: " << pProperties[0].type;
            ss << ", index: " << pProperties[0].index;
            ss << ", base: " << pProperties[0].base;
            ss << ", size: " << pProperties[0].size;
            ss << "}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDevicePciGetBars(zes_device_handle_t hDevice,
                                      uint32_t *pCount,
                                      zes_pci_bar_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesDevicePciGetBars");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDevicePciGetBars(hDevice, pCount, pProperties) + "..\n";
}
inline void trace_zesDevicePciGetBars(ze_result_t ret,
                                      zes_device_handle_t hDevice,
                                      uint32_t *pCount,
                                      zes_pci_bar_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDevicePciGetBars(hDevice, pCount, pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDevicePciGetStats(zes_device_handle_t hDevice,
                                               zes_pci_stats_t *pStats) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDevicePciGetStats(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pStats == nullptr) {
        ss << ", pStats: nullptr";
    } else {
        ss << ", pStats {";
        ss << "timestamp: " << pStats->timestamp;
        ss << ", replayCounter: " << pStats->replayCounter;
        ss << ", packetCounter: " << pStats->packetCounter;
        ss << ", rxCounter: " << pStats->rxCounter;
        ss << ", txCounter: " << pStats->txCounter;
        ss << ", speed {";
        ss << "gen: " << pStats->speed.gen;
        ss << ", width: " << pStats->speed.width;
        ss << ", maxBandwidth: " << pStats->speed.maxBandwidth;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDevicePciGetStats(zes_device_handle_t hDevice, zes_pci_stats_t *pStats) {
    TRACE_EVENT_BEGIN("API", "zesDevicePciGetStats");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDevicePciGetStats(hDevice, pStats) + "..\n";
}
inline void
trace_zesDevicePciGetStats(ze_result_t ret, zes_device_handle_t hDevice, zes_pci_stats_t *pStats) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDevicePciGetStats(hDevice, pStats) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceSetOverclockWaiver(zes_device_handle_t hDevice) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceSetOverclockWaiver(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceSetOverclockWaiver(zes_device_handle_t hDevice) {
    TRACE_EVENT_BEGIN("API", "zesDeviceSetOverclockWaiver");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceSetOverclockWaiver(hDevice) + "..\n";
}
inline void trace_zesDeviceSetOverclockWaiver(ze_result_t ret, zes_device_handle_t hDevice) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceSetOverclockWaiver(hDevice) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceGetOverclockDomains(zes_device_handle_t hDevice,
                                                       uint32_t *pOverclockDomains) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceGetOverclockDomains(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pOverclockDomains == nullptr) {
        ss << ", pOverclockDomains: nullptr";
    } else {
        ss << ", pOverclockDomains: " << *pOverclockDomains;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceGetOverclockDomains(zes_device_handle_t hDevice,
                                               uint32_t *pOverclockDomains) {
    TRACE_EVENT_BEGIN("API", "zesDeviceGetOverclockDomains");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetOverclockDomains(hDevice, pOverclockDomains) + "..\n";
}
inline void trace_zesDeviceGetOverclockDomains(ze_result_t ret,
                                               zes_device_handle_t hDevice,
                                               uint32_t *pOverclockDomains) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetOverclockDomains(hDevice, pOverclockDomains) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceGetOverclockControls(zes_device_handle_t hDevice,
                                                        zes_overclock_domain_t domainType,
                                                        uint32_t *pAvailableControls) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceGetOverclockControls(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ", domainType: " << domainType;
    if (pAvailableControls == nullptr) {
        ss << ", pAvailableControls: nullptr";
    } else {
        ss << ", pAvailableControls: " << *pAvailableControls;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceGetOverclockControls(zes_device_handle_t hDevice,
                                                zes_overclock_domain_t domainType,
                                                uint32_t *pAvailableControls) {
    TRACE_EVENT_BEGIN("API", "zesDeviceGetOverclockControls");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetOverclockControls(hDevice, domainType, pAvailableControls) +
                         "..\n";
}
inline void trace_zesDeviceGetOverclockControls(ze_result_t ret,
                                                zes_device_handle_t hDevice,
                                                zes_overclock_domain_t domainType,
                                                uint32_t *pAvailableControls) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetOverclockControls(hDevice, domainType, pAvailableControls) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceResetOverclockSettings(zes_device_handle_t hDevice,
                                                          ze_bool_t onShippedState) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceResetOverclockSettings(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ", onShippedState: " << onShippedState;
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceResetOverclockSettings(zes_device_handle_t hDevice,
                                                  ze_bool_t onShippedState) {
    TRACE_EVENT_BEGIN("API", "zesDeviceResetOverclockSettings");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceResetOverclockSettings(hDevice, onShippedState) + "..\n";
}
inline void trace_zesDeviceResetOverclockSettings(ze_result_t ret,
                                                  zes_device_handle_t hDevice,
                                                  ze_bool_t onShippedState) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceResetOverclockSettings(hDevice, onShippedState) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceReadOverclockState(zes_device_handle_t hDevice,
                                                      zes_overclock_mode_t *pOverclockMode,
                                                      ze_bool_t *pWaiverSetting,
                                                      ze_bool_t *pOverclockState,
                                                      zes_pending_action_t *pPendingAction,
                                                      ze_bool_t *pPendingReset) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceReadOverclockState(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pOverclockMode == nullptr) {
        ss << ", pOverclockMode: nullptr";
    } else {
        ss << ", pOverclockMode: " << *pOverclockMode;
    }
    if (pWaiverSetting == nullptr) {
        ss << ", pWaiverSetting: nullptr";
    } else {
        ss << ", pWaiverSetting: " << *pWaiverSetting;
    }
    if (pOverclockState == nullptr) {
        ss << ", pOverclockState: nullptr";
    } else {
        ss << ", pOverclockState: " << *pOverclockState;
    }
    if (pPendingAction == nullptr) {
        ss << ", pPendingAction: nullptr";
    } else {
        ss << ", pPendingAction: " << *pPendingAction;
    }
    if (pPendingReset == nullptr) {
        ss << ", pPendingReset: nullptr";
    } else {
        ss << ", pPendingReset: " << *pPendingReset;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceReadOverclockState(zes_device_handle_t hDevice,
                                              zes_overclock_mode_t *pOverclockMode,
                                              ze_bool_t *pWaiverSetting,
                                              ze_bool_t *pOverclockState,
                                              zes_pending_action_t *pPendingAction,
                                              ze_bool_t *pPendingReset) {
    TRACE_EVENT_BEGIN("API", "zesDeviceReadOverclockState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceReadOverclockState(hDevice,
                                                        pOverclockMode,
                                                        pWaiverSetting,
                                                        pOverclockState,
                                                        pPendingAction,
                                                        pPendingReset) +
                         "..\n";
}
inline void trace_zesDeviceReadOverclockState(ze_result_t ret,
                                              zes_device_handle_t hDevice,
                                              zes_overclock_mode_t *pOverclockMode,
                                              ze_bool_t *pWaiverSetting,
                                              ze_bool_t *pOverclockState,
                                              zes_pending_action_t *pPendingAction,
                                              ze_bool_t *pPendingReset) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceReadOverclockState(hDevice,
                                                        pOverclockMode,
                                                        pWaiverSetting,
                                                        pOverclockState,
                                                        pPendingAction,
                                                        pPendingReset) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumOverclockDomains(zes_device_handle_t hDevice,
                                                        uint32_t *pCount,
                                                        zes_overclock_handle_t *phDomainHandle) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumOverclockDomains(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phDomainHandle == nullptr) {
        ss << ", phDomainHandle: nullptr";
    } else {
        ss << ", phDomainHandle: " << *phDomainHandle;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumOverclockDomains(zes_device_handle_t hDevice,
                                                uint32_t *pCount,
                                                zes_overclock_handle_t *phDomainHandle) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumOverclockDomains");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumOverclockDomains(hDevice, pCount, phDomainHandle) + "..\n";
}
inline void trace_zesDeviceEnumOverclockDomains(ze_result_t ret,
                                                zes_device_handle_t hDevice,
                                                uint32_t *pCount,
                                                zes_overclock_handle_t *phDomainHandle) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumOverclockDomains(hDevice, pCount, phDomainHandle) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesOverclockGetDomainProperties(zes_overclock_handle_t hDomainHandle,
                                       zes_overclock_properties_t *pDomainProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesOverclockGetDomainProperties(";
    ss << std::hex << std::showbase;
    ss << "hDomainHandle: " << hDomainHandle;
    if (pDomainProperties == nullptr) {
        ss << ", pDomainProperties: nullptr";
    } else {
        ss << ", pDomainProperties {";
        ss << "stype: " << pDomainProperties->stype;
        ss << ", pNext: " << pDomainProperties->pNext;
        ss << ", domainType: " << pDomainProperties->domainType;
        ss << ", AvailableControls: " << pDomainProperties->AvailableControls;
        ss << ", VFProgramType: " << pDomainProperties->VFProgramType;
        ss << ", NumberOfVFPoints: " << pDomainProperties->NumberOfVFPoints;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesOverclockGetDomainProperties(zes_overclock_handle_t hDomainHandle,
                                                  zes_overclock_properties_t *pDomainProperties) {
    TRACE_EVENT_BEGIN("API", "zesOverclockGetDomainProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetDomainProperties(hDomainHandle, pDomainProperties) +
                         "..\n";
}
inline void trace_zesOverclockGetDomainProperties(ze_result_t ret,
                                                  zes_overclock_handle_t hDomainHandle,
                                                  zes_overclock_properties_t *pDomainProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetDomainProperties(hDomainHandle, pDomainProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesOverclockGetDomainVFProperties(zes_overclock_handle_t hDomainHandle,
                                                            zes_vf_property_t *pVFProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesOverclockGetDomainVFProperties(";
    ss << std::hex << std::showbase;
    ss << "hDomainHandle: " << hDomainHandle;
    if (pVFProperties == nullptr) {
        ss << ", pVFProperties: nullptr";
    } else {
        ss << ", pVFProperties {";
        ss << "MinFreq: " << pVFProperties->MinFreq;
        ss << ", MaxFreq: " << pVFProperties->MaxFreq;
        ss << ", StepFreq: " << pVFProperties->StepFreq;
        ss << ", MinVolt: " << pVFProperties->MinVolt;
        ss << ", MaxVolt: " << pVFProperties->MaxVolt;
        ss << ", StepVolt: " << pVFProperties->StepVolt;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesOverclockGetDomainVFProperties(zes_overclock_handle_t hDomainHandle,
                                                    zes_vf_property_t *pVFProperties) {
    TRACE_EVENT_BEGIN("API", "zesOverclockGetDomainVFProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetDomainVFProperties(hDomainHandle, pVFProperties) +
                         "..\n";
}
inline void trace_zesOverclockGetDomainVFProperties(ze_result_t ret,
                                                    zes_overclock_handle_t hDomainHandle,
                                                    zes_vf_property_t *pVFProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetDomainVFProperties(hDomainHandle, pVFProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesOverclockGetDomainControlProperties(zes_overclock_handle_t hDomainHandle,
                                              zes_overclock_control_t DomainControl,
                                              zes_control_property_t *pControlProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesOverclockGetDomainControlProperties(";
    ss << std::hex << std::showbase;
    ss << "hDomainHandle: " << hDomainHandle;
    ss << ", DomainControl: " << DomainControl;
    if (pControlProperties == nullptr) {
        ss << ", pControlProperties: nullptr";
    } else {
        ss << ", pControlProperties {";
        ss << "MinValue: " << pControlProperties->MinValue;
        ss << ", MaxValue: " << pControlProperties->MaxValue;
        ss << ", StepValue: " << pControlProperties->StepValue;
        ss << ", RefValue: " << pControlProperties->RefValue;
        ss << ", DefaultValue: " << pControlProperties->DefaultValue;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zesOverclockGetDomainControlProperties(zes_overclock_handle_t hDomainHandle,
                                             zes_overclock_control_t DomainControl,
                                             zes_control_property_t *pControlProperties) {
    TRACE_EVENT_BEGIN("API", "zesOverclockGetDomainControlProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetDomainControlProperties(hDomainHandle,
                                                                   DomainControl,
                                                                   pControlProperties) +
                         "..\n";
}
inline void
trace_zesOverclockGetDomainControlProperties(ze_result_t ret,
                                             zes_overclock_handle_t hDomainHandle,
                                             zes_overclock_control_t DomainControl,
                                             zes_control_property_t *pControlProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetDomainControlProperties(hDomainHandle,
                                                                   DomainControl,
                                                                   pControlProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesOverclockGetControlCurrentValue(zes_overclock_handle_t hDomainHandle,
                                                             zes_overclock_control_t DomainControl,
                                                             double *pValue) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesOverclockGetControlCurrentValue(";
    ss << std::hex << std::showbase;
    ss << "hDomainHandle: " << hDomainHandle;
    ss << ", DomainControl: " << DomainControl;
    if (pValue == nullptr) {
        ss << ", pValue: nullptr";
    } else {
        ss << ", pValue: " << *pValue;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesOverclockGetControlCurrentValue(zes_overclock_handle_t hDomainHandle,
                                                     zes_overclock_control_t DomainControl,
                                                     double *pValue) {
    TRACE_EVENT_BEGIN("API", "zesOverclockGetControlCurrentValue");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetControlCurrentValue(hDomainHandle,
                                                               DomainControl,
                                                               pValue) +
                         "..\n";
}
inline void trace_zesOverclockGetControlCurrentValue(ze_result_t ret,
                                                     zes_overclock_handle_t hDomainHandle,
                                                     zes_overclock_control_t DomainControl,
                                                     double *pValue) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetControlCurrentValue(hDomainHandle,
                                                               DomainControl,
                                                               pValue) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesOverclockGetControlPendingValue(zes_overclock_handle_t hDomainHandle,
                                                             zes_overclock_control_t DomainControl,
                                                             double *pValue) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesOverclockGetControlPendingValue(";
    ss << std::hex << std::showbase;
    ss << "hDomainHandle: " << hDomainHandle;
    ss << ", DomainControl: " << DomainControl;
    if (pValue == nullptr) {
        ss << ", pValue: nullptr";
    } else {
        ss << ", pValue: " << *pValue;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesOverclockGetControlPendingValue(zes_overclock_handle_t hDomainHandle,
                                                     zes_overclock_control_t DomainControl,
                                                     double *pValue) {
    TRACE_EVENT_BEGIN("API", "zesOverclockGetControlPendingValue");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetControlPendingValue(hDomainHandle,
                                                               DomainControl,
                                                               pValue) +
                         "..\n";
}
inline void trace_zesOverclockGetControlPendingValue(ze_result_t ret,
                                                     zes_overclock_handle_t hDomainHandle,
                                                     zes_overclock_control_t DomainControl,
                                                     double *pValue) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetControlPendingValue(hDomainHandle,
                                                               DomainControl,
                                                               pValue) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesOverclockSetControlUserValue(zes_overclock_handle_t hDomainHandle,
                                                          zes_overclock_control_t DomainControl,
                                                          double pValue,
                                                          zes_pending_action_t *pPendingAction) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesOverclockSetControlUserValue(";
    ss << std::hex << std::showbase;
    ss << "hDomainHandle: " << hDomainHandle;
    ss << ", DomainControl: " << DomainControl;
    ss << ", pValue: " << pValue;
    if (pPendingAction == nullptr) {
        ss << ", pPendingAction: nullptr";
    } else {
        ss << ", pPendingAction: " << *pPendingAction;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesOverclockSetControlUserValue(zes_overclock_handle_t hDomainHandle,
                                                  zes_overclock_control_t DomainControl,
                                                  double pValue,
                                                  zes_pending_action_t *pPendingAction) {
    TRACE_EVENT_BEGIN("API", "zesOverclockSetControlUserValue");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockSetControlUserValue(hDomainHandle,
                                                            DomainControl,
                                                            pValue,
                                                            pPendingAction) +
                         "..\n";
}
inline void trace_zesOverclockSetControlUserValue(ze_result_t ret,
                                                  zes_overclock_handle_t hDomainHandle,
                                                  zes_overclock_control_t DomainControl,
                                                  double pValue,
                                                  zes_pending_action_t *pPendingAction) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockSetControlUserValue(hDomainHandle,
                                                            DomainControl,
                                                            pValue,
                                                            pPendingAction) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesOverclockGetControlState(zes_overclock_handle_t hDomainHandle,
                                                      zes_overclock_control_t DomainControl,
                                                      zes_control_state_t *pControlState,
                                                      zes_pending_action_t *pPendingAction) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesOverclockGetControlState(";
    ss << std::hex << std::showbase;
    ss << "hDomainHandle: " << hDomainHandle;
    ss << ", DomainControl: " << DomainControl;
    if (pControlState == nullptr) {
        ss << ", pControlState: nullptr";
    } else {
        ss << ", pControlState: " << *pControlState;
    }
    if (pPendingAction == nullptr) {
        ss << ", pPendingAction: nullptr";
    } else {
        ss << ", pPendingAction: " << *pPendingAction;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesOverclockGetControlState(zes_overclock_handle_t hDomainHandle,
                                              zes_overclock_control_t DomainControl,
                                              zes_control_state_t *pControlState,
                                              zes_pending_action_t *pPendingAction) {
    TRACE_EVENT_BEGIN("API", "zesOverclockGetControlState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetControlState(hDomainHandle,
                                                        DomainControl,
                                                        pControlState,
                                                        pPendingAction) +
                         "..\n";
}
inline void trace_zesOverclockGetControlState(ze_result_t ret,
                                              zes_overclock_handle_t hDomainHandle,
                                              zes_overclock_control_t DomainControl,
                                              zes_control_state_t *pControlState,
                                              zes_pending_action_t *pPendingAction) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetControlState(hDomainHandle,
                                                        DomainControl,
                                                        pControlState,
                                                        pPendingAction) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesOverclockGetVFPointValues(zes_overclock_handle_t hDomainHandle,
                                                       zes_vf_type_t VFType,
                                                       zes_vf_array_type_t VFArrayType,
                                                       uint32_t PointIndex,
                                                       uint32_t *PointValue) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesOverclockGetVFPointValues(";
    ss << std::hex << std::showbase;
    ss << "hDomainHandle: " << hDomainHandle;
    ss << ", VFType: " << VFType;
    ss << ", VFArrayType: " << VFArrayType;
    ss << ", PointIndex: " << PointIndex;
    if (PointValue == nullptr) {
        ss << ", PointValue: nullptr";
    } else {
        ss << ", PointValue: " << *PointValue;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesOverclockGetVFPointValues(zes_overclock_handle_t hDomainHandle,
                                               zes_vf_type_t VFType,
                                               zes_vf_array_type_t VFArrayType,
                                               uint32_t PointIndex,
                                               uint32_t *PointValue) {
    TRACE_EVENT_BEGIN("API", "zesOverclockGetVFPointValues");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetVFPointValues(hDomainHandle,
                                                         VFType,
                                                         VFArrayType,
                                                         PointIndex,
                                                         PointValue) +
                         "..\n";
}
inline void trace_zesOverclockGetVFPointValues(ze_result_t ret,
                                               zes_overclock_handle_t hDomainHandle,
                                               zes_vf_type_t VFType,
                                               zes_vf_array_type_t VFArrayType,
                                               uint32_t PointIndex,
                                               uint32_t *PointValue) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockGetVFPointValues(hDomainHandle,
                                                         VFType,
                                                         VFArrayType,
                                                         PointIndex,
                                                         PointValue) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesOverclockSetVFPointValues(zes_overclock_handle_t hDomainHandle,
                                                       zes_vf_type_t VFType,
                                                       uint32_t PointIndex,
                                                       uint32_t PointValue) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesOverclockSetVFPointValues(";
    ss << std::hex << std::showbase;
    ss << "hDomainHandle: " << hDomainHandle;
    ss << ", VFType: " << VFType;
    ss << ", PointIndex: " << PointIndex;
    ss << ", PointValue: " << PointValue;
    ss << ")";
    return ss.str();
}
inline void trace_zesOverclockSetVFPointValues(zes_overclock_handle_t hDomainHandle,
                                               zes_vf_type_t VFType,
                                               uint32_t PointIndex,
                                               uint32_t PointValue) {
    TRACE_EVENT_BEGIN("API", "zesOverclockSetVFPointValues");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockSetVFPointValues(hDomainHandle,
                                                         VFType,
                                                         PointIndex,
                                                         PointValue) +
                         "..\n";
}
inline void trace_zesOverclockSetVFPointValues(ze_result_t ret,
                                               zes_overclock_handle_t hDomainHandle,
                                               zes_vf_type_t VFType,
                                               uint32_t PointIndex,
                                               uint32_t PointValue) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesOverclockSetVFPointValues(hDomainHandle,
                                                         VFType,
                                                         PointIndex,
                                                         PointValue) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumDiagnosticTestSuites(zes_device_handle_t hDevice,
                                                            uint32_t *pCount,
                                                            zes_diag_handle_t *phDiagnostics) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumDiagnosticTestSuites(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phDiagnostics == nullptr) {
        ss << ", phDiagnostics: nullptr";
    } else {
        ss << ", phDiagnostics: " << *phDiagnostics;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumDiagnosticTestSuites(zes_device_handle_t hDevice,
                                                    uint32_t *pCount,
                                                    zes_diag_handle_t *phDiagnostics) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumDiagnosticTestSuites");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumDiagnosticTestSuites(hDevice, pCount, phDiagnostics) +
                         "..\n";
}
inline void trace_zesDeviceEnumDiagnosticTestSuites(ze_result_t ret,
                                                    zes_device_handle_t hDevice,
                                                    uint32_t *pCount,
                                                    zes_diag_handle_t *phDiagnostics) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumDiagnosticTestSuites(hDevice, pCount, phDiagnostics) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDiagnosticsGetProperties(zes_diag_handle_t hDiagnostics,
                                                      zes_diag_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDiagnosticsGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hDiagnostics: " << hDiagnostics;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", name: ";
        ss << trace_ascii_array(pProperties->name, sizeof(pProperties->name)).str();
        ss << ", haveTests: " << pProperties->haveTests;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDiagnosticsGetProperties(zes_diag_handle_t hDiagnostics,
                                              zes_diag_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesDiagnosticsGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDiagnosticsGetProperties(hDiagnostics, pProperties) + "..\n";
}
inline void trace_zesDiagnosticsGetProperties(ze_result_t ret,
                                              zes_diag_handle_t hDiagnostics,
                                              zes_diag_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDiagnosticsGetProperties(hDiagnostics, pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDiagnosticsGetTests(zes_diag_handle_t hDiagnostics,
                                                 uint32_t *pCount,
                                                 zes_diag_test_t *pTests) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDiagnosticsGetTests(";
    ss << std::hex << std::showbase;
    ss << "hDiagnostics: " << hDiagnostics;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pTests == nullptr) {
        ss << ", pTests: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pTests[";
        if (count > 0) {
            ss << "{";
            ss << "index: " << pTests[0].index;
            ss << ", name: ";
            ss << trace_ascii_array(pTests[0].name, sizeof(pTests[0].name)).str();
            ss << "}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDiagnosticsGetTests(zes_diag_handle_t hDiagnostics,
                                         uint32_t *pCount,
                                         zes_diag_test_t *pTests) {
    TRACE_EVENT_BEGIN("API", "zesDiagnosticsGetTests");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDiagnosticsGetTests(hDiagnostics, pCount, pTests) + "..\n";
}
inline void trace_zesDiagnosticsGetTests(ze_result_t ret,
                                         zes_diag_handle_t hDiagnostics,
                                         uint32_t *pCount,
                                         zes_diag_test_t *pTests) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDiagnosticsGetTests(hDiagnostics, pCount, pTests) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDiagnosticsRunTests(zes_diag_handle_t hDiagnostics,
                                                 uint32_t startIndex,
                                                 uint32_t endIndex,
                                                 zes_diag_result_t *pResult) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDiagnosticsRunTests(";
    ss << std::hex << std::showbase;
    ss << "hDiagnostics: " << hDiagnostics;
    ss << ", startIndex: " << startIndex;
    ss << ", endIndex: " << endIndex;
    if (pResult == nullptr) {
        ss << ", pResult: nullptr";
    } else {
        ss << ", pResult: " << *pResult;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDiagnosticsRunTests(zes_diag_handle_t hDiagnostics,
                                         uint32_t startIndex,
                                         uint32_t endIndex,
                                         zes_diag_result_t *pResult) {
    TRACE_EVENT_BEGIN("API", "zesDiagnosticsRunTests");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDiagnosticsRunTests(hDiagnostics, startIndex, endIndex, pResult) +
                         "..\n";
}
inline void trace_zesDiagnosticsRunTests(ze_result_t ret,
                                         zes_diag_handle_t hDiagnostics,
                                         uint32_t startIndex,
                                         uint32_t endIndex,
                                         zes_diag_result_t *pResult) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDiagnosticsRunTests(hDiagnostics, startIndex, endIndex, pResult) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEccAvailable(zes_device_handle_t hDevice,
                                                ze_bool_t *pAvailable) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEccAvailable(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pAvailable == nullptr) {
        ss << ", pAvailable: nullptr";
    } else {
        ss << ", pAvailable: " << *pAvailable;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEccAvailable(zes_device_handle_t hDevice, ze_bool_t *pAvailable) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEccAvailable");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEccAvailable(hDevice, pAvailable) + "..\n";
}
inline void
trace_zesDeviceEccAvailable(ze_result_t ret, zes_device_handle_t hDevice, ze_bool_t *pAvailable) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEccAvailable(hDevice, pAvailable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEccConfigurable(zes_device_handle_t hDevice,
                                                   ze_bool_t *pConfigurable) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEccConfigurable(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pConfigurable == nullptr) {
        ss << ", pConfigurable: nullptr";
    } else {
        ss << ", pConfigurable: " << *pConfigurable;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEccConfigurable(zes_device_handle_t hDevice, ze_bool_t *pConfigurable) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEccConfigurable");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEccConfigurable(hDevice, pConfigurable) + "..\n";
}
inline void trace_zesDeviceEccConfigurable(ze_result_t ret,
                                           zes_device_handle_t hDevice,
                                           ze_bool_t *pConfigurable) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEccConfigurable(hDevice, pConfigurable) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceGetEccState(zes_device_handle_t hDevice,
                                               zes_device_ecc_properties_t *pState) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceGetEccState(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pState == nullptr) {
        ss << ", pState: nullptr";
    } else {
        ss << ", pState {";
        ss << "stype: " << pState->stype;
        ss << ", pNext: " << pState->pNext;
        ss << ", currentState: " << pState->currentState;
        ss << ", pendingState: " << pState->pendingState;
        ss << ", pendingAction: " << pState->pendingAction;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceGetEccState(zes_device_handle_t hDevice,
                                       zes_device_ecc_properties_t *pState) {
    TRACE_EVENT_BEGIN("API", "zesDeviceGetEccState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetEccState(hDevice, pState) + "..\n";
}
inline void trace_zesDeviceGetEccState(ze_result_t ret,
                                       zes_device_handle_t hDevice,
                                       zes_device_ecc_properties_t *pState) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetEccState(hDevice, pState) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceSetEccState(zes_device_handle_t hDevice,
                                               const zes_device_ecc_desc_t *newState,
                                               zes_device_ecc_properties_t *pState) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceSetEccState(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (newState == nullptr) {
        ss << ", newState: nullptr";
    } else {
        ss << ", newState {";
        ss << "stype: " << newState->stype;
        ss << ", pNext: " << newState->pNext;
        ss << ", state: " << newState->state;
        ss << "}";
    }
    if (pState == nullptr) {
        ss << ", pState: nullptr";
    } else {
        ss << ", pState {";
        ss << "stype: " << pState->stype;
        ss << ", pNext: " << pState->pNext;
        ss << ", currentState: " << pState->currentState;
        ss << ", pendingState: " << pState->pendingState;
        ss << ", pendingAction: " << pState->pendingAction;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceSetEccState(zes_device_handle_t hDevice,
                                       const zes_device_ecc_desc_t *newState,
                                       zes_device_ecc_properties_t *pState) {
    TRACE_EVENT_BEGIN("API", "zesDeviceSetEccState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceSetEccState(hDevice, newState, pState) + "..\n";
}
inline void trace_zesDeviceSetEccState(ze_result_t ret,
                                       zes_device_handle_t hDevice,
                                       const zes_device_ecc_desc_t *newState,
                                       zes_device_ecc_properties_t *pState) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceSetEccState(hDevice, newState, pState) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumEngineGroups(zes_device_handle_t hDevice,
                                                    uint32_t *pCount,
                                                    zes_engine_handle_t *phEngine) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumEngineGroups(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phEngine == nullptr) {
        ss << ", phEngine: nullptr";
    } else {
        ss << ", phEngine: " << *phEngine;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumEngineGroups(zes_device_handle_t hDevice,
                                            uint32_t *pCount,
                                            zes_engine_handle_t *phEngine) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumEngineGroups");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumEngineGroups(hDevice, pCount, phEngine) + "..\n";
}
inline void trace_zesDeviceEnumEngineGroups(ze_result_t ret,
                                            zes_device_handle_t hDevice,
                                            uint32_t *pCount,
                                            zes_engine_handle_t *phEngine) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumEngineGroups(hDevice, pCount, phEngine) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesEngineGetProperties(zes_engine_handle_t hEngine,
                                                 zes_engine_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesEngineGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hEngine: " << hEngine;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", type: " << pProperties->type;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesEngineGetProperties(zes_engine_handle_t hEngine,
                                         zes_engine_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesEngineGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesEngineGetProperties(hEngine, pProperties) + "..\n";
}
inline void trace_zesEngineGetProperties(ze_result_t ret,
                                         zes_engine_handle_t hEngine,
                                         zes_engine_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesEngineGetProperties(hEngine, pProperties) + trace_ze_result_t(ret);
}

inline std::string _trace_zesEngineGetActivity(zes_engine_handle_t hEngine,
                                               zes_engine_stats_t *pStats) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesEngineGetActivity(";
    ss << std::hex << std::showbase;
    ss << "hEngine: " << hEngine;
    if (pStats == nullptr) {
        ss << ", pStats: nullptr";
    } else {
        ss << ", pStats {";
        ss << "activeTime: " << pStats->activeTime;
        ss << ", timestamp: " << pStats->timestamp;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesEngineGetActivity(zes_engine_handle_t hEngine, zes_engine_stats_t *pStats) {
    TRACE_EVENT_BEGIN("API", "zesEngineGetActivity");
    if (IS_API_TRACE())
        std::cerr << _trace_zesEngineGetActivity(hEngine, pStats) + "..\n";
}
inline void trace_zesEngineGetActivity(ze_result_t ret,
                                       zes_engine_handle_t hEngine,
                                       zes_engine_stats_t *pStats) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesEngineGetActivity(hEngine, pStats) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEventRegister(zes_device_handle_t hDevice,
                                                 zes_event_type_flags_t events) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEventRegister(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ", events: " << events;
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEventRegister(zes_device_handle_t hDevice,
                                         zes_event_type_flags_t events) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEventRegister");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEventRegister(hDevice, events) + "..\n";
}
inline void trace_zesDeviceEventRegister(ze_result_t ret,
                                         zes_device_handle_t hDevice,
                                         zes_event_type_flags_t events) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEventRegister(hDevice, events) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDriverEventListen(ze_driver_handle_t hDriver,
                                               uint32_t timeout,
                                               uint32_t count,
                                               zes_device_handle_t *phDevices,
                                               uint32_t *pNumDeviceEvents,
                                               zes_event_type_flags_t *pEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDriverEventListen(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    ss << ", timeout: " << timeout;
    ss << ", count: " << count;
    if (phDevices == nullptr) {
        ss << ", phDevices: nullptr";
    } else {
        ss << ", phDevices: " << *phDevices;
    }
    if (pNumDeviceEvents == nullptr) {
        ss << ", pNumDeviceEvents: nullptr";
    } else {
        ss << ", pNumDeviceEvents: " << *pNumDeviceEvents;
    }
    if (pEvents == nullptr) {
        ss << ", pEvents: nullptr";
    } else {
        ss << ", pEvents: " << *pEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDriverEventListen(ze_driver_handle_t hDriver,
                                       uint32_t timeout,
                                       uint32_t count,
                                       zes_device_handle_t *phDevices,
                                       uint32_t *pNumDeviceEvents,
                                       zes_event_type_flags_t *pEvents) {
    TRACE_EVENT_BEGIN("API", "zesDriverEventListen");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDriverEventListen(hDriver,
                                                 timeout,
                                                 count,
                                                 phDevices,
                                                 pNumDeviceEvents,
                                                 pEvents) +
                         "..\n";
}
inline void trace_zesDriverEventListen(ze_result_t ret,
                                       ze_driver_handle_t hDriver,
                                       uint32_t timeout,
                                       uint32_t count,
                                       zes_device_handle_t *phDevices,
                                       uint32_t *pNumDeviceEvents,
                                       zes_event_type_flags_t *pEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDriverEventListen(hDriver,
                                                 timeout,
                                                 count,
                                                 phDevices,
                                                 pNumDeviceEvents,
                                                 pEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDriverEventListenEx(ze_driver_handle_t hDriver,
                                                 uint64_t timeout,
                                                 uint32_t count,
                                                 zes_device_handle_t *phDevices,
                                                 uint32_t *pNumDeviceEvents,
                                                 zes_event_type_flags_t *pEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDriverEventListenEx(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    ss << ", timeout: " << timeout;
    ss << ", count: " << count;
    if (phDevices == nullptr) {
        ss << ", phDevices: nullptr";
    } else {
        ss << ", phDevices: " << *phDevices;
    }
    if (pNumDeviceEvents == nullptr) {
        ss << ", pNumDeviceEvents: nullptr";
    } else {
        ss << ", pNumDeviceEvents: " << *pNumDeviceEvents;
    }
    if (pEvents == nullptr) {
        ss << ", pEvents: nullptr";
    } else {
        ss << ", pEvents: " << *pEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDriverEventListenEx(ze_driver_handle_t hDriver,
                                         uint64_t timeout,
                                         uint32_t count,
                                         zes_device_handle_t *phDevices,
                                         uint32_t *pNumDeviceEvents,
                                         zes_event_type_flags_t *pEvents) {
    TRACE_EVENT_BEGIN("API", "zesDriverEventListenEx");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDriverEventListenEx(hDriver,
                                                   timeout,
                                                   count,
                                                   phDevices,
                                                   pNumDeviceEvents,
                                                   pEvents) +
                         "..\n";
}
inline void trace_zesDriverEventListenEx(ze_result_t ret,
                                         ze_driver_handle_t hDriver,
                                         uint64_t timeout,
                                         uint32_t count,
                                         zes_device_handle_t *phDevices,
                                         uint32_t *pNumDeviceEvents,
                                         zes_event_type_flags_t *pEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDriverEventListenEx(hDriver,
                                                   timeout,
                                                   count,
                                                   phDevices,
                                                   pNumDeviceEvents,
                                                   pEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumFabricPorts(zes_device_handle_t hDevice,
                                                   uint32_t *pCount,
                                                   zes_fabric_port_handle_t *phPort) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumFabricPorts(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phPort == nullptr) {
        ss << ", phPort: nullptr";
    } else {
        ss << ", phPort: " << *phPort;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumFabricPorts(zes_device_handle_t hDevice,
                                           uint32_t *pCount,
                                           zes_fabric_port_handle_t *phPort) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumFabricPorts");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumFabricPorts(hDevice, pCount, phPort) + "..\n";
}
inline void trace_zesDeviceEnumFabricPorts(ze_result_t ret,
                                           zes_device_handle_t hDevice,
                                           uint32_t *pCount,
                                           zes_fabric_port_handle_t *phPort) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumFabricPorts(hDevice, pCount, phPort) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFabricPortGetProperties(zes_fabric_port_handle_t hPort,
                                                     zes_fabric_port_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFabricPortGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hPort: " << hPort;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", model: ";
        ss << trace_ascii_array(pProperties->model, sizeof(pProperties->model)).str();
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", portId {";
        ss << "fabricId: " << pProperties->portId.fabricId;
        ss << ", attachId: " << pProperties->portId.attachId;
        ss << ", portNumber: " << pProperties->portId.portNumber;
        ss << "}";
        ss << ", maxRxSpeed {";
        ss << "bitRate: " << pProperties->maxRxSpeed.bitRate;
        ss << ", width: " << pProperties->maxRxSpeed.width;
        ss << "}";
        ss << ", maxTxSpeed {";
        ss << "bitRate: " << pProperties->maxTxSpeed.bitRate;
        ss << ", width: " << pProperties->maxTxSpeed.width;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFabricPortGetProperties(zes_fabric_port_handle_t hPort,
                                             zes_fabric_port_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesFabricPortGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetProperties(hPort, pProperties) + "..\n";
}
inline void trace_zesFabricPortGetProperties(ze_result_t ret,
                                             zes_fabric_port_handle_t hPort,
                                             zes_fabric_port_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetProperties(hPort, pProperties) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFabricPortGetLinkType(zes_fabric_port_handle_t hPort,
                                                   zes_fabric_link_type_t *pLinkType) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFabricPortGetLinkType(";
    ss << std::hex << std::showbase;
    ss << "hPort: " << hPort;
    if (pLinkType == nullptr) {
        ss << ", pLinkType: nullptr";
    } else {
        ss << ", pLinkType {";
        ss << "desc: " << trace_ascii_array(pLinkType->desc, sizeof(pLinkType->desc)).str();
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFabricPortGetLinkType(zes_fabric_port_handle_t hPort,
                                           zes_fabric_link_type_t *pLinkType) {
    TRACE_EVENT_BEGIN("API", "zesFabricPortGetLinkType");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetLinkType(hPort, pLinkType) + "..\n";
}
inline void trace_zesFabricPortGetLinkType(ze_result_t ret,
                                           zes_fabric_port_handle_t hPort,
                                           zes_fabric_link_type_t *pLinkType) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetLinkType(hPort, pLinkType) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFabricPortGetConfig(zes_fabric_port_handle_t hPort,
                                                 zes_fabric_port_config_t *pConfig) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFabricPortGetConfig(";
    ss << std::hex << std::showbase;
    ss << "hPort: " << hPort;
    if (pConfig == nullptr) {
        ss << ", pConfig: nullptr";
    } else {
        ss << ", pConfig {";
        ss << "stype: " << pConfig->stype;
        ss << ", pNext: " << pConfig->pNext;
        ss << ", enabled: " << pConfig->enabled;
        ss << ", beaconing: " << pConfig->beaconing;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFabricPortGetConfig(zes_fabric_port_handle_t hPort,
                                         zes_fabric_port_config_t *pConfig) {
    TRACE_EVENT_BEGIN("API", "zesFabricPortGetConfig");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetConfig(hPort, pConfig) + "..\n";
}
inline void trace_zesFabricPortGetConfig(ze_result_t ret,
                                         zes_fabric_port_handle_t hPort,
                                         zes_fabric_port_config_t *pConfig) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetConfig(hPort, pConfig) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFabricPortSetConfig(zes_fabric_port_handle_t hPort,
                                                 const zes_fabric_port_config_t *pConfig) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFabricPortSetConfig(";
    ss << std::hex << std::showbase;
    ss << "hPort: " << hPort;
    if (pConfig == nullptr) {
        ss << ", pConfig: nullptr";
    } else {
        ss << ", pConfig {";
        ss << "stype: " << pConfig->stype;
        ss << ", pNext: " << pConfig->pNext;
        ss << ", enabled: " << pConfig->enabled;
        ss << ", beaconing: " << pConfig->beaconing;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFabricPortSetConfig(zes_fabric_port_handle_t hPort,
                                         const zes_fabric_port_config_t *pConfig) {
    TRACE_EVENT_BEGIN("API", "zesFabricPortSetConfig");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortSetConfig(hPort, pConfig) + "..\n";
}
inline void trace_zesFabricPortSetConfig(ze_result_t ret,
                                         zes_fabric_port_handle_t hPort,
                                         const zes_fabric_port_config_t *pConfig) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortSetConfig(hPort, pConfig) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFabricPortGetState(zes_fabric_port_handle_t hPort,
                                                zes_fabric_port_state_t *pState) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFabricPortGetState(";
    ss << std::hex << std::showbase;
    ss << "hPort: " << hPort;
    if (pState == nullptr) {
        ss << ", pState: nullptr";
    } else {
        ss << ", pState {";
        ss << "stype: " << pState->stype;
        ss << ", pNext: " << pState->pNext;
        ss << ", status: " << pState->status;
        ss << ", qualityIssues: " << pState->qualityIssues;
        ss << ", failureReasons: " << pState->failureReasons;
        ss << ", remotePortId {";
        ss << "fabricId: " << pState->remotePortId.fabricId;
        ss << ", attachId: " << pState->remotePortId.attachId;
        ss << ", portNumber: " << pState->remotePortId.portNumber;
        ss << "}";
        ss << ", rxSpeed {";
        ss << "bitRate: " << pState->rxSpeed.bitRate;
        ss << ", width: " << pState->rxSpeed.width;
        ss << "}";
        ss << ", txSpeed {";
        ss << "bitRate: " << pState->txSpeed.bitRate;
        ss << ", width: " << pState->txSpeed.width;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFabricPortGetState(zes_fabric_port_handle_t hPort,
                                        zes_fabric_port_state_t *pState) {
    TRACE_EVENT_BEGIN("API", "zesFabricPortGetState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetState(hPort, pState) + "..\n";
}
inline void trace_zesFabricPortGetState(ze_result_t ret,
                                        zes_fabric_port_handle_t hPort,
                                        zes_fabric_port_state_t *pState) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetState(hPort, pState) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFabricPortGetThroughput(zes_fabric_port_handle_t hPort,
                                                     zes_fabric_port_throughput_t *pThroughput) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFabricPortGetThroughput(";
    ss << std::hex << std::showbase;
    ss << "hPort: " << hPort;
    if (pThroughput == nullptr) {
        ss << ", pThroughput: nullptr";
    } else {
        ss << ", pThroughput {";
        ss << "timestamp: " << pThroughput->timestamp;
        ss << ", rxCounter: " << pThroughput->rxCounter;
        ss << ", txCounter: " << pThroughput->txCounter;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFabricPortGetThroughput(zes_fabric_port_handle_t hPort,
                                             zes_fabric_port_throughput_t *pThroughput) {
    TRACE_EVENT_BEGIN("API", "zesFabricPortGetThroughput");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetThroughput(hPort, pThroughput) + "..\n";
}
inline void trace_zesFabricPortGetThroughput(ze_result_t ret,
                                             zes_fabric_port_handle_t hPort,
                                             zes_fabric_port_throughput_t *pThroughput) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetThroughput(hPort, pThroughput) + trace_ze_result_t(ret);
}

inline std::string
_trace_zesFabricPortGetFabricErrorCounters(zes_fabric_port_handle_t hPort,
                                           zes_fabric_port_error_counters_t *pErrors) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFabricPortGetFabricErrorCounters(";
    ss << std::hex << std::showbase;
    ss << "hPort: " << hPort;
    if (pErrors == nullptr) {
        ss << ", pErrors: nullptr";
    } else {
        ss << ", pErrors {";
        ss << "stype: " << pErrors->stype;
        ss << ", pNext: " << pErrors->pNext;
        ss << ", linkFailureCount: " << pErrors->linkFailureCount;
        ss << ", fwCommErrorCount: " << pErrors->fwCommErrorCount;
        ss << ", fwErrorCount: " << pErrors->fwErrorCount;
        ss << ", linkDegradeCount: " << pErrors->linkDegradeCount;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFabricPortGetFabricErrorCounters(zes_fabric_port_handle_t hPort,
                                                      zes_fabric_port_error_counters_t *pErrors) {
    TRACE_EVENT_BEGIN("API", "zesFabricPortGetFabricErrorCounters");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetFabricErrorCounters(hPort, pErrors) + "..\n";
}
inline void trace_zesFabricPortGetFabricErrorCounters(ze_result_t ret,
                                                      zes_fabric_port_handle_t hPort,
                                                      zes_fabric_port_error_counters_t *pErrors) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetFabricErrorCounters(hPort, pErrors) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesFabricPortGetMultiPortThroughput(zes_device_handle_t hDevice,
                                           uint32_t numPorts,
                                           zes_fabric_port_handle_t *phPort,
                                           zes_fabric_port_throughput_t **pThroughput) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFabricPortGetMultiPortThroughput(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ", numPorts: " << numPorts;
    if (phPort == nullptr) {
        ss << ", phPort: nullptr";
    } else {
        ss << ", phPort: " << *phPort;
    }
    ss << ", pThroughput: " << pThroughput;
    ss << ")";
    return ss.str();
}
inline void trace_zesFabricPortGetMultiPortThroughput(zes_device_handle_t hDevice,
                                                      uint32_t numPorts,
                                                      zes_fabric_port_handle_t *phPort,
                                                      zes_fabric_port_throughput_t **pThroughput) {
    TRACE_EVENT_BEGIN("API", "zesFabricPortGetMultiPortThroughput");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetMultiPortThroughput(hDevice,
                                                                numPorts,
                                                                phPort,
                                                                pThroughput) +
                         "..\n";
}
inline void trace_zesFabricPortGetMultiPortThroughput(ze_result_t ret,
                                                      zes_device_handle_t hDevice,
                                                      uint32_t numPorts,
                                                      zes_fabric_port_handle_t *phPort,
                                                      zes_fabric_port_throughput_t **pThroughput) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFabricPortGetMultiPortThroughput(hDevice,
                                                                numPorts,
                                                                phPort,
                                                                pThroughput) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesDeviceEnumFans(zes_device_handle_t hDevice, uint32_t *pCount, zes_fan_handle_t *phFan) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumFans(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phFan == nullptr) {
        ss << ", phFan: nullptr";
    } else {
        ss << ", phFan: " << *phFan;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zesDeviceEnumFans(zes_device_handle_t hDevice, uint32_t *pCount, zes_fan_handle_t *phFan) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumFans");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumFans(hDevice, pCount, phFan) + "..\n";
}
inline void trace_zesDeviceEnumFans(ze_result_t ret,
                                    zes_device_handle_t hDevice,
                                    uint32_t *pCount,
                                    zes_fan_handle_t *phFan) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumFans(hDevice, pCount, phFan) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFanGetProperties(zes_fan_handle_t hFan,
                                              zes_fan_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFanGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hFan: " << hFan;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", canControl: " << pProperties->canControl;
        ss << ", supportedModes: " << pProperties->supportedModes;
        ss << ", supportedUnits: " << pProperties->supportedUnits;
        ss << ", maxRPM: " << pProperties->maxRPM;
        ss << ", maxPoints: " << pProperties->maxPoints;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFanGetProperties(zes_fan_handle_t hFan, zes_fan_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesFanGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFanGetProperties(hFan, pProperties) + "..\n";
}
inline void trace_zesFanGetProperties(ze_result_t ret,
                                      zes_fan_handle_t hFan,
                                      zes_fan_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFanGetProperties(hFan, pProperties) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFanGetConfig(zes_fan_handle_t hFan, zes_fan_config_t *pConfig) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFanGetConfig(";
    ss << std::hex << std::showbase;
    ss << "hFan: " << hFan;
    if (pConfig == nullptr) {
        ss << ", pConfig: nullptr";
    } else {
        ss << ", pConfig {";
        ss << "stype: " << pConfig->stype;
        ss << ", pNext: " << pConfig->pNext;
        ss << ", mode: " << pConfig->mode;
        ss << ", speedFixed {";
        ss << "speed: " << pConfig->speedFixed.speed;
        ss << ", units: " << pConfig->speedFixed.units;
        ss << "}";
        ss << ", speedTable {";
        ss << "numPoints: " << pConfig->speedTable.numPoints;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFanGetConfig(zes_fan_handle_t hFan, zes_fan_config_t *pConfig) {
    TRACE_EVENT_BEGIN("API", "zesFanGetConfig");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFanGetConfig(hFan, pConfig) + "..\n";
}
inline void
trace_zesFanGetConfig(ze_result_t ret, zes_fan_handle_t hFan, zes_fan_config_t *pConfig) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFanGetConfig(hFan, pConfig) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFanSetDefaultMode(zes_fan_handle_t hFan) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFanSetDefaultMode(";
    ss << std::hex << std::showbase;
    ss << "hFan: " << hFan;
    ss << ")";
    return ss.str();
}
inline void trace_zesFanSetDefaultMode(zes_fan_handle_t hFan) {
    TRACE_EVENT_BEGIN("API", "zesFanSetDefaultMode");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFanSetDefaultMode(hFan) + "..\n";
}
inline void trace_zesFanSetDefaultMode(ze_result_t ret, zes_fan_handle_t hFan) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFanSetDefaultMode(hFan) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFanSetFixedSpeedMode(zes_fan_handle_t hFan,
                                                  const zes_fan_speed_t *speed) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFanSetFixedSpeedMode(";
    ss << std::hex << std::showbase;
    ss << "hFan: " << hFan;
    if (speed == nullptr) {
        ss << ", speed: nullptr";
    } else {
        ss << ", speed {";
        ss << "speed: " << speed->speed;
        ss << ", units: " << speed->units;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFanSetFixedSpeedMode(zes_fan_handle_t hFan, const zes_fan_speed_t *speed) {
    TRACE_EVENT_BEGIN("API", "zesFanSetFixedSpeedMode");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFanSetFixedSpeedMode(hFan, speed) + "..\n";
}
inline void trace_zesFanSetFixedSpeedMode(ze_result_t ret,
                                          zes_fan_handle_t hFan,
                                          const zes_fan_speed_t *speed) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFanSetFixedSpeedMode(hFan, speed) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFanSetSpeedTableMode(zes_fan_handle_t hFan,
                                                  const zes_fan_speed_table_t *speedTable) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFanSetSpeedTableMode(";
    ss << std::hex << std::showbase;
    ss << "hFan: " << hFan;
    if (speedTable == nullptr) {
        ss << ", speedTable: nullptr";
    } else {
        ss << ", speedTable {";
        ss << "numPoints: " << speedTable->numPoints;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFanSetSpeedTableMode(zes_fan_handle_t hFan,
                                          const zes_fan_speed_table_t *speedTable) {
    TRACE_EVENT_BEGIN("API", "zesFanSetSpeedTableMode");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFanSetSpeedTableMode(hFan, speedTable) + "..\n";
}
inline void trace_zesFanSetSpeedTableMode(ze_result_t ret,
                                          zes_fan_handle_t hFan,
                                          const zes_fan_speed_table_t *speedTable) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFanSetSpeedTableMode(hFan, speedTable) + trace_ze_result_t(ret);
}

inline std::string
_trace_zesFanGetState(zes_fan_handle_t hFan, zes_fan_speed_units_t units, int32_t *pSpeed) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFanGetState(";
    ss << std::hex << std::showbase;
    ss << "hFan: " << hFan;
    ss << ", units: " << units;
    if (pSpeed == nullptr) {
        ss << ", pSpeed: nullptr";
    } else {
        ss << ", pSpeed: " << *pSpeed;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zesFanGetState(zes_fan_handle_t hFan, zes_fan_speed_units_t units, int32_t *pSpeed) {
    TRACE_EVENT_BEGIN("API", "zesFanGetState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFanGetState(hFan, units, pSpeed) + "..\n";
}
inline void trace_zesFanGetState(ze_result_t ret,
                                 zes_fan_handle_t hFan,
                                 zes_fan_speed_units_t units,
                                 int32_t *pSpeed) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFanGetState(hFan, units, pSpeed) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumFirmwares(zes_device_handle_t hDevice,
                                                 uint32_t *pCount,
                                                 zes_firmware_handle_t *phFirmware) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumFirmwares(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phFirmware == nullptr) {
        ss << ", phFirmware: nullptr";
    } else {
        ss << ", phFirmware: " << *phFirmware;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumFirmwares(zes_device_handle_t hDevice,
                                         uint32_t *pCount,
                                         zes_firmware_handle_t *phFirmware) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumFirmwares");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumFirmwares(hDevice, pCount, phFirmware) + "..\n";
}
inline void trace_zesDeviceEnumFirmwares(ze_result_t ret,
                                         zes_device_handle_t hDevice,
                                         uint32_t *pCount,
                                         zes_firmware_handle_t *phFirmware) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumFirmwares(hDevice, pCount, phFirmware) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFirmwareGetProperties(zes_firmware_handle_t hFirmware,
                                                   zes_firmware_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFirmwareGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hFirmware: " << hFirmware;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", canControl: " << pProperties->canControl;
        ss << ", name: " << trace_ascii_array(pProperties->name, sizeof(pProperties->name)).str();
        ss << ", version: "
           << trace_ascii_array(pProperties->version, sizeof(pProperties->version)).str();
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFirmwareGetProperties(zes_firmware_handle_t hFirmware,
                                           zes_firmware_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesFirmwareGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFirmwareGetProperties(hFirmware, pProperties) + "..\n";
}
inline void trace_zesFirmwareGetProperties(ze_result_t ret,
                                           zes_firmware_handle_t hFirmware,
                                           zes_firmware_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFirmwareGetProperties(hFirmware, pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesFirmwareFlash(zes_firmware_handle_t hFirmware, void *pImage, uint32_t size) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFirmwareFlash(";
    ss << std::hex << std::showbase;
    ss << "hFirmware: " << hFirmware;
    ss << ", pImage: " << pImage;
    ss << ", size: " << size;
    ss << ")";
    return ss.str();
}
inline void trace_zesFirmwareFlash(zes_firmware_handle_t hFirmware, void *pImage, uint32_t size) {
    TRACE_EVENT_BEGIN("API", "zesFirmwareFlash");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFirmwareFlash(hFirmware, pImage, size) + "..\n";
}
inline void trace_zesFirmwareFlash(ze_result_t ret,
                                   zes_firmware_handle_t hFirmware,
                                   void *pImage,
                                   uint32_t size) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFirmwareFlash(hFirmware, pImage, size) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFirmwareGetFlashProgress(zes_firmware_handle_t hFirmware,
                                                      uint32_t *pCompletionPercent) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFirmwareGetFlashProgress(";
    ss << std::hex << std::showbase;
    ss << "hFirmware: " << hFirmware;
    if (pCompletionPercent == nullptr) {
        ss << ", pCompletionPercent: nullptr";
    } else {
        ss << ", pCompletionPercent: " << *pCompletionPercent;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFirmwareGetFlashProgress(zes_firmware_handle_t hFirmware,
                                              uint32_t *pCompletionPercent) {
    TRACE_EVENT_BEGIN("API", "zesFirmwareGetFlashProgress");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFirmwareGetFlashProgress(hFirmware, pCompletionPercent) + "..\n";
}
inline void trace_zesFirmwareGetFlashProgress(ze_result_t ret,
                                              zes_firmware_handle_t hFirmware,
                                              uint32_t *pCompletionPercent) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFirmwareGetFlashProgress(hFirmware, pCompletionPercent) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFirmwareGetConsoleLogs(zes_firmware_handle_t hFirmware,
                                                    size_t *pSize,
                                                    char *pFirmwareLog) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFirmwareGetConsoleLogs(";
    ss << std::hex << std::showbase;
    ss << "hFirmware: " << hFirmware;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    if (pFirmwareLog == nullptr) {
        ss << ", pFirmwareLog: nullptr";
    } else {
        ss << ", pFirmwareLog: " << pFirmwareLog;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFirmwareGetConsoleLogs(zes_firmware_handle_t hFirmware,
                                            size_t *pSize,
                                            char *pFirmwareLog) {
    TRACE_EVENT_BEGIN("API", "zesFirmwareGetConsoleLogs");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFirmwareGetConsoleLogs(hFirmware, pSize, pFirmwareLog) + "..\n";
}
inline void trace_zesFirmwareGetConsoleLogs(ze_result_t ret,
                                            zes_firmware_handle_t hFirmware,
                                            size_t *pSize,
                                            char *pFirmwareLog) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFirmwareGetConsoleLogs(hFirmware, pSize, pFirmwareLog) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumFrequencyDomains(zes_device_handle_t hDevice,
                                                        uint32_t *pCount,
                                                        zes_freq_handle_t *phFrequency) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumFrequencyDomains(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phFrequency == nullptr) {
        ss << ", phFrequency: nullptr";
    } else {
        ss << ", phFrequency: " << *phFrequency;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumFrequencyDomains(zes_device_handle_t hDevice,
                                                uint32_t *pCount,
                                                zes_freq_handle_t *phFrequency) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumFrequencyDomains");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumFrequencyDomains(hDevice, pCount, phFrequency) + "..\n";
}
inline void trace_zesDeviceEnumFrequencyDomains(ze_result_t ret,
                                                zes_device_handle_t hDevice,
                                                uint32_t *pCount,
                                                zes_freq_handle_t *phFrequency) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumFrequencyDomains(hDevice, pCount, phFrequency) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyGetProperties(zes_freq_handle_t hFrequency,
                                                    zes_freq_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", type: " << pProperties->type;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", canControl: " << pProperties->canControl;
        ss << ", isThrottleEventSupported: " << pProperties->isThrottleEventSupported;
        ss << ", min: " << pProperties->min;
        ss << ", max: " << pProperties->max;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyGetProperties(zes_freq_handle_t hFrequency,
                                            zes_freq_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyGetProperties(hFrequency, pProperties) + "..\n";
}
inline void trace_zesFrequencyGetProperties(ze_result_t ret,
                                            zes_freq_handle_t hFrequency,
                                            zes_freq_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyGetProperties(hFrequency, pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyGetAvailableClocks(zes_freq_handle_t hFrequency,
                                                         uint32_t *pCount,
                                                         double *phFrequency) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyGetAvailableClocks(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phFrequency == nullptr) {
        ss << ", phFrequency: nullptr";
    } else {
        ss << ", phFrequency: " << *phFrequency;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyGetAvailableClocks(zes_freq_handle_t hFrequency,
                                                 uint32_t *pCount,
                                                 double *phFrequency) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyGetAvailableClocks");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyGetAvailableClocks(hFrequency, pCount, phFrequency) +
                         "..\n";
}
inline void trace_zesFrequencyGetAvailableClocks(ze_result_t ret,
                                                 zes_freq_handle_t hFrequency,
                                                 uint32_t *pCount,
                                                 double *phFrequency) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyGetAvailableClocks(hFrequency, pCount, phFrequency) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyGetRange(zes_freq_handle_t hFrequency,
                                               zes_freq_range_t *pLimits) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyGetRange(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    if (pLimits == nullptr) {
        ss << ", pLimits: nullptr";
    } else {
        ss << ", pLimits {";
        ss << "min: " << pLimits->min;
        ss << ", max: " << pLimits->max;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyGetRange(zes_freq_handle_t hFrequency, zes_freq_range_t *pLimits) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyGetRange");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyGetRange(hFrequency, pLimits) + "..\n";
}
inline void trace_zesFrequencyGetRange(ze_result_t ret,
                                       zes_freq_handle_t hFrequency,
                                       zes_freq_range_t *pLimits) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyGetRange(hFrequency, pLimits) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencySetRange(zes_freq_handle_t hFrequency,
                                               const zes_freq_range_t *pLimits) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencySetRange(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    if (pLimits == nullptr) {
        ss << ", pLimits: nullptr";
    } else {
        ss << ", pLimits {";
        ss << "min: " << pLimits->min;
        ss << ", max: " << pLimits->max;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencySetRange(zes_freq_handle_t hFrequency,
                                       const zes_freq_range_t *pLimits) {
    TRACE_EVENT_BEGIN("API", "zesFrequencySetRange");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencySetRange(hFrequency, pLimits) + "..\n";
}
inline void trace_zesFrequencySetRange(ze_result_t ret,
                                       zes_freq_handle_t hFrequency,
                                       const zes_freq_range_t *pLimits) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencySetRange(hFrequency, pLimits) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyGetState(zes_freq_handle_t hFrequency,
                                               zes_freq_state_t *pState) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyGetState(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    if (pState == nullptr) {
        ss << ", pState: nullptr";
    } else {
        ss << ", pState {";
        ss << "stype: " << pState->stype;
        ss << ", pNext: " << pState->pNext;
        ss << ", currentVoltage: " << pState->currentVoltage;
        ss << ", request: " << pState->request;
        ss << ", tdp: " << pState->tdp;
        ss << ", efficient: " << pState->efficient;
        ss << ", actual: " << pState->actual;
        ss << ", throttleReasons: " << pState->throttleReasons;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyGetState(zes_freq_handle_t hFrequency, zes_freq_state_t *pState) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyGetState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyGetState(hFrequency, pState) + "..\n";
}
inline void trace_zesFrequencyGetState(ze_result_t ret,
                                       zes_freq_handle_t hFrequency,
                                       zes_freq_state_t *pState) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyGetState(hFrequency, pState) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyGetThrottleTime(zes_freq_handle_t hFrequency,
                                                      zes_freq_throttle_time_t *pThrottleTime) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyGetThrottleTime(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    if (pThrottleTime == nullptr) {
        ss << ", pThrottleTime: nullptr";
    } else {
        ss << ", pThrottleTime {";
        ss << "throttleTime: " << pThrottleTime->throttleTime;
        ss << ", timestamp: " << pThrottleTime->timestamp;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyGetThrottleTime(zes_freq_handle_t hFrequency,
                                              zes_freq_throttle_time_t *pThrottleTime) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyGetThrottleTime");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyGetThrottleTime(hFrequency, pThrottleTime) + "..\n";
}
inline void trace_zesFrequencyGetThrottleTime(ze_result_t ret,
                                              zes_freq_handle_t hFrequency,
                                              zes_freq_throttle_time_t *pThrottleTime) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyGetThrottleTime(hFrequency, pThrottleTime) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyOcGetCapabilities(zes_freq_handle_t hFrequency,
                                                        zes_oc_capabilities_t *pOcCapabilities) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyOcGetCapabilities(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    if (pOcCapabilities == nullptr) {
        ss << ", pOcCapabilities: nullptr";
    } else {
        ss << ", pOcCapabilities {";
        ss << "stype: " << pOcCapabilities->stype;
        ss << ", pNext: " << pOcCapabilities->pNext;
        ss << ", isOcSupported: " << pOcCapabilities->isOcSupported;
        ss << ", maxFactoryDefaultFrequency: " << pOcCapabilities->maxFactoryDefaultFrequency;
        ss << ", maxFactoryDefaultVoltage: " << pOcCapabilities->maxFactoryDefaultVoltage;
        ss << ", maxOcFrequency: " << pOcCapabilities->maxOcFrequency;
        ss << ", minOcVoltageOffset: " << pOcCapabilities->minOcVoltageOffset;
        ss << ", maxOcVoltageOffset: " << pOcCapabilities->maxOcVoltageOffset;
        ss << ", maxOcVoltage: " << pOcCapabilities->maxOcVoltage;
        ss << ", isTjMaxSupported: " << pOcCapabilities->isTjMaxSupported;
        ss << ", isIccMaxSupported: " << pOcCapabilities->isIccMaxSupported;
        ss << ", isHighVoltModeCapable: " << pOcCapabilities->isHighVoltModeCapable;
        ss << ", isHighVoltModeEnabled: " << pOcCapabilities->isHighVoltModeEnabled;
        ss << ", isExtendedModeSupported: " << pOcCapabilities->isExtendedModeSupported;
        ss << ", isFixedModeSupported: " << pOcCapabilities->isFixedModeSupported;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyOcGetCapabilities(zes_freq_handle_t hFrequency,
                                                zes_oc_capabilities_t *pOcCapabilities) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyOcGetCapabilities");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcGetCapabilities(hFrequency, pOcCapabilities) + "..\n";
}
inline void trace_zesFrequencyOcGetCapabilities(ze_result_t ret,
                                                zes_freq_handle_t hFrequency,
                                                zes_oc_capabilities_t *pOcCapabilities) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcGetCapabilities(hFrequency, pOcCapabilities) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyOcGetFrequencyTarget(zes_freq_handle_t hFrequency,
                                                           double *pCurrentOcFrequency) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyOcGetFrequencyTarget(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    if (pCurrentOcFrequency == nullptr) {
        ss << ", pCurrentOcFrequency: nullptr";
    } else {
        ss << ", pCurrentOcFrequency: " << *pCurrentOcFrequency;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyOcGetFrequencyTarget(zes_freq_handle_t hFrequency,
                                                   double *pCurrentOcFrequency) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyOcGetFrequencyTarget");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcGetFrequencyTarget(hFrequency, pCurrentOcFrequency) +
                         "..\n";
}
inline void trace_zesFrequencyOcGetFrequencyTarget(ze_result_t ret,
                                                   zes_freq_handle_t hFrequency,
                                                   double *pCurrentOcFrequency) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcGetFrequencyTarget(hFrequency, pCurrentOcFrequency) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyOcSetFrequencyTarget(zes_freq_handle_t hFrequency,
                                                           double CurrentOcFrequency) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyOcSetFrequencyTarget(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    ss << ", CurrentOcFrequency: " << CurrentOcFrequency;
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyOcSetFrequencyTarget(zes_freq_handle_t hFrequency,
                                                   double CurrentOcFrequency) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyOcSetFrequencyTarget");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcSetFrequencyTarget(hFrequency, CurrentOcFrequency) +
                         "..\n";
}
inline void trace_zesFrequencyOcSetFrequencyTarget(ze_result_t ret,
                                                   zes_freq_handle_t hFrequency,
                                                   double CurrentOcFrequency) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcSetFrequencyTarget(hFrequency, CurrentOcFrequency) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyOcGetVoltageTarget(zes_freq_handle_t hFrequency,
                                                         double *pCurrentVoltageTarget,
                                                         double *pCurrentVoltageOffset) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyOcGetVoltageTarget(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    if (pCurrentVoltageTarget == nullptr) {
        ss << ", pCurrentVoltageTarget: nullptr";
    } else {
        ss << ", pCurrentVoltageTarget: " << *pCurrentVoltageTarget;
    }
    if (pCurrentVoltageOffset == nullptr) {
        ss << ", pCurrentVoltageOffset: nullptr";
    } else {
        ss << ", pCurrentVoltageOffset: " << *pCurrentVoltageOffset;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyOcGetVoltageTarget(zes_freq_handle_t hFrequency,
                                                 double *pCurrentVoltageTarget,
                                                 double *pCurrentVoltageOffset) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyOcGetVoltageTarget");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcGetVoltageTarget(hFrequency,
                                                           pCurrentVoltageTarget,
                                                           pCurrentVoltageOffset) +
                         "..\n";
}
inline void trace_zesFrequencyOcGetVoltageTarget(ze_result_t ret,
                                                 zes_freq_handle_t hFrequency,
                                                 double *pCurrentVoltageTarget,
                                                 double *pCurrentVoltageOffset) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcGetVoltageTarget(hFrequency,
                                                           pCurrentVoltageTarget,
                                                           pCurrentVoltageOffset) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyOcSetVoltageTarget(zes_freq_handle_t hFrequency,
                                                         double CurrentVoltageTarget,
                                                         double CurrentVoltageOffset) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyOcSetVoltageTarget(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    ss << ", CurrentVoltageTarget: " << CurrentVoltageTarget;
    ss << ", CurrentVoltageOffset: " << CurrentVoltageOffset;
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyOcSetVoltageTarget(zes_freq_handle_t hFrequency,
                                                 double CurrentVoltageTarget,
                                                 double CurrentVoltageOffset) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyOcSetVoltageTarget");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcSetVoltageTarget(hFrequency,
                                                           CurrentVoltageTarget,
                                                           CurrentVoltageOffset) +
                         "..\n";
}
inline void trace_zesFrequencyOcSetVoltageTarget(ze_result_t ret,
                                                 zes_freq_handle_t hFrequency,
                                                 double CurrentVoltageTarget,
                                                 double CurrentVoltageOffset) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcSetVoltageTarget(hFrequency,
                                                           CurrentVoltageTarget,
                                                           CurrentVoltageOffset) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyOcSetMode(zes_freq_handle_t hFrequency,
                                                zes_oc_mode_t CurrentOcMode) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyOcSetMode(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    ss << ", CurrentOcMode: " << CurrentOcMode;
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyOcSetMode(zes_freq_handle_t hFrequency, zes_oc_mode_t CurrentOcMode) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyOcSetMode");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcSetMode(hFrequency, CurrentOcMode) + "..\n";
}
inline void trace_zesFrequencyOcSetMode(ze_result_t ret,
                                        zes_freq_handle_t hFrequency,
                                        zes_oc_mode_t CurrentOcMode) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcSetMode(hFrequency, CurrentOcMode) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyOcGetMode(zes_freq_handle_t hFrequency,
                                                zes_oc_mode_t *pCurrentOcMode) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyOcGetMode(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    if (pCurrentOcMode == nullptr) {
        ss << ", pCurrentOcMode: nullptr";
    } else {
        ss << ", pCurrentOcMode: " << *pCurrentOcMode;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyOcGetMode(zes_freq_handle_t hFrequency,
                                        zes_oc_mode_t *pCurrentOcMode) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyOcGetMode");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcGetMode(hFrequency, pCurrentOcMode) + "..\n";
}
inline void trace_zesFrequencyOcGetMode(ze_result_t ret,
                                        zes_freq_handle_t hFrequency,
                                        zes_oc_mode_t *pCurrentOcMode) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcGetMode(hFrequency, pCurrentOcMode) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyOcGetIccMax(zes_freq_handle_t hFrequency, double *pOcIccMax) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyOcGetIccMax(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    if (pOcIccMax == nullptr) {
        ss << ", pOcIccMax: nullptr";
    } else {
        ss << ", pOcIccMax: " << *pOcIccMax;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyOcGetIccMax(zes_freq_handle_t hFrequency, double *pOcIccMax) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyOcGetIccMax");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcGetIccMax(hFrequency, pOcIccMax) + "..\n";
}
inline void
trace_zesFrequencyOcGetIccMax(ze_result_t ret, zes_freq_handle_t hFrequency, double *pOcIccMax) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcGetIccMax(hFrequency, pOcIccMax) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyOcSetIccMax(zes_freq_handle_t hFrequency, double ocIccMax) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyOcSetIccMax(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    ss << ", ocIccMax: " << ocIccMax;
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyOcSetIccMax(zes_freq_handle_t hFrequency, double ocIccMax) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyOcSetIccMax");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcSetIccMax(hFrequency, ocIccMax) + "..\n";
}
inline void
trace_zesFrequencyOcSetIccMax(ze_result_t ret, zes_freq_handle_t hFrequency, double ocIccMax) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcSetIccMax(hFrequency, ocIccMax) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyOcGetTjMax(zes_freq_handle_t hFrequency, double *pOcTjMax) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyOcGetTjMax(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    if (pOcTjMax == nullptr) {
        ss << ", pOcTjMax: nullptr";
    } else {
        ss << ", pOcTjMax: " << *pOcTjMax;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyOcGetTjMax(zes_freq_handle_t hFrequency, double *pOcTjMax) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyOcGetTjMax");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcGetTjMax(hFrequency, pOcTjMax) + "..\n";
}
inline void
trace_zesFrequencyOcGetTjMax(ze_result_t ret, zes_freq_handle_t hFrequency, double *pOcTjMax) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcGetTjMax(hFrequency, pOcTjMax) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFrequencyOcSetTjMax(zes_freq_handle_t hFrequency, double ocTjMax) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFrequencyOcSetTjMax(";
    ss << std::hex << std::showbase;
    ss << "hFrequency: " << hFrequency;
    ss << ", ocTjMax: " << ocTjMax;
    ss << ")";
    return ss.str();
}
inline void trace_zesFrequencyOcSetTjMax(zes_freq_handle_t hFrequency, double ocTjMax) {
    TRACE_EVENT_BEGIN("API", "zesFrequencyOcSetTjMax");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcSetTjMax(hFrequency, ocTjMax) + "..\n";
}
inline void
trace_zesFrequencyOcSetTjMax(ze_result_t ret, zes_freq_handle_t hFrequency, double ocTjMax) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFrequencyOcSetTjMax(hFrequency, ocTjMax) + trace_ze_result_t(ret);
}

inline std::string
_trace_zesDeviceEnumLeds(zes_device_handle_t hDevice, uint32_t *pCount, zes_led_handle_t *phLed) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumLeds(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phLed == nullptr) {
        ss << ", phLed: nullptr";
    } else {
        ss << ", phLed: " << *phLed;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zesDeviceEnumLeds(zes_device_handle_t hDevice, uint32_t *pCount, zes_led_handle_t *phLed) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumLeds");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumLeds(hDevice, pCount, phLed) + "..\n";
}
inline void trace_zesDeviceEnumLeds(ze_result_t ret,
                                    zes_device_handle_t hDevice,
                                    uint32_t *pCount,
                                    zes_led_handle_t *phLed) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumLeds(hDevice, pCount, phLed) + trace_ze_result_t(ret);
}

inline std::string _trace_zesLedGetProperties(zes_led_handle_t hLed,
                                              zes_led_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesLedGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hLed: " << hLed;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", canControl: " << pProperties->canControl;
        ss << ", haveRGB: " << pProperties->haveRGB;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesLedGetProperties(zes_led_handle_t hLed, zes_led_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesLedGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesLedGetProperties(hLed, pProperties) + "..\n";
}
inline void trace_zesLedGetProperties(ze_result_t ret,
                                      zes_led_handle_t hLed,
                                      zes_led_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesLedGetProperties(hLed, pProperties) + trace_ze_result_t(ret);
}

inline std::string _trace_zesLedGetState(zes_led_handle_t hLed, zes_led_state_t *pState) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesLedGetState(";
    ss << std::hex << std::showbase;
    ss << "hLed: " << hLed;
    if (pState == nullptr) {
        ss << ", pState: nullptr";
    } else {
        ss << ", pState {";
        ss << "stype: " << pState->stype;
        ss << ", pNext: " << pState->pNext;
        ss << ", isOn: " << pState->isOn;
        ss << ", color {";
        ss << "red: " << pState->color.red;
        ss << ", green: " << pState->color.green;
        ss << ", blue: " << pState->color.blue;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesLedGetState(zes_led_handle_t hLed, zes_led_state_t *pState) {
    TRACE_EVENT_BEGIN("API", "zesLedGetState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesLedGetState(hLed, pState) + "..\n";
}
inline void trace_zesLedGetState(ze_result_t ret, zes_led_handle_t hLed, zes_led_state_t *pState) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesLedGetState(hLed, pState) + trace_ze_result_t(ret);
}

inline std::string _trace_zesLedSetState(zes_led_handle_t hLed, ze_bool_t enable) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesLedSetState(";
    ss << std::hex << std::showbase;
    ss << "hLed: " << hLed;
    ss << ", enable: " << enable;
    ss << ")";
    return ss.str();
}
inline void trace_zesLedSetState(zes_led_handle_t hLed, ze_bool_t enable) {
    TRACE_EVENT_BEGIN("API", "zesLedSetState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesLedSetState(hLed, enable) + "..\n";
}
inline void trace_zesLedSetState(ze_result_t ret, zes_led_handle_t hLed, ze_bool_t enable) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesLedSetState(hLed, enable) + trace_ze_result_t(ret);
}

inline std::string _trace_zesLedSetColor(zes_led_handle_t hLed, const zes_led_color_t *pColor) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesLedSetColor(";
    ss << std::hex << std::showbase;
    ss << "hLed: " << hLed;
    if (pColor == nullptr) {
        ss << ", pColor: nullptr";
    } else {
        ss << ", pColor {";
        ss << "red: " << pColor->red;
        ss << ", green: " << pColor->green;
        ss << ", blue: " << pColor->blue;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesLedSetColor(zes_led_handle_t hLed, const zes_led_color_t *pColor) {
    TRACE_EVENT_BEGIN("API", "zesLedSetColor");
    if (IS_API_TRACE())
        std::cerr << _trace_zesLedSetColor(hLed, pColor) + "..\n";
}
inline void
trace_zesLedSetColor(ze_result_t ret, zes_led_handle_t hLed, const zes_led_color_t *pColor) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesLedSetColor(hLed, pColor) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumMemoryModules(zes_device_handle_t hDevice,
                                                     uint32_t *pCount,
                                                     zes_mem_handle_t *phMemory) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumMemoryModules(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phMemory == nullptr) {
        ss << ", phMemory: nullptr";
    } else {
        ss << ", phMemory: " << *phMemory;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumMemoryModules(zes_device_handle_t hDevice,
                                             uint32_t *pCount,
                                             zes_mem_handle_t *phMemory) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumMemoryModules");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumMemoryModules(hDevice, pCount, phMemory) + "..\n";
}
inline void trace_zesDeviceEnumMemoryModules(ze_result_t ret,
                                             zes_device_handle_t hDevice,
                                             uint32_t *pCount,
                                             zes_mem_handle_t *phMemory) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumMemoryModules(hDevice, pCount, phMemory) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesMemoryGetProperties(zes_mem_handle_t hMemory,
                                                 zes_mem_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesMemoryGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hMemory: " << hMemory;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", type: " << pProperties->type;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", location: " << pProperties->location;
        ss << ", physicalSize: " << pProperties->physicalSize;
        ss << ", busWidth: " << pProperties->busWidth;
        ss << ", numChannels: " << pProperties->numChannels;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesMemoryGetProperties(zes_mem_handle_t hMemory,
                                         zes_mem_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesMemoryGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesMemoryGetProperties(hMemory, pProperties) + "..\n";
}
inline void trace_zesMemoryGetProperties(ze_result_t ret,
                                         zes_mem_handle_t hMemory,
                                         zes_mem_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesMemoryGetProperties(hMemory, pProperties) + trace_ze_result_t(ret);
}

inline std::string _trace_zesMemoryGetState(zes_mem_handle_t hMemory, zes_mem_state_t *pState) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesMemoryGetState(";
    ss << std::hex << std::showbase;
    ss << "hMemory: " << hMemory;
    if (pState == nullptr) {
        ss << ", pState: nullptr";
    } else {
        ss << ", pState {";
        ss << "stype: " << pState->stype;
        ss << ", pNext: " << pState->pNext;
        ss << ", health: " << pState->health;
        ss << ", free: " << pState->free;
        ss << ", size: " << pState->size;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesMemoryGetState(zes_mem_handle_t hMemory, zes_mem_state_t *pState) {
    TRACE_EVENT_BEGIN("API", "zesMemoryGetState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesMemoryGetState(hMemory, pState) + "..\n";
}
inline void
trace_zesMemoryGetState(ze_result_t ret, zes_mem_handle_t hMemory, zes_mem_state_t *pState) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesMemoryGetState(hMemory, pState) + trace_ze_result_t(ret);
}

inline std::string _trace_zesMemoryGetBandwidth(zes_mem_handle_t hMemory,
                                                zes_mem_bandwidth_t *pBandwidth) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesMemoryGetBandwidth(";
    ss << std::hex << std::showbase;
    ss << "hMemory: " << hMemory;
    if (pBandwidth == nullptr) {
        ss << ", pBandwidth: nullptr";
    } else {
        ss << ", pBandwidth {";
        ss << "readCounter: " << pBandwidth->readCounter;
        ss << ", writeCounter: " << pBandwidth->writeCounter;
        ss << ", maxBandwidth: " << pBandwidth->maxBandwidth;
        ss << ", timestamp: " << pBandwidth->timestamp;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesMemoryGetBandwidth(zes_mem_handle_t hMemory, zes_mem_bandwidth_t *pBandwidth) {
    TRACE_EVENT_BEGIN("API", "zesMemoryGetBandwidth");
    if (IS_API_TRACE())
        std::cerr << _trace_zesMemoryGetBandwidth(hMemory, pBandwidth) + "..\n";
}
inline void trace_zesMemoryGetBandwidth(ze_result_t ret,
                                        zes_mem_handle_t hMemory,
                                        zes_mem_bandwidth_t *pBandwidth) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesMemoryGetBandwidth(hMemory, pBandwidth) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumPerformanceFactorDomains(zes_device_handle_t hDevice,
                                                                uint32_t *pCount,
                                                                zes_perf_handle_t *phPerf) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumPerformanceFactorDomains(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phPerf == nullptr) {
        ss << ", phPerf: nullptr";
    } else {
        ss << ", phPerf: " << *phPerf;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumPerformanceFactorDomains(zes_device_handle_t hDevice,
                                                        uint32_t *pCount,
                                                        zes_perf_handle_t *phPerf) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumPerformanceFactorDomains");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumPerformanceFactorDomains(hDevice, pCount, phPerf) + "..\n";
}
inline void trace_zesDeviceEnumPerformanceFactorDomains(ze_result_t ret,
                                                        zes_device_handle_t hDevice,
                                                        uint32_t *pCount,
                                                        zes_perf_handle_t *phPerf) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumPerformanceFactorDomains(hDevice, pCount, phPerf) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesPerformanceFactorGetProperties(zes_perf_handle_t hPerf,
                                                            zes_perf_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPerformanceFactorGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hPerf: " << hPerf;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", engines: " << pProperties->engines;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesPerformanceFactorGetProperties(zes_perf_handle_t hPerf,
                                                    zes_perf_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesPerformanceFactorGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPerformanceFactorGetProperties(hPerf, pProperties) + "..\n";
}
inline void trace_zesPerformanceFactorGetProperties(ze_result_t ret,
                                                    zes_perf_handle_t hPerf,
                                                    zes_perf_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPerformanceFactorGetProperties(hPerf, pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesPerformanceFactorGetConfig(zes_perf_handle_t hPerf, double *pFactor) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPerformanceFactorGetConfig(";
    ss << std::hex << std::showbase;
    ss << "hPerf: " << hPerf;
    if (pFactor == nullptr) {
        ss << ", pFactor: nullptr";
    } else {
        ss << ", pFactor: " << *pFactor;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesPerformanceFactorGetConfig(zes_perf_handle_t hPerf, double *pFactor) {
    TRACE_EVENT_BEGIN("API", "zesPerformanceFactorGetConfig");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPerformanceFactorGetConfig(hPerf, pFactor) + "..\n";
}
inline void
trace_zesPerformanceFactorGetConfig(ze_result_t ret, zes_perf_handle_t hPerf, double *pFactor) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPerformanceFactorGetConfig(hPerf, pFactor) + trace_ze_result_t(ret);
}

inline std::string _trace_zesPerformanceFactorSetConfig(zes_perf_handle_t hPerf, double factor) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPerformanceFactorSetConfig(";
    ss << std::hex << std::showbase;
    ss << "hPerf: " << hPerf;
    ss << ", factor: " << factor;
    ss << ")";
    return ss.str();
}
inline void trace_zesPerformanceFactorSetConfig(zes_perf_handle_t hPerf, double factor) {
    TRACE_EVENT_BEGIN("API", "zesPerformanceFactorSetConfig");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPerformanceFactorSetConfig(hPerf, factor) + "..\n";
}
inline void
trace_zesPerformanceFactorSetConfig(ze_result_t ret, zes_perf_handle_t hPerf, double factor) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPerformanceFactorSetConfig(hPerf, factor) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumPowerDomains(zes_device_handle_t hDevice,
                                                    uint32_t *pCount,
                                                    zes_pwr_handle_t *phPower) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumPowerDomains(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phPower == nullptr) {
        ss << ", phPower: nullptr";
    } else {
        ss << ", phPower: " << *phPower;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumPowerDomains(zes_device_handle_t hDevice,
                                            uint32_t *pCount,
                                            zes_pwr_handle_t *phPower) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumPowerDomains");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumPowerDomains(hDevice, pCount, phPower) + "..\n";
}
inline void trace_zesDeviceEnumPowerDomains(ze_result_t ret,
                                            zes_device_handle_t hDevice,
                                            uint32_t *pCount,
                                            zes_pwr_handle_t *phPower) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumPowerDomains(hDevice, pCount, phPower) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceGetCardPowerDomain(zes_device_handle_t hDevice,
                                                      zes_pwr_handle_t *phPower) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceGetCardPowerDomain(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (phPower == nullptr) {
        ss << ", phPower: nullptr";
    } else {
        ss << ", phPower: " << *phPower;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceGetCardPowerDomain(zes_device_handle_t hDevice,
                                              zes_pwr_handle_t *phPower) {
    TRACE_EVENT_BEGIN("API", "zesDeviceGetCardPowerDomain");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetCardPowerDomain(hDevice, phPower) + "..\n";
}
inline void trace_zesDeviceGetCardPowerDomain(ze_result_t ret,
                                              zes_device_handle_t hDevice,
                                              zes_pwr_handle_t *phPower) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetCardPowerDomain(hDevice, phPower) + trace_ze_result_t(ret);
}

inline std::string _trace_zesPowerGetProperties(zes_pwr_handle_t hPower,
                                                zes_power_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPowerGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hPower: " << hPower;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", canControl: " << pProperties->canControl;
        ss << ", isEnergyThresholdSupported: " << pProperties->isEnergyThresholdSupported;
        ss << ", defaultLimit: " << pProperties->defaultLimit;
        ss << ", minLimit: " << pProperties->minLimit;
        ss << ", maxLimit: " << pProperties->maxLimit;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesPowerGetProperties(zes_pwr_handle_t hPower,
                                        zes_power_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesPowerGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerGetProperties(hPower, pProperties) + "..\n";
}
inline void trace_zesPowerGetProperties(ze_result_t ret,
                                        zes_pwr_handle_t hPower,
                                        zes_power_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerGetProperties(hPower, pProperties) + trace_ze_result_t(ret);
}

inline std::string _trace_zesPowerGetEnergyCounter(zes_pwr_handle_t hPower,
                                                   zes_power_energy_counter_t *pEnergy) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPowerGetEnergyCounter(";
    ss << std::hex << std::showbase;
    ss << "hPower: " << hPower;
    if (pEnergy == nullptr) {
        ss << ", pEnergy: nullptr";
    } else {
        ss << ", pEnergy {";
        ss << "energy: " << pEnergy->energy;
        ss << ", timestamp: " << pEnergy->timestamp;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesPowerGetEnergyCounter(zes_pwr_handle_t hPower,
                                           zes_power_energy_counter_t *pEnergy) {
    TRACE_EVENT_BEGIN("API", "zesPowerGetEnergyCounter");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerGetEnergyCounter(hPower, pEnergy) + "..\n";
}
inline void trace_zesPowerGetEnergyCounter(ze_result_t ret,
                                           zes_pwr_handle_t hPower,
                                           zes_power_energy_counter_t *pEnergy) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerGetEnergyCounter(hPower, pEnergy) + trace_ze_result_t(ret);
}

inline std::string _trace_zesPowerGetLimits(zes_pwr_handle_t hPower,
                                            zes_power_sustained_limit_t *pSustained,
                                            zes_power_burst_limit_t *pBurst,
                                            zes_power_peak_limit_t *pPeak) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPowerGetLimits(";
    ss << std::hex << std::showbase;
    ss << "hPower: " << hPower;
    if (pSustained == nullptr) {
        ss << ", pSustained: nullptr";
    } else {
        ss << ", pSustained {";
        ss << "enabled: " << pSustained->enabled;
        ss << ", power: " << pSustained->power;
        ss << ", interval: " << pSustained->interval;
        ss << "}";
    }
    if (pBurst == nullptr) {
        ss << ", pBurst: nullptr";
    } else {
        ss << ", pBurst {";
        ss << "enabled: " << pBurst->enabled;
        ss << ", power: " << pBurst->power;
        ss << "}";
    }
    if (pPeak == nullptr) {
        ss << ", pPeak: nullptr";
    } else {
        ss << ", pPeak {";
        ss << "powerAC: " << pPeak->powerAC;
        ss << ", powerDC: " << pPeak->powerDC;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesPowerGetLimits(zes_pwr_handle_t hPower,
                                    zes_power_sustained_limit_t *pSustained,
                                    zes_power_burst_limit_t *pBurst,
                                    zes_power_peak_limit_t *pPeak) {
    TRACE_EVENT_BEGIN("API", "zesPowerGetLimits");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerGetLimits(hPower, pSustained, pBurst, pPeak) + "..\n";
}
inline void trace_zesPowerGetLimits(ze_result_t ret,
                                    zes_pwr_handle_t hPower,
                                    zes_power_sustained_limit_t *pSustained,
                                    zes_power_burst_limit_t *pBurst,
                                    zes_power_peak_limit_t *pPeak) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerGetLimits(hPower, pSustained, pBurst, pPeak) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesPowerSetLimits(zes_pwr_handle_t hPower,
                                            const zes_power_sustained_limit_t *pSustained,
                                            const zes_power_burst_limit_t *pBurst,
                                            const zes_power_peak_limit_t *pPeak) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPowerSetLimits(";
    ss << std::hex << std::showbase;
    ss << "hPower: " << hPower;
    if (pSustained == nullptr) {
        ss << ", pSustained: nullptr";
    } else {
        ss << ", pSustained {";
        ss << "enabled: " << pSustained->enabled;
        ss << ", power: " << pSustained->power;
        ss << ", interval: " << pSustained->interval;
        ss << "}";
    }
    if (pBurst == nullptr) {
        ss << ", pBurst: nullptr";
    } else {
        ss << ", pBurst {";
        ss << "enabled: " << pBurst->enabled;
        ss << ", power: " << pBurst->power;
        ss << "}";
    }
    if (pPeak == nullptr) {
        ss << ", pPeak: nullptr";
    } else {
        ss << ", pPeak {";
        ss << "powerAC: " << pPeak->powerAC;
        ss << ", powerDC: " << pPeak->powerDC;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesPowerSetLimits(zes_pwr_handle_t hPower,
                                    const zes_power_sustained_limit_t *pSustained,
                                    const zes_power_burst_limit_t *pBurst,
                                    const zes_power_peak_limit_t *pPeak) {
    TRACE_EVENT_BEGIN("API", "zesPowerSetLimits");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerSetLimits(hPower, pSustained, pBurst, pPeak) + "..\n";
}
inline void trace_zesPowerSetLimits(ze_result_t ret,
                                    zes_pwr_handle_t hPower,
                                    const zes_power_sustained_limit_t *pSustained,
                                    const zes_power_burst_limit_t *pBurst,
                                    const zes_power_peak_limit_t *pPeak) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerSetLimits(hPower, pSustained, pBurst, pPeak) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesPowerGetEnergyThreshold(zes_pwr_handle_t hPower,
                                                     zes_energy_threshold_t *pThreshold) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPowerGetEnergyThreshold(";
    ss << std::hex << std::showbase;
    ss << "hPower: " << hPower;
    if (pThreshold == nullptr) {
        ss << ", pThreshold: nullptr";
    } else {
        ss << ", pThreshold {";
        ss << "enable: " << pThreshold->enable;
        ss << ", threshold: " << pThreshold->threshold;
        ss << ", processId: " << pThreshold->processId;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesPowerGetEnergyThreshold(zes_pwr_handle_t hPower,
                                             zes_energy_threshold_t *pThreshold) {
    TRACE_EVENT_BEGIN("API", "zesPowerGetEnergyThreshold");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerGetEnergyThreshold(hPower, pThreshold) + "..\n";
}
inline void trace_zesPowerGetEnergyThreshold(ze_result_t ret,
                                             zes_pwr_handle_t hPower,
                                             zes_energy_threshold_t *pThreshold) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerGetEnergyThreshold(hPower, pThreshold) + trace_ze_result_t(ret);
}

inline std::string _trace_zesPowerSetEnergyThreshold(zes_pwr_handle_t hPower, double threshold) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPowerSetEnergyThreshold(";
    ss << std::hex << std::showbase;
    ss << "hPower: " << hPower;
    ss << ", threshold: " << threshold;
    ss << ")";
    return ss.str();
}
inline void trace_zesPowerSetEnergyThreshold(zes_pwr_handle_t hPower, double threshold) {
    TRACE_EVENT_BEGIN("API", "zesPowerSetEnergyThreshold");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerSetEnergyThreshold(hPower, threshold) + "..\n";
}
inline void
trace_zesPowerSetEnergyThreshold(ze_result_t ret, zes_pwr_handle_t hPower, double threshold) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerSetEnergyThreshold(hPower, threshold) + trace_ze_result_t(ret);
}

inline std::string
_trace_zesDeviceEnumPsus(zes_device_handle_t hDevice, uint32_t *pCount, zes_psu_handle_t *phPsu) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumPsus(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phPsu == nullptr) {
        ss << ", phPsu: nullptr";
    } else {
        ss << ", phPsu: " << *phPsu;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zesDeviceEnumPsus(zes_device_handle_t hDevice, uint32_t *pCount, zes_psu_handle_t *phPsu) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumPsus");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumPsus(hDevice, pCount, phPsu) + "..\n";
}
inline void trace_zesDeviceEnumPsus(ze_result_t ret,
                                    zes_device_handle_t hDevice,
                                    uint32_t *pCount,
                                    zes_psu_handle_t *phPsu) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumPsus(hDevice, pCount, phPsu) + trace_ze_result_t(ret);
}

inline std::string _trace_zesPsuGetProperties(zes_psu_handle_t hPsu,
                                              zes_psu_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPsuGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hPsu: " << hPsu;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", haveFan: " << pProperties->haveFan;
        ss << ", ampLimit: " << pProperties->ampLimit;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesPsuGetProperties(zes_psu_handle_t hPsu, zes_psu_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesPsuGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPsuGetProperties(hPsu, pProperties) + "..\n";
}
inline void trace_zesPsuGetProperties(ze_result_t ret,
                                      zes_psu_handle_t hPsu,
                                      zes_psu_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPsuGetProperties(hPsu, pProperties) + trace_ze_result_t(ret);
}

inline std::string _trace_zesPsuGetState(zes_psu_handle_t hPsu, zes_psu_state_t *pState) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPsuGetState(";
    ss << std::hex << std::showbase;
    ss << "hPsu: " << hPsu;
    if (pState == nullptr) {
        ss << ", pState: nullptr";
    } else {
        ss << ", pState {";
        ss << "stype: " << pState->stype;
        ss << ", pNext: " << pState->pNext;
        ss << ", voltStatus: " << pState->voltStatus;
        ss << ", fanFailed: " << pState->fanFailed;
        ss << ", temperature: " << pState->temperature;
        ss << ", current: " << pState->current;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesPsuGetState(zes_psu_handle_t hPsu, zes_psu_state_t *pState) {
    TRACE_EVENT_BEGIN("API", "zesPsuGetState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPsuGetState(hPsu, pState) + "..\n";
}
inline void trace_zesPsuGetState(ze_result_t ret, zes_psu_handle_t hPsu, zes_psu_state_t *pState) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPsuGetState(hPsu, pState) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumRasErrorSets(zes_device_handle_t hDevice,
                                                    uint32_t *pCount,
                                                    zes_ras_handle_t *phRas) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumRasErrorSets(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phRas == nullptr) {
        ss << ", phRas: nullptr";
    } else {
        ss << ", phRas: " << *phRas;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumRasErrorSets(zes_device_handle_t hDevice,
                                            uint32_t *pCount,
                                            zes_ras_handle_t *phRas) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumRasErrorSets");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumRasErrorSets(hDevice, pCount, phRas) + "..\n";
}
inline void trace_zesDeviceEnumRasErrorSets(ze_result_t ret,
                                            zes_device_handle_t hDevice,
                                            uint32_t *pCount,
                                            zes_ras_handle_t *phRas) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumRasErrorSets(hDevice, pCount, phRas) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesRasGetProperties(zes_ras_handle_t hRas,
                                              zes_ras_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesRasGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hRas: " << hRas;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", type: " << pProperties->type;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesRasGetProperties(zes_ras_handle_t hRas, zes_ras_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesRasGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesRasGetProperties(hRas, pProperties) + "..\n";
}
inline void trace_zesRasGetProperties(ze_result_t ret,
                                      zes_ras_handle_t hRas,
                                      zes_ras_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesRasGetProperties(hRas, pProperties) + trace_ze_result_t(ret);
}

inline std::string _trace_zesRasGetConfig(zes_ras_handle_t hRas, zes_ras_config_t *pConfig) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesRasGetConfig(";
    ss << std::hex << std::showbase;
    ss << "hRas: " << hRas;
    if (pConfig == nullptr) {
        ss << ", pConfig: nullptr";
    } else {
        ss << ", pConfig {";
        ss << "stype: " << pConfig->stype;
        ss << ", pNext: " << pConfig->pNext;
        ss << ", totalThreshold: " << pConfig->totalThreshold;
        ss << ", detailedThresholds {";
        ss << "stype: " << pConfig->detailedThresholds.stype;
        ss << ", pNext: " << pConfig->detailedThresholds.pNext;
        ss << ", category: " << pConfig->detailedThresholds.category[0]
           << pConfig->detailedThresholds.category[1] << pConfig->detailedThresholds.category[2]
           << pConfig->detailedThresholds.category[3] << pConfig->detailedThresholds.category[4]
           << pConfig->detailedThresholds.category[5] << pConfig->detailedThresholds.category[6];
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesRasGetConfig(zes_ras_handle_t hRas, zes_ras_config_t *pConfig) {
    TRACE_EVENT_BEGIN("API", "zesRasGetConfig");
    if (IS_API_TRACE())
        std::cerr << _trace_zesRasGetConfig(hRas, pConfig) + "..\n";
}
inline void
trace_zesRasGetConfig(ze_result_t ret, zes_ras_handle_t hRas, zes_ras_config_t *pConfig) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesRasGetConfig(hRas, pConfig) + trace_ze_result_t(ret);
}

inline std::string _trace_zesRasSetConfig(zes_ras_handle_t hRas, const zes_ras_config_t *pConfig) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesRasSetConfig(";
    ss << std::hex << std::showbase;
    ss << "hRas: " << hRas;
    if (pConfig == nullptr) {
        ss << ", pConfig: nullptr";
    } else {
        ss << ", pConfig {";
        ss << "stype: " << pConfig->stype;
        ss << ", pNext: " << pConfig->pNext;
        ss << ", totalThreshold: " << pConfig->totalThreshold;
        ss << ", detailedThresholds {";
        ss << "stype: " << pConfig->detailedThresholds.stype;
        ss << ", pNext: " << pConfig->detailedThresholds.pNext;
        ss << ", category: " << pConfig->detailedThresholds.category[0]
           << pConfig->detailedThresholds.category[1] << pConfig->detailedThresholds.category[2]
           << pConfig->detailedThresholds.category[3] << pConfig->detailedThresholds.category[4]
           << pConfig->detailedThresholds.category[5] << pConfig->detailedThresholds.category[6];
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesRasSetConfig(zes_ras_handle_t hRas, const zes_ras_config_t *pConfig) {
    TRACE_EVENT_BEGIN("API", "zesRasSetConfig");
    if (IS_API_TRACE())
        std::cerr << _trace_zesRasSetConfig(hRas, pConfig) + "..\n";
}
inline void
trace_zesRasSetConfig(ze_result_t ret, zes_ras_handle_t hRas, const zes_ras_config_t *pConfig) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesRasSetConfig(hRas, pConfig) + trace_ze_result_t(ret);
}

inline std::string
_trace_zesRasGetState(zes_ras_handle_t hRas, ze_bool_t clear, zes_ras_state_t *pState) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesRasGetState(";
    ss << std::hex << std::showbase;
    ss << "hRas: " << hRas;
    ss << ", clear: " << clear;
    if (pState == nullptr) {
        ss << ", pState: nullptr";
    } else {
        ss << ", pState {";
        ss << "stype: " << pState->stype;
        ss << ", pNext: " << pState->pNext;
        ss << ", category: " << pState->category[0] << pState->category[1] << pState->category[2]
           << pState->category[3] << pState->category[4] << pState->category[5]
           << pState->category[6];
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesRasGetState(zes_ras_handle_t hRas, ze_bool_t clear, zes_ras_state_t *pState) {
    TRACE_EVENT_BEGIN("API", "zesRasGetState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesRasGetState(hRas, clear, pState) + "..\n";
}
inline void trace_zesRasGetState(ze_result_t ret,
                                 zes_ras_handle_t hRas,
                                 ze_bool_t clear,
                                 zes_ras_state_t *pState) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesRasGetState(hRas, clear, pState) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumSchedulers(zes_device_handle_t hDevice,
                                                  uint32_t *pCount,
                                                  zes_sched_handle_t *phScheduler) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumSchedulers(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phScheduler == nullptr) {
        ss << ", phScheduler: nullptr";
    } else {
        ss << ", phScheduler: " << *phScheduler;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumSchedulers(zes_device_handle_t hDevice,
                                          uint32_t *pCount,
                                          zes_sched_handle_t *phScheduler) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumSchedulers");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumSchedulers(hDevice, pCount, phScheduler) + "..\n";
}
inline void trace_zesDeviceEnumSchedulers(ze_result_t ret,
                                          zes_device_handle_t hDevice,
                                          uint32_t *pCount,
                                          zes_sched_handle_t *phScheduler) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumSchedulers(hDevice, pCount, phScheduler) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesSchedulerGetProperties(zes_sched_handle_t hScheduler,
                                                    zes_sched_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesSchedulerGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hScheduler: " << hScheduler;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", canControl: " << pProperties->canControl;
        ss << ", engines: " << pProperties->engines;
        ss << ", supportedModes: " << pProperties->supportedModes;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesSchedulerGetProperties(zes_sched_handle_t hScheduler,
                                            zes_sched_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesSchedulerGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerGetProperties(hScheduler, pProperties) + "..\n";
}
inline void trace_zesSchedulerGetProperties(ze_result_t ret,
                                            zes_sched_handle_t hScheduler,
                                            zes_sched_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerGetProperties(hScheduler, pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesSchedulerGetCurrentMode(zes_sched_handle_t hScheduler,
                                                     zes_sched_mode_t *pMode) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesSchedulerGetCurrentMode(";
    ss << std::hex << std::showbase;
    ss << "hScheduler: " << hScheduler;
    if (pMode == nullptr) {
        ss << ", pMode: nullptr";
    } else {
        ss << ", pMode: " << *pMode;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesSchedulerGetCurrentMode(zes_sched_handle_t hScheduler,
                                             zes_sched_mode_t *pMode) {
    TRACE_EVENT_BEGIN("API", "zesSchedulerGetCurrentMode");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerGetCurrentMode(hScheduler, pMode) + "..\n";
}
inline void trace_zesSchedulerGetCurrentMode(ze_result_t ret,
                                             zes_sched_handle_t hScheduler,
                                             zes_sched_mode_t *pMode) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerGetCurrentMode(hScheduler, pMode) + trace_ze_result_t(ret);
}

inline std::string
_trace_zesSchedulerGetTimeoutModeProperties(zes_sched_handle_t hScheduler,
                                            ze_bool_t getDefaults,
                                            zes_sched_timeout_properties_t *pConfig) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesSchedulerGetTimeoutModeProperties(";
    ss << std::hex << std::showbase;
    ss << "hScheduler: " << hScheduler;
    ss << ", getDefaults: " << getDefaults;
    if (pConfig == nullptr) {
        ss << ", pConfig: nullptr";
    } else {
        ss << ", pConfig {";
        ss << "stype: " << pConfig->stype;
        ss << ", pNext: " << pConfig->pNext;
        ss << ", watchdogTimeout: " << pConfig->watchdogTimeout;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesSchedulerGetTimeoutModeProperties(zes_sched_handle_t hScheduler,
                                                       ze_bool_t getDefaults,
                                                       zes_sched_timeout_properties_t *pConfig) {
    TRACE_EVENT_BEGIN("API", "zesSchedulerGetTimeoutModeProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerGetTimeoutModeProperties(hScheduler, getDefaults, pConfig) +
                         "..\n";
}
inline void trace_zesSchedulerGetTimeoutModeProperties(ze_result_t ret,
                                                       zes_sched_handle_t hScheduler,
                                                       ze_bool_t getDefaults,
                                                       zes_sched_timeout_properties_t *pConfig) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerGetTimeoutModeProperties(hScheduler, getDefaults, pConfig) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesSchedulerGetTimesliceModeProperties(zes_sched_handle_t hScheduler,
                                              ze_bool_t getDefaults,
                                              zes_sched_timeslice_properties_t *pConfig) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesSchedulerGetTimesliceModeProperties(";
    ss << std::hex << std::showbase;
    ss << "hScheduler: " << hScheduler;
    ss << ", getDefaults: " << getDefaults;
    if (pConfig == nullptr) {
        ss << ", pConfig: nullptr";
    } else {
        ss << ", pConfig {";
        ss << "stype: " << pConfig->stype;
        ss << ", pNext: " << pConfig->pNext;
        ss << ", interval: " << pConfig->interval;
        ss << ", yieldTimeout: " << pConfig->yieldTimeout;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zesSchedulerGetTimesliceModeProperties(zes_sched_handle_t hScheduler,
                                             ze_bool_t getDefaults,
                                             zes_sched_timeslice_properties_t *pConfig) {
    TRACE_EVENT_BEGIN("API", "zesSchedulerGetTimesliceModeProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerGetTimesliceModeProperties(hScheduler,
                                                                   getDefaults,
                                                                   pConfig) +
                         "..\n";
}
inline void
trace_zesSchedulerGetTimesliceModeProperties(ze_result_t ret,
                                             zes_sched_handle_t hScheduler,
                                             ze_bool_t getDefaults,
                                             zes_sched_timeslice_properties_t *pConfig) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerGetTimesliceModeProperties(hScheduler,
                                                                   getDefaults,
                                                                   pConfig) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesSchedulerSetTimeoutMode(zes_sched_handle_t hScheduler,
                                                     zes_sched_timeout_properties_t *pProperties,
                                                     ze_bool_t *pNeedReload) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesSchedulerSetTimeoutMode(";
    ss << std::hex << std::showbase;
    ss << "hScheduler: " << hScheduler;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", watchdogTimeout: " << pProperties->watchdogTimeout;
        ss << "}";
    }
    if (pNeedReload == nullptr) {
        ss << ", pNeedReload: nullptr";
    } else {
        ss << ", pNeedReload: " << *pNeedReload;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesSchedulerSetTimeoutMode(zes_sched_handle_t hScheduler,
                                             zes_sched_timeout_properties_t *pProperties,
                                             ze_bool_t *pNeedReload) {
    TRACE_EVENT_BEGIN("API", "zesSchedulerSetTimeoutMode");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerSetTimeoutMode(hScheduler, pProperties, pNeedReload) +
                         "..\n";
}
inline void trace_zesSchedulerSetTimeoutMode(ze_result_t ret,
                                             zes_sched_handle_t hScheduler,
                                             zes_sched_timeout_properties_t *pProperties,
                                             ze_bool_t *pNeedReload) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerSetTimeoutMode(hScheduler, pProperties, pNeedReload) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesSchedulerSetTimesliceMode(zes_sched_handle_t hScheduler,
                                    zes_sched_timeslice_properties_t *pProperties,
                                    ze_bool_t *pNeedReload) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesSchedulerSetTimesliceMode(";
    ss << std::hex << std::showbase;
    ss << "hScheduler: " << hScheduler;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", interval: " << pProperties->interval;
        ss << ", yieldTimeout: " << pProperties->yieldTimeout;
        ss << "}";
    }
    if (pNeedReload == nullptr) {
        ss << ", pNeedReload: nullptr";
    } else {
        ss << ", pNeedReload: " << *pNeedReload;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesSchedulerSetTimesliceMode(zes_sched_handle_t hScheduler,
                                               zes_sched_timeslice_properties_t *pProperties,
                                               ze_bool_t *pNeedReload) {
    TRACE_EVENT_BEGIN("API", "zesSchedulerSetTimesliceMode");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerSetTimesliceMode(hScheduler, pProperties, pNeedReload) +
                         "..\n";
}
inline void trace_zesSchedulerSetTimesliceMode(ze_result_t ret,
                                               zes_sched_handle_t hScheduler,
                                               zes_sched_timeslice_properties_t *pProperties,
                                               ze_bool_t *pNeedReload) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerSetTimesliceMode(hScheduler, pProperties, pNeedReload) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesSchedulerSetExclusiveMode(zes_sched_handle_t hScheduler,
                                                       ze_bool_t *pNeedReload) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesSchedulerSetExclusiveMode(";
    ss << std::hex << std::showbase;
    ss << "hScheduler: " << hScheduler;
    if (pNeedReload == nullptr) {
        ss << ", pNeedReload: nullptr";
    } else {
        ss << ", pNeedReload: " << *pNeedReload;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesSchedulerSetExclusiveMode(zes_sched_handle_t hScheduler,
                                               ze_bool_t *pNeedReload) {
    TRACE_EVENT_BEGIN("API", "zesSchedulerSetExclusiveMode");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerSetExclusiveMode(hScheduler, pNeedReload) + "..\n";
}
inline void trace_zesSchedulerSetExclusiveMode(ze_result_t ret,
                                               zes_sched_handle_t hScheduler,
                                               ze_bool_t *pNeedReload) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerSetExclusiveMode(hScheduler, pNeedReload) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesSchedulerSetComputeUnitDebugMode(zes_sched_handle_t hScheduler,
                                                              ze_bool_t *pNeedReload) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesSchedulerSetComputeUnitDebugMode(";
    ss << std::hex << std::showbase;
    ss << "hScheduler: " << hScheduler;
    if (pNeedReload == nullptr) {
        ss << ", pNeedReload: nullptr";
    } else {
        ss << ", pNeedReload: " << *pNeedReload;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesSchedulerSetComputeUnitDebugMode(zes_sched_handle_t hScheduler,
                                                      ze_bool_t *pNeedReload) {
    TRACE_EVENT_BEGIN("API", "zesSchedulerSetComputeUnitDebugMode");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerSetComputeUnitDebugMode(hScheduler, pNeedReload) + "..\n";
}
inline void trace_zesSchedulerSetComputeUnitDebugMode(ze_result_t ret,
                                                      zes_sched_handle_t hScheduler,
                                                      ze_bool_t *pNeedReload) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesSchedulerSetComputeUnitDebugMode(hScheduler, pNeedReload) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumStandbyDomains(zes_device_handle_t hDevice,
                                                      uint32_t *pCount,
                                                      zes_standby_handle_t *phStandby) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumStandbyDomains(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phStandby == nullptr) {
        ss << ", phStandby: nullptr";
    } else {
        ss << ", phStandby: " << *phStandby;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumStandbyDomains(zes_device_handle_t hDevice,
                                              uint32_t *pCount,
                                              zes_standby_handle_t *phStandby) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumStandbyDomains");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumStandbyDomains(hDevice, pCount, phStandby) + "..\n";
}
inline void trace_zesDeviceEnumStandbyDomains(ze_result_t ret,
                                              zes_device_handle_t hDevice,
                                              uint32_t *pCount,
                                              zes_standby_handle_t *phStandby) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumStandbyDomains(hDevice, pCount, phStandby) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesStandbyGetProperties(zes_standby_handle_t hStandby,
                                                  zes_standby_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesStandbyGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hStandby: " << hStandby;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", type: " << pProperties->type;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesStandbyGetProperties(zes_standby_handle_t hStandby,
                                          zes_standby_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesStandbyGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesStandbyGetProperties(hStandby, pProperties) + "..\n";
}
inline void trace_zesStandbyGetProperties(ze_result_t ret,
                                          zes_standby_handle_t hStandby,
                                          zes_standby_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesStandbyGetProperties(hStandby, pProperties) + trace_ze_result_t(ret);
}

inline std::string _trace_zesStandbyGetMode(zes_standby_handle_t hStandby,
                                            zes_standby_promo_mode_t *pMode) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesStandbyGetMode(";
    ss << std::hex << std::showbase;
    ss << "hStandby: " << hStandby;
    if (pMode == nullptr) {
        ss << ", pMode: nullptr";
    } else {
        ss << ", pMode: " << *pMode;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesStandbyGetMode(zes_standby_handle_t hStandby,
                                    zes_standby_promo_mode_t *pMode) {
    TRACE_EVENT_BEGIN("API", "zesStandbyGetMode");
    if (IS_API_TRACE())
        std::cerr << _trace_zesStandbyGetMode(hStandby, pMode) + "..\n";
}
inline void trace_zesStandbyGetMode(ze_result_t ret,
                                    zes_standby_handle_t hStandby,
                                    zes_standby_promo_mode_t *pMode) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesStandbyGetMode(hStandby, pMode) + trace_ze_result_t(ret);
}

inline std::string _trace_zesStandbySetMode(zes_standby_handle_t hStandby,
                                            zes_standby_promo_mode_t mode) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesStandbySetMode(";
    ss << std::hex << std::showbase;
    ss << "hStandby: " << hStandby;
    ss << ", mode: " << mode;
    ss << ")";
    return ss.str();
}
inline void trace_zesStandbySetMode(zes_standby_handle_t hStandby, zes_standby_promo_mode_t mode) {
    TRACE_EVENT_BEGIN("API", "zesStandbySetMode");
    if (IS_API_TRACE())
        std::cerr << _trace_zesStandbySetMode(hStandby, mode) + "..\n";
}
inline void trace_zesStandbySetMode(ze_result_t ret,
                                    zes_standby_handle_t hStandby,
                                    zes_standby_promo_mode_t mode) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesStandbySetMode(hStandby, mode) + trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumTemperatureSensors(zes_device_handle_t hDevice,
                                                          uint32_t *pCount,
                                                          zes_temp_handle_t *phTemperature) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumTemperatureSensors(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phTemperature == nullptr) {
        ss << ", phTemperature: nullptr";
    } else {
        ss << ", phTemperature: " << *phTemperature;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumTemperatureSensors(zes_device_handle_t hDevice,
                                                  uint32_t *pCount,
                                                  zes_temp_handle_t *phTemperature) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumTemperatureSensors");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumTemperatureSensors(hDevice, pCount, phTemperature) +
                         "..\n";
}
inline void trace_zesDeviceEnumTemperatureSensors(ze_result_t ret,
                                                  zes_device_handle_t hDevice,
                                                  uint32_t *pCount,
                                                  zes_temp_handle_t *phTemperature) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumTemperatureSensors(hDevice, pCount, phTemperature) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesTemperatureGetProperties(zes_temp_handle_t hTemperature,
                                                      zes_temp_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesTemperatureGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hTemperature: " << hTemperature;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", type: " << pProperties->type;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", maxTemperature: " << pProperties->maxTemperature;
        ss << ", isCriticalTempSupported: " << pProperties->isCriticalTempSupported;
        ss << ", isThreshold1Supported: " << pProperties->isThreshold1Supported;
        ss << ", isThreshold2Supported: " << pProperties->isThreshold2Supported;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesTemperatureGetProperties(zes_temp_handle_t hTemperature,
                                              zes_temp_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesTemperatureGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zesTemperatureGetProperties(hTemperature, pProperties) + "..\n";
}
inline void trace_zesTemperatureGetProperties(ze_result_t ret,
                                              zes_temp_handle_t hTemperature,
                                              zes_temp_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesTemperatureGetProperties(hTemperature, pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesTemperatureGetConfig(zes_temp_handle_t hTemperature,
                                                  zes_temp_config_t *pConfig) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesTemperatureGetConfig(";
    ss << std::hex << std::showbase;
    ss << "hTemperature: " << hTemperature;
    if (pConfig == nullptr) {
        ss << ", pConfig: nullptr";
    } else {
        ss << ", pConfig {";
        ss << "stype: " << pConfig->stype;
        ss << ", pNext: " << pConfig->pNext;
        ss << ", enableCritical: " << pConfig->enableCritical;
        ss << ", threshold1 {";
        ss << "enableLowToHigh: " << pConfig->threshold1.enableLowToHigh;
        ss << ", enableHighToLow: " << pConfig->threshold1.enableHighToLow;
        ss << ", threshold: " << pConfig->threshold1.threshold;
        ss << "}";
        ss << ", threshold2 {";
        ss << "enableLowToHigh: " << pConfig->threshold2.enableLowToHigh;
        ss << ", enableHighToLow: " << pConfig->threshold2.enableHighToLow;
        ss << ", threshold: " << pConfig->threshold2.threshold;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesTemperatureGetConfig(zes_temp_handle_t hTemperature,
                                          zes_temp_config_t *pConfig) {
    TRACE_EVENT_BEGIN("API", "zesTemperatureGetConfig");
    if (IS_API_TRACE())
        std::cerr << _trace_zesTemperatureGetConfig(hTemperature, pConfig) + "..\n";
}
inline void trace_zesTemperatureGetConfig(ze_result_t ret,
                                          zes_temp_handle_t hTemperature,
                                          zes_temp_config_t *pConfig) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesTemperatureGetConfig(hTemperature, pConfig) + trace_ze_result_t(ret);
}

inline std::string _trace_zesTemperatureSetConfig(zes_temp_handle_t hTemperature,
                                                  const zes_temp_config_t *pConfig) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesTemperatureSetConfig(";
    ss << std::hex << std::showbase;
    ss << "hTemperature: " << hTemperature;
    if (pConfig == nullptr) {
        ss << ", pConfig: nullptr";
    } else {
        ss << ", pConfig {";
        ss << "stype: " << pConfig->stype;
        ss << ", pNext: " << pConfig->pNext;
        ss << ", enableCritical: " << pConfig->enableCritical;
        ss << ", threshold1 {";
        ss << "enableLowToHigh: " << pConfig->threshold1.enableLowToHigh;
        ss << ", enableHighToLow: " << pConfig->threshold1.enableHighToLow;
        ss << ", threshold: " << pConfig->threshold1.threshold;
        ss << "}";
        ss << ", threshold2 {";
        ss << "enableLowToHigh: " << pConfig->threshold2.enableLowToHigh;
        ss << ", enableHighToLow: " << pConfig->threshold2.enableHighToLow;
        ss << ", threshold: " << pConfig->threshold2.threshold;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesTemperatureSetConfig(zes_temp_handle_t hTemperature,
                                          const zes_temp_config_t *pConfig) {
    TRACE_EVENT_BEGIN("API", "zesTemperatureSetConfig");
    if (IS_API_TRACE())
        std::cerr << _trace_zesTemperatureSetConfig(hTemperature, pConfig) + "..\n";
}
inline void trace_zesTemperatureSetConfig(ze_result_t ret,
                                          zes_temp_handle_t hTemperature,
                                          const zes_temp_config_t *pConfig) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesTemperatureSetConfig(hTemperature, pConfig) + trace_ze_result_t(ret);
}

inline std::string _trace_zesTemperatureGetState(zes_temp_handle_t hTemperature,
                                                 double *pTemperature) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesTemperatureGetState(";
    ss << std::hex << std::showbase;
    ss << "hTemperature: " << hTemperature;
    if (pTemperature == nullptr) {
        ss << ", pTemperature: nullptr";
    } else {
        ss << ", pTemperature: " << *pTemperature;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesTemperatureGetState(zes_temp_handle_t hTemperature, double *pTemperature) {
    TRACE_EVENT_BEGIN("API", "zesTemperatureGetState");
    if (IS_API_TRACE())
        std::cerr << _trace_zesTemperatureGetState(hTemperature, pTemperature) + "..\n";
}
inline void trace_zesTemperatureGetState(ze_result_t ret,
                                         zes_temp_handle_t hTemperature,
                                         double *pTemperature) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesTemperatureGetState(hTemperature, pTemperature) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesPowerGetLimitsExt(zes_pwr_handle_t hPower,
                                               uint32_t *pCount,
                                               zes_power_limit_ext_desc_t *pSustained) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPowerGetLimitsExt(";
    ss << std::hex << std::showbase;
    ss << "hPower: " << hPower;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pSustained == nullptr) {
        ss << ", pSustained: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pSustained[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pSustained[0].stype;
            ss << ", pNext: " << pSustained[0].pNext;
            ss << ", level: " << pSustained[0].level;
            ss << ", source: " << pSustained[0].source;
            ss << ", limitUnit: " << pSustained[0].limitUnit;
            ss << ", enabledStateLocked: " << pSustained[0].enabledStateLocked;
            ss << ", enabled: " << pSustained[0].enabled;
            ss << ", intervalValueLocked: " << pSustained[0].intervalValueLocked;
            ss << ", interval: " << pSustained[0].interval;
            ss << ", limitValueLocked: " << pSustained[0].limitValueLocked;
            ss << ", limit: " << pSustained[0].limit;
            ss << "}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesPowerGetLimitsExt(zes_pwr_handle_t hPower,
                                       uint32_t *pCount,
                                       zes_power_limit_ext_desc_t *pSustained) {
    TRACE_EVENT_BEGIN("API", "zesPowerGetLimitsExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerGetLimitsExt(hPower, pCount, pSustained) + "..\n";
}
inline void trace_zesPowerGetLimitsExt(ze_result_t ret,
                                       zes_pwr_handle_t hPower,
                                       uint32_t *pCount,
                                       zes_power_limit_ext_desc_t *pSustained) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerGetLimitsExt(hPower, pCount, pSustained) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesPowerSetLimitsExt(zes_pwr_handle_t hPower,
                                               uint32_t *pCount,
                                               zes_power_limit_ext_desc_t *pSustained) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesPowerSetLimitsExt(";
    ss << std::hex << std::showbase;
    ss << "hPower: " << hPower;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pSustained == nullptr) {
        ss << ", pSustained: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pSustained[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pSustained[0].stype;
            ss << ", pNext: " << pSustained[0].pNext;
            ss << ", level: " << pSustained[0].level;
            ss << ", source: " << pSustained[0].source;
            ss << ", limitUnit: " << pSustained[0].limitUnit;
            ss << ", enabledStateLocked: " << pSustained[0].enabledStateLocked;
            ss << ", enabled: " << pSustained[0].enabled;
            ss << ", intervalValueLocked: " << pSustained[0].intervalValueLocked;
            ss << ", interval: " << pSustained[0].interval;
            ss << ", limitValueLocked: " << pSustained[0].limitValueLocked;
            ss << ", limit: " << pSustained[0].limit;
            ss << "}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesPowerSetLimitsExt(zes_pwr_handle_t hPower,
                                       uint32_t *pCount,
                                       zes_power_limit_ext_desc_t *pSustained) {
    TRACE_EVENT_BEGIN("API", "zesPowerSetLimitsExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerSetLimitsExt(hPower, pCount, pSustained) + "..\n";
}
inline void trace_zesPowerSetLimitsExt(ze_result_t ret,
                                       zes_pwr_handle_t hPower,
                                       uint32_t *pCount,
                                       zes_power_limit_ext_desc_t *pSustained) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesPowerSetLimitsExt(hPower, pCount, pSustained) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesEngineGetActivityExt(zes_engine_handle_t hEngine,
                                                  uint32_t *pCount,
                                                  zes_engine_stats_t *pStats) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesEngineGetActivityExt(";
    ss << std::hex << std::showbase;
    ss << "hEngine: " << hEngine;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pStats == nullptr) {
        ss << ", pStats: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pStats[";
        if (count > 0) {
            ss << "{";
            ss << "activeTime: " << pStats[0].activeTime;
            ss << ", timestamp: " << pStats[0].timestamp;
            ss << "}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesEngineGetActivityExt(zes_engine_handle_t hEngine,
                                          uint32_t *pCount,
                                          zes_engine_stats_t *pStats) {
    TRACE_EVENT_BEGIN("API", "zesEngineGetActivityExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zesEngineGetActivityExt(hEngine, pCount, pStats) + "..\n";
}
inline void trace_zesEngineGetActivityExt(ze_result_t ret,
                                          zes_engine_handle_t hEngine,
                                          uint32_t *pCount,
                                          zes_engine_stats_t *pStats) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesEngineGetActivityExt(hEngine, pCount, pStats) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesRasGetStateExp(zes_ras_handle_t hRas, uint32_t *pCount, zes_ras_state_exp_t *pState) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesRasGetStateExp(";
    ss << std::hex << std::showbase;
    ss << "hRas: " << hRas;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pState == nullptr) {
        ss << ", pState: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pState[";
        if (count > 0) {
            ss << "{";
            ss << "category: " << pState[0].category;
            ss << ", errorCounter: " << pState[0].errorCounter;
            ss << "}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zesRasGetStateExp(zes_ras_handle_t hRas, uint32_t *pCount, zes_ras_state_exp_t *pState) {
    TRACE_EVENT_BEGIN("API", "zesRasGetStateExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesRasGetStateExp(hRas, pCount, pState) + "..\n";
}
inline void trace_zesRasGetStateExp(ze_result_t ret,
                                    zes_ras_handle_t hRas,
                                    uint32_t *pCount,
                                    zes_ras_state_exp_t *pState) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesRasGetStateExp(hRas, pCount, pState) + trace_ze_result_t(ret);
}

inline std::string _trace_zesRasClearStateExp(zes_ras_handle_t hRas,
                                              zes_ras_error_category_exp_t category) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesRasClearStateExp(";
    ss << std::hex << std::showbase;
    ss << "hRas: " << hRas;
    ss << ", category: " << category;
    ss << ")";
    return ss.str();
}
inline void trace_zesRasClearStateExp(zes_ras_handle_t hRas,
                                      zes_ras_error_category_exp_t category) {
    TRACE_EVENT_BEGIN("API", "zesRasClearStateExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesRasClearStateExp(hRas, category) + "..\n";
}
inline void trace_zesRasClearStateExp(ze_result_t ret,
                                      zes_ras_handle_t hRas,
                                      zes_ras_error_category_exp_t category) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesRasClearStateExp(hRas, category) + trace_ze_result_t(ret);
}

inline std::string _trace_zesFirmwareGetSecurityVersionExp(zes_firmware_handle_t hFirmware,
                                                           char *pVersion) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFirmwareGetSecurityVersionExp(";
    ss << std::hex << std::showbase;
    ss << "hFirmware: " << hFirmware;
    if (pVersion == nullptr) {
        ss << ", pVersion: nullptr";
    } else {
        ss << ", pVersion: " << pVersion;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesFirmwareGetSecurityVersionExp(zes_firmware_handle_t hFirmware,
                                                   char *pVersion) {
    TRACE_EVENT_BEGIN("API", "zesFirmwareGetSecurityVersionExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFirmwareGetSecurityVersionExp(hFirmware, pVersion) + "..\n";
}
inline void trace_zesFirmwareGetSecurityVersionExp(ze_result_t ret,
                                                   zes_firmware_handle_t hFirmware,
                                                   char *pVersion) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFirmwareGetSecurityVersionExp(hFirmware, pVersion) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesFirmwareSetSecurityVersionExp(zes_firmware_handle_t hFirmware) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesFirmwareSetSecurityVersionExp(";
    ss << std::hex << std::showbase;
    ss << "hFirmware: " << hFirmware;
    ss << ")";
    return ss.str();
}
inline void trace_zesFirmwareSetSecurityVersionExp(zes_firmware_handle_t hFirmware) {
    TRACE_EVENT_BEGIN("API", "zesFirmwareSetSecurityVersionExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFirmwareSetSecurityVersionExp(hFirmware) + "..\n";
}
inline void trace_zesFirmwareSetSecurityVersionExp(ze_result_t ret,
                                                   zes_firmware_handle_t hFirmware) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesFirmwareSetSecurityVersionExp(hFirmware) + trace_ze_result_t(ret);
}

inline std::string
_trace_zesDeviceGetSubDevicePropertiesExp(zes_device_handle_t hDevice,
                                          uint32_t *pCount,
                                          zes_subdevice_exp_properties_t *pSubdeviceProps) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceGetSubDevicePropertiesExp(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pSubdeviceProps == nullptr) {
        ss << ", pSubdeviceProps: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pSubdeviceProps[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pSubdeviceProps[0].stype;
            ss << ", pNext: " << pSubdeviceProps[0].pNext;
            ss << ", subdeviceId: " << pSubdeviceProps[0].subdeviceId;
            ss << ", uuid {";
            ss << "id: ";
            ss << trace_u8_array_hex(pSubdeviceProps[0].uuid.id, sizeof(pSubdeviceProps[0].uuid.id))
                      .str();
            ss << "}}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zesDeviceGetSubDevicePropertiesExp(zes_device_handle_t hDevice,
                                         uint32_t *pCount,
                                         zes_subdevice_exp_properties_t *pSubdeviceProps) {
    TRACE_EVENT_BEGIN("API", "zesDeviceGetSubDevicePropertiesExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetSubDevicePropertiesExp(hDevice, pCount, pSubdeviceProps) +
                         "..\n";
}
inline void
trace_zesDeviceGetSubDevicePropertiesExp(ze_result_t ret,
                                         zes_device_handle_t hDevice,
                                         uint32_t *pCount,
                                         zes_subdevice_exp_properties_t *pSubdeviceProps) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceGetSubDevicePropertiesExp(hDevice, pCount, pSubdeviceProps) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDriverGetDeviceByUuidExp(zes_driver_handle_t hDriver,
                                                      zes_uuid_t uuid,
                                                      zes_device_handle_t *phDevice,
                                                      ze_bool_t *onSubdevice,
                                                      uint32_t *subdeviceId) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDriverGetDeviceByUuidExp(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    ss << ", uuid {";
    ss << "id: ";
    ss << trace_u8_array_hex(uuid.id, sizeof(uuid.id)).str();
    ss << "}";
    if (phDevice == nullptr) {
        ss << ", phDevice: nullptr";
    } else {
        ss << ", phDevice: " << *phDevice;
    }
    if (onSubdevice == nullptr) {
        ss << ", onSubdevice: nullptr";
    } else {
        ss << ", onSubdevice: " << *onSubdevice;
    }
    if (subdeviceId == nullptr) {
        ss << ", subdeviceId: nullptr";
    } else {
        ss << ", subdeviceId: " << *subdeviceId;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDriverGetDeviceByUuidExp(zes_driver_handle_t hDriver,
                                              zes_uuid_t uuid,
                                              zes_device_handle_t *phDevice,
                                              ze_bool_t *onSubdevice,
                                              uint32_t *subdeviceId) {
    TRACE_EVENT_BEGIN("API", "zesDriverGetDeviceByUuidExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDriverGetDeviceByUuidExp(hDriver,
                                                        uuid,
                                                        phDevice,
                                                        onSubdevice,
                                                        subdeviceId) +
                         "..\n";
}
inline void trace_zesDriverGetDeviceByUuidExp(ze_result_t ret,
                                              zes_driver_handle_t hDriver,
                                              zes_uuid_t uuid,
                                              zes_device_handle_t *phDevice,
                                              ze_bool_t *onSubdevice,
                                              uint32_t *subdeviceId) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDriverGetDeviceByUuidExp(hDriver,
                                                        uuid,
                                                        phDevice,
                                                        onSubdevice,
                                                        subdeviceId) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumActiveVFExp(zes_device_handle_t hDevice,
                                                   uint32_t *pCount,
                                                   zes_vf_handle_t *phVFhandle) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumActiveVFExp(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phVFhandle == nullptr) {
        ss << ", phVFhandle: nullptr";
    } else {
        ss << ", phVFhandle: " << *phVFhandle;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumActiveVFExp(zes_device_handle_t hDevice,
                                           uint32_t *pCount,
                                           zes_vf_handle_t *phVFhandle) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumActiveVFExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumActiveVFExp(hDevice, pCount, phVFhandle) + "..\n";
}
inline void trace_zesDeviceEnumActiveVFExp(ze_result_t ret,
                                           zes_device_handle_t hDevice,
                                           uint32_t *pCount,
                                           zes_vf_handle_t *phVFhandle) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumActiveVFExp(hDevice, pCount, phVFhandle) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesVFManagementGetVFPropertiesExp(zes_vf_handle_t hVFhandle,
                                                            zes_vf_exp_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesVFManagementGetVFPropertiesExp(";
    ss << std::hex << std::showbase;
    ss << "hVFhandle: " << hVFhandle;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", address {";
        ss << "domain: " << pProperties->address.domain;
        ss << ", bus: " << pProperties->address.bus;
        ss << ", device: " << pProperties->address.device;
        ss << ", function: " << pProperties->address.function;
        ss << "}";
        ss << ", uuid {";
        ss << "id: ";
        ss << trace_u8_array_hex(pProperties->uuid.id, sizeof(pProperties->uuid.id)).str();
        ss << "}";
        ss << ", flags: " << pProperties->flags;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesVFManagementGetVFPropertiesExp(zes_vf_handle_t hVFhandle,
                                                    zes_vf_exp_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zesVFManagementGetVFPropertiesExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementGetVFPropertiesExp(hVFhandle, pProperties) + "..\n";
}
inline void trace_zesVFManagementGetVFPropertiesExp(ze_result_t ret,
                                                    zes_vf_handle_t hVFhandle,
                                                    zes_vf_exp_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementGetVFPropertiesExp(hVFhandle, pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesVFManagementGetVFMemoryUtilizationExp(zes_vf_handle_t hVFhandle,
                                                uint32_t *pCount,
                                                zes_vf_util_mem_exp_t *pMemUtil) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesVFManagementGetVFMemoryUtilizationExp(";
    ss << std::hex << std::showbase;
    ss << "hVFhandle: " << hVFhandle;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pMemUtil == nullptr) {
        ss << ", pMemUtil: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pMemUtil[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pMemUtil[0].stype;
            ss << ", pNext: " << pMemUtil[0].pNext;
            ss << ", memTypeFlags: " << pMemUtil[0].memTypeFlags;
            ss << ", free: " << pMemUtil[0].free;
            ss << ", size: " << pMemUtil[0].size;
            ss << ", timestamp: " << pMemUtil[0].timestamp;
            ss << "}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesVFManagementGetVFMemoryUtilizationExp(zes_vf_handle_t hVFhandle,
                                                           uint32_t *pCount,
                                                           zes_vf_util_mem_exp_t *pMemUtil) {
    TRACE_EVENT_BEGIN("API", "zesVFManagementGetVFMemoryUtilizationExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementGetVFMemoryUtilizationExp(hVFhandle, pCount, pMemUtil) +
                         "..\n";
}
inline void trace_zesVFManagementGetVFMemoryUtilizationExp(ze_result_t ret,
                                                           zes_vf_handle_t hVFhandle,
                                                           uint32_t *pCount,
                                                           zes_vf_util_mem_exp_t *pMemUtil) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementGetVFMemoryUtilizationExp(hVFhandle, pCount, pMemUtil) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesVFManagementGetVFEngineUtilizationExp(zes_vf_handle_t hVFhandle,
                                                uint32_t *pCount,
                                                zes_vf_util_engine_exp_t *pEngineUtil) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesVFManagementGetVFEngineUtilizationExp(";
    ss << std::hex << std::showbase;
    ss << "hVFhandle: " << hVFhandle;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pEngineUtil == nullptr) {
        ss << ", pEngineUtil: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pEngineUtil[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pEngineUtil[0].stype;
            ss << ", pNext: " << pEngineUtil[0].pNext;
            ss << ", type: " << pEngineUtil[0].type;
            ss << ", activeCounterValue: " << pEngineUtil[0].activeCounterValue;
            ss << ", samplingCounterValue: " << pEngineUtil[0].samplingCounterValue;
            ss << ", timestamp: " << pEngineUtil[0].timestamp;
            ss << "}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesVFManagementGetVFEngineUtilizationExp(zes_vf_handle_t hVFhandle,
                                                           uint32_t *pCount,
                                                           zes_vf_util_engine_exp_t *pEngineUtil) {
    TRACE_EVENT_BEGIN("API", "zesVFManagementGetVFEngineUtilizationExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementGetVFEngineUtilizationExp(hVFhandle,
                                                                     pCount,
                                                                     pEngineUtil) +
                         "..\n";
}
inline void trace_zesVFManagementGetVFEngineUtilizationExp(ze_result_t ret,
                                                           zes_vf_handle_t hVFhandle,
                                                           uint32_t *pCount,
                                                           zes_vf_util_engine_exp_t *pEngineUtil) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementGetVFEngineUtilizationExp(hVFhandle,
                                                                     pCount,
                                                                     pEngineUtil) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesVFManagementSetVFTelemetryModeExp(zes_vf_handle_t hVFhandle,
                                                               zes_vf_info_util_exp_flags_t flags,
                                                               ze_bool_t enable) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesVFManagementSetVFTelemetryModeExp(";
    ss << std::hex << std::showbase;
    ss << "hVFhandle: " << hVFhandle;
    ss << ", flags: " << flags;
    ss << ", enable: " << enable;
    ss << ")";
    return ss.str();
}
inline void trace_zesVFManagementSetVFTelemetryModeExp(zes_vf_handle_t hVFhandle,
                                                       zes_vf_info_util_exp_flags_t flags,
                                                       ze_bool_t enable) {
    TRACE_EVENT_BEGIN("API", "zesVFManagementSetVFTelemetryModeExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementSetVFTelemetryModeExp(hVFhandle, flags, enable) + "..\n";
}
inline void trace_zesVFManagementSetVFTelemetryModeExp(ze_result_t ret,
                                                       zes_vf_handle_t hVFhandle,
                                                       zes_vf_info_util_exp_flags_t flags,
                                                       ze_bool_t enable) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementSetVFTelemetryModeExp(hVFhandle, flags, enable) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesVFManagementSetVFTelemetrySamplingIntervalExp(zes_vf_handle_t hVFhandle,
                                                        zes_vf_info_util_exp_flags_t flag,
                                                        uint64_t samplingInterval) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid()
       << "] zesVFManagementSetVFTelemetrySamplingIntervalExp(";
    ss << std::hex << std::showbase;
    ss << "hVFhandle: " << hVFhandle;
    ss << ", flag: " << flag;
    ss << ", samplingInterval: " << samplingInterval;
    ss << ")";
    return ss.str();
}
inline void
trace_zesVFManagementSetVFTelemetrySamplingIntervalExp(zes_vf_handle_t hVFhandle,
                                                       zes_vf_info_util_exp_flags_t flag,
                                                       uint64_t samplingInterval) {
    TRACE_EVENT_BEGIN("API", "zesVFManagementSetVFTelemetrySamplingIntervalExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementSetVFTelemetrySamplingIntervalExp(hVFhandle,
                                                                             flag,
                                                                             samplingInterval) +
                         "..\n";
}
inline void
trace_zesVFManagementSetVFTelemetrySamplingIntervalExp(ze_result_t ret,
                                                       zes_vf_handle_t hVFhandle,
                                                       zes_vf_info_util_exp_flags_t flag,
                                                       uint64_t samplingInterval) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementSetVFTelemetrySamplingIntervalExp(hVFhandle,
                                                                             flag,
                                                                             samplingInterval) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zesDeviceEnumEnabledVFExp(zes_device_handle_t hDevice,
                                                    uint32_t *pCount,
                                                    zes_vf_handle_t *phVFhandle) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesDeviceEnumEnabledVFExp(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phVFhandle == nullptr) {
        ss << ", phVFhandle: nullptr";
    } else {
        ss << ", phVFhandle: " << *phVFhandle;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesDeviceEnumEnabledVFExp(zes_device_handle_t hDevice,
                                            uint32_t *pCount,
                                            zes_vf_handle_t *phVFhandle) {
    TRACE_EVENT_BEGIN("API", "zesDeviceEnumEnabledVFExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumEnabledVFExp(hDevice, pCount, phVFhandle) + "..\n";
}
inline void trace_zesDeviceEnumEnabledVFExp(ze_result_t ret,
                                            zes_device_handle_t hDevice,
                                            uint32_t *pCount,
                                            zes_vf_handle_t *phVFhandle) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesDeviceEnumEnabledVFExp(hDevice, pCount, phVFhandle) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesVFManagementGetVFCapabilitiesExp(zes_vf_handle_t hVFhandle,
                                           zes_vf_exp_capabilities_t *pCapability) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesVFManagementGetVFCapabilitiesExp(";
    ss << std::hex << std::showbase;
    ss << "hVFhandle: " << hVFhandle;
    if (pCapability == nullptr) {
        ss << ", pCapability: nullptr";
    } else {
        ss << ", pCapability {";
        ss << "stype: " << pCapability->stype;
        ss << ", pNext: " << pCapability->pNext;
        ss << ", address {";
        ss << "domain: " << pCapability->address.domain;
        ss << ", bus: " << pCapability->address.bus;
        ss << ", device: " << pCapability->address.device;
        ss << ", function: " << pCapability->address.function;
        ss << "}";
        ss << ", vfDeviceMemSize: " << pCapability->vfDeviceMemSize;
        ss << ", vfID: " << pCapability->vfID;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesVFManagementGetVFCapabilitiesExp(zes_vf_handle_t hVFhandle,
                                                      zes_vf_exp_capabilities_t *pCapability) {
    TRACE_EVENT_BEGIN("API", "zesVFManagementGetVFCapabilitiesExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementGetVFCapabilitiesExp(hVFhandle, pCapability) + "..\n";
}
inline void trace_zesVFManagementGetVFCapabilitiesExp(ze_result_t ret,
                                                      zes_vf_handle_t hVFhandle,
                                                      zes_vf_exp_capabilities_t *pCapability) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementGetVFCapabilitiesExp(hVFhandle, pCapability) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesVFManagementGetVFMemoryUtilizationExp2(zes_vf_handle_t hVFhandle,
                                                 uint32_t *pCount,
                                                 zes_vf_util_mem_exp2_t *pMemUtil) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesVFManagementGetVFMemoryUtilizationExp2(";
    ss << std::hex << std::showbase;
    ss << "hVFhandle: " << hVFhandle;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pMemUtil == nullptr) {
        ss << ", pMemUtil: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pMemUtil[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pMemUtil[0].stype;
            ss << ", pNext: " << pMemUtil[0].pNext;
            ss << ", vfMemLocation: " << pMemUtil[0].vfMemLocation;
            ss << ", vfMemUtilized: " << pMemUtil[0].vfMemUtilized;
            ss << "}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zesVFManagementGetVFMemoryUtilizationExp2(zes_vf_handle_t hVFhandle,
                                                            uint32_t *pCount,
                                                            zes_vf_util_mem_exp2_t *pMemUtil) {
    TRACE_EVENT_BEGIN("API", "zesVFManagementGetVFMemoryUtilizationExp2");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementGetVFMemoryUtilizationExp2(hVFhandle, pCount, pMemUtil) +
                         "..\n";
}
inline void trace_zesVFManagementGetVFMemoryUtilizationExp2(ze_result_t ret,
                                                            zes_vf_handle_t hVFhandle,
                                                            uint32_t *pCount,
                                                            zes_vf_util_mem_exp2_t *pMemUtil) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementGetVFMemoryUtilizationExp2(hVFhandle, pCount, pMemUtil) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zesVFManagementGetVFEngineUtilizationExp2(zes_vf_handle_t hVFhandle,
                                                 uint32_t *pCount,
                                                 zes_vf_util_engine_exp2_t *pEngineUtil) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zesVFManagementGetVFEngineUtilizationExp2(";
    ss << std::hex << std::showbase;
    ss << "hVFhandle: " << hVFhandle;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pEngineUtil == nullptr) {
        ss << ", pEngineUtil: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pEngineUtil[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pEngineUtil[0].stype;
            ss << ", pNext: " << pEngineUtil[0].pNext;
            ss << ", vfEngineType: " << pEngineUtil[0].vfEngineType;
            ss << ", activeCounterValue: " << pEngineUtil[0].activeCounterValue;
            ss << ", samplingCounterValue: " << pEngineUtil[0].samplingCounterValue;
            ss << "}";
            if (count > 1) {
                ss << ", ...";
            }
        }
        ss << "]";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zesVFManagementGetVFEngineUtilizationExp2(zes_vf_handle_t hVFhandle,
                                                uint32_t *pCount,
                                                zes_vf_util_engine_exp2_t *pEngineUtil) {
    TRACE_EVENT_BEGIN("API", "zesVFManagementGetVFEngineUtilizationExp2");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementGetVFEngineUtilizationExp2(hVFhandle,
                                                                      pCount,
                                                                      pEngineUtil) +
                         "..\n";
}
inline void
trace_zesVFManagementGetVFEngineUtilizationExp2(ze_result_t ret,
                                                zes_vf_handle_t hVFhandle,
                                                uint32_t *pCount,
                                                zes_vf_util_engine_exp2_t *pEngineUtil) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zesVFManagementGetVFEngineUtilizationExp2(hVFhandle,
                                                                      pCount,
                                                                      pEngineUtil) +
                         trace_ze_result_t(ret);
}
