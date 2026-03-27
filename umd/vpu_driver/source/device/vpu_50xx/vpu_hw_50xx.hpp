/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>

namespace VPU {

#define PCI_DEVICE_ID_PTL_P 0xb03e
#define PCI_DEVICE_ID_WCL 0xfd3e

struct VPUHwInfo;

VPUHwInfo getHwInfo50xx(uint32_t deviceId);

} // namespace VPU
