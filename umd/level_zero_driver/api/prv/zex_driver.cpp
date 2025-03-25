/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "level_zero_driver/api/prv/zex_driver.hpp"

#include "level_zero_driver/source/driver.hpp"
#include "level_zero_driver/source/ext/disk_cache.hpp"

#include <filesystem>
#include <string>

extern "C" {
ze_result_t ZE_APICALL zexDiskCacheSetSize(size_t size) {
    L0::DiskCache &diskCache = L0::Driver::getInstance()->getDiskCache();
    diskCache.setMaxSize(size);
    return ZE_RESULT_SUCCESS;
}

ze_result_t ZE_APICALL zexDiskCacheGetSize(size_t *size) {
    if (!size)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;
    L0::DiskCache &diskCache = L0::Driver::getInstance()->getDiskCache();
    *size = diskCache.getMaxSize();
    return ZE_RESULT_SUCCESS;
}

ze_result_t ZE_APICALL zexDiskCacheGetDirectory(char *path, size_t *len) {
    if (!len || !path)
        return ZE_RESULT_ERROR_INVALID_NULL_POINTER;

    L0::DiskCache &diskCache = L0::Driver::getInstance()->getDiskCache();
    std::string cacheDir = diskCache.getCacheDirPath().string();
    *len = cacheDir.copy(path, *len - 1);
    path[*len] = 0;
    return ZE_RESULT_SUCCESS;
}
}
