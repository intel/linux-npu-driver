/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "gtest/gtest.h"

#include <filesystem>
#include <vector>
#include <fstream>

inline void loadBlobFromFile(const std::string &path, std::vector<uint8_t> &blob) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    ASSERT_TRUE(file.is_open()) << "Failed to open file: " << path;
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    blob.resize(static_cast<size_t>(fileSize));
    file.read(reinterpret_cast<char *>(blob.data()), fileSize);
}
