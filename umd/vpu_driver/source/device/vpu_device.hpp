/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/metric_info.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"

#include <array>
#include <memory>
#include <string>
#include <vector>

namespace VPU {
class OsInterface;
class VPUDriverApi;

enum class EngineType { COMPUTE = 0, COPY, INVALID, ENGINE_MAX = INVALID };

class VPUDevice {
  public:
    bool init(bool enableMetrics);

    VPUDevice(std::string devPath, OsInterface &osInfc);
    virtual ~VPUDevice() = default;

    const VPUHwInfo &getHwInfo() const;
    const std::vector<GroupInfo> getMetricGroupsInfo() const;
    bool getCapMetricStreamer() const;
    virtual std::unique_ptr<VPUDeviceContext> createDeviceContext();

    size_t getNumberOfEngineGroups(void) const;
    size_t getEngineMaxMemoryFillSize();
    EngineType getEngineType(uint32_t engGrpIdx);
    EngineType getEngineTypeFromOrdinal(uint32_t engGrpOrdinal, bool &isCopyOnly);
    bool engineSupportCompute(EngineType engineType) const;
    bool engineSupportCopy(EngineType engineType) const { return true; }
    bool engineSupportCooperativeKernel(EngineType engineType) const { return false; }
    bool engineSupportMetrics(EngineType engineType) const { return false; }
    int getBDF(uint32_t *domain, uint32_t *bus, uint32_t *dev, uint32_t *func);

    /**
     * Return device's connection status.
     */
    bool isConnected();

    std::string jsmApiVersion;
    std::string mappedInferenceVersion;

  private:
    virtual bool initializeCaps(VPUDriverApi *drvApi);
    virtual bool initializeMetricGroups(VPUDriverApi *drvApi);

  protected:
    VPUHwInfo hwInfo = {};
    std::vector<GroupInfo> groupsInfo = {};

    std::string devPath;
    OsInterface &osInfc;
    static constexpr std::array<EngineType, 2> engineGroups = {EngineType::COMPUTE,
                                                               EngineType::COPY};
};

} // namespace VPU
