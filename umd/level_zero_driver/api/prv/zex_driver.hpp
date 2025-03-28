/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */
#include <stddef.h>

#include <level_zero/ze_api.h>

extern "C" {
ze_result_t ZE_APICALL zexDiskCacheSetSize(size_t size);
ze_result_t ZE_APICALL zexDiskCacheGetSize(size_t *size);
ze_result_t ZE_APICALL zexDiskCacheGetDirectory(char *path, size_t *len);
}