/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

namespace VPU {

#define PCI_DEVICE_ID_NVL 0xd71d

struct VPUHwInfo;

VPUHwInfo getHwInfo60xx();

} // namespace VPU
