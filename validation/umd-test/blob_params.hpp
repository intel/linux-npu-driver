/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "test_app.h"

#include "gtest/gtest.h"
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <variant>

bool loadBlobDataFromNode(const std::string &blobPath,
                          std::vector<std::string> inputFiles,
                          std::vector<std::string> outputFiles,
                          std::vector<std::vector<char>> &inputBin,
                          std::vector<std::vector<char>> &outputBin);

bool loadBlobFromPath(const std::string &blobPath, std::vector<char> &npuBlob);
