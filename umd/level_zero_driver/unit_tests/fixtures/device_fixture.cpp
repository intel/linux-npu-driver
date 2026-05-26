/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"

#include <ze_ddi.h>
#include <zer_ddi.h>
#include <zes_ddi.h>
#include <zet_ddi.h>

zet_dditable_driver_t toolsDdiTable = {};
ze_dditable_driver_t coreDDITable = {};
zes_dditable_driver_t sysmanDdiTable = {};
zer_dditable_driver_t runtimeDdiTable = {};

namespace L0 {
namespace ult {} // namespace ult
} // namespace L0
