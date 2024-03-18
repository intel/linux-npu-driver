/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <cstdio>
#include <cstdarg>
#include <string>

enum LogLevel { QUIET = 0, ERROR, WARNING, INFO, VERBOSE };

namespace VPU {

#ifndef UMD_LOGLEVEL
#define UMD_LOGLEVEL QUIET
#endif

void printLog(LogLevel debugLevel,
              const char *file,
              const char *function,
              const int line,
              const char *format,
              ...) __attribute__((format(printf, 5, 6)));

void setLogLevel(LogLevel level);
void setLogLevel(std::string_view str);

LogLevel getLogLevel();

} // namespace VPU

#define LOG(LEVEL, ...) VPU::printLog(LEVEL, __FILE__, __func__, __LINE__, __VA_ARGS__)

#define LOG_V(...) LOG(VERBOSE, __VA_ARGS__)

#define LOG_I(...) LOG(INFO, __VA_ARGS__)

#define LOG_W(...) LOG(WARNING, __VA_ARGS__)

#define LOG_E(...) LOG(ERROR, __VA_ARGS__)
