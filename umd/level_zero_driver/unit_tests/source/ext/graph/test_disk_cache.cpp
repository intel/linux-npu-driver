/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "level_zero_driver/ext/source/graph/disk_cache.hpp"
#include "vpu_driver/unit_tests/mocks/gmock_os_interface_imp.hpp"

#include <level_zero/ze_graph_ext.h>
#include <memory>
#include <string>

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
    EXPECT_EQ(diskCache.getBlob(""), DiskCache::Blob());
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
    EXPECT_EQ(cache->getBlob(key).size(), 0);
}

} // namespace L0
