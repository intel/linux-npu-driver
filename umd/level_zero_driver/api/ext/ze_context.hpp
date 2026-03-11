/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <ze_api.h>
#include <ze_context_npu_ext.h>

namespace L0 {
ze_result_t zeContextSetProperties(ze_context_handle_t hContext,
                                   ze_context_properties_npu_ext_t *pContextProperties);

ze_result_t zeContextReleaseMemory(ze_context_handle_t hContext);

} // namespace L0
