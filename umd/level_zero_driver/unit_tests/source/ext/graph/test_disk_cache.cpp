/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "level_zero_driver/source/ext/blob_container.hpp"
#include "level_zero_driver/source/ext/disk_cache.hpp"
#include "level_zero_driver/source/ext/hash_function.hpp"
#include "vpu_driver/unit_tests/mocks/gmock_os_interface_imp.hpp"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <ze_graph_ext.h>

#ifdef ZLIB
#include <zlib.h>
#endif

namespace L0 {

class DiskCacheNegTest : public ::testing::Test {
  public:
    VPU::GMockOsInterfaceImp osInfc;
};

TEST_F(DiskCacheNegTest, UninitializedCachedExpectNoReturn) {
    EXPECT_CALL(osInfc, osiCreateDirectories).WillOnce(::testing::Return(false));

    DiskCache diskCache(osInfc);

    ze_graph_desc_2_t desc = {};
    EXPECT_EQ(diskCache.computeKey(desc), "");
    EXPECT_EQ(diskCache.getBlob(""), nullptr);
    diskCache.setBlob("", {});
}

class DiskCacheTest : public ::testing::Test {
  public:
    void SetUp() override {
        EXPECT_CALL(osInfc, osiCreateDirectories).WillOnce(::testing::Return(true));
        EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillRepeatedly(nullptr);
        cache = std::make_unique<DiskCache>(osInfc);
    }

    VPU::GMockOsInterfaceImp osInfc;
    std::unique_ptr<DiskCache> cache;
};

TEST_F(DiskCacheTest, ComputeKeys) {
    ze_graph_desc_2_t desc = {};
    auto emptyKey = cache->computeKey(desc);

    std::string model = "<model resnet-50-pytorch>random</model>";
    desc.pInput = reinterpret_cast<uint8_t *>(model.data());
    desc.inputSize = model.size();
    auto modelKey = cache->computeKey(desc);

    std::string buildFlags = "extra-precision-param";
    desc.pBuildFlags = buildFlags.c_str();
    auto buildKey = cache->computeKey(desc);

    desc.flags = ZE_GRAPH_FLAG_ENABLE_PROFILING | ZE_GRAPH_FLAG_DISABLE_CACHING;
    auto flagKey = cache->computeKey(desc);

    EXPECT_NE(emptyKey, "");
    EXPECT_NE(emptyKey, buildKey);
    EXPECT_NE(modelKey, buildKey);
    EXPECT_NE(flagKey, buildKey);
    EXPECT_NE(flagKey, modelKey);
}

TEST_F(DiskCacheTest, MissCache) {
    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    EXPECT_EQ(cache->getBlob(key), nullptr);
}

TEST_F(DiskCacheTest, MissCacheInvalidFileZeroSize) {
    auto osFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*osFile, size).WillOnce(::testing::Return(0));
    EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillOnce(::testing::Return(std::move(osFile)));
    EXPECT_CALL(osInfc, osiFileRemove).WillOnce(::testing::Return(true));

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    EXPECT_EQ(cache->getBlob(key), nullptr);
}

TEST_F(DiskCacheTest, MissCacheInvalidFileEqChecksumSize) {
    auto osFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*osFile, size).WillOnce(::testing::Return(sizeof(DiskCacheFileFormat::Footer)));
    EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillOnce(::testing::Return(std::move(osFile)));
    EXPECT_CALL(osInfc, osiFileRemove).WillOnce(::testing::Return(true));

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    EXPECT_EQ(cache->getBlob(key), nullptr);
}

static std::vector<uint8_t> buildCacheFileImage(const std::vector<uint8_t> &blobData,
                                                const std::string &compatStr) {
    auto footer =
        DiskCacheFileFormat::makeFooter(blobData.size(),
                                        blobData.data(),
                                        blobData.size(),
                                        reinterpret_cast<const uint8_t *>(compatStr.data()),
                                        compatStr.size());

    const auto *footerPtr = reinterpret_cast<const uint8_t *>(&footer);
    std::vector<uint8_t> image;
    image.insert(image.end(), blobData.begin(), blobData.end());
    image.insert(image.end(), compatStr.begin(), compatStr.end());
    image.insert(image.end(), footerPtr, footerPtr + sizeof(footer));
    return image;
}

TEST_F(DiskCacheTest, HitCache) {
    std::vector<uint8_t> blobData(64, 0);
    auto image = buildCacheFileImage(blobData, "");

    auto osFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*osFile, size).WillRepeatedly(::testing::Return(image.size()));
    EXPECT_CALL(*osFile, mmap).WillRepeatedly(::testing::Return(image.data()));

    EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillOnce(::testing::Return(std::move(osFile)));

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    auto blob = cache->getBlob(key);
    EXPECT_NE(blob, nullptr);
    EXPECT_EQ(blob->ptr, image.data());
    EXPECT_EQ(blob->size, blobData.size());
    EXPECT_EQ(blob->compatibilityString, "");
}

TEST_F(DiskCacheTest, HitCacheWithCompatibilityString) {
    std::vector<uint8_t> blobData(64, 0xAB);
    std::string compatStr = "npu-compatibility-string";
    auto image = buildCacheFileImage(blobData, compatStr);

    auto osFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*osFile, size).WillRepeatedly(::testing::Return(image.size()));
    EXPECT_CALL(*osFile, mmap).WillRepeatedly(::testing::Return(image.data()));

    EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillOnce(::testing::Return(std::move(osFile)));

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    auto blob = cache->getBlob(key);
    ASSERT_NE(blob, nullptr);
    EXPECT_EQ(blob->ptr, image.data());
    EXPECT_EQ(blob->size, blobData.size());
    EXPECT_EQ(blob->compatibilityString, compatStr);
}

TEST_F(DiskCacheTest, MissCacheOldFormatFileWithoutFooter) {
    // Old format: [blob][digest over blob], no footer
    std::vector<uint8_t> blobData(64, 0x5A);
    auto digest = HashCity::getDigest(blobData.data(), blobData.size());

    std::vector<uint8_t> image;
    image.insert(image.end(), blobData.begin(), blobData.end());
    image.insert(image.end(), digest.begin(), digest.end());

    auto osFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*osFile, size).WillRepeatedly(::testing::Return(image.size()));
    EXPECT_CALL(*osFile, mmap).WillRepeatedly(::testing::Return(image.data()));

    EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillOnce(::testing::Return(std::move(osFile)));
    EXPECT_CALL(osInfc, osiFileRemove).WillOnce(::testing::Return(true));

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    EXPECT_EQ(cache->getBlob(key), nullptr);
}

TEST_F(DiskCacheTest, MissCacheCorruptedBlobEndOffsetTooLarge) {
    std::vector<uint8_t> blobData(64, 0x11);
    auto image = buildCacheFileImage(blobData, "compat");

    // Corrupt blobEndOffset in the footer to exceed the payload size
    size_t footerOffset = image.size() - sizeof(DiskCacheFileFormat::Footer);
    uint64_t hugeOffset = std::numeric_limits<uint64_t>::max();
    memcpy(image.data() + footerOffset + offsetof(DiskCacheFileFormat::Footer, blobEndOffset),
           &hugeOffset,
           sizeof(hugeOffset));

    auto osFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*osFile, size).WillRepeatedly(::testing::Return(image.size()));
    EXPECT_CALL(*osFile, mmap).WillRepeatedly(::testing::Return(image.data()));

    EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillOnce(::testing::Return(std::move(osFile)));
    EXPECT_CALL(osInfc, osiFileRemove).WillOnce(::testing::Return(true));

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    EXPECT_EQ(cache->getBlob(key), nullptr);
}

TEST_F(DiskCacheTest, MissCacheZeroBlobEndOffset) {
    std::vector<uint8_t> blobData(64, 0x88);
    auto image = buildCacheFileImage(blobData, "compat");

    // blobEndOffset == 0 means a zero-byte blob and must be rejected
    size_t footerOffset = image.size() - sizeof(DiskCacheFileFormat::Footer);
    uint64_t zeroOffset = 0;
    memcpy(image.data() + footerOffset + offsetof(DiskCacheFileFormat::Footer, blobEndOffset),
           &zeroOffset,
           sizeof(zeroOffset));

    auto osFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*osFile, size).WillRepeatedly(::testing::Return(image.size()));
    EXPECT_CALL(*osFile, mmap).WillRepeatedly(::testing::Return(image.data()));

    EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillOnce(::testing::Return(std::move(osFile)));
    EXPECT_CALL(osInfc, osiFileRemove).WillOnce(::testing::Return(true));

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    EXPECT_EQ(cache->getBlob(key), nullptr);
}

TEST_F(DiskCacheTest, MissCacheCorruptedChecksum) {
    std::vector<uint8_t> blobData(64, 0x22);
    auto image = buildCacheFileImage(blobData, "compat");

    // Corrupt one byte of the blob so the digest no longer matches
    image[0] ^= 0xFF;

    auto osFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*osFile, size).WillRepeatedly(::testing::Return(image.size()));
    EXPECT_CALL(*osFile, mmap).WillRepeatedly(::testing::Return(image.data()));

    EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillOnce(::testing::Return(std::move(osFile)));
    EXPECT_CALL(osInfc, osiFileRemove).WillOnce(::testing::Return(true));

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    EXPECT_EQ(cache->getBlob(key), nullptr);
}

TEST_F(DiskCacheTest, MissCacheUnsupportedFooterVersion) {
    std::vector<uint8_t> blobData(64, 0x44);
    auto image = buildCacheFileImage(blobData, "compat");

    size_t footerOffset = image.size() - sizeof(DiskCacheFileFormat::Footer);
    /* Bump footer version to an unsupported value */
    image[footerOffset + offsetof(DiskCacheFileFormat::Footer, version)] += 1;

    auto osFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*osFile, size).WillRepeatedly(::testing::Return(image.size()));
    EXPECT_CALL(*osFile, mmap).WillRepeatedly(::testing::Return(image.data()));

    EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillOnce(::testing::Return(std::move(osFile)));
    EXPECT_CALL(osInfc, osiFileRemove).WillOnce(::testing::Return(true));

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    EXPECT_EQ(cache->getBlob(key), nullptr);
}

TEST_F(DiskCacheTest, MissCacheUnsupportedFooterSize) {
    std::vector<uint8_t> blobData(64, 0x55);
    auto image = buildCacheFileImage(blobData, "compat");

    size_t footerOffset = image.size() - sizeof(DiskCacheFileFormat::Footer);
    /* Change footer size field to an unexpected value */
    image[footerOffset + offsetof(DiskCacheFileFormat::Footer, size)] += 1;

    auto osFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*osFile, size).WillRepeatedly(::testing::Return(image.size()));
    EXPECT_CALL(*osFile, mmap).WillRepeatedly(::testing::Return(image.data()));

    EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillOnce(::testing::Return(std::move(osFile)));
    EXPECT_CALL(osInfc, osiFileRemove).WillOnce(::testing::Return(true));

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    EXPECT_EQ(cache->getBlob(key), nullptr);
}

TEST_F(DiskCacheTest, ChainedDigestDiffersFromLegacyDigest) {
    std::vector<uint8_t> blobData(64, 0x77);
    auto image = buildCacheFileImage(blobData, "compat");

    /* An old reader computes the legacy one-shot digest over the whole payload (blob + compat +
       footer). It must not match the stored chained digest, otherwise a downgraded driver would
       accept a new-format file and return blob + metadata as an inflated blob */
    auto legacyDigest = HashCity::getDigest(image.data(), image.size() - HashCity::DigestLength);
    std::string storedDigest(image.end() - static_cast<ptrdiff_t>(HashCity::DigestLength),
                             image.end());
    EXPECT_NE(legacyDigest, storedDigest);
}

TEST_F(DiskCacheTest, SetBlobWritesExpectedFileFormat) {
    std::vector<uint8_t> blobData(64, 0x33);
    std::string compatStr = "compat-str";

    std::vector<uint8_t> written;
    auto writeFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*writeFile, write).WillRepeatedly([&written](const void *in, size_t size) {
        EXPECT_NE(in, nullptr);
        EXPECT_GT(size, 0u);
        const auto *data = static_cast<const uint8_t *>(in);
        written.insert(written.end(), data, data + size);
        return true;
    });
    EXPECT_CALL(osInfc, osiOpenWithExclusiveLock).WillOnce(::testing::Return(std::move(writeFile)));

    EXPECT_CALL(osInfc, osiOpenWithSharedLock)
        .WillOnce([&written](const std::filesystem::path &, bool) {
            auto readFile = std::make_unique<VPU::GMockOsFileImp>();
            EXPECT_CALL(*readFile, size).WillRepeatedly([&written]() { return written.size(); });
            EXPECT_CALL(*readFile, mmap).WillRepeatedly([&written]() {
                return static_cast<void *>(written.data());
            });
            return readFile;
        });

    /* setBlob() calls getCacheSize() which scans the (empty) cache directory */
    EXPECT_CALL(osInfc, osiScanDir).WillRepeatedly(::testing::Return());

    auto blob = std::make_unique<BlobContainer>(blobData.data(), blobData.size());
    blob->compatibilityString = compatStr;

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    auto newBlob = cache->setBlob(key, std::move(blob));
    ASSERT_NE(newBlob, nullptr);

    /* Readback must restore the compatibility string from the file */
    EXPECT_EQ(newBlob->size, blobData.size());
    EXPECT_EQ(newBlob->compatibilityString, compatStr);

    /* Verify the on-disk layout: a single footer write, no separate digest write */
    ASSERT_EQ(written.size(),
              blobData.size() + compatStr.size() + sizeof(DiskCacheFileFormat::Footer));
    EXPECT_EQ(memcmp(written.data(), blobData.data(), blobData.size()), 0);
    EXPECT_EQ(memcmp(written.data() + blobData.size(), compatStr.data(), compatStr.size()), 0);

    const uint8_t *footerBytes = written.data() + blobData.size() + compatStr.size();
    // clang-format off
    const uint8_t expHeader[offsetof(DiskCacheFileFormat::Footer, digest)] = {
        'N', 'P', 'U', 'C', 'A', 'C', 'H', 'E', /* magic */
        1, 0, 0, 0,                             /* version = 1, LE */
        sizeof(DiskCacheFileFormat::Footer), 0, 0, 0, /* size, LE */
        64, 0, 0, 0, 0, 0, 0, 0,                /* blobEndOffset = blob size = 64, LE */
    };
    // clang-format on
    EXPECT_EQ(memcmp(footerBytes, expHeader, sizeof(expHeader)), 0);
}

TEST_F(DiskCacheTest, SetBlobWithEmptyCompatibilityString) {
    std::vector<uint8_t> blobData(64, 0x99);

    std::vector<uint8_t> written;
    auto writeFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*writeFile, write).WillRepeatedly([&written](const void *in, size_t size) {
        /* OsFile::write rejects zero-sized writes, setBlob must not issue them */
        EXPECT_NE(in, nullptr);
        EXPECT_GT(size, 0u);
        const auto *data = static_cast<const uint8_t *>(in);
        written.insert(written.end(), data, data + size);
        return true;
    });
    EXPECT_CALL(osInfc, osiOpenWithExclusiveLock).WillOnce(::testing::Return(std::move(writeFile)));

    EXPECT_CALL(osInfc, osiOpenWithSharedLock)
        .WillOnce([&written](const std::filesystem::path &, bool) {
            auto readFile = std::make_unique<VPU::GMockOsFileImp>();
            EXPECT_CALL(*readFile, size).WillRepeatedly([&written]() { return written.size(); });
            EXPECT_CALL(*readFile, mmap).WillRepeatedly([&written]() {
                return static_cast<void *>(written.data());
            });
            return readFile;
        });

    /* setBlob() calls getCacheSize() which scans the (empty) cache directory */
    EXPECT_CALL(osInfc, osiScanDir).WillRepeatedly(::testing::Return());

    auto blob = std::make_unique<BlobContainer>(blobData.data(), blobData.size());

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    auto newBlob = cache->setBlob(key, std::move(blob));
    ASSERT_NE(newBlob, nullptr);
    EXPECT_EQ(newBlob->size, blobData.size());
    EXPECT_EQ(newBlob->compatibilityString, "");

    ASSERT_EQ(written.size(), blobData.size() + sizeof(DiskCacheFileFormat::Footer));
    /* blobEndOffset must equal the blob size when compatibility string is empty */
    uint64_t blobEndOffset = 0;
    memcpy(&blobEndOffset,
           written.data() + blobData.size() + offsetof(DiskCacheFileFormat::Footer, blobEndOffset),
           sizeof(blobEndOffset));
    EXPECT_EQ(blobEndOffset, blobData.size());
}

TEST_F(DiskCacheTest, GetDigestCheck) {
    std::string testInput = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

    auto result =
        HashCity::getDigest(reinterpret_cast<const uint8_t *>(testInput.data()), testInput.size());
    EXPECT_EQ(result.substr(HashCity::DigestLength - sizeof(uint64_t) * 2),
              std::string("0000000000000000"));
}

class HashCityTest : public testing::TestWithParam<std::pair<const char *, const char *>> {};

INSTANTIATE_TEST_SUITE_P(
    ,
    HashCityTest,
    ::testing::ValuesIn(std::vector<std::pair<const char *, const char *>>{
        {"abc", "71e7cfd72d484e2fda4675aed3aaa913b4181ae784906c4f7baac0e933392cc54aa458b7b1e7face"},
        {"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
         "c04078f85a30f6ba2593a1cb793e5ee01fef4771cbcbad6265a756bd6df1d5e14aa458b7b1e7face"},
    }));

TEST_P(HashCityTest, ComputeHash) {
    auto [input, expected] = GetParam();

    HashCity hash;
    std::string fakeConfig = "fake-config";
    hash.updateConfigurationHash(reinterpret_cast<const uint8_t *>(fakeConfig.data()),
                                 fakeConfig.size());
    ASSERT_EQ(expected, hash.final(reinterpret_cast<const uint8_t *>(input), strlen(input)));
}

} // namespace L0
