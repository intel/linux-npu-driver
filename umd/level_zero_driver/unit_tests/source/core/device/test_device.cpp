/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stdint.h>

#include "gtest/gtest.h"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/unit_tests/mocks/mock_os_interface_imp.hpp"
#include "vpu_driver/unit_tests/mocks/mock_vpu_device.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <level_zero/ze_api.h>
#include <level_zero/ze_intel_npu_uuid.h>
#include <string.h>
#include <string>

namespace L0 {
namespace ult {

TEST(DeviceCap, capabilitiesShouldBeInitializedWithDeviceInitialization) {
    VPU::MockOsInterfaceImp mockOSInfc;
    VPU::MockVPUDevice vpuDevice(FAKE_TEST_DEV_NODE, mockOSInfc);
    EXPECT_TRUE(vpuDevice.init(true));
}

using SingleDeviceTest = Test<DeviceFixture>;

TEST_F(SingleDeviceTest, deviceConnectionStatusReturnned) {
    // Assume device connected intially.
    EXPECT_EQ(ZE_RESULT_SUCCESS, device->getStatus());

    // Device disconnected.
    mockVpuDevice->mockGetOsInterface().deviceConnected = false;
    EXPECT_EQ(ZE_RESULT_ERROR_DEVICE_LOST, device->getStatus());

    // Device connected.
    mockVpuDevice->mockGetOsInterface().deviceConnected = true;
    EXPECT_EQ(ZE_RESULT_SUCCESS, device->getStatus());
}

TEST_F(SingleDeviceTest, checkVPUSupportingMemoryAccess) {
    ze_device_memory_access_properties_t memProps;
    auto result = device->getMemoryAccessProperties(&memProps);

    // Should success.
    EXPECT_EQ(ZE_RESULT_SUCCESS, result);

    // Device allocation is supported.
    EXPECT_EQ((uint32_t)3, memProps.deviceAllocCapabilities);

    // Shared single device allocation is not supported.
    EXPECT_EQ((uint32_t)0, memProps.sharedSingleDeviceAllocCapabilities);

    // Shared cross device allocation is not supported.
    EXPECT_EQ((uint32_t)0, memProps.sharedCrossDeviceAllocCapabilities);

    // Host allocation is supported.
    // - read write
    // - atomic
    auto hostAllocCap = memProps.hostAllocCapabilities;
    EXPECT_EQ((uint32_t)ZE_MEMORY_ACCESS_CAP_FLAG_RW,
              (ZE_MEMORY_ACCESS_CAP_FLAG_RW & hostAllocCap));
    EXPECT_EQ((uint32_t)ZE_MEMORY_ACCESS_CAP_FLAG_ATOMIC,
              (ZE_MEMORY_ACCESS_CAP_FLAG_ATOMIC & hostAllocCap));
    EXPECT_EQ((uint32_t)0, (ZE_MEMORY_ACCESS_CAP_FLAG_CONCURRENT & hostAllocCap));
    EXPECT_EQ((uint32_t)0, (ZE_MEMORY_ACCESS_CAP_FLAG_CONCURRENT_ATOMIC & hostAllocCap));

    // Shared system allocation supported.
    auto sharedSysAllocCap = memProps.sharedSystemAllocCapabilities;
    EXPECT_EQ((uint32_t)0, (ZE_MEMORY_ACCESS_CAP_FLAG_RW & sharedSysAllocCap));
    EXPECT_EQ((uint32_t)0, (ZE_MEMORY_ACCESS_CAP_FLAG_ATOMIC & sharedSysAllocCap));
    EXPECT_EQ((uint32_t)0, (ZE_MEMORY_ACCESS_CAP_FLAG_CONCURRENT & sharedSysAllocCap));
    EXPECT_EQ((uint32_t)0, (ZE_MEMORY_ACCESS_CAP_FLAG_CONCURRENT_ATOMIC & sharedSysAllocCap));
}

TEST_F(SingleDeviceTest, givenCallToDevicePropertiesThenBasicPropertiesCorrectlyReturned) {
    ze_device_properties_t l0DevProps = {};
    l0DevProps.stype = ZE_STRUCTURE_TYPE_DEVICE_PROPERTIES;

    device->getProperties(&l0DevProps);

    auto hwInfo = mockVpuDevice->getHwInfo();

    // Device type.
    EXPECT_EQ(ZE_DEVICE_TYPE_VPU, l0DevProps.type);

    // Vendor ID.
    EXPECT_EQ(l0DevProps.vendorId, INTEL_PCI_VENDOR_ID);

    // Device ID.
    EXPECT_EQ(l0DevProps.deviceId, hwInfo.deviceId);

    // Sub device ID.
    EXPECT_EQ(l0DevProps.subdeviceId, hwInfo.deviceRevision);

    // Core clock rate.
    EXPECT_EQ(l0DevProps.coreClockRate, hwInfo.coreClockRate);

    // Max mem alloc size.
    EXPECT_GT(l0DevProps.maxMemAllocSize, 0);

    // Max hardware contexts.
    EXPECT_EQ(l0DevProps.maxHardwareContexts, hwInfo.maxHardwareContexts);

    // Max command queue priority.
    EXPECT_EQ(l0DevProps.maxCommandQueuePriority, hwInfo.maxCommandQueuePriority);

    // Number of threads per EU.
    EXPECT_EQ(l0DevProps.numThreadsPerEU, hwInfo.numThreadsPerEU);

    // Physical EU SIMD width.
    EXPECT_EQ(l0DevProps.physicalEUSimdWidth, hwInfo.physicalEUSimdWidth);

    // Number of EUs per sub-slice
    EXPECT_EQ(l0DevProps.numEUsPerSubslice, hwInfo.nExecUnits);

    // Number of sub-slices per slice.
    EXPECT_EQ(l0DevProps.numSubslicesPerSlice, hwInfo.numSubslicesPerSlice);

    // Number of slices.
    EXPECT_EQ(l0DevProps.numSlices, 2u);

    const int NS_IN_SEC = 1'000'000'000;
    // Resolution of device timer in nanoseconds used for profiling, timestamps, etc.
    EXPECT_EQ(l0DevProps.timerResolution, NS_IN_SEC / 38'400'000);

    // Number of valid bits in the timestamp values.
    EXPECT_EQ(l0DevProps.timestampValidBits, 64u);

    // Number of valid bits in the kernel timestamp values.
    EXPECT_EQ(l0DevProps.kernelTimestampValidBits, 0u);

    // Device name.
    EXPECT_STREQ(l0DevProps.name, hwInfo.name);

    // Device UUID.
    ze_device_uuid_t uuid = ze_intel_npu_device_uuid;
    EXPECT_EQ(memcmp(&l0DevProps.uuid, &uuid, sizeof(l0DevProps.uuid)), 0);
}

TEST_F(SingleDeviceTest, givenCallToGetDeviceMemoryPropertiesExpectedValuesReturned) {
    uint32_t count = 0;

    // Checking count NULL value
    EXPECT_EQ(device->getMemoryProperties(nullptr, nullptr), ZE_RESULT_ERROR_INVALID_NULL_POINTER);

    // Checking count returns correct number of memory properties with count == 0
    EXPECT_EQ(device->getMemoryProperties(&count, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_EQ(count, 1u);

    // Checking count returns correct number of memory properties with count greater than returned
    // value
    count = count + 1;
    EXPECT_EQ(device->getMemoryProperties(&count, nullptr), ZE_RESULT_SUCCESS);
    EXPECT_EQ(count, 1u);

    auto hwInfo = mockVpuDevice->getHwInfo();
    ze_device_memory_properties_t *memProperties = new ze_device_memory_properties_t[count];
    EXPECT_EQ(device->getMemoryProperties(&count, memProperties), ZE_RESULT_SUCCESS);

    // Checking returned values
    EXPECT_EQ(memProperties->flags, 0u);
    EXPECT_EQ(memProperties->maxClockRate, hwInfo.coreClockRate);
    EXPECT_EQ(memProperties->maxBusWidth, 0);
    EXPECT_EQ(memProperties->totalSize, 0);
    EXPECT_EQ(strncmp(memProperties->name,
                      device->getDeviceMemoryName(),
                      strlen(device->getDeviceMemoryName())),
              0);

    delete[] memProperties;
}

} // namespace ult
} // namespace L0
