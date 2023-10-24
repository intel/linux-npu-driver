/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <variant>

bool getModelFromPath(std::string modelXmlPath, std::vector<char> &xml, std::vector<char> &bin);
