/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "npu_driver_compiler.h"

#include <string_view>

extern vcl_log_level_t cidLogLevel;

void setCidLogLevel(std::string_view &str);
