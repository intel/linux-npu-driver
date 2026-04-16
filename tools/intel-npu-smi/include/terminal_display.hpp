/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#ifndef INTEL_NPU_SMI_TERMINAL_DISPLAY_HPP
#define INTEL_NPU_SMI_TERMINAL_DISPLAY_HPP

#include "npu_smi_common.hpp"

#include <string>
#include <vector>
#include <cstdint>

namespace intel::npu::smi {

/**
 * @brief ANSI color codes for terminal output
 */
class ColorCodes {
public:
    /**
     * @brief Construct ColorCodes
     * @param enabled Enable colored output
     */
    explicit ColorCodes(bool enabled = false);

    /**
     * @brief Apply color to text
     * @param code Color code (3=yellow, 10=green, 13=magenta, 14=cyan)
     * @param text Text to colorize
     * @return Colorized string
     */
    std::string color(int code, const std::string& text) const;

private:
    bool enabled_;
    std::string yellow_;     ///< Yellow color (code 3)
    std::string green_;      ///< Green color (code 10)
    std::string magenta_;    ///< Magenta color (code 13)
    std::string cyan_;       ///< Cyan color (code 14)
    std::string reset_;      ///< Reset color
};

/**
 * @brief NPU metrics data structure for display
 */
struct NpuMetrics {
    double power_watts = 0.0;           ///< Power usage in watts
    double frequency_hz = 0.0;          ///< Frequency in Hz
    double bandwidth = 0.0;             ///< Bandwidth value
    std::string bandwidth_unit;         ///< Bandwidth unit (MiB/s or GiB/s)
    uint32_t tile_config = 0;           ///< Tile configuration
    uint32_t temperature_celsius = 0;   ///< Temperature in Celsius
    int utilization_percent = 0;        ///< Utilization percentage (0-100)
    double memory_usage = 0.0;          ///< Memory usage value
    std::string memory_unit;            ///< Memory unit
};

/**
 * @brief Terminal display manager for NPU metrics
 *
 * Handles formatting and display of NPU telemetry data
 * in a tabular format with optional color support.
 */
class TerminalDisplay {
public:
    /**
     * @brief Construct TerminalDisplay
     * @param colored Enable colored output
     */
    explicit TerminalDisplay(bool colored = false);

    /**
     * @brief Destructor
     */
    ~TerminalDisplay() = default;

    /**
     * @brief Clear the terminal screen
     */
    static void clearScreen();

    /**
     * @brief Display the NPU metrics dashboard
     * @param metrics Current NPU metrics
     * @param pciid PCI device ID
     * @param driver_version Driver version string
     * @param fw_version Firmware version string
     * @param processes Active processes using NPU
     */
    void displayDashboard(
        const NpuMetrics& metrics,
        const std::string& pciid,
        const std::string& driver_version,
        const std::string& fw_version,
        const std::vector<ProcessInfo>& processes) const;

    /**
     * @brief Format a numeric value with specified precision
     * @param value Numeric value
     * @param precision Decimal precision
     * @return Formatted string
     */
    static std::string formatNumber(double value, int precision = 2);

private:
    ColorCodes colors_;

    /**
     * @brief Create a field with value and unit
     * @param val Value string
     * @param unit Unit string (empty for no unit)
     * @param width Field width
     * @return Formatted field string
     */
    std::string createField(const std::string& val, const std::string& unit, size_t width) const;

    /**
     * @brief Create bracketed text
     * @param text Text to bracket
     * @return Bracketed string with colors
     */
    std::string bracket(const std::string& text) const;
};

}  // namespace intel::npu::smi

#endif  // INTEL_NPU_SMI_TERMINAL_DISPLAY_HPP
