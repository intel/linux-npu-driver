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
    CMDLIST = BIT(0),
    CMDQUEUE = BIT(1),
    CONTEXT = BIT(2),
    DEVICE = BIT(3),
    DRIVER = BIT(4),
    EVENT = BIT(5),
    FENCE = BIT(6),
    FSYS = BIT(7),
    GRAPH = BIT(8),
    IOCTL = BIT(9),
    MEMORY = BIT(10),
    METRIC = BIT(11),
    MISC = BIT(12),
    UTEST = BIT(13),
    VPU_CMD = BIT(14),
    VPU_CTX = BIT(15),
    VPU_JOB = BIT(16),
    CACHE = BIT(17),
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
