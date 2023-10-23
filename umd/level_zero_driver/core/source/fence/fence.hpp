/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "level_zero_driver/core/source/cmdqueue/cmdqueue.hpp"

#include <level_zero/ze_api.h>

struct _ze_fence_handle_t {};

namespace L0 {

struct Fence : _ze_fence_handle_t, IContextObject {
    Fence(Context *pContext, const ze_fence_desc_t *desc);
    ~Fence() = default;

    ze_result_t destroy();
    ze_result_t hostSynchronize(uint64_t timeout);
    ze_result_t queryStatus() { return hostSynchronize(0); }
    ze_result_t reset();

    static Fence *fromHandle(ze_fence_handle_t handle) { return static_cast<Fence *>(handle); }
    inline ze_fence_handle_t toHandle() { return this; }

    /**
     * @brief Copies submitted VPUJob vector for synchronization.
     *
     * @param jobs [in]: Submitted jobs
     */
    void setTrackedJobs(std::vector<std::shared_ptr<VPU::VPUJob>> &jobs);

    /**
     * @brief Get number of kept sync jobs
     *
     * @return uint32_t
     */
    size_t getTrackedJobCount() const { return trackedJobs.size(); }

  protected:
    Context *pContext = nullptr;
    bool signaled = false;
    std::vector<std::shared_ptr<VPU::VPUJob>> trackedJobs;
};

} // namespace L0
