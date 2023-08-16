/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "vpu_driver/unit_tests/options.hpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleMock(&argc, argv);
    UnitTestOptions::parseLogOptions(argc, argv);
    return RUN_ALL_TESTS();
}
