/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <filesystem>

namespace TestOptions {

extern std::filesystem::path blobPath;
extern std::filesystem::path modelPath;
extern std::string modelFlags;

}; // namespace TestOptions
