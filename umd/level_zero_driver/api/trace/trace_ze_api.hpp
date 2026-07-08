/*
 * Copyright (C) 2024-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Generated header based on ze_api.h@version v1.11-r1.11.4

#pragma once

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/trace_perfetto.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <ze_api.h>
#include <ze_command_queue_npu_ext.h>
#include <ze_context_npu_ext.h>

#ifdef ENABLE_NPU_LOGGING
#define IS_API_TRACE() (VPU::getLogLevel() == INFO && VPU::getLogMask() & API)
#define IS_API_DDI_TRACE() (VPU::getLogLevel() == INFO && VPU::getLogMask() & API_DDI)
#define IS_API_EXT_TRACE() (VPU::getLogLevel() == INFO && VPU::getLogMask() & API_EXT)
#else
#define IS_API_TRACE() false
#define IS_API_DDI_TRACE() false
#define IS_API_EXT_TRACE() false
#endif

inline std::stringstream trace_ascii_array(const char *value, size_t maxLen) {
    std::stringstream ss;
    for (size_t i = 0; i <= maxLen; ++i) {
        if (i == maxLen) {
            ss << "...";
            break;
        }
        const auto c = static_cast<unsigned char>(value[i]);
        if (c == '\0') {
            break;
        }
        if (std::isprint(c) != 0) {
            ss << static_cast<char>(c);
        } else {
            ss << "0x" << std::hex << std::setw(2) << std::setfill('0')
               << static_cast<unsigned int>(c);
        }
    }
    return ss;
}

inline std::stringstream trace_u8_array_hex(const uint8_t *value, size_t len) {
    std::stringstream ss;
    for (size_t i = 0; i < len; ++i) {
        const auto c = static_cast<unsigned char>(value[i]);
        if (i > 0) {
            ss << " ";
        }
        ss << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(c);
    }
    return ss;
}

inline std::stringstream trace_char_array_hex(const char *value, size_t len) {
    return trace_u8_array_hex(reinterpret_cast<const uint8_t *>(value), len);
}

const inline char *ze_result_to_str(unsigned long r) {
    switch (r) {
        CASE_RETURN_STR(ZE_RESULT_SUCCESS);
        CASE_RETURN_STR(ZE_RESULT_NOT_READY);
        CASE_RETURN_STR(ZE_RESULT_ERROR_DEVICE_LOST);
        CASE_RETURN_STR(ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY);
        CASE_RETURN_STR(ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY);
        CASE_RETURN_STR(ZE_RESULT_ERROR_MODULE_BUILD_FAILURE);
        CASE_RETURN_STR(ZE_RESULT_ERROR_MODULE_LINK_FAILURE);
        CASE_RETURN_STR(ZE_RESULT_ERROR_DEVICE_REQUIRES_RESET);
        CASE_RETURN_STR(ZE_RESULT_ERROR_DEVICE_IN_LOW_POWER_STATE);
        CASE_RETURN_STR(ZE_RESULT_EXP_ERROR_DEVICE_IS_NOT_VERTEX);
        CASE_RETURN_STR(ZE_RESULT_EXP_ERROR_VERTEX_IS_NOT_DEVICE);
        CASE_RETURN_STR(ZE_RESULT_EXP_ERROR_REMOTE_DEVICE);
        CASE_RETURN_STR(ZE_RESULT_EXP_ERROR_OPERANDS_INCOMPATIBLE);
        CASE_RETURN_STR(ZE_RESULT_EXP_RTAS_BUILD_RETRY);
        CASE_RETURN_STR(ZE_RESULT_EXP_RTAS_BUILD_DEFERRED);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INSUFFICIENT_PERMISSIONS);
        CASE_RETURN_STR(ZE_RESULT_ERROR_NOT_AVAILABLE);
        CASE_RETURN_STR(ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE);
        CASE_RETURN_STR(ZE_RESULT_WARNING_DROPPED_DATA);
        CASE_RETURN_STR(ZE_RESULT_ERROR_UNINITIALIZED);
        CASE_RETURN_STR(ZE_RESULT_ERROR_UNSUPPORTED_VERSION);
        CASE_RETURN_STR(ZE_RESULT_ERROR_UNSUPPORTED_FEATURE);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_ARGUMENT);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_NULL_HANDLE);
        CASE_RETURN_STR(ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_NULL_POINTER);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_SIZE);
        CASE_RETURN_STR(ZE_RESULT_ERROR_UNSUPPORTED_SIZE);
        CASE_RETURN_STR(ZE_RESULT_ERROR_UNSUPPORTED_ALIGNMENT);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_SYNCHRONIZATION_OBJECT);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_ENUMERATION);
        CASE_RETURN_STR(ZE_RESULT_ERROR_UNSUPPORTED_ENUMERATION);
        CASE_RETURN_STR(ZE_RESULT_ERROR_UNSUPPORTED_IMAGE_FORMAT);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_NATIVE_BINARY);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_GLOBAL_NAME);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_KERNEL_NAME);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_FUNCTION_NAME);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_GROUP_SIZE_DIMENSION);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_GLOBAL_WIDTH_DIMENSION);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_KERNEL_ARGUMENT_INDEX);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_KERNEL_ARGUMENT_SIZE);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_KERNEL_ATTRIBUTE_VALUE);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_MODULE_UNLINKED);
        CASE_RETURN_STR(ZE_RESULT_ERROR_INVALID_COMMAND_LIST_TYPE);
        CASE_RETURN_STR(ZE_RESULT_ERROR_OVERLAPPING_REGIONS);
        CASE_RETURN_STR(ZE_RESULT_WARNING_ACTION_REQUIRED);
        CASE_RETURN_STR(ZE_RESULT_ERROR_UNKNOWN);
        CASE_RETURN_STR(ZE_RESULT_FORCE_UINT32);
    default:
        return "No stringify for ze_result_t";
    };
}

inline std::string trace_ze_result_t(ze_result_t ret) {
    std::stringstream ss;
    if (ret)
        ss << " = " << ret << " [" << ze_result_to_str(ret) << "]\n";
    else
        ss << " = 0\n";
    return ss.str();
}

inline std::string _trace_zeInit(ze_init_flags_t flags) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeInit(";
    ss << std::hex << std::showbase;
    ss << "flags: " << flags;
    ss << ")";
    return ss.str();
}
inline void trace_zeInit(ze_init_flags_t flags) {
    TRACE_EVENT_BEGIN("API", "zeInit");
    if (IS_API_TRACE())
        std::cerr << _trace_zeInit(flags) + "..\n";
}
inline void trace_zeInit(ze_result_t ret, ze_init_flags_t flags) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeInit(flags) + trace_ze_result_t(ret);
}

inline std::string _trace_zeDriverGet(uint32_t *pCount, ze_driver_handle_t *phDrivers) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDriverGet(";
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
inline void trace_zeDriverGet(uint32_t *pCount, ze_driver_handle_t *phDrivers) {
    TRACE_EVENT_BEGIN("API", "zeDriverGet");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGet(pCount, phDrivers) + "..\n";
}
inline void trace_zeDriverGet(ze_result_t ret, uint32_t *pCount, ze_driver_handle_t *phDrivers) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGet(pCount, phDrivers) + trace_ze_result_t(ret);
}

inline std::string _trace_zeInitDrivers(uint32_t *pCount,
                                        ze_driver_handle_t *phDrivers,
                                        ze_init_driver_type_desc_t *desc) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeInitDrivers(";
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
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeInitDrivers(uint32_t *pCount,
                                ze_driver_handle_t *phDrivers,
                                ze_init_driver_type_desc_t *desc) {
    TRACE_EVENT_BEGIN("API", "zeInitDrivers");
    if (IS_API_TRACE())
        std::cerr << _trace_zeInitDrivers(pCount, phDrivers, desc) + "..\n";
}
inline void trace_zeInitDrivers(ze_result_t ret,
                                uint32_t *pCount,
                                ze_driver_handle_t *phDrivers,
                                ze_init_driver_type_desc_t *desc) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeInitDrivers(pCount, phDrivers, desc) + trace_ze_result_t(ret);
}

inline std::string _trace_zeDriverGetApiVersion(ze_driver_handle_t hDriver,
                                                ze_api_version_t *version) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDriverGetApiVersion(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    if (version == nullptr) {
        ss << ", version: nullptr";
    } else {
        ss << ", version: " << *version;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDriverGetApiVersion(ze_driver_handle_t hDriver, ze_api_version_t *version) {
    TRACE_EVENT_BEGIN("API", "zeDriverGetApiVersion");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGetApiVersion(hDriver, version) + "..\n";
}
inline void trace_zeDriverGetApiVersion(ze_result_t ret,
                                        ze_driver_handle_t hDriver,
                                        ze_api_version_t *version) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGetApiVersion(hDriver, version) + trace_ze_result_t(ret);
}

inline std::string _trace_zeDriverGetProperties(ze_driver_handle_t hDriver,
                                                ze_driver_properties_t *pDriverProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDriverGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    if (pDriverProperties == nullptr) {
        ss << ", pDriverProperties: nullptr";
    } else {
        ss << ", pDriverProperties {";
        ss << "stype: " << pDriverProperties->stype;
        ss << ", pNext: " << pDriverProperties->pNext;
        ss << ", uuid {";
        ss << "id: ";
        ss << trace_u8_array_hex(pDriverProperties->uuid.id, sizeof(pDriverProperties->uuid.id))
                  .str();
        ss << "}";
        ss << ", driverVersion: " << pDriverProperties->driverVersion;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDriverGetProperties(ze_driver_handle_t hDriver,
                                        ze_driver_properties_t *pDriverProperties) {
    TRACE_EVENT_BEGIN("API", "zeDriverGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGetProperties(hDriver, pDriverProperties) + "..\n";
}
inline void trace_zeDriverGetProperties(ze_result_t ret,
                                        ze_driver_handle_t hDriver,
                                        ze_driver_properties_t *pDriverProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGetProperties(hDriver, pDriverProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeDriverGetIpcProperties(ze_driver_handle_t hDriver,
                                                   ze_driver_ipc_properties_t *pIpcProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDriverGetIpcProperties(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    if (pIpcProperties == nullptr) {
        ss << ", pIpcProperties: nullptr";
    } else {
        ss << ", pIpcProperties {";
        ss << "stype: " << pIpcProperties->stype;
        ss << ", pNext: " << pIpcProperties->pNext;
        ss << ", flags: " << pIpcProperties->flags;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDriverGetIpcProperties(ze_driver_handle_t hDriver,
                                           ze_driver_ipc_properties_t *pIpcProperties) {
    TRACE_EVENT_BEGIN("API", "zeDriverGetIpcProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGetIpcProperties(hDriver, pIpcProperties) + "..\n";
}
inline void trace_zeDriverGetIpcProperties(ze_result_t ret,
                                           ze_driver_handle_t hDriver,
                                           ze_driver_ipc_properties_t *pIpcProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGetIpcProperties(hDriver, pIpcProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeDriverGetExtensionProperties(ze_driver_handle_t hDriver,
                                      uint32_t *pCount,
                                      ze_driver_extension_properties_t *pExtensionProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDriverGetExtensionProperties(";
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
            ss << "name: ";
            ss << trace_ascii_array(pExtensionProperties[0].name,
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
trace_zeDriverGetExtensionProperties(ze_driver_handle_t hDriver,
                                     uint32_t *pCount,
                                     ze_driver_extension_properties_t *pExtensionProperties) {
    TRACE_EVENT_BEGIN("API", "zeDriverGetExtensionProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGetExtensionProperties(hDriver, pCount, pExtensionProperties) +
                         "..\n";
}
inline void
trace_zeDriverGetExtensionProperties(ze_result_t ret,
                                     ze_driver_handle_t hDriver,
                                     uint32_t *pCount,
                                     ze_driver_extension_properties_t *pExtensionProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGetExtensionProperties(hDriver, pCount, pExtensionProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeDriverGetExtensionFunctionAddress(ze_driver_handle_t hDriver,
                                                              const char *name,
                                                              void **ppFunctionAddress) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDriverGetExtensionFunctionAddress(";
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
inline void trace_zeDriverGetExtensionFunctionAddress(ze_driver_handle_t hDriver,
                                                      const char *name,
                                                      void **ppFunctionAddress) {
    TRACE_EVENT_BEGIN("API", "zeDriverGetExtensionFunctionAddress");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGetExtensionFunctionAddress(hDriver, name, ppFunctionAddress) +
                         "..\n";
}
inline void trace_zeDriverGetExtensionFunctionAddress(ze_result_t ret,
                                                      ze_driver_handle_t hDriver,
                                                      const char *name,
                                                      void **ppFunctionAddress) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGetExtensionFunctionAddress(hDriver, name, ppFunctionAddress) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeDriverGetLastErrorDescription(ze_driver_handle_t hDriver,
                                                          const char **ppString) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDriverGetLastErrorDescription(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    ss << ", ppString: " << ppString;
    ss << ")";
    return ss.str();
}
inline void trace_zeDriverGetLastErrorDescription(ze_driver_handle_t hDriver,
                                                  const char **ppString) {
    TRACE_EVENT_BEGIN("API", "zeDriverGetLastErrorDescription");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGetLastErrorDescription(hDriver, ppString) + "..\n";
}
inline void trace_zeDriverGetLastErrorDescription(ze_result_t ret,
                                                  ze_driver_handle_t hDriver,
                                                  const char **ppString) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverGetLastErrorDescription(hDriver, ppString) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeDeviceGet(ze_driver_handle_t hDriver, uint32_t *pCount, ze_device_handle_t *phDevices) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGet(";
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
trace_zeDeviceGet(ze_driver_handle_t hDriver, uint32_t *pCount, ze_device_handle_t *phDevices) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGet");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGet(hDriver, pCount, phDevices) + "..\n";
}
inline void trace_zeDeviceGet(ze_result_t ret,
                              ze_driver_handle_t hDriver,
                              uint32_t *pCount,
                              ze_device_handle_t *phDevices) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGet(hDriver, pCount, phDevices) + trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceGetRootDevice(ze_device_handle_t hDevice,
                                                ze_device_handle_t *phRootDevice) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetRootDevice(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (phRootDevice == nullptr) {
        ss << ", phRootDevice: nullptr";
    } else {
        ss << ", phRootDevice: " << *phRootDevice;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetRootDevice(ze_device_handle_t hDevice,
                                        ze_device_handle_t *phRootDevice) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetRootDevice");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetRootDevice(hDevice, phRootDevice) + "..\n";
}
inline void trace_zeDeviceGetRootDevice(ze_result_t ret,
                                        ze_device_handle_t hDevice,
                                        ze_device_handle_t *phRootDevice) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetRootDevice(hDevice, phRootDevice) + trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceGetSubDevices(ze_device_handle_t hDevice,
                                                uint32_t *pCount,
                                                ze_device_handle_t *phSubdevices) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetSubDevices(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phSubdevices == nullptr) {
        ss << ", phSubdevices: nullptr";
    } else {
        ss << ", phSubdevices: " << *phSubdevices;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetSubDevices(ze_device_handle_t hDevice,
                                        uint32_t *pCount,
                                        ze_device_handle_t *phSubdevices) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetSubDevices");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetSubDevices(hDevice, pCount, phSubdevices) + "..\n";
}
inline void trace_zeDeviceGetSubDevices(ze_result_t ret,
                                        ze_device_handle_t hDevice,
                                        uint32_t *pCount,
                                        ze_device_handle_t *phSubdevices) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetSubDevices(hDevice, pCount, phSubdevices) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceGetProperties(ze_device_handle_t hDevice,
                                                ze_device_properties_t *pDeviceProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pDeviceProperties == nullptr) {
        ss << ", pDeviceProperties: nullptr";
    } else {
        ss << ", pDeviceProperties {";
        ss << "stype: " << pDeviceProperties->stype;
        ss << ", pNext: " << pDeviceProperties->pNext;
        ss << ", type: " << pDeviceProperties->type;
        ss << ", vendorId: " << pDeviceProperties->vendorId;
        ss << ", deviceId: " << pDeviceProperties->deviceId;
        ss << ", flags: " << pDeviceProperties->flags;
        ss << ", subdeviceId: " << pDeviceProperties->subdeviceId;
        ss << ", coreClockRate: " << pDeviceProperties->coreClockRate;
        ss << ", maxMemAllocSize: " << pDeviceProperties->maxMemAllocSize;
        ss << ", maxHardwareContexts: " << pDeviceProperties->maxHardwareContexts;
        ss << ", maxCommandQueuePriority: " << pDeviceProperties->maxCommandQueuePriority;
        ss << ", numThreadsPerEU: " << pDeviceProperties->numThreadsPerEU;
        ss << ", physicalEUSimdWidth: " << pDeviceProperties->physicalEUSimdWidth;
        ss << ", numEUsPerSubslice: " << pDeviceProperties->numEUsPerSubslice;
        ss << ", numSubslicesPerSlice: " << pDeviceProperties->numSubslicesPerSlice;
        ss << ", numSlices: " << pDeviceProperties->numSlices;
        ss << ", timerResolution: " << pDeviceProperties->timerResolution;
        ss << ", timestampValidBits: " << pDeviceProperties->timestampValidBits;
        ss << ", kernelTimestampValidBits: " << pDeviceProperties->kernelTimestampValidBits;
        ss << ", uuid {";
        ss << "id: ";
        ss << trace_u8_array_hex(pDeviceProperties->uuid.id, sizeof(pDeviceProperties->uuid.id))
                  .str();
        ss << "}";
        ss << ", name: ";
        ss << trace_ascii_array(pDeviceProperties->name, sizeof(pDeviceProperties->name)).str();
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetProperties(ze_device_handle_t hDevice,
                                        ze_device_properties_t *pDeviceProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetProperties(hDevice, pDeviceProperties) + "..\n";
}
inline void trace_zeDeviceGetProperties(ze_result_t ret,
                                        ze_device_handle_t hDevice,
                                        ze_device_properties_t *pDeviceProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetProperties(hDevice, pDeviceProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeDeviceGetComputeProperties(ze_device_handle_t hDevice,
                                    ze_device_compute_properties_t *pComputeProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetComputeProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pComputeProperties == nullptr) {
        ss << ", pComputeProperties: nullptr";
    } else {
        ss << ", pComputeProperties {";
        ss << "stype: " << pComputeProperties->stype;
        ss << ", pNext: " << pComputeProperties->pNext;
        ss << ", maxTotalGroupSize: " << pComputeProperties->maxTotalGroupSize;
        ss << ", maxGroupSizeX: " << pComputeProperties->maxGroupSizeX;
        ss << ", maxGroupSizeY: " << pComputeProperties->maxGroupSizeY;
        ss << ", maxGroupSizeZ: " << pComputeProperties->maxGroupSizeZ;
        ss << ", maxGroupCountX: " << pComputeProperties->maxGroupCountX;
        ss << ", maxGroupCountY: " << pComputeProperties->maxGroupCountY;
        ss << ", maxGroupCountZ: " << pComputeProperties->maxGroupCountZ;
        ss << ", maxSharedLocalMemory: " << pComputeProperties->maxSharedLocalMemory;
        ss << ", numSubGroupSizes: " << pComputeProperties->numSubGroupSizes;
        ss << ", subGroupSizes: " << pComputeProperties->subGroupSizes[0]
           << pComputeProperties->subGroupSizes[1] << pComputeProperties->subGroupSizes[2]
           << pComputeProperties->subGroupSizes[3] << pComputeProperties->subGroupSizes[4]
           << pComputeProperties->subGroupSizes[5] << pComputeProperties->subGroupSizes[6]
           << pComputeProperties->subGroupSizes[7];
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetComputeProperties(ze_device_handle_t hDevice,
                                               ze_device_compute_properties_t *pComputeProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetComputeProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetComputeProperties(hDevice, pComputeProperties) + "..\n";
}
inline void trace_zeDeviceGetComputeProperties(ze_result_t ret,
                                               ze_device_handle_t hDevice,
                                               ze_device_compute_properties_t *pComputeProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetComputeProperties(hDevice, pComputeProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeDeviceGetModuleProperties(ze_device_handle_t hDevice,
                                   ze_device_module_properties_t *pModuleProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetModuleProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pModuleProperties == nullptr) {
        ss << ", pModuleProperties: nullptr";
    } else {
        ss << ", pModuleProperties {";
        ss << "stype: " << pModuleProperties->stype;
        ss << ", pNext: " << pModuleProperties->pNext;
        ss << ", spirvVersionSupported: " << pModuleProperties->spirvVersionSupported;
        ss << ", flags: " << pModuleProperties->flags;
        ss << ", fp16flags: " << pModuleProperties->fp16flags;
        ss << ", fp32flags: " << pModuleProperties->fp32flags;
        ss << ", fp64flags: " << pModuleProperties->fp64flags;
        ss << ", maxArgumentsSize: " << pModuleProperties->maxArgumentsSize;
        ss << ", printfBufferSize: " << pModuleProperties->printfBufferSize;
        ss << ", nativeKernelSupported {";
        ss << "id: ";
        ss << trace_u8_array_hex(pModuleProperties->nativeKernelSupported.id,
                                 sizeof(pModuleProperties->nativeKernelSupported.id))
                  .str();
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetModuleProperties(ze_device_handle_t hDevice,
                                              ze_device_module_properties_t *pModuleProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetModuleProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetModuleProperties(hDevice, pModuleProperties) + "..\n";
}
inline void trace_zeDeviceGetModuleProperties(ze_result_t ret,
                                              ze_device_handle_t hDevice,
                                              ze_device_module_properties_t *pModuleProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetModuleProperties(hDevice, pModuleProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceGetCommandQueueGroupProperties(
    ze_device_handle_t hDevice,
    uint32_t *pCount,
    ze_command_queue_group_properties_t *pCommandQueueGroupProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetCommandQueueGroupProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pCommandQueueGroupProperties == nullptr) {
        ss << ", pCommandQueueGroupProperties: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pCommandQueueGroupProperties[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pCommandQueueGroupProperties[0].stype;
            ss << ", pNext: " << pCommandQueueGroupProperties[0].pNext;
            ss << ", flags: " << pCommandQueueGroupProperties[0].flags;
            ss << ", maxMemoryFillPatternSize: "
               << pCommandQueueGroupProperties[0].maxMemoryFillPatternSize;
            ss << ", numQueues: " << pCommandQueueGroupProperties[0].numQueues;
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
inline void trace_zeDeviceGetCommandQueueGroupProperties(
    ze_device_handle_t hDevice,
    uint32_t *pCount,
    ze_command_queue_group_properties_t *pCommandQueueGroupProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetCommandQueueGroupProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetCommandQueueGroupProperties(hDevice,
                                                                   pCount,
                                                                   pCommandQueueGroupProperties) +
                         "..\n";
}
inline void trace_zeDeviceGetCommandQueueGroupProperties(
    ze_result_t ret,
    ze_device_handle_t hDevice,
    uint32_t *pCount,
    ze_command_queue_group_properties_t *pCommandQueueGroupProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetCommandQueueGroupProperties(hDevice,
                                                                   pCount,
                                                                   pCommandQueueGroupProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeDeviceGetMemoryProperties(ze_device_handle_t hDevice,
                                   uint32_t *pCount,
                                   ze_device_memory_properties_t *pMemProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetMemoryProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pMemProperties == nullptr) {
        ss << ", pMemProperties: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pMemProperties[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pMemProperties[0].stype;
            ss << ", pNext: " << pMemProperties[0].pNext;
            ss << ", flags: " << pMemProperties[0].flags;
            ss << ", maxClockRate: " << pMemProperties[0].maxClockRate;
            ss << ", maxBusWidth: " << pMemProperties[0].maxBusWidth;
            ss << ", totalSize: " << pMemProperties[0].totalSize;
            ss << ", name: ";
            ss << trace_ascii_array(pMemProperties[0].name, sizeof(pMemProperties[0].name)).str();
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
inline void trace_zeDeviceGetMemoryProperties(ze_device_handle_t hDevice,
                                              uint32_t *pCount,
                                              ze_device_memory_properties_t *pMemProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetMemoryProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetMemoryProperties(hDevice, pCount, pMemProperties) + "..\n";
}
inline void trace_zeDeviceGetMemoryProperties(ze_result_t ret,
                                              ze_device_handle_t hDevice,
                                              uint32_t *pCount,
                                              ze_device_memory_properties_t *pMemProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetMemoryProperties(hDevice, pCount, pMemProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceGetMemoryAccessProperties(
    ze_device_handle_t hDevice,
    ze_device_memory_access_properties_t *pMemAccessProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetMemoryAccessProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pMemAccessProperties == nullptr) {
        ss << ", pMemAccessProperties: nullptr";
    } else {
        ss << ", pMemAccessProperties {";
        ss << "stype: " << pMemAccessProperties->stype;
        ss << ", pNext: " << pMemAccessProperties->pNext;
        ss << ", hostAllocCapabilities: " << pMemAccessProperties->hostAllocCapabilities;
        ss << ", deviceAllocCapabilities: " << pMemAccessProperties->deviceAllocCapabilities;
        ss << ", sharedSingleDeviceAllocCapabilities: "
           << pMemAccessProperties->sharedSingleDeviceAllocCapabilities;
        ss << ", sharedCrossDeviceAllocCapabilities: "
           << pMemAccessProperties->sharedCrossDeviceAllocCapabilities;
        ss << ", sharedSystemAllocCapabilities: "
           << pMemAccessProperties->sharedSystemAllocCapabilities;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetMemoryAccessProperties(
    ze_device_handle_t hDevice,
    ze_device_memory_access_properties_t *pMemAccessProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetMemoryAccessProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetMemoryAccessProperties(hDevice, pMemAccessProperties) +
                         "..\n";
}
inline void trace_zeDeviceGetMemoryAccessProperties(
    ze_result_t ret,
    ze_device_handle_t hDevice,
    ze_device_memory_access_properties_t *pMemAccessProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetMemoryAccessProperties(hDevice, pMemAccessProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeDeviceGetCacheProperties(ze_device_handle_t hDevice,
                                  uint32_t *pCount,
                                  ze_device_cache_properties_t *pCacheProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetCacheProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pCacheProperties == nullptr) {
        ss << ", pCacheProperties: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pCacheProperties[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pCacheProperties[0].stype;
            ss << ", pNext: " << pCacheProperties[0].pNext;
            ss << ", flags: " << pCacheProperties[0].flags;
            ss << ", cacheSize: " << pCacheProperties[0].cacheSize;
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
inline void trace_zeDeviceGetCacheProperties(ze_device_handle_t hDevice,
                                             uint32_t *pCount,
                                             ze_device_cache_properties_t *pCacheProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetCacheProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetCacheProperties(hDevice, pCount, pCacheProperties) + "..\n";
}
inline void trace_zeDeviceGetCacheProperties(ze_result_t ret,
                                             ze_device_handle_t hDevice,
                                             uint32_t *pCount,
                                             ze_device_cache_properties_t *pCacheProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetCacheProperties(hDevice, pCount, pCacheProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeDeviceGetImageProperties(ze_device_handle_t hDevice,
                                  ze_device_image_properties_t *pImageProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetImageProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pImageProperties == nullptr) {
        ss << ", pImageProperties: nullptr";
    } else {
        ss << ", pImageProperties {";
        ss << "stype: " << pImageProperties->stype;
        ss << ", pNext: " << pImageProperties->pNext;
        ss << ", maxImageDims1D: " << pImageProperties->maxImageDims1D;
        ss << ", maxImageDims2D: " << pImageProperties->maxImageDims2D;
        ss << ", maxImageDims3D: " << pImageProperties->maxImageDims3D;
        ss << ", maxImageBufferSize: " << pImageProperties->maxImageBufferSize;
        ss << ", maxImageArraySlices: " << pImageProperties->maxImageArraySlices;
        ss << ", maxSamplers: " << pImageProperties->maxSamplers;
        ss << ", maxReadImageArgs: " << pImageProperties->maxReadImageArgs;
        ss << ", maxWriteImageArgs: " << pImageProperties->maxWriteImageArgs;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetImageProperties(ze_device_handle_t hDevice,
                                             ze_device_image_properties_t *pImageProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetImageProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetImageProperties(hDevice, pImageProperties) + "..\n";
}
inline void trace_zeDeviceGetImageProperties(ze_result_t ret,
                                             ze_device_handle_t hDevice,
                                             ze_device_image_properties_t *pImageProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetImageProperties(hDevice, pImageProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceGetExternalMemoryProperties(
    ze_device_handle_t hDevice,
    ze_device_external_memory_properties_t *pExternalMemoryProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetExternalMemoryProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pExternalMemoryProperties == nullptr) {
        ss << ", pExternalMemoryProperties: nullptr";
    } else {
        ss << ", pExternalMemoryProperties {";
        ss << "stype: " << pExternalMemoryProperties->stype;
        ss << ", pNext: " << pExternalMemoryProperties->pNext;
        ss << ", memoryAllocationImportTypes: "
           << pExternalMemoryProperties->memoryAllocationImportTypes;
        ss << ", memoryAllocationExportTypes: "
           << pExternalMemoryProperties->memoryAllocationExportTypes;
        ss << ", imageImportTypes: " << pExternalMemoryProperties->imageImportTypes;
        ss << ", imageExportTypes: " << pExternalMemoryProperties->imageExportTypes;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetExternalMemoryProperties(
    ze_device_handle_t hDevice,
    ze_device_external_memory_properties_t *pExternalMemoryProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetExternalMemoryProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetExternalMemoryProperties(hDevice,
                                                                pExternalMemoryProperties) +
                         "..\n";
}
inline void trace_zeDeviceGetExternalMemoryProperties(
    ze_result_t ret,
    ze_device_handle_t hDevice,
    ze_device_external_memory_properties_t *pExternalMemoryProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetExternalMemoryProperties(hDevice,
                                                                pExternalMemoryProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceGetP2PProperties(ze_device_handle_t hDevice,
                                                   ze_device_handle_t hPeerDevice,
                                                   ze_device_p2p_properties_t *pP2PProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetP2PProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ", hPeerDevice: " << hPeerDevice;
    if (pP2PProperties == nullptr) {
        ss << ", pP2PProperties: nullptr";
    } else {
        ss << ", pP2PProperties {";
        ss << "stype: " << pP2PProperties->stype;
        ss << ", pNext: " << pP2PProperties->pNext;
        ss << ", flags: " << pP2PProperties->flags;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetP2PProperties(ze_device_handle_t hDevice,
                                           ze_device_handle_t hPeerDevice,
                                           ze_device_p2p_properties_t *pP2PProperties) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetP2PProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetP2PProperties(hDevice, hPeerDevice, pP2PProperties) + "..\n";
}
inline void trace_zeDeviceGetP2PProperties(ze_result_t ret,
                                           ze_device_handle_t hDevice,
                                           ze_device_handle_t hPeerDevice,
                                           ze_device_p2p_properties_t *pP2PProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetP2PProperties(hDevice, hPeerDevice, pP2PProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceCanAccessPeer(ze_device_handle_t hDevice,
                                                ze_device_handle_t hPeerDevice,
                                                ze_bool_t *value) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceCanAccessPeer(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ", hPeerDevice: " << hPeerDevice;
    if (value == nullptr) {
        ss << ", value: nullptr";
    } else {
        ss << ", value: " << *value;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceCanAccessPeer(ze_device_handle_t hDevice,
                                        ze_device_handle_t hPeerDevice,
                                        ze_bool_t *value) {
    TRACE_EVENT_BEGIN("API", "zeDeviceCanAccessPeer");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceCanAccessPeer(hDevice, hPeerDevice, value) + "..\n";
}
inline void trace_zeDeviceCanAccessPeer(ze_result_t ret,
                                        ze_device_handle_t hDevice,
                                        ze_device_handle_t hPeerDevice,
                                        ze_bool_t *value) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceCanAccessPeer(hDevice, hPeerDevice, value) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceGetStatus(ze_device_handle_t hDevice) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetStatus(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetStatus(ze_device_handle_t hDevice) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetStatus");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetStatus(hDevice) + "..\n";
}
inline void trace_zeDeviceGetStatus(ze_result_t ret, ze_device_handle_t hDevice) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetStatus(hDevice) + trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceGetGlobalTimestamps(ze_device_handle_t hDevice,
                                                      uint64_t *hostTimestamp,
                                                      uint64_t *deviceTimestamp) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetGlobalTimestamps(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (hostTimestamp == nullptr) {
        ss << ", hostTimestamp: nullptr";
    } else {
        ss << ", hostTimestamp: " << *hostTimestamp;
    }
    if (deviceTimestamp == nullptr) {
        ss << ", deviceTimestamp: nullptr";
    } else {
        ss << ", deviceTimestamp: " << *deviceTimestamp;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetGlobalTimestamps(ze_device_handle_t hDevice,
                                              uint64_t *hostTimestamp,
                                              uint64_t *deviceTimestamp) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetGlobalTimestamps");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetGlobalTimestamps(hDevice, hostTimestamp, deviceTimestamp) +
                         "..\n";
}
inline void trace_zeDeviceGetGlobalTimestamps(ze_result_t ret,
                                              ze_device_handle_t hDevice,
                                              uint64_t *hostTimestamp,
                                              uint64_t *deviceTimestamp) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetGlobalTimestamps(hDevice, hostTimestamp, deviceTimestamp) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeContextCreate(ze_driver_handle_t hDriver,
                                          const ze_context_desc_t *desc,
                                          ze_context_handle_t *phContext) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeContextCreate(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << "}";
    }
    if (phContext == nullptr) {
        ss << ", phContext: nullptr";
    } else {
        ss << ", phContext: " << *phContext;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeContextCreate(ze_driver_handle_t hDriver,
                                  const ze_context_desc_t *desc,
                                  ze_context_handle_t *phContext) {
    TRACE_EVENT_BEGIN("API", "zeContextCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextCreate(hDriver, desc, phContext) + "..\n";
}
inline void trace_zeContextCreate(ze_result_t ret,
                                  ze_driver_handle_t hDriver,
                                  const ze_context_desc_t *desc,
                                  ze_context_handle_t *phContext) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextCreate(hDriver, desc, phContext) + trace_ze_result_t(ret);
}

inline std::string _trace_zeContextCreateEx(ze_driver_handle_t hDriver,
                                            const ze_context_desc_t *desc,
                                            uint32_t numDevices,
                                            ze_device_handle_t *phDevices,
                                            ze_context_handle_t *phContext) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeContextCreateEx(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << "}";
    }
    ss << ", numDevices: " << numDevices;
    if (phDevices == nullptr) {
        ss << ", phDevices: nullptr";
    } else {
        ss << ", phDevices: " << *phDevices;
    }
    if (phContext == nullptr) {
        ss << ", phContext: nullptr";
    } else {
        ss << ", phContext: " << *phContext;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeContextCreateEx(ze_driver_handle_t hDriver,
                                    const ze_context_desc_t *desc,
                                    uint32_t numDevices,
                                    ze_device_handle_t *phDevices,
                                    ze_context_handle_t *phContext) {
    TRACE_EVENT_BEGIN("API", "zeContextCreateEx");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextCreateEx(hDriver, desc, numDevices, phDevices, phContext) +
                         "..\n";
}
inline void trace_zeContextCreateEx(ze_result_t ret,
                                    ze_driver_handle_t hDriver,
                                    const ze_context_desc_t *desc,
                                    uint32_t numDevices,
                                    ze_device_handle_t *phDevices,
                                    ze_context_handle_t *phContext) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextCreateEx(hDriver, desc, numDevices, phDevices, phContext) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeContextDestroy(ze_context_handle_t hContext) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeContextDestroy(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ")";
    return ss.str();
}
inline void trace_zeContextDestroy(ze_context_handle_t hContext) {
    TRACE_EVENT_BEGIN("API", "zeContextDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextDestroy(hContext) + "..\n";
}
inline void trace_zeContextDestroy(ze_result_t ret, ze_context_handle_t hContext) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextDestroy(hContext) + trace_ze_result_t(ret);
}

inline std::string _trace_zeContextGetStatus(ze_context_handle_t hContext) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeContextGetStatus(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ")";
    return ss.str();
}
inline void trace_zeContextGetStatus(ze_context_handle_t hContext) {
    TRACE_EVENT_BEGIN("API", "zeContextGetStatus");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextGetStatus(hContext) + "..\n";
}
inline void trace_zeContextGetStatus(ze_result_t ret, ze_context_handle_t hContext) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextGetStatus(hContext) + trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandQueueCreate(ze_context_handle_t hContext,
                                               ze_device_handle_t hDevice,
                                               const ze_command_queue_desc_t *desc,
                                               ze_command_queue_handle_t *phCommandQueue) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandQueueCreate(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", ordinal: " << desc->ordinal;
        ss << ", index: " << desc->index;
        ss << ", flags: " << desc->flags;
        ss << ", mode: " << desc->mode;
        ss << ", priority: " << desc->priority;
        ss << "}";
        if (desc->pNext) {
            const auto *descriptorType =
                reinterpret_cast<const ze_structure_type_command_queue_npu_ext_t *>(desc->pNext);
            switch (*descriptorType) {
            case ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC_NPU_EXT: {
                const auto *pNext =
                    reinterpret_cast<const ze_command_queue_desc_npu_ext_t *>(desc->pNext);
                ss << ", ext desc {";
                ss << "stype: " << pNext->stype;
                ss << ", pNext: " << pNext->pNext;
                ss << ", turbo: " << pNext->turbo;
                ss << "}";
            } break;
            case ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC_NPU_EXT_2: {
                const auto *pNext =
                    reinterpret_cast<const ze_command_queue_desc_npu_ext_2_t *>(desc->pNext);
                ss << ", ext2 desc {";
                ss << "stype: " << pNext->stype;
                ss << ", pNext: " << pNext->pNext;
                ss << ", options: " << pNext->options;
                ss << "}";
            } break;
            default:
                break;
            }
        }
    }
    if (phCommandQueue == nullptr) {
        ss << ", phCommandQueue: nullptr";
    } else {
        ss << ", phCommandQueue: " << *phCommandQueue;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandQueueCreate(ze_context_handle_t hContext,
                                       ze_device_handle_t hDevice,
                                       const ze_command_queue_desc_t *desc,
                                       ze_command_queue_handle_t *phCommandQueue) {
    TRACE_EVENT_BEGIN("API", "zeCommandQueueCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandQueueCreate(hContext, hDevice, desc, phCommandQueue) + "..\n";
}
inline void trace_zeCommandQueueCreate(ze_result_t ret,
                                       ze_context_handle_t hContext,
                                       ze_device_handle_t hDevice,
                                       const ze_command_queue_desc_t *desc,
                                       ze_command_queue_handle_t *phCommandQueue) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandQueueCreate(hContext, hDevice, desc, phCommandQueue) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandQueueDestroy(ze_command_queue_handle_t hCommandQueue) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandQueueDestroy(";
    ss << std::hex << std::showbase;
    ss << "hCommandQueue: " << hCommandQueue;
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandQueueDestroy(ze_command_queue_handle_t hCommandQueue) {
    TRACE_EVENT_BEGIN("API", "zeCommandQueueDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandQueueDestroy(hCommandQueue) + "..\n";
}
inline void trace_zeCommandQueueDestroy(ze_result_t ret, ze_command_queue_handle_t hCommandQueue) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandQueueDestroy(hCommandQueue) + trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandQueueExecuteCommandLists(ze_command_queue_handle_t hCommandQueue,
                                         uint32_t numCommandLists,
                                         ze_command_list_handle_t *phCommandLists,
                                         ze_fence_handle_t hFence) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandQueueExecuteCommandLists(";
    ss << std::hex << std::showbase;
    ss << "hCommandQueue: " << hCommandQueue;
    ss << ", numCommandLists: " << numCommandLists;
    if (phCommandLists == nullptr) {
        ss << ", phCommandLists: nullptr";
    } else {
        ss << ", phCommandLists: " << *phCommandLists;
    }
    ss << ", hFence: " << hFence;
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandQueueExecuteCommandLists(ze_command_queue_handle_t hCommandQueue,
                                                    uint32_t numCommandLists,
                                                    ze_command_list_handle_t *phCommandLists,
                                                    ze_fence_handle_t hFence) {
    if (hFence)
        TRACE_EVENT_BEGIN("API",
                          "zeCommandQueueExecuteCommandLists",
                          perfetto::Flow::ProcessScoped((uint64_t)hFence));
    else
        TRACE_EVENT_BEGIN("API", "zeCommandQueueExecuteCommandLists");

    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandQueueExecuteCommandLists(hCommandQueue,
                                                              numCommandLists,
                                                              phCommandLists,
                                                              hFence) +
                         "..\n";
}
inline void trace_zeCommandQueueExecuteCommandLists(ze_result_t ret,
                                                    ze_command_queue_handle_t hCommandQueue,
                                                    uint32_t numCommandLists,
                                                    ze_command_list_handle_t *phCommandLists,
                                                    ze_fence_handle_t hFence) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandQueueExecuteCommandLists(hCommandQueue,
                                                              numCommandLists,
                                                              phCommandLists,
                                                              hFence) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandQueueSynchronize(ze_command_queue_handle_t hCommandQueue,
                                                    uint64_t timeout) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandQueueSynchronize(";
    ss << std::hex << std::showbase;
    ss << "hCommandQueue: " << hCommandQueue;
    ss << ", timeout: " << timeout;
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandQueueSynchronize(ze_command_queue_handle_t hCommandQueue,
                                            uint64_t timeout) {
    TRACE_EVENT_BEGIN("API", "zeCommandQueueSynchronize");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandQueueSynchronize(hCommandQueue, timeout) + "..\n";
}
inline void trace_zeCommandQueueSynchronize(ze_result_t ret,
                                            ze_command_queue_handle_t hCommandQueue,
                                            uint64_t timeout) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandQueueSynchronize(hCommandQueue, timeout) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandQueueGetOrdinal(ze_command_queue_handle_t hCommandQueue,
                                                   uint32_t *pOrdinal) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandQueueGetOrdinal(";
    ss << std::hex << std::showbase;
    ss << "hCommandQueue: " << hCommandQueue;
    if (pOrdinal == nullptr) {
        ss << ", pOrdinal: nullptr";
    } else {
        ss << ", pOrdinal: " << *pOrdinal;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandQueueGetOrdinal(ze_command_queue_handle_t hCommandQueue,
                                           uint32_t *pOrdinal) {
    TRACE_EVENT_BEGIN("API", "zeCommandQueueGetOrdinal");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandQueueGetOrdinal(hCommandQueue, pOrdinal) + "..\n";
}
inline void trace_zeCommandQueueGetOrdinal(ze_result_t ret,
                                           ze_command_queue_handle_t hCommandQueue,
                                           uint32_t *pOrdinal) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandQueueGetOrdinal(hCommandQueue, pOrdinal) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandQueueGetIndex(ze_command_queue_handle_t hCommandQueue,
                                                 uint32_t *pIndex) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandQueueGetIndex(";
    ss << std::hex << std::showbase;
    ss << "hCommandQueue: " << hCommandQueue;
    if (pIndex == nullptr) {
        ss << ", pIndex: nullptr";
    } else {
        ss << ", pIndex: " << *pIndex;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandQueueGetIndex(ze_command_queue_handle_t hCommandQueue,
                                         uint32_t *pIndex) {
    TRACE_EVENT_BEGIN("API", "zeCommandQueueGetIndex");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandQueueGetIndex(hCommandQueue, pIndex) + "..\n";
}
inline void trace_zeCommandQueueGetIndex(ze_result_t ret,
                                         ze_command_queue_handle_t hCommandQueue,
                                         uint32_t *pIndex) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandQueueGetIndex(hCommandQueue, pIndex) + trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListCreate(ze_context_handle_t hContext,
                                              ze_device_handle_t hDevice,
                                              const ze_command_list_desc_t *desc,
                                              ze_command_list_handle_t *phCommandList) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListCreate(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", commandQueueGroupOrdinal: " << desc->commandQueueGroupOrdinal;
        ss << ", flags: " << desc->flags;
        ss << "}";
    }
    if (phCommandList == nullptr) {
        ss << ", phCommandList: nullptr";
    } else {
        ss << ", phCommandList: " << *phCommandList;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListCreate(ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      const ze_command_list_desc_t *desc,
                                      ze_command_list_handle_t *phCommandList) {
    TRACE_EVENT_BEGIN("API", "zeCommandListCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListCreate(hContext, hDevice, desc, phCommandList) + "..\n";
}
inline void trace_zeCommandListCreate(ze_result_t ret,
                                      ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      const ze_command_list_desc_t *desc,
                                      ze_command_list_handle_t *phCommandList) {
    if (phCommandList)
        TRACE_EVENT_END("API", perfetto::Flow::ProcessScoped((uint64_t)*phCommandList));
    else
        TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListCreate(hContext, hDevice, desc, phCommandList) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListCreateImmediate(ze_context_handle_t hContext,
                                                       ze_device_handle_t hDevice,
                                                       const ze_command_queue_desc_t *altdesc,
                                                       ze_command_list_handle_t *phCommandList) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListCreateImmediate(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (altdesc == nullptr) {
        ss << ", altdesc: nullptr";
    } else {
        ss << ", altdesc {";
        ss << "stype: " << altdesc->stype;
        ss << ", pNext: " << altdesc->pNext;
        ss << ", ordinal: " << altdesc->ordinal;
        ss << ", index: " << altdesc->index;
        ss << ", flags: " << altdesc->flags;
        ss << ", mode: " << altdesc->mode;
        ss << ", priority: " << altdesc->priority;
        ss << "}";
    }
    if (phCommandList == nullptr) {
        ss << ", phCommandList: nullptr";
    } else {
        ss << ", phCommandList: " << *phCommandList;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListCreateImmediate(ze_context_handle_t hContext,
                                               ze_device_handle_t hDevice,
                                               const ze_command_queue_desc_t *altdesc,
                                               ze_command_list_handle_t *phCommandList) {
    TRACE_EVENT_BEGIN("API", "zeCommandListCreateImmediate");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListCreateImmediate(hContext,
                                                         hDevice,
                                                         altdesc,
                                                         phCommandList) +
                         "..\n";
}
inline void trace_zeCommandListCreateImmediate(ze_result_t ret,
                                               ze_context_handle_t hContext,
                                               ze_device_handle_t hDevice,
                                               const ze_command_queue_desc_t *altdesc,
                                               ze_command_list_handle_t *phCommandList) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListCreateImmediate(hContext,
                                                         hDevice,
                                                         altdesc,
                                                         phCommandList) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListDestroy(ze_command_list_handle_t hCommandList) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListDestroy(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListDestroy(ze_command_list_handle_t hCommandList) {
    TRACE_EVENT_BEGIN("API",
                      "zeCommandListDestroy",
                      perfetto::TerminatingFlow::ProcessScoped((uint64_t)hCommandList));
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListDestroy(hCommandList) + "..\n";
}
inline void trace_zeCommandListDestroy(ze_result_t ret, ze_command_list_handle_t hCommandList) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListDestroy(hCommandList) + trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListClose(ze_command_list_handle_t hCommandList) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListClose(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListClose(ze_command_list_handle_t hCommandList) {
    TRACE_EVENT_BEGIN("API",
                      "zeCommandListClose",
                      perfetto::Flow::ProcessScoped((uint64_t)hCommandList));
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListClose(hCommandList) + "..\n";
}
inline void trace_zeCommandListClose(ze_result_t ret, ze_command_list_handle_t hCommandList) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListClose(hCommandList) + trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListReset(ze_command_list_handle_t hCommandList) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListReset(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListReset(ze_command_list_handle_t hCommandList) {
    TRACE_EVENT_BEGIN("API",
                      "zeCommandListReset",
                      perfetto::Flow::ProcessScoped((uint64_t)hCommandList));
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListReset(hCommandList) + "..\n";
}
inline void trace_zeCommandListReset(ze_result_t ret, ze_command_list_handle_t hCommandList) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListReset(hCommandList) + trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListAppendWriteGlobalTimestamp(ze_command_list_handle_t hCommandList,
                                               uint64_t *dstptr,
                                               ze_event_handle_t hSignalEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendWriteGlobalTimestamp(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    if (dstptr == nullptr) {
        ss << ", dstptr: nullptr";
    } else {
        ss << ", dstptr: " << *dstptr;
    }
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendWriteGlobalTimestamp(ze_command_list_handle_t hCommandList,
                                                          uint64_t *dstptr,
                                                          ze_event_handle_t hSignalEvent,
                                                          uint32_t numWaitEvents,
                                                          ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendWriteGlobalTimestamp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendWriteGlobalTimestamp(hCommandList,
                                                                    dstptr,
                                                                    hSignalEvent,
                                                                    numWaitEvents,
                                                                    phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendWriteGlobalTimestamp(ze_result_t ret,
                                                          ze_command_list_handle_t hCommandList,
                                                          uint64_t *dstptr,
                                                          ze_event_handle_t hSignalEvent,
                                                          uint32_t numWaitEvents,
                                                          ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendWriteGlobalTimestamp(hCommandList,
                                                                    dstptr,
                                                                    hSignalEvent,
                                                                    numWaitEvents,
                                                                    phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListHostSynchronize(ze_command_list_handle_t hCommandList,
                                                       uint64_t timeout) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListHostSynchronize(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", timeout: " << timeout;
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListHostSynchronize(ze_command_list_handle_t hCommandList,
                                               uint64_t timeout) {
    TRACE_EVENT_BEGIN("API", "zeCommandListHostSynchronize");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListHostSynchronize(hCommandList, timeout) + "..\n";
}
inline void trace_zeCommandListHostSynchronize(ze_result_t ret,
                                               ze_command_list_handle_t hCommandList,
                                               uint64_t timeout) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListHostSynchronize(hCommandList, timeout) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListGetDeviceHandle(ze_command_list_handle_t hCommandList,
                                                       ze_device_handle_t *phDevice) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListGetDeviceHandle(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    if (phDevice == nullptr) {
        ss << ", phDevice: nullptr";
    } else {
        ss << ", phDevice: " << *phDevice;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListGetDeviceHandle(ze_command_list_handle_t hCommandList,
                                               ze_device_handle_t *phDevice) {
    TRACE_EVENT_BEGIN("API", "zeCommandListGetDeviceHandle");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListGetDeviceHandle(hCommandList, phDevice) + "..\n";
}
inline void trace_zeCommandListGetDeviceHandle(ze_result_t ret,
                                               ze_command_list_handle_t hCommandList,
                                               ze_device_handle_t *phDevice) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListGetDeviceHandle(hCommandList, phDevice) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListGetContextHandle(ze_command_list_handle_t hCommandList,
                                                        ze_context_handle_t *phContext) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListGetContextHandle(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    if (phContext == nullptr) {
        ss << ", phContext: nullptr";
    } else {
        ss << ", phContext: " << *phContext;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListGetContextHandle(ze_command_list_handle_t hCommandList,
                                                ze_context_handle_t *phContext) {
    TRACE_EVENT_BEGIN("API", "zeCommandListGetContextHandle");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListGetContextHandle(hCommandList, phContext) + "..\n";
}
inline void trace_zeCommandListGetContextHandle(ze_result_t ret,
                                                ze_command_list_handle_t hCommandList,
                                                ze_context_handle_t *phContext) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListGetContextHandle(hCommandList, phContext) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListGetOrdinal(ze_command_list_handle_t hCommandList,
                                                  uint32_t *pOrdinal) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListGetOrdinal(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    if (pOrdinal == nullptr) {
        ss << ", pOrdinal: nullptr";
    } else {
        ss << ", pOrdinal: " << *pOrdinal;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListGetOrdinal(ze_command_list_handle_t hCommandList,
                                          uint32_t *pOrdinal) {
    TRACE_EVENT_BEGIN("API", "zeCommandListGetOrdinal");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListGetOrdinal(hCommandList, pOrdinal) + "..\n";
}
inline void trace_zeCommandListGetOrdinal(ze_result_t ret,
                                          ze_command_list_handle_t hCommandList,
                                          uint32_t *pOrdinal) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListGetOrdinal(hCommandList, pOrdinal) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListImmediateGetIndex(ze_command_list_handle_t hCommandListImmediate,
                                      uint32_t *pIndex) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListImmediateGetIndex(";
    ss << std::hex << std::showbase;
    ss << "hCommandListImmediate: " << hCommandListImmediate;
    if (pIndex == nullptr) {
        ss << ", pIndex: nullptr";
    } else {
        ss << ", pIndex: " << *pIndex;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListImmediateGetIndex(ze_command_list_handle_t hCommandListImmediate,
                                                 uint32_t *pIndex) {
    TRACE_EVENT_BEGIN("API", "zeCommandListImmediateGetIndex");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListImmediateGetIndex(hCommandListImmediate, pIndex) + "..\n";
}
inline void trace_zeCommandListImmediateGetIndex(ze_result_t ret,
                                                 ze_command_list_handle_t hCommandListImmediate,
                                                 uint32_t *pIndex) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListImmediateGetIndex(hCommandListImmediate, pIndex) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListIsImmediate(ze_command_list_handle_t hCommandList,
                                                   ze_bool_t *pIsImmediate) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListIsImmediate(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    if (pIsImmediate == nullptr) {
        ss << ", pIsImmediate: nullptr";
    } else {
        ss << ", pIsImmediate: " << *pIsImmediate;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListIsImmediate(ze_command_list_handle_t hCommandList,
                                           ze_bool_t *pIsImmediate) {
    TRACE_EVENT_BEGIN("API", "zeCommandListIsImmediate");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListIsImmediate(hCommandList, pIsImmediate) + "..\n";
}
inline void trace_zeCommandListIsImmediate(ze_result_t ret,
                                           ze_command_list_handle_t hCommandList,
                                           ze_bool_t *pIsImmediate) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListIsImmediate(hCommandList, pIsImmediate) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendBarrier(ze_command_list_handle_t hCommandList,
                                                     ze_event_handle_t hSignalEvent,
                                                     uint32_t numWaitEvents,
                                                     ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendBarrier(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendBarrier(ze_command_list_handle_t hCommandList,
                                             ze_event_handle_t hSignalEvent,
                                             uint32_t numWaitEvents,
                                             ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendBarrier");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendBarrier(hCommandList,
                                                       hSignalEvent,
                                                       numWaitEvents,
                                                       phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendBarrier(ze_result_t ret,
                                             ze_command_list_handle_t hCommandList,
                                             ze_event_handle_t hSignalEvent,
                                             uint32_t numWaitEvents,
                                             ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendBarrier(hCommandList,
                                                       hSignalEvent,
                                                       numWaitEvents,
                                                       phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListAppendMemoryRangesBarrier(ze_command_list_handle_t hCommandList,
                                              uint32_t numRanges,
                                              const size_t *pRangeSizes,
                                              const void **pRanges,
                                              ze_event_handle_t hSignalEvent,
                                              uint32_t numWaitEvents,
                                              ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendMemoryRangesBarrier(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", numRanges: " << numRanges;
    if (pRangeSizes == nullptr) {
        ss << ", pRangeSizes: nullptr";
    } else {
        ss << ", pRangeSizes: " << *pRangeSizes;
    }
    ss << ", pRanges: " << pRanges;
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendMemoryRangesBarrier(ze_command_list_handle_t hCommandList,
                                                         uint32_t numRanges,
                                                         const size_t *pRangeSizes,
                                                         const void **pRanges,
                                                         ze_event_handle_t hSignalEvent,
                                                         uint32_t numWaitEvents,
                                                         ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendMemoryRangesBarrier");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemoryRangesBarrier(hCommandList,
                                                                   numRanges,
                                                                   pRangeSizes,
                                                                   pRanges,
                                                                   hSignalEvent,
                                                                   numWaitEvents,
                                                                   phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendMemoryRangesBarrier(ze_result_t ret,
                                                         ze_command_list_handle_t hCommandList,
                                                         uint32_t numRanges,
                                                         const size_t *pRangeSizes,
                                                         const void **pRanges,
                                                         ze_event_handle_t hSignalEvent,
                                                         uint32_t numWaitEvents,
                                                         ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemoryRangesBarrier(hCommandList,
                                                                   numRanges,
                                                                   pRangeSizes,
                                                                   pRanges,
                                                                   hSignalEvent,
                                                                   numWaitEvents,
                                                                   phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeContextSystemBarrier(ze_context_handle_t hContext,
                                                 ze_device_handle_t hDevice) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeContextSystemBarrier(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ")";
    return ss.str();
}
inline void trace_zeContextSystemBarrier(ze_context_handle_t hContext, ze_device_handle_t hDevice) {
    TRACE_EVENT_BEGIN("API", "zeContextSystemBarrier");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextSystemBarrier(hContext, hDevice) + "..\n";
}
inline void trace_zeContextSystemBarrier(ze_result_t ret,
                                         ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextSystemBarrier(hContext, hDevice) + trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendMemoryCopy(ze_command_list_handle_t hCommandList,
                                                        void *dstptr,
                                                        const void *srcptr,
                                                        size_t size,
                                                        ze_event_handle_t hSignalEvent,
                                                        uint32_t numWaitEvents,
                                                        ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendMemoryCopy(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", dstptr: " << dstptr;
    ss << ", srcptr: " << srcptr;
    ss << ", size: " << size;
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendMemoryCopy(ze_command_list_handle_t hCommandList,
                                                void *dstptr,
                                                const void *srcptr,
                                                size_t size,
                                                ze_event_handle_t hSignalEvent,
                                                uint32_t numWaitEvents,
                                                ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendMemoryCopy");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemoryCopy(hCommandList,
                                                          dstptr,
                                                          srcptr,
                                                          size,
                                                          hSignalEvent,
                                                          numWaitEvents,
                                                          phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendMemoryCopy(ze_result_t ret,
                                                ze_command_list_handle_t hCommandList,
                                                void *dstptr,
                                                const void *srcptr,
                                                size_t size,
                                                ze_event_handle_t hSignalEvent,
                                                uint32_t numWaitEvents,
                                                ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemoryCopy(hCommandList,
                                                          dstptr,
                                                          srcptr,
                                                          size,
                                                          hSignalEvent,
                                                          numWaitEvents,
                                                          phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendMemoryFill(ze_command_list_handle_t hCommandList,
                                                        void *ptr,
                                                        const void *pattern,
                                                        size_t pattern_size,
                                                        size_t size,
                                                        ze_event_handle_t hSignalEvent,
                                                        uint32_t numWaitEvents,
                                                        ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendMemoryFill(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", ptr: " << ptr;
    ss << ", pattern: " << pattern;
    ss << ", pattern_size: " << pattern_size;
    ss << ", size: " << size;
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendMemoryFill(ze_command_list_handle_t hCommandList,
                                                void *ptr,
                                                const void *pattern,
                                                size_t pattern_size,
                                                size_t size,
                                                ze_event_handle_t hSignalEvent,
                                                uint32_t numWaitEvents,
                                                ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendMemoryFill");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemoryFill(hCommandList,
                                                          ptr,
                                                          pattern,
                                                          pattern_size,
                                                          size,
                                                          hSignalEvent,
                                                          numWaitEvents,
                                                          phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendMemoryFill(ze_result_t ret,
                                                ze_command_list_handle_t hCommandList,
                                                void *ptr,
                                                const void *pattern,
                                                size_t pattern_size,
                                                size_t size,
                                                ze_event_handle_t hSignalEvent,
                                                uint32_t numWaitEvents,
                                                ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemoryFill(hCommandList,
                                                          ptr,
                                                          pattern,
                                                          pattern_size,
                                                          size,
                                                          hSignalEvent,
                                                          numWaitEvents,
                                                          phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendMemoryCopyRegion(ze_command_list_handle_t hCommandList,
                                                              void *dstptr,
                                                              const ze_copy_region_t *dstRegion,
                                                              uint32_t dstPitch,
                                                              uint32_t dstSlicePitch,
                                                              const void *srcptr,
                                                              const ze_copy_region_t *srcRegion,
                                                              uint32_t srcPitch,
                                                              uint32_t srcSlicePitch,
                                                              ze_event_handle_t hSignalEvent,
                                                              uint32_t numWaitEvents,
                                                              ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendMemoryCopyRegion(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", dstptr: " << dstptr;
    if (dstRegion == nullptr) {
        ss << ", dstRegion: nullptr";
    } else {
        ss << ", dstRegion {";
        ss << "originX: " << dstRegion->originX;
        ss << ", originY: " << dstRegion->originY;
        ss << ", originZ: " << dstRegion->originZ;
        ss << ", width: " << dstRegion->width;
        ss << ", height: " << dstRegion->height;
        ss << ", depth: " << dstRegion->depth;
        ss << "}";
    }
    ss << ", dstPitch: " << dstPitch;
    ss << ", dstSlicePitch: " << dstSlicePitch;
    ss << ", srcptr: " << srcptr;
    if (srcRegion == nullptr) {
        ss << ", srcRegion: nullptr";
    } else {
        ss << ", srcRegion {";
        ss << "originX: " << srcRegion->originX;
        ss << ", originY: " << srcRegion->originY;
        ss << ", originZ: " << srcRegion->originZ;
        ss << ", width: " << srcRegion->width;
        ss << ", height: " << srcRegion->height;
        ss << ", depth: " << srcRegion->depth;
        ss << "}";
    }
    ss << ", srcPitch: " << srcPitch;
    ss << ", srcSlicePitch: " << srcSlicePitch;
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendMemoryCopyRegion(ze_command_list_handle_t hCommandList,
                                                      void *dstptr,
                                                      const ze_copy_region_t *dstRegion,
                                                      uint32_t dstPitch,
                                                      uint32_t dstSlicePitch,
                                                      const void *srcptr,
                                                      const ze_copy_region_t *srcRegion,
                                                      uint32_t srcPitch,
                                                      uint32_t srcSlicePitch,
                                                      ze_event_handle_t hSignalEvent,
                                                      uint32_t numWaitEvents,
                                                      ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendMemoryCopyRegion");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemoryCopyRegion(hCommandList,
                                                                dstptr,
                                                                dstRegion,
                                                                dstPitch,
                                                                dstSlicePitch,
                                                                srcptr,
                                                                srcRegion,
                                                                srcPitch,
                                                                srcSlicePitch,
                                                                hSignalEvent,
                                                                numWaitEvents,
                                                                phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendMemoryCopyRegion(ze_result_t ret,
                                                      ze_command_list_handle_t hCommandList,
                                                      void *dstptr,
                                                      const ze_copy_region_t *dstRegion,
                                                      uint32_t dstPitch,
                                                      uint32_t dstSlicePitch,
                                                      const void *srcptr,
                                                      const ze_copy_region_t *srcRegion,
                                                      uint32_t srcPitch,
                                                      uint32_t srcSlicePitch,
                                                      ze_event_handle_t hSignalEvent,
                                                      uint32_t numWaitEvents,
                                                      ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemoryCopyRegion(hCommandList,
                                                                dstptr,
                                                                dstRegion,
                                                                dstPitch,
                                                                dstSlicePitch,
                                                                srcptr,
                                                                srcRegion,
                                                                srcPitch,
                                                                srcSlicePitch,
                                                                hSignalEvent,
                                                                numWaitEvents,
                                                                phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListAppendMemoryCopyFromContext(ze_command_list_handle_t hCommandList,
                                                void *dstptr,
                                                ze_context_handle_t hContextSrc,
                                                const void *srcptr,
                                                size_t size,
                                                ze_event_handle_t hSignalEvent,
                                                uint32_t numWaitEvents,
                                                ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendMemoryCopyFromContext(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", dstptr: " << dstptr;
    ss << ", hContextSrc: " << hContextSrc;
    ss << ", srcptr: " << srcptr;
    ss << ", size: " << size;
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendMemoryCopyFromContext(ze_command_list_handle_t hCommandList,
                                                           void *dstptr,
                                                           ze_context_handle_t hContextSrc,
                                                           const void *srcptr,
                                                           size_t size,
                                                           ze_event_handle_t hSignalEvent,
                                                           uint32_t numWaitEvents,
                                                           ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendMemoryCopyFromContext");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemoryCopyFromContext(hCommandList,
                                                                     dstptr,
                                                                     hContextSrc,
                                                                     srcptr,
                                                                     size,
                                                                     hSignalEvent,
                                                                     numWaitEvents,
                                                                     phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendMemoryCopyFromContext(ze_result_t ret,
                                                           ze_command_list_handle_t hCommandList,
                                                           void *dstptr,
                                                           ze_context_handle_t hContextSrc,
                                                           const void *srcptr,
                                                           size_t size,
                                                           ze_event_handle_t hSignalEvent,
                                                           uint32_t numWaitEvents,
                                                           ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemoryCopyFromContext(hCommandList,
                                                                     dstptr,
                                                                     hContextSrc,
                                                                     srcptr,
                                                                     size,
                                                                     hSignalEvent,
                                                                     numWaitEvents,
                                                                     phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendImageCopy(ze_command_list_handle_t hCommandList,
                                                       ze_image_handle_t hDstImage,
                                                       ze_image_handle_t hSrcImage,
                                                       ze_event_handle_t hSignalEvent,
                                                       uint32_t numWaitEvents,
                                                       ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendImageCopy(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", hDstImage: " << hDstImage;
    ss << ", hSrcImage: " << hSrcImage;
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendImageCopy(ze_command_list_handle_t hCommandList,
                                               ze_image_handle_t hDstImage,
                                               ze_image_handle_t hSrcImage,
                                               ze_event_handle_t hSignalEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendImageCopy");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendImageCopy(hCommandList,
                                                         hDstImage,
                                                         hSrcImage,
                                                         hSignalEvent,
                                                         numWaitEvents,
                                                         phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendImageCopy(ze_result_t ret,
                                               ze_command_list_handle_t hCommandList,
                                               ze_image_handle_t hDstImage,
                                               ze_image_handle_t hSrcImage,
                                               ze_event_handle_t hSignalEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendImageCopy(hCommandList,
                                                         hDstImage,
                                                         hSrcImage,
                                                         hSignalEvent,
                                                         numWaitEvents,
                                                         phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendImageCopyRegion(ze_command_list_handle_t hCommandList,
                                                             ze_image_handle_t hDstImage,
                                                             ze_image_handle_t hSrcImage,
                                                             const ze_image_region_t *pDstRegion,
                                                             const ze_image_region_t *pSrcRegion,
                                                             ze_event_handle_t hSignalEvent,
                                                             uint32_t numWaitEvents,
                                                             ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendImageCopyRegion(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", hDstImage: " << hDstImage;
    ss << ", hSrcImage: " << hSrcImage;
    if (pDstRegion == nullptr) {
        ss << ", pDstRegion: nullptr";
    } else {
        ss << ", pDstRegion {";
        ss << "originX: " << pDstRegion->originX;
        ss << ", originY: " << pDstRegion->originY;
        ss << ", originZ: " << pDstRegion->originZ;
        ss << ", width: " << pDstRegion->width;
        ss << ", height: " << pDstRegion->height;
        ss << ", depth: " << pDstRegion->depth;
        ss << "}";
    }
    if (pSrcRegion == nullptr) {
        ss << ", pSrcRegion: nullptr";
    } else {
        ss << ", pSrcRegion {";
        ss << "originX: " << pSrcRegion->originX;
        ss << ", originY: " << pSrcRegion->originY;
        ss << ", originZ: " << pSrcRegion->originZ;
        ss << ", width: " << pSrcRegion->width;
        ss << ", height: " << pSrcRegion->height;
        ss << ", depth: " << pSrcRegion->depth;
        ss << "}";
    }
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendImageCopyRegion(ze_command_list_handle_t hCommandList,
                                                     ze_image_handle_t hDstImage,
                                                     ze_image_handle_t hSrcImage,
                                                     const ze_image_region_t *pDstRegion,
                                                     const ze_image_region_t *pSrcRegion,
                                                     ze_event_handle_t hSignalEvent,
                                                     uint32_t numWaitEvents,
                                                     ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendImageCopyRegion");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendImageCopyRegion(hCommandList,
                                                               hDstImage,
                                                               hSrcImage,
                                                               pDstRegion,
                                                               pSrcRegion,
                                                               hSignalEvent,
                                                               numWaitEvents,
                                                               phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendImageCopyRegion(ze_result_t ret,
                                                     ze_command_list_handle_t hCommandList,
                                                     ze_image_handle_t hDstImage,
                                                     ze_image_handle_t hSrcImage,
                                                     const ze_image_region_t *pDstRegion,
                                                     const ze_image_region_t *pSrcRegion,
                                                     ze_event_handle_t hSignalEvent,
                                                     uint32_t numWaitEvents,
                                                     ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendImageCopyRegion(hCommandList,
                                                               hDstImage,
                                                               hSrcImage,
                                                               pDstRegion,
                                                               pSrcRegion,
                                                               hSignalEvent,
                                                               numWaitEvents,
                                                               phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListAppendImageCopyToMemory(ze_command_list_handle_t hCommandList,
                                            void *dstptr,
                                            ze_image_handle_t hSrcImage,
                                            const ze_image_region_t *pSrcRegion,
                                            ze_event_handle_t hSignalEvent,
                                            uint32_t numWaitEvents,
                                            ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendImageCopyToMemory(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", dstptr: " << dstptr;
    ss << ", hSrcImage: " << hSrcImage;
    if (pSrcRegion == nullptr) {
        ss << ", pSrcRegion: nullptr";
    } else {
        ss << ", pSrcRegion {";
        ss << "originX: " << pSrcRegion->originX;
        ss << ", originY: " << pSrcRegion->originY;
        ss << ", originZ: " << pSrcRegion->originZ;
        ss << ", width: " << pSrcRegion->width;
        ss << ", height: " << pSrcRegion->height;
        ss << ", depth: " << pSrcRegion->depth;
        ss << "}";
    }
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendImageCopyToMemory(ze_command_list_handle_t hCommandList,
                                                       void *dstptr,
                                                       ze_image_handle_t hSrcImage,
                                                       const ze_image_region_t *pSrcRegion,
                                                       ze_event_handle_t hSignalEvent,
                                                       uint32_t numWaitEvents,
                                                       ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendImageCopyToMemory");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendImageCopyToMemory(hCommandList,
                                                                 dstptr,
                                                                 hSrcImage,
                                                                 pSrcRegion,
                                                                 hSignalEvent,
                                                                 numWaitEvents,
                                                                 phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendImageCopyToMemory(ze_result_t ret,
                                                       ze_command_list_handle_t hCommandList,
                                                       void *dstptr,
                                                       ze_image_handle_t hSrcImage,
                                                       const ze_image_region_t *pSrcRegion,
                                                       ze_event_handle_t hSignalEvent,
                                                       uint32_t numWaitEvents,
                                                       ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendImageCopyToMemory(hCommandList,
                                                                 dstptr,
                                                                 hSrcImage,
                                                                 pSrcRegion,
                                                                 hSignalEvent,
                                                                 numWaitEvents,
                                                                 phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListAppendImageCopyFromMemory(ze_command_list_handle_t hCommandList,
                                              ze_image_handle_t hDstImage,
                                              const void *srcptr,
                                              const ze_image_region_t *pDstRegion,
                                              ze_event_handle_t hSignalEvent,
                                              uint32_t numWaitEvents,
                                              ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendImageCopyFromMemory(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", hDstImage: " << hDstImage;
    ss << ", srcptr: " << srcptr;
    if (pDstRegion == nullptr) {
        ss << ", pDstRegion: nullptr";
    } else {
        ss << ", pDstRegion {";
        ss << "originX: " << pDstRegion->originX;
        ss << ", originY: " << pDstRegion->originY;
        ss << ", originZ: " << pDstRegion->originZ;
        ss << ", width: " << pDstRegion->width;
        ss << ", height: " << pDstRegion->height;
        ss << ", depth: " << pDstRegion->depth;
        ss << "}";
    }
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendImageCopyFromMemory(ze_command_list_handle_t hCommandList,
                                                         ze_image_handle_t hDstImage,
                                                         const void *srcptr,
                                                         const ze_image_region_t *pDstRegion,
                                                         ze_event_handle_t hSignalEvent,
                                                         uint32_t numWaitEvents,
                                                         ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendImageCopyFromMemory");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendImageCopyFromMemory(hCommandList,
                                                                   hDstImage,
                                                                   srcptr,
                                                                   pDstRegion,
                                                                   hSignalEvent,
                                                                   numWaitEvents,
                                                                   phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendImageCopyFromMemory(ze_result_t ret,
                                                         ze_command_list_handle_t hCommandList,
                                                         ze_image_handle_t hDstImage,
                                                         const void *srcptr,
                                                         const ze_image_region_t *pDstRegion,
                                                         ze_event_handle_t hSignalEvent,
                                                         uint32_t numWaitEvents,
                                                         ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendImageCopyFromMemory(hCommandList,
                                                                   hDstImage,
                                                                   srcptr,
                                                                   pDstRegion,
                                                                   hSignalEvent,
                                                                   numWaitEvents,
                                                                   phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendMemoryPrefetch(ze_command_list_handle_t hCommandList,
                                                            const void *ptr,
                                                            size_t size) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendMemoryPrefetch(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", ptr: " << ptr;
    ss << ", size: " << size;
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendMemoryPrefetch(ze_command_list_handle_t hCommandList,
                                                    const void *ptr,
                                                    size_t size) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendMemoryPrefetch");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemoryPrefetch(hCommandList, ptr, size) + "..\n";
}
inline void trace_zeCommandListAppendMemoryPrefetch(ze_result_t ret,
                                                    ze_command_list_handle_t hCommandList,
                                                    const void *ptr,
                                                    size_t size) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemoryPrefetch(hCommandList, ptr, size) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendMemAdvise(ze_command_list_handle_t hCommandList,
                                                       ze_device_handle_t hDevice,
                                                       const void *ptr,
                                                       size_t size,
                                                       ze_memory_advice_t advice) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendMemAdvise(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", hDevice: " << hDevice;
    ss << ", ptr: " << ptr;
    ss << ", size: " << size;
    ss << ", advice: " << advice;
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendMemAdvise(ze_command_list_handle_t hCommandList,
                                               ze_device_handle_t hDevice,
                                               const void *ptr,
                                               size_t size,
                                               ze_memory_advice_t advice) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendMemAdvise");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemAdvise(hCommandList, hDevice, ptr, size, advice) +
                         "..\n";
}
inline void trace_zeCommandListAppendMemAdvise(ze_result_t ret,
                                               ze_command_list_handle_t hCommandList,
                                               ze_device_handle_t hDevice,
                                               const void *ptr,
                                               size_t size,
                                               ze_memory_advice_t advice) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendMemAdvise(hCommandList, hDevice, ptr, size, advice) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeEventPoolCreate(ze_context_handle_t hContext,
                                            const ze_event_pool_desc_t *desc,
                                            uint32_t numDevices,
                                            ze_device_handle_t *phDevices,
                                            ze_event_pool_handle_t *phEventPool) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventPoolCreate(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << ", count: " << desc->count;
        ss << "}";
    }
    ss << ", numDevices: " << numDevices;
    if (phDevices == nullptr) {
        ss << ", phDevices: nullptr";
    } else {
        ss << ", phDevices: " << *phDevices;
    }
    if (phEventPool == nullptr) {
        ss << ", phEventPool: nullptr";
    } else {
        ss << ", phEventPool: " << *phEventPool;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeEventPoolCreate(ze_context_handle_t hContext,
                                    const ze_event_pool_desc_t *desc,
                                    uint32_t numDevices,
                                    ze_device_handle_t *phDevices,
                                    ze_event_pool_handle_t *phEventPool) {
    TRACE_EVENT_BEGIN("API", "zeEventPoolCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolCreate(hContext, desc, numDevices, phDevices, phEventPool) +
                         "..\n";
}
inline void trace_zeEventPoolCreate(ze_result_t ret,
                                    ze_context_handle_t hContext,
                                    const ze_event_pool_desc_t *desc,
                                    uint32_t numDevices,
                                    ze_device_handle_t *phDevices,
                                    ze_event_pool_handle_t *phEventPool) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolCreate(hContext, desc, numDevices, phDevices, phEventPool) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeEventPoolDestroy(ze_event_pool_handle_t hEventPool) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventPoolDestroy(";
    ss << std::hex << std::showbase;
    ss << "hEventPool: " << hEventPool;
    ss << ")";
    return ss.str();
}
inline void trace_zeEventPoolDestroy(ze_event_pool_handle_t hEventPool) {
    TRACE_EVENT_BEGIN("API", "zeEventPoolDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolDestroy(hEventPool) + "..\n";
}
inline void trace_zeEventPoolDestroy(ze_result_t ret, ze_event_pool_handle_t hEventPool) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolDestroy(hEventPool) + trace_ze_result_t(ret);
}

inline std::string _trace_zeEventCreate(ze_event_pool_handle_t hEventPool,
                                        const ze_event_desc_t *desc,
                                        ze_event_handle_t *phEvent) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventCreate(";
    ss << std::hex << std::showbase;
    ss << "hEventPool: " << hEventPool;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", index: " << desc->index;
        ss << ", signal: " << desc->signal;
        ss << ", wait: " << desc->wait;
        ss << "}";
    }
    if (phEvent == nullptr) {
        ss << ", phEvent: nullptr";
    } else {
        ss << ", phEvent: " << *phEvent;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeEventCreate(ze_event_pool_handle_t hEventPool,
                                const ze_event_desc_t *desc,
                                ze_event_handle_t *phEvent) {
    TRACE_EVENT_BEGIN("API", "zeEventCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventCreate(hEventPool, desc, phEvent) + "..\n";
}
inline void trace_zeEventCreate(ze_result_t ret,
                                ze_event_pool_handle_t hEventPool,
                                const ze_event_desc_t *desc,
                                ze_event_handle_t *phEvent) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventCreate(hEventPool, desc, phEvent) + trace_ze_result_t(ret);
}

inline std::string _trace_zeEventDestroy(ze_event_handle_t hEvent) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventDestroy(";
    ss << std::hex << std::showbase;
    ss << "hEvent: " << hEvent;
    ss << ")";
    return ss.str();
}
inline void trace_zeEventDestroy(ze_event_handle_t hEvent) {
    TRACE_EVENT_BEGIN("API", "zeEventDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventDestroy(hEvent) + "..\n";
}
inline void trace_zeEventDestroy(ze_result_t ret, ze_event_handle_t hEvent) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventDestroy(hEvent) + trace_ze_result_t(ret);
}

inline std::string _trace_zeEventPoolGetIpcHandle(ze_event_pool_handle_t hEventPool,
                                                  ze_ipc_event_pool_handle_t *phIpc) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventPoolGetIpcHandle(";
    ss << std::hex << std::showbase;
    ss << "hEventPool: " << hEventPool;
    if (phIpc == nullptr) {
        ss << ", phIpc: nullptr";
    } else {
        ss << ", phIpc {";
        ss << "data: ";
        ss << trace_char_array_hex(phIpc->data, sizeof(phIpc->data)).str();
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeEventPoolGetIpcHandle(ze_event_pool_handle_t hEventPool,
                                          ze_ipc_event_pool_handle_t *phIpc) {
    TRACE_EVENT_BEGIN("API", "zeEventPoolGetIpcHandle");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolGetIpcHandle(hEventPool, phIpc) + "..\n";
}
inline void trace_zeEventPoolGetIpcHandle(ze_result_t ret,
                                          ze_event_pool_handle_t hEventPool,
                                          ze_ipc_event_pool_handle_t *phIpc) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolGetIpcHandle(hEventPool, phIpc) + trace_ze_result_t(ret);
}

inline std::string _trace_zeEventPoolPutIpcHandle(ze_context_handle_t hContext,
                                                  ze_ipc_event_pool_handle_t hIpc) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventPoolPutIpcHandle(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hIpc {";
    ss << "data: ";
    ss << trace_char_array_hex(hIpc.data, sizeof(hIpc.data)).str();
    ss << "}";
    ss << ")";
    return ss.str();
}
inline void trace_zeEventPoolPutIpcHandle(ze_context_handle_t hContext,
                                          ze_ipc_event_pool_handle_t hIpc) {
    TRACE_EVENT_BEGIN("API", "zeEventPoolPutIpcHandle");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolPutIpcHandle(hContext, hIpc) + "..\n";
}
inline void trace_zeEventPoolPutIpcHandle(ze_result_t ret,
                                          ze_context_handle_t hContext,
                                          ze_ipc_event_pool_handle_t hIpc) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolPutIpcHandle(hContext, hIpc) + trace_ze_result_t(ret);
}

inline std::string _trace_zeEventPoolOpenIpcHandle(ze_context_handle_t hContext,
                                                   ze_ipc_event_pool_handle_t hIpc,
                                                   ze_event_pool_handle_t *phEventPool) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventPoolOpenIpcHandle(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hIpc {";
    ss << "data: ";
    ss << trace_char_array_hex(hIpc.data, sizeof(hIpc.data)).str();
    ss << "}";
    if (phEventPool == nullptr) {
        ss << ", phEventPool: nullptr";
    } else {
        ss << ", phEventPool: " << *phEventPool;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeEventPoolOpenIpcHandle(ze_context_handle_t hContext,
                                           ze_ipc_event_pool_handle_t hIpc,
                                           ze_event_pool_handle_t *phEventPool) {
    TRACE_EVENT_BEGIN("API", "zeEventPoolOpenIpcHandle");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolOpenIpcHandle(hContext, hIpc, phEventPool) + "..\n";
}
inline void trace_zeEventPoolOpenIpcHandle(ze_result_t ret,
                                           ze_context_handle_t hContext,
                                           ze_ipc_event_pool_handle_t hIpc,
                                           ze_event_pool_handle_t *phEventPool) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolOpenIpcHandle(hContext, hIpc, phEventPool) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeEventPoolCloseIpcHandle(ze_event_pool_handle_t hEventPool) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventPoolCloseIpcHandle(";
    ss << std::hex << std::showbase;
    ss << "hEventPool: " << hEventPool;
    ss << ")";
    return ss.str();
}
inline void trace_zeEventPoolCloseIpcHandle(ze_event_pool_handle_t hEventPool) {
    TRACE_EVENT_BEGIN("API", "zeEventPoolCloseIpcHandle");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolCloseIpcHandle(hEventPool) + "..\n";
}
inline void trace_zeEventPoolCloseIpcHandle(ze_result_t ret, ze_event_pool_handle_t hEventPool) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolCloseIpcHandle(hEventPool) + trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendSignalEvent(ze_command_list_handle_t hCommandList,
                                                         ze_event_handle_t hEvent) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendSignalEvent(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", hEvent: " << hEvent;
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendSignalEvent(ze_command_list_handle_t hCommandList,
                                                 ze_event_handle_t hEvent) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendSignalEvent");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendSignalEvent(hCommandList, hEvent) + "..\n";
}
inline void trace_zeCommandListAppendSignalEvent(ze_result_t ret,
                                                 ze_command_list_handle_t hCommandList,
                                                 ze_event_handle_t hEvent) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendSignalEvent(hCommandList, hEvent) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendWaitOnEvents(ze_command_list_handle_t hCommandList,
                                                          uint32_t numEvents,
                                                          ze_event_handle_t *phEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendWaitOnEvents(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", numEvents: " << numEvents;
    if (phEvents == nullptr) {
        ss << ", phEvents: nullptr";
    } else {
        ss << ", phEvents: " << *phEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendWaitOnEvents(ze_command_list_handle_t hCommandList,
                                                  uint32_t numEvents,
                                                  ze_event_handle_t *phEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendWaitOnEvents");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendWaitOnEvents(hCommandList, numEvents, phEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendWaitOnEvents(ze_result_t ret,
                                                  ze_command_list_handle_t hCommandList,
                                                  uint32_t numEvents,
                                                  ze_event_handle_t *phEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendWaitOnEvents(hCommandList, numEvents, phEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeEventHostSignal(ze_event_handle_t hEvent) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventHostSignal(";
    ss << std::hex << std::showbase;
    ss << "hEvent: " << hEvent;
    ss << ")";
    return ss.str();
}
inline void trace_zeEventHostSignal(ze_event_handle_t hEvent) {
    TRACE_EVENT_BEGIN("API", "zeEventHostSignal");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventHostSignal(hEvent) + "..\n";
}
inline void trace_zeEventHostSignal(ze_result_t ret, ze_event_handle_t hEvent) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventHostSignal(hEvent) + trace_ze_result_t(ret);
}

inline std::string _trace_zeEventHostSynchronize(ze_event_handle_t hEvent, uint64_t timeout) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventHostSynchronize(";
    ss << std::hex << std::showbase;
    ss << "hEvent: " << hEvent;
    ss << ", timeout: " << timeout;
    ss << ")";
    return ss.str();
}
inline void trace_zeEventHostSynchronize(ze_event_handle_t hEvent, uint64_t timeout) {
    TRACE_EVENT_BEGIN("API", "zeEventHostSynchronize");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventHostSynchronize(hEvent, timeout) + "..\n";
}
inline void
trace_zeEventHostSynchronize(ze_result_t ret, ze_event_handle_t hEvent, uint64_t timeout) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventHostSynchronize(hEvent, timeout) + trace_ze_result_t(ret);
}

inline std::string _trace_zeEventQueryStatus(ze_event_handle_t hEvent) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventQueryStatus(";
    ss << std::hex << std::showbase;
    ss << "hEvent: " << hEvent;
    ss << ")";
    return ss.str();
}
inline void trace_zeEventQueryStatus(ze_event_handle_t hEvent) {
    TRACE_EVENT_BEGIN("API", "zeEventQueryStatus");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventQueryStatus(hEvent) + "..\n";
}
inline void trace_zeEventQueryStatus(ze_result_t ret, ze_event_handle_t hEvent) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventQueryStatus(hEvent) + trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendEventReset(ze_command_list_handle_t hCommandList,
                                                        ze_event_handle_t hEvent) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendEventReset(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", hEvent: " << hEvent;
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendEventReset(ze_command_list_handle_t hCommandList,
                                                ze_event_handle_t hEvent) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendEventReset");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendEventReset(hCommandList, hEvent) + "..\n";
}
inline void trace_zeCommandListAppendEventReset(ze_result_t ret,
                                                ze_command_list_handle_t hCommandList,
                                                ze_event_handle_t hEvent) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendEventReset(hCommandList, hEvent) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeEventHostReset(ze_event_handle_t hEvent) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventHostReset(";
    ss << std::hex << std::showbase;
    ss << "hEvent: " << hEvent;
    ss << ")";
    return ss.str();
}
inline void trace_zeEventHostReset(ze_event_handle_t hEvent) {
    TRACE_EVENT_BEGIN("API", "zeEventHostReset");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventHostReset(hEvent) + "..\n";
}
inline void trace_zeEventHostReset(ze_result_t ret, ze_event_handle_t hEvent) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventHostReset(hEvent) + trace_ze_result_t(ret);
}

inline std::string _trace_zeEventQueryKernelTimestamp(ze_event_handle_t hEvent,
                                                      ze_kernel_timestamp_result_t *dstptr) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventQueryKernelTimestamp(";
    ss << std::hex << std::showbase;
    ss << "hEvent: " << hEvent;
    if (dstptr == nullptr) {
        ss << ", dstptr: nullptr";
    } else {
        ss << ", dstptr {";
        ss << ", global {";
        ss << "kernelStart: " << dstptr->global.kernelStart;
        ss << ", kernelEnd: " << dstptr->global.kernelEnd;
        ss << "}";
        ss << ", context {";
        ss << "kernelStart: " << dstptr->context.kernelStart;
        ss << ", kernelEnd: " << dstptr->context.kernelEnd;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeEventQueryKernelTimestamp(ze_event_handle_t hEvent,
                                              ze_kernel_timestamp_result_t *dstptr) {
    TRACE_EVENT_BEGIN("API", "zeEventQueryKernelTimestamp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventQueryKernelTimestamp(hEvent, dstptr) + "..\n";
}
inline void trace_zeEventQueryKernelTimestamp(ze_result_t ret,
                                              ze_event_handle_t hEvent,
                                              ze_kernel_timestamp_result_t *dstptr) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventQueryKernelTimestamp(hEvent, dstptr) + trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListAppendQueryKernelTimestamps(ze_command_list_handle_t hCommandList,
                                                uint32_t numEvents,
                                                ze_event_handle_t *phEvents,
                                                void *dstptr,
                                                const size_t *pOffsets,
                                                ze_event_handle_t hSignalEvent,
                                                uint32_t numWaitEvents,
                                                ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendQueryKernelTimestamps(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", numEvents: " << numEvents;
    if (phEvents == nullptr) {
        ss << ", phEvents: nullptr";
    } else {
        ss << ", phEvents: " << *phEvents;
    }
    ss << ", dstptr: " << dstptr;
    if (pOffsets == nullptr) {
        ss << ", pOffsets: nullptr";
    } else {
        ss << ", pOffsets: " << *pOffsets;
    }
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendQueryKernelTimestamps(ze_command_list_handle_t hCommandList,
                                                           uint32_t numEvents,
                                                           ze_event_handle_t *phEvents,
                                                           void *dstptr,
                                                           const size_t *pOffsets,
                                                           ze_event_handle_t hSignalEvent,
                                                           uint32_t numWaitEvents,
                                                           ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendQueryKernelTimestamps");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendQueryKernelTimestamps(hCommandList,
                                                                     numEvents,
                                                                     phEvents,
                                                                     dstptr,
                                                                     pOffsets,
                                                                     hSignalEvent,
                                                                     numWaitEvents,
                                                                     phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendQueryKernelTimestamps(ze_result_t ret,
                                                           ze_command_list_handle_t hCommandList,
                                                           uint32_t numEvents,
                                                           ze_event_handle_t *phEvents,
                                                           void *dstptr,
                                                           const size_t *pOffsets,
                                                           ze_event_handle_t hSignalEvent,
                                                           uint32_t numWaitEvents,
                                                           ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendQueryKernelTimestamps(hCommandList,
                                                                     numEvents,
                                                                     phEvents,
                                                                     dstptr,
                                                                     pOffsets,
                                                                     hSignalEvent,
                                                                     numWaitEvents,
                                                                     phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeEventGetEventPool(ze_event_handle_t hEvent,
                                              ze_event_pool_handle_t *phEventPool) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventGetEventPool(";
    ss << std::hex << std::showbase;
    ss << "hEvent: " << hEvent;
    if (phEventPool == nullptr) {
        ss << ", phEventPool: nullptr";
    } else {
        ss << ", phEventPool: " << *phEventPool;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeEventGetEventPool(ze_event_handle_t hEvent,
                                      ze_event_pool_handle_t *phEventPool) {
    TRACE_EVENT_BEGIN("API", "zeEventGetEventPool");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventGetEventPool(hEvent, phEventPool) + "..\n";
}
inline void trace_zeEventGetEventPool(ze_result_t ret,
                                      ze_event_handle_t hEvent,
                                      ze_event_pool_handle_t *phEventPool) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventGetEventPool(hEvent, phEventPool) + trace_ze_result_t(ret);
}

inline std::string _trace_zeEventGetSignalScope(ze_event_handle_t hEvent,
                                                ze_event_scope_flags_t *pSignalScope) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventGetSignalScope(";
    ss << std::hex << std::showbase;
    ss << "hEvent: " << hEvent;
    if (pSignalScope == nullptr) {
        ss << ", pSignalScope: nullptr";
    } else {
        ss << ", pSignalScope: " << *pSignalScope;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeEventGetSignalScope(ze_event_handle_t hEvent,
                                        ze_event_scope_flags_t *pSignalScope) {
    TRACE_EVENT_BEGIN("API", "zeEventGetSignalScope");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventGetSignalScope(hEvent, pSignalScope) + "..\n";
}
inline void trace_zeEventGetSignalScope(ze_result_t ret,
                                        ze_event_handle_t hEvent,
                                        ze_event_scope_flags_t *pSignalScope) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventGetSignalScope(hEvent, pSignalScope) + trace_ze_result_t(ret);
}

inline std::string _trace_zeEventGetWaitScope(ze_event_handle_t hEvent,
                                              ze_event_scope_flags_t *pWaitScope) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventGetWaitScope(";
    ss << std::hex << std::showbase;
    ss << "hEvent: " << hEvent;
    if (pWaitScope == nullptr) {
        ss << ", pWaitScope: nullptr";
    } else {
        ss << ", pWaitScope: " << *pWaitScope;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeEventGetWaitScope(ze_event_handle_t hEvent,
                                      ze_event_scope_flags_t *pWaitScope) {
    TRACE_EVENT_BEGIN("API", "zeEventGetWaitScope");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventGetWaitScope(hEvent, pWaitScope) + "..\n";
}
inline void trace_zeEventGetWaitScope(ze_result_t ret,
                                      ze_event_handle_t hEvent,
                                      ze_event_scope_flags_t *pWaitScope) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventGetWaitScope(hEvent, pWaitScope) + trace_ze_result_t(ret);
}

inline std::string _trace_zeEventPoolGetContextHandle(ze_event_pool_handle_t hEventPool,
                                                      ze_context_handle_t *phContext) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventPoolGetContextHandle(";
    ss << std::hex << std::showbase;
    ss << "hEventPool: " << hEventPool;
    if (phContext == nullptr) {
        ss << ", phContext: nullptr";
    } else {
        ss << ", phContext: " << *phContext;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeEventPoolGetContextHandle(ze_event_pool_handle_t hEventPool,
                                              ze_context_handle_t *phContext) {
    TRACE_EVENT_BEGIN("API", "zeEventPoolGetContextHandle");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolGetContextHandle(hEventPool, phContext) + "..\n";
}
inline void trace_zeEventPoolGetContextHandle(ze_result_t ret,
                                              ze_event_pool_handle_t hEventPool,
                                              ze_context_handle_t *phContext) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolGetContextHandle(hEventPool, phContext) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeEventPoolGetFlags(ze_event_pool_handle_t hEventPool,
                                              ze_event_pool_flags_t *pFlags) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventPoolGetFlags(";
    ss << std::hex << std::showbase;
    ss << "hEventPool: " << hEventPool;
    if (pFlags == nullptr) {
        ss << ", pFlags: nullptr";
    } else {
        ss << ", pFlags: " << *pFlags;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeEventPoolGetFlags(ze_event_pool_handle_t hEventPool,
                                      ze_event_pool_flags_t *pFlags) {
    TRACE_EVENT_BEGIN("API", "zeEventPoolGetFlags");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolGetFlags(hEventPool, pFlags) + "..\n";
}
inline void trace_zeEventPoolGetFlags(ze_result_t ret,
                                      ze_event_pool_handle_t hEventPool,
                                      ze_event_pool_flags_t *pFlags) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventPoolGetFlags(hEventPool, pFlags) + trace_ze_result_t(ret);
}

inline std::string _trace_zeFenceCreate(ze_command_queue_handle_t hCommandQueue,
                                        const ze_fence_desc_t *desc,
                                        ze_fence_handle_t *phFence) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeFenceCreate(";
    ss << std::hex << std::showbase;
    ss << "hCommandQueue: " << hCommandQueue;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << "}";
    }
    if (phFence == nullptr) {
        ss << ", phFence: nullptr";
    } else {
        ss << ", phFence: " << *phFence;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeFenceCreate(ze_command_queue_handle_t hCommandQueue,
                                const ze_fence_desc_t *desc,
                                ze_fence_handle_t *phFence) {
    TRACE_EVENT_BEGIN("API", "zeFenceCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFenceCreate(hCommandQueue, desc, phFence) + "..\n";
}
inline void trace_zeFenceCreate(ze_result_t ret,
                                ze_command_queue_handle_t hCommandQueue,
                                const ze_fence_desc_t *desc,
                                ze_fence_handle_t *phFence) {
    if (phFence)
        TRACE_EVENT_END("API", perfetto::Flow::ProcessScoped((uint64_t)*phFence));
    else
        TRACE_EVENT_END("API");

    if (IS_API_TRACE())
        std::cerr << _trace_zeFenceCreate(hCommandQueue, desc, phFence) + trace_ze_result_t(ret);
}

inline std::string _trace_zeFenceDestroy(ze_fence_handle_t hFence) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeFenceDestroy(";
    ss << std::hex << std::showbase;
    ss << "hFence: " << hFence;
    ss << ")";
    return ss.str();
}
inline void trace_zeFenceDestroy(ze_fence_handle_t hFence) {
    TRACE_EVENT_BEGIN("API",
                      "zeFenceDestroy",
                      perfetto::TerminatingFlow::ProcessScoped((uint64_t)hFence));
    if (IS_API_TRACE())
        std::cerr << _trace_zeFenceDestroy(hFence) + "..\n";
}
inline void trace_zeFenceDestroy(ze_result_t ret, ze_fence_handle_t hFence) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFenceDestroy(hFence) + trace_ze_result_t(ret);
}

inline std::string _trace_zeFenceHostSynchronize(ze_fence_handle_t hFence, uint64_t timeout) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeFenceHostSynchronize(";
    ss << std::hex << std::showbase;
    ss << "hFence: " << hFence;
    ss << ", timeout: " << timeout;
    ss << ")";
    return ss.str();
}
inline void trace_zeFenceHostSynchronize(ze_fence_handle_t hFence, uint64_t timeout) {
    TRACE_EVENT_BEGIN("API",
                      "zeFenceHostSynchronize",
                      perfetto::Flow::ProcessScoped((uint64_t)hFence));
    if (IS_API_TRACE())
        std::cerr << _trace_zeFenceHostSynchronize(hFence, timeout) + "..\n";
}
inline void
trace_zeFenceHostSynchronize(ze_result_t ret, ze_fence_handle_t hFence, uint64_t timeout) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFenceHostSynchronize(hFence, timeout) + trace_ze_result_t(ret);
}

inline std::string _trace_zeFenceQueryStatus(ze_fence_handle_t hFence) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeFenceQueryStatus(";
    ss << std::hex << std::showbase;
    ss << "hFence: " << hFence;
    ss << ")";
    return ss.str();
}
inline void trace_zeFenceQueryStatus(ze_fence_handle_t hFence) {
    TRACE_EVENT_BEGIN("API", "zeFenceQueryStatus");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFenceQueryStatus(hFence) + "..\n";
}
inline void trace_zeFenceQueryStatus(ze_result_t ret, ze_fence_handle_t hFence) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFenceQueryStatus(hFence) + trace_ze_result_t(ret);
}

inline std::string _trace_zeFenceReset(ze_fence_handle_t hFence) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeFenceReset(";
    ss << std::hex << std::showbase;
    ss << "hFence: " << hFence;
    ss << ")";
    return ss.str();
}
inline void trace_zeFenceReset(ze_fence_handle_t hFence) {
    TRACE_EVENT_BEGIN("API", "zeFenceReset", perfetto::Flow::ProcessScoped((uint64_t)hFence));
    if (IS_API_TRACE())
        std::cerr << _trace_zeFenceReset(hFence) + "..\n";
}
inline void trace_zeFenceReset(ze_result_t ret, ze_fence_handle_t hFence) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFenceReset(hFence) + trace_ze_result_t(ret);
}

inline std::string _trace_zeImageGetProperties(ze_device_handle_t hDevice,
                                               const ze_image_desc_t *desc,
                                               ze_image_properties_t *pImageProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeImageGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << ", type: " << desc->type;
        ss << ", format {";
        ss << "layout: " << desc->format.layout;
        ss << ", type: " << desc->format.type;
        ss << ", x: " << desc->format.x;
        ss << ", y: " << desc->format.y;
        ss << ", z: " << desc->format.z;
        ss << ", w: " << desc->format.w;
        ss << "}";
        ss << ", width: " << desc->width;
        ss << ", height: " << desc->height;
        ss << ", depth: " << desc->depth;
        ss << ", arraylevels: " << desc->arraylevels;
        ss << ", miplevels: " << desc->miplevels;
        ss << "}";
    }
    if (pImageProperties == nullptr) {
        ss << ", pImageProperties: nullptr";
    } else {
        ss << ", pImageProperties {";
        ss << "stype: " << pImageProperties->stype;
        ss << ", pNext: " << pImageProperties->pNext;
        ss << ", samplerFilterFlags: " << pImageProperties->samplerFilterFlags;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeImageGetProperties(ze_device_handle_t hDevice,
                                       const ze_image_desc_t *desc,
                                       ze_image_properties_t *pImageProperties) {
    TRACE_EVENT_BEGIN("API", "zeImageGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageGetProperties(hDevice, desc, pImageProperties) + "..\n";
}
inline void trace_zeImageGetProperties(ze_result_t ret,
                                       ze_device_handle_t hDevice,
                                       const ze_image_desc_t *desc,
                                       ze_image_properties_t *pImageProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageGetProperties(hDevice, desc, pImageProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeImageCreate(ze_context_handle_t hContext,
                                        ze_device_handle_t hDevice,
                                        const ze_image_desc_t *desc,
                                        ze_image_handle_t *phImage) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeImageCreate(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << ", type: " << desc->type;
        ss << ", format {";
        ss << "layout: " << desc->format.layout;
        ss << ", type: " << desc->format.type;
        ss << ", x: " << desc->format.x;
        ss << ", y: " << desc->format.y;
        ss << ", z: " << desc->format.z;
        ss << ", w: " << desc->format.w;
        ss << "}";
        ss << ", width: " << desc->width;
        ss << ", height: " << desc->height;
        ss << ", depth: " << desc->depth;
        ss << ", arraylevels: " << desc->arraylevels;
        ss << ", miplevels: " << desc->miplevels;
        ss << "}";
    }
    if (phImage == nullptr) {
        ss << ", phImage: nullptr";
    } else {
        ss << ", phImage: " << *phImage;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeImageCreate(ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                const ze_image_desc_t *desc,
                                ze_image_handle_t *phImage) {
    TRACE_EVENT_BEGIN("API", "zeImageCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageCreate(hContext, hDevice, desc, phImage) + "..\n";
}
inline void trace_zeImageCreate(ze_result_t ret,
                                ze_context_handle_t hContext,
                                ze_device_handle_t hDevice,
                                const ze_image_desc_t *desc,
                                ze_image_handle_t *phImage) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageCreate(hContext, hDevice, desc, phImage) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeImageDestroy(ze_image_handle_t hImage) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeImageDestroy(";
    ss << std::hex << std::showbase;
    ss << "hImage: " << hImage;
    ss << ")";
    return ss.str();
}
inline void trace_zeImageDestroy(ze_image_handle_t hImage) {
    TRACE_EVENT_BEGIN("API", "zeImageDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageDestroy(hImage) + "..\n";
}
inline void trace_zeImageDestroy(ze_result_t ret, ze_image_handle_t hImage) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageDestroy(hImage) + trace_ze_result_t(ret);
}

inline std::string _trace_zeMemAllocShared(ze_context_handle_t hContext,
                                           const ze_device_mem_alloc_desc_t *device_desc,
                                           const ze_host_mem_alloc_desc_t *host_desc,
                                           size_t size,
                                           size_t alignment,
                                           ze_device_handle_t hDevice,
                                           void **pptr) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemAllocShared(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    if (device_desc == nullptr) {
        ss << ", device_desc: nullptr";
    } else {
        ss << ", device_desc {";
        ss << "stype: " << device_desc->stype;
        ss << ", pNext: " << device_desc->pNext;
        ss << ", flags: " << device_desc->flags;
        ss << ", ordinal: " << device_desc->ordinal;
        ss << "}";
    }
    if (host_desc == nullptr) {
        ss << ", host_desc: nullptr";
    } else {
        ss << ", host_desc {";
        ss << "stype: " << host_desc->stype;
        ss << ", pNext: " << host_desc->pNext;
        ss << ", flags: " << host_desc->flags;
        ss << "}";
    }
    ss << ", size: " << size;
    ss << ", alignment: " << alignment;
    ss << ", hDevice: " << hDevice;
    ss << ", pptr: " << pptr;
    ss << ")";
    return ss.str();
}
inline void trace_zeMemAllocShared(ze_context_handle_t hContext,
                                   const ze_device_mem_alloc_desc_t *device_desc,
                                   const ze_host_mem_alloc_desc_t *host_desc,
                                   size_t size,
                                   size_t alignment,
                                   ze_device_handle_t hDevice,
                                   void **pptr) {
    TRACE_EVENT_BEGIN("API", "zeMemAllocShared");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemAllocShared(hContext,
                                             device_desc,
                                             host_desc,
                                             size,
                                             alignment,
                                             hDevice,
                                             pptr) +
                         "..\n";
}
inline void trace_zeMemAllocShared(ze_result_t ret,
                                   ze_context_handle_t hContext,
                                   const ze_device_mem_alloc_desc_t *device_desc,
                                   const ze_host_mem_alloc_desc_t *host_desc,
                                   size_t size,
                                   size_t alignment,
                                   ze_device_handle_t hDevice,
                                   void **pptr) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemAllocShared(hContext,
                                             device_desc,
                                             host_desc,
                                             size,
                                             alignment,
                                             hDevice,
                                             pptr) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeMemAllocDevice(ze_context_handle_t hContext,
                                           const ze_device_mem_alloc_desc_t *device_desc,
                                           size_t size,
                                           size_t alignment,
                                           ze_device_handle_t hDevice,
                                           void **pptr) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemAllocDevice(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    if (device_desc == nullptr) {
        ss << ", device_desc: nullptr";
    } else {
        ss << ", device_desc {";
        ss << "stype: " << device_desc->stype;
        ss << ", pNext: " << device_desc->pNext;
        ss << ", flags: " << device_desc->flags;
        ss << ", ordinal: " << device_desc->ordinal;
        ss << "}";
    }
    ss << ", size: " << size;
    ss << ", alignment: " << alignment;
    ss << ", hDevice: " << hDevice;
    ss << ", pptr: " << pptr;
    ss << ")";
    return ss.str();
}
inline void trace_zeMemAllocDevice(ze_context_handle_t hContext,
                                   const ze_device_mem_alloc_desc_t *device_desc,
                                   size_t size,
                                   size_t alignment,
                                   ze_device_handle_t hDevice,
                                   void **pptr) {
    TRACE_EVENT_BEGIN("API", "zeMemAllocDevice");
    if (IS_API_TRACE())
        std::cerr
            << _trace_zeMemAllocDevice(hContext, device_desc, size, alignment, hDevice, pptr) +
                   "..\n";
}
inline void trace_zeMemAllocDevice(ze_result_t ret,
                                   ze_context_handle_t hContext,
                                   const ze_device_mem_alloc_desc_t *device_desc,
                                   size_t size,
                                   size_t alignment,
                                   ze_device_handle_t hDevice,
                                   void **pptr) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr
            << _trace_zeMemAllocDevice(hContext, device_desc, size, alignment, hDevice, pptr) +
                   trace_ze_result_t(ret);
}

inline std::string _trace_zeMemAllocHost(ze_context_handle_t hContext,
                                         const ze_host_mem_alloc_desc_t *host_desc,
                                         size_t size,
                                         size_t alignment,
                                         void **pptr) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemAllocHost(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    if (host_desc == nullptr) {
        ss << ", host_desc: nullptr";
    } else {
        ss << ", host_desc {";
        ss << "stype: " << host_desc->stype;
        ss << ", pNext: " << host_desc->pNext;
        ss << ", flags: " << host_desc->flags;
        ss << "}";
    }
    ss << ", size: " << size;
    ss << ", alignment: " << alignment;
    ss << ", pptr: " << pptr;
    ss << ")";
    return ss.str();
}
inline void trace_zeMemAllocHost(ze_context_handle_t hContext,
                                 const ze_host_mem_alloc_desc_t *host_desc,
                                 size_t size,
                                 size_t alignment,
                                 void **pptr) {
    TRACE_EVENT_BEGIN("API", "zeMemAllocHost");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemAllocHost(hContext, host_desc, size, alignment, pptr) + "..\n";
}
inline void trace_zeMemAllocHost(ze_result_t ret,
                                 ze_context_handle_t hContext,
                                 const ze_host_mem_alloc_desc_t *host_desc,
                                 size_t size,
                                 size_t alignment,
                                 void **pptr) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemAllocHost(hContext, host_desc, size, alignment, pptr) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeMemFree(ze_context_handle_t hContext, void *ptr) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemFree(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", ptr: " << ptr;
    ss << ")";
    return ss.str();
}
inline void trace_zeMemFree(ze_context_handle_t hContext, void *ptr) {
    TRACE_EVENT_BEGIN("API", "zeMemFree");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemFree(hContext, ptr) + "..\n";
}
inline void trace_zeMemFree(ze_result_t ret, ze_context_handle_t hContext, void *ptr) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemFree(hContext, ptr) + trace_ze_result_t(ret);
}

inline std::string
_trace_zeMemGetAllocProperties(ze_context_handle_t hContext,
                               const void *ptr,
                               ze_memory_allocation_properties_t *pMemAllocProperties,
                               ze_device_handle_t *phDevice) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemGetAllocProperties(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", ptr: " << ptr;
    if (pMemAllocProperties == nullptr) {
        ss << ", pMemAllocProperties: nullptr";
    } else {
        ss << ", pMemAllocProperties {";
        ss << "stype: " << pMemAllocProperties->stype;
        ss << ", pNext: " << pMemAllocProperties->pNext;
        ss << ", type: " << pMemAllocProperties->type;
        ss << ", id: " << pMemAllocProperties->id;
        ss << ", pageSize: " << pMemAllocProperties->pageSize;
        ss << "}";
    }
    if (phDevice == nullptr) {
        ss << ", phDevice: nullptr";
    } else {
        ss << ", phDevice: " << *phDevice;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeMemGetAllocProperties(ze_context_handle_t hContext,
                                          const void *ptr,
                                          ze_memory_allocation_properties_t *pMemAllocProperties,
                                          ze_device_handle_t *phDevice) {
    TRACE_EVENT_BEGIN("API", "zeMemGetAllocProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetAllocProperties(hContext, ptr, pMemAllocProperties, phDevice) +
                         "..\n";
}
inline void trace_zeMemGetAllocProperties(ze_result_t ret,
                                          ze_context_handle_t hContext,
                                          const void *ptr,
                                          ze_memory_allocation_properties_t *pMemAllocProperties,
                                          ze_device_handle_t *phDevice) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetAllocProperties(hContext, ptr, pMemAllocProperties, phDevice) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeMemGetAddressRange(ze_context_handle_t hContext,
                                               const void *ptr,
                                               void **pBase,
                                               size_t *pSize) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemGetAddressRange(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", ptr: " << ptr;
    ss << ", pBase: " << pBase;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeMemGetAddressRange(ze_context_handle_t hContext,
                                       const void *ptr,
                                       void **pBase,
                                       size_t *pSize) {
    TRACE_EVENT_BEGIN("API", "zeMemGetAddressRange");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetAddressRange(hContext, ptr, pBase, pSize) + "..\n";
}
inline void trace_zeMemGetAddressRange(ze_result_t ret,
                                       ze_context_handle_t hContext,
                                       const void *ptr,
                                       void **pBase,
                                       size_t *pSize) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetAddressRange(hContext, ptr, pBase, pSize) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeMemGetIpcHandle(ze_context_handle_t hContext,
                                            const void *ptr,
                                            ze_ipc_mem_handle_t *pIpcHandle) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemGetIpcHandle(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", ptr: " << ptr;
    if (pIpcHandle == nullptr) {
        ss << ", pIpcHandle: nullptr";
    } else {
        ss << ", pIpcHandle {";
        ss << "data: ";
        ss << trace_char_array_hex(pIpcHandle->data, sizeof(pIpcHandle->data)).str();
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeMemGetIpcHandle(ze_context_handle_t hContext,
                                    const void *ptr,
                                    ze_ipc_mem_handle_t *pIpcHandle) {
    TRACE_EVENT_BEGIN("API", "zeMemGetIpcHandle");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetIpcHandle(hContext, ptr, pIpcHandle) + "..\n";
}
inline void trace_zeMemGetIpcHandle(ze_result_t ret,
                                    ze_context_handle_t hContext,
                                    const void *ptr,
                                    ze_ipc_mem_handle_t *pIpcHandle) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetIpcHandle(hContext, ptr, pIpcHandle) + trace_ze_result_t(ret);
}

inline std::string _trace_zeMemGetIpcHandleFromFileDescriptorExp(ze_context_handle_t hContext,
                                                                 uint64_t handle,
                                                                 ze_ipc_mem_handle_t *pIpcHandle) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemGetIpcHandleFromFileDescriptorExp(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", handle: " << handle;
    if (pIpcHandle == nullptr) {
        ss << ", pIpcHandle: nullptr";
    } else {
        ss << ", pIpcHandle {";
        ss << "data: ";
        ss << trace_char_array_hex(pIpcHandle->data, sizeof(pIpcHandle->data)).str();
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeMemGetIpcHandleFromFileDescriptorExp(ze_context_handle_t hContext,
                                                         uint64_t handle,
                                                         ze_ipc_mem_handle_t *pIpcHandle) {
    TRACE_EVENT_BEGIN("API", "zeMemGetIpcHandleFromFileDescriptorExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetIpcHandleFromFileDescriptorExp(hContext, handle, pIpcHandle) +
                         "..\n";
}
inline void trace_zeMemGetIpcHandleFromFileDescriptorExp(ze_result_t ret,
                                                         ze_context_handle_t hContext,
                                                         uint64_t handle,
                                                         ze_ipc_mem_handle_t *pIpcHandle) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetIpcHandleFromFileDescriptorExp(hContext, handle, pIpcHandle) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeMemGetFileDescriptorFromIpcHandleExp(ze_context_handle_t hContext,
                                                                 ze_ipc_mem_handle_t ipcHandle,
                                                                 uint64_t *pHandle) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemGetFileDescriptorFromIpcHandleExp(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", ipcHandle {";
    ss << "data: ";
    ss << trace_char_array_hex(ipcHandle.data, sizeof(ipcHandle.data)).str();
    ss << "}";
    if (pHandle == nullptr) {
        ss << ", pHandle: nullptr";
    } else {
        ss << ", pHandle: " << *pHandle;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeMemGetFileDescriptorFromIpcHandleExp(ze_context_handle_t hContext,
                                                         ze_ipc_mem_handle_t ipcHandle,
                                                         uint64_t *pHandle) {
    TRACE_EVENT_BEGIN("API", "zeMemGetFileDescriptorFromIpcHandleExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetFileDescriptorFromIpcHandleExp(hContext, ipcHandle, pHandle) +
                         "..\n";
}
inline void trace_zeMemGetFileDescriptorFromIpcHandleExp(ze_result_t ret,
                                                         ze_context_handle_t hContext,
                                                         ze_ipc_mem_handle_t ipcHandle,
                                                         uint64_t *pHandle) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetFileDescriptorFromIpcHandleExp(hContext, ipcHandle, pHandle) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeMemPutIpcHandle(ze_context_handle_t hContext,
                                            ze_ipc_mem_handle_t handle) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemPutIpcHandle(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", handle {";
    ss << "data: ";
    ss << trace_char_array_hex(handle.data, sizeof(handle.data)).str();
    ss << "}";
    ss << ")";
    return ss.str();
}
inline void trace_zeMemPutIpcHandle(ze_context_handle_t hContext, ze_ipc_mem_handle_t handle) {
    TRACE_EVENT_BEGIN("API", "zeMemPutIpcHandle");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemPutIpcHandle(hContext, handle) + "..\n";
}
inline void
trace_zeMemPutIpcHandle(ze_result_t ret, ze_context_handle_t hContext, ze_ipc_mem_handle_t handle) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemPutIpcHandle(hContext, handle) + trace_ze_result_t(ret);
}

inline std::string _trace_zeMemOpenIpcHandle(ze_context_handle_t hContext,
                                             ze_device_handle_t hDevice,
                                             ze_ipc_mem_handle_t handle,
                                             ze_ipc_memory_flags_t flags,
                                             void **pptr) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemOpenIpcHandle(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", handle {";
    ss << "data: ";
    ss << trace_char_array_hex(handle.data, sizeof(handle.data)).str();
    ss << "}";
    ss << ", flags: " << flags;
    ss << ", pptr: " << pptr;
    ss << ")";
    return ss.str();
}
inline void trace_zeMemOpenIpcHandle(ze_context_handle_t hContext,
                                     ze_device_handle_t hDevice,
                                     ze_ipc_mem_handle_t handle,
                                     ze_ipc_memory_flags_t flags,
                                     void **pptr) {
    TRACE_EVENT_BEGIN("API", "zeMemOpenIpcHandle");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemOpenIpcHandle(hContext, hDevice, handle, flags, pptr) + "..\n";
}
inline void trace_zeMemOpenIpcHandle(ze_result_t ret,
                                     ze_context_handle_t hContext,
                                     ze_device_handle_t hDevice,
                                     ze_ipc_mem_handle_t handle,
                                     ze_ipc_memory_flags_t flags,
                                     void **pptr) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemOpenIpcHandle(hContext, hDevice, handle, flags, pptr) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeMemCloseIpcHandle(ze_context_handle_t hContext, const void *ptr) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemCloseIpcHandle(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", ptr: " << ptr;
    ss << ")";
    return ss.str();
}
inline void trace_zeMemCloseIpcHandle(ze_context_handle_t hContext, const void *ptr) {
    TRACE_EVENT_BEGIN("API", "zeMemCloseIpcHandle");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemCloseIpcHandle(hContext, ptr) + "..\n";
}
inline void
trace_zeMemCloseIpcHandle(ze_result_t ret, ze_context_handle_t hContext, const void *ptr) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemCloseIpcHandle(hContext, ptr) + trace_ze_result_t(ret);
}

inline std::string _trace_zeMemSetAtomicAccessAttributeExp(ze_context_handle_t hContext,
                                                           ze_device_handle_t hDevice,
                                                           const void *ptr,
                                                           size_t size,
                                                           ze_memory_atomic_attr_exp_flags_t attr) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemSetAtomicAccessAttributeExp(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", ptr: " << ptr;
    ss << ", size: " << size;
    ss << ", attr: " << attr;
    ss << ")";
    return ss.str();
}
inline void trace_zeMemSetAtomicAccessAttributeExp(ze_context_handle_t hContext,
                                                   ze_device_handle_t hDevice,
                                                   const void *ptr,
                                                   size_t size,
                                                   ze_memory_atomic_attr_exp_flags_t attr) {
    TRACE_EVENT_BEGIN("API", "zeMemSetAtomicAccessAttributeExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemSetAtomicAccessAttributeExp(hContext, hDevice, ptr, size, attr) +
                         "..\n";
}
inline void trace_zeMemSetAtomicAccessAttributeExp(ze_result_t ret,
                                                   ze_context_handle_t hContext,
                                                   ze_device_handle_t hDevice,
                                                   const void *ptr,
                                                   size_t size,
                                                   ze_memory_atomic_attr_exp_flags_t attr) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemSetAtomicAccessAttributeExp(hContext, hDevice, ptr, size, attr) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeMemGetAtomicAccessAttributeExp(ze_context_handle_t hContext,
                                        ze_device_handle_t hDevice,
                                        const void *ptr,
                                        size_t size,
                                        ze_memory_atomic_attr_exp_flags_t *pAttr) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemGetAtomicAccessAttributeExp(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", ptr: " << ptr;
    ss << ", size: " << size;
    if (pAttr == nullptr) {
        ss << ", pAttr: nullptr";
    } else {
        ss << ", pAttr: " << *pAttr;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeMemGetAtomicAccessAttributeExp(ze_context_handle_t hContext,
                                                   ze_device_handle_t hDevice,
                                                   const void *ptr,
                                                   size_t size,
                                                   ze_memory_atomic_attr_exp_flags_t *pAttr) {
    TRACE_EVENT_BEGIN("API", "zeMemGetAtomicAccessAttributeExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetAtomicAccessAttributeExp(hContext, hDevice, ptr, size, pAttr) +
                         "..\n";
}
inline void trace_zeMemGetAtomicAccessAttributeExp(ze_result_t ret,
                                                   ze_context_handle_t hContext,
                                                   ze_device_handle_t hDevice,
                                                   const void *ptr,
                                                   size_t size,
                                                   ze_memory_atomic_attr_exp_flags_t *pAttr) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetAtomicAccessAttributeExp(hContext, hDevice, ptr, size, pAttr) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeModuleCreate(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         const ze_module_desc_t *desc,
                                         ze_module_handle_t *phModule,
                                         ze_module_build_log_handle_t *phBuildLog) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeModuleCreate(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", format: " << desc->format;
        ss << ", inputSize: " << desc->inputSize;
        if (desc->pInputModule == nullptr) {
            ss << ", pInputModule: nullptr";
        } else {
            ss << ", pInputModule: " << *desc->pInputModule;
        }
        if (desc->pBuildFlags == nullptr) {
            ss << ", pBuildFlags: nullptr";
        } else {
            ss << ", pBuildFlags: " << desc->pBuildFlags;
        }
        if (desc->pConstants == nullptr) {
            ss << ", pConstants: nullptr";
        } else {
            ss << ", pConstants {";
            ss << "numConstants: " << desc->pConstants->numConstants;
            if (desc->pConstants->pConstantIds == nullptr) {
                ss << ", pConstantIds: nullptr";
            } else {
                ss << ", pConstantIds: " << *desc->pConstants->pConstantIds;
            }
            ss << ", pConstantValues: " << desc->pConstants->pConstantValues;
            ss << "}";
            ss << "}";
        }
    }
    if (phModule == nullptr) {
        ss << ", phModule: nullptr";
    } else {
        ss << ", phModule: " << *phModule;
    }
    if (phBuildLog == nullptr) {
        ss << ", phBuildLog: nullptr";
    } else {
        ss << ", phBuildLog: " << *phBuildLog;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeModuleCreate(ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 const ze_module_desc_t *desc,
                                 ze_module_handle_t *phModule,
                                 ze_module_build_log_handle_t *phBuildLog) {
    TRACE_EVENT_BEGIN("API", "zeModuleCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleCreate(hContext, hDevice, desc, phModule, phBuildLog) + "..\n";
}
inline void trace_zeModuleCreate(ze_result_t ret,
                                 ze_context_handle_t hContext,
                                 ze_device_handle_t hDevice,
                                 const ze_module_desc_t *desc,
                                 ze_module_handle_t *phModule,
                                 ze_module_build_log_handle_t *phBuildLog) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleCreate(hContext, hDevice, desc, phModule, phBuildLog) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeModuleDestroy(ze_module_handle_t hModule) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeModuleDestroy(";
    ss << std::hex << std::showbase;
    ss << "hModule: " << hModule;
    ss << ")";
    return ss.str();
}
inline void trace_zeModuleDestroy(ze_module_handle_t hModule) {
    TRACE_EVENT_BEGIN("API", "zeModuleDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleDestroy(hModule) + "..\n";
}
inline void trace_zeModuleDestroy(ze_result_t ret, ze_module_handle_t hModule) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleDestroy(hModule) + trace_ze_result_t(ret);
}

inline std::string _trace_zeModuleDynamicLink(uint32_t numModules,
                                              ze_module_handle_t *phModules,
                                              ze_module_build_log_handle_t *phLinkLog) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeModuleDynamicLink(";
    ss << std::hex << std::showbase;
    ss << "numModules: " << numModules;
    if (phModules == nullptr) {
        ss << ", phModules: nullptr";
    } else {
        ss << ", phModules: " << *phModules;
    }
    if (phLinkLog == nullptr) {
        ss << ", phLinkLog: nullptr";
    } else {
        ss << ", phLinkLog: " << *phLinkLog;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeModuleDynamicLink(uint32_t numModules,
                                      ze_module_handle_t *phModules,
                                      ze_module_build_log_handle_t *phLinkLog) {
    TRACE_EVENT_BEGIN("API", "zeModuleDynamicLink");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleDynamicLink(numModules, phModules, phLinkLog) + "..\n";
}
inline void trace_zeModuleDynamicLink(ze_result_t ret,
                                      uint32_t numModules,
                                      ze_module_handle_t *phModules,
                                      ze_module_build_log_handle_t *phLinkLog) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleDynamicLink(numModules, phModules, phLinkLog) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeModuleBuildLogDestroy(ze_module_build_log_handle_t hModuleBuildLog) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeModuleBuildLogDestroy(";
    ss << std::hex << std::showbase;
    ss << "hModuleBuildLog: " << hModuleBuildLog;
    ss << ")";
    return ss.str();
}
inline void trace_zeModuleBuildLogDestroy(ze_module_build_log_handle_t hModuleBuildLog) {
    TRACE_EVENT_BEGIN("API", "zeModuleBuildLogDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleBuildLogDestroy(hModuleBuildLog) + "..\n";
}
inline void trace_zeModuleBuildLogDestroy(ze_result_t ret,
                                          ze_module_build_log_handle_t hModuleBuildLog) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleBuildLogDestroy(hModuleBuildLog) + trace_ze_result_t(ret);
}

inline std::string _trace_zeModuleBuildLogGetString(ze_module_build_log_handle_t hModuleBuildLog,
                                                    size_t *pSize,
                                                    char *pBuildLog) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeModuleBuildLogGetString(";
    ss << std::hex << std::showbase;
    ss << "hModuleBuildLog: " << hModuleBuildLog;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    if (pBuildLog == nullptr) {
        ss << ", pBuildLog: nullptr";
    } else {
        ss << ", pBuildLog: " << pBuildLog;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeModuleBuildLogGetString(ze_module_build_log_handle_t hModuleBuildLog,
                                            size_t *pSize,
                                            char *pBuildLog) {
    TRACE_EVENT_BEGIN("API", "zeModuleBuildLogGetString");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleBuildLogGetString(hModuleBuildLog, pSize, pBuildLog) + "..\n";
}
inline void trace_zeModuleBuildLogGetString(ze_result_t ret,
                                            ze_module_build_log_handle_t hModuleBuildLog,
                                            size_t *pSize,
                                            char *pBuildLog) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleBuildLogGetString(hModuleBuildLog, pSize, pBuildLog) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeModuleGetNativeBinary(ze_module_handle_t hModule,
                                                  size_t *pSize,
                                                  uint8_t *pModuleNativeBinary) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeModuleGetNativeBinary(";
    ss << std::hex << std::showbase;
    ss << "hModule: " << hModule;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    if (pModuleNativeBinary == nullptr) {
        ss << ", pModuleNativeBinary: nullptr";
    } else {
        ss << ", pModuleNativeBinary: " << *pModuleNativeBinary;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeModuleGetNativeBinary(ze_module_handle_t hModule,
                                          size_t *pSize,
                                          uint8_t *pModuleNativeBinary) {
    TRACE_EVENT_BEGIN("API", "zeModuleGetNativeBinary");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleGetNativeBinary(hModule, pSize, pModuleNativeBinary) + "..\n";
}
inline void trace_zeModuleGetNativeBinary(ze_result_t ret,
                                          ze_module_handle_t hModule,
                                          size_t *pSize,
                                          uint8_t *pModuleNativeBinary) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleGetNativeBinary(hModule, pSize, pModuleNativeBinary) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeModuleGetGlobalPointer(ze_module_handle_t hModule,
                                                   const char *pGlobalName,
                                                   size_t *pSize,
                                                   void **pptr) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeModuleGetGlobalPointer(";
    ss << std::hex << std::showbase;
    ss << "hModule: " << hModule;
    if (pGlobalName == nullptr) {
        ss << ", pGlobalName: nullptr";
    } else {
        ss << ", pGlobalName: " << pGlobalName;
    }
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    ss << ", pptr: " << pptr;
    ss << ")";
    return ss.str();
}
inline void trace_zeModuleGetGlobalPointer(ze_module_handle_t hModule,
                                           const char *pGlobalName,
                                           size_t *pSize,
                                           void **pptr) {
    TRACE_EVENT_BEGIN("API", "zeModuleGetGlobalPointer");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleGetGlobalPointer(hModule, pGlobalName, pSize, pptr) + "..\n";
}
inline void trace_zeModuleGetGlobalPointer(ze_result_t ret,
                                           ze_module_handle_t hModule,
                                           const char *pGlobalName,
                                           size_t *pSize,
                                           void **pptr) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleGetGlobalPointer(hModule, pGlobalName, pSize, pptr) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeModuleGetKernelNames(ze_module_handle_t hModule, uint32_t *pCount, const char **pNames) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeModuleGetKernelNames(";
    ss << std::hex << std::showbase;
    ss << "hModule: " << hModule;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    ss << ", pNames: " << pNames;
    ss << ")";
    return ss.str();
}
inline void
trace_zeModuleGetKernelNames(ze_module_handle_t hModule, uint32_t *pCount, const char **pNames) {
    TRACE_EVENT_BEGIN("API", "zeModuleGetKernelNames");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleGetKernelNames(hModule, pCount, pNames) + "..\n";
}
inline void trace_zeModuleGetKernelNames(ze_result_t ret,
                                         ze_module_handle_t hModule,
                                         uint32_t *pCount,
                                         const char **pNames) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleGetKernelNames(hModule, pCount, pNames) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeModuleGetProperties(ze_module_handle_t hModule,
                                                ze_module_properties_t *pModuleProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeModuleGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hModule: " << hModule;
    if (pModuleProperties == nullptr) {
        ss << ", pModuleProperties: nullptr";
    } else {
        ss << ", pModuleProperties {";
        ss << "stype: " << pModuleProperties->stype;
        ss << ", pNext: " << pModuleProperties->pNext;
        ss << ", flags: " << pModuleProperties->flags;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeModuleGetProperties(ze_module_handle_t hModule,
                                        ze_module_properties_t *pModuleProperties) {
    TRACE_EVENT_BEGIN("API", "zeModuleGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleGetProperties(hModule, pModuleProperties) + "..\n";
}
inline void trace_zeModuleGetProperties(ze_result_t ret,
                                        ze_module_handle_t hModule,
                                        ze_module_properties_t *pModuleProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleGetProperties(hModule, pModuleProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelCreate(ze_module_handle_t hModule,
                                         const ze_kernel_desc_t *desc,
                                         ze_kernel_handle_t *phKernel) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelCreate(";
    ss << std::hex << std::showbase;
    ss << "hModule: " << hModule;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        if (desc->pKernelName == nullptr) {
            ss << ", pKernelName: nullptr";
        } else {
            ss << ", pKernelName: " << desc->pKernelName;
        }
        ss << "}";
    }
    if (phKernel == nullptr) {
        ss << ", phKernel: nullptr";
    } else {
        ss << ", phKernel: " << *phKernel;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelCreate(ze_module_handle_t hModule,
                                 const ze_kernel_desc_t *desc,
                                 ze_kernel_handle_t *phKernel) {
    TRACE_EVENT_BEGIN("API", "zeKernelCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelCreate(hModule, desc, phKernel) + "..\n";
}
inline void trace_zeKernelCreate(ze_result_t ret,
                                 ze_module_handle_t hModule,
                                 const ze_kernel_desc_t *desc,
                                 ze_kernel_handle_t *phKernel) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelCreate(hModule, desc, phKernel) + trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelDestroy(ze_kernel_handle_t hKernel) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelDestroy(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelDestroy(ze_kernel_handle_t hKernel) {
    TRACE_EVENT_BEGIN("API", "zeKernelDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelDestroy(hKernel) + "..\n";
}
inline void trace_zeKernelDestroy(ze_result_t ret, ze_kernel_handle_t hKernel) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelDestroy(hKernel) + trace_ze_result_t(ret);
}

inline std::string _trace_zeModuleGetFunctionPointer(ze_module_handle_t hModule,
                                                     const char *pFunctionName,
                                                     void **pfnFunction) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeModuleGetFunctionPointer(";
    ss << std::hex << std::showbase;
    ss << "hModule: " << hModule;
    if (pFunctionName == nullptr) {
        ss << ", pFunctionName: nullptr";
    } else {
        ss << ", pFunctionName: " << pFunctionName;
    }
    ss << ", pfnFunction: " << pfnFunction;
    ss << ")";
    return ss.str();
}
inline void trace_zeModuleGetFunctionPointer(ze_module_handle_t hModule,
                                             const char *pFunctionName,
                                             void **pfnFunction) {
    TRACE_EVENT_BEGIN("API", "zeModuleGetFunctionPointer");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleGetFunctionPointer(hModule, pFunctionName, pfnFunction) +
                         "..\n";
}
inline void trace_zeModuleGetFunctionPointer(ze_result_t ret,
                                             ze_module_handle_t hModule,
                                             const char *pFunctionName,
                                             void **pfnFunction) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleGetFunctionPointer(hModule, pFunctionName, pfnFunction) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelSetGroupSize(ze_kernel_handle_t hKernel,
                                               uint32_t groupSizeX,
                                               uint32_t groupSizeY,
                                               uint32_t groupSizeZ) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelSetGroupSize(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    ss << ", groupSizeX: " << groupSizeX;
    ss << ", groupSizeY: " << groupSizeY;
    ss << ", groupSizeZ: " << groupSizeZ;
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelSetGroupSize(ze_kernel_handle_t hKernel,
                                       uint32_t groupSizeX,
                                       uint32_t groupSizeY,
                                       uint32_t groupSizeZ) {
    TRACE_EVENT_BEGIN("API", "zeKernelSetGroupSize");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSetGroupSize(hKernel, groupSizeX, groupSizeY, groupSizeZ) +
                         "..\n";
}
inline void trace_zeKernelSetGroupSize(ze_result_t ret,
                                       ze_kernel_handle_t hKernel,
                                       uint32_t groupSizeX,
                                       uint32_t groupSizeY,
                                       uint32_t groupSizeZ) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSetGroupSize(hKernel, groupSizeX, groupSizeY, groupSizeZ) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelSuggestGroupSize(ze_kernel_handle_t hKernel,
                                                   uint32_t globalSizeX,
                                                   uint32_t globalSizeY,
                                                   uint32_t globalSizeZ,
                                                   uint32_t *groupSizeX,
                                                   uint32_t *groupSizeY,
                                                   uint32_t *groupSizeZ) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelSuggestGroupSize(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    ss << ", globalSizeX: " << globalSizeX;
    ss << ", globalSizeY: " << globalSizeY;
    ss << ", globalSizeZ: " << globalSizeZ;
    if (groupSizeX == nullptr) {
        ss << ", groupSizeX: nullptr";
    } else {
        ss << ", groupSizeX: " << *groupSizeX;
    }
    if (groupSizeY == nullptr) {
        ss << ", groupSizeY: nullptr";
    } else {
        ss << ", groupSizeY: " << *groupSizeY;
    }
    if (groupSizeZ == nullptr) {
        ss << ", groupSizeZ: nullptr";
    } else {
        ss << ", groupSizeZ: " << *groupSizeZ;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelSuggestGroupSize(ze_kernel_handle_t hKernel,
                                           uint32_t globalSizeX,
                                           uint32_t globalSizeY,
                                           uint32_t globalSizeZ,
                                           uint32_t *groupSizeX,
                                           uint32_t *groupSizeY,
                                           uint32_t *groupSizeZ) {
    TRACE_EVENT_BEGIN("API", "zeKernelSuggestGroupSize");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSuggestGroupSize(hKernel,
                                                     globalSizeX,
                                                     globalSizeY,
                                                     globalSizeZ,
                                                     groupSizeX,
                                                     groupSizeY,
                                                     groupSizeZ) +
                         "..\n";
}
inline void trace_zeKernelSuggestGroupSize(ze_result_t ret,
                                           ze_kernel_handle_t hKernel,
                                           uint32_t globalSizeX,
                                           uint32_t globalSizeY,
                                           uint32_t globalSizeZ,
                                           uint32_t *groupSizeX,
                                           uint32_t *groupSizeY,
                                           uint32_t *groupSizeZ) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSuggestGroupSize(hKernel,
                                                     globalSizeX,
                                                     globalSizeY,
                                                     globalSizeZ,
                                                     groupSizeX,
                                                     groupSizeY,
                                                     groupSizeZ) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelSuggestMaxCooperativeGroupCount(ze_kernel_handle_t hKernel,
                                                                  uint32_t *totalGroupCount) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelSuggestMaxCooperativeGroupCount(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    if (totalGroupCount == nullptr) {
        ss << ", totalGroupCount: nullptr";
    } else {
        ss << ", totalGroupCount: " << *totalGroupCount;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelSuggestMaxCooperativeGroupCount(ze_kernel_handle_t hKernel,
                                                          uint32_t *totalGroupCount) {
    TRACE_EVENT_BEGIN("API", "zeKernelSuggestMaxCooperativeGroupCount");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSuggestMaxCooperativeGroupCount(hKernel, totalGroupCount) +
                         "..\n";
}
inline void trace_zeKernelSuggestMaxCooperativeGroupCount(ze_result_t ret,
                                                          ze_kernel_handle_t hKernel,
                                                          uint32_t *totalGroupCount) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSuggestMaxCooperativeGroupCount(hKernel, totalGroupCount) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelSetArgumentValue(ze_kernel_handle_t hKernel,
                                                   uint32_t argIndex,
                                                   size_t argSize,
                                                   const void *pArgValue) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelSetArgumentValue(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    ss << ", argIndex: " << argIndex;
    ss << ", argSize: " << argSize;
    ss << ", pArgValue: " << pArgValue;
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelSetArgumentValue(ze_kernel_handle_t hKernel,
                                           uint32_t argIndex,
                                           size_t argSize,
                                           const void *pArgValue) {
    TRACE_EVENT_BEGIN("API", "zeKernelSetArgumentValue");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSetArgumentValue(hKernel, argIndex, argSize, pArgValue) +
                         "..\n";
}
inline void trace_zeKernelSetArgumentValue(ze_result_t ret,
                                           ze_kernel_handle_t hKernel,
                                           uint32_t argIndex,
                                           size_t argSize,
                                           const void *pArgValue) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSetArgumentValue(hKernel, argIndex, argSize, pArgValue) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelSetIndirectAccess(ze_kernel_handle_t hKernel,
                                                    ze_kernel_indirect_access_flags_t flags) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelSetIndirectAccess(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    ss << ", flags: " << flags;
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelSetIndirectAccess(ze_kernel_handle_t hKernel,
                                            ze_kernel_indirect_access_flags_t flags) {
    TRACE_EVENT_BEGIN("API", "zeKernelSetIndirectAccess");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSetIndirectAccess(hKernel, flags) + "..\n";
}
inline void trace_zeKernelSetIndirectAccess(ze_result_t ret,
                                            ze_kernel_handle_t hKernel,
                                            ze_kernel_indirect_access_flags_t flags) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSetIndirectAccess(hKernel, flags) + trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelGetIndirectAccess(ze_kernel_handle_t hKernel,
                                                    ze_kernel_indirect_access_flags_t *pFlags) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelGetIndirectAccess(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    if (pFlags == nullptr) {
        ss << ", pFlags: nullptr";
    } else {
        ss << ", pFlags: " << *pFlags;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelGetIndirectAccess(ze_kernel_handle_t hKernel,
                                            ze_kernel_indirect_access_flags_t *pFlags) {
    TRACE_EVENT_BEGIN("API", "zeKernelGetIndirectAccess");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelGetIndirectAccess(hKernel, pFlags) + "..\n";
}
inline void trace_zeKernelGetIndirectAccess(ze_result_t ret,
                                            ze_kernel_handle_t hKernel,
                                            ze_kernel_indirect_access_flags_t *pFlags) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelGetIndirectAccess(hKernel, pFlags) + trace_ze_result_t(ret);
}

inline std::string
_trace_zeKernelGetSourceAttributes(ze_kernel_handle_t hKernel, uint32_t *pSize, char **pString) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelGetSourceAttributes(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    ss << ", pString: " << pString;
    ss << ")";
    return ss.str();
}
inline void
trace_zeKernelGetSourceAttributes(ze_kernel_handle_t hKernel, uint32_t *pSize, char **pString) {
    TRACE_EVENT_BEGIN("API", "zeKernelGetSourceAttributes");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelGetSourceAttributes(hKernel, pSize, pString) + "..\n";
}
inline void trace_zeKernelGetSourceAttributes(ze_result_t ret,
                                              ze_kernel_handle_t hKernel,
                                              uint32_t *pSize,
                                              char **pString) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelGetSourceAttributes(hKernel, pSize, pString) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelSetCacheConfig(ze_kernel_handle_t hKernel,
                                                 ze_cache_config_flags_t flags) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelSetCacheConfig(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    ss << ", flags: " << flags;
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelSetCacheConfig(ze_kernel_handle_t hKernel,
                                         ze_cache_config_flags_t flags) {
    TRACE_EVENT_BEGIN("API", "zeKernelSetCacheConfig");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSetCacheConfig(hKernel, flags) + "..\n";
}
inline void trace_zeKernelSetCacheConfig(ze_result_t ret,
                                         ze_kernel_handle_t hKernel,
                                         ze_cache_config_flags_t flags) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSetCacheConfig(hKernel, flags) + trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelGetProperties(ze_kernel_handle_t hKernel,
                                                ze_kernel_properties_t *pKernelProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    if (pKernelProperties == nullptr) {
        ss << ", pKernelProperties: nullptr";
    } else {
        ss << ", pKernelProperties {";
        ss << "stype: " << pKernelProperties->stype;
        ss << ", pNext: " << pKernelProperties->pNext;
        ss << ", numKernelArgs: " << pKernelProperties->numKernelArgs;
        ss << ", requiredGroupSizeX: " << pKernelProperties->requiredGroupSizeX;
        ss << ", requiredGroupSizeY: " << pKernelProperties->requiredGroupSizeY;
        ss << ", requiredGroupSizeZ: " << pKernelProperties->requiredGroupSizeZ;
        ss << ", requiredNumSubGroups: " << pKernelProperties->requiredNumSubGroups;
        ss << ", requiredSubgroupSize: " << pKernelProperties->requiredSubgroupSize;
        ss << ", maxSubgroupSize: " << pKernelProperties->maxSubgroupSize;
        ss << ", maxNumSubgroups: " << pKernelProperties->maxNumSubgroups;
        ss << ", localMemSize: " << pKernelProperties->localMemSize;
        ss << ", privateMemSize: " << pKernelProperties->privateMemSize;
        ss << ", spillMemSize: " << pKernelProperties->spillMemSize;
        ss << ", uuid {";
        ss << "kid: ";
        ss << trace_u8_array_hex(pKernelProperties->uuid.kid, sizeof(pKernelProperties->uuid.kid))
                  .str();
        ss << ", mid: ";
        ss << trace_u8_array_hex(pKernelProperties->uuid.mid, sizeof(pKernelProperties->uuid.mid))
                  .str();
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelGetProperties(ze_kernel_handle_t hKernel,
                                        ze_kernel_properties_t *pKernelProperties) {
    TRACE_EVENT_BEGIN("API", "zeKernelGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelGetProperties(hKernel, pKernelProperties) + "..\n";
}
inline void trace_zeKernelGetProperties(ze_result_t ret,
                                        ze_kernel_handle_t hKernel,
                                        ze_kernel_properties_t *pKernelProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelGetProperties(hKernel, pKernelProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelGetName(ze_kernel_handle_t hKernel, size_t *pSize, char *pName) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelGetName(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    if (pName == nullptr) {
        ss << ", pName: nullptr";
    } else {
        ss << ", pName: " << pName;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelGetName(ze_kernel_handle_t hKernel, size_t *pSize, char *pName) {
    TRACE_EVENT_BEGIN("API", "zeKernelGetName");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelGetName(hKernel, pSize, pName) + "..\n";
}
inline void
trace_zeKernelGetName(ze_result_t ret, ze_kernel_handle_t hKernel, size_t *pSize, char *pName) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelGetName(hKernel, pSize, pName) + trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendLaunchKernel(ze_command_list_handle_t hCommandList,
                                                          ze_kernel_handle_t hKernel,
                                                          const ze_group_count_t *pLaunchFuncArgs,
                                                          ze_event_handle_t hSignalEvent,
                                                          uint32_t numWaitEvents,
                                                          ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendLaunchKernel(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", hKernel: " << hKernel;
    if (pLaunchFuncArgs == nullptr) {
        ss << ", pLaunchFuncArgs: nullptr";
    } else {
        ss << ", pLaunchFuncArgs {";
        ss << "groupCountX: " << pLaunchFuncArgs->groupCountX;
        ss << ", groupCountY: " << pLaunchFuncArgs->groupCountY;
        ss << ", groupCountZ: " << pLaunchFuncArgs->groupCountZ;
        ss << "}";
    }
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendLaunchKernel(ze_command_list_handle_t hCommandList,
                                                  ze_kernel_handle_t hKernel,
                                                  const ze_group_count_t *pLaunchFuncArgs,
                                                  ze_event_handle_t hSignalEvent,
                                                  uint32_t numWaitEvents,
                                                  ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendLaunchKernel");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendLaunchKernel(hCommandList,
                                                            hKernel,
                                                            pLaunchFuncArgs,
                                                            hSignalEvent,
                                                            numWaitEvents,
                                                            phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendLaunchKernel(ze_result_t ret,
                                                  ze_command_list_handle_t hCommandList,
                                                  ze_kernel_handle_t hKernel,
                                                  const ze_group_count_t *pLaunchFuncArgs,
                                                  ze_event_handle_t hSignalEvent,
                                                  uint32_t numWaitEvents,
                                                  ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendLaunchKernel(hCommandList,
                                                            hKernel,
                                                            pLaunchFuncArgs,
                                                            hSignalEvent,
                                                            numWaitEvents,
                                                            phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListAppendLaunchCooperativeKernel(ze_command_list_handle_t hCommandList,
                                                  ze_kernel_handle_t hKernel,
                                                  const ze_group_count_t *pLaunchFuncArgs,
                                                  ze_event_handle_t hSignalEvent,
                                                  uint32_t numWaitEvents,
                                                  ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendLaunchCooperativeKernel(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", hKernel: " << hKernel;
    if (pLaunchFuncArgs == nullptr) {
        ss << ", pLaunchFuncArgs: nullptr";
    } else {
        ss << ", pLaunchFuncArgs {";
        ss << "groupCountX: " << pLaunchFuncArgs->groupCountX;
        ss << ", groupCountY: " << pLaunchFuncArgs->groupCountY;
        ss << ", groupCountZ: " << pLaunchFuncArgs->groupCountZ;
        ss << "}";
    }
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeCommandListAppendLaunchCooperativeKernel(ze_command_list_handle_t hCommandList,
                                                 ze_kernel_handle_t hKernel,
                                                 const ze_group_count_t *pLaunchFuncArgs,
                                                 ze_event_handle_t hSignalEvent,
                                                 uint32_t numWaitEvents,
                                                 ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendLaunchCooperativeKernel");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendLaunchCooperativeKernel(hCommandList,
                                                                       hKernel,
                                                                       pLaunchFuncArgs,
                                                                       hSignalEvent,
                                                                       numWaitEvents,
                                                                       phWaitEvents) +
                         "..\n";
}
inline void
trace_zeCommandListAppendLaunchCooperativeKernel(ze_result_t ret,
                                                 ze_command_list_handle_t hCommandList,
                                                 ze_kernel_handle_t hKernel,
                                                 const ze_group_count_t *pLaunchFuncArgs,
                                                 ze_event_handle_t hSignalEvent,
                                                 uint32_t numWaitEvents,
                                                 ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendLaunchCooperativeKernel(hCommandList,
                                                                       hKernel,
                                                                       pLaunchFuncArgs,
                                                                       hSignalEvent,
                                                                       numWaitEvents,
                                                                       phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListAppendLaunchKernelIndirect(ze_command_list_handle_t hCommandList,
                                               ze_kernel_handle_t hKernel,
                                               const ze_group_count_t *pLaunchArgumentsBuffer,
                                               ze_event_handle_t hSignalEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendLaunchKernelIndirect(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", hKernel: " << hKernel;
    if (pLaunchArgumentsBuffer == nullptr) {
        ss << ", pLaunchArgumentsBuffer: nullptr";
    } else {
        ss << ", pLaunchArgumentsBuffer {";
        ss << "groupCountX: " << pLaunchArgumentsBuffer->groupCountX;
        ss << ", groupCountY: " << pLaunchArgumentsBuffer->groupCountY;
        ss << ", groupCountZ: " << pLaunchArgumentsBuffer->groupCountZ;
        ss << "}";
    }
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeCommandListAppendLaunchKernelIndirect(ze_command_list_handle_t hCommandList,
                                              ze_kernel_handle_t hKernel,
                                              const ze_group_count_t *pLaunchArgumentsBuffer,
                                              ze_event_handle_t hSignalEvent,
                                              uint32_t numWaitEvents,
                                              ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendLaunchKernelIndirect");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendLaunchKernelIndirect(hCommandList,
                                                                    hKernel,
                                                                    pLaunchArgumentsBuffer,
                                                                    hSignalEvent,
                                                                    numWaitEvents,
                                                                    phWaitEvents) +
                         "..\n";
}
inline void
trace_zeCommandListAppendLaunchKernelIndirect(ze_result_t ret,
                                              ze_command_list_handle_t hCommandList,
                                              ze_kernel_handle_t hKernel,
                                              const ze_group_count_t *pLaunchArgumentsBuffer,
                                              ze_event_handle_t hSignalEvent,
                                              uint32_t numWaitEvents,
                                              ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendLaunchKernelIndirect(hCommandList,
                                                                    hKernel,
                                                                    pLaunchArgumentsBuffer,
                                                                    hSignalEvent,
                                                                    numWaitEvents,
                                                                    phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeCommandListAppendLaunchMultipleKernelsIndirect(
    ze_command_list_handle_t hCommandList,
    uint32_t numKernels,
    ze_kernel_handle_t *phKernels,
    const uint32_t *pCountBuffer,
    const ze_group_count_t *pLaunchArgumentsBuffer,
    ze_event_handle_t hSignalEvent,
    uint32_t numWaitEvents,
    ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid()
       << "] zeCommandListAppendLaunchMultipleKernelsIndirect(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", numKernels: " << numKernels;
    if (phKernels == nullptr) {
        ss << ", phKernels: nullptr";
    } else {
        ss << ", phKernels: " << *phKernels;
    }
    if (pCountBuffer == nullptr) {
        ss << ", pCountBuffer: nullptr";
    } else {
        ss << ", pCountBuffer: " << *pCountBuffer;
    }
    if (pLaunchArgumentsBuffer == nullptr) {
        ss << ", pLaunchArgumentsBuffer: nullptr";
    } else {
        ss << ", pLaunchArgumentsBuffer {";
        ss << "groupCountX: " << pLaunchArgumentsBuffer->groupCountX;
        ss << ", groupCountY: " << pLaunchArgumentsBuffer->groupCountY;
        ss << ", groupCountZ: " << pLaunchArgumentsBuffer->groupCountZ;
        ss << "}";
    }
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendLaunchMultipleKernelsIndirect(
    ze_command_list_handle_t hCommandList,
    uint32_t numKernels,
    ze_kernel_handle_t *phKernels,
    const uint32_t *pCountBuffer,
    const ze_group_count_t *pLaunchArgumentsBuffer,
    ze_event_handle_t hSignalEvent,
    uint32_t numWaitEvents,
    ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendLaunchMultipleKernelsIndirect");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendLaunchMultipleKernelsIndirect(hCommandList,
                                                                             numKernels,
                                                                             phKernels,
                                                                             pCountBuffer,
                                                                             pLaunchArgumentsBuffer,
                                                                             hSignalEvent,
                                                                             numWaitEvents,
                                                                             phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendLaunchMultipleKernelsIndirect(
    ze_result_t ret,
    ze_command_list_handle_t hCommandList,
    uint32_t numKernels,
    ze_kernel_handle_t *phKernels,
    const uint32_t *pCountBuffer,
    const ze_group_count_t *pLaunchArgumentsBuffer,
    ze_event_handle_t hSignalEvent,
    uint32_t numWaitEvents,
    ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendLaunchMultipleKernelsIndirect(hCommandList,
                                                                             numKernels,
                                                                             phKernels,
                                                                             pCountBuffer,
                                                                             pLaunchArgumentsBuffer,
                                                                             hSignalEvent,
                                                                             numWaitEvents,
                                                                             phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeContextMakeMemoryResident(ze_context_handle_t hContext,
                                                      ze_device_handle_t hDevice,
                                                      void *ptr,
                                                      size_t size) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeContextMakeMemoryResident(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", ptr: " << ptr;
    ss << ", size: " << size;
    ss << ")";
    return ss.str();
}
inline void trace_zeContextMakeMemoryResident(ze_context_handle_t hContext,
                                              ze_device_handle_t hDevice,
                                              void *ptr,
                                              size_t size) {
    TRACE_EVENT_BEGIN("API", "zeContextMakeMemoryResident");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextMakeMemoryResident(hContext, hDevice, ptr, size) + "..\n";
}
inline void trace_zeContextMakeMemoryResident(ze_result_t ret,
                                              ze_context_handle_t hContext,
                                              ze_device_handle_t hDevice,
                                              void *ptr,
                                              size_t size) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextMakeMemoryResident(hContext, hDevice, ptr, size) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeContextEvictMemory(ze_context_handle_t hContext,
                                               ze_device_handle_t hDevice,
                                               void *ptr,
                                               size_t size) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeContextEvictMemory(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", ptr: " << ptr;
    ss << ", size: " << size;
    ss << ")";
    return ss.str();
}
inline void trace_zeContextEvictMemory(ze_context_handle_t hContext,
                                       ze_device_handle_t hDevice,
                                       void *ptr,
                                       size_t size) {
    TRACE_EVENT_BEGIN("API", "zeContextEvictMemory");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextEvictMemory(hContext, hDevice, ptr, size) + "..\n";
}
inline void trace_zeContextEvictMemory(ze_result_t ret,
                                       ze_context_handle_t hContext,
                                       ze_device_handle_t hDevice,
                                       void *ptr,
                                       size_t size) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextEvictMemory(hContext, hDevice, ptr, size) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeContextMakeImageResident(ze_context_handle_t hContext,
                                                     ze_device_handle_t hDevice,
                                                     ze_image_handle_t hImage) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeContextMakeImageResident(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", hImage: " << hImage;
    ss << ")";
    return ss.str();
}
inline void trace_zeContextMakeImageResident(ze_context_handle_t hContext,
                                             ze_device_handle_t hDevice,
                                             ze_image_handle_t hImage) {
    TRACE_EVENT_BEGIN("API", "zeContextMakeImageResident");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextMakeImageResident(hContext, hDevice, hImage) + "..\n";
}
inline void trace_zeContextMakeImageResident(ze_result_t ret,
                                             ze_context_handle_t hContext,
                                             ze_device_handle_t hDevice,
                                             ze_image_handle_t hImage) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextMakeImageResident(hContext, hDevice, hImage) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeContextEvictImage(ze_context_handle_t hContext,
                                              ze_device_handle_t hDevice,
                                              ze_image_handle_t hImage) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeContextEvictImage(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", hImage: " << hImage;
    ss << ")";
    return ss.str();
}
inline void trace_zeContextEvictImage(ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      ze_image_handle_t hImage) {
    TRACE_EVENT_BEGIN("API", "zeContextEvictImage");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextEvictImage(hContext, hDevice, hImage) + "..\n";
}
inline void trace_zeContextEvictImage(ze_result_t ret,
                                      ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      ze_image_handle_t hImage) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeContextEvictImage(hContext, hDevice, hImage) + trace_ze_result_t(ret);
}

inline std::string _trace_zeSamplerCreate(ze_context_handle_t hContext,
                                          ze_device_handle_t hDevice,
                                          const ze_sampler_desc_t *desc,
                                          ze_sampler_handle_t *phSampler) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeSamplerCreate(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", addressMode: " << desc->addressMode;
        ss << ", filterMode: " << desc->filterMode;
        ss << ", isNormalized: " << desc->isNormalized;
        ss << "}";
    }
    if (phSampler == nullptr) {
        ss << ", phSampler: nullptr";
    } else {
        ss << ", phSampler: " << *phSampler;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeSamplerCreate(ze_context_handle_t hContext,
                                  ze_device_handle_t hDevice,
                                  const ze_sampler_desc_t *desc,
                                  ze_sampler_handle_t *phSampler) {
    TRACE_EVENT_BEGIN("API", "zeSamplerCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zeSamplerCreate(hContext, hDevice, desc, phSampler) + "..\n";
}
inline void trace_zeSamplerCreate(ze_result_t ret,
                                  ze_context_handle_t hContext,
                                  ze_device_handle_t hDevice,
                                  const ze_sampler_desc_t *desc,
                                  ze_sampler_handle_t *phSampler) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeSamplerCreate(hContext, hDevice, desc, phSampler) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeSamplerDestroy(ze_sampler_handle_t hSampler) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeSamplerDestroy(";
    ss << std::hex << std::showbase;
    ss << "hSampler: " << hSampler;
    ss << ")";
    return ss.str();
}
inline void trace_zeSamplerDestroy(ze_sampler_handle_t hSampler) {
    TRACE_EVENT_BEGIN("API", "zeSamplerDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zeSamplerDestroy(hSampler) + "..\n";
}
inline void trace_zeSamplerDestroy(ze_result_t ret, ze_sampler_handle_t hSampler) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeSamplerDestroy(hSampler) + trace_ze_result_t(ret);
}

inline std::string _trace_zeVirtualMemReserve(ze_context_handle_t hContext,
                                              const void *pStart,
                                              size_t size,
                                              void **pptr) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeVirtualMemReserve(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", pStart: " << pStart;
    ss << ", size: " << size;
    ss << ", pptr: " << pptr;
    ss << ")";
    return ss.str();
}
inline void trace_zeVirtualMemReserve(ze_context_handle_t hContext,
                                      const void *pStart,
                                      size_t size,
                                      void **pptr) {
    TRACE_EVENT_BEGIN("API", "zeVirtualMemReserve");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemReserve(hContext, pStart, size, pptr) + "..\n";
}
inline void trace_zeVirtualMemReserve(ze_result_t ret,
                                      ze_context_handle_t hContext,
                                      const void *pStart,
                                      size_t size,
                                      void **pptr) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemReserve(hContext, pStart, size, pptr) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeVirtualMemFree(ze_context_handle_t hContext, const void *ptr, size_t size) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeVirtualMemFree(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", ptr: " << ptr;
    ss << ", size: " << size;
    ss << ")";
    return ss.str();
}
inline void trace_zeVirtualMemFree(ze_context_handle_t hContext, const void *ptr, size_t size) {
    TRACE_EVENT_BEGIN("API", "zeVirtualMemFree");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemFree(hContext, ptr, size) + "..\n";
}
inline void trace_zeVirtualMemFree(ze_result_t ret,
                                   ze_context_handle_t hContext,
                                   const void *ptr,
                                   size_t size) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemFree(hContext, ptr, size) + trace_ze_result_t(ret);
}

inline std::string _trace_zeVirtualMemQueryPageSize(ze_context_handle_t hContext,
                                                    ze_device_handle_t hDevice,
                                                    size_t size,
                                                    size_t *pagesize) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeVirtualMemQueryPageSize(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", size: " << size;
    if (pagesize == nullptr) {
        ss << ", pagesize: nullptr";
    } else {
        ss << ", pagesize: " << *pagesize;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeVirtualMemQueryPageSize(ze_context_handle_t hContext,
                                            ze_device_handle_t hDevice,
                                            size_t size,
                                            size_t *pagesize) {
    TRACE_EVENT_BEGIN("API", "zeVirtualMemQueryPageSize");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemQueryPageSize(hContext, hDevice, size, pagesize) + "..\n";
}
inline void trace_zeVirtualMemQueryPageSize(ze_result_t ret,
                                            ze_context_handle_t hContext,
                                            ze_device_handle_t hDevice,
                                            size_t size,
                                            size_t *pagesize) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemQueryPageSize(hContext, hDevice, size, pagesize) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zePhysicalMemCreate(ze_context_handle_t hContext,
                                              ze_device_handle_t hDevice,
                                              ze_physical_mem_desc_t *desc,
                                              ze_physical_mem_handle_t *phPhysicalMemory) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zePhysicalMemCreate(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << ", size: " << desc->size;
        ss << "}";
    }
    if (phPhysicalMemory == nullptr) {
        ss << ", phPhysicalMemory: nullptr";
    } else {
        ss << ", phPhysicalMemory: " << *phPhysicalMemory;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zePhysicalMemCreate(ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      ze_physical_mem_desc_t *desc,
                                      ze_physical_mem_handle_t *phPhysicalMemory) {
    TRACE_EVENT_BEGIN("API", "zePhysicalMemCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zePhysicalMemCreate(hContext, hDevice, desc, phPhysicalMemory) + "..\n";
}
inline void trace_zePhysicalMemCreate(ze_result_t ret,
                                      ze_context_handle_t hContext,
                                      ze_device_handle_t hDevice,
                                      ze_physical_mem_desc_t *desc,
                                      ze_physical_mem_handle_t *phPhysicalMemory) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zePhysicalMemCreate(hContext, hDevice, desc, phPhysicalMemory) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zePhysicalMemDestroy(ze_context_handle_t hContext,
                                               ze_physical_mem_handle_t hPhysicalMemory) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zePhysicalMemDestroy(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hPhysicalMemory: " << hPhysicalMemory;
    ss << ")";
    return ss.str();
}
inline void trace_zePhysicalMemDestroy(ze_context_handle_t hContext,
                                       ze_physical_mem_handle_t hPhysicalMemory) {
    TRACE_EVENT_BEGIN("API", "zePhysicalMemDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zePhysicalMemDestroy(hContext, hPhysicalMemory) + "..\n";
}
inline void trace_zePhysicalMemDestroy(ze_result_t ret,
                                       ze_context_handle_t hContext,
                                       ze_physical_mem_handle_t hPhysicalMemory) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zePhysicalMemDestroy(hContext, hPhysicalMemory) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeVirtualMemMap(ze_context_handle_t hContext,
                                          const void *ptr,
                                          size_t size,
                                          ze_physical_mem_handle_t hPhysicalMemory,
                                          size_t offset,
                                          ze_memory_access_attribute_t access) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeVirtualMemMap(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", ptr: " << ptr;
    ss << ", size: " << size;
    ss << ", hPhysicalMemory: " << hPhysicalMemory;
    ss << ", offset: " << offset;
    ss << ", access: " << access;
    ss << ")";
    return ss.str();
}
inline void trace_zeVirtualMemMap(ze_context_handle_t hContext,
                                  const void *ptr,
                                  size_t size,
                                  ze_physical_mem_handle_t hPhysicalMemory,
                                  size_t offset,
                                  ze_memory_access_attribute_t access) {
    TRACE_EVENT_BEGIN("API", "zeVirtualMemMap");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemMap(hContext, ptr, size, hPhysicalMemory, offset, access) +
                         "..\n";
}
inline void trace_zeVirtualMemMap(ze_result_t ret,
                                  ze_context_handle_t hContext,
                                  const void *ptr,
                                  size_t size,
                                  ze_physical_mem_handle_t hPhysicalMemory,
                                  size_t offset,
                                  ze_memory_access_attribute_t access) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemMap(hContext, ptr, size, hPhysicalMemory, offset, access) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeVirtualMemUnmap(ze_context_handle_t hContext, const void *ptr, size_t size) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeVirtualMemUnmap(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", ptr: " << ptr;
    ss << ", size: " << size;
    ss << ")";
    return ss.str();
}
inline void trace_zeVirtualMemUnmap(ze_context_handle_t hContext, const void *ptr, size_t size) {
    TRACE_EVENT_BEGIN("API", "zeVirtualMemUnmap");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemUnmap(hContext, ptr, size) + "..\n";
}
inline void trace_zeVirtualMemUnmap(ze_result_t ret,
                                    ze_context_handle_t hContext,
                                    const void *ptr,
                                    size_t size) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemUnmap(hContext, ptr, size) + trace_ze_result_t(ret);
}

inline std::string _trace_zeVirtualMemSetAccessAttribute(ze_context_handle_t hContext,
                                                         const void *ptr,
                                                         size_t size,
                                                         ze_memory_access_attribute_t access) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeVirtualMemSetAccessAttribute(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", ptr: " << ptr;
    ss << ", size: " << size;
    ss << ", access: " << access;
    ss << ")";
    return ss.str();
}
inline void trace_zeVirtualMemSetAccessAttribute(ze_context_handle_t hContext,
                                                 const void *ptr,
                                                 size_t size,
                                                 ze_memory_access_attribute_t access) {
    TRACE_EVENT_BEGIN("API", "zeVirtualMemSetAccessAttribute");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemSetAccessAttribute(hContext, ptr, size, access) + "..\n";
}
inline void trace_zeVirtualMemSetAccessAttribute(ze_result_t ret,
                                                 ze_context_handle_t hContext,
                                                 const void *ptr,
                                                 size_t size,
                                                 ze_memory_access_attribute_t access) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemSetAccessAttribute(hContext, ptr, size, access) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeVirtualMemGetAccessAttribute(ze_context_handle_t hContext,
                                                         const void *ptr,
                                                         size_t size,
                                                         ze_memory_access_attribute_t *access,
                                                         size_t *outSize) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeVirtualMemGetAccessAttribute(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", ptr: " << ptr;
    ss << ", size: " << size;
    if (access == nullptr) {
        ss << ", access: nullptr";
    } else {
        ss << ", access: " << *access;
    }
    if (outSize == nullptr) {
        ss << ", outSize: nullptr";
    } else {
        ss << ", outSize: " << *outSize;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeVirtualMemGetAccessAttribute(ze_context_handle_t hContext,
                                                 const void *ptr,
                                                 size_t size,
                                                 ze_memory_access_attribute_t *access,
                                                 size_t *outSize) {
    TRACE_EVENT_BEGIN("API", "zeVirtualMemGetAccessAttribute");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemGetAccessAttribute(hContext, ptr, size, access, outSize) +
                         "..\n";
}
inline void trace_zeVirtualMemGetAccessAttribute(ze_result_t ret,
                                                 ze_context_handle_t hContext,
                                                 const void *ptr,
                                                 size_t size,
                                                 ze_memory_access_attribute_t *access,
                                                 size_t *outSize) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeVirtualMemGetAccessAttribute(hContext, ptr, size, access, outSize) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelSetGlobalOffsetExp(ze_kernel_handle_t hKernel,
                                                     uint32_t offsetX,
                                                     uint32_t offsetY,
                                                     uint32_t offsetZ) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelSetGlobalOffsetExp(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    ss << ", offsetX: " << offsetX;
    ss << ", offsetY: " << offsetY;
    ss << ", offsetZ: " << offsetZ;
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelSetGlobalOffsetExp(ze_kernel_handle_t hKernel,
                                             uint32_t offsetX,
                                             uint32_t offsetY,
                                             uint32_t offsetZ) {
    TRACE_EVENT_BEGIN("API", "zeKernelSetGlobalOffsetExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSetGlobalOffsetExp(hKernel, offsetX, offsetY, offsetZ) + "..\n";
}
inline void trace_zeKernelSetGlobalOffsetExp(ze_result_t ret,
                                             ze_kernel_handle_t hKernel,
                                             uint32_t offsetX,
                                             uint32_t offsetY,
                                             uint32_t offsetZ) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSetGlobalOffsetExp(hKernel, offsetX, offsetY, offsetZ) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceReserveCacheExt(ze_device_handle_t hDevice,
                                                  size_t cacheLevel,
                                                  size_t cacheReservationSize) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceReserveCacheExt(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ", cacheLevel: " << cacheLevel;
    ss << ", cacheReservationSize: " << cacheReservationSize;
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceReserveCacheExt(ze_device_handle_t hDevice,
                                          size_t cacheLevel,
                                          size_t cacheReservationSize) {
    TRACE_EVENT_BEGIN("API", "zeDeviceReserveCacheExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceReserveCacheExt(hDevice, cacheLevel, cacheReservationSize) +
                         "..\n";
}
inline void trace_zeDeviceReserveCacheExt(ze_result_t ret,
                                          ze_device_handle_t hDevice,
                                          size_t cacheLevel,
                                          size_t cacheReservationSize) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceReserveCacheExt(hDevice, cacheLevel, cacheReservationSize) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceSetCacheAdviceExt(ze_device_handle_t hDevice,
                                                    void *ptr,
                                                    size_t regionSize,
                                                    ze_cache_ext_region_t cacheRegion) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceSetCacheAdviceExt(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ", ptr: " << ptr;
    ss << ", regionSize: " << regionSize;
    ss << ", cacheRegion: " << cacheRegion;
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceSetCacheAdviceExt(ze_device_handle_t hDevice,
                                            void *ptr,
                                            size_t regionSize,
                                            ze_cache_ext_region_t cacheRegion) {
    TRACE_EVENT_BEGIN("API", "zeDeviceSetCacheAdviceExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceSetCacheAdviceExt(hDevice, ptr, regionSize, cacheRegion) +
                         "..\n";
}
inline void trace_zeDeviceSetCacheAdviceExt(ze_result_t ret,
                                            ze_device_handle_t hDevice,
                                            void *ptr,
                                            size_t regionSize,
                                            ze_cache_ext_region_t cacheRegion) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceSetCacheAdviceExt(hDevice, ptr, regionSize, cacheRegion) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeEventQueryTimestampsExp(ze_event_handle_t hEvent,
                                                    ze_device_handle_t hDevice,
                                                    uint32_t *pCount,
                                                    ze_kernel_timestamp_result_t *pTimestamps) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventQueryTimestampsExp(";
    ss << std::hex << std::showbase;
    ss << "hEvent: " << hEvent;
    ss << ", hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pTimestamps == nullptr) {
        ss << ", pTimestamps: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pTimestamps[";
        if (count > 0) {
            ss << "{ global {";
            ss << "kernelStart: " << pTimestamps[0].global.kernelStart;
            ss << ", kernelEnd: " << pTimestamps[0].global.kernelEnd;
            ss << "}, context {";
            ss << "kernelStart: " << pTimestamps[0].context.kernelStart;
            ss << ", kernelEnd: " << pTimestamps[0].context.kernelEnd;
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
inline void trace_zeEventQueryTimestampsExp(ze_event_handle_t hEvent,
                                            ze_device_handle_t hDevice,
                                            uint32_t *pCount,
                                            ze_kernel_timestamp_result_t *pTimestamps) {
    TRACE_EVENT_BEGIN("API", "zeEventQueryTimestampsExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventQueryTimestampsExp(hEvent, hDevice, pCount, pTimestamps) +
                         "..\n";
}
inline void trace_zeEventQueryTimestampsExp(ze_result_t ret,
                                            ze_event_handle_t hEvent,
                                            ze_device_handle_t hDevice,
                                            uint32_t *pCount,
                                            ze_kernel_timestamp_result_t *pTimestamps) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventQueryTimestampsExp(hEvent, hDevice, pCount, pTimestamps) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeImageGetMemoryPropertiesExp(ze_image_handle_t hImage,
                                     ze_image_memory_properties_exp_t *pMemoryProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeImageGetMemoryPropertiesExp(";
    ss << std::hex << std::showbase;
    ss << "hImage: " << hImage;
    if (pMemoryProperties == nullptr) {
        ss << ", pMemoryProperties: nullptr";
    } else {
        ss << ", pMemoryProperties {";
        ss << "stype: " << pMemoryProperties->stype;
        ss << ", pNext: " << pMemoryProperties->pNext;
        ss << ", size: " << pMemoryProperties->size;
        ss << ", rowPitch: " << pMemoryProperties->rowPitch;
        ss << ", slicePitch: " << pMemoryProperties->slicePitch;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeImageGetMemoryPropertiesExp(ze_image_handle_t hImage,
                                    ze_image_memory_properties_exp_t *pMemoryProperties) {
    TRACE_EVENT_BEGIN("API", "zeImageGetMemoryPropertiesExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageGetMemoryPropertiesExp(hImage, pMemoryProperties) + "..\n";
}
inline void
trace_zeImageGetMemoryPropertiesExp(ze_result_t ret,
                                    ze_image_handle_t hImage,
                                    ze_image_memory_properties_exp_t *pMemoryProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageGetMemoryPropertiesExp(hImage, pMemoryProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeImageViewCreateExt(ze_context_handle_t hContext,
                                               ze_device_handle_t hDevice,
                                               const ze_image_desc_t *desc,
                                               ze_image_handle_t hImage,
                                               ze_image_handle_t *phImageView) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeImageViewCreateExt(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << ", type: " << desc->type;
        ss << ", format {";
        ss << "layout: " << desc->format.layout;
        ss << ", type: " << desc->format.type;
        ss << ", x: " << desc->format.x;
        ss << ", y: " << desc->format.y;
        ss << ", z: " << desc->format.z;
        ss << ", w: " << desc->format.w;
        ss << "}";
        ss << ", width: " << desc->width;
        ss << ", height: " << desc->height;
        ss << ", depth: " << desc->depth;
        ss << ", arraylevels: " << desc->arraylevels;
        ss << ", miplevels: " << desc->miplevels;
        ss << "}";
    }
    ss << ", hImage: " << hImage;
    if (phImageView == nullptr) {
        ss << ", phImageView: nullptr";
    } else {
        ss << ", phImageView: " << *phImageView;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeImageViewCreateExt(ze_context_handle_t hContext,
                                       ze_device_handle_t hDevice,
                                       const ze_image_desc_t *desc,
                                       ze_image_handle_t hImage,
                                       ze_image_handle_t *phImageView) {
    TRACE_EVENT_BEGIN("API", "zeImageViewCreateExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageViewCreateExt(hContext, hDevice, desc, hImage, phImageView) +
                         "..\n";
}
inline void trace_zeImageViewCreateExt(ze_result_t ret,
                                       ze_context_handle_t hContext,
                                       ze_device_handle_t hDevice,
                                       const ze_image_desc_t *desc,
                                       ze_image_handle_t hImage,
                                       ze_image_handle_t *phImageView) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageViewCreateExt(hContext, hDevice, desc, hImage, phImageView) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeImageViewCreateExp(ze_context_handle_t hContext,
                                               ze_device_handle_t hDevice,
                                               const ze_image_desc_t *desc,
                                               ze_image_handle_t hImage,
                                               ze_image_handle_t *phImageView) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeImageViewCreateExp(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << ", type: " << desc->type;
        ss << ", format {";
        ss << "layout: " << desc->format.layout;
        ss << ", type: " << desc->format.type;
        ss << ", x: " << desc->format.x;
        ss << ", y: " << desc->format.y;
        ss << ", z: " << desc->format.z;
        ss << ", w: " << desc->format.w;
        ss << "}";
        ss << ", width: " << desc->width;
        ss << ", height: " << desc->height;
        ss << ", depth: " << desc->depth;
        ss << ", arraylevels: " << desc->arraylevels;
        ss << ", miplevels: " << desc->miplevels;
        ss << "}";
    }
    ss << ", hImage: " << hImage;
    if (phImageView == nullptr) {
        ss << ", phImageView: nullptr";
    } else {
        ss << ", phImageView: " << *phImageView;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeImageViewCreateExp(ze_context_handle_t hContext,
                                       ze_device_handle_t hDevice,
                                       const ze_image_desc_t *desc,
                                       ze_image_handle_t hImage,
                                       ze_image_handle_t *phImageView) {
    TRACE_EVENT_BEGIN("API", "zeImageViewCreateExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageViewCreateExp(hContext, hDevice, desc, hImage, phImageView) +
                         "..\n";
}
inline void trace_zeImageViewCreateExp(ze_result_t ret,
                                       ze_context_handle_t hContext,
                                       ze_device_handle_t hDevice,
                                       const ze_image_desc_t *desc,
                                       ze_image_handle_t hImage,
                                       ze_image_handle_t *phImageView) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageViewCreateExp(hContext, hDevice, desc, hImage, phImageView) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeKernelSchedulingHintExp(ze_kernel_handle_t hKernel,
                                                    ze_scheduling_hint_exp_desc_t *pHint) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeKernelSchedulingHintExp(";
    ss << std::hex << std::showbase;
    ss << "hKernel: " << hKernel;
    if (pHint == nullptr) {
        ss << ", pHint: nullptr";
    } else {
        ss << ", pHint {";
        ss << "stype: " << pHint->stype;
        ss << ", pNext: " << pHint->pNext;
        ss << ", flags: " << pHint->flags;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeKernelSchedulingHintExp(ze_kernel_handle_t hKernel,
                                            ze_scheduling_hint_exp_desc_t *pHint) {
    TRACE_EVENT_BEGIN("API", "zeKernelSchedulingHintExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSchedulingHintExp(hKernel, pHint) + "..\n";
}
inline void trace_zeKernelSchedulingHintExp(ze_result_t ret,
                                            ze_kernel_handle_t hKernel,
                                            ze_scheduling_hint_exp_desc_t *pHint) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeKernelSchedulingHintExp(hKernel, pHint) + trace_ze_result_t(ret);
}

inline std::string _trace_zeDevicePciGetPropertiesExt(ze_device_handle_t hDevice,
                                                      ze_pci_ext_properties_t *pPciProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDevicePciGetPropertiesExt(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pPciProperties == nullptr) {
        ss << ", pPciProperties: nullptr";
    } else {
        ss << ", pPciProperties {";
        ss << "stype: " << pPciProperties->stype;
        ss << ", pNext: " << pPciProperties->pNext;
        ss << ", address {";
        ss << "domain: " << pPciProperties->address.domain;
        ss << ", bus: " << pPciProperties->address.bus;
        ss << ", device: " << pPciProperties->address.device;
        ss << ", function: " << pPciProperties->address.function;
        ss << "}";
        ss << ", maxSpeed {";
        ss << "genVersion: " << pPciProperties->maxSpeed.genVersion;
        ss << ", width: " << pPciProperties->maxSpeed.width;
        ss << ", maxBandwidth: " << pPciProperties->maxSpeed.maxBandwidth;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDevicePciGetPropertiesExt(ze_device_handle_t hDevice,
                                              ze_pci_ext_properties_t *pPciProperties) {
    TRACE_EVENT_BEGIN("API", "zeDevicePciGetPropertiesExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDevicePciGetPropertiesExt(hDevice, pPciProperties) + "..\n";
}
inline void trace_zeDevicePciGetPropertiesExt(ze_result_t ret,
                                              ze_device_handle_t hDevice,
                                              ze_pci_ext_properties_t *pPciProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDevicePciGetPropertiesExt(hDevice, pPciProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListAppendImageCopyToMemoryExt(ze_command_list_handle_t hCommandList,
                                               void *dstptr,
                                               ze_image_handle_t hSrcImage,
                                               const ze_image_region_t *pSrcRegion,
                                               uint32_t destRowPitch,
                                               uint32_t destSlicePitch,
                                               ze_event_handle_t hSignalEvent,
                                               uint32_t numWaitEvents,
                                               ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendImageCopyToMemoryExt(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", dstptr: " << dstptr;
    ss << ", hSrcImage: " << hSrcImage;
    if (pSrcRegion == nullptr) {
        ss << ", pSrcRegion: nullptr";
    } else {
        ss << ", pSrcRegion {";
        ss << "originX: " << pSrcRegion->originX;
        ss << ", originY: " << pSrcRegion->originY;
        ss << ", originZ: " << pSrcRegion->originZ;
        ss << ", width: " << pSrcRegion->width;
        ss << ", height: " << pSrcRegion->height;
        ss << ", depth: " << pSrcRegion->depth;
        ss << "}";
    }
    ss << ", destRowPitch: " << destRowPitch;
    ss << ", destSlicePitch: " << destSlicePitch;
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendImageCopyToMemoryExt(ze_command_list_handle_t hCommandList,
                                                          void *dstptr,
                                                          ze_image_handle_t hSrcImage,
                                                          const ze_image_region_t *pSrcRegion,
                                                          uint32_t destRowPitch,
                                                          uint32_t destSlicePitch,
                                                          ze_event_handle_t hSignalEvent,
                                                          uint32_t numWaitEvents,
                                                          ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendImageCopyToMemoryExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendImageCopyToMemoryExt(hCommandList,
                                                                    dstptr,
                                                                    hSrcImage,
                                                                    pSrcRegion,
                                                                    destRowPitch,
                                                                    destSlicePitch,
                                                                    hSignalEvent,
                                                                    numWaitEvents,
                                                                    phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendImageCopyToMemoryExt(ze_result_t ret,
                                                          ze_command_list_handle_t hCommandList,
                                                          void *dstptr,
                                                          ze_image_handle_t hSrcImage,
                                                          const ze_image_region_t *pSrcRegion,
                                                          uint32_t destRowPitch,
                                                          uint32_t destSlicePitch,
                                                          ze_event_handle_t hSignalEvent,
                                                          uint32_t numWaitEvents,
                                                          ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendImageCopyToMemoryExt(hCommandList,
                                                                    dstptr,
                                                                    hSrcImage,
                                                                    pSrcRegion,
                                                                    destRowPitch,
                                                                    destSlicePitch,
                                                                    hSignalEvent,
                                                                    numWaitEvents,
                                                                    phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListAppendImageCopyFromMemoryExt(ze_command_list_handle_t hCommandList,
                                                 ze_image_handle_t hDstImage,
                                                 const void *srcptr,
                                                 const ze_image_region_t *pDstRegion,
                                                 uint32_t srcRowPitch,
                                                 uint32_t srcSlicePitch,
                                                 ze_event_handle_t hSignalEvent,
                                                 uint32_t numWaitEvents,
                                                 ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListAppendImageCopyFromMemoryExt(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", hDstImage: " << hDstImage;
    ss << ", srcptr: " << srcptr;
    if (pDstRegion == nullptr) {
        ss << ", pDstRegion: nullptr";
    } else {
        ss << ", pDstRegion {";
        ss << "originX: " << pDstRegion->originX;
        ss << ", originY: " << pDstRegion->originY;
        ss << ", originZ: " << pDstRegion->originZ;
        ss << ", width: " << pDstRegion->width;
        ss << ", height: " << pDstRegion->height;
        ss << ", depth: " << pDstRegion->depth;
        ss << "}";
    }
    ss << ", srcRowPitch: " << srcRowPitch;
    ss << ", srcSlicePitch: " << srcSlicePitch;
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListAppendImageCopyFromMemoryExt(ze_command_list_handle_t hCommandList,
                                                            ze_image_handle_t hDstImage,
                                                            const void *srcptr,
                                                            const ze_image_region_t *pDstRegion,
                                                            uint32_t srcRowPitch,
                                                            uint32_t srcSlicePitch,
                                                            ze_event_handle_t hSignalEvent,
                                                            uint32_t numWaitEvents,
                                                            ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListAppendImageCopyFromMemoryExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendImageCopyFromMemoryExt(hCommandList,
                                                                      hDstImage,
                                                                      srcptr,
                                                                      pDstRegion,
                                                                      srcRowPitch,
                                                                      srcSlicePitch,
                                                                      hSignalEvent,
                                                                      numWaitEvents,
                                                                      phWaitEvents) +
                         "..\n";
}
inline void trace_zeCommandListAppendImageCopyFromMemoryExt(ze_result_t ret,
                                                            ze_command_list_handle_t hCommandList,
                                                            ze_image_handle_t hDstImage,
                                                            const void *srcptr,
                                                            const ze_image_region_t *pDstRegion,
                                                            uint32_t srcRowPitch,
                                                            uint32_t srcSlicePitch,
                                                            ze_event_handle_t hSignalEvent,
                                                            uint32_t numWaitEvents,
                                                            ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListAppendImageCopyFromMemoryExt(hCommandList,
                                                                      hDstImage,
                                                                      srcptr,
                                                                      pDstRegion,
                                                                      srcRowPitch,
                                                                      srcSlicePitch,
                                                                      hSignalEvent,
                                                                      numWaitEvents,
                                                                      phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeImageGetAllocPropertiesExt(ze_context_handle_t hContext,
                                    ze_image_handle_t hImage,
                                    ze_image_allocation_ext_properties_t *pImageAllocProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeImageGetAllocPropertiesExt(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hImage: " << hImage;
    if (pImageAllocProperties == nullptr) {
        ss << ", pImageAllocProperties: nullptr";
    } else {
        ss << ", pImageAllocProperties {";
        ss << "stype: " << pImageAllocProperties->stype;
        ss << ", pNext: " << pImageAllocProperties->pNext;
        ss << ", id: " << pImageAllocProperties->id;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeImageGetAllocPropertiesExt(ze_context_handle_t hContext,
                                   ze_image_handle_t hImage,
                                   ze_image_allocation_ext_properties_t *pImageAllocProperties) {
    TRACE_EVENT_BEGIN("API", "zeImageGetAllocPropertiesExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageGetAllocPropertiesExt(hContext, hImage, pImageAllocProperties) +
                         "..\n";
}
inline void
trace_zeImageGetAllocPropertiesExt(ze_result_t ret,
                                   ze_context_handle_t hContext,
                                   ze_image_handle_t hImage,
                                   ze_image_allocation_ext_properties_t *pImageAllocProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageGetAllocPropertiesExt(hContext, hImage, pImageAllocProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeModuleInspectLinkageExt(ze_linkage_inspection_ext_desc_t *pInspectDesc,
                                                    uint32_t numModules,
                                                    ze_module_handle_t *phModules,
                                                    ze_module_build_log_handle_t *phLog) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeModuleInspectLinkageExt(";
    ss << std::hex << std::showbase;
    if (pInspectDesc == nullptr) {
        ss << ", pInspectDesc: nullptr";
    } else {
        ss << ", pInspectDesc {";
        ss << "stype: " << pInspectDesc->stype;
        ss << ", pNext: " << pInspectDesc->pNext;
        ss << ", flags: " << pInspectDesc->flags;
        ss << "}";
    }
    ss << ", numModules: " << numModules;
    if (phModules == nullptr) {
        ss << ", phModules: nullptr";
    } else {
        ss << ", phModules: " << *phModules;
    }
    if (phLog == nullptr) {
        ss << ", phLog: nullptr";
    } else {
        ss << ", phLog: " << *phLog;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeModuleInspectLinkageExt(ze_linkage_inspection_ext_desc_t *pInspectDesc,
                                            uint32_t numModules,
                                            ze_module_handle_t *phModules,
                                            ze_module_build_log_handle_t *phLog) {
    TRACE_EVENT_BEGIN("API", "zeModuleInspectLinkageExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleInspectLinkageExt(pInspectDesc, numModules, phModules, phLog) +
                         "..\n";
}
inline void trace_zeModuleInspectLinkageExt(ze_result_t ret,
                                            ze_linkage_inspection_ext_desc_t *pInspectDesc,
                                            uint32_t numModules,
                                            ze_module_handle_t *phModules,
                                            ze_module_build_log_handle_t *phLog) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeModuleInspectLinkageExt(pInspectDesc, numModules, phModules, phLog) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeMemFreeExt(ze_context_handle_t hContext,
                                       const ze_memory_free_ext_desc_t *pMemFreeDesc,
                                       void *ptr) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemFreeExt(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    if (pMemFreeDesc == nullptr) {
        ss << ", pMemFreeDesc: nullptr";
    } else {
        ss << ", pMemFreeDesc {";
        ss << "stype: " << pMemFreeDesc->stype;
        ss << ", pNext: " << pMemFreeDesc->pNext;
        ss << ", freePolicy: " << pMemFreeDesc->freePolicy;
        ss << "}";
    }
    ss << ", ptr: " << ptr;
    ss << ")";
    return ss.str();
}
inline void trace_zeMemFreeExt(ze_context_handle_t hContext,
                               const ze_memory_free_ext_desc_t *pMemFreeDesc,
                               void *ptr) {
    TRACE_EVENT_BEGIN("API", "zeMemFreeExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemFreeExt(hContext, pMemFreeDesc, ptr) + "..\n";
}
inline void trace_zeMemFreeExt(ze_result_t ret,
                               ze_context_handle_t hContext,
                               const ze_memory_free_ext_desc_t *pMemFreeDesc,
                               void *ptr) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemFreeExt(hContext, pMemFreeDesc, ptr) + trace_ze_result_t(ret);
}

inline std::string _trace_zeFabricVertexGetExp(ze_driver_handle_t hDriver,
                                               uint32_t *pCount,
                                               ze_fabric_vertex_handle_t *phVertices) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeFabricVertexGetExp(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phVertices == nullptr) {
        ss << ", phVertices: nullptr";
    } else {
        ss << ", phVertices: " << *phVertices;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeFabricVertexGetExp(ze_driver_handle_t hDriver,
                                       uint32_t *pCount,
                                       ze_fabric_vertex_handle_t *phVertices) {
    TRACE_EVENT_BEGIN("API", "zeFabricVertexGetExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricVertexGetExp(hDriver, pCount, phVertices) + "..\n";
}
inline void trace_zeFabricVertexGetExp(ze_result_t ret,
                                       ze_driver_handle_t hDriver,
                                       uint32_t *pCount,
                                       ze_fabric_vertex_handle_t *phVertices) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricVertexGetExp(hDriver, pCount, phVertices) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeFabricVertexGetSubVerticesExp(ze_fabric_vertex_handle_t hVertex,
                                       uint32_t *pCount,
                                       ze_fabric_vertex_handle_t *phSubvertices) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeFabricVertexGetSubVerticesExp(";
    ss << std::hex << std::showbase;
    ss << "hVertex: " << hVertex;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phSubvertices == nullptr) {
        ss << ", phSubvertices: nullptr";
    } else {
        ss << ", phSubvertices: " << *phSubvertices;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeFabricVertexGetSubVerticesExp(ze_fabric_vertex_handle_t hVertex,
                                                  uint32_t *pCount,
                                                  ze_fabric_vertex_handle_t *phSubvertices) {
    TRACE_EVENT_BEGIN("API", "zeFabricVertexGetSubVerticesExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricVertexGetSubVerticesExp(hVertex, pCount, phSubvertices) +
                         "..\n";
}
inline void trace_zeFabricVertexGetSubVerticesExp(ze_result_t ret,
                                                  ze_fabric_vertex_handle_t hVertex,
                                                  uint32_t *pCount,
                                                  ze_fabric_vertex_handle_t *phSubvertices) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricVertexGetSubVerticesExp(hVertex, pCount, phSubvertices) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeFabricVertexGetPropertiesExp(ze_fabric_vertex_handle_t hVertex,
                                      ze_fabric_vertex_exp_properties_t *pVertexProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeFabricVertexGetPropertiesExp(";
    ss << std::hex << std::showbase;
    ss << "hVertex: " << hVertex;
    if (pVertexProperties == nullptr) {
        ss << ", pVertexProperties: nullptr";
    } else {
        ss << ", pVertexProperties {";
        ss << "stype: " << pVertexProperties->stype;
        ss << ", pNext: " << pVertexProperties->pNext;
        ss << ", uuid {";
        ss << "id: ";
        ss << trace_u8_array_hex(pVertexProperties->uuid.id, sizeof(pVertexProperties->uuid.id))
                  .str();
        ss << "}";
        ss << ", type: " << pVertexProperties->type;
        ss << ", remote: " << pVertexProperties->remote;
        ss << ", address {";
        ss << "domain: " << pVertexProperties->address.domain;
        ss << ", bus: " << pVertexProperties->address.bus;
        ss << ", device: " << pVertexProperties->address.device;
        ss << ", function: " << pVertexProperties->address.function;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeFabricVertexGetPropertiesExp(ze_fabric_vertex_handle_t hVertex,
                                     ze_fabric_vertex_exp_properties_t *pVertexProperties) {
    TRACE_EVENT_BEGIN("API", "zeFabricVertexGetPropertiesExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricVertexGetPropertiesExp(hVertex, pVertexProperties) + "..\n";
}
inline void
trace_zeFabricVertexGetPropertiesExp(ze_result_t ret,
                                     ze_fabric_vertex_handle_t hVertex,
                                     ze_fabric_vertex_exp_properties_t *pVertexProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricVertexGetPropertiesExp(hVertex, pVertexProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeFabricVertexGetDeviceExp(ze_fabric_vertex_handle_t hVertex,
                                                     ze_device_handle_t *phDevice) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeFabricVertexGetDeviceExp(";
    ss << std::hex << std::showbase;
    ss << "hVertex: " << hVertex;
    if (phDevice == nullptr) {
        ss << ", phDevice: nullptr";
    } else {
        ss << ", phDevice: " << *phDevice;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeFabricVertexGetDeviceExp(ze_fabric_vertex_handle_t hVertex,
                                             ze_device_handle_t *phDevice) {
    TRACE_EVENT_BEGIN("API", "zeFabricVertexGetDeviceExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricVertexGetDeviceExp(hVertex, phDevice) + "..\n";
}
inline void trace_zeFabricVertexGetDeviceExp(ze_result_t ret,
                                             ze_fabric_vertex_handle_t hVertex,
                                             ze_device_handle_t *phDevice) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricVertexGetDeviceExp(hVertex, phDevice) + trace_ze_result_t(ret);
}

inline std::string _trace_zeDeviceGetFabricVertexExp(ze_device_handle_t hDevice,
                                                     ze_fabric_vertex_handle_t *phVertex) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDeviceGetFabricVertexExp(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (phVertex == nullptr) {
        ss << ", phVertex: nullptr";
    } else {
        ss << ", phVertex: " << *phVertex;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeDeviceGetFabricVertexExp(ze_device_handle_t hDevice,
                                             ze_fabric_vertex_handle_t *phVertex) {
    TRACE_EVENT_BEGIN("API", "zeDeviceGetFabricVertexExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetFabricVertexExp(hDevice, phVertex) + "..\n";
}
inline void trace_zeDeviceGetFabricVertexExp(ze_result_t ret,
                                             ze_device_handle_t hDevice,
                                             ze_fabric_vertex_handle_t *phVertex) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDeviceGetFabricVertexExp(hDevice, phVertex) + trace_ze_result_t(ret);
}

inline std::string _trace_zeFabricEdgeGetExp(ze_fabric_vertex_handle_t hVertexA,
                                             ze_fabric_vertex_handle_t hVertexB,
                                             uint32_t *pCount,
                                             ze_fabric_edge_handle_t *phEdges) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeFabricEdgeGetExp(";
    ss << std::hex << std::showbase;
    ss << "hVertexA: " << hVertexA;
    ss << ", hVertexB: " << hVertexB;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phEdges == nullptr) {
        ss << ", phEdges: nullptr";
    } else {
        ss << ", phEdges: " << *phEdges;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeFabricEdgeGetExp(ze_fabric_vertex_handle_t hVertexA,
                                     ze_fabric_vertex_handle_t hVertexB,
                                     uint32_t *pCount,
                                     ze_fabric_edge_handle_t *phEdges) {
    TRACE_EVENT_BEGIN("API", "zeFabricEdgeGetExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricEdgeGetExp(hVertexA, hVertexB, pCount, phEdges) + "..\n";
}
inline void trace_zeFabricEdgeGetExp(ze_result_t ret,
                                     ze_fabric_vertex_handle_t hVertexA,
                                     ze_fabric_vertex_handle_t hVertexB,
                                     uint32_t *pCount,
                                     ze_fabric_edge_handle_t *phEdges) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricEdgeGetExp(hVertexA, hVertexB, pCount, phEdges) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeFabricEdgeGetVerticesExp(ze_fabric_edge_handle_t hEdge,
                                                     ze_fabric_vertex_handle_t *phVertexA,
                                                     ze_fabric_vertex_handle_t *phVertexB) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeFabricEdgeGetVerticesExp(";
    ss << std::hex << std::showbase;
    ss << "hEdge: " << hEdge;
    if (phVertexA == nullptr) {
        ss << ", phVertexA: nullptr";
    } else {
        ss << ", phVertexA: " << *phVertexA;
    }
    if (phVertexB == nullptr) {
        ss << ", phVertexB: nullptr";
    } else {
        ss << ", phVertexB: " << *phVertexB;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeFabricEdgeGetVerticesExp(ze_fabric_edge_handle_t hEdge,
                                             ze_fabric_vertex_handle_t *phVertexA,
                                             ze_fabric_vertex_handle_t *phVertexB) {
    TRACE_EVENT_BEGIN("API", "zeFabricEdgeGetVerticesExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricEdgeGetVerticesExp(hEdge, phVertexA, phVertexB) + "..\n";
}
inline void trace_zeFabricEdgeGetVerticesExp(ze_result_t ret,
                                             ze_fabric_edge_handle_t hEdge,
                                             ze_fabric_vertex_handle_t *phVertexA,
                                             ze_fabric_vertex_handle_t *phVertexB) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricEdgeGetVerticesExp(hEdge, phVertexA, phVertexB) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeFabricEdgeGetPropertiesExp(ze_fabric_edge_handle_t hEdge,
                                    ze_fabric_edge_exp_properties_t *pEdgeProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeFabricEdgeGetPropertiesExp(";
    ss << std::hex << std::showbase;
    ss << "hEdge: " << hEdge;
    if (pEdgeProperties == nullptr) {
        ss << ", pEdgeProperties: nullptr";
    } else {
        ss << ", pEdgeProperties {";
        ss << "stype: " << pEdgeProperties->stype;
        ss << ", pNext: " << pEdgeProperties->pNext;
        ss << ", uuid {";
        ss << "id: ";
        ss << trace_u8_array_hex(pEdgeProperties->uuid.id, sizeof(pEdgeProperties->uuid.id)).str();
        ss << "}";
        ss << ", model: ";
        ss << trace_ascii_array(pEdgeProperties->model, sizeof(pEdgeProperties->model)).str();
        ss << ", bandwidth: " << pEdgeProperties->bandwidth;
        ss << ", bandwidthUnit: " << pEdgeProperties->bandwidthUnit;
        ss << ", latency: " << pEdgeProperties->latency;
        ss << ", latencyUnit: " << pEdgeProperties->latencyUnit;
        ss << ", duplexity: " << pEdgeProperties->duplexity;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeFabricEdgeGetPropertiesExp(ze_fabric_edge_handle_t hEdge,
                                               ze_fabric_edge_exp_properties_t *pEdgeProperties) {
    TRACE_EVENT_BEGIN("API", "zeFabricEdgeGetPropertiesExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricEdgeGetPropertiesExp(hEdge, pEdgeProperties) + "..\n";
}
inline void trace_zeFabricEdgeGetPropertiesExp(ze_result_t ret,
                                               ze_fabric_edge_handle_t hEdge,
                                               ze_fabric_edge_exp_properties_t *pEdgeProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeFabricEdgeGetPropertiesExp(hEdge, pEdgeProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeEventQueryKernelTimestampsExt(
    ze_event_handle_t hEvent,
    ze_device_handle_t hDevice,
    uint32_t *pCount,
    ze_event_query_kernel_timestamps_results_ext_properties_t *pResults) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeEventQueryKernelTimestampsExt(";
    ss << std::hex << std::showbase;
    ss << "hEvent: " << hEvent;
    ss << ", hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pResults == nullptr) {
        ss << ", pResults: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pResults[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pResults[0].stype;
            ss << ", pNext: " << pResults[0].pNext;
            if (pResults[0].pKernelTimestampsBuffer == nullptr) {
                ss << ", pKernelTimestampsBuffer: nullptr";
            } else {
                ss << ", pKernelTimestampsBuffer {";
                ss << ", global {";
                ss << "kernelStart: " << pResults[0].pKernelTimestampsBuffer->global.kernelStart;
                ss << ", kernelEnd: " << pResults[0].pKernelTimestampsBuffer->global.kernelEnd;
                ss << "}";
                ss << ", context {";
                ss << "kernelStart: " << pResults[0].pKernelTimestampsBuffer->context.kernelStart;
                ss << ", kernelEnd: " << pResults[0].pKernelTimestampsBuffer->context.kernelEnd;
                ss << "}";
                ss << "}";
            }
            if (pResults[0].pSynchronizedTimestampsBuffer == nullptr) {
                ss << ", pSynchronizedTimestampsBuffer: nullptr";
            } else {
                ss << ", pSynchronizedTimestampsBuffer {";
                ss << ", global {";
                ss << "kernelStart: "
                   << pResults[0].pSynchronizedTimestampsBuffer->global.kernelStart;
                ss << ", kernelEnd: "
                   << pResults[0].pSynchronizedTimestampsBuffer->global.kernelEnd;
                ss << "}";
                ss << ", context {";
                ss << "kernelStart: "
                   << pResults[0].pSynchronizedTimestampsBuffer->context.kernelStart;
                ss << ", kernelEnd: "
                   << pResults[0].pSynchronizedTimestampsBuffer->context.kernelEnd;
                ss << "}";
                ss << "}";
            }
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
inline void trace_zeEventQueryKernelTimestampsExt(
    ze_event_handle_t hEvent,
    ze_device_handle_t hDevice,
    uint32_t *pCount,
    ze_event_query_kernel_timestamps_results_ext_properties_t *pResults) {
    TRACE_EVENT_BEGIN("API", "zeEventQueryKernelTimestampsExt");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventQueryKernelTimestampsExt(hEvent, hDevice, pCount, pResults) +
                         "..\n";
}
inline void trace_zeEventQueryKernelTimestampsExt(
    ze_result_t ret,
    ze_event_handle_t hEvent,
    ze_device_handle_t hDevice,
    uint32_t *pCount,
    ze_event_query_kernel_timestamps_results_ext_properties_t *pResults) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeEventQueryKernelTimestampsExt(hEvent, hDevice, pCount, pResults) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeRTASBuilderCreateExp(ze_driver_handle_t hDriver,
                                                 const ze_rtas_builder_exp_desc_t *pDescriptor,
                                                 ze_rtas_builder_exp_handle_t *phBuilder) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeRTASBuilderCreateExp(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    if (pDescriptor == nullptr) {
        ss << ", pDescriptor: nullptr";
    } else {
        ss << ", pDescriptor {";
        ss << "stype: " << pDescriptor->stype;
        ss << ", pNext: " << pDescriptor->pNext;
        ss << ", builderVersion: " << pDescriptor->builderVersion;
        ss << "}";
    }
    if (phBuilder == nullptr) {
        ss << ", phBuilder: nullptr";
    } else {
        ss << ", phBuilder: " << *phBuilder;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeRTASBuilderCreateExp(ze_driver_handle_t hDriver,
                                         const ze_rtas_builder_exp_desc_t *pDescriptor,
                                         ze_rtas_builder_exp_handle_t *phBuilder) {
    TRACE_EVENT_BEGIN("API", "zeRTASBuilderCreateExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASBuilderCreateExp(hDriver, pDescriptor, phBuilder) + "..\n";
}
inline void trace_zeRTASBuilderCreateExp(ze_result_t ret,
                                         ze_driver_handle_t hDriver,
                                         const ze_rtas_builder_exp_desc_t *pDescriptor,
                                         ze_rtas_builder_exp_handle_t *phBuilder) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASBuilderCreateExp(hDriver, pDescriptor, phBuilder) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeRTASBuilderGetBuildPropertiesExp(
    ze_rtas_builder_exp_handle_t hBuilder,
    const ze_rtas_builder_build_op_exp_desc_t *pBuildOpDescriptor,
    ze_rtas_builder_exp_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeRTASBuilderGetBuildPropertiesExp(";
    ss << std::hex << std::showbase;
    ss << "hBuilder: " << hBuilder;
    if (pBuildOpDescriptor == nullptr) {
        ss << ", pBuildOpDescriptor: nullptr";
    } else {
        ss << ", pBuildOpDescriptor {";
        ss << "stype: " << pBuildOpDescriptor->stype;
        ss << ", pNext: " << pBuildOpDescriptor->pNext;
        ss << ", rtasFormat: " << pBuildOpDescriptor->rtasFormat;
        ss << ", buildQuality: " << pBuildOpDescriptor->buildQuality;
        ss << ", buildFlags: " << pBuildOpDescriptor->buildFlags;
        ss << ", ppGeometries: " << pBuildOpDescriptor->ppGeometries;
        ss << ", numGeometries: " << pBuildOpDescriptor->numGeometries;
        ss << "}";
    }
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", flags: " << pProperties->flags;
        ss << ", rtasBufferSizeBytesExpected: " << pProperties->rtasBufferSizeBytesExpected;
        ss << ", rtasBufferSizeBytesMaxRequired: " << pProperties->rtasBufferSizeBytesMaxRequired;
        ss << ", scratchBufferSizeBytes: " << pProperties->scratchBufferSizeBytes;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeRTASBuilderGetBuildPropertiesExp(
    ze_rtas_builder_exp_handle_t hBuilder,
    const ze_rtas_builder_build_op_exp_desc_t *pBuildOpDescriptor,
    ze_rtas_builder_exp_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zeRTASBuilderGetBuildPropertiesExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASBuilderGetBuildPropertiesExp(hBuilder,
                                                               pBuildOpDescriptor,
                                                               pProperties) +
                         "..\n";
}
inline void trace_zeRTASBuilderGetBuildPropertiesExp(
    ze_result_t ret,
    ze_rtas_builder_exp_handle_t hBuilder,
    const ze_rtas_builder_build_op_exp_desc_t *pBuildOpDescriptor,
    ze_rtas_builder_exp_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASBuilderGetBuildPropertiesExp(hBuilder,
                                                               pBuildOpDescriptor,
                                                               pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeDriverRTASFormatCompatibilityCheckExp(ze_driver_handle_t hDriver,
                                               ze_rtas_format_exp_t rtasFormatA,
                                               ze_rtas_format_exp_t rtasFormatB) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeDriverRTASFormatCompatibilityCheckExp(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    ss << ", rtasFormatA: " << rtasFormatA;
    ss << ", rtasFormatB: " << rtasFormatB;
    ss << ")";
    return ss.str();
}
inline void trace_zeDriverRTASFormatCompatibilityCheckExp(ze_driver_handle_t hDriver,
                                                          ze_rtas_format_exp_t rtasFormatA,
                                                          ze_rtas_format_exp_t rtasFormatB) {
    TRACE_EVENT_BEGIN("API", "zeDriverRTASFormatCompatibilityCheckExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverRTASFormatCompatibilityCheckExp(hDriver,
                                                                    rtasFormatA,
                                                                    rtasFormatB) +
                         "..\n";
}
inline void trace_zeDriverRTASFormatCompatibilityCheckExp(ze_result_t ret,
                                                          ze_driver_handle_t hDriver,
                                                          ze_rtas_format_exp_t rtasFormatA,
                                                          ze_rtas_format_exp_t rtasFormatB) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeDriverRTASFormatCompatibilityCheckExp(hDriver,
                                                                    rtasFormatA,
                                                                    rtasFormatB) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeRTASBuilderBuildExp(ze_rtas_builder_exp_handle_t hBuilder,
                             const ze_rtas_builder_build_op_exp_desc_t *pBuildOpDescriptor,
                             void *pScratchBuffer,
                             size_t scratchBufferSizeBytes,
                             void *pRtasBuffer,
                             size_t rtasBufferSizeBytes,
                             ze_rtas_parallel_operation_exp_handle_t hParallelOperation,
                             void *pBuildUserPtr,
                             ze_rtas_aabb_exp_t *pBounds,
                             size_t *pRtasBufferSizeBytes) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeRTASBuilderBuildExp(";
    ss << std::hex << std::showbase;
    ss << "hBuilder: " << hBuilder;
    if (pBuildOpDescriptor == nullptr) {
        ss << ", pBuildOpDescriptor: nullptr";
    } else {
        ss << ", pBuildOpDescriptor {";
        ss << "stype: " << pBuildOpDescriptor->stype;
        ss << ", pNext: " << pBuildOpDescriptor->pNext;
        ss << ", rtasFormat: " << pBuildOpDescriptor->rtasFormat;
        ss << ", buildQuality: " << pBuildOpDescriptor->buildQuality;
        ss << ", buildFlags: " << pBuildOpDescriptor->buildFlags;
        ss << ", ppGeometries: " << pBuildOpDescriptor->ppGeometries;
        ss << ", numGeometries: " << pBuildOpDescriptor->numGeometries;
        ss << "}";
    }
    ss << ", pScratchBuffer: " << pScratchBuffer;
    ss << ", scratchBufferSizeBytes: " << scratchBufferSizeBytes;
    ss << ", pRtasBuffer: " << pRtasBuffer;
    ss << ", rtasBufferSizeBytes: " << rtasBufferSizeBytes;
    ss << ", hParallelOperation: " << hParallelOperation;
    ss << ", pBuildUserPtr: " << pBuildUserPtr;
    if (pBounds == nullptr) {
        ss << ", pBounds: nullptr";
    } else {
        ss << ", pBounds {";
        ss << ", lower {";
        ss << "x: " << pBounds->lower.x;
        ss << ", y: " << pBounds->lower.y;
        ss << ", z: " << pBounds->lower.z;
        ss << "}";
        ss << ", upper {";
        ss << "x: " << pBounds->upper.x;
        ss << ", y: " << pBounds->upper.y;
        ss << ", z: " << pBounds->upper.z;
        ss << "}";
        ss << "}";
    }
    if (pRtasBufferSizeBytes == nullptr) {
        ss << ", pRtasBufferSizeBytes: nullptr";
    } else {
        ss << ", pRtasBufferSizeBytes: " << *pRtasBufferSizeBytes;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeRTASBuilderBuildExp(ze_rtas_builder_exp_handle_t hBuilder,
                            const ze_rtas_builder_build_op_exp_desc_t *pBuildOpDescriptor,
                            void *pScratchBuffer,
                            size_t scratchBufferSizeBytes,
                            void *pRtasBuffer,
                            size_t rtasBufferSizeBytes,
                            ze_rtas_parallel_operation_exp_handle_t hParallelOperation,
                            void *pBuildUserPtr,
                            ze_rtas_aabb_exp_t *pBounds,
                            size_t *pRtasBufferSizeBytes) {
    TRACE_EVENT_BEGIN("API", "zeRTASBuilderBuildExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASBuilderBuildExp(hBuilder,
                                                  pBuildOpDescriptor,
                                                  pScratchBuffer,
                                                  scratchBufferSizeBytes,
                                                  pRtasBuffer,
                                                  rtasBufferSizeBytes,
                                                  hParallelOperation,
                                                  pBuildUserPtr,
                                                  pBounds,
                                                  pRtasBufferSizeBytes) +
                         "..\n";
}
inline void
trace_zeRTASBuilderBuildExp(ze_result_t ret,
                            ze_rtas_builder_exp_handle_t hBuilder,
                            const ze_rtas_builder_build_op_exp_desc_t *pBuildOpDescriptor,
                            void *pScratchBuffer,
                            size_t scratchBufferSizeBytes,
                            void *pRtasBuffer,
                            size_t rtasBufferSizeBytes,
                            ze_rtas_parallel_operation_exp_handle_t hParallelOperation,
                            void *pBuildUserPtr,
                            ze_rtas_aabb_exp_t *pBounds,
                            size_t *pRtasBufferSizeBytes) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASBuilderBuildExp(hBuilder,
                                                  pBuildOpDescriptor,
                                                  pScratchBuffer,
                                                  scratchBufferSizeBytes,
                                                  pRtasBuffer,
                                                  rtasBufferSizeBytes,
                                                  hParallelOperation,
                                                  pBuildUserPtr,
                                                  pBounds,
                                                  pRtasBufferSizeBytes) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeRTASBuilderDestroyExp(ze_rtas_builder_exp_handle_t hBuilder) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeRTASBuilderDestroyExp(";
    ss << std::hex << std::showbase;
    ss << "hBuilder: " << hBuilder;
    ss << ")";
    return ss.str();
}
inline void trace_zeRTASBuilderDestroyExp(ze_rtas_builder_exp_handle_t hBuilder) {
    TRACE_EVENT_BEGIN("API", "zeRTASBuilderDestroyExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASBuilderDestroyExp(hBuilder) + "..\n";
}
inline void trace_zeRTASBuilderDestroyExp(ze_result_t ret, ze_rtas_builder_exp_handle_t hBuilder) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASBuilderDestroyExp(hBuilder) + trace_ze_result_t(ret);
}

inline std::string _trace_zeRTASParallelOperationCreateExp(
    ze_driver_handle_t hDriver,
    ze_rtas_parallel_operation_exp_handle_t *phParallelOperation) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeRTASParallelOperationCreateExp(";
    ss << std::hex << std::showbase;
    ss << "hDriver: " << hDriver;
    if (phParallelOperation == nullptr) {
        ss << ", phParallelOperation: nullptr";
    } else {
        ss << ", phParallelOperation: " << *phParallelOperation;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeRTASParallelOperationCreateExp(
    ze_driver_handle_t hDriver,
    ze_rtas_parallel_operation_exp_handle_t *phParallelOperation) {
    TRACE_EVENT_BEGIN("API", "zeRTASParallelOperationCreateExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASParallelOperationCreateExp(hDriver, phParallelOperation) + "..\n";
}
inline void trace_zeRTASParallelOperationCreateExp(
    ze_result_t ret,
    ze_driver_handle_t hDriver,
    ze_rtas_parallel_operation_exp_handle_t *phParallelOperation) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASParallelOperationCreateExp(hDriver, phParallelOperation) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeRTASParallelOperationGetPropertiesExp(
    ze_rtas_parallel_operation_exp_handle_t hParallelOperation,
    ze_rtas_parallel_operation_exp_properties_t *pProperties) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeRTASParallelOperationGetPropertiesExp(";
    ss << std::hex << std::showbase;
    ss << "hParallelOperation: " << hParallelOperation;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", flags: " << pProperties->flags;
        ss << ", maxConcurrency: " << pProperties->maxConcurrency;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeRTASParallelOperationGetPropertiesExp(
    ze_rtas_parallel_operation_exp_handle_t hParallelOperation,
    ze_rtas_parallel_operation_exp_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zeRTASParallelOperationGetPropertiesExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASParallelOperationGetPropertiesExp(hParallelOperation,
                                                                    pProperties) +
                         "..\n";
}
inline void trace_zeRTASParallelOperationGetPropertiesExp(
    ze_result_t ret,
    ze_rtas_parallel_operation_exp_handle_t hParallelOperation,
    ze_rtas_parallel_operation_exp_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASParallelOperationGetPropertiesExp(hParallelOperation,
                                                                    pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeRTASParallelOperationJoinExp(ze_rtas_parallel_operation_exp_handle_t hParallelOperation) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeRTASParallelOperationJoinExp(";
    ss << std::hex << std::showbase;
    ss << "hParallelOperation: " << hParallelOperation;
    ss << ")";
    return ss.str();
}
inline void
trace_zeRTASParallelOperationJoinExp(ze_rtas_parallel_operation_exp_handle_t hParallelOperation) {
    TRACE_EVENT_BEGIN("API", "zeRTASParallelOperationJoinExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASParallelOperationJoinExp(hParallelOperation) + "..\n";
}
inline void
trace_zeRTASParallelOperationJoinExp(ze_result_t ret,
                                     ze_rtas_parallel_operation_exp_handle_t hParallelOperation) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASParallelOperationJoinExp(hParallelOperation) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeRTASParallelOperationDestroyExp(
    ze_rtas_parallel_operation_exp_handle_t hParallelOperation) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeRTASParallelOperationDestroyExp(";
    ss << std::hex << std::showbase;
    ss << "hParallelOperation: " << hParallelOperation;
    ss << ")";
    return ss.str();
}
inline void trace_zeRTASParallelOperationDestroyExp(
    ze_rtas_parallel_operation_exp_handle_t hParallelOperation) {
    TRACE_EVENT_BEGIN("API", "zeRTASParallelOperationDestroyExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASParallelOperationDestroyExp(hParallelOperation) + "..\n";
}
inline void trace_zeRTASParallelOperationDestroyExp(
    ze_result_t ret,
    ze_rtas_parallel_operation_exp_handle_t hParallelOperation) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeRTASParallelOperationDestroyExp(hParallelOperation) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeMemGetPitchFor2dImage(ze_context_handle_t hContext,
                                                  ze_device_handle_t hDevice,
                                                  size_t imageWidth,
                                                  size_t imageHeight,
                                                  unsigned int elementSizeInBytes,
                                                  size_t *rowPitch) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeMemGetPitchFor2dImage(";
    ss << std::hex << std::showbase;
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", imageWidth: " << imageWidth;
    ss << ", imageHeight: " << imageHeight;
    ss << ", elementSizeInBytes: " << elementSizeInBytes;
    if (rowPitch == nullptr) {
        ss << ", rowPitch: nullptr";
    } else {
        ss << ", rowPitch: " << *rowPitch;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeMemGetPitchFor2dImage(ze_context_handle_t hContext,
                                          ze_device_handle_t hDevice,
                                          size_t imageWidth,
                                          size_t imageHeight,
                                          unsigned int elementSizeInBytes,
                                          size_t *rowPitch) {
    TRACE_EVENT_BEGIN("API", "zeMemGetPitchFor2dImage");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetPitchFor2dImage(hContext,
                                                    hDevice,
                                                    imageWidth,
                                                    imageHeight,
                                                    elementSizeInBytes,
                                                    rowPitch) +
                         "..\n";
}
inline void trace_zeMemGetPitchFor2dImage(ze_result_t ret,
                                          ze_context_handle_t hContext,
                                          ze_device_handle_t hDevice,
                                          size_t imageWidth,
                                          size_t imageHeight,
                                          unsigned int elementSizeInBytes,
                                          size_t *rowPitch) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeMemGetPitchFor2dImage(hContext,
                                                    hDevice,
                                                    imageWidth,
                                                    imageHeight,
                                                    elementSizeInBytes,
                                                    rowPitch) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zeImageGetDeviceOffsetExp(ze_image_handle_t hImage,
                                                    uint64_t *pDeviceOffset) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeImageGetDeviceOffsetExp(";
    ss << std::hex << std::showbase;
    ss << "hImage: " << hImage;
    if (pDeviceOffset == nullptr) {
        ss << ", pDeviceOffset: nullptr";
    } else {
        ss << ", pDeviceOffset: " << *pDeviceOffset;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeImageGetDeviceOffsetExp(ze_image_handle_t hImage, uint64_t *pDeviceOffset) {
    TRACE_EVENT_BEGIN("API", "zeImageGetDeviceOffsetExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageGetDeviceOffsetExp(hImage, pDeviceOffset) + "..\n";
}
inline void trace_zeImageGetDeviceOffsetExp(ze_result_t ret,
                                            ze_image_handle_t hImage,
                                            uint64_t *pDeviceOffset) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeImageGetDeviceOffsetExp(hImage, pDeviceOffset) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListCreateCloneExp(ze_command_list_handle_t hCommandList,
                                   ze_command_list_handle_t *phClonedCommandList) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListCreateCloneExp(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    if (phClonedCommandList == nullptr) {
        ss << ", phClonedCommandList: nullptr";
    } else {
        ss << ", phClonedCommandList: " << *phClonedCommandList;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListCreateCloneExp(ze_command_list_handle_t hCommandList,
                                              ze_command_list_handle_t *phClonedCommandList) {
    TRACE_EVENT_BEGIN("API", "zeCommandListCreateCloneExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListCreateCloneExp(hCommandList, phClonedCommandList) + "..\n";
}
inline void trace_zeCommandListCreateCloneExp(ze_result_t ret,
                                              ze_command_list_handle_t hCommandList,
                                              ze_command_list_handle_t *phClonedCommandList) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListCreateCloneExp(hCommandList, phClonedCommandList) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListImmediateAppendCommandListsExp(ze_command_list_handle_t hCommandListImmediate,
                                                   uint32_t numCommandLists,
                                                   ze_command_list_handle_t *phCommandLists,
                                                   ze_event_handle_t hSignalEvent,
                                                   uint32_t numWaitEvents,
                                                   ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListImmediateAppendCommandListsExp(";
    ss << std::hex << std::showbase;
    ss << "hCommandListImmediate: " << hCommandListImmediate;
    ss << ", numCommandLists: " << numCommandLists;
    if (phCommandLists == nullptr) {
        ss << ", phCommandLists: nullptr";
    } else {
        ss << ", phCommandLists: " << *phCommandLists;
    }
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeCommandListImmediateAppendCommandListsExp(ze_command_list_handle_t hCommandListImmediate,
                                                  uint32_t numCommandLists,
                                                  ze_command_list_handle_t *phCommandLists,
                                                  ze_event_handle_t hSignalEvent,
                                                  uint32_t numWaitEvents,
                                                  ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListImmediateAppendCommandListsExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListImmediateAppendCommandListsExp(hCommandListImmediate,
                                                                        numCommandLists,
                                                                        phCommandLists,
                                                                        hSignalEvent,
                                                                        numWaitEvents,
                                                                        phWaitEvents) +
                         "..\n";
}
inline void
trace_zeCommandListImmediateAppendCommandListsExp(ze_result_t ret,
                                                  ze_command_list_handle_t hCommandListImmediate,
                                                  uint32_t numCommandLists,
                                                  ze_command_list_handle_t *phCommandLists,
                                                  ze_event_handle_t hSignalEvent,
                                                  uint32_t numWaitEvents,
                                                  ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListImmediateAppendCommandListsExp(hCommandListImmediate,
                                                                        numCommandLists,
                                                                        phCommandLists,
                                                                        hSignalEvent,
                                                                        numWaitEvents,
                                                                        phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListGetNextCommandIdExp(ze_command_list_handle_t hCommandList,
                                        const ze_mutable_command_id_exp_desc_t *desc,
                                        uint64_t *pCommandId) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListGetNextCommandIdExp(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << "}";
    }
    if (pCommandId == nullptr) {
        ss << ", pCommandId: nullptr";
    } else {
        ss << ", pCommandId: " << *pCommandId;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListGetNextCommandIdExp(ze_command_list_handle_t hCommandList,
                                                   const ze_mutable_command_id_exp_desc_t *desc,
                                                   uint64_t *pCommandId) {
    TRACE_EVENT_BEGIN("API", "zeCommandListGetNextCommandIdExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListGetNextCommandIdExp(hCommandList, desc, pCommandId) +
                         "..\n";
}
inline void trace_zeCommandListGetNextCommandIdExp(ze_result_t ret,
                                                   ze_command_list_handle_t hCommandList,
                                                   const ze_mutable_command_id_exp_desc_t *desc,
                                                   uint64_t *pCommandId) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListGetNextCommandIdExp(hCommandList, desc, pCommandId) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListGetNextCommandIdWithKernelsExp(ze_command_list_handle_t hCommandList,
                                                   const ze_mutable_command_id_exp_desc_t *desc,
                                                   uint32_t numKernels,
                                                   ze_kernel_handle_t *phKernels,
                                                   uint64_t *pCommandId) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListGetNextCommandIdWithKernelsExp(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << "}";
    }
    ss << ", numKernels: " << numKernels;
    if (phKernels == nullptr) {
        ss << ", phKernels: nullptr";
    } else {
        ss << ", phKernels: " << *phKernels;
    }
    if (pCommandId == nullptr) {
        ss << ", pCommandId: nullptr";
    } else {
        ss << ", pCommandId: " << *pCommandId;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeCommandListGetNextCommandIdWithKernelsExp(ze_command_list_handle_t hCommandList,
                                                  const ze_mutable_command_id_exp_desc_t *desc,
                                                  uint32_t numKernels,
                                                  ze_kernel_handle_t *phKernels,
                                                  uint64_t *pCommandId) {
    TRACE_EVENT_BEGIN("API", "zeCommandListGetNextCommandIdWithKernelsExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListGetNextCommandIdWithKernelsExp(hCommandList,
                                                                        desc,
                                                                        numKernels,
                                                                        phKernels,
                                                                        pCommandId) +
                         "..\n";
}
inline void
trace_zeCommandListGetNextCommandIdWithKernelsExp(ze_result_t ret,
                                                  ze_command_list_handle_t hCommandList,
                                                  const ze_mutable_command_id_exp_desc_t *desc,
                                                  uint32_t numKernels,
                                                  ze_kernel_handle_t *phKernels,
                                                  uint64_t *pCommandId) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListGetNextCommandIdWithKernelsExp(hCommandList,
                                                                        desc,
                                                                        numKernels,
                                                                        phKernels,
                                                                        pCommandId) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListUpdateMutableCommandsExp(ze_command_list_handle_t hCommandList,
                                             const ze_mutable_commands_exp_desc_t *desc) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListUpdateMutableCommandsExp(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", flags: " << desc->flags;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeCommandListUpdateMutableCommandsExp(ze_command_list_handle_t hCommandList,
                                            const ze_mutable_commands_exp_desc_t *desc) {
    TRACE_EVENT_BEGIN("API", "zeCommandListUpdateMutableCommandsExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListUpdateMutableCommandsExp(hCommandList, desc) + "..\n";
}
inline void
trace_zeCommandListUpdateMutableCommandsExp(ze_result_t ret,
                                            ze_command_list_handle_t hCommandList,
                                            const ze_mutable_commands_exp_desc_t *desc) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListUpdateMutableCommandsExp(hCommandList, desc) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListUpdateMutableCommandSignalEventExp(ze_command_list_handle_t hCommandList,
                                                       uint64_t commandId,
                                                       ze_event_handle_t hSignalEvent) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListUpdateMutableCommandSignalEventExp(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", commandId: " << commandId;
    ss << ", hSignalEvent: " << hSignalEvent;
    ss << ")";
    return ss.str();
}
inline void
trace_zeCommandListUpdateMutableCommandSignalEventExp(ze_command_list_handle_t hCommandList,
                                                      uint64_t commandId,
                                                      ze_event_handle_t hSignalEvent) {
    TRACE_EVENT_BEGIN("API", "zeCommandListUpdateMutableCommandSignalEventExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListUpdateMutableCommandSignalEventExp(hCommandList,
                                                                            commandId,
                                                                            hSignalEvent) +
                         "..\n";
}
inline void
trace_zeCommandListUpdateMutableCommandSignalEventExp(ze_result_t ret,
                                                      ze_command_list_handle_t hCommandList,
                                                      uint64_t commandId,
                                                      ze_event_handle_t hSignalEvent) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListUpdateMutableCommandSignalEventExp(hCommandList,
                                                                            commandId,
                                                                            hSignalEvent) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListUpdateMutableCommandWaitEventsExp(ze_command_list_handle_t hCommandList,
                                                      uint64_t commandId,
                                                      uint32_t numWaitEvents,
                                                      ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListUpdateMutableCommandWaitEventsExp(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", commandId: " << commandId;
    ss << ", numWaitEvents: " << numWaitEvents;
    if (phWaitEvents == nullptr) {
        ss << ", phWaitEvents: nullptr";
    } else {
        ss << ", phWaitEvents: " << *phWaitEvents;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zeCommandListUpdateMutableCommandWaitEventsExp(ze_command_list_handle_t hCommandList,
                                                     uint64_t commandId,
                                                     uint32_t numWaitEvents,
                                                     ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zeCommandListUpdateMutableCommandWaitEventsExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListUpdateMutableCommandWaitEventsExp(hCommandList,
                                                                           commandId,
                                                                           numWaitEvents,
                                                                           phWaitEvents) +
                         "..\n";
}
inline void
trace_zeCommandListUpdateMutableCommandWaitEventsExp(ze_result_t ret,
                                                     ze_command_list_handle_t hCommandList,
                                                     uint64_t commandId,
                                                     uint32_t numWaitEvents,
                                                     ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListUpdateMutableCommandWaitEventsExp(hCommandList,
                                                                           commandId,
                                                                           numWaitEvents,
                                                                           phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zeCommandListUpdateMutableCommandKernelsExp(ze_command_list_handle_t hCommandList,
                                                   uint32_t numKernels,
                                                   uint64_t *pCommandId,
                                                   ze_kernel_handle_t *phKernels) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zeCommandListUpdateMutableCommandKernelsExp(";
    ss << std::hex << std::showbase;
    ss << "hCommandList: " << hCommandList;
    ss << ", numKernels: " << numKernels;
    if (pCommandId == nullptr) {
        ss << ", pCommandId: nullptr";
    } else {
        ss << ", pCommandId: " << *pCommandId;
    }
    if (phKernels == nullptr) {
        ss << ", phKernels: nullptr";
    } else {
        ss << ", phKernels: " << *phKernels;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zeCommandListUpdateMutableCommandKernelsExp(ze_command_list_handle_t hCommandList,
                                                              uint32_t numKernels,
                                                              uint64_t *pCommandId,
                                                              ze_kernel_handle_t *phKernels) {
    TRACE_EVENT_BEGIN("API", "zeCommandListUpdateMutableCommandKernelsExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListUpdateMutableCommandKernelsExp(hCommandList,
                                                                        numKernels,
                                                                        pCommandId,
                                                                        phKernels) +
                         "..\n";
}
inline void trace_zeCommandListUpdateMutableCommandKernelsExp(ze_result_t ret,
                                                              ze_command_list_handle_t hCommandList,
                                                              uint32_t numKernels,
                                                              uint64_t *pCommandId,
                                                              ze_kernel_handle_t *phKernels) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zeCommandListUpdateMutableCommandKernelsExp(hCommandList,
                                                                        numKernels,
                                                                        pCommandId,
                                                                        phKernels) +
                         trace_ze_result_t(ret);
}
