/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

// IWYU pragma: no_include <bits/chrono.h>
// IWYU pragma: no_include <bits/types/struct_rusage.h>
// IWYU pragma: no_include <linux/sysinfo.h>
// IWYU pragma: no_include <sys/time.h>

#include "vpu_driver/source/utilities/stats.hpp"

#include "vpu_driver/source/utilities/log.hpp"

#include <chrono> // IWYU pragma: keep
#include <filesystem>
#include <sys/resource.h>
#include <sys/sysinfo.h>

MemoryStatistics &MemoryStatistics::get() {
    static MemoryStatistics m;
    return m;
}

void MemoryStatistics::enable(std::string_view statsPath) {
    if (statsPath.empty()) {
        return;
    }

    const std::filesystem::path statsFilePath(statsPath);
    if (!std::filesystem::exists(statsFilePath.parent_path()))
        std::filesystem::create_directories(statsFilePath.parent_path());

    statOut.open(statsFilePath.c_str(), std::ios_base::out | std::ios_base::app);
    if (!statOut.is_open()) {
        LOG_E("Can not open statistics file.");
        return;
    }

    enabled = true;
    statOut << "#Timestamp, "
            << "SysUsedRam, "
            << "SysUsedShared, "
            << "SysUsedSwap, "
            << "SysUsedHigh, "
            << "DrvUsedDevice, "
            << "DrvUsedHost, "
            << "DrvUsedShared, "
            << "DrvUsedInternal, "
            << "RSS[Kb], "
            << "UserTime[sec.usec], "
            << "SysTime[sec.usec]" << std::endl;
    snapshot();
    return;
}

bool MemoryStatistics::isEnabled() {
    return enabled;
}

void MemoryStatistics::inc(VPU::VPUBufferObject::Location loc, size_t size) {
    if (!enabled)
        return;

    std::lock_guard<std::mutex> lock(mtx);
    switch (loc) {
    case VPU::VPUBufferObject::Location::Internal:
        internalAllocSize += size;
        break;
    case VPU::VPUBufferObject::Location::Host:
    case VPU::VPUBufferObject::Location::ExternalHost:
        hostAllocSize += size;
        break;
    case VPU::VPUBufferObject::Location::Device:
    case VPU::VPUBufferObject::Location::ExternalDevice:
        deviceAllocSize += size;
        break;
    case VPU::VPUBufferObject::Location::Shared:
    case VPU::VPUBufferObject::Location::ExternalShared:
        sharedAllocSize += size;
        break;
    default:
        break;
    }
}

void MemoryStatistics::dec(VPU::VPUBufferObject::Location loc, size_t size) {
    if (!enabled)
        return;

    std::lock_guard<std::mutex> lock(mtx);
    switch (loc) {
    case VPU::VPUBufferObject::Location::Internal:
        internalAllocSize -= size;
        break;
    case VPU::VPUBufferObject::Location::Host:
    case VPU::VPUBufferObject::Location::ExternalHost:
        hostAllocSize -= size;
        break;
    case VPU::VPUBufferObject::Location::Device:
    case VPU::VPUBufferObject::Location::ExternalDevice:
        deviceAllocSize -= size;
        break;
    case VPU::VPUBufferObject::Location::Shared:
    case VPU::VPUBufferObject::Location::ExternalShared:
        sharedAllocSize -= size;
        break;
    default:
        break;
    }
}

void MemoryStatistics::snapshot() {
    if (!enabled)
        return;
    struct sysinfo sysStats = {};
    struct rusage procStats = {};

    auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
                         std::chrono::steady_clock::now().time_since_epoch())
                         .count();
    if (sysinfo(&sysStats) != 0 || getrusage(RUSAGE_SELF, &procStats) != 0) {
        LOG_W("Can not get statistic information from system");
        return;
    }

    std::lock_guard<std::mutex> lock(mtx);
    statOut << static_cast<float>(timestamp) / 1000000.f << ", ";
    statOut << ((sysStats.totalram - sysStats.freeram) * sysStats.mem_unit) << ", ";
    statOut << (sysStats.sharedram * sysStats.mem_unit) << ", ";
    statOut << ((sysStats.totalswap - sysStats.freeswap) * sysStats.mem_unit) << ", ";
    statOut << ((sysStats.totalhigh - sysStats.freehigh) * sysStats.mem_unit) << ", ";
    statOut << (deviceAllocSize) << ", ";
    statOut << (hostAllocSize) << ", ";
    statOut << (sharedAllocSize) << ", ";
    statOut << (internalAllocSize) << ", ";
    statOut << procStats.ru_maxrss << ", ";
    statOut << procStats.ru_utime.tv_sec << "." << procStats.ru_utime.tv_usec << ", ";
    statOut << procStats.ru_stime.tv_sec << "." << procStats.ru_stime.tv_usec << std::endl;
}
