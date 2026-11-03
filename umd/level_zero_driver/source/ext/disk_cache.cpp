/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "disk_cache.hpp"

#include <stdint.h>

#include "blob_container.hpp"
#include "level_zero_driver/source/ext/hash_function.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <charconv>
#include <filesystem>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <string_view>
#include <sys/stat.h>
#include <time.h>
#include <utility>
#include <ze_graph_ext.h>

namespace L0 {

static std::array<char, HashCity::DigestLength>
computeFooterDigest(const DiskCacheFileFormat::Footer &footer,
                    const uint8_t *blobPtr,
                    size_t blobSize,
                    const uint8_t *compatPtr,
                    size_t compatSize) {
    HashCity hash;
    if (compatSize)
        hash.updateConfigurationHash(compatPtr, compatSize);
    /* Hash only the header fields (everything before digest), digest itself is
       not part of its own input */
    hash.updateConfigurationHash(reinterpret_cast<const uint8_t *>(&footer),
                                 offsetof(DiskCacheFileFormat::Footer, digest));
    std::string digestStr = hash.final(blobPtr, blobSize);

    std::array<char, HashCity::DigestLength> digest;
    memcpy(digest.data(), digestStr.data(), HashCity::DigestLength);
    return digest;
}

DiskCacheFileFormat::Footer DiskCacheFileFormat::makeFooter(uint64_t blobEndOffset,
                                                            const uint8_t *blobPtr,
                                                            size_t blobSize,
                                                            const uint8_t *compatPtr,
                                                            size_t compatSize) {
    Footer footer{.magic = expectedMagic,
                  .version = expectedVersion,
                  .size = static_cast<uint32_t>(sizeof(Footer)),
                  .blobEndOffset = blobEndOffset,
                  .digest = {}};
    footer.digest = computeFooterDigest(footer, blobPtr, blobSize, compatPtr, compatSize);
    return footer;
}

bool DiskCacheFileFormat::verify(const Footer &footer,
                                 const uint8_t *blobPtr,
                                 size_t blobSize,
                                 const uint8_t *compatPtr,
                                 size_t compatSize) {
    if (!footer.isValid())
        return false;

    auto expectedDigest = computeFooterDigest(footer, blobPtr, blobSize, compatPtr, compatSize);
    return expectedDigest == footer.digest;
}

static std::filesystem::path getCacheDir() {
#ifdef ANDROID
    return std::filesystem::path("/data/data") / getprogname() / "ze_intel_npu_cache";
#else
    const char *env = getenv("HOME");
    if (env)
        return std::filesystem::path(env) / ".cache/ze_intel_npu_cache";
    return std::filesystem::current_path() / ".cache/ze_intel_npu_cache";
#endif
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
    return 4 * GB;
}

DiskCache::DiskCache(VPU::OsInterface &osInfc)
    : osInfc(osInfc)
    , cachePath()
    , maxSize() {
    maxSize = getCacheMaxSize();
    if (maxSize == 0) {
        LOG_W("Cache size is set to 0, disabling cache");
        return;
    }

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

    LOG(CACHE, "Cache is initialized, path: %s, max size: %lu", cachePath.c_str(), maxSize);
}

size_t DiskCache::getCacheSize() {
    if (cachePath.empty()) {
        LOG_W("Cache path is empty, disabling cache");
        return 0;
    }

    size_t size = 0;
    osInfc.osiScanDir(cachePath, [&size](const char *name, struct stat &stat) {
        size += static_cast<size_t>(stat.st_size);
    });
    return size;
}

DiskCache::Key DiskCache::computeKey(const ze_graph_desc_2_t &desc,
                                     const vcl_compiler_properties_t *vclProp) {
    if (cachePath.empty())
        return {};

    HashCity hash;
    constexpr uint32_t driverVersion = DRIVER_VERSION;
    hash.updateConfigurationHash(reinterpret_cast<const uint8_t *>(&driverVersion),
                                 sizeof(driverVersion));
    if (vclProp) {
        hash.updateConfigurationHash(reinterpret_cast<const uint8_t *>(vclProp->id),
                                     strlen(vclProp->id));
        hash.updateConfigurationHash(reinterpret_cast<const uint8_t *>(&vclProp->version),
                                     sizeof(vclProp->version));
        hash.updateConfigurationHash(reinterpret_cast<const uint8_t *>(&vclProp->supportedOpsets),
                                     sizeof(vclProp->supportedOpsets));
    }
    hash.updateConfigurationHash(reinterpret_cast<const uint8_t *>(&desc.format),
                                 sizeof(desc.format));

    if (desc.pBuildFlags) {
        hash.updateConfigurationHash(reinterpret_cast<const uint8_t *>(desc.pBuildFlags),
                                     strlen(desc.pBuildFlags));
    }
    hash.updateConfigurationHash(reinterpret_cast<const uint8_t *>(&desc.flags),
                                 sizeof(desc.flags));

    const ze_structure_type_graph_ext_t *type =
        static_cast<const ze_structure_type_graph_ext_t *>(desc.pNext);
    if (type != nullptr && *type == ZE_STRUCTURE_TYPE_GRAPH_INPUT_HASH) {
        const ze_graph_input_hash_t *inputHash =
            static_cast<const ze_graph_input_hash_t *>(desc.pNext);
        return hash.final(reinterpret_cast<const uint8_t *>(&inputHash->hash),
                          sizeof(inputHash->hash));
    }

    return hash.final(desc.pInput, desc.inputSize);
}

std::unique_ptr<BlobContainer> DiskCache::getBlob(const Key &key) {
    if (cachePath.empty())
        return {};

    std::string filename = key;
    std::filesystem::path dataPath = cachePath / filename;

    auto file = osInfc.osiOpenWithSharedLock(dataPath, false);
    if (!file) {
        LOG(CACHE, "Cache missed using %s key", filename.c_str());
        return nullptr;
    }

    if (file->size() <= sizeof(DiskCacheFileFormat::Footer)) {
        LOG_W("Cache missed using %s key: File is too small to hold the footer, removing it",
              filename.c_str());
        /* Remove the file without setting exclusive lock compared to "setBlob()" function */
        osInfc.osiFileRemove(dataPath);
        return nullptr;
    }

    if (file->mmap() == nullptr) {
        LOG_W("Cache missed using %s key: Failed to mmap file", filename.c_str());
        return nullptr;
    }

    uint8_t *filePtr = static_cast<uint8_t *>(file->mmap());
    if (filePtr == nullptr)
        return nullptr;

    /* Payload is everything before the footer: blob + compatibility string */
    size_t payloadSize = file->size() - sizeof(DiskCacheFileFormat::Footer);
    const uint8_t *footerPtr = filePtr + payloadSize;

    /* Copy the footer out of the mapping, it is not guaranteed to be aligned */
    DiskCacheFileFormat::Footer footer;
    memcpy(&footer, footerPtr, sizeof(footer));

    /* blobEndOffset must leave at least one blob byte and must not exceed the payload */
    if (footer.blobEndOffset == 0 || footer.blobEndOffset > payloadSize) {
        LOG_W("Cache missed using %s key: Invalid blob end offset, removing it", filename.c_str());
        /* Remove the file without setting exclusive lock compared to "setBlob()" function */
        osInfc.osiFileRemove(dataPath);
        return nullptr;
    }

    size_t blobSize = static_cast<size_t>(footer.blobEndOffset);
    size_t compatSize = payloadSize - blobSize;
    const uint8_t *compatPtr = filePtr + blobSize;

    if (!DiskCacheFileFormat::verify(footer, filePtr, blobSize, compatPtr, compatSize)) {
        LOG_W("Cache missed using %s key: Invalid footer or checksum, removing it",
              filename.c_str());
        /* Remove the file without setting exclusive lock compared to "setBlob()" function */
        osInfc.osiFileRemove(dataPath);
        return nullptr;
    }

    LOG(CACHE, "Cache hit using %s key", filename.c_str());
    auto blob = std::make_unique<BlobContainer>(filePtr, blobSize, std::move(file));
    blob->compatibilityString.assign(reinterpret_cast<const char *>(compatPtr), compatSize);
    return blob;
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

std::unique_ptr<BlobContainer> DiskCache::setBlob(const Key &key,
                                                  std::unique_ptr<BlobContainer> blob) {
    if (blob == nullptr || cachePath.empty())
        return blob;
    if (blob->size == 0)
        return blob;

    const std::string &compatStr = blob->compatibilityString;
    // Compatibility string and footer (which includes the digest) are stored after blob
    size_t metadataSize = compatStr.size() + sizeof(DiskCacheFileFormat::Footer);
    if (blob->size > std::numeric_limits<size_t>::max() - metadataSize)
        return blob;
    size_t cachedBlobSize = blob->size + metadataSize;
    size_t cacheSize = getCacheSize();

    if (cachedBlobSize > maxSize) {
        return blob;
    } else if (cacheSize + cachedBlobSize > maxSize) {
        cacheSize -= removeLeastUsedFiles(osInfc, cachePath, cacheSize + cachedBlobSize - maxSize);
    }

    std::filesystem::path dstPath = cachePath / key;
    auto file = osInfc.osiOpenWithExclusiveLock(dstPath, true);
    if (!file) {
        return blob;
    }
    if (!file->write(blob->ptr, blob->size)) {
        osInfc.osiFileRemove(dstPath);
        return blob;
    }

    if (!compatStr.empty() && !file->write(compatStr.data(), compatStr.size())) {
        osInfc.osiFileRemove(dstPath);
        return blob;
    }

    DiskCacheFileFormat::Footer footer =
        DiskCacheFileFormat::makeFooter(blob->size,
                                        blob->ptr,
                                        blob->size,
                                        reinterpret_cast<const uint8_t *>(compatStr.data()),
                                        compatStr.size());
    if (!file->write(&footer, sizeof(footer))) {
        osInfc.osiFileRemove(dstPath);
        return blob;
    }

    cacheSize += cachedBlobSize;
    LOG(CACHE,
        "Cache set %s key, data size: %lu, cache size: %lu",
        key.c_str(),
        blob->size,
        cacheSize);

    file.reset();

    auto newBlob = getBlob(key);
    if (newBlob == nullptr) {
        LOG_E("Failed to read back cached blob for key %s\n", key.c_str());
        return blob;
    }
    return newBlob;
}

} // namespace L0
