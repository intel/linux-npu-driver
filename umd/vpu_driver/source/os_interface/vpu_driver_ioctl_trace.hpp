/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <string>

const char *driver_ioctl_request_str(unsigned int request);
std::string driver_ioctl_trace(int fd, unsigned int request, void *arg);
const char *driver_struct_param_cap_index_str(unsigned index);
