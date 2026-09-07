/*
 * Copyright (C) 2022-2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include <bits/chrono.h>

#pragma once

#include <stdint.h>

#include "fence.hpp" // IWYU pragma: keep
#include "level_zero_driver/api/zet_misc.hpp"
#include "level_zero_driver/include/l0_handler.hpp"

#include <chrono> // IWYU pragma: keep
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>
#include <ze_api.h>
#include <ze_command_queue_npu_ext.h>

struct _ze_command_queue_handle_t : public L0::_ze_generic_handle_t {};
namespace VPU {
class VPUJob;
class VPUBufferObject;
class VPUDeviceQueue;
} // namespace VPU

namespace L0 {
struct Context;

struct CommandQueue : _ze_command_queue_handle_t, IContextObject {
    CommandQueue(Context *context,
                 std::unique_ptr<VPU::VPUDeviceQueue> queue,
                 uint32_t ordinal,
                 uint32_t index,
                 ze_command_queue_flags_t flags,
                 ze_command_queue_mode_t mode,
                 ze_command_queue_priority_t priority);
    ~CommandQueue() override;

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
    ze_result_t getOrdinal(uint32_t *pOrdinal) const;
    ze_result_t getIndex(uint32_t *pIndex) const;
    ze_result_t getFlags(ze_command_queue_flags_t *pFlags) const;
    ze_result_t getMode(ze_command_queue_mode_t *pMode) const;
    ze_result_t getPriority(ze_command_queue_priority_t *pPriority) const;

    void destroyFence(Fence *pFence);
    ze_result_t waitForJobs(std::chrono::steady_clock::time_point timeout,
                            const std::vector<std::shared_ptr<VPU::VPUJob>> &jobs);
    ze_result_t setWorkloadType(ze_command_queue_workload_type_t workloadType);

  protected:
    std::unique_ptr<VPU::VPUDeviceQueue> vpuQueue;
    Context *pContext = nullptr;
    uint32_t ordinal = 0;
    uint32_t index = 0;
    ze_command_queue_flags_t flags = 0;
    ze_command_queue_mode_t mode = ZE_COMMAND_QUEUE_MODE_DEFAULT;
    ze_command_queue_priority_t priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL;

    std::vector<std::shared_ptr<VPU::VPUJob>> trackedJobs;
    std::shared_mutex fenceMutex;
    std::unordered_map<Fence *, std::unique_ptr<Fence>> fences;
    bool isSynchronousMode = false;

    std::mutex preemptionMutex;
    std::shared_ptr<VPU::VPUBufferObject> preemptionBuffer = nullptr;
};

} // namespace L0
