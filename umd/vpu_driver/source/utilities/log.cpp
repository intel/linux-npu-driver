/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/utilities/log.hpp"

namespace VPU {

static LogLevel curLogLevel = UMD_LOGLEVEL;

static inline const char *getBaseName(const char *filePath) {
    const char *lastDirPos = __builtin_strrchr(filePath, '/');
    const char *fn = (lastDirPos == nullptr) ? filePath : lastDirPos + 1;
    return fn;
}

void printLog(LogLevel debugLevel,
              const char *file,
              const char *function,
              const int line,
              const char *format,
              ...) {
    if (debugLevel <= getLogLevel()) {
        const char *prefix;
        switch (debugLevel) {
        case VERBOSE:
            prefix = "V";
            break;
        case ERROR:
            prefix = "E";
            break;
        case WARNING:
            prefix = "W";
            break;
        case INFO:
            prefix = "I";
            break;
        default:
            prefix = "?";
            break;
        }
        fprintf(stderr, "VPU_LOG:[%s] %s::%s():%d: ", prefix, getBaseName(file), function, line);
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fprintf(stderr, "\n");
    }
}

LogLevel getLogLevel() {
    return curLogLevel;
}

void setLogLevel(LogLevel level) {
    if (level <= VERBOSE && level >= QUIET) {
        curLogLevel = level;
        return;
    }
    LOG_W("Invalid log level(%d) keeping current level(%d)\n", level, curLogLevel);
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
    } else if (str == "VERBOSE" || str == "verbose") {
        setLogLevel(VERBOSE);
    }
}

} // namespace VPU
