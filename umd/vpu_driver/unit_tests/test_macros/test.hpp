/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

template <typename Fixture>
struct Test : public Fixture, public ::testing::Test {
    void SetUp() override { Fixture::SetUp(); }

    void TearDown() override { Fixture::TearDown(); }
};
