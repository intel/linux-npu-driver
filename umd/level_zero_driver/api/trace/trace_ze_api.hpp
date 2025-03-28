/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Generated header based on ze_api.h@version v1.11-r1.11.4

#pragma once

#include "vpu_driver/source/utilities/log.hpp"
#include "vpu_driver/source/utilities/trace_perfetto.hpp"

#include <iostream>
#include <level_zero/ze_api.h>
#include <sstream>

#define IS_API_TRACE() (VPU::getLogLevel() == INFO && VPU::getLogMask() & API)
#define IS_API_DDI_TRACE() (VPU::getLogLevel() == INFO && VPU::getLogMask() & API_DDI)
#define IS_API_EXT_TRACE() (VPU::getLogLevel() == INFO && VPU::getLogMask() & API_EXT)

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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeInit(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDriverGet(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeInitDrivers(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDriverGetApiVersion(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDriverGetProperties(";
    ss << "hDriver: " << hDriver;
    if (pDriverProperties == nullptr) {
        ss << ", pDriverProperties: nullptr";
    } else {
        ss << ", pDriverProperties {";
        ss << "stype: " << pDriverProperties->stype;
        ss << ", pNext: " << pDriverProperties->pNext;
        ss << ", uuid {";
        ss << "id: " << static_cast<int>(pDriverProperties->uuid.id[0]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[1]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[2]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[3]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[4]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[5]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[6]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[7]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[8]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[9]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[10]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[11]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[12]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[13]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[14]) << " "
           << static_cast<int>(pDriverProperties->uuid.id[15]);
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDriverGetIpcProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDriverGetExtensionProperties(";
    ss << "hDriver: " << hDriver;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pExtensionProperties == nullptr) {
        ss << ", pExtensionProperties: nullptr";
    } else {
        ss << ", pExtensionProperties {";
        ss << "name: " << pExtensionProperties->name[0] << pExtensionProperties->name[1]
           << pExtensionProperties->name[2] << pExtensionProperties->name[3]
           << pExtensionProperties->name[4] << pExtensionProperties->name[5]
           << pExtensionProperties->name[6] << pExtensionProperties->name[7]
           << pExtensionProperties->name[8] << pExtensionProperties->name[9]
           << pExtensionProperties->name[10] << pExtensionProperties->name[11]
           << pExtensionProperties->name[12] << pExtensionProperties->name[13]
           << pExtensionProperties->name[14] << pExtensionProperties->name[15]
           << pExtensionProperties->name[16] << pExtensionProperties->name[17]
           << pExtensionProperties->name[18] << pExtensionProperties->name[19]
           << pExtensionProperties->name[20] << pExtensionProperties->name[21]
           << pExtensionProperties->name[22] << pExtensionProperties->name[23]
           << pExtensionProperties->name[24] << pExtensionProperties->name[25]
           << pExtensionProperties->name[26] << pExtensionProperties->name[27]
           << pExtensionProperties->name[28] << pExtensionProperties->name[29]
           << pExtensionProperties->name[30] << pExtensionProperties->name[31]
           << pExtensionProperties->name[32] << pExtensionProperties->name[33]
           << pExtensionProperties->name[34] << pExtensionProperties->name[35]
           << pExtensionProperties->name[36] << pExtensionProperties->name[37]
           << pExtensionProperties->name[38] << pExtensionProperties->name[39]
           << pExtensionProperties->name[40] << pExtensionProperties->name[41]
           << pExtensionProperties->name[42] << pExtensionProperties->name[43]
           << pExtensionProperties->name[44] << pExtensionProperties->name[45]
           << pExtensionProperties->name[46] << pExtensionProperties->name[47]
           << pExtensionProperties->name[48] << pExtensionProperties->name[49]
           << pExtensionProperties->name[50] << pExtensionProperties->name[51]
           << pExtensionProperties->name[52] << pExtensionProperties->name[53]
           << pExtensionProperties->name[54] << pExtensionProperties->name[55]
           << pExtensionProperties->name[56] << pExtensionProperties->name[57]
           << pExtensionProperties->name[58] << pExtensionProperties->name[59]
           << pExtensionProperties->name[60] << pExtensionProperties->name[61]
           << pExtensionProperties->name[62] << pExtensionProperties->name[63]
           << pExtensionProperties->name[64] << pExtensionProperties->name[65]
           << pExtensionProperties->name[66] << pExtensionProperties->name[67]
           << pExtensionProperties->name[68] << pExtensionProperties->name[69]
           << pExtensionProperties->name[70] << pExtensionProperties->name[71]
           << pExtensionProperties->name[72] << pExtensionProperties->name[73]
           << pExtensionProperties->name[74] << pExtensionProperties->name[75]
           << pExtensionProperties->name[76] << pExtensionProperties->name[77]
           << pExtensionProperties->name[78] << pExtensionProperties->name[79]
           << pExtensionProperties->name[80] << pExtensionProperties->name[81]
           << pExtensionProperties->name[82] << pExtensionProperties->name[83]
           << pExtensionProperties->name[84] << pExtensionProperties->name[85]
           << pExtensionProperties->name[86] << pExtensionProperties->name[87]
           << pExtensionProperties->name[88] << pExtensionProperties->name[89]
           << pExtensionProperties->name[90] << pExtensionProperties->name[91]
           << pExtensionProperties->name[92] << pExtensionProperties->name[93]
           << pExtensionProperties->name[94] << pExtensionProperties->name[95]
           << pExtensionProperties->name[96] << pExtensionProperties->name[97]
           << pExtensionProperties->name[98] << pExtensionProperties->name[99]
           << pExtensionProperties->name[100] << pExtensionProperties->name[101]
           << pExtensionProperties->name[102] << pExtensionProperties->name[103]
           << pExtensionProperties->name[104] << pExtensionProperties->name[105]
           << pExtensionProperties->name[106] << pExtensionProperties->name[107]
           << pExtensionProperties->name[108] << pExtensionProperties->name[109]
           << pExtensionProperties->name[110] << pExtensionProperties->name[111]
           << pExtensionProperties->name[112] << pExtensionProperties->name[113]
           << pExtensionProperties->name[114] << pExtensionProperties->name[115]
           << pExtensionProperties->name[116] << pExtensionProperties->name[117]
           << pExtensionProperties->name[118] << pExtensionProperties->name[119]
           << pExtensionProperties->name[120] << pExtensionProperties->name[121]
           << pExtensionProperties->name[122] << pExtensionProperties->name[123]
           << pExtensionProperties->name[124] << pExtensionProperties->name[125]
           << pExtensionProperties->name[126] << pExtensionProperties->name[127]
           << pExtensionProperties->name[128] << pExtensionProperties->name[129]
           << pExtensionProperties->name[130] << pExtensionProperties->name[131]
           << pExtensionProperties->name[132] << pExtensionProperties->name[133]
           << pExtensionProperties->name[134] << pExtensionProperties->name[135]
           << pExtensionProperties->name[136] << pExtensionProperties->name[137]
           << pExtensionProperties->name[138] << pExtensionProperties->name[139]
           << pExtensionProperties->name[140] << pExtensionProperties->name[141]
           << pExtensionProperties->name[142] << pExtensionProperties->name[143]
           << pExtensionProperties->name[144] << pExtensionProperties->name[145]
           << pExtensionProperties->name[146] << pExtensionProperties->name[147]
           << pExtensionProperties->name[148] << pExtensionProperties->name[149]
           << pExtensionProperties->name[150] << pExtensionProperties->name[151]
           << pExtensionProperties->name[152] << pExtensionProperties->name[153]
           << pExtensionProperties->name[154] << pExtensionProperties->name[155]
           << pExtensionProperties->name[156] << pExtensionProperties->name[157]
           << pExtensionProperties->name[158] << pExtensionProperties->name[159]
           << pExtensionProperties->name[160] << pExtensionProperties->name[161]
           << pExtensionProperties->name[162] << pExtensionProperties->name[163]
           << pExtensionProperties->name[164] << pExtensionProperties->name[165]
           << pExtensionProperties->name[166] << pExtensionProperties->name[167]
           << pExtensionProperties->name[168] << pExtensionProperties->name[169]
           << pExtensionProperties->name[170] << pExtensionProperties->name[171]
           << pExtensionProperties->name[172] << pExtensionProperties->name[173]
           << pExtensionProperties->name[174] << pExtensionProperties->name[175]
           << pExtensionProperties->name[176] << pExtensionProperties->name[177]
           << pExtensionProperties->name[178] << pExtensionProperties->name[179]
           << pExtensionProperties->name[180] << pExtensionProperties->name[181]
           << pExtensionProperties->name[182] << pExtensionProperties->name[183]
           << pExtensionProperties->name[184] << pExtensionProperties->name[185]
           << pExtensionProperties->name[186] << pExtensionProperties->name[187]
           << pExtensionProperties->name[188] << pExtensionProperties->name[189]
           << pExtensionProperties->name[190] << pExtensionProperties->name[191]
           << pExtensionProperties->name[192] << pExtensionProperties->name[193]
           << pExtensionProperties->name[194] << pExtensionProperties->name[195]
           << pExtensionProperties->name[196] << pExtensionProperties->name[197]
           << pExtensionProperties->name[198] << pExtensionProperties->name[199]
           << pExtensionProperties->name[200] << pExtensionProperties->name[201]
           << pExtensionProperties->name[202] << pExtensionProperties->name[203]
           << pExtensionProperties->name[204] << pExtensionProperties->name[205]
           << pExtensionProperties->name[206] << pExtensionProperties->name[207]
           << pExtensionProperties->name[208] << pExtensionProperties->name[209]
           << pExtensionProperties->name[210] << pExtensionProperties->name[211]
           << pExtensionProperties->name[212] << pExtensionProperties->name[213]
           << pExtensionProperties->name[214] << pExtensionProperties->name[215]
           << pExtensionProperties->name[216] << pExtensionProperties->name[217]
           << pExtensionProperties->name[218] << pExtensionProperties->name[219]
           << pExtensionProperties->name[220] << pExtensionProperties->name[221]
           << pExtensionProperties->name[222] << pExtensionProperties->name[223]
           << pExtensionProperties->name[224] << pExtensionProperties->name[225]
           << pExtensionProperties->name[226] << pExtensionProperties->name[227]
           << pExtensionProperties->name[228] << pExtensionProperties->name[229]
           << pExtensionProperties->name[230] << pExtensionProperties->name[231]
           << pExtensionProperties->name[232] << pExtensionProperties->name[233]
           << pExtensionProperties->name[234] << pExtensionProperties->name[235]
           << pExtensionProperties->name[236] << pExtensionProperties->name[237]
           << pExtensionProperties->name[238] << pExtensionProperties->name[239]
           << pExtensionProperties->name[240] << pExtensionProperties->name[241]
           << pExtensionProperties->name[242] << pExtensionProperties->name[243]
           << pExtensionProperties->name[244] << pExtensionProperties->name[245]
           << pExtensionProperties->name[246] << pExtensionProperties->name[247]
           << pExtensionProperties->name[248] << pExtensionProperties->name[249]
           << pExtensionProperties->name[250] << pExtensionProperties->name[251]
           << pExtensionProperties->name[252] << pExtensionProperties->name[253]
           << pExtensionProperties->name[254] << pExtensionProperties->name[255];
        ss << ", version: " << pExtensionProperties->version;
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDriverGetExtensionFunctionAddress(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDriverGetLastErrorDescription(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGet(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetRootDevice(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetSubDevices(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetProperties(";
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
        ss << "id: " << static_cast<int>(pDeviceProperties->uuid.id[0]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[1]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[2]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[3]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[4]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[5]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[6]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[7]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[8]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[9]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[10]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[11]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[12]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[13]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[14]) << " "
           << static_cast<int>(pDeviceProperties->uuid.id[15]);
        ss << "}";
        ss << ", name: " << pDeviceProperties->name[0] << pDeviceProperties->name[1]
           << pDeviceProperties->name[2] << pDeviceProperties->name[3] << pDeviceProperties->name[4]
           << pDeviceProperties->name[5] << pDeviceProperties->name[6] << pDeviceProperties->name[7]
           << pDeviceProperties->name[8] << pDeviceProperties->name[9]
           << pDeviceProperties->name[10] << pDeviceProperties->name[11]
           << pDeviceProperties->name[12] << pDeviceProperties->name[13]
           << pDeviceProperties->name[14] << pDeviceProperties->name[15]
           << pDeviceProperties->name[16] << pDeviceProperties->name[17]
           << pDeviceProperties->name[18] << pDeviceProperties->name[19]
           << pDeviceProperties->name[20] << pDeviceProperties->name[21]
           << pDeviceProperties->name[22] << pDeviceProperties->name[23]
           << pDeviceProperties->name[24] << pDeviceProperties->name[25]
           << pDeviceProperties->name[26] << pDeviceProperties->name[27]
           << pDeviceProperties->name[28] << pDeviceProperties->name[29]
           << pDeviceProperties->name[30] << pDeviceProperties->name[31]
           << pDeviceProperties->name[32] << pDeviceProperties->name[33]
           << pDeviceProperties->name[34] << pDeviceProperties->name[35]
           << pDeviceProperties->name[36] << pDeviceProperties->name[37]
           << pDeviceProperties->name[38] << pDeviceProperties->name[39]
           << pDeviceProperties->name[40] << pDeviceProperties->name[41]
           << pDeviceProperties->name[42] << pDeviceProperties->name[43]
           << pDeviceProperties->name[44] << pDeviceProperties->name[45]
           << pDeviceProperties->name[46] << pDeviceProperties->name[47]
           << pDeviceProperties->name[48] << pDeviceProperties->name[49]
           << pDeviceProperties->name[50] << pDeviceProperties->name[51]
           << pDeviceProperties->name[52] << pDeviceProperties->name[53]
           << pDeviceProperties->name[54] << pDeviceProperties->name[55]
           << pDeviceProperties->name[56] << pDeviceProperties->name[57]
           << pDeviceProperties->name[58] << pDeviceProperties->name[59]
           << pDeviceProperties->name[60] << pDeviceProperties->name[61]
           << pDeviceProperties->name[62] << pDeviceProperties->name[63]
           << pDeviceProperties->name[64] << pDeviceProperties->name[65]
           << pDeviceProperties->name[66] << pDeviceProperties->name[67]
           << pDeviceProperties->name[68] << pDeviceProperties->name[69]
           << pDeviceProperties->name[70] << pDeviceProperties->name[71]
           << pDeviceProperties->name[72] << pDeviceProperties->name[73]
           << pDeviceProperties->name[74] << pDeviceProperties->name[75]
           << pDeviceProperties->name[76] << pDeviceProperties->name[77]
           << pDeviceProperties->name[78] << pDeviceProperties->name[79]
           << pDeviceProperties->name[80] << pDeviceProperties->name[81]
           << pDeviceProperties->name[82] << pDeviceProperties->name[83]
           << pDeviceProperties->name[84] << pDeviceProperties->name[85]
           << pDeviceProperties->name[86] << pDeviceProperties->name[87]
           << pDeviceProperties->name[88] << pDeviceProperties->name[89]
           << pDeviceProperties->name[90] << pDeviceProperties->name[91]
           << pDeviceProperties->name[92] << pDeviceProperties->name[93]
           << pDeviceProperties->name[94] << pDeviceProperties->name[95]
           << pDeviceProperties->name[96] << pDeviceProperties->name[97]
           << pDeviceProperties->name[98] << pDeviceProperties->name[99]
           << pDeviceProperties->name[100] << pDeviceProperties->name[101]
           << pDeviceProperties->name[102] << pDeviceProperties->name[103]
           << pDeviceProperties->name[104] << pDeviceProperties->name[105]
           << pDeviceProperties->name[106] << pDeviceProperties->name[107]
           << pDeviceProperties->name[108] << pDeviceProperties->name[109]
           << pDeviceProperties->name[110] << pDeviceProperties->name[111]
           << pDeviceProperties->name[112] << pDeviceProperties->name[113]
           << pDeviceProperties->name[114] << pDeviceProperties->name[115]
           << pDeviceProperties->name[116] << pDeviceProperties->name[117]
           << pDeviceProperties->name[118] << pDeviceProperties->name[119]
           << pDeviceProperties->name[120] << pDeviceProperties->name[121]
           << pDeviceProperties->name[122] << pDeviceProperties->name[123]
           << pDeviceProperties->name[124] << pDeviceProperties->name[125]
           << pDeviceProperties->name[126] << pDeviceProperties->name[127]
           << pDeviceProperties->name[128] << pDeviceProperties->name[129]
           << pDeviceProperties->name[130] << pDeviceProperties->name[131]
           << pDeviceProperties->name[132] << pDeviceProperties->name[133]
           << pDeviceProperties->name[134] << pDeviceProperties->name[135]
           << pDeviceProperties->name[136] << pDeviceProperties->name[137]
           << pDeviceProperties->name[138] << pDeviceProperties->name[139]
           << pDeviceProperties->name[140] << pDeviceProperties->name[141]
           << pDeviceProperties->name[142] << pDeviceProperties->name[143]
           << pDeviceProperties->name[144] << pDeviceProperties->name[145]
           << pDeviceProperties->name[146] << pDeviceProperties->name[147]
           << pDeviceProperties->name[148] << pDeviceProperties->name[149]
           << pDeviceProperties->name[150] << pDeviceProperties->name[151]
           << pDeviceProperties->name[152] << pDeviceProperties->name[153]
           << pDeviceProperties->name[154] << pDeviceProperties->name[155]
           << pDeviceProperties->name[156] << pDeviceProperties->name[157]
           << pDeviceProperties->name[158] << pDeviceProperties->name[159]
           << pDeviceProperties->name[160] << pDeviceProperties->name[161]
           << pDeviceProperties->name[162] << pDeviceProperties->name[163]
           << pDeviceProperties->name[164] << pDeviceProperties->name[165]
           << pDeviceProperties->name[166] << pDeviceProperties->name[167]
           << pDeviceProperties->name[168] << pDeviceProperties->name[169]
           << pDeviceProperties->name[170] << pDeviceProperties->name[171]
           << pDeviceProperties->name[172] << pDeviceProperties->name[173]
           << pDeviceProperties->name[174] << pDeviceProperties->name[175]
           << pDeviceProperties->name[176] << pDeviceProperties->name[177]
           << pDeviceProperties->name[178] << pDeviceProperties->name[179]
           << pDeviceProperties->name[180] << pDeviceProperties->name[181]
           << pDeviceProperties->name[182] << pDeviceProperties->name[183]
           << pDeviceProperties->name[184] << pDeviceProperties->name[185]
           << pDeviceProperties->name[186] << pDeviceProperties->name[187]
           << pDeviceProperties->name[188] << pDeviceProperties->name[189]
           << pDeviceProperties->name[190] << pDeviceProperties->name[191]
           << pDeviceProperties->name[192] << pDeviceProperties->name[193]
           << pDeviceProperties->name[194] << pDeviceProperties->name[195]
           << pDeviceProperties->name[196] << pDeviceProperties->name[197]
           << pDeviceProperties->name[198] << pDeviceProperties->name[199]
           << pDeviceProperties->name[200] << pDeviceProperties->name[201]
           << pDeviceProperties->name[202] << pDeviceProperties->name[203]
           << pDeviceProperties->name[204] << pDeviceProperties->name[205]
           << pDeviceProperties->name[206] << pDeviceProperties->name[207]
           << pDeviceProperties->name[208] << pDeviceProperties->name[209]
           << pDeviceProperties->name[210] << pDeviceProperties->name[211]
           << pDeviceProperties->name[212] << pDeviceProperties->name[213]
           << pDeviceProperties->name[214] << pDeviceProperties->name[215]
           << pDeviceProperties->name[216] << pDeviceProperties->name[217]
           << pDeviceProperties->name[218] << pDeviceProperties->name[219]
           << pDeviceProperties->name[220] << pDeviceProperties->name[221]
           << pDeviceProperties->name[222] << pDeviceProperties->name[223]
           << pDeviceProperties->name[224] << pDeviceProperties->name[225]
           << pDeviceProperties->name[226] << pDeviceProperties->name[227]
           << pDeviceProperties->name[228] << pDeviceProperties->name[229]
           << pDeviceProperties->name[230] << pDeviceProperties->name[231]
           << pDeviceProperties->name[232] << pDeviceProperties->name[233]
           << pDeviceProperties->name[234] << pDeviceProperties->name[235]
           << pDeviceProperties->name[236] << pDeviceProperties->name[237]
           << pDeviceProperties->name[238] << pDeviceProperties->name[239]
           << pDeviceProperties->name[240] << pDeviceProperties->name[241]
           << pDeviceProperties->name[242] << pDeviceProperties->name[243]
           << pDeviceProperties->name[244] << pDeviceProperties->name[245]
           << pDeviceProperties->name[246] << pDeviceProperties->name[247]
           << pDeviceProperties->name[248] << pDeviceProperties->name[249]
           << pDeviceProperties->name[250] << pDeviceProperties->name[251]
           << pDeviceProperties->name[252] << pDeviceProperties->name[253]
           << pDeviceProperties->name[254] << pDeviceProperties->name[255];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetComputeProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetModuleProperties(";
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
        ss << "id: " << static_cast<int>(pModuleProperties->nativeKernelSupported.id[0]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[1]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[2]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[3]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[4]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[5]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[6]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[7]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[8]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[9]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[10]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[11]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[12]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[13]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[14]) << " "
           << static_cast<int>(pModuleProperties->nativeKernelSupported.id[15]);
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetCommandQueueGroupProperties(";
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pCommandQueueGroupProperties == nullptr) {
        ss << ", pCommandQueueGroupProperties: nullptr";
    } else {
        ss << ", pCommandQueueGroupProperties {";
        ss << "stype: " << pCommandQueueGroupProperties->stype;
        ss << ", pNext: " << pCommandQueueGroupProperties->pNext;
        ss << ", flags: " << pCommandQueueGroupProperties->flags;
        ss << ", maxMemoryFillPatternSize: "
           << pCommandQueueGroupProperties->maxMemoryFillPatternSize;
        ss << ", numQueues: " << pCommandQueueGroupProperties->numQueues;
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetMemoryProperties(";
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pMemProperties == nullptr) {
        ss << ", pMemProperties: nullptr";
    } else {
        ss << ", pMemProperties {";
        ss << "stype: " << pMemProperties->stype;
        ss << ", pNext: " << pMemProperties->pNext;
        ss << ", flags: " << pMemProperties->flags;
        ss << ", maxClockRate: " << pMemProperties->maxClockRate;
        ss << ", maxBusWidth: " << pMemProperties->maxBusWidth;
        ss << ", totalSize: " << pMemProperties->totalSize;
        ss << ", name: " << pMemProperties->name[0] << pMemProperties->name[1]
           << pMemProperties->name[2] << pMemProperties->name[3] << pMemProperties->name[4]
           << pMemProperties->name[5] << pMemProperties->name[6] << pMemProperties->name[7]
           << pMemProperties->name[8] << pMemProperties->name[9] << pMemProperties->name[10]
           << pMemProperties->name[11] << pMemProperties->name[12] << pMemProperties->name[13]
           << pMemProperties->name[14] << pMemProperties->name[15] << pMemProperties->name[16]
           << pMemProperties->name[17] << pMemProperties->name[18] << pMemProperties->name[19]
           << pMemProperties->name[20] << pMemProperties->name[21] << pMemProperties->name[22]
           << pMemProperties->name[23] << pMemProperties->name[24] << pMemProperties->name[25]
           << pMemProperties->name[26] << pMemProperties->name[27] << pMemProperties->name[28]
           << pMemProperties->name[29] << pMemProperties->name[30] << pMemProperties->name[31]
           << pMemProperties->name[32] << pMemProperties->name[33] << pMemProperties->name[34]
           << pMemProperties->name[35] << pMemProperties->name[36] << pMemProperties->name[37]
           << pMemProperties->name[38] << pMemProperties->name[39] << pMemProperties->name[40]
           << pMemProperties->name[41] << pMemProperties->name[42] << pMemProperties->name[43]
           << pMemProperties->name[44] << pMemProperties->name[45] << pMemProperties->name[46]
           << pMemProperties->name[47] << pMemProperties->name[48] << pMemProperties->name[49]
           << pMemProperties->name[50] << pMemProperties->name[51] << pMemProperties->name[52]
           << pMemProperties->name[53] << pMemProperties->name[54] << pMemProperties->name[55]
           << pMemProperties->name[56] << pMemProperties->name[57] << pMemProperties->name[58]
           << pMemProperties->name[59] << pMemProperties->name[60] << pMemProperties->name[61]
           << pMemProperties->name[62] << pMemProperties->name[63] << pMemProperties->name[64]
           << pMemProperties->name[65] << pMemProperties->name[66] << pMemProperties->name[67]
           << pMemProperties->name[68] << pMemProperties->name[69] << pMemProperties->name[70]
           << pMemProperties->name[71] << pMemProperties->name[72] << pMemProperties->name[73]
           << pMemProperties->name[74] << pMemProperties->name[75] << pMemProperties->name[76]
           << pMemProperties->name[77] << pMemProperties->name[78] << pMemProperties->name[79]
           << pMemProperties->name[80] << pMemProperties->name[81] << pMemProperties->name[82]
           << pMemProperties->name[83] << pMemProperties->name[84] << pMemProperties->name[85]
           << pMemProperties->name[86] << pMemProperties->name[87] << pMemProperties->name[88]
           << pMemProperties->name[89] << pMemProperties->name[90] << pMemProperties->name[91]
           << pMemProperties->name[92] << pMemProperties->name[93] << pMemProperties->name[94]
           << pMemProperties->name[95] << pMemProperties->name[96] << pMemProperties->name[97]
           << pMemProperties->name[98] << pMemProperties->name[99] << pMemProperties->name[100]
           << pMemProperties->name[101] << pMemProperties->name[102] << pMemProperties->name[103]
           << pMemProperties->name[104] << pMemProperties->name[105] << pMemProperties->name[106]
           << pMemProperties->name[107] << pMemProperties->name[108] << pMemProperties->name[109]
           << pMemProperties->name[110] << pMemProperties->name[111] << pMemProperties->name[112]
           << pMemProperties->name[113] << pMemProperties->name[114] << pMemProperties->name[115]
           << pMemProperties->name[116] << pMemProperties->name[117] << pMemProperties->name[118]
           << pMemProperties->name[119] << pMemProperties->name[120] << pMemProperties->name[121]
           << pMemProperties->name[122] << pMemProperties->name[123] << pMemProperties->name[124]
           << pMemProperties->name[125] << pMemProperties->name[126] << pMemProperties->name[127]
           << pMemProperties->name[128] << pMemProperties->name[129] << pMemProperties->name[130]
           << pMemProperties->name[131] << pMemProperties->name[132] << pMemProperties->name[133]
           << pMemProperties->name[134] << pMemProperties->name[135] << pMemProperties->name[136]
           << pMemProperties->name[137] << pMemProperties->name[138] << pMemProperties->name[139]
           << pMemProperties->name[140] << pMemProperties->name[141] << pMemProperties->name[142]
           << pMemProperties->name[143] << pMemProperties->name[144] << pMemProperties->name[145]
           << pMemProperties->name[146] << pMemProperties->name[147] << pMemProperties->name[148]
           << pMemProperties->name[149] << pMemProperties->name[150] << pMemProperties->name[151]
           << pMemProperties->name[152] << pMemProperties->name[153] << pMemProperties->name[154]
           << pMemProperties->name[155] << pMemProperties->name[156] << pMemProperties->name[157]
           << pMemProperties->name[158] << pMemProperties->name[159] << pMemProperties->name[160]
           << pMemProperties->name[161] << pMemProperties->name[162] << pMemProperties->name[163]
           << pMemProperties->name[164] << pMemProperties->name[165] << pMemProperties->name[166]
           << pMemProperties->name[167] << pMemProperties->name[168] << pMemProperties->name[169]
           << pMemProperties->name[170] << pMemProperties->name[171] << pMemProperties->name[172]
           << pMemProperties->name[173] << pMemProperties->name[174] << pMemProperties->name[175]
           << pMemProperties->name[176] << pMemProperties->name[177] << pMemProperties->name[178]
           << pMemProperties->name[179] << pMemProperties->name[180] << pMemProperties->name[181]
           << pMemProperties->name[182] << pMemProperties->name[183] << pMemProperties->name[184]
           << pMemProperties->name[185] << pMemProperties->name[186] << pMemProperties->name[187]
           << pMemProperties->name[188] << pMemProperties->name[189] << pMemProperties->name[190]
           << pMemProperties->name[191] << pMemProperties->name[192] << pMemProperties->name[193]
           << pMemProperties->name[194] << pMemProperties->name[195] << pMemProperties->name[196]
           << pMemProperties->name[197] << pMemProperties->name[198] << pMemProperties->name[199]
           << pMemProperties->name[200] << pMemProperties->name[201] << pMemProperties->name[202]
           << pMemProperties->name[203] << pMemProperties->name[204] << pMemProperties->name[205]
           << pMemProperties->name[206] << pMemProperties->name[207] << pMemProperties->name[208]
           << pMemProperties->name[209] << pMemProperties->name[210] << pMemProperties->name[211]
           << pMemProperties->name[212] << pMemProperties->name[213] << pMemProperties->name[214]
           << pMemProperties->name[215] << pMemProperties->name[216] << pMemProperties->name[217]
           << pMemProperties->name[218] << pMemProperties->name[219] << pMemProperties->name[220]
           << pMemProperties->name[221] << pMemProperties->name[222] << pMemProperties->name[223]
           << pMemProperties->name[224] << pMemProperties->name[225] << pMemProperties->name[226]
           << pMemProperties->name[227] << pMemProperties->name[228] << pMemProperties->name[229]
           << pMemProperties->name[230] << pMemProperties->name[231] << pMemProperties->name[232]
           << pMemProperties->name[233] << pMemProperties->name[234] << pMemProperties->name[235]
           << pMemProperties->name[236] << pMemProperties->name[237] << pMemProperties->name[238]
           << pMemProperties->name[239] << pMemProperties->name[240] << pMemProperties->name[241]
           << pMemProperties->name[242] << pMemProperties->name[243] << pMemProperties->name[244]
           << pMemProperties->name[245] << pMemProperties->name[246] << pMemProperties->name[247]
           << pMemProperties->name[248] << pMemProperties->name[249] << pMemProperties->name[250]
           << pMemProperties->name[251] << pMemProperties->name[252] << pMemProperties->name[253]
           << pMemProperties->name[254] << pMemProperties->name[255];
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetMemoryAccessProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetCacheProperties(";
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pCacheProperties == nullptr) {
        ss << ", pCacheProperties: nullptr";
    } else {
        ss << ", pCacheProperties {";
        ss << "stype: " << pCacheProperties->stype;
        ss << ", pNext: " << pCacheProperties->pNext;
        ss << ", flags: " << pCacheProperties->flags;
        ss << ", cacheSize: " << pCacheProperties->cacheSize;
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetImageProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetExternalMemoryProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetP2PProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceCanAccessPeer(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetStatus(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetGlobalTimestamps(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeContextCreate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeContextCreateEx(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeContextDestroy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeContextGetStatus(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandQueueCreate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandQueueDestroy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandQueueExecuteCommandLists(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandQueueSynchronize(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandQueueGetOrdinal(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandQueueGetIndex(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListCreate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListCreateImmediate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListDestroy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListClose(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListReset(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendWriteGlobalTimestamp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListHostSynchronize(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListGetDeviceHandle(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListGetContextHandle(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListGetOrdinal(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListImmediateGetIndex(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListIsImmediate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendBarrier(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendMemoryRangesBarrier(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeContextSystemBarrier(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendMemoryCopy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendMemoryFill(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendMemoryCopyRegion(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendMemoryCopyFromContext(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendImageCopy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendImageCopyRegion(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendImageCopyToMemory(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendImageCopyFromMemory(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendMemoryPrefetch(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendMemAdvise(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventPoolCreate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventPoolDestroy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventCreate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventDestroy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventPoolGetIpcHandle(";
    ss << "hEventPool: " << hEventPool;
    if (phIpc == nullptr) {
        ss << ", phIpc: nullptr";
    } else {
        ss << ", phIpc {";
        ss << "data: " << phIpc->data[0] << phIpc->data[1] << phIpc->data[2] << phIpc->data[3]
           << phIpc->data[4] << phIpc->data[5] << phIpc->data[6] << phIpc->data[7] << phIpc->data[8]
           << phIpc->data[9] << phIpc->data[10] << phIpc->data[11] << phIpc->data[12]
           << phIpc->data[13] << phIpc->data[14] << phIpc->data[15] << phIpc->data[16]
           << phIpc->data[17] << phIpc->data[18] << phIpc->data[19] << phIpc->data[20]
           << phIpc->data[21] << phIpc->data[22] << phIpc->data[23] << phIpc->data[24]
           << phIpc->data[25] << phIpc->data[26] << phIpc->data[27] << phIpc->data[28]
           << phIpc->data[29] << phIpc->data[30] << phIpc->data[31] << phIpc->data[32]
           << phIpc->data[33] << phIpc->data[34] << phIpc->data[35] << phIpc->data[36]
           << phIpc->data[37] << phIpc->data[38] << phIpc->data[39] << phIpc->data[40]
           << phIpc->data[41] << phIpc->data[42] << phIpc->data[43] << phIpc->data[44]
           << phIpc->data[45] << phIpc->data[46] << phIpc->data[47] << phIpc->data[48]
           << phIpc->data[49] << phIpc->data[50] << phIpc->data[51] << phIpc->data[52]
           << phIpc->data[53] << phIpc->data[54] << phIpc->data[55] << phIpc->data[56]
           << phIpc->data[57] << phIpc->data[58] << phIpc->data[59] << phIpc->data[60]
           << phIpc->data[61] << phIpc->data[62] << phIpc->data[63];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventPoolPutIpcHandle(";
    ss << "hContext: " << hContext;
    ss << ", hIpc {";
    ss << "data: " << hIpc.data[0] << hIpc.data[1] << hIpc.data[2] << hIpc.data[3] << hIpc.data[4]
       << hIpc.data[5] << hIpc.data[6] << hIpc.data[7] << hIpc.data[8] << hIpc.data[9]
       << hIpc.data[10] << hIpc.data[11] << hIpc.data[12] << hIpc.data[13] << hIpc.data[14]
       << hIpc.data[15] << hIpc.data[16] << hIpc.data[17] << hIpc.data[18] << hIpc.data[19]
       << hIpc.data[20] << hIpc.data[21] << hIpc.data[22] << hIpc.data[23] << hIpc.data[24]
       << hIpc.data[25] << hIpc.data[26] << hIpc.data[27] << hIpc.data[28] << hIpc.data[29]
       << hIpc.data[30] << hIpc.data[31] << hIpc.data[32] << hIpc.data[33] << hIpc.data[34]
       << hIpc.data[35] << hIpc.data[36] << hIpc.data[37] << hIpc.data[38] << hIpc.data[39]
       << hIpc.data[40] << hIpc.data[41] << hIpc.data[42] << hIpc.data[43] << hIpc.data[44]
       << hIpc.data[45] << hIpc.data[46] << hIpc.data[47] << hIpc.data[48] << hIpc.data[49]
       << hIpc.data[50] << hIpc.data[51] << hIpc.data[52] << hIpc.data[53] << hIpc.data[54]
       << hIpc.data[55] << hIpc.data[56] << hIpc.data[57] << hIpc.data[58] << hIpc.data[59]
       << hIpc.data[60] << hIpc.data[61] << hIpc.data[62] << hIpc.data[63];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventPoolOpenIpcHandle(";
    ss << "hContext: " << hContext;
    ss << ", hIpc {";
    ss << "data: " << hIpc.data[0] << hIpc.data[1] << hIpc.data[2] << hIpc.data[3] << hIpc.data[4]
       << hIpc.data[5] << hIpc.data[6] << hIpc.data[7] << hIpc.data[8] << hIpc.data[9]
       << hIpc.data[10] << hIpc.data[11] << hIpc.data[12] << hIpc.data[13] << hIpc.data[14]
       << hIpc.data[15] << hIpc.data[16] << hIpc.data[17] << hIpc.data[18] << hIpc.data[19]
       << hIpc.data[20] << hIpc.data[21] << hIpc.data[22] << hIpc.data[23] << hIpc.data[24]
       << hIpc.data[25] << hIpc.data[26] << hIpc.data[27] << hIpc.data[28] << hIpc.data[29]
       << hIpc.data[30] << hIpc.data[31] << hIpc.data[32] << hIpc.data[33] << hIpc.data[34]
       << hIpc.data[35] << hIpc.data[36] << hIpc.data[37] << hIpc.data[38] << hIpc.data[39]
       << hIpc.data[40] << hIpc.data[41] << hIpc.data[42] << hIpc.data[43] << hIpc.data[44]
       << hIpc.data[45] << hIpc.data[46] << hIpc.data[47] << hIpc.data[48] << hIpc.data[49]
       << hIpc.data[50] << hIpc.data[51] << hIpc.data[52] << hIpc.data[53] << hIpc.data[54]
       << hIpc.data[55] << hIpc.data[56] << hIpc.data[57] << hIpc.data[58] << hIpc.data[59]
       << hIpc.data[60] << hIpc.data[61] << hIpc.data[62] << hIpc.data[63];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventPoolCloseIpcHandle(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendSignalEvent(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendWaitOnEvents(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventHostSignal(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventHostSynchronize(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventQueryStatus(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendEventReset(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventHostReset(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventQueryKernelTimestamp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendQueryKernelTimestamps(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventGetEventPool(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventGetSignalScope(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventGetWaitScope(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventPoolGetContextHandle(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventPoolGetFlags(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeFenceCreate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeFenceDestroy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeFenceHostSynchronize(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeFenceQueryStatus(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeFenceReset(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeImageGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeImageCreate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeImageDestroy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemAllocShared(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemAllocDevice(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemAllocHost(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemFree(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemGetAllocProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemGetAddressRange(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemGetIpcHandle(";
    ss << "hContext: " << hContext;
    ss << ", ptr: " << ptr;
    if (pIpcHandle == nullptr) {
        ss << ", pIpcHandle: nullptr";
    } else {
        ss << ", pIpcHandle {";
        ss << "data: " << pIpcHandle->data[0] << pIpcHandle->data[1] << pIpcHandle->data[2]
           << pIpcHandle->data[3] << pIpcHandle->data[4] << pIpcHandle->data[5]
           << pIpcHandle->data[6] << pIpcHandle->data[7] << pIpcHandle->data[8]
           << pIpcHandle->data[9] << pIpcHandle->data[10] << pIpcHandle->data[11]
           << pIpcHandle->data[12] << pIpcHandle->data[13] << pIpcHandle->data[14]
           << pIpcHandle->data[15] << pIpcHandle->data[16] << pIpcHandle->data[17]
           << pIpcHandle->data[18] << pIpcHandle->data[19] << pIpcHandle->data[20]
           << pIpcHandle->data[21] << pIpcHandle->data[22] << pIpcHandle->data[23]
           << pIpcHandle->data[24] << pIpcHandle->data[25] << pIpcHandle->data[26]
           << pIpcHandle->data[27] << pIpcHandle->data[28] << pIpcHandle->data[29]
           << pIpcHandle->data[30] << pIpcHandle->data[31] << pIpcHandle->data[32]
           << pIpcHandle->data[33] << pIpcHandle->data[34] << pIpcHandle->data[35]
           << pIpcHandle->data[36] << pIpcHandle->data[37] << pIpcHandle->data[38]
           << pIpcHandle->data[39] << pIpcHandle->data[40] << pIpcHandle->data[41]
           << pIpcHandle->data[42] << pIpcHandle->data[43] << pIpcHandle->data[44]
           << pIpcHandle->data[45] << pIpcHandle->data[46] << pIpcHandle->data[47]
           << pIpcHandle->data[48] << pIpcHandle->data[49] << pIpcHandle->data[50]
           << pIpcHandle->data[51] << pIpcHandle->data[52] << pIpcHandle->data[53]
           << pIpcHandle->data[54] << pIpcHandle->data[55] << pIpcHandle->data[56]
           << pIpcHandle->data[57] << pIpcHandle->data[58] << pIpcHandle->data[59]
           << pIpcHandle->data[60] << pIpcHandle->data[61] << pIpcHandle->data[62]
           << pIpcHandle->data[63];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemGetIpcHandleFromFileDescriptorExp(";
    ss << "hContext: " << hContext;
    ss << ", handle: " << handle;
    if (pIpcHandle == nullptr) {
        ss << ", pIpcHandle: nullptr";
    } else {
        ss << ", pIpcHandle {";
        ss << "data: " << pIpcHandle->data[0] << pIpcHandle->data[1] << pIpcHandle->data[2]
           << pIpcHandle->data[3] << pIpcHandle->data[4] << pIpcHandle->data[5]
           << pIpcHandle->data[6] << pIpcHandle->data[7] << pIpcHandle->data[8]
           << pIpcHandle->data[9] << pIpcHandle->data[10] << pIpcHandle->data[11]
           << pIpcHandle->data[12] << pIpcHandle->data[13] << pIpcHandle->data[14]
           << pIpcHandle->data[15] << pIpcHandle->data[16] << pIpcHandle->data[17]
           << pIpcHandle->data[18] << pIpcHandle->data[19] << pIpcHandle->data[20]
           << pIpcHandle->data[21] << pIpcHandle->data[22] << pIpcHandle->data[23]
           << pIpcHandle->data[24] << pIpcHandle->data[25] << pIpcHandle->data[26]
           << pIpcHandle->data[27] << pIpcHandle->data[28] << pIpcHandle->data[29]
           << pIpcHandle->data[30] << pIpcHandle->data[31] << pIpcHandle->data[32]
           << pIpcHandle->data[33] << pIpcHandle->data[34] << pIpcHandle->data[35]
           << pIpcHandle->data[36] << pIpcHandle->data[37] << pIpcHandle->data[38]
           << pIpcHandle->data[39] << pIpcHandle->data[40] << pIpcHandle->data[41]
           << pIpcHandle->data[42] << pIpcHandle->data[43] << pIpcHandle->data[44]
           << pIpcHandle->data[45] << pIpcHandle->data[46] << pIpcHandle->data[47]
           << pIpcHandle->data[48] << pIpcHandle->data[49] << pIpcHandle->data[50]
           << pIpcHandle->data[51] << pIpcHandle->data[52] << pIpcHandle->data[53]
           << pIpcHandle->data[54] << pIpcHandle->data[55] << pIpcHandle->data[56]
           << pIpcHandle->data[57] << pIpcHandle->data[58] << pIpcHandle->data[59]
           << pIpcHandle->data[60] << pIpcHandle->data[61] << pIpcHandle->data[62]
           << pIpcHandle->data[63];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemGetFileDescriptorFromIpcHandleExp(";
    ss << "hContext: " << hContext;
    ss << ", ipcHandle {";
    ss << "data: " << ipcHandle.data[0] << ipcHandle.data[1] << ipcHandle.data[2]
       << ipcHandle.data[3] << ipcHandle.data[4] << ipcHandle.data[5] << ipcHandle.data[6]
       << ipcHandle.data[7] << ipcHandle.data[8] << ipcHandle.data[9] << ipcHandle.data[10]
       << ipcHandle.data[11] << ipcHandle.data[12] << ipcHandle.data[13] << ipcHandle.data[14]
       << ipcHandle.data[15] << ipcHandle.data[16] << ipcHandle.data[17] << ipcHandle.data[18]
       << ipcHandle.data[19] << ipcHandle.data[20] << ipcHandle.data[21] << ipcHandle.data[22]
       << ipcHandle.data[23] << ipcHandle.data[24] << ipcHandle.data[25] << ipcHandle.data[26]
       << ipcHandle.data[27] << ipcHandle.data[28] << ipcHandle.data[29] << ipcHandle.data[30]
       << ipcHandle.data[31] << ipcHandle.data[32] << ipcHandle.data[33] << ipcHandle.data[34]
       << ipcHandle.data[35] << ipcHandle.data[36] << ipcHandle.data[37] << ipcHandle.data[38]
       << ipcHandle.data[39] << ipcHandle.data[40] << ipcHandle.data[41] << ipcHandle.data[42]
       << ipcHandle.data[43] << ipcHandle.data[44] << ipcHandle.data[45] << ipcHandle.data[46]
       << ipcHandle.data[47] << ipcHandle.data[48] << ipcHandle.data[49] << ipcHandle.data[50]
       << ipcHandle.data[51] << ipcHandle.data[52] << ipcHandle.data[53] << ipcHandle.data[54]
       << ipcHandle.data[55] << ipcHandle.data[56] << ipcHandle.data[57] << ipcHandle.data[58]
       << ipcHandle.data[59] << ipcHandle.data[60] << ipcHandle.data[61] << ipcHandle.data[62]
       << ipcHandle.data[63];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemPutIpcHandle(";
    ss << "hContext: " << hContext;
    ss << ", handle {";
    ss << "data: " << handle.data[0] << handle.data[1] << handle.data[2] << handle.data[3]
       << handle.data[4] << handle.data[5] << handle.data[6] << handle.data[7] << handle.data[8]
       << handle.data[9] << handle.data[10] << handle.data[11] << handle.data[12] << handle.data[13]
       << handle.data[14] << handle.data[15] << handle.data[16] << handle.data[17]
       << handle.data[18] << handle.data[19] << handle.data[20] << handle.data[21]
       << handle.data[22] << handle.data[23] << handle.data[24] << handle.data[25]
       << handle.data[26] << handle.data[27] << handle.data[28] << handle.data[29]
       << handle.data[30] << handle.data[31] << handle.data[32] << handle.data[33]
       << handle.data[34] << handle.data[35] << handle.data[36] << handle.data[37]
       << handle.data[38] << handle.data[39] << handle.data[40] << handle.data[41]
       << handle.data[42] << handle.data[43] << handle.data[44] << handle.data[45]
       << handle.data[46] << handle.data[47] << handle.data[48] << handle.data[49]
       << handle.data[50] << handle.data[51] << handle.data[52] << handle.data[53]
       << handle.data[54] << handle.data[55] << handle.data[56] << handle.data[57]
       << handle.data[58] << handle.data[59] << handle.data[60] << handle.data[61]
       << handle.data[62] << handle.data[63];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemOpenIpcHandle(";
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", handle {";
    ss << "data: " << handle.data[0] << handle.data[1] << handle.data[2] << handle.data[3]
       << handle.data[4] << handle.data[5] << handle.data[6] << handle.data[7] << handle.data[8]
       << handle.data[9] << handle.data[10] << handle.data[11] << handle.data[12] << handle.data[13]
       << handle.data[14] << handle.data[15] << handle.data[16] << handle.data[17]
       << handle.data[18] << handle.data[19] << handle.data[20] << handle.data[21]
       << handle.data[22] << handle.data[23] << handle.data[24] << handle.data[25]
       << handle.data[26] << handle.data[27] << handle.data[28] << handle.data[29]
       << handle.data[30] << handle.data[31] << handle.data[32] << handle.data[33]
       << handle.data[34] << handle.data[35] << handle.data[36] << handle.data[37]
       << handle.data[38] << handle.data[39] << handle.data[40] << handle.data[41]
       << handle.data[42] << handle.data[43] << handle.data[44] << handle.data[45]
       << handle.data[46] << handle.data[47] << handle.data[48] << handle.data[49]
       << handle.data[50] << handle.data[51] << handle.data[52] << handle.data[53]
       << handle.data[54] << handle.data[55] << handle.data[56] << handle.data[57]
       << handle.data[58] << handle.data[59] << handle.data[60] << handle.data[61]
       << handle.data[62] << handle.data[63];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemCloseIpcHandle(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemSetAtomicAccessAttributeExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemGetAtomicAccessAttributeExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeModuleCreate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeModuleDestroy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeModuleDynamicLink(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeModuleBuildLogDestroy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeModuleBuildLogGetString(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeModuleGetNativeBinary(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeModuleGetGlobalPointer(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeModuleGetKernelNames(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeModuleGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelCreate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelDestroy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeModuleGetFunctionPointer(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelSetGroupSize(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelSuggestGroupSize(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelSuggestMaxCooperativeGroupCount(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelSetArgumentValue(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelSetIndirectAccess(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelGetIndirectAccess(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelGetSourceAttributes(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelSetCacheConfig(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelGetProperties(";
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
        ss << "kid: " << static_cast<int>(pKernelProperties->uuid.kid[0]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[1]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[2]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[3]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[4]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[5]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[6]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[7]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[8]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[9]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[10]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[11]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[12]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[13]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[14]) << " "
           << static_cast<int>(pKernelProperties->uuid.kid[15]);
        ss << ", mid: " << static_cast<int>(pKernelProperties->uuid.mid[0]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[1]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[2]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[3]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[4]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[5]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[6]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[7]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[8]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[9]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[10]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[11]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[12]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[13]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[14]) << " "
           << static_cast<int>(pKernelProperties->uuid.mid[15]);
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelGetName(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendLaunchKernel(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendLaunchCooperativeKernel(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendLaunchKernelIndirect(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendLaunchMultipleKernelsIndirect(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeContextMakeMemoryResident(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeContextEvictMemory(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeContextMakeImageResident(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeContextEvictImage(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeSamplerCreate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeSamplerDestroy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeVirtualMemReserve(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeVirtualMemFree(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeVirtualMemQueryPageSize(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zePhysicalMemCreate(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zePhysicalMemDestroy(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeVirtualMemMap(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeVirtualMemUnmap(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeVirtualMemSetAccessAttribute(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeVirtualMemGetAccessAttribute(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelSetGlobalOffsetExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceReserveCacheExt(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceSetCacheAdviceExt(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventQueryTimestampsExp(";
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
        ss << ", pTimestamps {";
        ss << ", global {";
        ss << "kernelStart: " << pTimestamps->global.kernelStart;
        ss << ", kernelEnd: " << pTimestamps->global.kernelEnd;
        ss << "}";
        ss << ", context {";
        ss << "kernelStart: " << pTimestamps->context.kernelStart;
        ss << ", kernelEnd: " << pTimestamps->context.kernelEnd;
        ss << "}";
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeImageGetMemoryPropertiesExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeImageViewCreateExt(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeImageViewCreateExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeKernelSchedulingHintExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDevicePciGetPropertiesExt(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendImageCopyToMemoryExt(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListAppendImageCopyFromMemoryExt(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeImageGetAllocPropertiesExt(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeModuleInspectLinkageExt(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemFreeExt(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeFabricVertexGetExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeFabricVertexGetSubVerticesExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeFabricVertexGetPropertiesExp(";
    ss << "hVertex: " << hVertex;
    if (pVertexProperties == nullptr) {
        ss << ", pVertexProperties: nullptr";
    } else {
        ss << ", pVertexProperties {";
        ss << "stype: " << pVertexProperties->stype;
        ss << ", pNext: " << pVertexProperties->pNext;
        ss << ", uuid {";
        ss << "id: " << static_cast<int>(pVertexProperties->uuid.id[0]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[1]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[2]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[3]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[4]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[5]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[6]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[7]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[8]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[9]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[10]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[11]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[12]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[13]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[14]) << " "
           << static_cast<int>(pVertexProperties->uuid.id[15]);
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeFabricVertexGetDeviceExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDeviceGetFabricVertexExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeFabricEdgeGetExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeFabricEdgeGetVerticesExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeFabricEdgeGetPropertiesExp(";
    ss << "hEdge: " << hEdge;
    if (pEdgeProperties == nullptr) {
        ss << ", pEdgeProperties: nullptr";
    } else {
        ss << ", pEdgeProperties {";
        ss << "stype: " << pEdgeProperties->stype;
        ss << ", pNext: " << pEdgeProperties->pNext;
        ss << ", uuid {";
        ss << "id: " << static_cast<int>(pEdgeProperties->uuid.id[0]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[1]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[2]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[3]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[4]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[5]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[6]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[7]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[8]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[9]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[10]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[11]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[12]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[13]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[14]) << " "
           << static_cast<int>(pEdgeProperties->uuid.id[15]);
        ss << "}";
        ss << ", model: " << pEdgeProperties->model[0] << pEdgeProperties->model[1]
           << pEdgeProperties->model[2] << pEdgeProperties->model[3] << pEdgeProperties->model[4]
           << pEdgeProperties->model[5] << pEdgeProperties->model[6] << pEdgeProperties->model[7]
           << pEdgeProperties->model[8] << pEdgeProperties->model[9] << pEdgeProperties->model[10]
           << pEdgeProperties->model[11] << pEdgeProperties->model[12] << pEdgeProperties->model[13]
           << pEdgeProperties->model[14] << pEdgeProperties->model[15] << pEdgeProperties->model[16]
           << pEdgeProperties->model[17] << pEdgeProperties->model[18] << pEdgeProperties->model[19]
           << pEdgeProperties->model[20] << pEdgeProperties->model[21] << pEdgeProperties->model[22]
           << pEdgeProperties->model[23] << pEdgeProperties->model[24] << pEdgeProperties->model[25]
           << pEdgeProperties->model[26] << pEdgeProperties->model[27] << pEdgeProperties->model[28]
           << pEdgeProperties->model[29] << pEdgeProperties->model[30] << pEdgeProperties->model[31]
           << pEdgeProperties->model[32] << pEdgeProperties->model[33] << pEdgeProperties->model[34]
           << pEdgeProperties->model[35] << pEdgeProperties->model[36] << pEdgeProperties->model[37]
           << pEdgeProperties->model[38] << pEdgeProperties->model[39] << pEdgeProperties->model[40]
           << pEdgeProperties->model[41] << pEdgeProperties->model[42] << pEdgeProperties->model[43]
           << pEdgeProperties->model[44] << pEdgeProperties->model[45] << pEdgeProperties->model[46]
           << pEdgeProperties->model[47] << pEdgeProperties->model[48] << pEdgeProperties->model[49]
           << pEdgeProperties->model[50] << pEdgeProperties->model[51] << pEdgeProperties->model[52]
           << pEdgeProperties->model[53] << pEdgeProperties->model[54] << pEdgeProperties->model[55]
           << pEdgeProperties->model[56] << pEdgeProperties->model[57] << pEdgeProperties->model[58]
           << pEdgeProperties->model[59] << pEdgeProperties->model[60] << pEdgeProperties->model[61]
           << pEdgeProperties->model[62] << pEdgeProperties->model[63] << pEdgeProperties->model[64]
           << pEdgeProperties->model[65] << pEdgeProperties->model[66] << pEdgeProperties->model[67]
           << pEdgeProperties->model[68] << pEdgeProperties->model[69] << pEdgeProperties->model[70]
           << pEdgeProperties->model[71] << pEdgeProperties->model[72] << pEdgeProperties->model[73]
           << pEdgeProperties->model[74] << pEdgeProperties->model[75] << pEdgeProperties->model[76]
           << pEdgeProperties->model[77] << pEdgeProperties->model[78] << pEdgeProperties->model[79]
           << pEdgeProperties->model[80] << pEdgeProperties->model[81] << pEdgeProperties->model[82]
           << pEdgeProperties->model[83] << pEdgeProperties->model[84] << pEdgeProperties->model[85]
           << pEdgeProperties->model[86] << pEdgeProperties->model[87] << pEdgeProperties->model[88]
           << pEdgeProperties->model[89] << pEdgeProperties->model[90] << pEdgeProperties->model[91]
           << pEdgeProperties->model[92] << pEdgeProperties->model[93] << pEdgeProperties->model[94]
           << pEdgeProperties->model[95] << pEdgeProperties->model[96] << pEdgeProperties->model[97]
           << pEdgeProperties->model[98] << pEdgeProperties->model[99]
           << pEdgeProperties->model[100] << pEdgeProperties->model[101]
           << pEdgeProperties->model[102] << pEdgeProperties->model[103]
           << pEdgeProperties->model[104] << pEdgeProperties->model[105]
           << pEdgeProperties->model[106] << pEdgeProperties->model[107]
           << pEdgeProperties->model[108] << pEdgeProperties->model[109]
           << pEdgeProperties->model[110] << pEdgeProperties->model[111]
           << pEdgeProperties->model[112] << pEdgeProperties->model[113]
           << pEdgeProperties->model[114] << pEdgeProperties->model[115]
           << pEdgeProperties->model[116] << pEdgeProperties->model[117]
           << pEdgeProperties->model[118] << pEdgeProperties->model[119]
           << pEdgeProperties->model[120] << pEdgeProperties->model[121]
           << pEdgeProperties->model[122] << pEdgeProperties->model[123]
           << pEdgeProperties->model[124] << pEdgeProperties->model[125]
           << pEdgeProperties->model[126] << pEdgeProperties->model[127]
           << pEdgeProperties->model[128] << pEdgeProperties->model[129]
           << pEdgeProperties->model[130] << pEdgeProperties->model[131]
           << pEdgeProperties->model[132] << pEdgeProperties->model[133]
           << pEdgeProperties->model[134] << pEdgeProperties->model[135]
           << pEdgeProperties->model[136] << pEdgeProperties->model[137]
           << pEdgeProperties->model[138] << pEdgeProperties->model[139]
           << pEdgeProperties->model[140] << pEdgeProperties->model[141]
           << pEdgeProperties->model[142] << pEdgeProperties->model[143]
           << pEdgeProperties->model[144] << pEdgeProperties->model[145]
           << pEdgeProperties->model[146] << pEdgeProperties->model[147]
           << pEdgeProperties->model[148] << pEdgeProperties->model[149]
           << pEdgeProperties->model[150] << pEdgeProperties->model[151]
           << pEdgeProperties->model[152] << pEdgeProperties->model[153]
           << pEdgeProperties->model[154] << pEdgeProperties->model[155]
           << pEdgeProperties->model[156] << pEdgeProperties->model[157]
           << pEdgeProperties->model[158] << pEdgeProperties->model[159]
           << pEdgeProperties->model[160] << pEdgeProperties->model[161]
           << pEdgeProperties->model[162] << pEdgeProperties->model[163]
           << pEdgeProperties->model[164] << pEdgeProperties->model[165]
           << pEdgeProperties->model[166] << pEdgeProperties->model[167]
           << pEdgeProperties->model[168] << pEdgeProperties->model[169]
           << pEdgeProperties->model[170] << pEdgeProperties->model[171]
           << pEdgeProperties->model[172] << pEdgeProperties->model[173]
           << pEdgeProperties->model[174] << pEdgeProperties->model[175]
           << pEdgeProperties->model[176] << pEdgeProperties->model[177]
           << pEdgeProperties->model[178] << pEdgeProperties->model[179]
           << pEdgeProperties->model[180] << pEdgeProperties->model[181]
           << pEdgeProperties->model[182] << pEdgeProperties->model[183]
           << pEdgeProperties->model[184] << pEdgeProperties->model[185]
           << pEdgeProperties->model[186] << pEdgeProperties->model[187]
           << pEdgeProperties->model[188] << pEdgeProperties->model[189]
           << pEdgeProperties->model[190] << pEdgeProperties->model[191]
           << pEdgeProperties->model[192] << pEdgeProperties->model[193]
           << pEdgeProperties->model[194] << pEdgeProperties->model[195]
           << pEdgeProperties->model[196] << pEdgeProperties->model[197]
           << pEdgeProperties->model[198] << pEdgeProperties->model[199]
           << pEdgeProperties->model[200] << pEdgeProperties->model[201]
           << pEdgeProperties->model[202] << pEdgeProperties->model[203]
           << pEdgeProperties->model[204] << pEdgeProperties->model[205]
           << pEdgeProperties->model[206] << pEdgeProperties->model[207]
           << pEdgeProperties->model[208] << pEdgeProperties->model[209]
           << pEdgeProperties->model[210] << pEdgeProperties->model[211]
           << pEdgeProperties->model[212] << pEdgeProperties->model[213]
           << pEdgeProperties->model[214] << pEdgeProperties->model[215]
           << pEdgeProperties->model[216] << pEdgeProperties->model[217]
           << pEdgeProperties->model[218] << pEdgeProperties->model[219]
           << pEdgeProperties->model[220] << pEdgeProperties->model[221]
           << pEdgeProperties->model[222] << pEdgeProperties->model[223]
           << pEdgeProperties->model[224] << pEdgeProperties->model[225]
           << pEdgeProperties->model[226] << pEdgeProperties->model[227]
           << pEdgeProperties->model[228] << pEdgeProperties->model[229]
           << pEdgeProperties->model[230] << pEdgeProperties->model[231]
           << pEdgeProperties->model[232] << pEdgeProperties->model[233]
           << pEdgeProperties->model[234] << pEdgeProperties->model[235]
           << pEdgeProperties->model[236] << pEdgeProperties->model[237]
           << pEdgeProperties->model[238] << pEdgeProperties->model[239]
           << pEdgeProperties->model[240] << pEdgeProperties->model[241]
           << pEdgeProperties->model[242] << pEdgeProperties->model[243]
           << pEdgeProperties->model[244] << pEdgeProperties->model[245]
           << pEdgeProperties->model[246] << pEdgeProperties->model[247]
           << pEdgeProperties->model[248] << pEdgeProperties->model[249]
           << pEdgeProperties->model[250] << pEdgeProperties->model[251]
           << pEdgeProperties->model[252] << pEdgeProperties->model[253]
           << pEdgeProperties->model[254] << pEdgeProperties->model[255];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeEventQueryKernelTimestampsExt(";
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
        ss << ", pResults {";
        ss << "stype: " << pResults->stype;
        ss << ", pNext: " << pResults->pNext;
        if (pResults->pKernelTimestampsBuffer == nullptr) {
            ss << ", pKernelTimestampsBuffer: nullptr";
        } else {
            ss << ", pKernelTimestampsBuffer {";
            ss << ", global {";
            ss << "kernelStart: " << pResults->pKernelTimestampsBuffer->global.kernelStart;
            ss << ", kernelEnd: " << pResults->pKernelTimestampsBuffer->global.kernelEnd;
            ss << "}";
            ss << ", context {";
            ss << "kernelStart: " << pResults->pKernelTimestampsBuffer->context.kernelStart;
            ss << ", kernelEnd: " << pResults->pKernelTimestampsBuffer->context.kernelEnd;
            ss << "}";
            ss << "}";
        }
        if (pResults->pSynchronizedTimestampsBuffer == nullptr) {
            ss << ", pSynchronizedTimestampsBuffer: nullptr";
        } else {
            ss << ", pSynchronizedTimestampsBuffer {";
            ss << ", global {";
            ss << "kernelStart: " << pResults->pSynchronizedTimestampsBuffer->global.kernelStart;
            ss << ", kernelEnd: " << pResults->pSynchronizedTimestampsBuffer->global.kernelEnd;
            ss << "}";
            ss << ", context {";
            ss << "kernelStart: " << pResults->pSynchronizedTimestampsBuffer->context.kernelStart;
            ss << ", kernelEnd: " << pResults->pSynchronizedTimestampsBuffer->context.kernelEnd;
            ss << "}";
            ss << "}";
            ss << "}";
        }
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeRTASBuilderCreateExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeRTASBuilderGetBuildPropertiesExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeDriverRTASFormatCompatibilityCheckExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeRTASBuilderBuildExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeRTASBuilderDestroyExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeRTASParallelOperationCreateExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeRTASParallelOperationGetPropertiesExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeRTASParallelOperationJoinExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeRTASParallelOperationDestroyExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeMemGetPitchFor2dImage(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeImageGetDeviceOffsetExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListCreateCloneExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListImmediateAppendCommandListsExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListGetNextCommandIdExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListGetNextCommandIdWithKernelsExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListUpdateMutableCommandsExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListUpdateMutableCommandSignalEventExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListUpdateMutableCommandWaitEventsExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zeCommandListUpdateMutableCommandKernelsExp(";
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
