/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "disk_cache.hpp"

#include <stdint.h>

#include "compiler.hpp"
#include "hash_function.hpp"
#include "level_zero_driver/ext/source/graph/blob_container.hpp"
#include "npu_driver_compiler.h"
#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <charconv>
#include <filesystem>
#include <functional>
#include <level_zero/ze_graph_ext.h>
#include <map>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <string_view>
#include <sys/stat.h>
#include <time.h>
#include <utility>

namespace L0 {

static std::filesystem::path getCacheDir() {
    const char *env = getenv("ZE_INTEL_NPU_CACHE_DIR");
    if (env)
        return env;
    env = getenv("XDG_CACHE_HOME");
    if (env)
        return std::filesystem::path(env) / "ze_intel_npu_cache";
    env = getenv("HOME");
    if (env)
        return std::filesystem::path(env) / ".cache/ze_intel_npu_cache";
    return std::filesystem::current_path() / ".cache/ze_intel_npu_cache";
}

static size_t getCacheMaxSize() {
    const char *env = getenv("ZE_INTEL_NPU_CACHE_SIZE");
    if (env) {
        size_t val = 0;
        std::string_view envStr = env;
        // On error "from_chars" function leave "val" unmodified
        std::from_chars(envStr.begin(), envStr.end(), val);
        return val;
    }
    constexpr size_t GB = 1024 * 1024 * 1024;
    return 1 * GB;
}

static size_t getCacheSize(VPU::OsInterface &osInfc, std::filesystem::path &cachePath) {
    size_t size = 0;
    osInfc.osiScanDir(cachePath, [&size](const char *name, struct stat &stat) {
        size += static_cast<size_t>(stat.st_size);
    });
    return size;
}

DiskCache::DiskCache(VPU::OsInterface &osInfc)
    : osInfc(osInfc)
    , cachePath()
    , maxSize() {
    cachePath = getCacheDir();
    if (cachePath.empty()) {
        LOG_W("Cache path is empty, disabling cache");
        return;
    }

    if (!osInfc.osiCreateDirectories(cachePath)) {
        LOG_W("Failed to create cache directory, disabling cache");
        cachePath.clear();
        return;
    }

    maxSize = getCacheMaxSize();
    LOG(CACHE, "Cache is initialized, path: %s, max size: %lu", cachePath.c_str(), maxSize);
}

DiskCache::Key DiskCache::computeKey(const ze_graph_desc_2_t &desc) {
    if (cachePath.empty())
        return {};

    HashSha1 hash;
    constexpr uint32_t driverVersion = DRIVER_VERSION;
    hash.update(reinterpret_cast<const uint8_t *>(&driverVersion), sizeof(driverVersion));
    vcl_compiler_properties_t vclProp = {};
    if (Compiler::getCompilerProperties(&vclProp)) {
        hash.update(reinterpret_cast<const uint8_t *>(vclProp.id), strlen(vclProp.id));
        hash.update(reinterpret_cast<const uint8_t *>(&vclProp.version), sizeof(vclProp.version));
        hash.update(reinterpret_cast<const uint8_t *>(&vclProp.supportedOpsets),
                    sizeof(vclProp.supportedOpsets));
    }
    hash.update(reinterpret_cast<const uint8_t *>(&desc.format), sizeof(desc.format));
    hash.update(desc.pInput, desc.inputSize);
    if (desc.pBuildFlags) {
        hash.update(reinterpret_cast<const uint8_t *>(desc.pBuildFlags), strlen(desc.pBuildFlags));
    }
    hash.update(reinterpret_cast<const uint8_t *>(&desc.flags), sizeof(desc.flags));

    return hash.final();
}

std::unique_ptr<BlobContainer> DiskCache::getBlob(const Key &key) {
    if (cachePath.empty())
        return {};

    std::string filename = key;
    std::filesystem::path dataPath = cachePath / filename;

    auto file = osInfc.osiOpenWithSharedLock(dataPath, false);
    if (!file || file->size() == 0 || file->mmap() == nullptr) {
        LOG(CACHE, "Cache missed using %s key", filename.c_str());
        return nullptr;
    }

    LOG(CACHE, "Cache hit using %s key", filename.c_str());
    return std::make_unique<BlobFileContainer>(static_cast<uint8_t *>(file->mmap()),
                                               file->size(),
                                               std::move(file));
}

static size_t
removeLeastUsedFiles(VPU::OsInterface &osInfc, std::filesystem::path &cachePath, size_t expSize) {
    std::map<time_t, std::string> sortedFiles;
    osInfc.osiScanDir(cachePath, [&sortedFiles](const char *name, struct stat &stat) {
        /* Posix struct stat is used because std::filesystem only show timestamp of last write */
        sortedFiles.emplace(stat.st_atim.tv_sec, name);
    });

    size_t removedSize = 0;
    for (auto &[timestamp, filename] : sortedFiles) {
        auto filePath = cachePath / filename;
        auto file = osInfc.osiOpenWithExclusiveLock(filePath, false);
        if (!file)
            continue;

        auto fileSize = file->size();
        if (!osInfc.osiFileRemove(filePath))
            continue;

        LOG(CACHE,
            "Removed: %s, last access: %lu, size: %lu",
            filename.c_str(),
            timestamp,
            fileSize);
        removedSize += fileSize;
        if (removedSize >= expSize)
            break;
    }

    return removedSize;
}

void DiskCache::setBlob(const Key &key, const std::unique_ptr<BlobContainer> &blob) {
    if (blob == nullptr || cachePath.empty())
        return;

    size_t cacheSize = getCacheSize(osInfc, cachePath);
    if (cacheSize + blob->size > maxSize) {
        cacheSize -= removeLeastUsedFiles(osInfc, cachePath, cacheSize + blob->size - maxSize);
    }

    std::filesystem::path dstPath = cachePath / key;
    auto file = osInfc.osiOpenWithExclusiveLock(dstPath, true);
    if (!file)
        return;

    if (!file->write(blob->ptr, blob->size)) {
        osInfc.osiFileRemove(dstPath);
        return;
    }

    cacheSize += blob->size;
    LOG(CACHE,
        "Cache set %s key, data size: %lu, cache size: %lu",
        key.c_str(),
        blob->size,
        cacheSize);
}

} // namespace L0
