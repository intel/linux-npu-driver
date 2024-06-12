/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

namespace VPU {

#define PCI_DEVICE_ID_LNL 0x643e

struct VPUHwInfo;

VPUHwInfo getHwInfo40xx();

} // namespace VPU
