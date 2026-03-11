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

#include <cstring>
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

TEST_F(DiskCacheTest, MissCacheInvalidFileSize) {
    auto osFile = std::make_unique<VPU::GMockOsFileImp>();
    EXPECT_CALL(*osFile, size).WillOnce(::testing::Return(0));

    EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillOnce(::testing::Return(std::move(osFile)));

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    EXPECT_EQ(cache->getBlob(key), nullptr);
}

TEST_F(DiskCacheTest, HitCache) {
    // blob size + checksum
    constexpr size_t fileSize = 64 + HashCity::DigestLength;

    auto osFile = std::make_unique<VPU::GMockOsFileImp>();
    auto mmapPtr = std::make_unique<uint8_t[]>(fileSize);

    uint64_t checksumOffset = fileSize - HashCity::DigestLength;
    auto *checksumPtr = reinterpret_cast<HashCity::DigestType>(mmapPtr.get() + checksumOffset);

    auto checksum = HashCity::getDigest(mmapPtr.get(), checksumOffset);
    memcpy(checksumPtr, checksum.data(), checksum.size());

    EXPECT_CALL(*osFile, size).WillRepeatedly(::testing::Return(fileSize));
    EXPECT_CALL(*osFile, mmap).WillRepeatedly(::testing::Return(mmapPtr.get()));

    EXPECT_CALL(osInfc, osiOpenWithSharedLock).WillOnce(::testing::Return(std::move(osFile)));

    ze_graph_desc_2_t desc = {};
    auto key = cache->computeKey(desc);
    auto blob = cache->getBlob(key);
    EXPECT_NE(blob, nullptr);
    EXPECT_EQ(blob->ptr, mmapPtr.get());
    EXPECT_EQ(blob->size, fileSize - HashCity::DigestLength);
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
