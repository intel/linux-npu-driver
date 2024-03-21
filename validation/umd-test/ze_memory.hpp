/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "ze_scope.hpp"
#include <level_zero/ze_api.h>

namespace zeMemory {

inline void allocError(const char *msg) {
#ifdef GTEST_TEST
    ASSERT_FALSE(true) << msg;
#else
    throw std::runtime_error(msg);
#endif
}

inline std::shared_ptr<void> allocShared(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         uint32_t size,
                                         ze_host_mem_alloc_flags_t flags = 0) {
    ze_device_mem_alloc_desc_t devDesc = {.stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
                                          .pNext = nullptr,
                                          .flags = 0,
                                          .ordinal = 0};
    ze_host_mem_alloc_desc_t hostDesc = {.stype = ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC,
                                         .pNext = nullptr,
                                         .flags = flags};
    ze_result_t ret;
    auto scopedMem = zeScope::memAllocShared(hContext, devDesc, hostDesc, size, 0, hDevice, ret);
    if (ret != ZE_RESULT_SUCCESS)
        allocError("Failed to allocate shared memory");
    memset(scopedMem.get(), 0, size);
    return scopedMem;
}

inline std::shared_ptr<void>
allocDevice(ze_context_handle_t hContext, ze_device_handle_t hDevice, size_t size) {
    ze_device_mem_alloc_desc_t desc = {.stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC,
                                       .pNext = nullptr,
                                       .flags = 0,
                                       .ordinal = 0};
    ze_result_t ret;
    auto scopedMem = zeScope::memAllocDevice(hContext, desc, size, 0, hDevice, ret);
    if (ret != ZE_RESULT_SUCCESS)
        allocError("Failed to allocate device memory");
    return scopedMem;
}

inline std::shared_ptr<void>
allocHost(ze_context_handle_t hContext, size_t size, ze_host_mem_alloc_flags_t flags = 0) {
    ze_host_mem_alloc_desc_t desc = {.stype = ZE_STRUCTURE_TYPE_HOST_MEM_ALLOC_DESC,
                                     .pNext = nullptr,
                                     .flags = flags};
    ze_result_t ret;
    auto scopedMem = zeScope::memAllocHost(hContext, desc, size, 0, ret);
    if (ret != ZE_RESULT_SUCCESS)
        allocError("Failed to allocate host memory");
    memset(scopedMem.get(), 0, size);
    return scopedMem;
}
}; // namespace zeMemory
