/*
 * Copyright (C) 2022-2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "umd_common.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(Utils, safeCastFromSingedToSignedConversion) {
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<int, char>(0)), 0));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<int, char>(CHAR_MIN)), CHAR_MIN));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<int, char>(CHAR_MAX)), CHAR_MAX));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<int, long long>(0)), 0));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<int, long long>(INT_MIN)), INT_MIN));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<int, long long>(INT_MAX)), INT_MAX));

    EXPECT_THROW((safe_cast<int, long long>(LLONG_MIN)), std::overflow_error);
    EXPECT_THROW((safe_cast<int, long long>(LLONG_MAX)), std::overflow_error);
}

TEST(Utils, safeCastFromUnsignedToUnsignedConversion) {
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<unsigned long long, unsigned>(0)), 0));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<unsigned long long, unsigned>(UINT_MAX)), UINT_MAX));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<unsigned, unsigned long long>(UINT_MAX)), UINT_MAX));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<unsigned, unsigned long long>(0)), 0));

    EXPECT_THROW((safe_cast<unsigned, unsigned long long>(ULLONG_MAX)), std::overflow_error);
}

TEST(Utils, safeCastFromSingedToUnignedConversion) {
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<unsigned, char>(0)), 0));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<unsigned, char>(CHAR_MAX)), CHAR_MAX));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<unsigned, int>(0)), 0));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<unsigned, int>(INT_MAX)), INT_MAX));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<unsigned, long long>(0)), 0));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<unsigned, long long>(INT_MAX)), INT_MAX));

    EXPECT_THROW((safe_cast<unsigned, char>(CHAR_MIN)), std::overflow_error);
    EXPECT_THROW((safe_cast<unsigned, int>(INT_MIN)), std::overflow_error);
    EXPECT_THROW((safe_cast<unsigned, long long>(LLONG_MIN)), std::overflow_error);
    EXPECT_THROW((safe_cast<unsigned, long long>(LLONG_MAX)), std::overflow_error);
}

TEST(Utils, safeCastFromUnsignedToSingedConversion) {
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<int, unsigned char>(0)), 0));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<int, unsigned char>(UCHAR_MAX)), UCHAR_MAX));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<int, unsigned>(0)), 0));
    EXPECT_NO_THROW(EXPECT_EQ((safe_cast<int, unsigned long long>(0)), 0));

    EXPECT_THROW((safe_cast<int, unsigned>(UINT_MAX)), std::overflow_error);
    EXPECT_THROW((safe_cast<int, unsigned long long>(ULLONG_MAX)), std::overflow_error);
}
