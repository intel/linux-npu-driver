/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"
#include "testenv.hpp"

int main(int argc, char **argv) {
    struct option options[] = {{"config", required_argument, NULL, 'c'}, {NULL, 0, NULL, '0'}};
    const char *configurationPath = nullptr;
    char opt;

    do {
        opt = getopt_long(argc, argv, ":c:", options, NULL);
        if (opt == 'c')
            configurationPath = optarg;
    } while (opt >= 0);
    optind = 0;

    ::testing::AddGlobalTestEnvironment(Environment::getInstance());

    if (!Environment::setupGlobalConfig(configurationPath))
        return -EINVAL;

    test_app::parse_args(argc, argv);
    return test_app::run();
}
