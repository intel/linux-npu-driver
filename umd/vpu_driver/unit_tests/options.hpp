/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/utilities/log.hpp"

#include <array>
#include <cstring>
#include <getopt.h>

namespace UnitTestOptions {

inline LogLevel getLogLevelFromStr(const char *str) {
    static constexpr std::array<std::pair<const char *, LogLevel>, 5> logLevel = {
        {{"verbose", VERBOSE},
         {"info", INFO},
         {"warning", WARNING},
         {"error", ERROR},
         {"quiet", QUIET}}};
    for (const auto &l : logLevel)
        if (strncmp(l.first, str, strlen(l.first)) == 0)
            return l.second;
    LOG_E("Failed to set log level. Log level %s does not exist", str);
    return ERROR;
}

inline void parseLogOptions(int argc, char **argv) {
    static struct option longOptions[] = {{"verbose", no_argument, 0, 'v'},
                                          {"loglevel", required_argument, 0, 'l'},
                                          {0, 0, 0, 0}};

    int opt;
    while ((opt = getopt_long(argc, argv, "vl:", longOptions, nullptr)) != -1) {
        switch (opt) {
        case 'v':
            VPU::setLogLevel(VERBOSE);
            break;
        case 'l':
            VPU::setLogLevel(getLogLevelFromStr(optarg));
            break;
        }
    }
}
} // namespace UnitTestOptions
