/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include <stdint.h>

#include "vpu_driver/source/device/hw_info.hpp"
#include "vpu_driver/source/device/metric_info.hpp"
#include "vpu_driver/source/device/vpu_device_context.hpp"

#include <memory>
#include <string>
#include <vector>

namespace VPU {
class OsInterface;
class VPUDriverApi;

class VPUDevice {
  public:
    bool init(bool enableMetrics);

    VPUDevice(std::string devPath, OsInterface &osInfc);
    virtual ~VPUDevice() = default;

    const VPUHwInfo &getHwInfo() const;
    const std::vector<GroupInfo> getMetricGroupsInfo() const;
    bool getCapMetricStreamer() const;
    virtual std::unique_ptr<VPUDeviceContext> createDeviceContext();

    int getBDF(uint32_t *domain, uint32_t *bus, uint32_t *dev, uint32_t *func);
    bool getActiveTime(uint64_t &activeTimeUs);

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
};

} // namespace VPU
