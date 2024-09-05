/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include <bits/chrono.h>

#pragma once

#include <stdint.h>

#include "level_zero_driver/core/source/fence/fence.hpp" // IWYU pragma: keep
#include "level_zero_driver/include/l0_handler.hpp"

#include <chrono> // IWYU pragma: keep
#include <level_zero/ze_api.h>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

namespace L0 {
struct Context;
struct Device;
} // namespace L0

namespace VPU {
class VPUJob;
} // namespace VPU

struct _ze_command_queue_handle_t {};

namespace L0 {

struct CommandQueue : _ze_command_queue_handle_t, IContextObject {
    CommandQueue(Context *context,
                 Device *device,
                 bool isCopyOnly,
                 ze_command_queue_priority_t priority);
    ~CommandQueue() = default;

    static ze_result_t create(ze_context_handle_t hContext,
                              ze_device_handle_t hDevice,
                              const ze_command_queue_desc_t *desc,
                              ze_command_queue_handle_t *phCommandQueue);

    inline ze_command_queue_handle_t toHandle() { return this; }
    static CommandQueue *fromHandle(ze_command_queue_handle_t handle) {
        return static_cast<CommandQueue *>(handle);
    }

    ze_result_t createFence(const ze_fence_desc_t *desc, ze_fence_handle_t *phFence);
    ze_result_t destroy();
    ze_result_t executeCommandLists(uint32_t nCommandLists,
                                    ze_command_list_handle_t *phCommandLists,
                                    ze_fence_handle_t hFence);
    ze_result_t synchronize(uint64_t timeout);

    void destroyFence(Fence *pFence);
    ze_result_t waitForJobs(std::chrono::steady_clock::time_point timeout,
                            const std::vector<std::shared_ptr<VPU::VPUJob>> &jobs);
    bool isCopyOnly() { return isCopyOnlyCommandQueue; }

  protected:
    Context *pContext = nullptr;
    Device *device = nullptr;
    bool isCopyOnlyCommandQueue = false;
    ze_command_queue_priority_t priority;
    std::vector<std::shared_ptr<VPU::VPUJob>> trackedJobs;

    std::shared_mutex fenceMutex;
    std::unordered_map<Fence *, std::unique_ptr<Fence>> fences;
};

} // namespace L0
