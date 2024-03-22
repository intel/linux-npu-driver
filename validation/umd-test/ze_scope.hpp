/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <level_zero/ze_api.h>
#include <level_zero/zet_api.h>
#include <level_zero/ze_graph_ext.h>
#include <level_zero/ze_graph_profiling_ext.h>

#include "umd_extensions.h"

#ifdef VPU_GTEST_APP
#include <gtest/gtest.h>
#else
#define EXPECT_EQ(lhs, rhs)                                              \
    if (lhs != rhs) {                                                    \
        fprintf(stderr, "%s:%d: %s failed\n", __FILE__, __LINE__, #lhs); \
    }
#endif

#include <memory>

namespace zeScope {

template <class T>
using SharedPtr = std::shared_ptr<std::remove_pointer_t<T>>;

inline SharedPtr<ze_context_handle_t>
contextCreate(ze_driver_handle_t drv, ze_context_desc_t &desc, ze_result_t &ret) {
    ze_context_handle_t handle = nullptr;
    ret = zeContextCreate(drv, &desc, &handle);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<ze_context_handle_t>(std::move(handle), [](auto x) {
        EXPECT_EQ(zeContextDestroy(x), ZE_RESULT_SUCCESS);
    });
}

inline SharedPtr<void> memAllocHost(ze_context_handle_t ctx,
                                    ze_host_mem_alloc_desc_t &hostDesc,
                                    size_t size,
                                    size_t alignment,
                                    ze_result_t &ret) {
    void *ptr = nullptr;
    ret = zeMemAllocHost(ctx, &hostDesc, size, alignment, &ptr);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<void>(std::move(ptr),
                           [ctx](void *ptr) { EXPECT_EQ(zeMemFree(ctx, ptr), ZE_RESULT_SUCCESS); });
}

inline SharedPtr<void> memAllocShared(ze_context_handle_t ctx,
                                      ze_device_mem_alloc_desc_t &devDesc,
                                      ze_host_mem_alloc_desc_t &hostDesc,
                                      size_t size,
                                      size_t alignment,
                                      ze_device_handle_t dev,
                                      ze_result_t &ret) {
    void *ptr = nullptr;
    ret = zeMemAllocShared(ctx, &devDesc, &hostDesc, size, alignment, dev, &ptr);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<void>(std::move(ptr),
                           [ctx](void *ptr) { EXPECT_EQ(zeMemFree(ctx, ptr), ZE_RESULT_SUCCESS); });
}

inline SharedPtr<void> memAllocDevice(ze_context_handle_t ctx,
                                      ze_device_mem_alloc_desc_t &devDesc,
                                      size_t size,
                                      size_t alignment,
                                      ze_device_handle_t dev,
                                      ze_result_t &ret) {
    void *ptr = nullptr;
    ret = zeMemAllocDevice(ctx, &devDesc, size, alignment, dev, &ptr);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<void>(std::move(ptr),
                           [ctx](void *ptr) { EXPECT_EQ(zeMemFree(ctx, ptr), ZE_RESULT_SUCCESS); });
}

inline SharedPtr<ze_command_queue_handle_t> commandQueueCreate(ze_context_handle_t ctx,
                                                               ze_device_handle_t dev,
                                                               const ze_command_queue_desc_t &desc,
                                                               ze_result_t &ret) {
    ze_command_queue_handle_t handle = nullptr;
    ret = zeCommandQueueCreate(ctx, dev, &desc, &handle);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<ze_command_queue_handle_t>(std::move(handle), [](auto x) {
        EXPECT_EQ(zeCommandQueueDestroy(x), ZE_RESULT_SUCCESS);
    });
}

inline SharedPtr<ze_command_list_handle_t> commandListCreate(ze_context_handle_t ctx,
                                                             ze_device_handle_t dev,
                                                             const ze_command_list_desc_t &desc,
                                                             ze_result_t &ret) {
    ze_command_list_handle_t handle = nullptr;
    ret = zeCommandListCreate(ctx, dev, &desc, &handle);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<ze_command_list_handle_t>(std::move(handle), [](auto x) {
        EXPECT_EQ(zeCommandListDestroy(x), ZE_RESULT_SUCCESS);
    });
}

inline SharedPtr<ze_graph_handle_t> graphCreate(graph_dditable_ext_t *ddi,
                                                ze_context_handle_t ctx,
                                                ze_device_handle_t dev,
                                                const ze_graph_desc_t &desc,
                                                ze_result_t &ret) {
    ze_graph_handle_t handle = nullptr;

    if (!ddi) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        return nullptr;
    }

    ret = ddi->pfnCreate(ctx, dev, &desc, &handle);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<ze_graph_handle_t>(std::move(handle), [ddi](auto x) {
        EXPECT_EQ(ddi->pfnDestroy(x), ZE_RESULT_SUCCESS);
    });
}

inline SharedPtr<ze_graph_handle_t> graphCreate2(graph_dditable_ext_t *ddi,
                                                 ze_context_handle_t ctx,
                                                 ze_device_handle_t dev,
                                                 const ze_graph_desc_2_t &desc,
                                                 ze_result_t &ret) {
    ze_graph_handle_t handle = nullptr;

    if (!ddi) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        return nullptr;
    }

    ret = ddi->pfnCreate2(ctx, dev, &desc, &handle);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<ze_graph_handle_t>(std::move(handle), [ddi](auto x) {
        EXPECT_EQ(ddi->pfnDestroy(x), ZE_RESULT_SUCCESS);
    });
}

inline SharedPtr<ze_fence_handle_t>
fenceCreate(ze_command_queue_handle_t queue, const ze_fence_desc_t &desc, ze_result_t &ret) {
    ze_fence_handle_t handle = nullptr;
    ret = zeFenceCreate(queue, &desc, &handle);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<ze_fence_handle_t>(std::move(handle), [](auto x) {
        EXPECT_EQ(zeFenceDestroy(x), ZE_RESULT_SUCCESS);
    });
}

inline SharedPtr<ze_event_pool_handle_t> eventPoolCreate(ze_context_handle_t ctx,
                                                         const ze_event_pool_desc_t &desc,
                                                         uint32_t numDevices,
                                                         ze_device_handle_t &phDevices,
                                                         ze_result_t &ret) {
    ze_event_pool_handle_t handle = nullptr;
    ret = zeEventPoolCreate(ctx, &desc, numDevices, &phDevices, &handle);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<ze_event_pool_handle_t>(std::move(handle), [](auto x) {
        EXPECT_EQ(zeEventPoolDestroy(x), ZE_RESULT_SUCCESS);
    });
}

inline SharedPtr<ze_event_handle_t>
eventCreate(ze_event_pool_handle_t pool, const ze_event_desc_t &desc, ze_result_t &ret) {
    ze_event_handle_t handle = nullptr;
    ret = zeEventCreate(pool, &desc, &handle);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<ze_event_handle_t>(std::move(handle), [](auto x) {
        EXPECT_EQ(zeEventDestroy(x), ZE_RESULT_SUCCESS);
    });
}

inline SharedPtr<ze_graph_profiling_pool_handle_t>
profilingPoolCreate(ze_graph_profiling_dditable_ext_t *ddi,
                    ze_graph_handle_t graphHandle,
                    uint32_t poolSize,
                    ze_result_t &ret) {
    ze_graph_profiling_pool_handle_t handle = nullptr;

    if (!ddi) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        return nullptr;
    }

    ret = ddi->pfnProfilingPoolCreate(graphHandle, poolSize, &handle);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<ze_graph_profiling_pool_handle_t>(std::move(handle), [ddi](auto x) {
        EXPECT_EQ(ddi->pfnProfilingPoolDestroy(x), ZE_RESULT_SUCCESS);
    });
}

inline SharedPtr<ze_graph_profiling_query_handle_t>
profilingQueryCreate(ze_graph_profiling_dditable_ext_t *ddi,
                     ze_graph_profiling_pool_handle_t hProfilingPool,
                     uint32_t index,
                     ze_result_t &ret) {
    ze_graph_profiling_query_handle_t handle = nullptr;

    if (!ddi) {
        ret = ZE_RESULT_ERROR_INVALID_NULL_POINTER;
        return nullptr;
    }

    ret = ddi->pfnProfilingQueryCreate(hProfilingPool, index, &handle);
    if (ret != ZE_RESULT_SUCCESS)
        return nullptr;

    return SharedPtr<ze_graph_profiling_query_handle_t>(std::move(handle), [ddi](auto x) {
        EXPECT_EQ(ddi->pfnProfilingQueryDestroy(x), ZE_RESULT_SUCCESS);
    });
}

}; // namespace zeScope
