/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "terminal_display.hpp"
#include "file_utils.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace intel::npu::smi {

// Helper: compute visible length of a string (strip ANSI escape codes)
static size_t visibleLength(const std::string& s) {
    size_t len = 0;
    bool in_escape = false;
    for (char c : s) {
        if (in_escape) {
            if (c == 'm') {
                in_escape = false;
            }
        } else if (c == '\033') {
            in_escape = true;
        } else {
            // Handle multi-byte UTF-8: only count leading bytes
            if ((static_cast<unsigned char>(c) & 0xC0) != 0x80) {
                len++;
            }
        }
    }
    return len;
}

// Helper: pad string to desired visible width (right-aligned)
static std::string padRight(const std::string& s, size_t width) {
    size_t vis = visibleLength(s);
    if (vis >= width) return s;
    return std::string(width - vis, ' ') + s;
}

// Helper: pad string to desired visible width (left-aligned)
static std::string padLeft(const std::string& s, size_t width) {
    size_t vis = visibleLength(s);
    if (vis >= width) return s;
    return s + std::string(width - vis, ' ');
}

ColorCodes::ColorCodes(bool enabled) : enabled_(enabled) {
    if (enabled) {
        yellow_ = "\033[38;5;3m";
        green_ = "\033[38;5;10m";
        magenta_ = "\033[38;5;13m";
        cyan_ = "\033[38;5;14m";
        reset_ = "\033[0m";
    }
}

std::string ColorCodes::color(int code, const std::string& text) const {
    if (!enabled_) {
        return text;
    }

    switch (code) {
        case 3: return yellow_ + text + reset_;
        case 10: return green_ + text + reset_;
        case 13: return magenta_ + text + reset_;
        case 14: return cyan_ + text + reset_;
        default: return text;
    }
}

TerminalDisplay::TerminalDisplay(bool colored)
    : colors_(colored) {
}

void TerminalDisplay::clearScreen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

std::string TerminalDisplay::formatNumber(double value, int precision) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

std::string TerminalDisplay::bracket(const std::string& text) const {
    return colors_.color(13, "[") + colors_.color(14, text) + colors_.color(13, "]");
}

std::string TerminalDisplay::createField(const std::string& val, const std::string& unit, size_t width) const {
    std::string field = val;
    if (!unit.empty()) {
        field += " " + bracket(unit);
    }
    return padRight(field, width);
}

void TerminalDisplay::displayDashboard(
    const NpuMetrics& metrics,
    const std::string& pciid,
    const std::string& driver_version,
    const std::string& fw_version,
    const std::vector<ProcessInfo>& processes) const {

    // Build display strings with colors
    std::string PLUS = colors_.color(3, "+");
    std::string VL = colors_.color(3, "|");
    std::string HL = PLUS + colors_.color(3, "-----------------------------------------------------------------------------------------------") + PLUS;
    std::string HDL = PLUS + colors_.color(3, "===============================================================================================") + PLUS;

    auto SP = [](const std::string& x, size_t n) {
        return std::string(n, ' ') + x + std::string(n, ' ');
    };

    // Maximum visible width for firmware version display
    constexpr size_t FW_DISPLAY_WIDTH = 75;

    // Header
    std::cout << HL << "\n";
    std::cout << VL << " " << colors_.color(14, "INTEL NPU") << " "
              << colors_.color(10, "Device") << ": " << padRight(pciid, 6) << " " << VL << " "
              << colors_.color(10, "Driver version") << ": " << padRight(driver_version, 50) << " " << VL << "\n";

    std::string fw_display = fw_version.substr(0, std::min(fw_version.length(), FW_DISPLAY_WIDTH));
    std::cout << VL << " " << colors_.color(10, "Firmware version") << ": " << padLeft(fw_display, FW_DISPLAY_WIDTH) << " " << VL << "\n";

    if (fw_version.length() > FW_DISPLAY_WIDTH) {
        std::cout << VL << " " << padLeft(fw_version.substr(FW_DISPLAY_WIDTH), 94) << VL << "\n";
    }

    // Metrics header row 1
    std::cout << HDL << "\n";
    std::cout << VL << SP(colors_.color(10, "Power Usage"), 7) << " " << VL << " "
              << SP(colors_.color(10, "DPU Frequency"), 4) << VL
              << SP(colors_.color(10, "NPU DDR Average Bandwidth"), 1) << VL
              << SP(colors_.color(10, "Tile Config"), 3) << VL << "\n";

    // Metrics values row 1
    std::cout << VL << createField(formatNumber(metrics.power_watts, 2), "W", 25) << " " << VL << " "
              << createField(formatNumber(metrics.frequency_hz, 0), "Hz", 20) << " " << VL << " "
              << createField(formatNumber(metrics.bandwidth, 3), metrics.bandwidth_unit, 25) << " " << VL << " "
              << padRight(std::to_string(metrics.tile_config), 15) << " " << VL << "\n";

    // Metrics header row 2
    std::cout << HDL << "\n";
    std::cout << VL << SP(colors_.color(10, "NPU Temperature"), 5) << " " << VL
              << SP(colors_.color(10, "NPU Utilization"), 7) << VL << " "
              << padRight(colors_.color(10, "Memory Usage"), 37) << VL << "\n";

    // Metrics values row 2
    std::cout << VL << " " << createField(std::to_string(metrics.temperature_celsius), "°C", 24) << " " << VL << " "
              << createField(std::to_string(std::min(metrics.utilization_percent, 100)), "%", 27) << " " << VL << " "
              << createField(formatNumber(metrics.memory_usage, 2), metrics.memory_unit, 36) << " " << VL << "\n";

    std::cout << HDL << "\n";

    // Process list
    if (!processes.empty()) {
        std::cout << VL << SP(colors_.color(10, "PID"), 4) << VL
                  << SP(colors_.color(10, "Active NPU processes"), 23) << " " << VL
                  << SP(colors_.color(10, "Memory"), 4) << " " << VL << "\n";
        std::cout << HL << "\n";

        constexpr int MAXW = 65;
        for (const auto& proc : processes) {
            double proc_mem = 0;
            std::string proc_mem_unit;
            utils::formatMemory(proc.memory_kib * 1024, proc_mem, proc_mem_unit);

            std::string command_display = proc.command.substr(0, std::min(proc.command.length(), size_t(MAXW)));
            std::cout << VL << " " << padLeft(colors_.color(13, std::to_string(proc.pid)), 9)
                      << " " << VL << " " << padLeft(command_display, MAXW)
                      << " " << VL << " " << padRight(createField(formatNumber(proc_mem, 2), proc_mem_unit, 13), 13) << " " << VL << "\n";

            // Print continuation lines for long commands
            if (proc.command.length() > MAXW) {
                std::string remaining = proc.command.substr(MAXW);
                while (!remaining.empty()) {
                    std::string chunk = remaining.substr(0, std::min(remaining.length(), size_t(MAXW)));
                    remaining = remaining.length() > MAXW ? remaining.substr(MAXW) : "";
                    std::cout << VL << " " << std::string(9, ' ') << " " << VL << " "
                              << padLeft(chunk, MAXW) << " " << VL << " "
                              << std::string(13, ' ') << " " << VL << "\n";
                }
            }
        }
        std::cout << HL << "\n";
    }
}

}  // namespace intel::npu::smi
