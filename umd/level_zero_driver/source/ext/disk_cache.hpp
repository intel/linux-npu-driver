/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "blob_container.hpp"
#include "hash_function.hpp"
#include "npu_driver_compiler.h"

#include <array>
#include <bit>
#include <filesystem>
#include <memory>
#include <string>
#include <ze_graph_ext.h>

namespace VPU {
class OsInterface;
} // namespace VPU

namespace L0 {

/*
 * Cache file layout (blob is always stored at offset 0):
 *   [blob data][compatibility string bytes][Footer]
 *
 * Footer.digest covers blob data, compatibility string and the rest of the
 * footer. It is computed as a chained HashCity digest (configuration hash
 * over compatibility string and footer header, final hash over blob data),
 * which intentionally differs from the legacy digest over raw file bytes so
 * that old readers reject new files and vice versa.
 */
struct DiskCacheFileFormat {
    static constexpr std::array<uint8_t, 8> expectedMagic =
        {'N', 'P', 'U', 'C', 'A', 'C', 'H', 'E'};
    static constexpr uint32_t expectedVersion = 1;

    /* Serialized to the file as-is, all integer fields are stored little-endian.
       blobEndOffset is the absolute offset (from the start of the file) where the
       compatibility string begins, i.e. it equals the blob size since the blob is
       always stored at offset 0. */
    struct Footer {
        std::array<uint8_t, 8> magic;
        uint32_t version;
        uint32_t size;
        uint64_t blobEndOffset;
        std::array<char, HashCity::DigestLength> digest;

        bool isValid() const {
            return magic == expectedMagic && version == expectedVersion && size == sizeof(Footer);
        }
    };

    static_assert(std::endian::native == std::endian::little,
                  "Cache file format requires a little-endian host");
    static_assert(sizeof(Footer) == 24 + HashCity::DigestLength,
                  "Footer must be packed without padding");
    static_assert(offsetof(Footer, magic) == 0);
    static_assert(offsetof(Footer, version) == 8);
    static_assert(offsetof(Footer, size) == 12);
    static_assert(offsetof(Footer, blobEndOffset) == 16);
    static_assert(offsetof(Footer, digest) == 24);

    /* Builds a footer with blobEndOffset set and digest computed over the given
       blob/compatibility string content and the footer's own header fields. */
    static Footer makeFooter(uint64_t blobEndOffset,
                             const uint8_t *blobPtr,
                             size_t blobSize,
                             const uint8_t *compatPtr,
                             size_t compatSize);

    /* Validates magic/version/size and recomputes the digest to compare against
       footer.digest. */
    static bool verify(const Footer &footer,
                       const uint8_t *blobPtr,
                       size_t blobSize,
                       const uint8_t *compatPtr,
                       size_t compatSize);
};

class DiskCache {
  public:
    DiskCache(VPU::OsInterface &osInfc);

    using Key = std::string;

    Key computeKey(const ze_graph_desc_2_t &desc,
                   const vcl_compiler_properties_t *vclProp = nullptr);
    std::unique_ptr<BlobContainer> getBlob(const Key &key);
    std::unique_ptr<BlobContainer> setBlob(const Key &key, std::unique_ptr<BlobContainer> blob);

    void setMaxSize(size_t size) { maxSize = size; }
    size_t getMaxSize() { return maxSize; }
    size_t getCacheSize();

  private:
    VPU::OsInterface &osInfc;
    std::filesystem::path cachePath;
    size_t maxSize;
};

} // namespace L0
