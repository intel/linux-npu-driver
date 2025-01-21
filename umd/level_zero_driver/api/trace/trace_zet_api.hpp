/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Generated header based on zet_api.h@version v1.11-r1.11.4

#pragma once

#include "trace_ze_api.hpp"

#include <iostream>
#include <level_zero/zet_api.h>
#include <sstream>

inline std::string _trace_zetModuleGetDebugInfo(zet_module_handle_t hModule,
                                                zet_module_debug_info_format_t format,
                                                size_t *pSize,
                                                uint8_t *pDebugInfo) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetModuleGetDebugInfo(";
    ss << "hModule: " << hModule;
    ss << ", format: " << format;
    if (pSize == nullptr) {
        ss << ", pSize: nullptr";
    } else {
        ss << ", pSize: " << *pSize;
    }
    if (pDebugInfo == nullptr) {
        ss << ", pDebugInfo: nullptr";
    } else {
        ss << ", pDebugInfo: " << *pDebugInfo;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetModuleGetDebugInfo(zet_module_handle_t hModule,
                                        zet_module_debug_info_format_t format,
                                        size_t *pSize,
                                        uint8_t *pDebugInfo) {
    TRACE_EVENT_BEGIN("API", "zetModuleGetDebugInfo");
    if (IS_API_TRACE())
        std::cerr << _trace_zetModuleGetDebugInfo(hModule, format, pSize, pDebugInfo) + "..\n";
}
inline void trace_zetModuleGetDebugInfo(ze_result_t ret,
                                        zet_module_handle_t hModule,
                                        zet_module_debug_info_format_t format,
                                        size_t *pSize,
                                        uint8_t *pDebugInfo) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetModuleGetDebugInfo(hModule, format, pSize, pDebugInfo) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetDeviceGetDebugProperties(zet_device_handle_t hDevice,
                                   zet_device_debug_properties_t *pDebugProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDeviceGetDebugProperties(";
    ss << "hDevice: " << hDevice;
    if (pDebugProperties == nullptr) {
        ss << ", pDebugProperties: nullptr";
    } else {
        ss << ", pDebugProperties {";
        ss << "stype: " << pDebugProperties->stype;
        ss << ", pNext: " << pDebugProperties->pNext;
        ss << ", flags: " << pDebugProperties->flags;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetDeviceGetDebugProperties(zet_device_handle_t hDevice,
                                              zet_device_debug_properties_t *pDebugProperties) {
    TRACE_EVENT_BEGIN("API", "zetDeviceGetDebugProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDeviceGetDebugProperties(hDevice, pDebugProperties) + "..\n";
}
inline void trace_zetDeviceGetDebugProperties(ze_result_t ret,
                                              zet_device_handle_t hDevice,
                                              zet_device_debug_properties_t *pDebugProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDeviceGetDebugProperties(hDevice, pDebugProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetDebugAttach(zet_device_handle_t hDevice,
                                         const zet_debug_config_t *config,
                                         zet_debug_session_handle_t *phDebug) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDebugAttach(";
    ss << "hDevice: " << hDevice;
    if (config == nullptr) {
        ss << ", config: nullptr";
    } else {
        ss << ", config {";
        ss << "pid: " << config->pid;
        ss << "}";
    }
    if (phDebug == nullptr) {
        ss << ", phDebug: nullptr";
    } else {
        ss << ", phDebug: " << *phDebug;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetDebugAttach(zet_device_handle_t hDevice,
                                 const zet_debug_config_t *config,
                                 zet_debug_session_handle_t *phDebug) {
    TRACE_EVENT_BEGIN("API", "zetDebugAttach");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugAttach(hDevice, config, phDebug) + "..\n";
}
inline void trace_zetDebugAttach(ze_result_t ret,
                                 zet_device_handle_t hDevice,
                                 const zet_debug_config_t *config,
                                 zet_debug_session_handle_t *phDebug) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugAttach(hDevice, config, phDebug) + trace_ze_result_t(ret);
}

inline std::string _trace_zetDebugDetach(zet_debug_session_handle_t hDebug) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDebugDetach(";
    ss << "hDebug: " << hDebug;
    ss << ")";
    return ss.str();
}
inline void trace_zetDebugDetach(zet_debug_session_handle_t hDebug) {
    TRACE_EVENT_BEGIN("API", "zetDebugDetach");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugDetach(hDebug) + "..\n";
}
inline void trace_zetDebugDetach(ze_result_t ret, zet_debug_session_handle_t hDebug) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugDetach(hDebug) + trace_ze_result_t(ret);
}

inline std::string _trace_zetDebugReadEvent(zet_debug_session_handle_t hDebug,
                                            uint64_t timeout,
                                            zet_debug_event_t *event) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDebugReadEvent(";
    ss << "hDebug: " << hDebug;
    ss << ", timeout: " << timeout;
    if (event == nullptr) {
        ss << ", event: nullptr";
    } else {
        ss << ", event {";
        ss << "type: " << event->type;
        ss << ", flags: " << event->flags;
        ss << ", info {";
        ss << ", detached {";
        ss << "reason: " << event->info.detached.reason;
        ss << "}";
        ss << ", module {";
        ss << "format: " << event->info.module.format;
        ss << ", moduleBegin: " << event->info.module.moduleBegin;
        ss << ", moduleEnd: " << event->info.module.moduleEnd;
        ss << ", load: " << event->info.module.load;
        ss << "}";
        ss << ", thread {";
        ss << ", thread {";
        ss << "slice: " << event->info.thread.thread.slice;
        ss << ", subslice: " << event->info.thread.thread.subslice;
        ss << ", eu: " << event->info.thread.thread.eu;
        ss << ", thread: " << event->info.thread.thread.thread;
        ss << "}";
        ss << "}";
        ss << ", page_fault {";
        ss << "address: " << event->info.page_fault.address;
        ss << ", mask: " << event->info.page_fault.mask;
        ss << ", reason: " << event->info.page_fault.reason;
        ss << "}";
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetDebugReadEvent(zet_debug_session_handle_t hDebug,
                                    uint64_t timeout,
                                    zet_debug_event_t *event) {
    TRACE_EVENT_BEGIN("API", "zetDebugReadEvent");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugReadEvent(hDebug, timeout, event) + "..\n";
}
inline void trace_zetDebugReadEvent(ze_result_t ret,
                                    zet_debug_session_handle_t hDebug,
                                    uint64_t timeout,
                                    zet_debug_event_t *event) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugReadEvent(hDebug, timeout, event) + trace_ze_result_t(ret);
}

inline std::string _trace_zetDebugAcknowledgeEvent(zet_debug_session_handle_t hDebug,
                                                   const zet_debug_event_t *event) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDebugAcknowledgeEvent(";
    ss << "hDebug: " << hDebug;
    if (event == nullptr) {
        ss << ", event: nullptr";
    } else {
        ss << ", event {";
        ss << "type: " << event->type;
        ss << ", flags: " << event->flags;
        ss << ", info {";
        ss << ", detached {";
        ss << "reason: " << event->info.detached.reason;
        ss << "}";
        ss << ", module {";
        ss << "format: " << event->info.module.format;
        ss << ", moduleBegin: " << event->info.module.moduleBegin;
        ss << ", moduleEnd: " << event->info.module.moduleEnd;
        ss << ", load: " << event->info.module.load;
        ss << "}";
        ss << ", thread {";
        ss << ", thread {";
        ss << "slice: " << event->info.thread.thread.slice;
        ss << ", subslice: " << event->info.thread.thread.subslice;
        ss << ", eu: " << event->info.thread.thread.eu;
        ss << ", thread: " << event->info.thread.thread.thread;
        ss << "}";
        ss << "}";
        ss << ", page_fault {";
        ss << "address: " << event->info.page_fault.address;
        ss << ", mask: " << event->info.page_fault.mask;
        ss << ", reason: " << event->info.page_fault.reason;
        ss << "}";
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetDebugAcknowledgeEvent(zet_debug_session_handle_t hDebug,
                                           const zet_debug_event_t *event) {
    TRACE_EVENT_BEGIN("API", "zetDebugAcknowledgeEvent");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugAcknowledgeEvent(hDebug, event) + "..\n";
}
inline void trace_zetDebugAcknowledgeEvent(ze_result_t ret,
                                           zet_debug_session_handle_t hDebug,
                                           const zet_debug_event_t *event) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugAcknowledgeEvent(hDebug, event) + trace_ze_result_t(ret);
}

inline std::string _trace_zetDebugInterrupt(zet_debug_session_handle_t hDebug,
                                            ze_device_thread_t thread) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDebugInterrupt(";
    ss << "hDebug: " << hDebug;
    ss << ", thread {";
    ss << "slice: " << thread.slice;
    ss << ", subslice: " << thread.subslice;
    ss << ", eu: " << thread.eu;
    ss << ", thread: " << thread.thread;
    ss << "}";
    ss << ")";
    return ss.str();
}
inline void trace_zetDebugInterrupt(zet_debug_session_handle_t hDebug, ze_device_thread_t thread) {
    TRACE_EVENT_BEGIN("API", "zetDebugInterrupt");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugInterrupt(hDebug, thread) + "..\n";
}
inline void trace_zetDebugInterrupt(ze_result_t ret,
                                    zet_debug_session_handle_t hDebug,
                                    ze_device_thread_t thread) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugInterrupt(hDebug, thread) + trace_ze_result_t(ret);
}

inline std::string _trace_zetDebugResume(zet_debug_session_handle_t hDebug,
                                         ze_device_thread_t thread) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDebugResume(";
    ss << "hDebug: " << hDebug;
    ss << ", thread {";
    ss << "slice: " << thread.slice;
    ss << ", subslice: " << thread.subslice;
    ss << ", eu: " << thread.eu;
    ss << ", thread: " << thread.thread;
    ss << "}";
    ss << ")";
    return ss.str();
}
inline void trace_zetDebugResume(zet_debug_session_handle_t hDebug, ze_device_thread_t thread) {
    TRACE_EVENT_BEGIN("API", "zetDebugResume");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugResume(hDebug, thread) + "..\n";
}
inline void trace_zetDebugResume(ze_result_t ret,
                                 zet_debug_session_handle_t hDebug,
                                 ze_device_thread_t thread) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugResume(hDebug, thread) + trace_ze_result_t(ret);
}

inline std::string _trace_zetDebugReadMemory(zet_debug_session_handle_t hDebug,
                                             ze_device_thread_t thread,
                                             const zet_debug_memory_space_desc_t *desc,
                                             size_t size,
                                             void *buffer) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDebugReadMemory(";
    ss << "hDebug: " << hDebug;
    ss << ", thread {";
    ss << "slice: " << thread.slice;
    ss << ", subslice: " << thread.subslice;
    ss << ", eu: " << thread.eu;
    ss << ", thread: " << thread.thread;
    ss << "}";
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", type: " << desc->type;
        ss << ", address: " << desc->address;
        ss << "}";
    }
    ss << ", size: " << size;
    ss << ", buffer: " << buffer;
    ss << ")";
    return ss.str();
}
inline void trace_zetDebugReadMemory(zet_debug_session_handle_t hDebug,
                                     ze_device_thread_t thread,
                                     const zet_debug_memory_space_desc_t *desc,
                                     size_t size,
                                     void *buffer) {
    TRACE_EVENT_BEGIN("API", "zetDebugReadMemory");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugReadMemory(hDebug, thread, desc, size, buffer) + "..\n";
}
inline void trace_zetDebugReadMemory(ze_result_t ret,
                                     zet_debug_session_handle_t hDebug,
                                     ze_device_thread_t thread,
                                     const zet_debug_memory_space_desc_t *desc,
                                     size_t size,
                                     void *buffer) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugReadMemory(hDebug, thread, desc, size, buffer) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetDebugWriteMemory(zet_debug_session_handle_t hDebug,
                                              ze_device_thread_t thread,
                                              const zet_debug_memory_space_desc_t *desc,
                                              size_t size,
                                              const void *buffer) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDebugWriteMemory(";
    ss << "hDebug: " << hDebug;
    ss << ", thread {";
    ss << "slice: " << thread.slice;
    ss << ", subslice: " << thread.subslice;
    ss << ", eu: " << thread.eu;
    ss << ", thread: " << thread.thread;
    ss << "}";
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", type: " << desc->type;
        ss << ", address: " << desc->address;
        ss << "}";
    }
    ss << ", size: " << size;
    ss << ", buffer: " << buffer;
    ss << ")";
    return ss.str();
}
inline void trace_zetDebugWriteMemory(zet_debug_session_handle_t hDebug,
                                      ze_device_thread_t thread,
                                      const zet_debug_memory_space_desc_t *desc,
                                      size_t size,
                                      const void *buffer) {
    TRACE_EVENT_BEGIN("API", "zetDebugWriteMemory");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugWriteMemory(hDebug, thread, desc, size, buffer) + "..\n";
}
inline void trace_zetDebugWriteMemory(ze_result_t ret,
                                      zet_debug_session_handle_t hDebug,
                                      ze_device_thread_t thread,
                                      const zet_debug_memory_space_desc_t *desc,
                                      size_t size,
                                      const void *buffer) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugWriteMemory(hDebug, thread, desc, size, buffer) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetDebugGetRegisterSetProperties(zet_device_handle_t hDevice,
                                        uint32_t *pCount,
                                        zet_debug_regset_properties_t *pRegisterSetProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDebugGetRegisterSetProperties(";
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pRegisterSetProperties == nullptr) {
        ss << ", pRegisterSetProperties: nullptr";
    } else {
        ss << ", pRegisterSetProperties {";
        ss << "stype: " << pRegisterSetProperties->stype;
        ss << ", pNext: " << pRegisterSetProperties->pNext;
        ss << ", type: " << pRegisterSetProperties->type;
        ss << ", version: " << pRegisterSetProperties->version;
        ss << ", generalFlags: " << pRegisterSetProperties->generalFlags;
        ss << ", deviceFlags: " << pRegisterSetProperties->deviceFlags;
        ss << ", count: " << pRegisterSetProperties->count;
        ss << ", bitSize: " << pRegisterSetProperties->bitSize;
        ss << ", byteSize: " << pRegisterSetProperties->byteSize;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zetDebugGetRegisterSetProperties(zet_device_handle_t hDevice,
                                       uint32_t *pCount,
                                       zet_debug_regset_properties_t *pRegisterSetProperties) {
    TRACE_EVENT_BEGIN("API", "zetDebugGetRegisterSetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugGetRegisterSetProperties(hDevice,
                                                             pCount,
                                                             pRegisterSetProperties) +
                         "..\n";
}
inline void
trace_zetDebugGetRegisterSetProperties(ze_result_t ret,
                                       zet_device_handle_t hDevice,
                                       uint32_t *pCount,
                                       zet_debug_regset_properties_t *pRegisterSetProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugGetRegisterSetProperties(hDevice,
                                                             pCount,
                                                             pRegisterSetProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetDebugGetThreadRegisterSetProperties(
    zet_debug_session_handle_t hDebug,
    ze_device_thread_t thread,
    uint32_t *pCount,
    zet_debug_regset_properties_t *pRegisterSetProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDebugGetThreadRegisterSetProperties(";
    ss << "hDebug: " << hDebug;
    ss << ", thread {";
    ss << "slice: " << thread.slice;
    ss << ", subslice: " << thread.subslice;
    ss << ", eu: " << thread.eu;
    ss << ", thread: " << thread.thread;
    ss << "}";
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pRegisterSetProperties == nullptr) {
        ss << ", pRegisterSetProperties: nullptr";
    } else {
        ss << ", pRegisterSetProperties {";
        ss << "stype: " << pRegisterSetProperties->stype;
        ss << ", pNext: " << pRegisterSetProperties->pNext;
        ss << ", type: " << pRegisterSetProperties->type;
        ss << ", version: " << pRegisterSetProperties->version;
        ss << ", generalFlags: " << pRegisterSetProperties->generalFlags;
        ss << ", deviceFlags: " << pRegisterSetProperties->deviceFlags;
        ss << ", count: " << pRegisterSetProperties->count;
        ss << ", bitSize: " << pRegisterSetProperties->bitSize;
        ss << ", byteSize: " << pRegisterSetProperties->byteSize;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetDebugGetThreadRegisterSetProperties(
    zet_debug_session_handle_t hDebug,
    ze_device_thread_t thread,
    uint32_t *pCount,
    zet_debug_regset_properties_t *pRegisterSetProperties) {
    TRACE_EVENT_BEGIN("API", "zetDebugGetThreadRegisterSetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugGetThreadRegisterSetProperties(hDebug,
                                                                   thread,
                                                                   pCount,
                                                                   pRegisterSetProperties) +
                         "..\n";
}
inline void trace_zetDebugGetThreadRegisterSetProperties(
    ze_result_t ret,
    zet_debug_session_handle_t hDebug,
    ze_device_thread_t thread,
    uint32_t *pCount,
    zet_debug_regset_properties_t *pRegisterSetProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDebugGetThreadRegisterSetProperties(hDebug,
                                                                   thread,
                                                                   pCount,
                                                                   pRegisterSetProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetDebugReadRegisters(zet_debug_session_handle_t hDebug,
                                                ze_device_thread_t thread,
                                                uint32_t type,
                                                uint32_t start,
                                                uint32_t count,
                                                void *pRegisterValues) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDebugReadRegisters(";
    ss << "hDebug: " << hDebug;
    ss << ", thread {";
    ss << "slice: " << thread.slice;
    ss << ", subslice: " << thread.subslice;
    ss << ", eu: " << thread.eu;
    ss << ", thread: " << thread.thread;
    ss << "}";
    ss << ", type: " << type;
    ss << ", start: " << start;
    ss << ", count: " << count;
    ss << ", pRegisterValues: " << pRegisterValues;
    ss << ")";
    return ss.str();
}
inline void trace_zetDebugReadRegisters(zet_debug_session_handle_t hDebug,
                                        ze_device_thread_t thread,
                                        uint32_t type,
                                        uint32_t start,
                                        uint32_t count,
                                        void *pRegisterValues) {
    TRACE_EVENT_BEGIN("API", "zetDebugReadRegisters");
    if (IS_API_TRACE())
        std::cerr
            << _trace_zetDebugReadRegisters(hDebug, thread, type, start, count, pRegisterValues) +
                   "..\n";
}
inline void trace_zetDebugReadRegisters(ze_result_t ret,
                                        zet_debug_session_handle_t hDebug,
                                        ze_device_thread_t thread,
                                        uint32_t type,
                                        uint32_t start,
                                        uint32_t count,
                                        void *pRegisterValues) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr
            << _trace_zetDebugReadRegisters(hDebug, thread, type, start, count, pRegisterValues) +
                   trace_ze_result_t(ret);
}

inline std::string _trace_zetDebugWriteRegisters(zet_debug_session_handle_t hDebug,
                                                 ze_device_thread_t thread,
                                                 uint32_t type,
                                                 uint32_t start,
                                                 uint32_t count,
                                                 void *pRegisterValues) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDebugWriteRegisters(";
    ss << "hDebug: " << hDebug;
    ss << ", thread {";
    ss << "slice: " << thread.slice;
    ss << ", subslice: " << thread.subslice;
    ss << ", eu: " << thread.eu;
    ss << ", thread: " << thread.thread;
    ss << "}";
    ss << ", type: " << type;
    ss << ", start: " << start;
    ss << ", count: " << count;
    ss << ", pRegisterValues: " << pRegisterValues;
    ss << ")";
    return ss.str();
}
inline void trace_zetDebugWriteRegisters(zet_debug_session_handle_t hDebug,
                                         ze_device_thread_t thread,
                                         uint32_t type,
                                         uint32_t start,
                                         uint32_t count,
                                         void *pRegisterValues) {
    TRACE_EVENT_BEGIN("API", "zetDebugWriteRegisters");
    if (IS_API_TRACE())
        std::cerr
            << _trace_zetDebugWriteRegisters(hDebug, thread, type, start, count, pRegisterValues) +
                   "..\n";
}
inline void trace_zetDebugWriteRegisters(ze_result_t ret,
                                         zet_debug_session_handle_t hDebug,
                                         ze_device_thread_t thread,
                                         uint32_t type,
                                         uint32_t start,
                                         uint32_t count,
                                         void *pRegisterValues) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr
            << _trace_zetDebugWriteRegisters(hDebug, thread, type, start, count, pRegisterValues) +
                   trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricGroupGet(zet_device_handle_t hDevice,
                                            uint32_t *pCount,
                                            zet_metric_group_handle_t *phMetricGroups) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGroupGet(";
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phMetricGroups == nullptr) {
        ss << ", phMetricGroups: nullptr";
    } else {
        ss << ", phMetricGroups: " << *phMetricGroups;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGroupGet(zet_device_handle_t hDevice,
                                    uint32_t *pCount,
                                    zet_metric_group_handle_t *phMetricGroups) {
    TRACE_EVENT_BEGIN("API", "zetMetricGroupGet");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupGet(hDevice, pCount, phMetricGroups) + "..\n";
}
inline void trace_zetMetricGroupGet(ze_result_t ret,
                                    zet_device_handle_t hDevice,
                                    uint32_t *pCount,
                                    zet_metric_group_handle_t *phMetricGroups) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupGet(hDevice, pCount, phMetricGroups) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricGroupGetProperties(zet_metric_group_handle_t hMetricGroup,
                                                      zet_metric_group_properties_t *pProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGroupGetProperties(";
    ss << "hMetricGroup: " << hMetricGroup;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
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
           << pProperties->name[63] << pProperties->name[64] << pProperties->name[65]
           << pProperties->name[66] << pProperties->name[67] << pProperties->name[68]
           << pProperties->name[69] << pProperties->name[70] << pProperties->name[71]
           << pProperties->name[72] << pProperties->name[73] << pProperties->name[74]
           << pProperties->name[75] << pProperties->name[76] << pProperties->name[77]
           << pProperties->name[78] << pProperties->name[79] << pProperties->name[80]
           << pProperties->name[81] << pProperties->name[82] << pProperties->name[83]
           << pProperties->name[84] << pProperties->name[85] << pProperties->name[86]
           << pProperties->name[87] << pProperties->name[88] << pProperties->name[89]
           << pProperties->name[90] << pProperties->name[91] << pProperties->name[92]
           << pProperties->name[93] << pProperties->name[94] << pProperties->name[95]
           << pProperties->name[96] << pProperties->name[97] << pProperties->name[98]
           << pProperties->name[99] << pProperties->name[100] << pProperties->name[101]
           << pProperties->name[102] << pProperties->name[103] << pProperties->name[104]
           << pProperties->name[105] << pProperties->name[106] << pProperties->name[107]
           << pProperties->name[108] << pProperties->name[109] << pProperties->name[110]
           << pProperties->name[111] << pProperties->name[112] << pProperties->name[113]
           << pProperties->name[114] << pProperties->name[115] << pProperties->name[116]
           << pProperties->name[117] << pProperties->name[118] << pProperties->name[119]
           << pProperties->name[120] << pProperties->name[121] << pProperties->name[122]
           << pProperties->name[123] << pProperties->name[124] << pProperties->name[125]
           << pProperties->name[126] << pProperties->name[127] << pProperties->name[128]
           << pProperties->name[129] << pProperties->name[130] << pProperties->name[131]
           << pProperties->name[132] << pProperties->name[133] << pProperties->name[134]
           << pProperties->name[135] << pProperties->name[136] << pProperties->name[137]
           << pProperties->name[138] << pProperties->name[139] << pProperties->name[140]
           << pProperties->name[141] << pProperties->name[142] << pProperties->name[143]
           << pProperties->name[144] << pProperties->name[145] << pProperties->name[146]
           << pProperties->name[147] << pProperties->name[148] << pProperties->name[149]
           << pProperties->name[150] << pProperties->name[151] << pProperties->name[152]
           << pProperties->name[153] << pProperties->name[154] << pProperties->name[155]
           << pProperties->name[156] << pProperties->name[157] << pProperties->name[158]
           << pProperties->name[159] << pProperties->name[160] << pProperties->name[161]
           << pProperties->name[162] << pProperties->name[163] << pProperties->name[164]
           << pProperties->name[165] << pProperties->name[166] << pProperties->name[167]
           << pProperties->name[168] << pProperties->name[169] << pProperties->name[170]
           << pProperties->name[171] << pProperties->name[172] << pProperties->name[173]
           << pProperties->name[174] << pProperties->name[175] << pProperties->name[176]
           << pProperties->name[177] << pProperties->name[178] << pProperties->name[179]
           << pProperties->name[180] << pProperties->name[181] << pProperties->name[182]
           << pProperties->name[183] << pProperties->name[184] << pProperties->name[185]
           << pProperties->name[186] << pProperties->name[187] << pProperties->name[188]
           << pProperties->name[189] << pProperties->name[190] << pProperties->name[191]
           << pProperties->name[192] << pProperties->name[193] << pProperties->name[194]
           << pProperties->name[195] << pProperties->name[196] << pProperties->name[197]
           << pProperties->name[198] << pProperties->name[199] << pProperties->name[200]
           << pProperties->name[201] << pProperties->name[202] << pProperties->name[203]
           << pProperties->name[204] << pProperties->name[205] << pProperties->name[206]
           << pProperties->name[207] << pProperties->name[208] << pProperties->name[209]
           << pProperties->name[210] << pProperties->name[211] << pProperties->name[212]
           << pProperties->name[213] << pProperties->name[214] << pProperties->name[215]
           << pProperties->name[216] << pProperties->name[217] << pProperties->name[218]
           << pProperties->name[219] << pProperties->name[220] << pProperties->name[221]
           << pProperties->name[222] << pProperties->name[223] << pProperties->name[224]
           << pProperties->name[225] << pProperties->name[226] << pProperties->name[227]
           << pProperties->name[228] << pProperties->name[229] << pProperties->name[230]
           << pProperties->name[231] << pProperties->name[232] << pProperties->name[233]
           << pProperties->name[234] << pProperties->name[235] << pProperties->name[236]
           << pProperties->name[237] << pProperties->name[238] << pProperties->name[239]
           << pProperties->name[240] << pProperties->name[241] << pProperties->name[242]
           << pProperties->name[243] << pProperties->name[244] << pProperties->name[245]
           << pProperties->name[246] << pProperties->name[247] << pProperties->name[248]
           << pProperties->name[249] << pProperties->name[250] << pProperties->name[251]
           << pProperties->name[252] << pProperties->name[253] << pProperties->name[254]
           << pProperties->name[255];
        ss << ", description: " << pProperties->description[0] << pProperties->description[1]
           << pProperties->description[2] << pProperties->description[3]
           << pProperties->description[4] << pProperties->description[5]
           << pProperties->description[6] << pProperties->description[7]
           << pProperties->description[8] << pProperties->description[9]
           << pProperties->description[10] << pProperties->description[11]
           << pProperties->description[12] << pProperties->description[13]
           << pProperties->description[14] << pProperties->description[15]
           << pProperties->description[16] << pProperties->description[17]
           << pProperties->description[18] << pProperties->description[19]
           << pProperties->description[20] << pProperties->description[21]
           << pProperties->description[22] << pProperties->description[23]
           << pProperties->description[24] << pProperties->description[25]
           << pProperties->description[26] << pProperties->description[27]
           << pProperties->description[28] << pProperties->description[29]
           << pProperties->description[30] << pProperties->description[31]
           << pProperties->description[32] << pProperties->description[33]
           << pProperties->description[34] << pProperties->description[35]
           << pProperties->description[36] << pProperties->description[37]
           << pProperties->description[38] << pProperties->description[39]
           << pProperties->description[40] << pProperties->description[41]
           << pProperties->description[42] << pProperties->description[43]
           << pProperties->description[44] << pProperties->description[45]
           << pProperties->description[46] << pProperties->description[47]
           << pProperties->description[48] << pProperties->description[49]
           << pProperties->description[50] << pProperties->description[51]
           << pProperties->description[52] << pProperties->description[53]
           << pProperties->description[54] << pProperties->description[55]
           << pProperties->description[56] << pProperties->description[57]
           << pProperties->description[58] << pProperties->description[59]
           << pProperties->description[60] << pProperties->description[61]
           << pProperties->description[62] << pProperties->description[63]
           << pProperties->description[64] << pProperties->description[65]
           << pProperties->description[66] << pProperties->description[67]
           << pProperties->description[68] << pProperties->description[69]
           << pProperties->description[70] << pProperties->description[71]
           << pProperties->description[72] << pProperties->description[73]
           << pProperties->description[74] << pProperties->description[75]
           << pProperties->description[76] << pProperties->description[77]
           << pProperties->description[78] << pProperties->description[79]
           << pProperties->description[80] << pProperties->description[81]
           << pProperties->description[82] << pProperties->description[83]
           << pProperties->description[84] << pProperties->description[85]
           << pProperties->description[86] << pProperties->description[87]
           << pProperties->description[88] << pProperties->description[89]
           << pProperties->description[90] << pProperties->description[91]
           << pProperties->description[92] << pProperties->description[93]
           << pProperties->description[94] << pProperties->description[95]
           << pProperties->description[96] << pProperties->description[97]
           << pProperties->description[98] << pProperties->description[99]
           << pProperties->description[100] << pProperties->description[101]
           << pProperties->description[102] << pProperties->description[103]
           << pProperties->description[104] << pProperties->description[105]
           << pProperties->description[106] << pProperties->description[107]
           << pProperties->description[108] << pProperties->description[109]
           << pProperties->description[110] << pProperties->description[111]
           << pProperties->description[112] << pProperties->description[113]
           << pProperties->description[114] << pProperties->description[115]
           << pProperties->description[116] << pProperties->description[117]
           << pProperties->description[118] << pProperties->description[119]
           << pProperties->description[120] << pProperties->description[121]
           << pProperties->description[122] << pProperties->description[123]
           << pProperties->description[124] << pProperties->description[125]
           << pProperties->description[126] << pProperties->description[127]
           << pProperties->description[128] << pProperties->description[129]
           << pProperties->description[130] << pProperties->description[131]
           << pProperties->description[132] << pProperties->description[133]
           << pProperties->description[134] << pProperties->description[135]
           << pProperties->description[136] << pProperties->description[137]
           << pProperties->description[138] << pProperties->description[139]
           << pProperties->description[140] << pProperties->description[141]
           << pProperties->description[142] << pProperties->description[143]
           << pProperties->description[144] << pProperties->description[145]
           << pProperties->description[146] << pProperties->description[147]
           << pProperties->description[148] << pProperties->description[149]
           << pProperties->description[150] << pProperties->description[151]
           << pProperties->description[152] << pProperties->description[153]
           << pProperties->description[154] << pProperties->description[155]
           << pProperties->description[156] << pProperties->description[157]
           << pProperties->description[158] << pProperties->description[159]
           << pProperties->description[160] << pProperties->description[161]
           << pProperties->description[162] << pProperties->description[163]
           << pProperties->description[164] << pProperties->description[165]
           << pProperties->description[166] << pProperties->description[167]
           << pProperties->description[168] << pProperties->description[169]
           << pProperties->description[170] << pProperties->description[171]
           << pProperties->description[172] << pProperties->description[173]
           << pProperties->description[174] << pProperties->description[175]
           << pProperties->description[176] << pProperties->description[177]
           << pProperties->description[178] << pProperties->description[179]
           << pProperties->description[180] << pProperties->description[181]
           << pProperties->description[182] << pProperties->description[183]
           << pProperties->description[184] << pProperties->description[185]
           << pProperties->description[186] << pProperties->description[187]
           << pProperties->description[188] << pProperties->description[189]
           << pProperties->description[190] << pProperties->description[191]
           << pProperties->description[192] << pProperties->description[193]
           << pProperties->description[194] << pProperties->description[195]
           << pProperties->description[196] << pProperties->description[197]
           << pProperties->description[198] << pProperties->description[199]
           << pProperties->description[200] << pProperties->description[201]
           << pProperties->description[202] << pProperties->description[203]
           << pProperties->description[204] << pProperties->description[205]
           << pProperties->description[206] << pProperties->description[207]
           << pProperties->description[208] << pProperties->description[209]
           << pProperties->description[210] << pProperties->description[211]
           << pProperties->description[212] << pProperties->description[213]
           << pProperties->description[214] << pProperties->description[215]
           << pProperties->description[216] << pProperties->description[217]
           << pProperties->description[218] << pProperties->description[219]
           << pProperties->description[220] << pProperties->description[221]
           << pProperties->description[222] << pProperties->description[223]
           << pProperties->description[224] << pProperties->description[225]
           << pProperties->description[226] << pProperties->description[227]
           << pProperties->description[228] << pProperties->description[229]
           << pProperties->description[230] << pProperties->description[231]
           << pProperties->description[232] << pProperties->description[233]
           << pProperties->description[234] << pProperties->description[235]
           << pProperties->description[236] << pProperties->description[237]
           << pProperties->description[238] << pProperties->description[239]
           << pProperties->description[240] << pProperties->description[241]
           << pProperties->description[242] << pProperties->description[243]
           << pProperties->description[244] << pProperties->description[245]
           << pProperties->description[246] << pProperties->description[247]
           << pProperties->description[248] << pProperties->description[249]
           << pProperties->description[250] << pProperties->description[251]
           << pProperties->description[252] << pProperties->description[253]
           << pProperties->description[254] << pProperties->description[255];
        ss << ", samplingType: " << pProperties->samplingType;
        ss << ", domain: " << pProperties->domain;
        ss << ", metricCount: " << pProperties->metricCount;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGroupGetProperties(zet_metric_group_handle_t hMetricGroup,
                                              zet_metric_group_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zetMetricGroupGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupGetProperties(hMetricGroup, pProperties) + "..\n";
}
inline void trace_zetMetricGroupGetProperties(ze_result_t ret,
                                              zet_metric_group_handle_t hMetricGroup,
                                              zet_metric_group_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupGetProperties(hMetricGroup, pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetMetricGroupCalculateMetricValues(zet_metric_group_handle_t hMetricGroup,
                                           zet_metric_group_calculation_type_t type,
                                           size_t rawDataSize,
                                           const uint8_t *pRawData,
                                           uint32_t *pMetricValueCount,
                                           zet_typed_value_t *pMetricValues) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGroupCalculateMetricValues(";
    ss << "hMetricGroup: " << hMetricGroup;
    ss << ", type: " << type;
    ss << ", rawDataSize: " << rawDataSize;
    if (pRawData == nullptr) {
        ss << ", pRawData: nullptr";
    } else {
        ss << ", pRawData: " << *pRawData;
    }
    if (pMetricValueCount == nullptr) {
        ss << ", pMetricValueCount: nullptr";
    } else {
        ss << ", pMetricValueCount: " << *pMetricValueCount;
    }
    if (pMetricValues == nullptr) {
        ss << ", pMetricValues: nullptr";
    } else {
        ss << ", pMetricValues {";
        ss << "type: " << pMetricValues->type;
        ss << ", value {";
        ss << "ui32: " << pMetricValues->value.ui32;
        ss << ", ui64: " << pMetricValues->value.ui64;
        ss << ", fp32: " << pMetricValues->value.fp32;
        ss << ", fp64: " << pMetricValues->value.fp64;
        ss << ", b8: " << pMetricValues->value.b8;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGroupCalculateMetricValues(zet_metric_group_handle_t hMetricGroup,
                                                      zet_metric_group_calculation_type_t type,
                                                      size_t rawDataSize,
                                                      const uint8_t *pRawData,
                                                      uint32_t *pMetricValueCount,
                                                      zet_typed_value_t *pMetricValues) {
    TRACE_EVENT_BEGIN("API", "zetMetricGroupCalculateMetricValues");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupCalculateMetricValues(hMetricGroup,
                                                                type,
                                                                rawDataSize,
                                                                pRawData,
                                                                pMetricValueCount,
                                                                pMetricValues) +
                         "..\n";
}
inline void trace_zetMetricGroupCalculateMetricValues(ze_result_t ret,
                                                      zet_metric_group_handle_t hMetricGroup,
                                                      zet_metric_group_calculation_type_t type,
                                                      size_t rawDataSize,
                                                      const uint8_t *pRawData,
                                                      uint32_t *pMetricValueCount,
                                                      zet_typed_value_t *pMetricValues) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupCalculateMetricValues(hMetricGroup,
                                                                type,
                                                                rawDataSize,
                                                                pRawData,
                                                                pMetricValueCount,
                                                                pMetricValues) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricGet(zet_metric_group_handle_t hMetricGroup,
                                       uint32_t *pCount,
                                       zet_metric_handle_t *phMetrics) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGet(";
    ss << "hMetricGroup: " << hMetricGroup;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phMetrics == nullptr) {
        ss << ", phMetrics: nullptr";
    } else {
        ss << ", phMetrics: " << *phMetrics;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGet(zet_metric_group_handle_t hMetricGroup,
                               uint32_t *pCount,
                               zet_metric_handle_t *phMetrics) {
    TRACE_EVENT_BEGIN("API", "zetMetricGet");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGet(hMetricGroup, pCount, phMetrics) + "..\n";
}
inline void trace_zetMetricGet(ze_result_t ret,
                               zet_metric_group_handle_t hMetricGroup,
                               uint32_t *pCount,
                               zet_metric_handle_t *phMetrics) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGet(hMetricGroup, pCount, phMetrics) + trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricGetProperties(zet_metric_handle_t hMetric,
                                                 zet_metric_properties_t *pProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGetProperties(";
    ss << "hMetric: " << hMetric;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
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
           << pProperties->name[63] << pProperties->name[64] << pProperties->name[65]
           << pProperties->name[66] << pProperties->name[67] << pProperties->name[68]
           << pProperties->name[69] << pProperties->name[70] << pProperties->name[71]
           << pProperties->name[72] << pProperties->name[73] << pProperties->name[74]
           << pProperties->name[75] << pProperties->name[76] << pProperties->name[77]
           << pProperties->name[78] << pProperties->name[79] << pProperties->name[80]
           << pProperties->name[81] << pProperties->name[82] << pProperties->name[83]
           << pProperties->name[84] << pProperties->name[85] << pProperties->name[86]
           << pProperties->name[87] << pProperties->name[88] << pProperties->name[89]
           << pProperties->name[90] << pProperties->name[91] << pProperties->name[92]
           << pProperties->name[93] << pProperties->name[94] << pProperties->name[95]
           << pProperties->name[96] << pProperties->name[97] << pProperties->name[98]
           << pProperties->name[99] << pProperties->name[100] << pProperties->name[101]
           << pProperties->name[102] << pProperties->name[103] << pProperties->name[104]
           << pProperties->name[105] << pProperties->name[106] << pProperties->name[107]
           << pProperties->name[108] << pProperties->name[109] << pProperties->name[110]
           << pProperties->name[111] << pProperties->name[112] << pProperties->name[113]
           << pProperties->name[114] << pProperties->name[115] << pProperties->name[116]
           << pProperties->name[117] << pProperties->name[118] << pProperties->name[119]
           << pProperties->name[120] << pProperties->name[121] << pProperties->name[122]
           << pProperties->name[123] << pProperties->name[124] << pProperties->name[125]
           << pProperties->name[126] << pProperties->name[127] << pProperties->name[128]
           << pProperties->name[129] << pProperties->name[130] << pProperties->name[131]
           << pProperties->name[132] << pProperties->name[133] << pProperties->name[134]
           << pProperties->name[135] << pProperties->name[136] << pProperties->name[137]
           << pProperties->name[138] << pProperties->name[139] << pProperties->name[140]
           << pProperties->name[141] << pProperties->name[142] << pProperties->name[143]
           << pProperties->name[144] << pProperties->name[145] << pProperties->name[146]
           << pProperties->name[147] << pProperties->name[148] << pProperties->name[149]
           << pProperties->name[150] << pProperties->name[151] << pProperties->name[152]
           << pProperties->name[153] << pProperties->name[154] << pProperties->name[155]
           << pProperties->name[156] << pProperties->name[157] << pProperties->name[158]
           << pProperties->name[159] << pProperties->name[160] << pProperties->name[161]
           << pProperties->name[162] << pProperties->name[163] << pProperties->name[164]
           << pProperties->name[165] << pProperties->name[166] << pProperties->name[167]
           << pProperties->name[168] << pProperties->name[169] << pProperties->name[170]
           << pProperties->name[171] << pProperties->name[172] << pProperties->name[173]
           << pProperties->name[174] << pProperties->name[175] << pProperties->name[176]
           << pProperties->name[177] << pProperties->name[178] << pProperties->name[179]
           << pProperties->name[180] << pProperties->name[181] << pProperties->name[182]
           << pProperties->name[183] << pProperties->name[184] << pProperties->name[185]
           << pProperties->name[186] << pProperties->name[187] << pProperties->name[188]
           << pProperties->name[189] << pProperties->name[190] << pProperties->name[191]
           << pProperties->name[192] << pProperties->name[193] << pProperties->name[194]
           << pProperties->name[195] << pProperties->name[196] << pProperties->name[197]
           << pProperties->name[198] << pProperties->name[199] << pProperties->name[200]
           << pProperties->name[201] << pProperties->name[202] << pProperties->name[203]
           << pProperties->name[204] << pProperties->name[205] << pProperties->name[206]
           << pProperties->name[207] << pProperties->name[208] << pProperties->name[209]
           << pProperties->name[210] << pProperties->name[211] << pProperties->name[212]
           << pProperties->name[213] << pProperties->name[214] << pProperties->name[215]
           << pProperties->name[216] << pProperties->name[217] << pProperties->name[218]
           << pProperties->name[219] << pProperties->name[220] << pProperties->name[221]
           << pProperties->name[222] << pProperties->name[223] << pProperties->name[224]
           << pProperties->name[225] << pProperties->name[226] << pProperties->name[227]
           << pProperties->name[228] << pProperties->name[229] << pProperties->name[230]
           << pProperties->name[231] << pProperties->name[232] << pProperties->name[233]
           << pProperties->name[234] << pProperties->name[235] << pProperties->name[236]
           << pProperties->name[237] << pProperties->name[238] << pProperties->name[239]
           << pProperties->name[240] << pProperties->name[241] << pProperties->name[242]
           << pProperties->name[243] << pProperties->name[244] << pProperties->name[245]
           << pProperties->name[246] << pProperties->name[247] << pProperties->name[248]
           << pProperties->name[249] << pProperties->name[250] << pProperties->name[251]
           << pProperties->name[252] << pProperties->name[253] << pProperties->name[254]
           << pProperties->name[255];
        ss << ", description: " << pProperties->description[0] << pProperties->description[1]
           << pProperties->description[2] << pProperties->description[3]
           << pProperties->description[4] << pProperties->description[5]
           << pProperties->description[6] << pProperties->description[7]
           << pProperties->description[8] << pProperties->description[9]
           << pProperties->description[10] << pProperties->description[11]
           << pProperties->description[12] << pProperties->description[13]
           << pProperties->description[14] << pProperties->description[15]
           << pProperties->description[16] << pProperties->description[17]
           << pProperties->description[18] << pProperties->description[19]
           << pProperties->description[20] << pProperties->description[21]
           << pProperties->description[22] << pProperties->description[23]
           << pProperties->description[24] << pProperties->description[25]
           << pProperties->description[26] << pProperties->description[27]
           << pProperties->description[28] << pProperties->description[29]
           << pProperties->description[30] << pProperties->description[31]
           << pProperties->description[32] << pProperties->description[33]
           << pProperties->description[34] << pProperties->description[35]
           << pProperties->description[36] << pProperties->description[37]
           << pProperties->description[38] << pProperties->description[39]
           << pProperties->description[40] << pProperties->description[41]
           << pProperties->description[42] << pProperties->description[43]
           << pProperties->description[44] << pProperties->description[45]
           << pProperties->description[46] << pProperties->description[47]
           << pProperties->description[48] << pProperties->description[49]
           << pProperties->description[50] << pProperties->description[51]
           << pProperties->description[52] << pProperties->description[53]
           << pProperties->description[54] << pProperties->description[55]
           << pProperties->description[56] << pProperties->description[57]
           << pProperties->description[58] << pProperties->description[59]
           << pProperties->description[60] << pProperties->description[61]
           << pProperties->description[62] << pProperties->description[63]
           << pProperties->description[64] << pProperties->description[65]
           << pProperties->description[66] << pProperties->description[67]
           << pProperties->description[68] << pProperties->description[69]
           << pProperties->description[70] << pProperties->description[71]
           << pProperties->description[72] << pProperties->description[73]
           << pProperties->description[74] << pProperties->description[75]
           << pProperties->description[76] << pProperties->description[77]
           << pProperties->description[78] << pProperties->description[79]
           << pProperties->description[80] << pProperties->description[81]
           << pProperties->description[82] << pProperties->description[83]
           << pProperties->description[84] << pProperties->description[85]
           << pProperties->description[86] << pProperties->description[87]
           << pProperties->description[88] << pProperties->description[89]
           << pProperties->description[90] << pProperties->description[91]
           << pProperties->description[92] << pProperties->description[93]
           << pProperties->description[94] << pProperties->description[95]
           << pProperties->description[96] << pProperties->description[97]
           << pProperties->description[98] << pProperties->description[99]
           << pProperties->description[100] << pProperties->description[101]
           << pProperties->description[102] << pProperties->description[103]
           << pProperties->description[104] << pProperties->description[105]
           << pProperties->description[106] << pProperties->description[107]
           << pProperties->description[108] << pProperties->description[109]
           << pProperties->description[110] << pProperties->description[111]
           << pProperties->description[112] << pProperties->description[113]
           << pProperties->description[114] << pProperties->description[115]
           << pProperties->description[116] << pProperties->description[117]
           << pProperties->description[118] << pProperties->description[119]
           << pProperties->description[120] << pProperties->description[121]
           << pProperties->description[122] << pProperties->description[123]
           << pProperties->description[124] << pProperties->description[125]
           << pProperties->description[126] << pProperties->description[127]
           << pProperties->description[128] << pProperties->description[129]
           << pProperties->description[130] << pProperties->description[131]
           << pProperties->description[132] << pProperties->description[133]
           << pProperties->description[134] << pProperties->description[135]
           << pProperties->description[136] << pProperties->description[137]
           << pProperties->description[138] << pProperties->description[139]
           << pProperties->description[140] << pProperties->description[141]
           << pProperties->description[142] << pProperties->description[143]
           << pProperties->description[144] << pProperties->description[145]
           << pProperties->description[146] << pProperties->description[147]
           << pProperties->description[148] << pProperties->description[149]
           << pProperties->description[150] << pProperties->description[151]
           << pProperties->description[152] << pProperties->description[153]
           << pProperties->description[154] << pProperties->description[155]
           << pProperties->description[156] << pProperties->description[157]
           << pProperties->description[158] << pProperties->description[159]
           << pProperties->description[160] << pProperties->description[161]
           << pProperties->description[162] << pProperties->description[163]
           << pProperties->description[164] << pProperties->description[165]
           << pProperties->description[166] << pProperties->description[167]
           << pProperties->description[168] << pProperties->description[169]
           << pProperties->description[170] << pProperties->description[171]
           << pProperties->description[172] << pProperties->description[173]
           << pProperties->description[174] << pProperties->description[175]
           << pProperties->description[176] << pProperties->description[177]
           << pProperties->description[178] << pProperties->description[179]
           << pProperties->description[180] << pProperties->description[181]
           << pProperties->description[182] << pProperties->description[183]
           << pProperties->description[184] << pProperties->description[185]
           << pProperties->description[186] << pProperties->description[187]
           << pProperties->description[188] << pProperties->description[189]
           << pProperties->description[190] << pProperties->description[191]
           << pProperties->description[192] << pProperties->description[193]
           << pProperties->description[194] << pProperties->description[195]
           << pProperties->description[196] << pProperties->description[197]
           << pProperties->description[198] << pProperties->description[199]
           << pProperties->description[200] << pProperties->description[201]
           << pProperties->description[202] << pProperties->description[203]
           << pProperties->description[204] << pProperties->description[205]
           << pProperties->description[206] << pProperties->description[207]
           << pProperties->description[208] << pProperties->description[209]
           << pProperties->description[210] << pProperties->description[211]
           << pProperties->description[212] << pProperties->description[213]
           << pProperties->description[214] << pProperties->description[215]
           << pProperties->description[216] << pProperties->description[217]
           << pProperties->description[218] << pProperties->description[219]
           << pProperties->description[220] << pProperties->description[221]
           << pProperties->description[222] << pProperties->description[223]
           << pProperties->description[224] << pProperties->description[225]
           << pProperties->description[226] << pProperties->description[227]
           << pProperties->description[228] << pProperties->description[229]
           << pProperties->description[230] << pProperties->description[231]
           << pProperties->description[232] << pProperties->description[233]
           << pProperties->description[234] << pProperties->description[235]
           << pProperties->description[236] << pProperties->description[237]
           << pProperties->description[238] << pProperties->description[239]
           << pProperties->description[240] << pProperties->description[241]
           << pProperties->description[242] << pProperties->description[243]
           << pProperties->description[244] << pProperties->description[245]
           << pProperties->description[246] << pProperties->description[247]
           << pProperties->description[248] << pProperties->description[249]
           << pProperties->description[250] << pProperties->description[251]
           << pProperties->description[252] << pProperties->description[253]
           << pProperties->description[254] << pProperties->description[255];
        ss << ", component: " << pProperties->component[0] << pProperties->component[1]
           << pProperties->component[2] << pProperties->component[3] << pProperties->component[4]
           << pProperties->component[5] << pProperties->component[6] << pProperties->component[7]
           << pProperties->component[8] << pProperties->component[9] << pProperties->component[10]
           << pProperties->component[11] << pProperties->component[12] << pProperties->component[13]
           << pProperties->component[14] << pProperties->component[15] << pProperties->component[16]
           << pProperties->component[17] << pProperties->component[18] << pProperties->component[19]
           << pProperties->component[20] << pProperties->component[21] << pProperties->component[22]
           << pProperties->component[23] << pProperties->component[24] << pProperties->component[25]
           << pProperties->component[26] << pProperties->component[27] << pProperties->component[28]
           << pProperties->component[29] << pProperties->component[30] << pProperties->component[31]
           << pProperties->component[32] << pProperties->component[33] << pProperties->component[34]
           << pProperties->component[35] << pProperties->component[36] << pProperties->component[37]
           << pProperties->component[38] << pProperties->component[39] << pProperties->component[40]
           << pProperties->component[41] << pProperties->component[42] << pProperties->component[43]
           << pProperties->component[44] << pProperties->component[45] << pProperties->component[46]
           << pProperties->component[47] << pProperties->component[48] << pProperties->component[49]
           << pProperties->component[50] << pProperties->component[51] << pProperties->component[52]
           << pProperties->component[53] << pProperties->component[54] << pProperties->component[55]
           << pProperties->component[56] << pProperties->component[57] << pProperties->component[58]
           << pProperties->component[59] << pProperties->component[60] << pProperties->component[61]
           << pProperties->component[62] << pProperties->component[63] << pProperties->component[64]
           << pProperties->component[65] << pProperties->component[66] << pProperties->component[67]
           << pProperties->component[68] << pProperties->component[69] << pProperties->component[70]
           << pProperties->component[71] << pProperties->component[72] << pProperties->component[73]
           << pProperties->component[74] << pProperties->component[75] << pProperties->component[76]
           << pProperties->component[77] << pProperties->component[78] << pProperties->component[79]
           << pProperties->component[80] << pProperties->component[81] << pProperties->component[82]
           << pProperties->component[83] << pProperties->component[84] << pProperties->component[85]
           << pProperties->component[86] << pProperties->component[87] << pProperties->component[88]
           << pProperties->component[89] << pProperties->component[90] << pProperties->component[91]
           << pProperties->component[92] << pProperties->component[93] << pProperties->component[94]
           << pProperties->component[95] << pProperties->component[96] << pProperties->component[97]
           << pProperties->component[98] << pProperties->component[99]
           << pProperties->component[100] << pProperties->component[101]
           << pProperties->component[102] << pProperties->component[103]
           << pProperties->component[104] << pProperties->component[105]
           << pProperties->component[106] << pProperties->component[107]
           << pProperties->component[108] << pProperties->component[109]
           << pProperties->component[110] << pProperties->component[111]
           << pProperties->component[112] << pProperties->component[113]
           << pProperties->component[114] << pProperties->component[115]
           << pProperties->component[116] << pProperties->component[117]
           << pProperties->component[118] << pProperties->component[119]
           << pProperties->component[120] << pProperties->component[121]
           << pProperties->component[122] << pProperties->component[123]
           << pProperties->component[124] << pProperties->component[125]
           << pProperties->component[126] << pProperties->component[127]
           << pProperties->component[128] << pProperties->component[129]
           << pProperties->component[130] << pProperties->component[131]
           << pProperties->component[132] << pProperties->component[133]
           << pProperties->component[134] << pProperties->component[135]
           << pProperties->component[136] << pProperties->component[137]
           << pProperties->component[138] << pProperties->component[139]
           << pProperties->component[140] << pProperties->component[141]
           << pProperties->component[142] << pProperties->component[143]
           << pProperties->component[144] << pProperties->component[145]
           << pProperties->component[146] << pProperties->component[147]
           << pProperties->component[148] << pProperties->component[149]
           << pProperties->component[150] << pProperties->component[151]
           << pProperties->component[152] << pProperties->component[153]
           << pProperties->component[154] << pProperties->component[155]
           << pProperties->component[156] << pProperties->component[157]
           << pProperties->component[158] << pProperties->component[159]
           << pProperties->component[160] << pProperties->component[161]
           << pProperties->component[162] << pProperties->component[163]
           << pProperties->component[164] << pProperties->component[165]
           << pProperties->component[166] << pProperties->component[167]
           << pProperties->component[168] << pProperties->component[169]
           << pProperties->component[170] << pProperties->component[171]
           << pProperties->component[172] << pProperties->component[173]
           << pProperties->component[174] << pProperties->component[175]
           << pProperties->component[176] << pProperties->component[177]
           << pProperties->component[178] << pProperties->component[179]
           << pProperties->component[180] << pProperties->component[181]
           << pProperties->component[182] << pProperties->component[183]
           << pProperties->component[184] << pProperties->component[185]
           << pProperties->component[186] << pProperties->component[187]
           << pProperties->component[188] << pProperties->component[189]
           << pProperties->component[190] << pProperties->component[191]
           << pProperties->component[192] << pProperties->component[193]
           << pProperties->component[194] << pProperties->component[195]
           << pProperties->component[196] << pProperties->component[197]
           << pProperties->component[198] << pProperties->component[199]
           << pProperties->component[200] << pProperties->component[201]
           << pProperties->component[202] << pProperties->component[203]
           << pProperties->component[204] << pProperties->component[205]
           << pProperties->component[206] << pProperties->component[207]
           << pProperties->component[208] << pProperties->component[209]
           << pProperties->component[210] << pProperties->component[211]
           << pProperties->component[212] << pProperties->component[213]
           << pProperties->component[214] << pProperties->component[215]
           << pProperties->component[216] << pProperties->component[217]
           << pProperties->component[218] << pProperties->component[219]
           << pProperties->component[220] << pProperties->component[221]
           << pProperties->component[222] << pProperties->component[223]
           << pProperties->component[224] << pProperties->component[225]
           << pProperties->component[226] << pProperties->component[227]
           << pProperties->component[228] << pProperties->component[229]
           << pProperties->component[230] << pProperties->component[231]
           << pProperties->component[232] << pProperties->component[233]
           << pProperties->component[234] << pProperties->component[235]
           << pProperties->component[236] << pProperties->component[237]
           << pProperties->component[238] << pProperties->component[239]
           << pProperties->component[240] << pProperties->component[241]
           << pProperties->component[242] << pProperties->component[243]
           << pProperties->component[244] << pProperties->component[245]
           << pProperties->component[246] << pProperties->component[247]
           << pProperties->component[248] << pProperties->component[249]
           << pProperties->component[250] << pProperties->component[251]
           << pProperties->component[252] << pProperties->component[253]
           << pProperties->component[254] << pProperties->component[255];
        ss << ", tierNumber: " << pProperties->tierNumber;
        ss << ", metricType: " << pProperties->metricType;
        ss << ", resultType: " << pProperties->resultType;
        ss << ", resultUnits: " << pProperties->resultUnits[0] << pProperties->resultUnits[1]
           << pProperties->resultUnits[2] << pProperties->resultUnits[3]
           << pProperties->resultUnits[4] << pProperties->resultUnits[5]
           << pProperties->resultUnits[6] << pProperties->resultUnits[7]
           << pProperties->resultUnits[8] << pProperties->resultUnits[9]
           << pProperties->resultUnits[10] << pProperties->resultUnits[11]
           << pProperties->resultUnits[12] << pProperties->resultUnits[13]
           << pProperties->resultUnits[14] << pProperties->resultUnits[15]
           << pProperties->resultUnits[16] << pProperties->resultUnits[17]
           << pProperties->resultUnits[18] << pProperties->resultUnits[19]
           << pProperties->resultUnits[20] << pProperties->resultUnits[21]
           << pProperties->resultUnits[22] << pProperties->resultUnits[23]
           << pProperties->resultUnits[24] << pProperties->resultUnits[25]
           << pProperties->resultUnits[26] << pProperties->resultUnits[27]
           << pProperties->resultUnits[28] << pProperties->resultUnits[29]
           << pProperties->resultUnits[30] << pProperties->resultUnits[31]
           << pProperties->resultUnits[32] << pProperties->resultUnits[33]
           << pProperties->resultUnits[34] << pProperties->resultUnits[35]
           << pProperties->resultUnits[36] << pProperties->resultUnits[37]
           << pProperties->resultUnits[38] << pProperties->resultUnits[39]
           << pProperties->resultUnits[40] << pProperties->resultUnits[41]
           << pProperties->resultUnits[42] << pProperties->resultUnits[43]
           << pProperties->resultUnits[44] << pProperties->resultUnits[45]
           << pProperties->resultUnits[46] << pProperties->resultUnits[47]
           << pProperties->resultUnits[48] << pProperties->resultUnits[49]
           << pProperties->resultUnits[50] << pProperties->resultUnits[51]
           << pProperties->resultUnits[52] << pProperties->resultUnits[53]
           << pProperties->resultUnits[54] << pProperties->resultUnits[55]
           << pProperties->resultUnits[56] << pProperties->resultUnits[57]
           << pProperties->resultUnits[58] << pProperties->resultUnits[59]
           << pProperties->resultUnits[60] << pProperties->resultUnits[61]
           << pProperties->resultUnits[62] << pProperties->resultUnits[63]
           << pProperties->resultUnits[64] << pProperties->resultUnits[65]
           << pProperties->resultUnits[66] << pProperties->resultUnits[67]
           << pProperties->resultUnits[68] << pProperties->resultUnits[69]
           << pProperties->resultUnits[70] << pProperties->resultUnits[71]
           << pProperties->resultUnits[72] << pProperties->resultUnits[73]
           << pProperties->resultUnits[74] << pProperties->resultUnits[75]
           << pProperties->resultUnits[76] << pProperties->resultUnits[77]
           << pProperties->resultUnits[78] << pProperties->resultUnits[79]
           << pProperties->resultUnits[80] << pProperties->resultUnits[81]
           << pProperties->resultUnits[82] << pProperties->resultUnits[83]
           << pProperties->resultUnits[84] << pProperties->resultUnits[85]
           << pProperties->resultUnits[86] << pProperties->resultUnits[87]
           << pProperties->resultUnits[88] << pProperties->resultUnits[89]
           << pProperties->resultUnits[90] << pProperties->resultUnits[91]
           << pProperties->resultUnits[92] << pProperties->resultUnits[93]
           << pProperties->resultUnits[94] << pProperties->resultUnits[95]
           << pProperties->resultUnits[96] << pProperties->resultUnits[97]
           << pProperties->resultUnits[98] << pProperties->resultUnits[99]
           << pProperties->resultUnits[100] << pProperties->resultUnits[101]
           << pProperties->resultUnits[102] << pProperties->resultUnits[103]
           << pProperties->resultUnits[104] << pProperties->resultUnits[105]
           << pProperties->resultUnits[106] << pProperties->resultUnits[107]
           << pProperties->resultUnits[108] << pProperties->resultUnits[109]
           << pProperties->resultUnits[110] << pProperties->resultUnits[111]
           << pProperties->resultUnits[112] << pProperties->resultUnits[113]
           << pProperties->resultUnits[114] << pProperties->resultUnits[115]
           << pProperties->resultUnits[116] << pProperties->resultUnits[117]
           << pProperties->resultUnits[118] << pProperties->resultUnits[119]
           << pProperties->resultUnits[120] << pProperties->resultUnits[121]
           << pProperties->resultUnits[122] << pProperties->resultUnits[123]
           << pProperties->resultUnits[124] << pProperties->resultUnits[125]
           << pProperties->resultUnits[126] << pProperties->resultUnits[127]
           << pProperties->resultUnits[128] << pProperties->resultUnits[129]
           << pProperties->resultUnits[130] << pProperties->resultUnits[131]
           << pProperties->resultUnits[132] << pProperties->resultUnits[133]
           << pProperties->resultUnits[134] << pProperties->resultUnits[135]
           << pProperties->resultUnits[136] << pProperties->resultUnits[137]
           << pProperties->resultUnits[138] << pProperties->resultUnits[139]
           << pProperties->resultUnits[140] << pProperties->resultUnits[141]
           << pProperties->resultUnits[142] << pProperties->resultUnits[143]
           << pProperties->resultUnits[144] << pProperties->resultUnits[145]
           << pProperties->resultUnits[146] << pProperties->resultUnits[147]
           << pProperties->resultUnits[148] << pProperties->resultUnits[149]
           << pProperties->resultUnits[150] << pProperties->resultUnits[151]
           << pProperties->resultUnits[152] << pProperties->resultUnits[153]
           << pProperties->resultUnits[154] << pProperties->resultUnits[155]
           << pProperties->resultUnits[156] << pProperties->resultUnits[157]
           << pProperties->resultUnits[158] << pProperties->resultUnits[159]
           << pProperties->resultUnits[160] << pProperties->resultUnits[161]
           << pProperties->resultUnits[162] << pProperties->resultUnits[163]
           << pProperties->resultUnits[164] << pProperties->resultUnits[165]
           << pProperties->resultUnits[166] << pProperties->resultUnits[167]
           << pProperties->resultUnits[168] << pProperties->resultUnits[169]
           << pProperties->resultUnits[170] << pProperties->resultUnits[171]
           << pProperties->resultUnits[172] << pProperties->resultUnits[173]
           << pProperties->resultUnits[174] << pProperties->resultUnits[175]
           << pProperties->resultUnits[176] << pProperties->resultUnits[177]
           << pProperties->resultUnits[178] << pProperties->resultUnits[179]
           << pProperties->resultUnits[180] << pProperties->resultUnits[181]
           << pProperties->resultUnits[182] << pProperties->resultUnits[183]
           << pProperties->resultUnits[184] << pProperties->resultUnits[185]
           << pProperties->resultUnits[186] << pProperties->resultUnits[187]
           << pProperties->resultUnits[188] << pProperties->resultUnits[189]
           << pProperties->resultUnits[190] << pProperties->resultUnits[191]
           << pProperties->resultUnits[192] << pProperties->resultUnits[193]
           << pProperties->resultUnits[194] << pProperties->resultUnits[195]
           << pProperties->resultUnits[196] << pProperties->resultUnits[197]
           << pProperties->resultUnits[198] << pProperties->resultUnits[199]
           << pProperties->resultUnits[200] << pProperties->resultUnits[201]
           << pProperties->resultUnits[202] << pProperties->resultUnits[203]
           << pProperties->resultUnits[204] << pProperties->resultUnits[205]
           << pProperties->resultUnits[206] << pProperties->resultUnits[207]
           << pProperties->resultUnits[208] << pProperties->resultUnits[209]
           << pProperties->resultUnits[210] << pProperties->resultUnits[211]
           << pProperties->resultUnits[212] << pProperties->resultUnits[213]
           << pProperties->resultUnits[214] << pProperties->resultUnits[215]
           << pProperties->resultUnits[216] << pProperties->resultUnits[217]
           << pProperties->resultUnits[218] << pProperties->resultUnits[219]
           << pProperties->resultUnits[220] << pProperties->resultUnits[221]
           << pProperties->resultUnits[222] << pProperties->resultUnits[223]
           << pProperties->resultUnits[224] << pProperties->resultUnits[225]
           << pProperties->resultUnits[226] << pProperties->resultUnits[227]
           << pProperties->resultUnits[228] << pProperties->resultUnits[229]
           << pProperties->resultUnits[230] << pProperties->resultUnits[231]
           << pProperties->resultUnits[232] << pProperties->resultUnits[233]
           << pProperties->resultUnits[234] << pProperties->resultUnits[235]
           << pProperties->resultUnits[236] << pProperties->resultUnits[237]
           << pProperties->resultUnits[238] << pProperties->resultUnits[239]
           << pProperties->resultUnits[240] << pProperties->resultUnits[241]
           << pProperties->resultUnits[242] << pProperties->resultUnits[243]
           << pProperties->resultUnits[244] << pProperties->resultUnits[245]
           << pProperties->resultUnits[246] << pProperties->resultUnits[247]
           << pProperties->resultUnits[248] << pProperties->resultUnits[249]
           << pProperties->resultUnits[250] << pProperties->resultUnits[251]
           << pProperties->resultUnits[252] << pProperties->resultUnits[253]
           << pProperties->resultUnits[254] << pProperties->resultUnits[255];
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGetProperties(zet_metric_handle_t hMetric,
                                         zet_metric_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zetMetricGetProperties");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGetProperties(hMetric, pProperties) + "..\n";
}
inline void trace_zetMetricGetProperties(ze_result_t ret,
                                         zet_metric_handle_t hMetric,
                                         zet_metric_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGetProperties(hMetric, pProperties) + trace_ze_result_t(ret);
}

inline std::string
_trace_zetContextActivateMetricGroups(zet_context_handle_t hContext,
                                      zet_device_handle_t hDevice,
                                      uint32_t count,
                                      zet_metric_group_handle_t *phMetricGroups) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetContextActivateMetricGroups(";
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", count: " << count;
    if (phMetricGroups == nullptr) {
        ss << ", phMetricGroups: nullptr";
    } else {
        ss << ", phMetricGroups: " << *phMetricGroups;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetContextActivateMetricGroups(zet_context_handle_t hContext,
                                                 zet_device_handle_t hDevice,
                                                 uint32_t count,
                                                 zet_metric_group_handle_t *phMetricGroups) {
    TRACE_EVENT_BEGIN("API", "zetContextActivateMetricGroups");
    if (IS_API_TRACE())
        std::cerr << _trace_zetContextActivateMetricGroups(hContext,
                                                           hDevice,
                                                           count,
                                                           phMetricGroups) +
                         "..\n";
}
inline void trace_zetContextActivateMetricGroups(ze_result_t ret,
                                                 zet_context_handle_t hContext,
                                                 zet_device_handle_t hDevice,
                                                 uint32_t count,
                                                 zet_metric_group_handle_t *phMetricGroups) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetContextActivateMetricGroups(hContext,
                                                           hDevice,
                                                           count,
                                                           phMetricGroups) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricStreamerOpen(zet_context_handle_t hContext,
                                                zet_device_handle_t hDevice,
                                                zet_metric_group_handle_t hMetricGroup,
                                                zet_metric_streamer_desc_t *desc,
                                                ze_event_handle_t hNotificationEvent,
                                                zet_metric_streamer_handle_t *phMetricStreamer) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricStreamerOpen(";
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", hMetricGroup: " << hMetricGroup;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", notifyEveryNReports: " << desc->notifyEveryNReports;
        ss << ", samplingPeriod: " << desc->samplingPeriod;
        ss << "}";
    }
    ss << ", hNotificationEvent: " << hNotificationEvent;
    if (phMetricStreamer == nullptr) {
        ss << ", phMetricStreamer: nullptr";
    } else {
        ss << ", phMetricStreamer: " << *phMetricStreamer;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricStreamerOpen(zet_context_handle_t hContext,
                                        zet_device_handle_t hDevice,
                                        zet_metric_group_handle_t hMetricGroup,
                                        zet_metric_streamer_desc_t *desc,
                                        ze_event_handle_t hNotificationEvent,
                                        zet_metric_streamer_handle_t *phMetricStreamer) {
    TRACE_EVENT_BEGIN("API", "zetMetricStreamerOpen");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricStreamerOpen(hContext,
                                                  hDevice,
                                                  hMetricGroup,
                                                  desc,
                                                  hNotificationEvent,
                                                  phMetricStreamer) +
                         "..\n";
}
inline void trace_zetMetricStreamerOpen(ze_result_t ret,
                                        zet_context_handle_t hContext,
                                        zet_device_handle_t hDevice,
                                        zet_metric_group_handle_t hMetricGroup,
                                        zet_metric_streamer_desc_t *desc,
                                        ze_event_handle_t hNotificationEvent,
                                        zet_metric_streamer_handle_t *phMetricStreamer) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricStreamerOpen(hContext,
                                                  hDevice,
                                                  hMetricGroup,
                                                  desc,
                                                  hNotificationEvent,
                                                  phMetricStreamer) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetCommandListAppendMetricStreamerMarker(zet_command_list_handle_t hCommandList,
                                                zet_metric_streamer_handle_t hMetricStreamer,
                                                uint32_t value) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetCommandListAppendMetricStreamerMarker(";
    ss << "hCommandList: " << hCommandList;
    ss << ", hMetricStreamer: " << hMetricStreamer;
    ss << ", value: " << value;
    ss << ")";
    return ss.str();
}
inline void
trace_zetCommandListAppendMetricStreamerMarker(zet_command_list_handle_t hCommandList,
                                               zet_metric_streamer_handle_t hMetricStreamer,
                                               uint32_t value) {
    TRACE_EVENT_BEGIN("API", "zetCommandListAppendMetricStreamerMarker");
    if (IS_API_TRACE())
        std::cerr << _trace_zetCommandListAppendMetricStreamerMarker(hCommandList,
                                                                     hMetricStreamer,
                                                                     value) +
                         "..\n";
}
inline void
trace_zetCommandListAppendMetricStreamerMarker(ze_result_t ret,
                                               zet_command_list_handle_t hCommandList,
                                               zet_metric_streamer_handle_t hMetricStreamer,
                                               uint32_t value) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetCommandListAppendMetricStreamerMarker(hCommandList,
                                                                     hMetricStreamer,
                                                                     value) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricStreamerClose(zet_metric_streamer_handle_t hMetricStreamer) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricStreamerClose(";
    ss << "hMetricStreamer: " << hMetricStreamer;
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricStreamerClose(zet_metric_streamer_handle_t hMetricStreamer) {
    TRACE_EVENT_BEGIN("API", "zetMetricStreamerClose");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricStreamerClose(hMetricStreamer) + "..\n";
}
inline void trace_zetMetricStreamerClose(ze_result_t ret,
                                         zet_metric_streamer_handle_t hMetricStreamer) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricStreamerClose(hMetricStreamer) + trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricStreamerReadData(zet_metric_streamer_handle_t hMetricStreamer,
                                                    uint32_t maxReportCount,
                                                    size_t *pRawDataSize,
                                                    uint8_t *pRawData) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricStreamerReadData(";
    ss << "hMetricStreamer: " << hMetricStreamer;
    ss << ", maxReportCount: " << maxReportCount;
    if (pRawDataSize == nullptr) {
        ss << ", pRawDataSize: nullptr";
    } else {
        ss << ", pRawDataSize: " << *pRawDataSize;
    }
    if (pRawData == nullptr) {
        ss << ", pRawData: nullptr";
    } else {
        ss << ", pRawData: " << *pRawData;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricStreamerReadData(zet_metric_streamer_handle_t hMetricStreamer,
                                            uint32_t maxReportCount,
                                            size_t *pRawDataSize,
                                            uint8_t *pRawData) {
    TRACE_EVENT_BEGIN("API", "zetMetricStreamerReadData");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricStreamerReadData(hMetricStreamer,
                                                      maxReportCount,
                                                      pRawDataSize,
                                                      pRawData) +
                         "..\n";
}
inline void trace_zetMetricStreamerReadData(ze_result_t ret,
                                            zet_metric_streamer_handle_t hMetricStreamer,
                                            uint32_t maxReportCount,
                                            size_t *pRawDataSize,
                                            uint8_t *pRawData) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricStreamerReadData(hMetricStreamer,
                                                      maxReportCount,
                                                      pRawDataSize,
                                                      pRawData) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetMetricQueryPoolCreate(zet_context_handle_t hContext,
                                zet_device_handle_t hDevice,
                                zet_metric_group_handle_t hMetricGroup,
                                const zet_metric_query_pool_desc_t *desc,
                                zet_metric_query_pool_handle_t *phMetricQueryPool) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricQueryPoolCreate(";
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", hMetricGroup: " << hMetricGroup;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", type: " << desc->type;
        ss << ", count: " << desc->count;
        ss << "}";
    }
    if (phMetricQueryPool == nullptr) {
        ss << ", phMetricQueryPool: nullptr";
    } else {
        ss << ", phMetricQueryPool: " << *phMetricQueryPool;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricQueryPoolCreate(zet_context_handle_t hContext,
                                           zet_device_handle_t hDevice,
                                           zet_metric_group_handle_t hMetricGroup,
                                           const zet_metric_query_pool_desc_t *desc,
                                           zet_metric_query_pool_handle_t *phMetricQueryPool) {
    TRACE_EVENT_BEGIN("API", "zetMetricQueryPoolCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricQueryPoolCreate(hContext,
                                                     hDevice,
                                                     hMetricGroup,
                                                     desc,
                                                     phMetricQueryPool) +
                         "..\n";
}
inline void trace_zetMetricQueryPoolCreate(ze_result_t ret,
                                           zet_context_handle_t hContext,
                                           zet_device_handle_t hDevice,
                                           zet_metric_group_handle_t hMetricGroup,
                                           const zet_metric_query_pool_desc_t *desc,
                                           zet_metric_query_pool_handle_t *phMetricQueryPool) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricQueryPoolCreate(hContext,
                                                     hDevice,
                                                     hMetricGroup,
                                                     desc,
                                                     phMetricQueryPool) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetMetricQueryPoolDestroy(zet_metric_query_pool_handle_t hMetricQueryPool) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricQueryPoolDestroy(";
    ss << "hMetricQueryPool: " << hMetricQueryPool;
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricQueryPoolDestroy(zet_metric_query_pool_handle_t hMetricQueryPool) {
    TRACE_EVENT_BEGIN("API", "zetMetricQueryPoolDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricQueryPoolDestroy(hMetricQueryPool) + "..\n";
}
inline void trace_zetMetricQueryPoolDestroy(ze_result_t ret,
                                            zet_metric_query_pool_handle_t hMetricQueryPool) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricQueryPoolDestroy(hMetricQueryPool) + trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricQueryCreate(zet_metric_query_pool_handle_t hMetricQueryPool,
                                               uint32_t index,
                                               zet_metric_query_handle_t *phMetricQuery) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricQueryCreate(";
    ss << "hMetricQueryPool: " << hMetricQueryPool;
    ss << ", index: " << index;
    if (phMetricQuery == nullptr) {
        ss << ", phMetricQuery: nullptr";
    } else {
        ss << ", phMetricQuery: " << *phMetricQuery;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricQueryCreate(zet_metric_query_pool_handle_t hMetricQueryPool,
                                       uint32_t index,
                                       zet_metric_query_handle_t *phMetricQuery) {
    TRACE_EVENT_BEGIN("API", "zetMetricQueryCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricQueryCreate(hMetricQueryPool, index, phMetricQuery) + "..\n";
}
inline void trace_zetMetricQueryCreate(ze_result_t ret,
                                       zet_metric_query_pool_handle_t hMetricQueryPool,
                                       uint32_t index,
                                       zet_metric_query_handle_t *phMetricQuery) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricQueryCreate(hMetricQueryPool, index, phMetricQuery) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricQueryDestroy(zet_metric_query_handle_t hMetricQuery) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricQueryDestroy(";
    ss << "hMetricQuery: " << hMetricQuery;
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricQueryDestroy(zet_metric_query_handle_t hMetricQuery) {
    TRACE_EVENT_BEGIN("API", "zetMetricQueryDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricQueryDestroy(hMetricQuery) + "..\n";
}
inline void trace_zetMetricQueryDestroy(ze_result_t ret, zet_metric_query_handle_t hMetricQuery) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricQueryDestroy(hMetricQuery) + trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricQueryReset(zet_metric_query_handle_t hMetricQuery) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricQueryReset(";
    ss << "hMetricQuery: " << hMetricQuery;
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricQueryReset(zet_metric_query_handle_t hMetricQuery) {
    TRACE_EVENT_BEGIN("API", "zetMetricQueryReset");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricQueryReset(hMetricQuery) + "..\n";
}
inline void trace_zetMetricQueryReset(ze_result_t ret, zet_metric_query_handle_t hMetricQuery) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricQueryReset(hMetricQuery) + trace_ze_result_t(ret);
}

inline std::string
_trace_zetCommandListAppendMetricQueryBegin(zet_command_list_handle_t hCommandList,
                                            zet_metric_query_handle_t hMetricQuery) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetCommandListAppendMetricQueryBegin(";
    ss << "hCommandList: " << hCommandList;
    ss << ", hMetricQuery: " << hMetricQuery;
    ss << ")";
    return ss.str();
}
inline void trace_zetCommandListAppendMetricQueryBegin(zet_command_list_handle_t hCommandList,
                                                       zet_metric_query_handle_t hMetricQuery) {
    TRACE_EVENT_BEGIN("API", "zetCommandListAppendMetricQueryBegin");
    if (IS_API_TRACE())
        std::cerr << _trace_zetCommandListAppendMetricQueryBegin(hCommandList, hMetricQuery) +
                         "..\n";
}
inline void trace_zetCommandListAppendMetricQueryBegin(ze_result_t ret,
                                                       zet_command_list_handle_t hCommandList,
                                                       zet_metric_query_handle_t hMetricQuery) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetCommandListAppendMetricQueryBegin(hCommandList, hMetricQuery) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetCommandListAppendMetricQueryEnd(zet_command_list_handle_t hCommandList,
                                                             zet_metric_query_handle_t hMetricQuery,
                                                             ze_event_handle_t hSignalEvent,
                                                             uint32_t numWaitEvents,
                                                             ze_event_handle_t *phWaitEvents) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetCommandListAppendMetricQueryEnd(";
    ss << "hCommandList: " << hCommandList;
    ss << ", hMetricQuery: " << hMetricQuery;
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
inline void trace_zetCommandListAppendMetricQueryEnd(zet_command_list_handle_t hCommandList,
                                                     zet_metric_query_handle_t hMetricQuery,
                                                     ze_event_handle_t hSignalEvent,
                                                     uint32_t numWaitEvents,
                                                     ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_BEGIN("API", "zetCommandListAppendMetricQueryEnd");
    if (IS_API_TRACE())
        std::cerr << _trace_zetCommandListAppendMetricQueryEnd(hCommandList,
                                                               hMetricQuery,
                                                               hSignalEvent,
                                                               numWaitEvents,
                                                               phWaitEvents) +
                         "..\n";
}
inline void trace_zetCommandListAppendMetricQueryEnd(ze_result_t ret,
                                                     zet_command_list_handle_t hCommandList,
                                                     zet_metric_query_handle_t hMetricQuery,
                                                     ze_event_handle_t hSignalEvent,
                                                     uint32_t numWaitEvents,
                                                     ze_event_handle_t *phWaitEvents) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetCommandListAppendMetricQueryEnd(hCommandList,
                                                               hMetricQuery,
                                                               hSignalEvent,
                                                               numWaitEvents,
                                                               phWaitEvents) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetCommandListAppendMetricMemoryBarrier(zet_command_list_handle_t hCommandList) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetCommandListAppendMetricMemoryBarrier(";
    ss << "hCommandList: " << hCommandList;
    ss << ")";
    return ss.str();
}
inline void trace_zetCommandListAppendMetricMemoryBarrier(zet_command_list_handle_t hCommandList) {
    TRACE_EVENT_BEGIN("API", "zetCommandListAppendMetricMemoryBarrier");
    if (IS_API_TRACE())
        std::cerr << _trace_zetCommandListAppendMetricMemoryBarrier(hCommandList) + "..\n";
}
inline void trace_zetCommandListAppendMetricMemoryBarrier(ze_result_t ret,
                                                          zet_command_list_handle_t hCommandList) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetCommandListAppendMetricMemoryBarrier(hCommandList) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricQueryGetData(zet_metric_query_handle_t hMetricQuery,
                                                size_t *pRawDataSize,
                                                uint8_t *pRawData) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricQueryGetData(";
    ss << "hMetricQuery: " << hMetricQuery;
    if (pRawDataSize == nullptr) {
        ss << ", pRawDataSize: nullptr";
    } else {
        ss << ", pRawDataSize: " << *pRawDataSize;
    }
    if (pRawData == nullptr) {
        ss << ", pRawData: nullptr";
    } else {
        ss << ", pRawData: " << *pRawData;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricQueryGetData(zet_metric_query_handle_t hMetricQuery,
                                        size_t *pRawDataSize,
                                        uint8_t *pRawData) {
    TRACE_EVENT_BEGIN("API", "zetMetricQueryGetData");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricQueryGetData(hMetricQuery, pRawDataSize, pRawData) + "..\n";
}
inline void trace_zetMetricQueryGetData(ze_result_t ret,
                                        zet_metric_query_handle_t hMetricQuery,
                                        size_t *pRawDataSize,
                                        uint8_t *pRawData) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricQueryGetData(hMetricQuery, pRawDataSize, pRawData) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetKernelGetProfileInfo(zet_kernel_handle_t hKernel,
                                                  zet_profile_properties_t *pProfileProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetKernelGetProfileInfo(";
    ss << "hKernel: " << hKernel;
    if (pProfileProperties == nullptr) {
        ss << ", pProfileProperties: nullptr";
    } else {
        ss << ", pProfileProperties {";
        ss << "stype: " << pProfileProperties->stype;
        ss << ", pNext: " << pProfileProperties->pNext;
        ss << ", flags: " << pProfileProperties->flags;
        ss << ", numTokens: " << pProfileProperties->numTokens;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetKernelGetProfileInfo(zet_kernel_handle_t hKernel,
                                          zet_profile_properties_t *pProfileProperties) {
    TRACE_EVENT_BEGIN("API", "zetKernelGetProfileInfo");
    if (IS_API_TRACE())
        std::cerr << _trace_zetKernelGetProfileInfo(hKernel, pProfileProperties) + "..\n";
}
inline void trace_zetKernelGetProfileInfo(ze_result_t ret,
                                          zet_kernel_handle_t hKernel,
                                          zet_profile_properties_t *pProfileProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetKernelGetProfileInfo(hKernel, pProfileProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetTracerExpCreate(zet_context_handle_t hContext,
                                             const zet_tracer_exp_desc_t *desc,
                                             zet_tracer_exp_handle_t *phTracer) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetTracerExpCreate(";
    ss << "hContext: " << hContext;
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", pUserData: " << desc->pUserData;
        ss << "}";
    }
    if (phTracer == nullptr) {
        ss << ", phTracer: nullptr";
    } else {
        ss << ", phTracer: " << *phTracer;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetTracerExpCreate(zet_context_handle_t hContext,
                                     const zet_tracer_exp_desc_t *desc,
                                     zet_tracer_exp_handle_t *phTracer) {
    TRACE_EVENT_BEGIN("API", "zetTracerExpCreate");
    if (IS_API_TRACE())
        std::cerr << _trace_zetTracerExpCreate(hContext, desc, phTracer) + "..\n";
}
inline void trace_zetTracerExpCreate(ze_result_t ret,
                                     zet_context_handle_t hContext,
                                     const zet_tracer_exp_desc_t *desc,
                                     zet_tracer_exp_handle_t *phTracer) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetTracerExpCreate(hContext, desc, phTracer) + trace_ze_result_t(ret);
}

inline std::string _trace_zetTracerExpDestroy(zet_tracer_exp_handle_t hTracer) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetTracerExpDestroy(";
    ss << "hTracer: " << hTracer;
    ss << ")";
    return ss.str();
}
inline void trace_zetTracerExpDestroy(zet_tracer_exp_handle_t hTracer) {
    TRACE_EVENT_BEGIN("API", "zetTracerExpDestroy");
    if (IS_API_TRACE())
        std::cerr << _trace_zetTracerExpDestroy(hTracer) + "..\n";
}
inline void trace_zetTracerExpDestroy(ze_result_t ret, zet_tracer_exp_handle_t hTracer) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetTracerExpDestroy(hTracer) + trace_ze_result_t(ret);
}

inline std::string _trace_zetTracerExpSetPrologues(zet_tracer_exp_handle_t hTracer,
                                                   zet_core_callbacks_t *pCoreCbs) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetTracerExpSetPrologues(";
    ss << "hTracer: " << hTracer;
    if (pCoreCbs == nullptr) {
        ss << ", pCoreCbs: nullptr";
    } else {
        ss << ", pCoreCbs: " << pCoreCbs;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetTracerExpSetPrologues(zet_tracer_exp_handle_t hTracer,
                                           zet_core_callbacks_t *pCoreCbs) {
    TRACE_EVENT_BEGIN("API", "zetTracerExpSetPrologues");
    if (IS_API_TRACE())
        std::cerr << _trace_zetTracerExpSetPrologues(hTracer, pCoreCbs) + "..\n";
}
inline void trace_zetTracerExpSetPrologues(ze_result_t ret,
                                           zet_tracer_exp_handle_t hTracer,
                                           zet_core_callbacks_t *pCoreCbs) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetTracerExpSetPrologues(hTracer, pCoreCbs) + trace_ze_result_t(ret);
}

inline std::string _trace_zetTracerExpSetEpilogues(zet_tracer_exp_handle_t hTracer,
                                                   zet_core_callbacks_t *pCoreCbs) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetTracerExpSetEpilogues(";
    ss << "hTracer: " << hTracer;
    if (pCoreCbs == nullptr) {
        ss << ", pCoreCbs: nullptr";
    } else {
        ss << ", pCoreCbs: " << pCoreCbs;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetTracerExpSetEpilogues(zet_tracer_exp_handle_t hTracer,
                                           zet_core_callbacks_t *pCoreCbs) {
    TRACE_EVENT_BEGIN("API", "zetTracerExpSetEpilogues");
    if (IS_API_TRACE())
        std::cerr << _trace_zetTracerExpSetEpilogues(hTracer, pCoreCbs) + "..\n";
}
inline void trace_zetTracerExpSetEpilogues(ze_result_t ret,
                                           zet_tracer_exp_handle_t hTracer,
                                           zet_core_callbacks_t *pCoreCbs) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetTracerExpSetEpilogues(hTracer, pCoreCbs) + trace_ze_result_t(ret);
}

inline std::string _trace_zetTracerExpSetEnabled(zet_tracer_exp_handle_t hTracer,
                                                 ze_bool_t enable) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetTracerExpSetEnabled(";
    ss << "hTracer: " << hTracer;
    ss << ", enable: " << enable;
    ss << ")";
    return ss.str();
}
inline void trace_zetTracerExpSetEnabled(zet_tracer_exp_handle_t hTracer, ze_bool_t enable) {
    TRACE_EVENT_BEGIN("API", "zetTracerExpSetEnabled");
    if (IS_API_TRACE())
        std::cerr << _trace_zetTracerExpSetEnabled(hTracer, enable) + "..\n";
}
inline void
trace_zetTracerExpSetEnabled(ze_result_t ret, zet_tracer_exp_handle_t hTracer, ze_bool_t enable) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetTracerExpSetEnabled(hTracer, enable) + trace_ze_result_t(ret);
}

inline std::string
_trace_zetDeviceGetConcurrentMetricGroupsExp(zet_device_handle_t hDevice,
                                             uint32_t metricGroupCount,
                                             zet_metric_group_handle_t *phMetricGroups,
                                             uint32_t *pMetricGroupsCountPerConcurrentGroup,
                                             uint32_t *pConcurrentGroupCount) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDeviceGetConcurrentMetricGroupsExp(";
    ss << "hDevice: " << hDevice;
    ss << ", metricGroupCount: " << metricGroupCount;
    if (phMetricGroups == nullptr) {
        ss << ", phMetricGroups: nullptr";
    } else {
        ss << ", phMetricGroups: " << *phMetricGroups;
    }
    if (pMetricGroupsCountPerConcurrentGroup == nullptr) {
        ss << ", pMetricGroupsCountPerConcurrentGroup: nullptr";
    } else {
        ss << ", pMetricGroupsCountPerConcurrentGroup: " << *pMetricGroupsCountPerConcurrentGroup;
    }
    if (pConcurrentGroupCount == nullptr) {
        ss << ", pConcurrentGroupCount: nullptr";
    } else {
        ss << ", pConcurrentGroupCount: " << *pConcurrentGroupCount;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zetDeviceGetConcurrentMetricGroupsExp(zet_device_handle_t hDevice,
                                            uint32_t metricGroupCount,
                                            zet_metric_group_handle_t *phMetricGroups,
                                            uint32_t *pMetricGroupsCountPerConcurrentGroup,
                                            uint32_t *pConcurrentGroupCount) {
    TRACE_EVENT_BEGIN("API", "zetDeviceGetConcurrentMetricGroupsExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDeviceGetConcurrentMetricGroupsExp(
                         hDevice,
                         metricGroupCount,
                         phMetricGroups,
                         pMetricGroupsCountPerConcurrentGroup,
                         pConcurrentGroupCount) +
                         "..\n";
}
inline void
trace_zetDeviceGetConcurrentMetricGroupsExp(ze_result_t ret,
                                            zet_device_handle_t hDevice,
                                            uint32_t metricGroupCount,
                                            zet_metric_group_handle_t *phMetricGroups,
                                            uint32_t *pMetricGroupsCountPerConcurrentGroup,
                                            uint32_t *pConcurrentGroupCount) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDeviceGetConcurrentMetricGroupsExp(
                         hDevice,
                         metricGroupCount,
                         phMetricGroups,
                         pMetricGroupsCountPerConcurrentGroup,
                         pConcurrentGroupCount) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricTracerCreateExp(zet_context_handle_t hContext,
                                                   zet_device_handle_t hDevice,
                                                   uint32_t metricGroupCount,
                                                   zet_metric_group_handle_t *phMetricGroups,
                                                   zet_metric_tracer_exp_desc_t *desc,
                                                   ze_event_handle_t hNotificationEvent,
                                                   zet_metric_tracer_exp_handle_t *phMetricTracer) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricTracerCreateExp(";
    ss << "hContext: " << hContext;
    ss << ", hDevice: " << hDevice;
    ss << ", metricGroupCount: " << metricGroupCount;
    if (phMetricGroups == nullptr) {
        ss << ", phMetricGroups: nullptr";
    } else {
        ss << ", phMetricGroups: " << *phMetricGroups;
    }
    if (desc == nullptr) {
        ss << ", desc: nullptr";
    } else {
        ss << ", desc {";
        ss << "stype: " << desc->stype;
        ss << ", pNext: " << desc->pNext;
        ss << ", notifyEveryNBytes: " << desc->notifyEveryNBytes;
        ss << "}";
    }
    ss << ", hNotificationEvent: " << hNotificationEvent;
    if (phMetricTracer == nullptr) {
        ss << ", phMetricTracer: nullptr";
    } else {
        ss << ", phMetricTracer: " << *phMetricTracer;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricTracerCreateExp(zet_context_handle_t hContext,
                                           zet_device_handle_t hDevice,
                                           uint32_t metricGroupCount,
                                           zet_metric_group_handle_t *phMetricGroups,
                                           zet_metric_tracer_exp_desc_t *desc,
                                           ze_event_handle_t hNotificationEvent,
                                           zet_metric_tracer_exp_handle_t *phMetricTracer) {
    TRACE_EVENT_BEGIN("API", "zetMetricTracerCreateExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricTracerCreateExp(hContext,
                                                     hDevice,
                                                     metricGroupCount,
                                                     phMetricGroups,
                                                     desc,
                                                     hNotificationEvent,
                                                     phMetricTracer) +
                         "..\n";
}
inline void trace_zetMetricTracerCreateExp(ze_result_t ret,
                                           zet_context_handle_t hContext,
                                           zet_device_handle_t hDevice,
                                           uint32_t metricGroupCount,
                                           zet_metric_group_handle_t *phMetricGroups,
                                           zet_metric_tracer_exp_desc_t *desc,
                                           ze_event_handle_t hNotificationEvent,
                                           zet_metric_tracer_exp_handle_t *phMetricTracer) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricTracerCreateExp(hContext,
                                                     hDevice,
                                                     metricGroupCount,
                                                     phMetricGroups,
                                                     desc,
                                                     hNotificationEvent,
                                                     phMetricTracer) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricTracerDestroyExp(zet_metric_tracer_exp_handle_t hMetricTracer) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricTracerDestroyExp(";
    ss << "hMetricTracer: " << hMetricTracer;
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricTracerDestroyExp(zet_metric_tracer_exp_handle_t hMetricTracer) {
    TRACE_EVENT_BEGIN("API", "zetMetricTracerDestroyExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricTracerDestroyExp(hMetricTracer) + "..\n";
}
inline void trace_zetMetricTracerDestroyExp(ze_result_t ret,
                                            zet_metric_tracer_exp_handle_t hMetricTracer) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricTracerDestroyExp(hMetricTracer) + trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricTracerEnableExp(zet_metric_tracer_exp_handle_t hMetricTracer,
                                                   ze_bool_t synchronous) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricTracerEnableExp(";
    ss << "hMetricTracer: " << hMetricTracer;
    ss << ", synchronous: " << synchronous;
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricTracerEnableExp(zet_metric_tracer_exp_handle_t hMetricTracer,
                                           ze_bool_t synchronous) {
    TRACE_EVENT_BEGIN("API", "zetMetricTracerEnableExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricTracerEnableExp(hMetricTracer, synchronous) + "..\n";
}
inline void trace_zetMetricTracerEnableExp(ze_result_t ret,
                                           zet_metric_tracer_exp_handle_t hMetricTracer,
                                           ze_bool_t synchronous) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricTracerEnableExp(hMetricTracer, synchronous) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricTracerDisableExp(zet_metric_tracer_exp_handle_t hMetricTracer,
                                                    ze_bool_t synchronous) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricTracerDisableExp(";
    ss << "hMetricTracer: " << hMetricTracer;
    ss << ", synchronous: " << synchronous;
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricTracerDisableExp(zet_metric_tracer_exp_handle_t hMetricTracer,
                                            ze_bool_t synchronous) {
    TRACE_EVENT_BEGIN("API", "zetMetricTracerDisableExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricTracerDisableExp(hMetricTracer, synchronous) + "..\n";
}
inline void trace_zetMetricTracerDisableExp(ze_result_t ret,
                                            zet_metric_tracer_exp_handle_t hMetricTracer,
                                            ze_bool_t synchronous) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricTracerDisableExp(hMetricTracer, synchronous) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricTracerReadDataExp(zet_metric_tracer_exp_handle_t hMetricTracer,
                                                     size_t *pRawDataSize,
                                                     uint8_t *pRawData) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricTracerReadDataExp(";
    ss << "hMetricTracer: " << hMetricTracer;
    if (pRawDataSize == nullptr) {
        ss << ", pRawDataSize: nullptr";
    } else {
        ss << ", pRawDataSize: " << *pRawDataSize;
    }
    if (pRawData == nullptr) {
        ss << ", pRawData: nullptr";
    } else {
        ss << ", pRawData: " << *pRawData;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricTracerReadDataExp(zet_metric_tracer_exp_handle_t hMetricTracer,
                                             size_t *pRawDataSize,
                                             uint8_t *pRawData) {
    TRACE_EVENT_BEGIN("API", "zetMetricTracerReadDataExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricTracerReadDataExp(hMetricTracer, pRawDataSize, pRawData) +
                         "..\n";
}
inline void trace_zetMetricTracerReadDataExp(ze_result_t ret,
                                             zet_metric_tracer_exp_handle_t hMetricTracer,
                                             size_t *pRawDataSize,
                                             uint8_t *pRawData) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricTracerReadDataExp(hMetricTracer, pRawDataSize, pRawData) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetMetricDecoderCreateExp(zet_metric_tracer_exp_handle_t hMetricTracer,
                                 zet_metric_decoder_exp_handle_t *phMetricDecoder) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricDecoderCreateExp(";
    ss << "hMetricTracer: " << hMetricTracer;
    if (phMetricDecoder == nullptr) {
        ss << ", phMetricDecoder: nullptr";
    } else {
        ss << ", phMetricDecoder: " << *phMetricDecoder;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricDecoderCreateExp(zet_metric_tracer_exp_handle_t hMetricTracer,
                                            zet_metric_decoder_exp_handle_t *phMetricDecoder) {
    TRACE_EVENT_BEGIN("API", "zetMetricDecoderCreateExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricDecoderCreateExp(hMetricTracer, phMetricDecoder) + "..\n";
}
inline void trace_zetMetricDecoderCreateExp(ze_result_t ret,
                                            zet_metric_tracer_exp_handle_t hMetricTracer,
                                            zet_metric_decoder_exp_handle_t *phMetricDecoder) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricDecoderCreateExp(hMetricTracer, phMetricDecoder) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetMetricDecoderDestroyExp(zet_metric_decoder_exp_handle_t phMetricDecoder) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricDecoderDestroyExp(";
    ss << "phMetricDecoder: " << phMetricDecoder;
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricDecoderDestroyExp(zet_metric_decoder_exp_handle_t phMetricDecoder) {
    TRACE_EVENT_BEGIN("API", "zetMetricDecoderDestroyExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricDecoderDestroyExp(phMetricDecoder) + "..\n";
}
inline void trace_zetMetricDecoderDestroyExp(ze_result_t ret,
                                             zet_metric_decoder_exp_handle_t phMetricDecoder) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricDecoderDestroyExp(phMetricDecoder) + trace_ze_result_t(ret);
}

inline std::string
_trace_zetMetricDecoderGetDecodableMetricsExp(zet_metric_decoder_exp_handle_t hMetricDecoder,
                                              uint32_t *pCount,
                                              zet_metric_handle_t *phMetrics) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricDecoderGetDecodableMetricsExp(";
    ss << "hMetricDecoder: " << hMetricDecoder;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phMetrics == nullptr) {
        ss << ", phMetrics: nullptr";
    } else {
        ss << ", phMetrics: " << *phMetrics;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zetMetricDecoderGetDecodableMetricsExp(zet_metric_decoder_exp_handle_t hMetricDecoder,
                                             uint32_t *pCount,
                                             zet_metric_handle_t *phMetrics) {
    TRACE_EVENT_BEGIN("API", "zetMetricDecoderGetDecodableMetricsExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricDecoderGetDecodableMetricsExp(hMetricDecoder,
                                                                   pCount,
                                                                   phMetrics) +
                         "..\n";
}
inline void
trace_zetMetricDecoderGetDecodableMetricsExp(ze_result_t ret,
                                             zet_metric_decoder_exp_handle_t hMetricDecoder,
                                             uint32_t *pCount,
                                             zet_metric_handle_t *phMetrics) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricDecoderGetDecodableMetricsExp(hMetricDecoder,
                                                                   pCount,
                                                                   phMetrics) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricTracerDecodeExp(zet_metric_decoder_exp_handle_t phMetricDecoder,
                                                   size_t *pRawDataSize,
                                                   uint8_t *pRawData,
                                                   uint32_t metricsCount,
                                                   zet_metric_handle_t *phMetrics,
                                                   uint32_t *pSetCount,
                                                   uint32_t *pMetricEntriesCountPerSet,
                                                   uint32_t *pMetricEntriesCount,
                                                   zet_metric_entry_exp_t *pMetricEntries) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricTracerDecodeExp(";
    ss << "phMetricDecoder: " << phMetricDecoder;
    if (pRawDataSize == nullptr) {
        ss << ", pRawDataSize: nullptr";
    } else {
        ss << ", pRawDataSize: " << *pRawDataSize;
    }
    if (pRawData == nullptr) {
        ss << ", pRawData: nullptr";
    } else {
        ss << ", pRawData: " << *pRawData;
    }
    ss << ", metricsCount: " << metricsCount;
    if (phMetrics == nullptr) {
        ss << ", phMetrics: nullptr";
    } else {
        ss << ", phMetrics: " << *phMetrics;
    }
    if (pSetCount == nullptr) {
        ss << ", pSetCount: nullptr";
    } else {
        ss << ", pSetCount: " << *pSetCount;
    }
    if (pMetricEntriesCountPerSet == nullptr) {
        ss << ", pMetricEntriesCountPerSet: nullptr";
    } else {
        ss << ", pMetricEntriesCountPerSet: " << *pMetricEntriesCountPerSet;
    }
    if (pMetricEntriesCount == nullptr) {
        ss << ", pMetricEntriesCount: nullptr";
    } else {
        ss << ", pMetricEntriesCount: " << *pMetricEntriesCount;
    }
    if (pMetricEntries == nullptr) {
        ss << ", pMetricEntries: nullptr";
    } else {
        ss << ", pMetricEntries {";
        ss << ", value {";
        ss << "ui32: " << pMetricEntries->value.ui32;
        ss << ", ui64: " << pMetricEntries->value.ui64;
        ss << ", fp32: " << pMetricEntries->value.fp32;
        ss << ", fp64: " << pMetricEntries->value.fp64;
        ss << ", b8: " << pMetricEntries->value.b8;
        ss << "}";
        ss << ", timeStamp: " << pMetricEntries->timeStamp;
        ss << ", metricIndex: " << pMetricEntries->metricIndex;
        ss << ", onSubdevice: " << pMetricEntries->onSubdevice;
        ss << ", subdeviceId: " << pMetricEntries->subdeviceId;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricTracerDecodeExp(zet_metric_decoder_exp_handle_t phMetricDecoder,
                                           size_t *pRawDataSize,
                                           uint8_t *pRawData,
                                           uint32_t metricsCount,
                                           zet_metric_handle_t *phMetrics,
                                           uint32_t *pSetCount,
                                           uint32_t *pMetricEntriesCountPerSet,
                                           uint32_t *pMetricEntriesCount,
                                           zet_metric_entry_exp_t *pMetricEntries) {
    TRACE_EVENT_BEGIN("API", "zetMetricTracerDecodeExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricTracerDecodeExp(phMetricDecoder,
                                                     pRawDataSize,
                                                     pRawData,
                                                     metricsCount,
                                                     phMetrics,
                                                     pSetCount,
                                                     pMetricEntriesCountPerSet,
                                                     pMetricEntriesCount,
                                                     pMetricEntries) +
                         "..\n";
}
inline void trace_zetMetricTracerDecodeExp(ze_result_t ret,
                                           zet_metric_decoder_exp_handle_t phMetricDecoder,
                                           size_t *pRawDataSize,
                                           uint8_t *pRawData,
                                           uint32_t metricsCount,
                                           zet_metric_handle_t *phMetrics,
                                           uint32_t *pSetCount,
                                           uint32_t *pMetricEntriesCountPerSet,
                                           uint32_t *pMetricEntriesCount,
                                           zet_metric_entry_exp_t *pMetricEntries) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricTracerDecodeExp(phMetricDecoder,
                                                     pRawDataSize,
                                                     pRawData,
                                                     metricsCount,
                                                     phMetrics,
                                                     pSetCount,
                                                     pMetricEntriesCountPerSet,
                                                     pMetricEntriesCount,
                                                     pMetricEntries) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetMetricGroupCalculateMultipleMetricValuesExp(zet_metric_group_handle_t hMetricGroup,
                                                      zet_metric_group_calculation_type_t type,
                                                      size_t rawDataSize,
                                                      const uint8_t *pRawData,
                                                      uint32_t *pSetCount,
                                                      uint32_t *pTotalMetricValueCount,
                                                      uint32_t *pMetricCounts,
                                                      zet_typed_value_t *pMetricValues) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGroupCalculateMultipleMetricValuesExp(";
    ss << "hMetricGroup: " << hMetricGroup;
    ss << ", type: " << type;
    ss << ", rawDataSize: " << rawDataSize;
    if (pRawData == nullptr) {
        ss << ", pRawData: nullptr";
    } else {
        ss << ", pRawData: " << *pRawData;
    }
    if (pSetCount == nullptr) {
        ss << ", pSetCount: nullptr";
    } else {
        ss << ", pSetCount: " << *pSetCount;
    }
    if (pTotalMetricValueCount == nullptr) {
        ss << ", pTotalMetricValueCount: nullptr";
    } else {
        ss << ", pTotalMetricValueCount: " << *pTotalMetricValueCount;
    }
    if (pMetricCounts == nullptr) {
        ss << ", pMetricCounts: nullptr";
    } else {
        ss << ", pMetricCounts: " << *pMetricCounts;
    }
    if (pMetricValues == nullptr) {
        ss << ", pMetricValues: nullptr";
    } else {
        ss << ", pMetricValues {";
        ss << "type: " << pMetricValues->type;
        ss << ", value {";
        ss << "ui32: " << pMetricValues->value.ui32;
        ss << ", ui64: " << pMetricValues->value.ui64;
        ss << ", fp32: " << pMetricValues->value.fp32;
        ss << ", fp64: " << pMetricValues->value.fp64;
        ss << ", b8: " << pMetricValues->value.b8;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zetMetricGroupCalculateMultipleMetricValuesExp(zet_metric_group_handle_t hMetricGroup,
                                                     zet_metric_group_calculation_type_t type,
                                                     size_t rawDataSize,
                                                     const uint8_t *pRawData,
                                                     uint32_t *pSetCount,
                                                     uint32_t *pTotalMetricValueCount,
                                                     uint32_t *pMetricCounts,
                                                     zet_typed_value_t *pMetricValues) {
    TRACE_EVENT_BEGIN("API", "zetMetricGroupCalculateMultipleMetricValuesExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupCalculateMultipleMetricValuesExp(hMetricGroup,
                                                                           type,
                                                                           rawDataSize,
                                                                           pRawData,
                                                                           pSetCount,
                                                                           pTotalMetricValueCount,
                                                                           pMetricCounts,
                                                                           pMetricValues) +
                         "..\n";
}
inline void
trace_zetMetricGroupCalculateMultipleMetricValuesExp(ze_result_t ret,
                                                     zet_metric_group_handle_t hMetricGroup,
                                                     zet_metric_group_calculation_type_t type,
                                                     size_t rawDataSize,
                                                     const uint8_t *pRawData,
                                                     uint32_t *pSetCount,
                                                     uint32_t *pTotalMetricValueCount,
                                                     uint32_t *pMetricCounts,
                                                     zet_typed_value_t *pMetricValues) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupCalculateMultipleMetricValuesExp(hMetricGroup,
                                                                           type,
                                                                           rawDataSize,
                                                                           pRawData,
                                                                           pSetCount,
                                                                           pTotalMetricValueCount,
                                                                           pMetricCounts,
                                                                           pMetricValues) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetMetricGroupGetGlobalTimestampsExp(zet_metric_group_handle_t hMetricGroup,
                                            ze_bool_t synchronizedWithHost,
                                            uint64_t *globalTimestamp,
                                            uint64_t *metricTimestamp) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGroupGetGlobalTimestampsExp(";
    ss << "hMetricGroup: " << hMetricGroup;
    ss << ", synchronizedWithHost: " << synchronizedWithHost;
    if (globalTimestamp == nullptr) {
        ss << ", globalTimestamp: nullptr";
    } else {
        ss << ", globalTimestamp: " << *globalTimestamp;
    }
    if (metricTimestamp == nullptr) {
        ss << ", metricTimestamp: nullptr";
    } else {
        ss << ", metricTimestamp: " << *metricTimestamp;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGroupGetGlobalTimestampsExp(zet_metric_group_handle_t hMetricGroup,
                                                       ze_bool_t synchronizedWithHost,
                                                       uint64_t *globalTimestamp,
                                                       uint64_t *metricTimestamp) {
    TRACE_EVENT_BEGIN("API", "zetMetricGroupGetGlobalTimestampsExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupGetGlobalTimestampsExp(hMetricGroup,
                                                                 synchronizedWithHost,
                                                                 globalTimestamp,
                                                                 metricTimestamp) +
                         "..\n";
}
inline void trace_zetMetricGroupGetGlobalTimestampsExp(ze_result_t ret,
                                                       zet_metric_group_handle_t hMetricGroup,
                                                       ze_bool_t synchronizedWithHost,
                                                       uint64_t *globalTimestamp,
                                                       uint64_t *metricTimestamp) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupGetGlobalTimestampsExp(hMetricGroup,
                                                                 synchronizedWithHost,
                                                                 globalTimestamp,
                                                                 metricTimestamp) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricGroupGetExportDataExp(zet_metric_group_handle_t hMetricGroup,
                                                         const uint8_t *pRawData,
                                                         size_t rawDataSize,
                                                         size_t *pExportDataSize,
                                                         uint8_t *pExportData) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGroupGetExportDataExp(";
    ss << "hMetricGroup: " << hMetricGroup;
    if (pRawData == nullptr) {
        ss << ", pRawData: nullptr";
    } else {
        ss << ", pRawData: " << *pRawData;
    }
    ss << ", rawDataSize: " << rawDataSize;
    if (pExportDataSize == nullptr) {
        ss << ", pExportDataSize: nullptr";
    } else {
        ss << ", pExportDataSize: " << *pExportDataSize;
    }
    if (pExportData == nullptr) {
        ss << ", pExportData: nullptr";
    } else {
        ss << ", pExportData: " << *pExportData;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGroupGetExportDataExp(zet_metric_group_handle_t hMetricGroup,
                                                 const uint8_t *pRawData,
                                                 size_t rawDataSize,
                                                 size_t *pExportDataSize,
                                                 uint8_t *pExportData) {
    TRACE_EVENT_BEGIN("API", "zetMetricGroupGetExportDataExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupGetExportDataExp(hMetricGroup,
                                                           pRawData,
                                                           rawDataSize,
                                                           pExportDataSize,
                                                           pExportData) +
                         "..\n";
}
inline void trace_zetMetricGroupGetExportDataExp(ze_result_t ret,
                                                 zet_metric_group_handle_t hMetricGroup,
                                                 const uint8_t *pRawData,
                                                 size_t rawDataSize,
                                                 size_t *pExportDataSize,
                                                 uint8_t *pExportData) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupGetExportDataExp(hMetricGroup,
                                                           pRawData,
                                                           rawDataSize,
                                                           pExportDataSize,
                                                           pExportData) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricGroupCalculateMetricExportDataExp(
    ze_driver_handle_t hDriver,
    zet_metric_group_calculation_type_t type,
    size_t exportDataSize,
    const uint8_t *pExportData,
    zet_metric_calculate_exp_desc_t *pCalculateDescriptor,
    uint32_t *pSetCount,
    uint32_t *pTotalMetricValueCount,
    uint32_t *pMetricCounts,
    zet_typed_value_t *pMetricValues) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGroupCalculateMetricExportDataExp(";
    ss << "hDriver: " << hDriver;
    ss << ", type: " << type;
    ss << ", exportDataSize: " << exportDataSize;
    if (pExportData == nullptr) {
        ss << ", pExportData: nullptr";
    } else {
        ss << ", pExportData: " << *pExportData;
    }
    if (pCalculateDescriptor == nullptr) {
        ss << ", pCalculateDescriptor: nullptr";
    } else {
        ss << ", pCalculateDescriptor {";
        ss << "stype: " << pCalculateDescriptor->stype;
        ss << ", pNext: " << pCalculateDescriptor->pNext;
        ss << ", rawReportSkipCount: " << pCalculateDescriptor->rawReportSkipCount;
        ss << "}";
    }
    if (pSetCount == nullptr) {
        ss << ", pSetCount: nullptr";
    } else {
        ss << ", pSetCount: " << *pSetCount;
    }
    if (pTotalMetricValueCount == nullptr) {
        ss << ", pTotalMetricValueCount: nullptr";
    } else {
        ss << ", pTotalMetricValueCount: " << *pTotalMetricValueCount;
    }
    if (pMetricCounts == nullptr) {
        ss << ", pMetricCounts: nullptr";
    } else {
        ss << ", pMetricCounts: " << *pMetricCounts;
    }
    if (pMetricValues == nullptr) {
        ss << ", pMetricValues: nullptr";
    } else {
        ss << ", pMetricValues {";
        ss << "type: " << pMetricValues->type;
        ss << ", value {";
        ss << "ui32: " << pMetricValues->value.ui32;
        ss << ", ui64: " << pMetricValues->value.ui64;
        ss << ", fp32: " << pMetricValues->value.fp32;
        ss << ", fp64: " << pMetricValues->value.fp64;
        ss << ", b8: " << pMetricValues->value.b8;
        ss << "}";
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGroupCalculateMetricExportDataExp(
    ze_driver_handle_t hDriver,
    zet_metric_group_calculation_type_t type,
    size_t exportDataSize,
    const uint8_t *pExportData,
    zet_metric_calculate_exp_desc_t *pCalculateDescriptor,
    uint32_t *pSetCount,
    uint32_t *pTotalMetricValueCount,
    uint32_t *pMetricCounts,
    zet_typed_value_t *pMetricValues) {
    TRACE_EVENT_BEGIN("API", "zetMetricGroupCalculateMetricExportDataExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupCalculateMetricExportDataExp(hDriver,
                                                                       type,
                                                                       exportDataSize,
                                                                       pExportData,
                                                                       pCalculateDescriptor,
                                                                       pSetCount,
                                                                       pTotalMetricValueCount,
                                                                       pMetricCounts,
                                                                       pMetricValues) +
                         "..\n";
}
inline void trace_zetMetricGroupCalculateMetricExportDataExp(
    ze_result_t ret,
    ze_driver_handle_t hDriver,
    zet_metric_group_calculation_type_t type,
    size_t exportDataSize,
    const uint8_t *pExportData,
    zet_metric_calculate_exp_desc_t *pCalculateDescriptor,
    uint32_t *pSetCount,
    uint32_t *pTotalMetricValueCount,
    uint32_t *pMetricCounts,
    zet_typed_value_t *pMetricValues) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupCalculateMetricExportDataExp(hDriver,
                                                                       type,
                                                                       exportDataSize,
                                                                       pExportData,
                                                                       pCalculateDescriptor,
                                                                       pSetCount,
                                                                       pTotalMetricValueCount,
                                                                       pMetricCounts,
                                                                       pMetricValues) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetMetricProgrammableGetExp(zet_device_handle_t hDevice,
                                   uint32_t *pCount,
                                   zet_metric_programmable_exp_handle_t *phMetricProgrammables) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricProgrammableGetExp(";
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (phMetricProgrammables == nullptr) {
        ss << ", phMetricProgrammables: nullptr";
    } else {
        ss << ", phMetricProgrammables: " << *phMetricProgrammables;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zetMetricProgrammableGetExp(zet_device_handle_t hDevice,
                                  uint32_t *pCount,
                                  zet_metric_programmable_exp_handle_t *phMetricProgrammables) {
    TRACE_EVENT_BEGIN("API", "zetMetricProgrammableGetExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricProgrammableGetExp(hDevice, pCount, phMetricProgrammables) +
                         "..\n";
}
inline void
trace_zetMetricProgrammableGetExp(ze_result_t ret,
                                  zet_device_handle_t hDevice,
                                  uint32_t *pCount,
                                  zet_metric_programmable_exp_handle_t *phMetricProgrammables) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricProgrammableGetExp(hDevice, pCount, phMetricProgrammables) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricProgrammableGetPropertiesExp(
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    zet_metric_programmable_exp_properties_t *pProperties) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricProgrammableGetPropertiesExp(";
    ss << "hMetricProgrammable: " << hMetricProgrammable;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
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
           << pProperties->name[63] << pProperties->name[64] << pProperties->name[65]
           << pProperties->name[66] << pProperties->name[67] << pProperties->name[68]
           << pProperties->name[69] << pProperties->name[70] << pProperties->name[71]
           << pProperties->name[72] << pProperties->name[73] << pProperties->name[74]
           << pProperties->name[75] << pProperties->name[76] << pProperties->name[77]
           << pProperties->name[78] << pProperties->name[79] << pProperties->name[80]
           << pProperties->name[81] << pProperties->name[82] << pProperties->name[83]
           << pProperties->name[84] << pProperties->name[85] << pProperties->name[86]
           << pProperties->name[87] << pProperties->name[88] << pProperties->name[89]
           << pProperties->name[90] << pProperties->name[91] << pProperties->name[92]
           << pProperties->name[93] << pProperties->name[94] << pProperties->name[95]
           << pProperties->name[96] << pProperties->name[97] << pProperties->name[98]
           << pProperties->name[99] << pProperties->name[100] << pProperties->name[101]
           << pProperties->name[102] << pProperties->name[103] << pProperties->name[104]
           << pProperties->name[105] << pProperties->name[106] << pProperties->name[107]
           << pProperties->name[108] << pProperties->name[109] << pProperties->name[110]
           << pProperties->name[111] << pProperties->name[112] << pProperties->name[113]
           << pProperties->name[114] << pProperties->name[115] << pProperties->name[116]
           << pProperties->name[117] << pProperties->name[118] << pProperties->name[119]
           << pProperties->name[120] << pProperties->name[121] << pProperties->name[122]
           << pProperties->name[123] << pProperties->name[124] << pProperties->name[125]
           << pProperties->name[126] << pProperties->name[127];
        ss << ", description: " << pProperties->description[0] << pProperties->description[1]
           << pProperties->description[2] << pProperties->description[3]
           << pProperties->description[4] << pProperties->description[5]
           << pProperties->description[6] << pProperties->description[7]
           << pProperties->description[8] << pProperties->description[9]
           << pProperties->description[10] << pProperties->description[11]
           << pProperties->description[12] << pProperties->description[13]
           << pProperties->description[14] << pProperties->description[15]
           << pProperties->description[16] << pProperties->description[17]
           << pProperties->description[18] << pProperties->description[19]
           << pProperties->description[20] << pProperties->description[21]
           << pProperties->description[22] << pProperties->description[23]
           << pProperties->description[24] << pProperties->description[25]
           << pProperties->description[26] << pProperties->description[27]
           << pProperties->description[28] << pProperties->description[29]
           << pProperties->description[30] << pProperties->description[31]
           << pProperties->description[32] << pProperties->description[33]
           << pProperties->description[34] << pProperties->description[35]
           << pProperties->description[36] << pProperties->description[37]
           << pProperties->description[38] << pProperties->description[39]
           << pProperties->description[40] << pProperties->description[41]
           << pProperties->description[42] << pProperties->description[43]
           << pProperties->description[44] << pProperties->description[45]
           << pProperties->description[46] << pProperties->description[47]
           << pProperties->description[48] << pProperties->description[49]
           << pProperties->description[50] << pProperties->description[51]
           << pProperties->description[52] << pProperties->description[53]
           << pProperties->description[54] << pProperties->description[55]
           << pProperties->description[56] << pProperties->description[57]
           << pProperties->description[58] << pProperties->description[59]
           << pProperties->description[60] << pProperties->description[61]
           << pProperties->description[62] << pProperties->description[63]
           << pProperties->description[64] << pProperties->description[65]
           << pProperties->description[66] << pProperties->description[67]
           << pProperties->description[68] << pProperties->description[69]
           << pProperties->description[70] << pProperties->description[71]
           << pProperties->description[72] << pProperties->description[73]
           << pProperties->description[74] << pProperties->description[75]
           << pProperties->description[76] << pProperties->description[77]
           << pProperties->description[78] << pProperties->description[79]
           << pProperties->description[80] << pProperties->description[81]
           << pProperties->description[82] << pProperties->description[83]
           << pProperties->description[84] << pProperties->description[85]
           << pProperties->description[86] << pProperties->description[87]
           << pProperties->description[88] << pProperties->description[89]
           << pProperties->description[90] << pProperties->description[91]
           << pProperties->description[92] << pProperties->description[93]
           << pProperties->description[94] << pProperties->description[95]
           << pProperties->description[96] << pProperties->description[97]
           << pProperties->description[98] << pProperties->description[99]
           << pProperties->description[100] << pProperties->description[101]
           << pProperties->description[102] << pProperties->description[103]
           << pProperties->description[104] << pProperties->description[105]
           << pProperties->description[106] << pProperties->description[107]
           << pProperties->description[108] << pProperties->description[109]
           << pProperties->description[110] << pProperties->description[111]
           << pProperties->description[112] << pProperties->description[113]
           << pProperties->description[114] << pProperties->description[115]
           << pProperties->description[116] << pProperties->description[117]
           << pProperties->description[118] << pProperties->description[119]
           << pProperties->description[120] << pProperties->description[121]
           << pProperties->description[122] << pProperties->description[123]
           << pProperties->description[124] << pProperties->description[125]
           << pProperties->description[126] << pProperties->description[127];
        ss << ", component: " << pProperties->component[0] << pProperties->component[1]
           << pProperties->component[2] << pProperties->component[3] << pProperties->component[4]
           << pProperties->component[5] << pProperties->component[6] << pProperties->component[7]
           << pProperties->component[8] << pProperties->component[9] << pProperties->component[10]
           << pProperties->component[11] << pProperties->component[12] << pProperties->component[13]
           << pProperties->component[14] << pProperties->component[15] << pProperties->component[16]
           << pProperties->component[17] << pProperties->component[18] << pProperties->component[19]
           << pProperties->component[20] << pProperties->component[21] << pProperties->component[22]
           << pProperties->component[23] << pProperties->component[24] << pProperties->component[25]
           << pProperties->component[26] << pProperties->component[27] << pProperties->component[28]
           << pProperties->component[29] << pProperties->component[30] << pProperties->component[31]
           << pProperties->component[32] << pProperties->component[33] << pProperties->component[34]
           << pProperties->component[35] << pProperties->component[36] << pProperties->component[37]
           << pProperties->component[38] << pProperties->component[39] << pProperties->component[40]
           << pProperties->component[41] << pProperties->component[42] << pProperties->component[43]
           << pProperties->component[44] << pProperties->component[45] << pProperties->component[46]
           << pProperties->component[47] << pProperties->component[48] << pProperties->component[49]
           << pProperties->component[50] << pProperties->component[51] << pProperties->component[52]
           << pProperties->component[53] << pProperties->component[54] << pProperties->component[55]
           << pProperties->component[56] << pProperties->component[57] << pProperties->component[58]
           << pProperties->component[59] << pProperties->component[60] << pProperties->component[61]
           << pProperties->component[62] << pProperties->component[63] << pProperties->component[64]
           << pProperties->component[65] << pProperties->component[66] << pProperties->component[67]
           << pProperties->component[68] << pProperties->component[69] << pProperties->component[70]
           << pProperties->component[71] << pProperties->component[72] << pProperties->component[73]
           << pProperties->component[74] << pProperties->component[75] << pProperties->component[76]
           << pProperties->component[77] << pProperties->component[78] << pProperties->component[79]
           << pProperties->component[80] << pProperties->component[81] << pProperties->component[82]
           << pProperties->component[83] << pProperties->component[84] << pProperties->component[85]
           << pProperties->component[86] << pProperties->component[87] << pProperties->component[88]
           << pProperties->component[89] << pProperties->component[90] << pProperties->component[91]
           << pProperties->component[92] << pProperties->component[93] << pProperties->component[94]
           << pProperties->component[95] << pProperties->component[96] << pProperties->component[97]
           << pProperties->component[98] << pProperties->component[99]
           << pProperties->component[100] << pProperties->component[101]
           << pProperties->component[102] << pProperties->component[103]
           << pProperties->component[104] << pProperties->component[105]
           << pProperties->component[106] << pProperties->component[107]
           << pProperties->component[108] << pProperties->component[109]
           << pProperties->component[110] << pProperties->component[111]
           << pProperties->component[112] << pProperties->component[113]
           << pProperties->component[114] << pProperties->component[115]
           << pProperties->component[116] << pProperties->component[117]
           << pProperties->component[118] << pProperties->component[119]
           << pProperties->component[120] << pProperties->component[121]
           << pProperties->component[122] << pProperties->component[123]
           << pProperties->component[124] << pProperties->component[125]
           << pProperties->component[126] << pProperties->component[127];
        ss << ", tierNumber: " << pProperties->tierNumber;
        ss << ", domain: " << pProperties->domain;
        ss << ", parameterCount: " << pProperties->parameterCount;
        ss << ", samplingType: " << pProperties->samplingType;
        ss << ", sourceId: " << pProperties->sourceId;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricProgrammableGetPropertiesExp(
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    zet_metric_programmable_exp_properties_t *pProperties) {
    TRACE_EVENT_BEGIN("API", "zetMetricProgrammableGetPropertiesExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricProgrammableGetPropertiesExp(hMetricProgrammable,
                                                                  pProperties) +
                         "..\n";
}
inline void trace_zetMetricProgrammableGetPropertiesExp(
    ze_result_t ret,
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    zet_metric_programmable_exp_properties_t *pProperties) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricProgrammableGetPropertiesExp(hMetricProgrammable,
                                                                  pProperties) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricProgrammableGetParamInfoExp(
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    uint32_t *pParameterCount,
    zet_metric_programmable_param_info_exp_t *pParameterInfo) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricProgrammableGetParamInfoExp(";
    ss << "hMetricProgrammable: " << hMetricProgrammable;
    if (pParameterCount == nullptr) {
        ss << ", pParameterCount: nullptr";
    } else {
        ss << ", pParameterCount: " << *pParameterCount;
    }
    if (pParameterInfo == nullptr) {
        ss << ", pParameterInfo: nullptr";
    } else {
        ss << ", pParameterInfo {";
        ss << "stype: " << pParameterInfo->stype;
        ss << ", pNext: " << pParameterInfo->pNext;
        ss << ", type: " << pParameterInfo->type;
        ss << ", name: " << pParameterInfo->name[0] << pParameterInfo->name[1]
           << pParameterInfo->name[2] << pParameterInfo->name[3] << pParameterInfo->name[4]
           << pParameterInfo->name[5] << pParameterInfo->name[6] << pParameterInfo->name[7]
           << pParameterInfo->name[8] << pParameterInfo->name[9] << pParameterInfo->name[10]
           << pParameterInfo->name[11] << pParameterInfo->name[12] << pParameterInfo->name[13]
           << pParameterInfo->name[14] << pParameterInfo->name[15] << pParameterInfo->name[16]
           << pParameterInfo->name[17] << pParameterInfo->name[18] << pParameterInfo->name[19]
           << pParameterInfo->name[20] << pParameterInfo->name[21] << pParameterInfo->name[22]
           << pParameterInfo->name[23] << pParameterInfo->name[24] << pParameterInfo->name[25]
           << pParameterInfo->name[26] << pParameterInfo->name[27] << pParameterInfo->name[28]
           << pParameterInfo->name[29] << pParameterInfo->name[30] << pParameterInfo->name[31]
           << pParameterInfo->name[32] << pParameterInfo->name[33] << pParameterInfo->name[34]
           << pParameterInfo->name[35] << pParameterInfo->name[36] << pParameterInfo->name[37]
           << pParameterInfo->name[38] << pParameterInfo->name[39] << pParameterInfo->name[40]
           << pParameterInfo->name[41] << pParameterInfo->name[42] << pParameterInfo->name[43]
           << pParameterInfo->name[44] << pParameterInfo->name[45] << pParameterInfo->name[46]
           << pParameterInfo->name[47] << pParameterInfo->name[48] << pParameterInfo->name[49]
           << pParameterInfo->name[50] << pParameterInfo->name[51] << pParameterInfo->name[52]
           << pParameterInfo->name[53] << pParameterInfo->name[54] << pParameterInfo->name[55]
           << pParameterInfo->name[56] << pParameterInfo->name[57] << pParameterInfo->name[58]
           << pParameterInfo->name[59] << pParameterInfo->name[60] << pParameterInfo->name[61]
           << pParameterInfo->name[62] << pParameterInfo->name[63] << pParameterInfo->name[64]
           << pParameterInfo->name[65] << pParameterInfo->name[66] << pParameterInfo->name[67]
           << pParameterInfo->name[68] << pParameterInfo->name[69] << pParameterInfo->name[70]
           << pParameterInfo->name[71] << pParameterInfo->name[72] << pParameterInfo->name[73]
           << pParameterInfo->name[74] << pParameterInfo->name[75] << pParameterInfo->name[76]
           << pParameterInfo->name[77] << pParameterInfo->name[78] << pParameterInfo->name[79]
           << pParameterInfo->name[80] << pParameterInfo->name[81] << pParameterInfo->name[82]
           << pParameterInfo->name[83] << pParameterInfo->name[84] << pParameterInfo->name[85]
           << pParameterInfo->name[86] << pParameterInfo->name[87] << pParameterInfo->name[88]
           << pParameterInfo->name[89] << pParameterInfo->name[90] << pParameterInfo->name[91]
           << pParameterInfo->name[92] << pParameterInfo->name[93] << pParameterInfo->name[94]
           << pParameterInfo->name[95] << pParameterInfo->name[96] << pParameterInfo->name[97]
           << pParameterInfo->name[98] << pParameterInfo->name[99] << pParameterInfo->name[100]
           << pParameterInfo->name[101] << pParameterInfo->name[102] << pParameterInfo->name[103]
           << pParameterInfo->name[104] << pParameterInfo->name[105] << pParameterInfo->name[106]
           << pParameterInfo->name[107] << pParameterInfo->name[108] << pParameterInfo->name[109]
           << pParameterInfo->name[110] << pParameterInfo->name[111] << pParameterInfo->name[112]
           << pParameterInfo->name[113] << pParameterInfo->name[114] << pParameterInfo->name[115]
           << pParameterInfo->name[116] << pParameterInfo->name[117] << pParameterInfo->name[118]
           << pParameterInfo->name[119] << pParameterInfo->name[120] << pParameterInfo->name[121]
           << pParameterInfo->name[122] << pParameterInfo->name[123] << pParameterInfo->name[124]
           << pParameterInfo->name[125] << pParameterInfo->name[126] << pParameterInfo->name[127];
        ss << ", valueInfoType: " << pParameterInfo->valueInfoType;
        ss << ", defaultValue {";
        ss << "ui32: " << pParameterInfo->defaultValue.ui32;
        ss << ", ui64: " << pParameterInfo->defaultValue.ui64;
        ss << ", fp32: " << pParameterInfo->defaultValue.fp32;
        ss << ", fp64: " << pParameterInfo->defaultValue.fp64;
        ss << ", b8: " << pParameterInfo->defaultValue.b8;
        ss << "}";
        ss << ", valueInfoCount: " << pParameterInfo->valueInfoCount;
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricProgrammableGetParamInfoExp(
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    uint32_t *pParameterCount,
    zet_metric_programmable_param_info_exp_t *pParameterInfo) {
    TRACE_EVENT_BEGIN("API", "zetMetricProgrammableGetParamInfoExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricProgrammableGetParamInfoExp(hMetricProgrammable,
                                                                 pParameterCount,
                                                                 pParameterInfo) +
                         "..\n";
}
inline void trace_zetMetricProgrammableGetParamInfoExp(
    ze_result_t ret,
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    uint32_t *pParameterCount,
    zet_metric_programmable_param_info_exp_t *pParameterInfo) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricProgrammableGetParamInfoExp(hMetricProgrammable,
                                                                 pParameterCount,
                                                                 pParameterInfo) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricProgrammableGetParamValueInfoExp(
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    uint32_t parameterOrdinal,
    uint32_t *pValueInfoCount,
    zet_metric_programmable_param_value_info_exp_t *pValueInfo) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricProgrammableGetParamValueInfoExp(";
    ss << "hMetricProgrammable: " << hMetricProgrammable;
    ss << ", parameterOrdinal: " << parameterOrdinal;
    if (pValueInfoCount == nullptr) {
        ss << ", pValueInfoCount: nullptr";
    } else {
        ss << ", pValueInfoCount: " << *pValueInfoCount;
    }
    if (pValueInfo == nullptr) {
        ss << ", pValueInfo: nullptr";
    } else {
        ss << ", pValueInfo {";
        ss << "stype: " << pValueInfo->stype;
        ss << ", pNext: " << pValueInfo->pNext;
        ss << ", valueInfo {";
        ss << "ui32: " << pValueInfo->valueInfo.ui32;
        ss << ", ui64: " << pValueInfo->valueInfo.ui64;
        ss << ", fp32: " << pValueInfo->valueInfo.fp32;
        ss << ", fp64: " << pValueInfo->valueInfo.fp64;
        ss << ", b8: " << pValueInfo->valueInfo.b8;
        ss << ", ui8: " << pValueInfo->valueInfo.ui8;
        ss << ", ui16: " << pValueInfo->valueInfo.ui16;
        ss << ", ui64Range {";
        ss << "ui64Min: " << pValueInfo->valueInfo.ui64Range.ui64Min;
        ss << ", ui64Max: " << pValueInfo->valueInfo.ui64Range.ui64Max;
        ss << "}";
        ss << ", fp64Range {";
        ss << "fp64Min: " << pValueInfo->valueInfo.fp64Range.fp64Min;
        ss << ", fp64Max: " << pValueInfo->valueInfo.fp64Range.fp64Max;
        ss << "}";
        ss << "}";
        ss << ", description: " << pValueInfo->description[0] << pValueInfo->description[1]
           << pValueInfo->description[2] << pValueInfo->description[3] << pValueInfo->description[4]
           << pValueInfo->description[5] << pValueInfo->description[6] << pValueInfo->description[7]
           << pValueInfo->description[8] << pValueInfo->description[9]
           << pValueInfo->description[10] << pValueInfo->description[11]
           << pValueInfo->description[12] << pValueInfo->description[13]
           << pValueInfo->description[14] << pValueInfo->description[15]
           << pValueInfo->description[16] << pValueInfo->description[17]
           << pValueInfo->description[18] << pValueInfo->description[19]
           << pValueInfo->description[20] << pValueInfo->description[21]
           << pValueInfo->description[22] << pValueInfo->description[23]
           << pValueInfo->description[24] << pValueInfo->description[25]
           << pValueInfo->description[26] << pValueInfo->description[27]
           << pValueInfo->description[28] << pValueInfo->description[29]
           << pValueInfo->description[30] << pValueInfo->description[31]
           << pValueInfo->description[32] << pValueInfo->description[33]
           << pValueInfo->description[34] << pValueInfo->description[35]
           << pValueInfo->description[36] << pValueInfo->description[37]
           << pValueInfo->description[38] << pValueInfo->description[39]
           << pValueInfo->description[40] << pValueInfo->description[41]
           << pValueInfo->description[42] << pValueInfo->description[43]
           << pValueInfo->description[44] << pValueInfo->description[45]
           << pValueInfo->description[46] << pValueInfo->description[47]
           << pValueInfo->description[48] << pValueInfo->description[49]
           << pValueInfo->description[50] << pValueInfo->description[51]
           << pValueInfo->description[52] << pValueInfo->description[53]
           << pValueInfo->description[54] << pValueInfo->description[55]
           << pValueInfo->description[56] << pValueInfo->description[57]
           << pValueInfo->description[58] << pValueInfo->description[59]
           << pValueInfo->description[60] << pValueInfo->description[61]
           << pValueInfo->description[62] << pValueInfo->description[63]
           << pValueInfo->description[64] << pValueInfo->description[65]
           << pValueInfo->description[66] << pValueInfo->description[67]
           << pValueInfo->description[68] << pValueInfo->description[69]
           << pValueInfo->description[70] << pValueInfo->description[71]
           << pValueInfo->description[72] << pValueInfo->description[73]
           << pValueInfo->description[74] << pValueInfo->description[75]
           << pValueInfo->description[76] << pValueInfo->description[77]
           << pValueInfo->description[78] << pValueInfo->description[79]
           << pValueInfo->description[80] << pValueInfo->description[81]
           << pValueInfo->description[82] << pValueInfo->description[83]
           << pValueInfo->description[84] << pValueInfo->description[85]
           << pValueInfo->description[86] << pValueInfo->description[87]
           << pValueInfo->description[88] << pValueInfo->description[89]
           << pValueInfo->description[90] << pValueInfo->description[91]
           << pValueInfo->description[92] << pValueInfo->description[93]
           << pValueInfo->description[94] << pValueInfo->description[95]
           << pValueInfo->description[96] << pValueInfo->description[97]
           << pValueInfo->description[98] << pValueInfo->description[99]
           << pValueInfo->description[100] << pValueInfo->description[101]
           << pValueInfo->description[102] << pValueInfo->description[103]
           << pValueInfo->description[104] << pValueInfo->description[105]
           << pValueInfo->description[106] << pValueInfo->description[107]
           << pValueInfo->description[108] << pValueInfo->description[109]
           << pValueInfo->description[110] << pValueInfo->description[111]
           << pValueInfo->description[112] << pValueInfo->description[113]
           << pValueInfo->description[114] << pValueInfo->description[115]
           << pValueInfo->description[116] << pValueInfo->description[117]
           << pValueInfo->description[118] << pValueInfo->description[119]
           << pValueInfo->description[120] << pValueInfo->description[121]
           << pValueInfo->description[122] << pValueInfo->description[123]
           << pValueInfo->description[124] << pValueInfo->description[125]
           << pValueInfo->description[126] << pValueInfo->description[127];
        ss << "}";
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricProgrammableGetParamValueInfoExp(
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    uint32_t parameterOrdinal,
    uint32_t *pValueInfoCount,
    zet_metric_programmable_param_value_info_exp_t *pValueInfo) {
    TRACE_EVENT_BEGIN("API", "zetMetricProgrammableGetParamValueInfoExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricProgrammableGetParamValueInfoExp(hMetricProgrammable,
                                                                      parameterOrdinal,
                                                                      pValueInfoCount,
                                                                      pValueInfo) +
                         "..\n";
}
inline void trace_zetMetricProgrammableGetParamValueInfoExp(
    ze_result_t ret,
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    uint32_t parameterOrdinal,
    uint32_t *pValueInfoCount,
    zet_metric_programmable_param_value_info_exp_t *pValueInfo) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricProgrammableGetParamValueInfoExp(hMetricProgrammable,
                                                                      parameterOrdinal,
                                                                      pValueInfoCount,
                                                                      pValueInfo) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricCreateFromProgrammableExp2(
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    uint32_t parameterCount,
    zet_metric_programmable_param_value_exp_t *pParameterValues,
    const char *pName,
    const char *pDescription,
    uint32_t *pMetricHandleCount,
    zet_metric_handle_t *phMetricHandles) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricCreateFromProgrammableExp2(";
    ss << "hMetricProgrammable: " << hMetricProgrammable;
    ss << ", parameterCount: " << parameterCount;
    if (pParameterValues == nullptr) {
        ss << ", pParameterValues: nullptr";
    } else {
        ss << ", pParameterValues {";
        ss << ", value {";
        ss << "ui32: " << pParameterValues->value.ui32;
        ss << ", ui64: " << pParameterValues->value.ui64;
        ss << ", fp32: " << pParameterValues->value.fp32;
        ss << ", fp64: " << pParameterValues->value.fp64;
        ss << ", b8: " << pParameterValues->value.b8;
        ss << "}";
        ss << "}";
    }
    if (pName == nullptr) {
        ss << ", pName: nullptr";
    } else {
        ss << ", pName: " << pName;
    }
    if (pDescription == nullptr) {
        ss << ", pDescription: nullptr";
    } else {
        ss << ", pDescription: " << pDescription;
    }
    if (pMetricHandleCount == nullptr) {
        ss << ", pMetricHandleCount: nullptr";
    } else {
        ss << ", pMetricHandleCount: " << *pMetricHandleCount;
    }
    if (phMetricHandles == nullptr) {
        ss << ", phMetricHandles: nullptr";
    } else {
        ss << ", phMetricHandles: " << *phMetricHandles;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricCreateFromProgrammableExp2(
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    uint32_t parameterCount,
    zet_metric_programmable_param_value_exp_t *pParameterValues,
    const char *pName,
    const char *pDescription,
    uint32_t *pMetricHandleCount,
    zet_metric_handle_t *phMetricHandles) {
    TRACE_EVENT_BEGIN("API", "zetMetricCreateFromProgrammableExp2");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricCreateFromProgrammableExp2(hMetricProgrammable,
                                                                parameterCount,
                                                                pParameterValues,
                                                                pName,
                                                                pDescription,
                                                                pMetricHandleCount,
                                                                phMetricHandles) +
                         "..\n";
}
inline void trace_zetMetricCreateFromProgrammableExp2(
    ze_result_t ret,
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    uint32_t parameterCount,
    zet_metric_programmable_param_value_exp_t *pParameterValues,
    const char *pName,
    const char *pDescription,
    uint32_t *pMetricHandleCount,
    zet_metric_handle_t *phMetricHandles) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricCreateFromProgrammableExp2(hMetricProgrammable,
                                                                parameterCount,
                                                                pParameterValues,
                                                                pName,
                                                                pDescription,
                                                                pMetricHandleCount,
                                                                phMetricHandles) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricCreateFromProgrammableExp(
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    zet_metric_programmable_param_value_exp_t *pParameterValues,
    uint32_t parameterCount,
    const char *pName,
    const char *pDescription,
    uint32_t *pMetricHandleCount,
    zet_metric_handle_t *phMetricHandles) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricCreateFromProgrammableExp(";
    ss << "hMetricProgrammable: " << hMetricProgrammable;
    if (pParameterValues == nullptr) {
        ss << ", pParameterValues: nullptr";
    } else {
        ss << ", pParameterValues {";
        ss << ", value {";
        ss << "ui32: " << pParameterValues->value.ui32;
        ss << ", ui64: " << pParameterValues->value.ui64;
        ss << ", fp32: " << pParameterValues->value.fp32;
        ss << ", fp64: " << pParameterValues->value.fp64;
        ss << ", b8: " << pParameterValues->value.b8;
        ss << "}";
        ss << "}";
    }
    ss << ", parameterCount: " << parameterCount;
    if (pName == nullptr) {
        ss << ", pName: nullptr";
    } else {
        ss << ", pName: " << pName;
    }
    if (pDescription == nullptr) {
        ss << ", pDescription: nullptr";
    } else {
        ss << ", pDescription: " << pDescription;
    }
    if (pMetricHandleCount == nullptr) {
        ss << ", pMetricHandleCount: nullptr";
    } else {
        ss << ", pMetricHandleCount: " << *pMetricHandleCount;
    }
    if (phMetricHandles == nullptr) {
        ss << ", phMetricHandles: nullptr";
    } else {
        ss << ", phMetricHandles: " << *phMetricHandles;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricCreateFromProgrammableExp(
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    zet_metric_programmable_param_value_exp_t *pParameterValues,
    uint32_t parameterCount,
    const char *pName,
    const char *pDescription,
    uint32_t *pMetricHandleCount,
    zet_metric_handle_t *phMetricHandles) {
    TRACE_EVENT_BEGIN("API", "zetMetricCreateFromProgrammableExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricCreateFromProgrammableExp(hMetricProgrammable,
                                                               pParameterValues,
                                                               parameterCount,
                                                               pName,
                                                               pDescription,
                                                               pMetricHandleCount,
                                                               phMetricHandles) +
                         "..\n";
}
inline void trace_zetMetricCreateFromProgrammableExp(
    ze_result_t ret,
    zet_metric_programmable_exp_handle_t hMetricProgrammable,
    zet_metric_programmable_param_value_exp_t *pParameterValues,
    uint32_t parameterCount,
    const char *pName,
    const char *pDescription,
    uint32_t *pMetricHandleCount,
    zet_metric_handle_t *phMetricHandles) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricCreateFromProgrammableExp(hMetricProgrammable,
                                                               pParameterValues,
                                                               parameterCount,
                                                               pName,
                                                               pDescription,
                                                               pMetricHandleCount,
                                                               phMetricHandles) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetDeviceCreateMetricGroupsFromMetricsExp(zet_device_handle_t hDevice,
                                                 uint32_t metricCount,
                                                 zet_metric_handle_t *phMetrics,
                                                 const char *pMetricGroupNamePrefix,
                                                 const char *pDescription,
                                                 uint32_t *pMetricGroupCount,
                                                 zet_metric_group_handle_t *phMetricGroup) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetDeviceCreateMetricGroupsFromMetricsExp(";
    ss << "hDevice: " << hDevice;
    ss << ", metricCount: " << metricCount;
    if (phMetrics == nullptr) {
        ss << ", phMetrics: nullptr";
    } else {
        ss << ", phMetrics: " << *phMetrics;
    }
    if (pMetricGroupNamePrefix == nullptr) {
        ss << ", pMetricGroupNamePrefix: nullptr";
    } else {
        ss << ", pMetricGroupNamePrefix: " << pMetricGroupNamePrefix;
    }
    if (pDescription == nullptr) {
        ss << ", pDescription: nullptr";
    } else {
        ss << ", pDescription: " << pDescription;
    }
    if (pMetricGroupCount == nullptr) {
        ss << ", pMetricGroupCount: nullptr";
    } else {
        ss << ", pMetricGroupCount: " << *pMetricGroupCount;
    }
    if (phMetricGroup == nullptr) {
        ss << ", phMetricGroup: nullptr";
    } else {
        ss << ", phMetricGroup: " << *phMetricGroup;
    }
    ss << ")";
    return ss.str();
}
inline void
trace_zetDeviceCreateMetricGroupsFromMetricsExp(zet_device_handle_t hDevice,
                                                uint32_t metricCount,
                                                zet_metric_handle_t *phMetrics,
                                                const char *pMetricGroupNamePrefix,
                                                const char *pDescription,
                                                uint32_t *pMetricGroupCount,
                                                zet_metric_group_handle_t *phMetricGroup) {
    TRACE_EVENT_BEGIN("API", "zetDeviceCreateMetricGroupsFromMetricsExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDeviceCreateMetricGroupsFromMetricsExp(hDevice,
                                                                      metricCount,
                                                                      phMetrics,
                                                                      pMetricGroupNamePrefix,
                                                                      pDescription,
                                                                      pMetricGroupCount,
                                                                      phMetricGroup) +
                         "..\n";
}
inline void
trace_zetDeviceCreateMetricGroupsFromMetricsExp(ze_result_t ret,
                                                zet_device_handle_t hDevice,
                                                uint32_t metricCount,
                                                zet_metric_handle_t *phMetrics,
                                                const char *pMetricGroupNamePrefix,
                                                const char *pDescription,
                                                uint32_t *pMetricGroupCount,
                                                zet_metric_group_handle_t *phMetricGroup) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDeviceCreateMetricGroupsFromMetricsExp(hDevice,
                                                                      metricCount,
                                                                      phMetrics,
                                                                      pMetricGroupNamePrefix,
                                                                      pDescription,
                                                                      pMetricGroupCount,
                                                                      phMetricGroup) +
                         trace_ze_result_t(ret);
}

inline std::string
_trace_zetMetricGroupCreateExp(zet_device_handle_t hDevice,
                               const char *pName,
                               const char *pDescription,
                               zet_metric_group_sampling_type_flags_t samplingType,
                               zet_metric_group_handle_t *phMetricGroup) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGroupCreateExp(";
    ss << "hDevice: " << hDevice;
    if (pName == nullptr) {
        ss << ", pName: nullptr";
    } else {
        ss << ", pName: " << pName;
    }
    if (pDescription == nullptr) {
        ss << ", pDescription: nullptr";
    } else {
        ss << ", pDescription: " << pDescription;
    }
    ss << ", samplingType: " << samplingType;
    if (phMetricGroup == nullptr) {
        ss << ", phMetricGroup: nullptr";
    } else {
        ss << ", phMetricGroup: " << *phMetricGroup;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGroupCreateExp(zet_device_handle_t hDevice,
                                          const char *pName,
                                          const char *pDescription,
                                          zet_metric_group_sampling_type_flags_t samplingType,
                                          zet_metric_group_handle_t *phMetricGroup) {
    TRACE_EVENT_BEGIN("API", "zetMetricGroupCreateExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupCreateExp(hDevice,
                                                    pName,
                                                    pDescription,
                                                    samplingType,
                                                    phMetricGroup) +
                         "..\n";
}
inline void trace_zetMetricGroupCreateExp(ze_result_t ret,
                                          zet_device_handle_t hDevice,
                                          const char *pName,
                                          const char *pDescription,
                                          zet_metric_group_sampling_type_flags_t samplingType,
                                          zet_metric_group_handle_t *phMetricGroup) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupCreateExp(hDevice,
                                                    pName,
                                                    pDescription,
                                                    samplingType,
                                                    phMetricGroup) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricGroupAddMetricExp(zet_metric_group_handle_t hMetricGroup,
                                                     zet_metric_handle_t hMetric,
                                                     size_t *pErrorStringSize,
                                                     char *pErrorString) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGroupAddMetricExp(";
    ss << "hMetricGroup: " << hMetricGroup;
    ss << ", hMetric: " << hMetric;
    if (pErrorStringSize == nullptr) {
        ss << ", pErrorStringSize: nullptr";
    } else {
        ss << ", pErrorStringSize: " << *pErrorStringSize;
    }
    if (pErrorString == nullptr) {
        ss << ", pErrorString: nullptr";
    } else {
        ss << ", pErrorString: " << pErrorString;
    }
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGroupAddMetricExp(zet_metric_group_handle_t hMetricGroup,
                                             zet_metric_handle_t hMetric,
                                             size_t *pErrorStringSize,
                                             char *pErrorString) {
    TRACE_EVENT_BEGIN("API", "zetMetricGroupAddMetricExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupAddMetricExp(hMetricGroup,
                                                       hMetric,
                                                       pErrorStringSize,
                                                       pErrorString) +
                         "..\n";
}
inline void trace_zetMetricGroupAddMetricExp(ze_result_t ret,
                                             zet_metric_group_handle_t hMetricGroup,
                                             zet_metric_handle_t hMetric,
                                             size_t *pErrorStringSize,
                                             char *pErrorString) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupAddMetricExp(hMetricGroup,
                                                       hMetric,
                                                       pErrorStringSize,
                                                       pErrorString) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricGroupRemoveMetricExp(zet_metric_group_handle_t hMetricGroup,
                                                        zet_metric_handle_t hMetric) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGroupRemoveMetricExp(";
    ss << "hMetricGroup: " << hMetricGroup;
    ss << ", hMetric: " << hMetric;
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGroupRemoveMetricExp(zet_metric_group_handle_t hMetricGroup,
                                                zet_metric_handle_t hMetric) {
    TRACE_EVENT_BEGIN("API", "zetMetricGroupRemoveMetricExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupRemoveMetricExp(hMetricGroup, hMetric) + "..\n";
}
inline void trace_zetMetricGroupRemoveMetricExp(ze_result_t ret,
                                                zet_metric_group_handle_t hMetricGroup,
                                                zet_metric_handle_t hMetric) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupRemoveMetricExp(hMetricGroup, hMetric) +
                         trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricGroupCloseExp(zet_metric_group_handle_t hMetricGroup) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGroupCloseExp(";
    ss << "hMetricGroup: " << hMetricGroup;
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGroupCloseExp(zet_metric_group_handle_t hMetricGroup) {
    TRACE_EVENT_BEGIN("API", "zetMetricGroupCloseExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupCloseExp(hMetricGroup) + "..\n";
}
inline void trace_zetMetricGroupCloseExp(ze_result_t ret, zet_metric_group_handle_t hMetricGroup) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupCloseExp(hMetricGroup) + trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricGroupDestroyExp(zet_metric_group_handle_t hMetricGroup) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricGroupDestroyExp(";
    ss << "hMetricGroup: " << hMetricGroup;
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricGroupDestroyExp(zet_metric_group_handle_t hMetricGroup) {
    TRACE_EVENT_BEGIN("API", "zetMetricGroupDestroyExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupDestroyExp(hMetricGroup) + "..\n";
}
inline void trace_zetMetricGroupDestroyExp(ze_result_t ret,
                                           zet_metric_group_handle_t hMetricGroup) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricGroupDestroyExp(hMetricGroup) + trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricDestroyExp(zet_metric_handle_t hMetric) {
    std::stringstream ss;
    ss << std::hex << std::showbase;
    ss << "NPU_LOG: [API] zetMetricDestroyExp(";
    ss << "hMetric: " << hMetric;
    ss << ")";
    return ss.str();
}
inline void trace_zetMetricDestroyExp(zet_metric_handle_t hMetric) {
    TRACE_EVENT_BEGIN("API", "zetMetricDestroyExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricDestroyExp(hMetric) + "..\n";
}
inline void trace_zetMetricDestroyExp(ze_result_t ret, zet_metric_handle_t hMetric) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetMetricDestroyExp(hMetric) + trace_ze_result_t(ret);
}
