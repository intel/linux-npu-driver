/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "vpu_driver/source/utilities/log.hpp"
#include "ze_ddi.h"
#include "zer_ddi.h"
#include "zes_ddi.h"
#include "zet_ddi.h"

#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

/* Define empty Ddi tables to satisfy linker, these tables are not used by unit tests */
zet_dditable_driver_t toolsDdiTable = {};
ze_dditable_driver_t coreDDITable = {};
zes_dditable_driver_t sysmanDdiTable = {};
zer_dditable_driver_t runtimeDdiTable = {};

static void printHelpMessage() {
    constexpr auto helpMessage =
        "\n"
        "Extra test options:\n"
        "  -v/--verbose            Set log level to verbose\n"
        "  -l/--loglevel <lvl>     Set log level - error, warning, info, verbose\n"
        "\n";

    printf("%s\n", helpMessage);
}

static bool parseOptions(int argc, char **argv) {
    static struct option longOptions[] = {{"verbose", no_argument, 0, 'v'},
                                          {"loglevel", required_argument, 0, 'l'},
                                          {}};

    int opt;
    while ((opt = getopt_long(argc, argv, "vl:b:m:", longOptions, nullptr)) != -1) {
        switch (opt) {
        case 'v':
            VPU::setLogLevel(INFO);
            break;
        case 'l':
            VPU::setLogLevel(optarg);
            break;
        default:
            printHelpMessage();
            return false;
        }
    }
    return true;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleMock(&argc, argv);
    if (!parseOptions(argc, argv))
        return 1;
    return RUN_ALL_TESTS();
}
