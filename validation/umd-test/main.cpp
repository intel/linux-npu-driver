/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_test.h"

#include <getopt.h>

namespace test_vars {
bool test_with_gpu;
bool disable_metrics;
} // namespace test_vars

static void setConfig(const char *optarg) {
    if (!Environment::setupGlobalConfig(optarg)) {
        fprintf(stderr, "Failed to set up config from path: %s", optarg);
        exit(1);
    }
}

static void setQuickFilter(const char *) {
    test_app::append_negative_filter("CommandBarrier.*:CommandGraphLong*:"
                                     "*GraphInference*:CompilerInDriverLong*:"
                                     "CompilerInDriverWithProfiling*:CompilerInDriverLayers*");
}

static void disableMetrics(const char *) {
    test_vars::disable_metrics = true;
    test_app::append_negative_filter("Metric*:ImmediateCmdList.MetricQuerryTest");
}

const char *helpMsg = "  -q/--quick\n"
                      "       Disable long running tests. Useful for pre-commit testing\n"
                      "  -c/--config [CONFIGURATION_PATH]\n"
                      "       Test configuration file in yaml format\n"
                      "  -G/--test_with_gpu\n"
                      "       Enable testing with loaded GPU L0\n"
                      "  -M/--disable_metrics\n"
                      "       Disabling metrics. No metric test will be run\n";

int main(int argc, char **argv) {
    ::testing::AddGlobalTestEnvironment(Environment::getInstance());

    // The config has to be set before InitGoogleTest to fill out parametrized tests
    struct option options[] = {{"config", required_argument, NULL, 'c'}, {NULL, 0, NULL, '0'}};
    char opt = 0;
    while (opt >= 0) {
        opt = getopt_long(argc, argv, "-:c:", options, NULL);
        if (opt == 'c') {
            setConfig(optarg);
            break;
        }
    };
    optind = 1;

    test_app::ArgumentMap args = {
        {'c', {"config", required_argument, [](auto) {}}},
        {'q', {"quick", no_argument, &setQuickFilter}},
        {'G', {"test_with_gpu", no_argument, [](auto) { test_vars::test_with_gpu = true; }}},
        {'M', {"disable_metrics", no_argument, &disableMetrics}},
    };

    test_app::parse_args(args, helpMsg, argc, argv);
    return test_app::run();
}
