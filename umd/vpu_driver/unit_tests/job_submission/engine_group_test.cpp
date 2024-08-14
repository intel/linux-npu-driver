/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "gtest/gtest.h"
#include "vpu_driver/source/device/vpu_device.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"

#include <memory>
#include <string>

using namespace VPU;

struct EngineGroupTest : public ::testing::Test {
    void SetUp() { EXPECT_EQ(2u, vpuDevice->getNumberOfEngineGroups()); }

    MockOsInterfaceImp osInfc;
    std::unique_ptr<MockVPUDevice> vpuDevice =
        std::make_unique<MockVPUDevice>(FAKE_TEST_DEV_NODE, osInfc);
};

/*
 * L0 specification defines engine groups' capabilities in ze_command_queue_group_property_flag_t
 *
 * ///< Command queue group supports enqueing compute commands.
 * ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COMPUTE
 * ///< Command queue group supports enqueing copy commands.
 * ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COPY
 * ///< Command queue group supports cooperative kernels.
 * ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_COOPERATIVE_KERNELS
 * ///< Command queue groups supports metric streamers and queries.
 * ZE_COMMAND_QUEUE_GROUP_PROPERTY_FLAG_METRICS
 */
TEST_F(EngineGroupTest, engineGroupsHoldsCapabilities) {
    auto nnEngType = vpuDevice->getEngineType(0u);
    EXPECT_EQ(nnEngType, EngineType::COMPUTE);
    EXPECT_TRUE(vpuDevice->engineSupportCompute(nnEngType));
    EXPECT_TRUE(vpuDevice->engineSupportCopy(nnEngType));
    EXPECT_FALSE(vpuDevice->engineSupportCooperativeKernel(nnEngType));
    EXPECT_FALSE(vpuDevice->engineSupportMetrics(nnEngType));

    auto copyEngType = vpuDevice->getEngineType(1u);
    EXPECT_FALSE(vpuDevice->engineSupportCompute(copyEngType));
    EXPECT_TRUE(vpuDevice->engineSupportCopy(copyEngType));
    EXPECT_FALSE(vpuDevice->engineSupportCooperativeKernel(copyEngType));
    EXPECT_FALSE(vpuDevice->engineSupportMetrics(copyEngType));
}

TEST_F(EngineGroupTest, engineGroupsMaxMemFillSizeCheck) {
    EXPECT_EQ(sizeof(uint32_t), vpuDevice->getEngineMaxMemoryFillSize());
}
