/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstdint>

#include <cstdio>
#include <cstring>
#include <string_view>

enum LogLevel { QUIET = 0, ERROR, WARNING, INFO };

#define BIT(nr) ((1UL) << (nr))
enum LogMask : uint64_t {
    CMDLIST = BIT(0),  // 0x1
    CMDQUEUE = BIT(1), // 0x2
    CONTEXT = BIT(2),  // 0x4
    DEVICE = BIT(3),   // 0x8
    DRIVER = BIT(4),   // 0x10
    EVENT = BIT(5),    // 0x20
    FENCE = BIT(6),    // 0x40
    FSYS = BIT(7),     // 0x80
    GRAPH = BIT(8),    // 0x100
    IOCTL = BIT(9),    // 0x200
    MEMORY = BIT(10),  // 0x400
    METRIC = BIT(11),  // 0x800
    MISC = BIT(12),    // 0x1000
    UTEST = BIT(13),   // 0x2000
    VPU_CMD = BIT(14), // 0x4000
    VPU_CTX = BIT(15), // 0x8000
    VPU_JOB = BIT(16), // 0x10000
    CACHE = BIT(17),   // 0x20000
};
#undef BIT

#ifndef UMD_LOGLEVEL
#define UMD_LOGLEVEL QUIET
#endif

#ifndef UMD_LOGMASK
#define UMD_LOGMASK                                                                            \
    (CMDLIST | CMDQUEUE | CONTEXT | DEVICE | DRIVER | EVENT | FENCE | GRAPH | IOCTL | MEMORY | \
     MISC | VPU_CMD | VPU_CTX | VPU_JOB | CACHE)
#endif

#define __FNAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define _LOG(LEVEL, MASK, fmt, ...)                           \
    do {                                                      \
        if (LEVEL > VPU::getLogLevel())                       \
            break;                                            \
                                                              \
        if ((LEVEL > WARNING) && !(MASK & VPU::getLogMask())) \
            break;                                            \
                                                              \
        if (LEVEL > WARNING)                                  \
            fprintf(stderr,                                   \
                    "NPU_LOG: [%s][%s:%d] " fmt "\n",         \
                    VPU::getLogMaskStr(MASK),                 \
                    __FNAME__,                                \
                    __LINE__,                                 \
                    ##__VA_ARGS__);                           \
        else                                                  \
            fprintf(stderr,                                   \
                    "NPU_LOG: *%s* [%s:%d] " fmt "\n",        \
                    VPU::getLogLevelStr(LEVEL),               \
                    __FNAME__,                                \
                    __LINE__,                                 \
                    ##__VA_ARGS__);                           \
    } while (0)

#define LOG_E(...) _LOG(ERROR, DRIVER, __VA_ARGS__)
#define LOG_W(...) _LOG(WARNING, DRIVER, __VA_ARGS__)
#define LOG(MASK, ...) _LOG(INFO, MASK, __VA_ARGS__)

#define CASE_RETURN_STR(x) \
    case x:                \
        return #x;

namespace VPU {

void setLogLevel(std::string_view str);
void setLogLevel(LogLevel level);
LogLevel getLogLevel();
const char *getLogLevelStr(LogLevel level);

void setLogMask(std::string_view str);
uint64_t getLogMask();
const char *getLogMaskStr(uint64_t mask);

} // namespace VPU
