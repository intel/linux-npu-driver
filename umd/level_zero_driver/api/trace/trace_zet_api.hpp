/*
 * Copyright (C) 2024-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// Generated header based on zet_api.h@version v1.11-r1.11.4

#pragma once

#include "trace_ze_api.hpp"

#include <iostream>
#include <sstream>
#include <zet_api.h>

inline std::string _trace_zetModuleGetDebugInfo(zet_module_handle_t hModule,
                                                zet_module_debug_info_format_t format,
                                                size_t *pSize,
                                                uint8_t *pDebugInfo) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetModuleGetDebugInfo(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDeviceGetDebugProperties(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDebugAttach(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDebugDetach(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDebugReadEvent(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDebugAcknowledgeEvent(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDebugInterrupt(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDebugResume(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDebugReadMemory(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDebugWriteMemory(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDebugGetRegisterSetProperties(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    if (pCount == nullptr) {
        ss << ", pCount: nullptr";
    } else {
        ss << ", pCount: " << *pCount;
    }
    if (pRegisterSetProperties == nullptr) {
        ss << ", pRegisterSetProperties: nullptr";
    } else {
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pRegisterSetProperties[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pRegisterSetProperties[0].stype;
            ss << ", pNext: " << pRegisterSetProperties[0].pNext;
            ss << ", type: " << pRegisterSetProperties[0].type;
            ss << ", version: " << pRegisterSetProperties[0].version;
            ss << ", generalFlags: " << pRegisterSetProperties[0].generalFlags;
            ss << ", deviceFlags: " << pRegisterSetProperties[0].deviceFlags;
            ss << ", count: " << pRegisterSetProperties[0].count;
            ss << ", bitSize: " << pRegisterSetProperties[0].bitSize;
            ss << ", byteSize: " << pRegisterSetProperties[0].byteSize;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDebugGetThreadRegisterSetProperties(";
    ss << std::hex << std::showbase;
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
        const uint32_t count = (pCount != nullptr) ? *pCount : 0U;
        ss << ", pRegisterSetProperties[";
        if (count > 0) {
            ss << "{";
            ss << "stype: " << pRegisterSetProperties[0].stype;
            ss << ", pNext: " << pRegisterSetProperties[0].pNext;
            ss << ", type: " << pRegisterSetProperties[0].type;
            ss << ", version: " << pRegisterSetProperties[0].version;
            ss << ", generalFlags: " << pRegisterSetProperties[0].generalFlags;
            ss << ", deviceFlags: " << pRegisterSetProperties[0].deviceFlags;
            ss << ", count: " << pRegisterSetProperties[0].count;
            ss << ", bitSize: " << pRegisterSetProperties[0].bitSize;
            ss << ", byteSize: " << pRegisterSetProperties[0].byteSize;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDebugReadRegisters(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDebugWriteRegisters(";
    ss << std::hex << std::showbase;
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

inline std::string _trace_zetDeviceEnableMetricsExp(zet_device_handle_t hDevice) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDeviceEnableMetricsExp(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ")";
    return ss.str();
}

inline void trace_zetDeviceEnableMetricsExp(zet_device_handle_t hDevice) {
    TRACE_EVENT_BEGIN("API", "zetDeviceEnableMetricsExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDeviceEnableMetricsExp(hDevice) + "..\n";
}

inline void trace_zetDeviceEnableMetricsExp(ze_result_t ret, zet_device_handle_t hDevice) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDeviceEnableMetricsExp(hDevice) + trace_ze_result_t(ret);
}

inline std::string _trace_zetDeviceDisableMetricsExp(zet_device_handle_t hDevice) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDeviceDisableMetricsExp(";
    ss << std::hex << std::showbase;
    ss << "hDevice: " << hDevice;
    ss << ")";
    return ss.str();
}

inline void trace_zetDeviceDisableMetricsExp(zet_device_handle_t hDevice) {
    TRACE_EVENT_BEGIN("API", "zetDeviceDisableMetricsExp");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDeviceDisableMetricsExp(hDevice) + "..\n";
}

inline void trace_zetDeviceDisableMetricsExp(ze_result_t ret, zet_device_handle_t hDevice) {
    TRACE_EVENT_END("API");
    if (IS_API_TRACE())
        std::cerr << _trace_zetDeviceDisableMetricsExp(hDevice) + trace_ze_result_t(ret);
}

inline std::string _trace_zetMetricGroupGet(zet_device_handle_t hDevice,
                                            uint32_t *pCount,
                                            zet_metric_group_handle_t *phMetricGroups) {
    std::stringstream ss;
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGroupGet(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGroupGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hMetricGroup: " << hMetricGroup;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", name: ";
        ss << trace_ascii_array(pProperties->name, sizeof(pProperties->name)).str();
        ss << ", description: "
           << trace_ascii_array(pProperties->description, sizeof(pProperties->description)).str();
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGroupCalculateMetricValues(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGet(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGetProperties(";
    ss << std::hex << std::showbase;
    ss << "hMetric: " << hMetric;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", name: ";
        ss << trace_ascii_array(pProperties->name, sizeof(pProperties->name)).str();
        ss << ", description: "
           << trace_ascii_array(pProperties->description, sizeof(pProperties->description)).str();
        ss << ", component: "
           << trace_ascii_array(pProperties->component, sizeof(pProperties->component)).str();
        ss << ", tierNumber: " << pProperties->tierNumber;
        ss << ", metricType: " << pProperties->metricType;
        ss << ", resultType: " << pProperties->resultType;
        ss << ", resultUnits: "
           << trace_ascii_array(pProperties->resultUnits, sizeof(pProperties->resultUnits)).str();
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetContextActivateMetricGroups(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricStreamerOpen(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetCommandListAppendMetricStreamerMarker(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricStreamerClose(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricStreamerReadData(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricQueryPoolCreate(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricQueryPoolDestroy(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricQueryCreate(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricQueryDestroy(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricQueryReset(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetCommandListAppendMetricQueryBegin(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetCommandListAppendMetricQueryEnd(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetCommandListAppendMetricMemoryBarrier(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricQueryGetData(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetKernelGetProfileInfo(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetTracerExpCreate(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetTracerExpDestroy(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetTracerExpSetPrologues(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetTracerExpSetEpilogues(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetTracerExpSetEnabled(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDeviceGetConcurrentMetricGroupsExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricTracerCreateExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricTracerDestroyExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricTracerEnableExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricTracerDisableExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricTracerReadDataExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricDecoderCreateExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricDecoderDestroyExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricDecoderGetDecodableMetricsExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricTracerDecodeExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGroupCalculateMultipleMetricValuesExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGroupGetGlobalTimestampsExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGroupGetExportDataExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGroupCalculateMetricExportDataExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricProgrammableGetExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricProgrammableGetPropertiesExp(";
    ss << std::hex << std::showbase;
    ss << "hMetricProgrammable: " << hMetricProgrammable;
    if (pProperties == nullptr) {
        ss << ", pProperties: nullptr";
    } else {
        ss << ", pProperties {";
        ss << "stype: " << pProperties->stype;
        ss << ", pNext: " << pProperties->pNext;
        ss << ", name: ";
        ss << trace_ascii_array(pProperties->name, sizeof(pProperties->name)).str();
        ss << ", description: "
           << trace_ascii_array(pProperties->description, sizeof(pProperties->description)).str();
        ss << ", component: "
           << trace_ascii_array(pProperties->component, sizeof(pProperties->component)).str();
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricProgrammableGetParamInfoExp(";
    ss << std::hex << std::showbase;
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
        ss << ", name: ";
        ss << trace_ascii_array(pParameterInfo->name, sizeof(pParameterInfo->name)).str();
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricProgrammableGetParamValueInfoExp(";
    ss << std::hex << std::showbase;
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
        ss << ", description: "
           << trace_ascii_array(pValueInfo->description, sizeof(pValueInfo->description)).str();
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricCreateFromProgrammableExp2(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricCreateFromProgrammableExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetDeviceCreateMetricGroupsFromMetricsExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGroupCreateExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGroupAddMetricExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGroupRemoveMetricExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGroupCloseExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricGroupDestroyExp(";
    ss << std::hex << std::showbase;
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
    ss << "NPU_LOG: [API][tid:" << gettid() << "] zetMetricDestroyExp(";
    ss << std::hex << std::showbase;
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
