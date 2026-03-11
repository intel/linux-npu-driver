/*
 * Copyright (C) 2025-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */
#include <stddef.h>
#include <stdint.h>

#include <ze_api.h>

extern "C" {
ze_result_t ZE_APICALL zexDiskCacheSetSize(size_t size);
ze_result_t ZE_APICALL zexDiskCacheGetSize(size_t *size);
ze_result_t ZE_APICALL zexDiskCacheGetDirectory(char *path, size_t *len);
ze_result_t ZE_APICALL zexContextSetIdlePruningTimeout(ze_context_handle_t hContext,
                                                       uint64_t timeoutMs);
}
