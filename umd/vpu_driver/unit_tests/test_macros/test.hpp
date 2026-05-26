/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "gtest/gtest.h"

template <typename Fixture>
struct Test : public Fixture, public ::testing::Test {
    void SetUp() override { Fixture::SetUp(); }

    void TearDown() override { Fixture::TearDown(); }
};

#define BREAK_ON_TEST_FAILURE()          \
    if (::testing::Test::HasFailure()) { \
        break;                           \
    }

#define RETURN_ON_TEST_FAILURE(x)        \
    if (::testing::Test::HasFailure()) { \
        return x;                        \
    }
