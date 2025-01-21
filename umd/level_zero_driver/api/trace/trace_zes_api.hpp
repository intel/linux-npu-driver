/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Generated header based on zes_api.h@version v1.11-r1.11.4

#pragma once

#include "trace_ze_api.hpp"

#include <iostream>
#include <level_zero/zes_api.h>
#include <sstream>

inline std::string _trace_zesInit(zes_init_flags_t flags) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesInit(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDriverGet(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDriverGetExtensionProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDriverGetExtensionFunctionAddress(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceGet(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceGetProperties(";
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
        ss << "id: " << static_cast<int>(pProperties->core.uuid.id[0]) << " "
           << static_cast<int>(pProperties->core.uuid.id[1]) << " "
           << static_cast<int>(pProperties->core.uuid.id[2]) << " "
           << static_cast<int>(pProperties->core.uuid.id[3]) << " "
           << static_cast<int>(pProperties->core.uuid.id[4]) << " "
           << static_cast<int>(pProperties->core.uuid.id[5]) << " "
           << static_cast<int>(pProperties->core.uuid.id[6]) << " "
           << static_cast<int>(pProperties->core.uuid.id[7]) << " "
           << static_cast<int>(pProperties->core.uuid.id[8]) << " "
           << static_cast<int>(pProperties->core.uuid.id[9]) << " "
           << static_cast<int>(pProperties->core.uuid.id[10]) << " "
           << static_cast<int>(pProperties->core.uuid.id[11]) << " "
           << static_cast<int>(pProperties->core.uuid.id[12]) << " "
           << static_cast<int>(pProperties->core.uuid.id[13]) << " "
           << static_cast<int>(pProperties->core.uuid.id[14]) << " "
           << static_cast<int>(pProperties->core.uuid.id[15]);
        ss << "}";
        ss << ", name: " << pProperties->core.name[0] << pProperties->core.name[1]
           << pProperties->core.name[2] << pProperties->core.name[3] << pProperties->core.name[4]
           << pProperties->core.name[5] << pProperties->core.name[6] << pProperties->core.name[7]
           << pProperties->core.name[8] << pProperties->core.name[9] << pProperties->core.name[10]
           << pProperties->core.name[11] << pProperties->core.name[12] << pProperties->core.name[13]
           << pProperties->core.name[14] << pProperties->core.name[15] << pProperties->core.name[16]
           << pProperties->core.name[17] << pProperties->core.name[18] << pProperties->core.name[19]
           << pProperties->core.name[20] << pProperties->core.name[21] << pProperties->core.name[22]
           << pProperties->core.name[23] << pProperties->core.name[24] << pProperties->core.name[25]
           << pProperties->core.name[26] << pProperties->core.name[27] << pProperties->core.name[28]
           << pProperties->core.name[29] << pProperties->core.name[30] << pProperties->core.name[31]
           << pProperties->core.name[32] << pProperties->core.name[33] << pProperties->core.name[34]
           << pProperties->core.name[35] << pProperties->core.name[36] << pProperties->core.name[37]
           << pProperties->core.name[38] << pProperties->core.name[39] << pProperties->core.name[40]
           << pProperties->core.name[41] << pProperties->core.name[42] << pProperties->core.name[43]
           << pProperties->core.name[44] << pProperties->core.name[45] << pProperties->core.name[46]
           << pProperties->core.name[47] << pProperties->core.name[48] << pProperties->core.name[49]
           << pProperties->core.name[50] << pProperties->core.name[51] << pProperties->core.name[52]
           << pProperties->core.name[53] << pProperties->core.name[54] << pProperties->core.name[55]
           << pProperties->core.name[56] << pProperties->core.name[57] << pProperties->core.name[58]
           << pProperties->core.name[59] << pProperties->core.name[60] << pProperties->core.name[61]
           << pProperties->core.name[62] << pProperties->core.name[63] << pProperties->core.name[64]
           << pProperties->core.name[65] << pProperties->core.name[66] << pProperties->core.name[67]
           << pProperties->core.name[68] << pProperties->core.name[69] << pProperties->core.name[70]
           << pProperties->core.name[71] << pProperties->core.name[72] << pProperties->core.name[73]
           << pProperties->core.name[74] << pProperties->core.name[75] << pProperties->core.name[76]
           << pProperties->core.name[77] << pProperties->core.name[78] << pProperties->core.name[79]
           << pProperties->core.name[80] << pProperties->core.name[81] << pProperties->core.name[82]
           << pProperties->core.name[83] << pProperties->core.name[84] << pProperties->core.name[85]
           << pProperties->core.name[86] << pProperties->core.name[87] << pProperties->core.name[88]
           << pProperties->core.name[89] << pProperties->core.name[90] << pProperties->core.name[91]
           << pProperties->core.name[92] << pProperties->core.name[93] << pProperties->core.name[94]
           << pProperties->core.name[95] << pProperties->core.name[96] << pProperties->core.name[97]
           << pProperties->core.name[98] << pProperties->core.name[99]
           << pProperties->core.name[100] << pProperties->core.name[101]
           << pProperties->core.name[102] << pProperties->core.name[103]
           << pProperties->core.name[104] << pProperties->core.name[105]
           << pProperties->core.name[106] << pProperties->core.name[107]
           << pProperties->core.name[108] << pProperties->core.name[109]
           << pProperties->core.name[110] << pProperties->core.name[111]
           << pProperties->core.name[112] << pProperties->core.name[113]
           << pProperties->core.name[114] << pProperties->core.name[115]
           << pProperties->core.name[116] << pProperties->core.name[117]
           << pProperties->core.name[118] << pProperties->core.name[119]
           << pProperties->core.name[120] << pProperties->core.name[121]
           << pProperties->core.name[122] << pProperties->core.name[123]
           << pProperties->core.name[124] << pProperties->core.name[125]
           << pProperties->core.name[126] << pProperties->core.name[127]
           << pProperties->core.name[128] << pProperties->core.name[129]
           << pProperties->core.name[130] << pProperties->core.name[131]
           << pProperties->core.name[132] << pProperties->core.name[133]
           << pProperties->core.name[134] << pProperties->core.name[135]
           << pProperties->core.name[136] << pProperties->core.name[137]
           << pProperties->core.name[138] << pProperties->core.name[139]
           << pProperties->core.name[140] << pProperties->core.name[141]
           << pProperties->core.name[142] << pProperties->core.name[143]
           << pProperties->core.name[144] << pProperties->core.name[145]
           << pProperties->core.name[146] << pProperties->core.name[147]
           << pProperties->core.name[148] << pProperties->core.name[149]
           << pProperties->core.name[150] << pProperties->core.name[151]
           << pProperties->core.name[152] << pProperties->core.name[153]
           << pProperties->core.name[154] << pProperties->core.name[155]
           << pProperties->core.name[156] << pProperties->core.name[157]
           << pProperties->core.name[158] << pProperties->core.name[159]
           << pProperties->core.name[160] << pProperties->core.name[161]
           << pProperties->core.name[162] << pProperties->core.name[163]
           << pProperties->core.name[164] << pProperties->core.name[165]
           << pProperties->core.name[166] << pProperties->core.name[167]
           << pProperties->core.name[168] << pProperties->core.name[169]
           << pProperties->core.name[170] << pProperties->core.name[171]
           << pProperties->core.name[172] << pProperties->core.name[173]
           << pProperties->core.name[174] << pProperties->core.name[175]
           << pProperties->core.name[176] << pProperties->core.name[177]
           << pProperties->core.name[178] << pProperties->core.name[179]
           << pProperties->core.name[180] << pProperties->core.name[181]
           << pProperties->core.name[182] << pProperties->core.name[183]
           << pProperties->core.name[184] << pProperties->core.name[185]
           << pProperties->core.name[186] << pProperties->core.name[187]
           << pProperties->core.name[188] << pProperties->core.name[189]
           << pProperties->core.name[190] << pProperties->core.name[191]
           << pProperties->core.name[192] << pProperties->core.name[193]
           << pProperties->core.name[194] << pProperties->core.name[195]
           << pProperties->core.name[196] << pProperties->core.name[197]
           << pProperties->core.name[198] << pProperties->core.name[199]
           << pProperties->core.name[200] << pProperties->core.name[201]
           << pProperties->core.name[202] << pProperties->core.name[203]
           << pProperties->core.name[204] << pProperties->core.name[205]
           << pProperties->core.name[206] << pProperties->core.name[207]
           << pProperties->core.name[208] << pProperties->core.name[209]
           << pProperties->core.name[210] << pProperties->core.name[211]
           << pProperties->core.name[212] << pProperties->core.name[213]
           << pProperties->core.name[214] << pProperties->core.name[215]
           << pProperties->core.name[216] << pProperties->core.name[217]
           << pProperties->core.name[218] << pProperties->core.name[219]
           << pProperties->core.name[220] << pProperties->core.name[221]
           << pProperties->core.name[222] << pProperties->core.name[223]
           << pProperties->core.name[224] << pProperties->core.name[225]
           << pProperties->core.name[226] << pProperties->core.name[227]
           << pProperties->core.name[228] << pProperties->core.name[229]
           << pProperties->core.name[230] << pProperties->core.name[231]
           << pProperties->core.name[232] << pProperties->core.name[233]
           << pProperties->core.name[234] << pProperties->core.name[235]
           << pProperties->core.name[236] << pProperties->core.name[237]
           << pProperties->core.name[238] << pProperties->core.name[239]
           << pProperties->core.name[240] << pProperties->core.name[241]
           << pProperties->core.name[242] << pProperties->core.name[243]
           << pProperties->core.name[244] << pProperties->core.name[245]
           << pProperties->core.name[246] << pProperties->core.name[247]
           << pProperties->core.name[248] << pProperties->core.name[249]
           << pProperties->core.name[250] << pProperties->core.name[251]
           << pProperties->core.name[252] << pProperties->core.name[253]
           << pProperties->core.name[254] << pProperties->core.name[255];
        ss << "}";
        ss << ", numSubdevices: " << pProperties->numSubdevices;
        ss << ", serialNumber: " << pProperties->serialNumber[0] << pProperties->serialNumber[1]
           << pProperties->serialNumber[2] << pProperties->serialNumber[3]
           << pProperties->serialNumber[4] << pProperties->serialNumber[5]
           << pProperties->serialNumber[6] << pProperties->serialNumber[7]
           << pProperties->serialNumber[8] << pProperties->serialNumber[9]
           << pProperties->serialNumber[10] << pProperties->serialNumber[11]
           << pProperties->serialNumber[12] << pProperties->serialNumber[13]
           << pProperties->serialNumber[14] << pProperties->serialNumber[15]
           << pProperties->serialNumber[16] << pProperties->serialNumber[17]
           << pProperties->serialNumber[18] << pProperties->serialNumber[19]
           << pProperties->serialNumber[20] << pProperties->serialNumber[21]
           << pProperties->serialNumber[22] << pProperties->serialNumber[23]
           << pProperties->serialNumber[24] << pProperties->serialNumber[25]
           << pProperties->serialNumber[26] << pProperties->serialNumber[27]
           << pProperties->serialNumber[28] << pProperties->serialNumber[29]
           << pProperties->serialNumber[30] << pProperties->serialNumber[31]
           << pProperties->serialNumber[32] << pProperties->serialNumber[33]
           << pProperties->serialNumber[34] << pProperties->serialNumber[35]
           << pProperties->serialNumber[36] << pProperties->serialNumber[37]
           << pProperties->serialNumber[38] << pProperties->serialNumber[39]
           << pProperties->serialNumber[40] << pProperties->serialNumber[41]
           << pProperties->serialNumber[42] << pProperties->serialNumber[43]
           << pProperties->serialNumber[44] << pProperties->serialNumber[45]
           << pProperties->serialNumber[46] << pProperties->serialNumber[47]
           << pProperties->serialNumber[48] << pProperties->serialNumber[49]
           << pProperties->serialNumber[50] << pProperties->serialNumber[51]
           << pProperties->serialNumber[52] << pProperties->serialNumber[53]
           << pProperties->serialNumber[54] << pProperties->serialNumber[55]
           << pProperties->serialNumber[56] << pProperties->serialNumber[57]
           << pProperties->serialNumber[58] << pProperties->serialNumber[59]
           << pProperties->serialNumber[60] << pProperties->serialNumber[61]
           << pProperties->serialNumber[62] << pProperties->serialNumber[63];
        ss << ", boardNumber: " << pProperties->boardNumber[0] << pProperties->boardNumber[1]
           << pProperties->boardNumber[2] << pProperties->boardNumber[3]
           << pProperties->boardNumber[4] << pProperties->boardNumber[5]
           << pProperties->boardNumber[6] << pProperties->boardNumber[7]
           << pProperties->boardNumber[8] << pProperties->boardNumber[9]
           << pProperties->boardNumber[10] << pProperties->boardNumber[11]
           << pProperties->boardNumber[12] << pProperties->boardNumber[13]
           << pProperties->boardNumber[14] << pProperties->boardNumber[15]
           << pProperties->boardNumber[16] << pProperties->boardNumber[17]
           << pProperties->boardNumber[18] << pProperties->boardNumber[19]
           << pProperties->boardNumber[20] << pProperties->boardNumber[21]
           << pProperties->boardNumber[22] << pProperties->boardNumber[23]
           << pProperties->boardNumber[24] << pProperties->boardNumber[25]
           << pProperties->boardNumber[26] << pProperties->boardNumber[27]
           << pProperties->boardNumber[28] << pProperties->boardNumber[29]
           << pProperties->boardNumber[30] << pProperties->boardNumber[31]
           << pProperties->boardNumber[32] << pProperties->boardNumber[33]
           << pProperties->boardNumber[34] << pProperties->boardNumber[35]
           << pProperties->boardNumber[36] << pProperties->boardNumber[37]
           << pProperties->boardNumber[38] << pProperties->boardNumber[39]
           << pProperties->boardNumber[40] << pProperties->boardNumber[41]
           << pProperties->boardNumber[42] << pProperties->boardNumber[43]
           << pProperties->boardNumber[44] << pProperties->boardNumber[45]
           << pProperties->boardNumber[46] << pProperties->boardNumber[47]
           << pProperties->boardNumber[48] << pProperties->boardNumber[49]
           << pProperties->boardNumber[50] << pProperties->boardNumber[51]
           << pProperties->boardNumber[52] << pProperties->boardNumber[53]
           << pProperties->boardNumber[54] << pProperties->boardNumber[55]
           << pProperties->boardNumber[56] << pProperties->boardNumber[57]
           << pProperties->boardNumber[58] << pProperties->boardNumber[59]
           << pProperties->boardNumber[60] << pProperties->boardNumber[61]
           << pProperties->boardNumber[62] << pProperties->boardNumber[63];
        ss << ", brandName: " << pProperties->brandName[0] << pProperties->brandName[1]
           << pProperties->brandName[2] << pProperties->brandName[3] << pProperties->brandName[4]
           << pProperties->brandName[5] << pProperties->brandName[6] << pProperties->brandName[7]
           << pProperties->brandName[8] << pProperties->brandName[9] << pProperties->brandName[10]
           << pProperties->brandName[11] << pProperties->brandName[12] << pProperties->brandName[13]
           << pProperties->brandName[14] << pProperties->brandName[15] << pProperties->brandName[16]
           << pProperties->brandName[17] << pProperties->brandName[18] << pProperties->brandName[19]
           << pProperties->brandName[20] << pProperties->brandName[21] << pProperties->brandName[22]
           << pProperties->brandName[23] << pProperties->brandName[24] << pProperties->brandName[25]
           << pProperties->brandName[26] << pProperties->brandName[27] << pProperties->brandName[28]
           << pProperties->brandName[29] << pProperties->brandName[30] << pProperties->brandName[31]
           << pProperties->brandName[32] << pProperties->brandName[33] << pProperties->brandName[34]
           << pProperties->brandName[35] << pProperties->brandName[36] << pProperties->brandName[37]
           << pProperties->brandName[38] << pProperties->brandName[39] << pProperties->brandName[40]
           << pProperties->brandName[41] << pProperties->brandName[42] << pProperties->brandName[43]
           << pProperties->brandName[44] << pProperties->brandName[45] << pProperties->brandName[46]
           << pProperties->brandName[47] << pProperties->brandName[48] << pProperties->brandName[49]
           << pProperties->brandName[50] << pProperties->brandName[51] << pProperties->brandName[52]
           << pProperties->brandName[53] << pProperties->brandName[54] << pProperties->brandName[55]
           << pProperties->brandName[56] << pProperties->brandName[57] << pProperties->brandName[58]
           << pProperties->brandName[59] << pProperties->brandName[60] << pProperties->brandName[61]
           << pProperties->brandName[62] << pProperties->brandName[63];
        ss << ", modelName: " << pProperties->modelName[0] << pProperties->modelName[1]
           << pProperties->modelName[2] << pProperties->modelName[3] << pProperties->modelName[4]
           << pProperties->modelName[5] << pProperties->modelName[6] << pProperties->modelName[7]
           << pProperties->modelName[8] << pProperties->modelName[9] << pProperties->modelName[10]
           << pProperties->modelName[11] << pProperties->modelName[12] << pProperties->modelName[13]
           << pProperties->modelName[14] << pProperties->modelName[15] << pProperties->modelName[16]
           << pProperties->modelName[17] << pProperties->modelName[18] << pProperties->modelName[19]
           << pProperties->modelName[20] << pProperties->modelName[21] << pProperties->modelName[22]
           << pProperties->modelName[23] << pProperties->modelName[24] << pProperties->modelName[25]
           << pProperties->modelName[26] << pProperties->modelName[27] << pProperties->modelName[28]
           << pProperties->modelName[29] << pProperties->modelName[30] << pProperties->modelName[31]
           << pProperties->modelName[32] << pProperties->modelName[33] << pProperties->modelName[34]
           << pProperties->modelName[35] << pProperties->modelName[36] << pProperties->modelName[37]
           << pProperties->modelName[38] << pProperties->modelName[39] << pProperties->modelName[40]
           << pProperties->modelName[41] << pProperties->modelName[42] << pProperties->modelName[43]
           << pProperties->modelName[44] << pProperties->modelName[45] << pProperties->modelName[46]
           << pProperties->modelName[47] << pProperties->modelName[48] << pProperties->modelName[49]
           << pProperties->modelName[50] << pProperties->modelName[51] << pProperties->modelName[52]
           << pProperties->modelName[53] << pProperties->modelName[54] << pProperties->modelName[55]
           << pProperties->modelName[56] << pProperties->modelName[57] << pProperties->modelName[58]
           << pProperties->modelName[59] << pProperties->modelName[60] << pProperties->modelName[61]
           << pProperties->modelName[62] << pProperties->modelName[63];
        ss << ", vendorName: " << pProperties->vendorName[0] << pProperties->vendorName[1]
           << pProperties->vendorName[2] << pProperties->vendorName[3] << pProperties->vendorName[4]
           << pProperties->vendorName[5] << pProperties->vendorName[6] << pProperties->vendorName[7]
           << pProperties->vendorName[8] << pProperties->vendorName[9]
           << pProperties->vendorName[10] << pProperties->vendorName[11]
           << pProperties->vendorName[12] << pProperties->vendorName[13]
           << pProperties->vendorName[14] << pProperties->vendorName[15]
           << pProperties->vendorName[16] << pProperties->vendorName[17]
           << pProperties->vendorName[18] << pProperties->vendorName[19]
           << pProperties->vendorName[20] << pProperties->vendorName[21]
           << pProperties->vendorName[22] << pProperties->vendorName[23]
           << pProperties->vendorName[24] << pProperties->vendorName[25]
           << pProperties->vendorName[26] << pProperties->vendorName[27]
           << pProperties->vendorName[28] << pProperties->vendorName[29]
           << pProperties->vendorName[30] << pProperties->vendorName[31]
           << pProperties->vendorName[32] << pProperties->vendorName[33]
           << pProperties->vendorName[34] << pProperties->vendorName[35]
           << pProperties->vendorName[36] << pProperties->vendorName[37]
           << pProperties->vendorName[38] << pProperties->vendorName[39]
           << pProperties->vendorName[40] << pProperties->vendorName[41]
           << pProperties->vendorName[42] << pProperties->vendorName[43]
           << pProperties->vendorName[44] << pProperties->vendorName[45]
           << pProperties->vendorName[46] << pProperties->vendorName[47]
           << pProperties->vendorName[48] << pProperties->vendorName[49]
           << pProperties->vendorName[50] << pProperties->vendorName[51]
           << pProperties->vendorName[52] << pProperties->vendorName[53]
           << pProperties->vendorName[54] << pProperties->vendorName[55]
           << pProperties->vendorName[56] << pProperties->vendorName[57]
           << pProperties->vendorName[58] << pProperties->vendorName[59]
           << pProperties->vendorName[60] << pProperties->vendorName[61]
           << pProperties->vendorName[62] << pProperties->vendorName[63];
        ss << ", driverVersion: " << pProperties->driverVersion[0] << pProperties->driverVersion[1]
           << pProperties->driverVersion[2] << pProperties->driverVersion[3]
           << pProperties->driverVersion[4] << pProperties->driverVersion[5]
           << pProperties->driverVersion[6] << pProperties->driverVersion[7]
           << pProperties->driverVersion[8] << pProperties->driverVersion[9]
           << pProperties->driverVersion[10] << pProperties->driverVersion[11]
           << pProperties->driverVersion[12] << pProperties->driverVersion[13]
           << pProperties->driverVersion[14] << pProperties->driverVersion[15]
           << pProperties->driverVersion[16] << pProperties->driverVersion[17]
           << pProperties->driverVersion[18] << pProperties->driverVersion[19]
           << pProperties->driverVersion[20] << pProperties->driverVersion[21]
           << pProperties->driverVersion[22] << pProperties->driverVersion[23]
           << pProperties->driverVersion[24] << pProperties->driverVersion[25]
           << pProperties->driverVersion[26] << pProperties->driverVersion[27]
           << pProperties->driverVersion[28] << pProperties->driverVersion[29]
           << pProperties->driverVersion[30] << pProperties->driverVersion[31]
           << pProperties->driverVersion[32] << pProperties->driverVersion[33]
           << pProperties->driverVersion[34] << pProperties->driverVersion[35]
           << pProperties->driverVersion[36] << pProperties->driverVersion[37]
           << pProperties->driverVersion[38] << pProperties->driverVersion[39]
           << pProperties->driverVersion[40] << pProperties->driverVersion[41]
           << pProperties->driverVersion[42] << pProperties->driverVersion[43]
           << pProperties->driverVersion[44] << pProperties->driverVersion[45]
           << pProperties->driverVersion[46] << pProperties->driverVersion[47]
           << pProperties->driverVersion[48] << pProperties->driverVersion[49]
           << pProperties->driverVersion[50] << pProperties->driverVersion[51]
           << pProperties->driverVersion[52] << pProperties->driverVersion[53]
           << pProperties->driverVersion[54] << pProperties->driverVersion[55]
           << pProperties->driverVersion[56] << pProperties->driverVersion[57]
           << pProperties->driverVersion[58] << pProperties->driverVersion[59]
           << pProperties->driverVersion[60] << pProperties->driverVersion[61]
           << pProperties->driverVersion[62] << pProperties->driverVersion[63];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceGetState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceReset(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceResetExt(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceProcessesGetState(";
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pProcesses == nullptr) {
        ss << ", pProcesses: nullptr";
    } else {
        ss << ", pProcesses {";
        ss << "stype: " << pProcesses->stype;
        ss << ", pNext: " << pProcesses->pNext;
        ss << ", processId: " << pProcesses->processId;
        ss << ", memSize: " << pProcesses->memSize;
        ss << ", sharedSize: " << pProcesses->sharedSize;
        ss << ", engines: " << pProcesses->engines;
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDevicePciGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDevicePciGetState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDevicePciGetBars(";
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", type: " << pProperties->type;
        ss << ", index: " << pProperties->index;
        ss << ", base: " << pProperties->base;
        ss << ", size: " << pProperties->size;
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDevicePciGetStats(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceSetOverclockWaiver(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceGetOverclockDomains(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceGetOverclockControls(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceResetOverclockSettings(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceReadOverclockState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumOverclockDomains(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesOverclockGetDomainProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesOverclockGetDomainVFProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesOverclockGetDomainControlProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesOverclockGetControlCurrentValue(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesOverclockGetControlPendingValue(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesOverclockSetControlUserValue(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesOverclockGetControlState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesOverclockGetVFPointValues(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesOverclockSetVFPointValues(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumDiagnosticTestSuites(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDiagnosticsGetProperties(";
    ss << "hDiagnostics: " << hDiagnostics;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", onSubdevice: " << pProperties->onSubdevice;
        ss << ", subdeviceId: " << pProperties->subdeviceId;
        ss << ", name: " << pProperties->name[0] << pProperties->name[1] << pProperties->name[2]
           << pProperties->name[3] << pProperties->name[4] << pProperties->name[5]
           << pProperties->name[6] << pProperties->name[7] << pProperties->name[8]
           << pProperties->name[9] << pProperties->name[10] << pProperties->name[11]
           << pProperties->name[12] << pProperties->name[13] << pProperties->name[14]
           << pProperties->name[15] << pProperties->name[16] << pProperties->name[17]
           << pProperties->name[18] << pProperties->name[19] << pProperties->name[20]
           << pProperties->name[21] << pProperties->name[22] << pProperties->name[23]
           << pProperties->name[24] << pProperties->name[25] << pProperties->name[26]
           << pProperties->name[27] << pProperties->name[28] << pProperties->name[29]
           << pProperties->name[30] << pProperties->name[31] << pProperties->name[32]
           << pProperties->name[33] << pProperties->name[34] << pProperties->name[35]
           << pProperties->name[36] << pProperties->name[37] << pProperties->name[38]
           << pProperties->name[39] << pProperties->name[40] << pProperties->name[41]
           << pProperties->name[42] << pProperties->name[43] << pProperties->name[44]
           << pProperties->name[45] << pProperties->name[46] << pProperties->name[47]
           << pProperties->name[48] << pProperties->name[49] << pProperties->name[50]
           << pProperties->name[51] << pProperties->name[52] << pProperties->name[53]
           << pProperties->name[54] << pProperties->name[55] << pProperties->name[56]
           << pProperties->name[57] << pProperties->name[58] << pProperties->name[59]
           << pProperties->name[60] << pProperties->name[61] << pProperties->name[62]
           << pProperties->name[63];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDiagnosticsGetTests(";
    ss << "hDiagnostics: " << hDiagnostics;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pTests == nullptr) {
        ss << ", pTests: nullptr";
    } else {
        ss << ", pTests {";
        ss << "index: " << pTests->index;
        ss << ", name: " << pTests->name[0] << pTests->name[1] << pTests->name[2] << pTests->name[3]
           << pTests->name[4] << pTests->name[5] << pTests->name[6] << pTests->name[7]
           << pTests->name[8] << pTests->name[9] << pTests->name[10] << pTests->name[11]
           << pTests->name[12] << pTests->name[13] << pTests->name[14] << pTests->name[15]
           << pTests->name[16] << pTests->name[17] << pTests->name[18] << pTests->name[19]
           << pTests->name[20] << pTests->name[21] << pTests->name[22] << pTests->name[23]
           << pTests->name[24] << pTests->name[25] << pTests->name[26] << pTests->name[27]
           << pTests->name[28] << pTests->name[29] << pTests->name[30] << pTests->name[31]
           << pTests->name[32] << pTests->name[33] << pTests->name[34] << pTests->name[35]
           << pTests->name[36] << pTests->name[37] << pTests->name[38] << pTests->name[39]
           << pTests->name[40] << pTests->name[41] << pTests->name[42] << pTests->name[43]
           << pTests->name[44] << pTests->name[45] << pTests->name[46] << pTests->name[47]
           << pTests->name[48] << pTests->name[49] << pTests->name[50] << pTests->name[51]
           << pTests->name[52] << pTests->name[53] << pTests->name[54] << pTests->name[55]
           << pTests->name[56] << pTests->name[57] << pTests->name[58] << pTests->name[59]
           << pTests->name[60] << pTests->name[61] << pTests->name[62] << pTests->name[63];
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDiagnosticsRunTests(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEccAvailable(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEccConfigurable(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceGetEccState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceSetEccState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumEngineGroups(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesEngineGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesEngineGetActivity(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEventRegister(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDriverEventListen(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDriverEventListenEx(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumFabricPorts(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFabricPortGetProperties(";
    ss << "hPort: " << hPort;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", model: " << pProperties->model[0] << pProperties->model[1] << pProperties->model[2]
           << pProperties->model[3] << pProperties->model[4] << pProperties->model[5]
           << pProperties->model[6] << pProperties->model[7] << pProperties->model[8]
           << pProperties->model[9] << pProperties->model[10] << pProperties->model[11]
           << pProperties->model[12] << pProperties->model[13] << pProperties->model[14]
           << pProperties->model[15] << pProperties->model[16] << pProperties->model[17]
           << pProperties->model[18] << pProperties->model[19] << pProperties->model[20]
           << pProperties->model[21] << pProperties->model[22] << pProperties->model[23]
           << pProperties->model[24] << pProperties->model[25] << pProperties->model[26]
           << pProperties->model[27] << pProperties->model[28] << pProperties->model[29]
           << pProperties->model[30] << pProperties->model[31] << pProperties->model[32]
           << pProperties->model[33] << pProperties->model[34] << pProperties->model[35]
           << pProperties->model[36] << pProperties->model[37] << pProperties->model[38]
           << pProperties->model[39] << pProperties->model[40] << pProperties->model[41]
           << pProperties->model[42] << pProperties->model[43] << pProperties->model[44]
           << pProperties->model[45] << pProperties->model[46] << pProperties->model[47]
           << pProperties->model[48] << pProperties->model[49] << pProperties->model[50]
           << pProperties->model[51] << pProperties->model[52] << pProperties->model[53]
           << pProperties->model[54] << pProperties->model[55] << pProperties->model[56]
           << pProperties->model[57] << pProperties->model[58] << pProperties->model[59]
           << pProperties->model[60] << pProperties->model[61] << pProperties->model[62]
           << pProperties->model[63] << pProperties->model[64] << pProperties->model[65]
           << pProperties->model[66] << pProperties->model[67] << pProperties->model[68]
           << pProperties->model[69] << pProperties->model[70] << pProperties->model[71]
           << pProperties->model[72] << pProperties->model[73] << pProperties->model[74]
           << pProperties->model[75] << pProperties->model[76] << pProperties->model[77]
           << pProperties->model[78] << pProperties->model[79] << pProperties->model[80]
           << pProperties->model[81] << pProperties->model[82] << pProperties->model[83]
           << pProperties->model[84] << pProperties->model[85] << pProperties->model[86]
           << pProperties->model[87] << pProperties->model[88] << pProperties->model[89]
           << pProperties->model[90] << pProperties->model[91] << pProperties->model[92]
           << pProperties->model[93] << pProperties->model[94] << pProperties->model[95]
           << pProperties->model[96] << pProperties->model[97] << pProperties->model[98]
           << pProperties->model[99] << pProperties->model[100] << pProperties->model[101]
           << pProperties->model[102] << pProperties->model[103] << pProperties->model[104]
           << pProperties->model[105] << pProperties->model[106] << pProperties->model[107]
           << pProperties->model[108] << pProperties->model[109] << pProperties->model[110]
           << pProperties->model[111] << pProperties->model[112] << pProperties->model[113]
           << pProperties->model[114] << pProperties->model[115] << pProperties->model[116]
           << pProperties->model[117] << pProperties->model[118] << pProperties->model[119]
           << pProperties->model[120] << pProperties->model[121] << pProperties->model[122]
           << pProperties->model[123] << pProperties->model[124] << pProperties->model[125]
           << pProperties->model[126] << pProperties->model[127] << pProperties->model[128]
           << pProperties->model[129] << pProperties->model[130] << pProperties->model[131]
           << pProperties->model[132] << pProperties->model[133] << pProperties->model[134]
           << pProperties->model[135] << pProperties->model[136] << pProperties->model[137]
           << pProperties->model[138] << pProperties->model[139] << pProperties->model[140]
           << pProperties->model[141] << pProperties->model[142] << pProperties->model[143]
           << pProperties->model[144] << pProperties->model[145] << pProperties->model[146]
           << pProperties->model[147] << pProperties->model[148] << pProperties->model[149]
           << pProperties->model[150] << pProperties->model[151] << pProperties->model[152]
           << pProperties->model[153] << pProperties->model[154] << pProperties->model[155]
           << pProperties->model[156] << pProperties->model[157] << pProperties->model[158]
           << pProperties->model[159] << pProperties->model[160] << pProperties->model[161]
           << pProperties->model[162] << pProperties->model[163] << pProperties->model[164]
           << pProperties->model[165] << pProperties->model[166] << pProperties->model[167]
           << pProperties->model[168] << pProperties->model[169] << pProperties->model[170]
           << pProperties->model[171] << pProperties->model[172] << pProperties->model[173]
           << pProperties->model[174] << pProperties->model[175] << pProperties->model[176]
           << pProperties->model[177] << pProperties->model[178] << pProperties->model[179]
           << pProperties->model[180] << pProperties->model[181] << pProperties->model[182]
           << pProperties->model[183] << pProperties->model[184] << pProperties->model[185]
           << pProperties->model[186] << pProperties->model[187] << pProperties->model[188]
           << pProperties->model[189] << pProperties->model[190] << pProperties->model[191]
           << pProperties->model[192] << pProperties->model[193] << pProperties->model[194]
           << pProperties->model[195] << pProperties->model[196] << pProperties->model[197]
           << pProperties->model[198] << pProperties->model[199] << pProperties->model[200]
           << pProperties->model[201] << pProperties->model[202] << pProperties->model[203]
           << pProperties->model[204] << pProperties->model[205] << pProperties->model[206]
           << pProperties->model[207] << pProperties->model[208] << pProperties->model[209]
           << pProperties->model[210] << pProperties->model[211] << pProperties->model[212]
           << pProperties->model[213] << pProperties->model[214] << pProperties->model[215]
           << pProperties->model[216] << pProperties->model[217] << pProperties->model[218]
           << pProperties->model[219] << pProperties->model[220] << pProperties->model[221]
           << pProperties->model[222] << pProperties->model[223] << pProperties->model[224]
           << pProperties->model[225] << pProperties->model[226] << pProperties->model[227]
           << pProperties->model[228] << pProperties->model[229] << pProperties->model[230]
           << pProperties->model[231] << pProperties->model[232] << pProperties->model[233]
           << pProperties->model[234] << pProperties->model[235] << pProperties->model[236]
           << pProperties->model[237] << pProperties->model[238] << pProperties->model[239]
           << pProperties->model[240] << pProperties->model[241] << pProperties->model[242]
           << pProperties->model[243] << pProperties->model[244] << pProperties->model[245]
           << pProperties->model[246] << pProperties->model[247] << pProperties->model[248]
           << pProperties->model[249] << pProperties->model[250] << pProperties->model[251]
           << pProperties->model[252] << pProperties->model[253] << pProperties->model[254]
           << pProperties->model[255];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFabricPortGetLinkType(";
    ss << "hPort: " << hPort;
    if (pLinkType == nullptr) {
        ss << ", pLinkType: nullptr";
    } else {
        ss << ", pLinkType {";
        ss << "desc: " << pLinkType->desc[0] << pLinkType->desc[1] << pLinkType->desc[2]
           << pLinkType->desc[3] << pLinkType->desc[4] << pLinkType->desc[5] << pLinkType->desc[6]
           << pLinkType->desc[7] << pLinkType->desc[8] << pLinkType->desc[9] << pLinkType->desc[10]
           << pLinkType->desc[11] << pLinkType->desc[12] << pLinkType->desc[13]
           << pLinkType->desc[14] << pLinkType->desc[15] << pLinkType->desc[16]
           << pLinkType->desc[17] << pLinkType->desc[18] << pLinkType->desc[19]
           << pLinkType->desc[20] << pLinkType->desc[21] << pLinkType->desc[22]
           << pLinkType->desc[23] << pLinkType->desc[24] << pLinkType->desc[25]
           << pLinkType->desc[26] << pLinkType->desc[27] << pLinkType->desc[28]
           << pLinkType->desc[29] << pLinkType->desc[30] << pLinkType->desc[31]
           << pLinkType->desc[32] << pLinkType->desc[33] << pLinkType->desc[34]
           << pLinkType->desc[35] << pLinkType->desc[36] << pLinkType->desc[37]
           << pLinkType->desc[38] << pLinkType->desc[39] << pLinkType->desc[40]
           << pLinkType->desc[41] << pLinkType->desc[42] << pLinkType->desc[43]
           << pLinkType->desc[44] << pLinkType->desc[45] << pLinkType->desc[46]
           << pLinkType->desc[47] << pLinkType->desc[48] << pLinkType->desc[49]
           << pLinkType->desc[50] << pLinkType->desc[51] << pLinkType->desc[52]
           << pLinkType->desc[53] << pLinkType->desc[54] << pLinkType->desc[55]
           << pLinkType->desc[56] << pLinkType->desc[57] << pLinkType->desc[58]
           << pLinkType->desc[59] << pLinkType->desc[60] << pLinkType->desc[61]
           << pLinkType->desc[62] << pLinkType->desc[63] << pLinkType->desc[64]
           << pLinkType->desc[65] << pLinkType->desc[66] << pLinkType->desc[67]
           << pLinkType->desc[68] << pLinkType->desc[69] << pLinkType->desc[70]
           << pLinkType->desc[71] << pLinkType->desc[72] << pLinkType->desc[73]
           << pLinkType->desc[74] << pLinkType->desc[75] << pLinkType->desc[76]
           << pLinkType->desc[77] << pLinkType->desc[78] << pLinkType->desc[79]
           << pLinkType->desc[80] << pLinkType->desc[81] << pLinkType->desc[82]
           << pLinkType->desc[83] << pLinkType->desc[84] << pLinkType->desc[85]
           << pLinkType->desc[86] << pLinkType->desc[87] << pLinkType->desc[88]
           << pLinkType->desc[89] << pLinkType->desc[90] << pLinkType->desc[91]
           << pLinkType->desc[92] << pLinkType->desc[93] << pLinkType->desc[94]
           << pLinkType->desc[95] << pLinkType->desc[96] << pLinkType->desc[97]
           << pLinkType->desc[98] << pLinkType->desc[99] << pLinkType->desc[100]
           << pLinkType->desc[101] << pLinkType->desc[102] << pLinkType->desc[103]
           << pLinkType->desc[104] << pLinkType->desc[105] << pLinkType->desc[106]
           << pLinkType->desc[107] << pLinkType->desc[108] << pLinkType->desc[109]
           << pLinkType->desc[110] << pLinkType->desc[111] << pLinkType->desc[112]
           << pLinkType->desc[113] << pLinkType->desc[114] << pLinkType->desc[115]
           << pLinkType->desc[116] << pLinkType->desc[117] << pLinkType->desc[118]
           << pLinkType->desc[119] << pLinkType->desc[120] << pLinkType->desc[121]
           << pLinkType->desc[122] << pLinkType->desc[123] << pLinkType->desc[124]
           << pLinkType->desc[125] << pLinkType->desc[126] << pLinkType->desc[127]
           << pLinkType->desc[128] << pLinkType->desc[129] << pLinkType->desc[130]
           << pLinkType->desc[131] << pLinkType->desc[132] << pLinkType->desc[133]
           << pLinkType->desc[134] << pLinkType->desc[135] << pLinkType->desc[136]
           << pLinkType->desc[137] << pLinkType->desc[138] << pLinkType->desc[139]
           << pLinkType->desc[140] << pLinkType->desc[141] << pLinkType->desc[142]
           << pLinkType->desc[143] << pLinkType->desc[144] << pLinkType->desc[145]
           << pLinkType->desc[146] << pLinkType->desc[147] << pLinkType->desc[148]
           << pLinkType->desc[149] << pLinkType->desc[150] << pLinkType->desc[151]
           << pLinkType->desc[152] << pLinkType->desc[153] << pLinkType->desc[154]
           << pLinkType->desc[155] << pLinkType->desc[156] << pLinkType->desc[157]
           << pLinkType->desc[158] << pLinkType->desc[159] << pLinkType->desc[160]
           << pLinkType->desc[161] << pLinkType->desc[162] << pLinkType->desc[163]
           << pLinkType->desc[164] << pLinkType->desc[165] << pLinkType->desc[166]
           << pLinkType->desc[167] << pLinkType->desc[168] << pLinkType->desc[169]
           << pLinkType->desc[170] << pLinkType->desc[171] << pLinkType->desc[172]
           << pLinkType->desc[173] << pLinkType->desc[174] << pLinkType->desc[175]
           << pLinkType->desc[176] << pLinkType->desc[177] << pLinkType->desc[178]
           << pLinkType->desc[179] << pLinkType->desc[180] << pLinkType->desc[181]
           << pLinkType->desc[182] << pLinkType->desc[183] << pLinkType->desc[184]
           << pLinkType->desc[185] << pLinkType->desc[186] << pLinkType->desc[187]
           << pLinkType->desc[188] << pLinkType->desc[189] << pLinkType->desc[190]
           << pLinkType->desc[191] << pLinkType->desc[192] << pLinkType->desc[193]
           << pLinkType->desc[194] << pLinkType->desc[195] << pLinkType->desc[196]
           << pLinkType->desc[197] << pLinkType->desc[198] << pLinkType->desc[199]
           << pLinkType->desc[200] << pLinkType->desc[201] << pLinkType->desc[202]
           << pLinkType->desc[203] << pLinkType->desc[204] << pLinkType->desc[205]
           << pLinkType->desc[206] << pLinkType->desc[207] << pLinkType->desc[208]
           << pLinkType->desc[209] << pLinkType->desc[210] << pLinkType->desc[211]
           << pLinkType->desc[212] << pLinkType->desc[213] << pLinkType->desc[214]
           << pLinkType->desc[215] << pLinkType->desc[216] << pLinkType->desc[217]
           << pLinkType->desc[218] << pLinkType->desc[219] << pLinkType->desc[220]
           << pLinkType->desc[221] << pLinkType->desc[222] << pLinkType->desc[223]
           << pLinkType->desc[224] << pLinkType->desc[225] << pLinkType->desc[226]
           << pLinkType->desc[227] << pLinkType->desc[228] << pLinkType->desc[229]
           << pLinkType->desc[230] << pLinkType->desc[231] << pLinkType->desc[232]
           << pLinkType->desc[233] << pLinkType->desc[234] << pLinkType->desc[235]
           << pLinkType->desc[236] << pLinkType->desc[237] << pLinkType->desc[238]
           << pLinkType->desc[239] << pLinkType->desc[240] << pLinkType->desc[241]
           << pLinkType->desc[242] << pLinkType->desc[243] << pLinkType->desc[244]
           << pLinkType->desc[245] << pLinkType->desc[246] << pLinkType->desc[247]
           << pLinkType->desc[248] << pLinkType->desc[249] << pLinkType->desc[250]
           << pLinkType->desc[251] << pLinkType->desc[252] << pLinkType->desc[253]
           << pLinkType->desc[254] << pLinkType->desc[255];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFabricPortGetConfig(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFabricPortSetConfig(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFabricPortGetState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFabricPortGetThroughput(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFabricPortGetFabricErrorCounters(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFabricPortGetMultiPortThroughput(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumFans(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFanGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFanGetConfig(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFanSetDefaultMode(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFanSetFixedSpeedMode(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFanSetSpeedTableMode(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFanGetState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumFirmwares(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFirmwareGetProperties(";
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
        ss << ", name: " << pProperties->name[0] << pProperties->name[1] << pProperties->name[2]
           << pProperties->name[3] << pProperties->name[4] << pProperties->name[5]
           << pProperties->name[6] << pProperties->name[7] << pProperties->name[8]
           << pProperties->name[9] << pProperties->name[10] << pProperties->name[11]
           << pProperties->name[12] << pProperties->name[13] << pProperties->name[14]
           << pProperties->name[15] << pProperties->name[16] << pProperties->name[17]
           << pProperties->name[18] << pProperties->name[19] << pProperties->name[20]
           << pProperties->name[21] << pProperties->name[22] << pProperties->name[23]
           << pProperties->name[24] << pProperties->name[25] << pProperties->name[26]
           << pProperties->name[27] << pProperties->name[28] << pProperties->name[29]
           << pProperties->name[30] << pProperties->name[31] << pProperties->name[32]
           << pProperties->name[33] << pProperties->name[34] << pProperties->name[35]
           << pProperties->name[36] << pProperties->name[37] << pProperties->name[38]
           << pProperties->name[39] << pProperties->name[40] << pProperties->name[41]
           << pProperties->name[42] << pProperties->name[43] << pProperties->name[44]
           << pProperties->name[45] << pProperties->name[46] << pProperties->name[47]
           << pProperties->name[48] << pProperties->name[49] << pProperties->name[50]
           << pProperties->name[51] << pProperties->name[52] << pProperties->name[53]
           << pProperties->name[54] << pProperties->name[55] << pProperties->name[56]
           << pProperties->name[57] << pProperties->name[58] << pProperties->name[59]
           << pProperties->name[60] << pProperties->name[61] << pProperties->name[62]
           << pProperties->name[63];
        ss << ", version: " << pProperties->version[0] << pProperties->version[1]
           << pProperties->version[2] << pProperties->version[3] << pProperties->version[4]
           << pProperties->version[5] << pProperties->version[6] << pProperties->version[7]
           << pProperties->version[8] << pProperties->version[9] << pProperties->version[10]
           << pProperties->version[11] << pProperties->version[12] << pProperties->version[13]
           << pProperties->version[14] << pProperties->version[15] << pProperties->version[16]
           << pProperties->version[17] << pProperties->version[18] << pProperties->version[19]
           << pProperties->version[20] << pProperties->version[21] << pProperties->version[22]
           << pProperties->version[23] << pProperties->version[24] << pProperties->version[25]
           << pProperties->version[26] << pProperties->version[27] << pProperties->version[28]
           << pProperties->version[29] << pProperties->version[30] << pProperties->version[31]
           << pProperties->version[32] << pProperties->version[33] << pProperties->version[34]
           << pProperties->version[35] << pProperties->version[36] << pProperties->version[37]
           << pProperties->version[38] << pProperties->version[39] << pProperties->version[40]
           << pProperties->version[41] << pProperties->version[42] << pProperties->version[43]
           << pProperties->version[44] << pProperties->version[45] << pProperties->version[46]
           << pProperties->version[47] << pProperties->version[48] << pProperties->version[49]
           << pProperties->version[50] << pProperties->version[51] << pProperties->version[52]
           << pProperties->version[53] << pProperties->version[54] << pProperties->version[55]
           << pProperties->version[56] << pProperties->version[57] << pProperties->version[58]
           << pProperties->version[59] << pProperties->version[60] << pProperties->version[61]
           << pProperties->version[62] << pProperties->version[63];
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFirmwareFlash(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFirmwareGetFlashProgress(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFirmwareGetConsoleLogs(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumFrequencyDomains(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyGetAvailableClocks(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyGetRange(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencySetRange(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyGetState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyGetThrottleTime(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyOcGetCapabilities(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyOcGetFrequencyTarget(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyOcSetFrequencyTarget(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyOcGetVoltageTarget(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyOcSetVoltageTarget(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyOcSetMode(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyOcGetMode(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyOcGetIccMax(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyOcSetIccMax(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyOcGetTjMax(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFrequencyOcSetTjMax(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumLeds(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesLedGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesLedGetState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesLedSetState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesLedSetColor(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumMemoryModules(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesMemoryGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesMemoryGetState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesMemoryGetBandwidth(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumPerformanceFactorDomains(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPerformanceFactorGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPerformanceFactorGetConfig(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPerformanceFactorSetConfig(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumPowerDomains(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceGetCardPowerDomain(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPowerGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPowerGetEnergyCounter(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPowerGetLimits(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPowerSetLimits(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPowerGetEnergyThreshold(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPowerSetEnergyThreshold(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumPsus(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPsuGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPsuGetState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumRasErrorSets(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesRasGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesRasGetConfig(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesRasSetConfig(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesRasGetState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumSchedulers(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesSchedulerGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesSchedulerGetCurrentMode(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesSchedulerGetTimeoutModeProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesSchedulerGetTimesliceModeProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesSchedulerSetTimeoutMode(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesSchedulerSetTimesliceMode(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesSchedulerSetExclusiveMode(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesSchedulerSetComputeUnitDebugMode(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumStandbyDomains(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesStandbyGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesStandbyGetMode(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesStandbySetMode(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumTemperatureSensors(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesTemperatureGetProperties(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesTemperatureGetConfig(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesTemperatureSetConfig(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesTemperatureGetState(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPowerGetLimitsExt(";
    ss << "hPower: " << hPower;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pSustained == nullptr) {
        ss << ", pSustained: nullptr";
    } else {
        ss << ", pSustained {";
        ss << "stype: " << pSustained->stype;
        ss << ", pNext: " << pSustained->pNext;
        ss << ", level: " << pSustained->level;
        ss << ", source: " << pSustained->source;
        ss << ", limitUnit: " << pSustained->limitUnit;
        ss << ", enabledStateLocked: " << pSustained->enabledStateLocked;
        ss << ", enabled: " << pSustained->enabled;
        ss << ", intervalValueLocked: " << pSustained->intervalValueLocked;
        ss << ", interval: " << pSustained->interval;
        ss << ", limitValueLocked: " << pSustained->limitValueLocked;
        ss << ", limit: " << pSustained->limit;
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesPowerSetLimitsExt(";
    ss << "hPower: " << hPower;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pSustained == nullptr) {
        ss << ", pSustained: nullptr";
    } else {
        ss << ", pSustained {";
        ss << "stype: " << pSustained->stype;
        ss << ", pNext: " << pSustained->pNext;
        ss << ", level: " << pSustained->level;
        ss << ", source: " << pSustained->source;
        ss << ", limitUnit: " << pSustained->limitUnit;
        ss << ", enabledStateLocked: " << pSustained->enabledStateLocked;
        ss << ", enabled: " << pSustained->enabled;
        ss << ", intervalValueLocked: " << pSustained->intervalValueLocked;
        ss << ", interval: " << pSustained->interval;
        ss << ", limitValueLocked: " << pSustained->limitValueLocked;
        ss << ", limit: " << pSustained->limit;
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesEngineGetActivityExt(";
    ss << "hEngine: " << hEngine;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesRasGetStateExp(";
    ss << "hRas: " << hRas;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pState == nullptr) {
        ss << ", pState: nullptr";
    } else {
        ss << ", pState {";
        ss << "category: " << pState->category;
        ss << ", errorCounter: " << pState->errorCounter;
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesRasClearStateExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFirmwareGetSecurityVersionExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesFirmwareSetSecurityVersionExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceGetSubDevicePropertiesExp(";
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pSubdeviceProps == nullptr) {
        ss << ", pSubdeviceProps: nullptr";
    } else {
        ss << ", pSubdeviceProps {";
        ss << "stype: " << pSubdeviceProps->stype;
        ss << ", pNext: " << pSubdeviceProps->pNext;
        ss << ", subdeviceId: " << pSubdeviceProps->subdeviceId;
        ss << ", uuid {";
        ss << "id: " << static_cast<int>(pSubdeviceProps->uuid.id[0]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[1]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[2]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[3]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[4]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[5]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[6]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[7]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[8]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[9]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[10]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[11]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[12]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[13]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[14]) << " "
           << static_cast<int>(pSubdeviceProps->uuid.id[15]);
        ss << "}";
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDriverGetDeviceByUuidExp(";
    ss << "hDriver: " << hDriver;
    ss << ", uuid {";
    ss << "id: " << static_cast<int>(uuid.id[0]) << " " << static_cast<int>(uuid.id[1]) << " "
       << static_cast<int>(uuid.id[2]) << " " << static_cast<int>(uuid.id[3]) << " "
       << static_cast<int>(uuid.id[4]) << " " << static_cast<int>(uuid.id[5]) << " "
       << static_cast<int>(uuid.id[6]) << " " << static_cast<int>(uuid.id[7]) << " "
       << static_cast<int>(uuid.id[8]) << " " << static_cast<int>(uuid.id[9]) << " "
       << static_cast<int>(uuid.id[10]) << " " << static_cast<int>(uuid.id[11]) << " "
       << static_cast<int>(uuid.id[12]) << " " << static_cast<int>(uuid.id[13]) << " "
       << static_cast<int>(uuid.id[14]) << " " << static_cast<int>(uuid.id[15]);
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumActiveVFExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesVFManagementGetVFPropertiesExp(";
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
        ss << "id: " << static_cast<int>(pProperties->uuid.id[0]) << " "
           << static_cast<int>(pProperties->uuid.id[1]) << " "
           << static_cast<int>(pProperties->uuid.id[2]) << " "
           << static_cast<int>(pProperties->uuid.id[3]) << " "
           << static_cast<int>(pProperties->uuid.id[4]) << " "
           << static_cast<int>(pProperties->uuid.id[5]) << " "
           << static_cast<int>(pProperties->uuid.id[6]) << " "
           << static_cast<int>(pProperties->uuid.id[7]) << " "
           << static_cast<int>(pProperties->uuid.id[8]) << " "
           << static_cast<int>(pProperties->uuid.id[9]) << " "
           << static_cast<int>(pProperties->uuid.id[10]) << " "
           << static_cast<int>(pProperties->uuid.id[11]) << " "
           << static_cast<int>(pProperties->uuid.id[12]) << " "
           << static_cast<int>(pProperties->uuid.id[13]) << " "
           << static_cast<int>(pProperties->uuid.id[14]) << " "
           << static_cast<int>(pProperties->uuid.id[15]);
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesVFManagementGetVFMemoryUtilizationExp(";
    ss << "hVFhandle: " << hVFhandle;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pMemUtil == nullptr) {
        ss << ", pMemUtil: nullptr";
    } else {
        ss << ", pMemUtil {";
        ss << "stype: " << pMemUtil->stype;
        ss << ", pNext: " << pMemUtil->pNext;
        ss << ", memTypeFlags: " << pMemUtil->memTypeFlags;
        ss << ", free: " << pMemUtil->free;
        ss << ", size: " << pMemUtil->size;
        ss << ", timestamp: " << pMemUtil->timestamp;
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesVFManagementGetVFEngineUtilizationExp(";
    ss << "hVFhandle: " << hVFhandle;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pEngineUtil == nullptr) {
        ss << ", pEngineUtil: nullptr";
    } else {
        ss << ", pEngineUtil {";
        ss << "stype: " << pEngineUtil->stype;
        ss << ", pNext: " << pEngineUtil->pNext;
        ss << ", type: " << pEngineUtil->type;
        ss << ", activeCounterValue: " << pEngineUtil->activeCounterValue;
        ss << ", samplingCounterValue: " << pEngineUtil->samplingCounterValue;
        ss << ", timestamp: " << pEngineUtil->timestamp;
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesVFManagementSetVFTelemetryModeExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesVFManagementSetVFTelemetrySamplingIntervalExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesDeviceEnumEnabledVFExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesVFManagementGetVFCapabilitiesExp(";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesVFManagementGetVFMemoryUtilizationExp2(";
    ss << "hVFhandle: " << hVFhandle;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pMemUtil == nullptr) {
        ss << ", pMemUtil: nullptr";
    } else {
        ss << ", pMemUtil {";
        ss << "stype: " << pMemUtil->stype;
        ss << ", pNext: " << pMemUtil->pNext;
        ss << ", vfMemLocation: " << pMemUtil->vfMemLocation;
        ss << ", vfMemUtilized: " << pMemUtil->vfMemUtilized;
        ss << "}";
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
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zesVFManagementGetVFEngineUtilizationExp2(";
    ss << "hVFhandle: " << hVFhandle;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pEngineUtil == nullptr) {
        ss << ", pEngineUtil: nullptr";
    } else {
        ss << ", pEngineUtil {";
        ss << "stype: " << pEngineUtil->stype;
        ss << ", pNext: " << pEngineUtil->pNext;
        ss << ", vfEngineType: " << pEngineUtil->vfEngineType;
        ss << ", activeCounterValue: " << pEngineUtil->activeCounterValue;
        ss << ", samplingCounterValue: " << pEngineUtil->samplingCounterValue;
        ss << "}";
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
