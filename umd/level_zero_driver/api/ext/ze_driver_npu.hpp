/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <ze_api.h>
#include <ze_driver_npu_ext.h>

namespace L0 {

ze_result_t ZE_APICALL zeDriverGetExtensionExt(ze_driver_handle_t hDriver,
                                               ze_driver_extension_npu_ext_t *pExtension);

} // namespace L0
