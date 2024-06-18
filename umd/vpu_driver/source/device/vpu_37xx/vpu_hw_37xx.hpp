/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

namespace VPU {

#define PCI_DEVICE_ID_MTL 0x7d1d
#define PCI_DEVICE_ID_ARL 0xad1d

struct VPUHwInfo;

VPUHwInfo getHwInfo37xx();

} // namespace VPU
