/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include <stddef.h>

#include "gtest/gtest.h"
#include "level_zero_driver/core/source/context/context.hpp"
#include "level_zero_driver/core/source/device/device.hpp"
#include "level_zero_driver/core/source/driver/driver_handle.hpp"
#include "level_zero_driver/unit_tests/fixtures/device_fixture.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/unit_tests/test_macros/test.hpp"

#include <level_zero/ze_api.h>
#include <memory>

namespace L0 {
namespace ult {

class ContextMemoryTest : public Test<ContextFixture> {
  public:
    size_t size = 10;
    size_t alignment = 1u;
    void *ptr = nullptr;

    size_t pSize = 0;
    void *basePtr = nullptr;

    ze_memory_allocation_properties_t pMemAllocProperties = {};
};

TEST_F(ContextMemoryTest,
       whenAllocatingHostAllocationFromContextWithWrongValuesThenReturnsFailure) {
    // Testing size
    size_t size = 0;
    size_t alignment = 1u;
    void *ptr = nullptr;
    ze_result_t result = ZE_RESULT_SUCCESS;

    ze_host_mem_alloc_desc_t hDesc = {};
    result = zeMemAllocHost(nullptr, &hDesc, size, alignment, &ptr);
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE, result);

    result = context->allocMemory(size,
                                  alignment,
                                  &ptr,
                                  VPU::VPUBufferObject::Location::Host,
                                  VPU::VPUBufferObject::Type::CachedShave);
    EXPECT_EQ(ZE_RESULT_ERROR_UNSUPPORTED_SIZE, result);

    // Testing alignment
    size = 10;
    alignment = 3u;

    result = context->allocMemory(size,
                                  alignment,
                                  &ptr,
                                  VPU::VPUBufferObject::Location::Host,
                                  VPU::VPUBufferObject::Type::CachedShave);
    EXPECT_EQ(ZE_RESULT_ERROR_UNSUPPORTED_ALIGNMENT, result);

    // Testing flags
    alignment = 1u;

    ze_device_mem_alloc_desc_t dDesc = {};
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_HANDLE,
              zeMemAllocDevice(nullptr,
                               &dDesc,
                               size,
                               alignment,
                               driverHandle->getPrimaryDevice()->toHandle(),
                               &ptr));
}

class ContextMemoryTestRange : public ContextMemoryTest {};

TEST_F(ContextMemoryTestRange, passNullPtrToGetMemAddressRangeExpectInvalidNullPointerError) {
    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              context->getMemAddressRange(nullptr, nullptr, nullptr));
}

TEST_F(ContextMemoryTestRange, passOutOfScopeMemoryToGetMemAddressRangeExpectNotAvailableError) {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              context->allocMemory(size,
                                   alignment,
                                   &ptr,
                                   VPU::VPUBufferObject::Location::Host,
                                   VPU::VPUBufferObject::Type::CachedShave));

    EXPECT_EQ(ZE_RESULT_ERROR_NOT_AVAILABLE,
              context->getMemAddressRange(static_cast<char *>(ptr) - 1, &basePtr, &pSize));
    EXPECT_NE(ptr, basePtr);
    EXPECT_NE(size, pSize);

    EXPECT_EQ(ZE_RESULT_ERROR_NOT_AVAILABLE,
              context->getMemAddressRange(static_cast<char *>(ptr) + size, &basePtr, &pSize));
    EXPECT_NE(ptr, basePtr);
    EXPECT_NE(size, pSize);

    context->freeMem(ptr);
}

TEST_F(ContextMemoryTestRange, passMemWithoutRangeToGetMemAddressRangeExpectSuccess) {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              context->allocMemory(size,
                                   alignment,
                                   &ptr,
                                   VPU::VPUBufferObject::Location::Host,
                                   VPU::VPUBufferObject::Type::CachedShave));

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              context->getMemAddressRange(static_cast<char *>(ptr) + size - 1, nullptr, nullptr));
    EXPECT_NE(ptr, basePtr);
    EXPECT_NE(size, pSize);

    context->freeMem(ptr);
}

TEST_F(ContextMemoryTestRange, passValidInputToGetMemAddressRangeExpectSuccess) {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              context->allocMemory(size,
                                   alignment,
                                   &ptr,
                                   VPU::VPUBufferObject::Location::Host,
                                   VPU::VPUBufferObject::Type::CachedShave));

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              context->getMemAddressRange(static_cast<char *>(ptr) + size - 1, &basePtr, &pSize));
    EXPECT_EQ(ptr, basePtr);
    EXPECT_EQ(size, pSize);

    context->freeMem(ptr);
}

class ContextMemoryTestProperties : public ContextMemoryTest {};

TEST_F(ContextMemoryTestProperties, passNullPtrToGetMemPropertiesExpectInvalidPointerError) {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              context->allocMemory(size,
                                   alignment,
                                   &ptr,
                                   VPU::VPUBufferObject::Location::Host,
                                   VPU::VPUBufferObject::Type::CachedShave));

    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              context->getMemAllocProperties(ptr, nullptr, nullptr));

    EXPECT_EQ(ZE_RESULT_ERROR_INVALID_NULL_POINTER,
              context->getMemAllocProperties(nullptr, &pMemAllocProperties, nullptr));

    context->freeMem(ptr);
}

TEST_F(ContextMemoryTestProperties,
       passOutOfScopeMememoryToGetMemPropertiesExpectNotAvailableError) {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              context->allocMemory(size,
                                   alignment,
                                   &ptr,
                                   VPU::VPUBufferObject::Location::Host,
                                   VPU::VPUBufferObject::Type::CachedShave));

    EXPECT_EQ(ZE_RESULT_ERROR_NOT_AVAILABLE,
              context->getMemAllocProperties(static_cast<char *>(ptr) - 1,
                                             &pMemAllocProperties,
                                             nullptr));
    EXPECT_EQ(ZE_RESULT_ERROR_NOT_AVAILABLE,
              context->getMemAllocProperties(static_cast<char *>(ptr) + size,
                                             &pMemAllocProperties,
                                             nullptr));

    context->freeMem(ptr);
}

TEST_F(ContextMemoryTestProperties, passHostMemoryAddressToGetMemPropertiesExpectSuccess) {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              context->allocMemory(size,
                                   alignment,
                                   &ptr,
                                   VPU::VPUBufferObject::Location::Host,
                                   VPU::VPUBufferObject::Type::CachedShave));

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              context->getMemAllocProperties(static_cast<char *>(ptr) + size - 1,
                                             &pMemAllocProperties,
                                             nullptr));
    EXPECT_EQ(pMemAllocProperties.stype, ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES);
    EXPECT_EQ(pMemAllocProperties.pNext, nullptr);
    EXPECT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_HOST);
    EXPECT_EQ(pMemAllocProperties.id, 0u);
    EXPECT_EQ(pMemAllocProperties.pageSize, size);

    context->freeMem(ptr);
}

TEST_F(ContextMemoryTestProperties, passDeviceMemoryAddressToGetMemPropertiesExpectSuccess) {
    EXPECT_EQ(ZE_RESULT_SUCCESS,
              context->allocMemory(size,
                                   alignment,
                                   &ptr,
                                   VPU::VPUBufferObject::Location::Device,
                                   VPU::VPUBufferObject::Type::WriteCombineShave));

    EXPECT_EQ(ZE_RESULT_SUCCESS,
              context->getMemAllocProperties(static_cast<char *>(ptr) + size - 1,
                                             &pMemAllocProperties,
                                             nullptr));
    EXPECT_EQ(pMemAllocProperties.stype, ZE_STRUCTURE_TYPE_MEMORY_ALLOCATION_PROPERTIES);
    EXPECT_EQ(pMemAllocProperties.pNext, nullptr);
    EXPECT_EQ(pMemAllocProperties.type, ZE_MEMORY_TYPE_DEVICE);
    EXPECT_EQ(pMemAllocProperties.id, 0u);
    EXPECT_EQ(pMemAllocProperties.pageSize, size);

    context->freeMem(ptr);
}

} // namespace ult
} // namespace L0
