/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"

#include <cstdlib>

namespace VPU {

static LogLevel logLevel = UMD_LOGLEVEL;
static uint64_t logMask = UMD_LOGMASK;

LogLevel getLogLevel() {
    return logLevel;
}

const char *getLogLevelStr(LogLevel level) {
    switch (level) {
        CASE_RETURN_STR(ERROR);
        CASE_RETURN_STR(WARNING);
    default:
        return "?";
    };
}

void setLogLevel(LogLevel level) {
    if (level <= INFO && level >= QUIET) {
        logLevel = level;
        return;
    }
    LOG_W("Invalid log level(%d) keeping current level(%d)\n", level, logLevel);
}

void setLogLevel(std::string_view str) {
    if (str == "QUIET" || str == "quiet") {
        setLogLevel(QUIET);
    } else if (str == "ERROR" || str == "error") {
        setLogLevel(ERROR);
    } else if (str == "WARNING" || str == "warning") {
        setLogLevel(WARNING);
    } else if (str == "INFO" || str == "info") {
        setLogLevel(INFO);
    } else {
        setLogLevel(UMD_LOGLEVEL);
    }
}

uint64_t getLogMask() {
    return logMask;
}

const char *getLogMaskStr(uint64_t mask) {
    switch (mask) {
        CASE_RETURN_STR(CMDLIST);
        CASE_RETURN_STR(CMDQUEUE);
        CASE_RETURN_STR(CONTEXT);
        CASE_RETURN_STR(DEVICE);
        CASE_RETURN_STR(DRIVER);
        CASE_RETURN_STR(EVENT);
        CASE_RETURN_STR(FENCE);
        CASE_RETURN_STR(FSYS);
        CASE_RETURN_STR(GRAPH);
        CASE_RETURN_STR(IOCTL);
        CASE_RETURN_STR(MEMORY);
        CASE_RETURN_STR(METRIC);
        CASE_RETURN_STR(MISC);
        CASE_RETURN_STR(UTEST);
        CASE_RETURN_STR(VPU_CMD);
        CASE_RETURN_STR(VPU_CTX);
        CASE_RETURN_STR(VPU_JOB);
        CASE_RETURN_STR(CACHE);
    default:
        return "?";
    };
}

void setLogMask(std::string_view str) {
    if (str != "") {
        uint64_t tmpMask;
        char *end;

        tmpMask = std::strtoul(str.data(), &end, 0);
        if (*end == '\0') {
            logMask = tmpMask;
            return;
        }
        LOG_W("Invalid log mask keeping current (0x%lx)", logMask);
    }
}

} // namespace VPU
