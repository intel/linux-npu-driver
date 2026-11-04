/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <gtest/gtest.h>

template <typename T1, typename T2, typename T3>
::testing::AssertionResult AssertEqAnyOf(const char *x_expr,
                                         const char *y_expr,
                                         const char *z_expr,
                                         const T1 &x,
                                         const T2 &y,
                                         const T3 &z) {
    if (x == y || x == z) {
        return ::testing::AssertionSuccess();
    }

    return ::testing::AssertionFailure()
           << "Value of: " << x_expr << "\n"
           << "  Actual: " << x << "\n"
           << "Expected: " << y_expr << " (" << y << ") or " << z_expr << " (" << z << ")";
}

#define EXPECT_EQ_ANY_OF2(x, y, z) EXPECT_PRED_FORMAT3(AssertEqAnyOf, x, y, z)
