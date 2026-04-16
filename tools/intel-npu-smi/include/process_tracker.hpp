/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#ifndef INTEL_NPU_SMI_PROCESS_TRACKER_HPP
#define INTEL_NPU_SMI_PROCESS_TRACKER_HPP

#include "npu_smi_common.hpp"

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace intel::npu::smi {

/**
 * @brief Tracks processes using the Intel NPU device
 *
 * Monitors /proc filesystem and device file descriptors to identify
 * processes actively using the NPU accelerator device.
 */
class ProcessTracker {
public:
    /**
     * @brief Construct a ProcessTracker for the specified device
     * @param device_path Path to the NPU device file (e.g., /dev/accel/accel0)
     */
    explicit ProcessTracker(const std::string& device_path);

    /**
     * @brief Destructor
     */
    ~ProcessTracker() = default;

    // Disable copy operations
    ProcessTracker(const ProcessTracker&) = delete;
    ProcessTracker& operator=(const ProcessTracker&) = delete;

    // Enable move operations
    ProcessTracker(ProcessTracker&&) = default;
    ProcessTracker& operator=(ProcessTracker&&) = default;

    /**
     * @brief Get list of processes currently using the NPU
     *
     * Scans /proc filesystem and device file descriptors to find active NPU
     * processes. Errors reading individual /proc entries are silently skipped,
     * resulting in those processes being absent from the returned list.
     *
     * @return Vector of ProcessInfo structures (may be empty if no NPU processes found)
     */
    std::vector<ProcessInfo> getActiveProcesses();

    /**
     * @brief Set the device path to monitor
     * @param device_path Path to the NPU device file
     */
    void setDevicePath(const std::string& device_path) { device_path_ = device_path; }

    /**
     * @brief Get the current device path
     * @return Device path string
     */
    const std::string& getDevicePath() const { return device_path_; }

private:
    std::string device_path_;

    /**
     * @brief Check if a file descriptor points to the NPU driver
     * @param pid Process ID
     * @param fd_name File descriptor name
     * @return Memory usage in KiB if NPU, 0 otherwise
     */
    static uint64_t checkFdinfoForIntelNpu(uint32_t pid, const std::string& fd_name);

    /**
     * @brief Get command line for a process
     * @param pid Process ID
     * @param fallback Fallback string if command cannot be read
     * @return Command line string
     */
    static std::string getProcessCommand(uint32_t pid, const std::string& fallback);

    /**
     * @brief Process file descriptors for a given PID
     * @param pid Process ID
     * @param fallback_command Fallback command string
     * @return unique_ptr to ProcessInfo, nullptr if not using NPU
     */
    static std::unique_ptr<ProcessInfo> processPidFds(uint32_t pid, const std::string& fallback_command);
};

}  // namespace intel::npu::smi

#endif  // INTEL_NPU_SMI_PROCESS_TRACKER_HPP
