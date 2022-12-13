/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/device/device.hpp"

#include <level_zero/ze_api.h>

struct _ze_command_queue_handle_t {};

namespace L0 {

struct CommandQueue : _ze_command_queue_handle_t {
    CommandQueue(Device *device,
                 const ze_command_queue_desc_t *desc,
                 VPU::VPUDeviceContext *ctx,
                 bool isCopyOnly)
        : device(device)
        , desc(*desc)
        , ctx(ctx)
        , isCopyOnlyCommandQueue(isCopyOnly) {}
    CommandQueue &operator=(const CommandQueue &) = delete;
    CommandQueue(const CommandQueue &rhs) = delete;
    ~CommandQueue() = default;

    static CommandQueue *
    create(Device *device, const ze_command_queue_desc_t *desc, VPU::VPUDeviceContext *ctx);

    inline ze_command_queue_handle_t toHandle() { return this; }
    static CommandQueue *fromHandle(ze_command_queue_handle_t handle) {
        return static_cast<CommandQueue *>(handle);
    }

    ze_result_t createFence(const ze_fence_desc_t *desc, ze_fence_handle_t *phFence);
    ze_result_t destroy();
    Device *getDevice() { return device; }
    ze_result_t executeCommandLists(uint32_t nCommandLists,
                                    ze_command_list_handle_t *phCommandLists,
                                    ze_fence_handle_t hFence);

    /**
     * @brief Busy wait until the command queue gets signalled.
     *
     * @param timeout [in]: Maximum waiting time. uint64_t::max() for unlimited waiting.
     * @return ze_result_t
     */
    ze_result_t synchronize(uint64_t timeout);

    /**
     * @brief Return the number of submitted command buffers for execution.
     */
    size_t getSubmittedJobCount() const { return trackedJobs.size(); }

  protected:
    Device *device = nullptr;
    const ze_command_queue_desc_t desc;
    VPU::VPUDeviceContext *ctx = nullptr;
    bool isCopyOnlyCommandQueue = false;
    std::vector<std::shared_ptr<VPU::VPUJob>> trackedJobs;
};

} // namespace L0
