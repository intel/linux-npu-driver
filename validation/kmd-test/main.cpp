/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "test_app.h"

int main(int argc, char **argv) {
    test_app::parse_args(argc, argv);
    return test_app::run();
}
