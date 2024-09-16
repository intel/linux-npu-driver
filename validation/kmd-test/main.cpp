/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "test_app.h"

static void setQuickTestFilter(const char *) {
    test_app::append_negative_filter("*.*SystemToSystem_4MB:"
                                     "*.*SystemToSystem_15MB:"
                                     "*.*_Slow*:"
                                     "*.Perf_*:"
                                     "CmdBufTest.*:"
                                     "UapiSubmit*:"
                                     "Unbind*:"
                                     "Security*:"
                                     "OsSuspend*:");
}

const char *helpMsg =
    "  -q/--quick\n"
    "      Disable long running tests. Useful for pre-commit testing\n"
    "  -T/--max_timeout [MSEC]\n"
    "       [EXPERIMENTAL] Set timeout to maximum for PM operations, job syncing, etc.\n";

int main(int argc, char **argv) {
    test_app::ArgumentMap args = {
        {'q', {"quick", no_argument, &setQuickTestFilter}},
        {'T', {"max_timeout", no_argument, [](auto) { test_app::max_timeout = true; }}},
    };

    test_app::parse_args(args, helpMsg, argc, argv);
    return test_app::run();
}
