/*
 * Copyright (C) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "vpu_driver/source/device/device_info.hpp"
#include "vpu_driver/source/os_interface/vpu_driver_api.hpp"
#include "vpu_driver/source/command/vpu_job.hpp"

#include <memory>
#include <uapi/drm/ivpu_drm.h>

namespace VPU {

enum class EngineType { COMPUTE = 0, COPY, INVALID, ENGINE_MAX = INVALID };

class VPUDevice {
  public:
    bool init();

    VPUDevice(std::string devnode, OsInterface &osInfc);
    virtual ~VPUDevice() = default;

    const DeviceInfo &getDeviceInfo() const;
    const std::vector<GroupInfo> getMetricGroupsInfo() const;
    virtual std::unique_ptr<VPUDeviceContext> createDeviceContext();

    size_t getNumberOfEngineGroups(void) const;
    size_t getEngineMaxMemoryFillSize(EngineType engineType);
    EngineType getEngineType(uint32_t engGrpIdx);
    EngineType getEngineTypeFromOrdinal(uint32_t engGrpOrdinal, bool &isCopyOnly);
    bool engineSupportCompute(EngineType engineType) const;
    bool engineSupportCopy(EngineType engineType) const { return true; }
    bool engineSupportCooperativeKernel(EngineType engineType) const { return false; }
    bool engineSupportMetrics(EngineType engineType) const { return false; }

    /**
     * Return device's connection status.
     */
    bool isConnected();

  private:
    virtual bool initializeCaps(VPUDriverApi *drvApi);

  protected:
    DeviceInfo deviceInfo = {};
    std::vector<uint8_t> metricGroupsInfo = {};

  private:
    std::string devnode;
    OsInterface &osInfc;
    static constexpr std::array<EngineType, 2> engineGroups = {EngineType::COMPUTE,
                                                               EngineType::COPY};
};

} // namespace VPU
