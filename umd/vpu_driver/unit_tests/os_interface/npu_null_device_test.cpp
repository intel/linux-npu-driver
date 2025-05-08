/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "gtest/gtest.h"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/vpu_37xx/vpu_hw_37xx.hpp"
#include "vpu_driver/source/device/vpu_40xx/vpu_hw_40xx.hpp"
#include "vpu_driver/source/os_interface/null_interface_imp.hpp"
#include "vpu_driver/source/os_interface/os_interface.hpp"

#include <bitset>
#include <stdlib.h>
#include <string>
#include <uapi/drm/ivpu_accel.h>
#include <utility>
#include <vector>

using namespace VPU;

const std::vector<std::pair<std::string, uint64_t>> testedPlatforms = {
    {"INPU_LNL", PCI_DEVICE_ID_LNL},
    {"LUNARLAKE", PCI_DEVICE_ID_LNL},
    {"INPU_MTL", PCI_DEVICE_ID_MTL},
    {"METEORLAKE", PCI_DEVICE_ID_MTL},
    {"ARROWLAKE", PCI_DEVICE_ID_ARL}};

struct NPUNullDeviceTest : public ::testing::Test {
    void TearDown() override {
        unsetEnv("ZE_INTEL_NPU_PLATFORM_OVERRIDE");
        unsetEnv("ZE_INTEL_NPU_REVISION_OVERRIDE");
        unsetEnv("ZE_INTEL_NPU_DISABLED_TILE_OVERRIDE");
        unsetEnv("ZE_INTEL_NPU_TILE_COUNT_OVERRIDE");
    }

    void setEnv(std::string key, std::string value) {
        ASSERT_EQ(setenv(key.c_str(), value.c_str(), 1), 0);
    }

    void unsetEnv(std::string key) { unsetenv(key.c_str()); }

    uint64_t getParam(uint32_t paramName) {
        auto devIf = NullOsInterfaceImp::getInstance();
        drm_ivpu_param param = {};
        param.param = paramName;
        EXPECT_EQ(devIf->osiIoctl(0, DRM_IOCTL_IVPU_GET_PARAM, &param), 0);
        return param.value;
    }
};

TEST_F(NPUNullDeviceTest, checkPlatformInitialization) {
    for (auto &platform : testedPlatforms) {
        setEnv("ZE_INTEL_NPU_PLATFORM_OVERRIDE", platform.first);
        ASSERT_TRUE(NullOsInterfaceImp::isNullDeviceRequested());
        ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
        ASSERT_EQ(getParam(DRM_IVPU_PARAM_DEVICE_ID), platform.second);
        ASSERT_EQ(getParam(DRM_IVPU_PARAM_DEVICE_REVISION), UINT16_MAX);
        ASSERT_EQ(getParam(DRM_IVPU_PARAM_TILE_CONFIG), 0x0);
    }
    /* check unsupported platform name */
    setEnv("ZE_INTEL_NPU_PLATFORM_OVERRIDE", "ANY_PLATFORM");
    ASSERT_TRUE(NullOsInterfaceImp::isNullDeviceRequested());
    ASSERT_FALSE(NullOsInterfaceImp::configureNullDevice());
}

TEST_F(NPUNullDeviceTest, checkRevisionInitialization) {
    setEnv("ZE_INTEL_NPU_PLATFORM_OVERRIDE", testedPlatforms[0].first);
    ASSERT_TRUE(NullOsInterfaceImp::isNullDeviceRequested());

    /* Default value is expected 0xFFFF */
    ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
    ASSERT_EQ(getParam(DRM_IVPU_PARAM_DEVICE_REVISION), UINT16_MAX);

    setEnv("ZE_INTEL_NPU_REVISION_OVERRIDE", "0x10");
    ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
    ASSERT_EQ(getParam(DRM_IVPU_PARAM_DEVICE_REVISION), 0x10);

    setEnv("ZE_INTEL_NPU_REVISION_OVERRIDE", "10");
    ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
    ASSERT_EQ(getParam(DRM_IVPU_PARAM_DEVICE_REVISION), 0xA);

    setEnv("ZE_INTEL_NPU_REVISION_OVERRIDE", "010");
    ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
    ASSERT_EQ(getParam(DRM_IVPU_PARAM_DEVICE_REVISION), 0x8);

    /* Check unsupported revision name */
    setEnv("ZE_INTEL_NPU_REVISION_OVERRIDE", "ANY");
    ASSERT_FALSE(NullOsInterfaceImp::configureNullDevice());
}

TEST_F(NPUNullDeviceTest, checkTileMaskInitialization) {
    setEnv("ZE_INTEL_NPU_PLATFORM_OVERRIDE", testedPlatforms[0].first);
    ASSERT_TRUE(NullOsInterfaceImp::isNullDeviceRequested());

    /* Default value is expected 0x0 - all tiles enabled*/
    ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
    ASSERT_EQ(getParam(DRM_IVPU_PARAM_TILE_CONFIG), 0x0);

    setEnv("ZE_INTEL_NPU_DISABLED_TILE_OVERRIDE", "0x11");
    ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
    ASSERT_EQ(getParam(DRM_IVPU_PARAM_TILE_CONFIG), 0x11);

    setEnv("ZE_INTEL_NPU_DISABLED_TILE_OVERRIDE", "11");
    ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
    ASSERT_EQ(getParam(DRM_IVPU_PARAM_TILE_CONFIG), 0xb);

    setEnv("ZE_INTEL_NPU_DISABLED_TILE_OVERRIDE", "011");
    ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
    ASSERT_EQ(getParam(DRM_IVPU_PARAM_TILE_CONFIG), 0x9);

    setEnv("ZE_INTEL_NPU_DISABLED_TILE_OVERRIDE", "b11");
    ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
    ASSERT_EQ(getParam(DRM_IVPU_PARAM_TILE_CONFIG), 0x3);

    /* Check unsupported mask */
    setEnv("ZE_INTEL_NPU_DISABLED_TILE_OVERRIDE", "ANY");
    ASSERT_FALSE(NullOsInterfaceImp::configureNullDevice());
}

TEST_F(NPUNullDeviceTest, checkTileNumberInitialization) {
    for (auto &platform : testedPlatforms) {
        setEnv("ZE_INTEL_NPU_PLATFORM_OVERRIDE", platform.first);
        ASSERT_TRUE(NullOsInterfaceImp::isNullDeviceRequested());

        auto platformHwInfo = getHwInfoByDeviceId(static_cast<uint32_t>(platform.second));
        std::bitset<32> maxTiles(platformHwInfo.tileFuseMask);

        /* Tiles enabled: 1 - max */
        std::bitset<64> disabledTileMask;
        for (size_t tileCount = 1; tileCount <= maxTiles.count(); tileCount++) {
            setEnv("ZE_INTEL_NPU_TILE_COUNT_OVERRIDE", std::to_string(tileCount));
            ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());

            disabledTileMask = getParam(DRM_IVPU_PARAM_TILE_CONFIG);
            ASSERT_EQ(disabledTileMask.count(), maxTiles.count() - tileCount);
        }

        /*Check formats, enable single tile in hex, dec, oct*/
        setEnv("ZE_INTEL_NPU_TILE_COUNT_OVERRIDE", "0x1");
        ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
        disabledTileMask = getParam(DRM_IVPU_PARAM_TILE_CONFIG);
        ASSERT_EQ(disabledTileMask.count(), maxTiles.count() - 1);

        setEnv("ZE_INTEL_NPU_TILE_COUNT_OVERRIDE", "1");
        ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
        disabledTileMask = getParam(DRM_IVPU_PARAM_TILE_CONFIG);
        ASSERT_EQ(disabledTileMask.count(), maxTiles.count() - 1);

        setEnv("ZE_INTEL_NPU_TILE_COUNT_OVERRIDE", "01");
        ASSERT_TRUE(NullOsInterfaceImp::configureNullDevice());
        disabledTileMask = getParam(DRM_IVPU_PARAM_TILE_CONFIG);
        ASSERT_EQ(disabledTileMask.count(), maxTiles.count() - 1);

        /*All tiles disabled and enabeled more than max tiles - illegal cases*/
        setEnv("ZE_INTEL_NPU_TILE_COUNT_OVERRIDE", std::to_string(0));
        ASSERT_FALSE(NullOsInterfaceImp::configureNullDevice());
        setEnv("ZE_INTEL_NPU_TILE_COUNT_OVERRIDE", std::to_string(maxTiles.count() + 1));
        ASSERT_FALSE(NullOsInterfaceImp::configureNullDevice());
    }
}
