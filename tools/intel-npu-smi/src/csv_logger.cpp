/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "csv_logger.hpp"
#include "terminal_display.hpp"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <iomanip>

namespace intel::npu::smi {

CsvLogger::CsvLogger(const std::string& filename)
    : filename_(filename) {
    file_.open(filename, std::ios::out | std::ios::trunc);
    if (!file_.is_open()) {
        fprintf(stderr, "Warning: Failed to open CSV file '%s' for writing.\n", filename.c_str());
    }
}

CsvLogger::~CsvLogger() {
    if (file_.is_open()) {
        file_.close();
    }
}

void CsvLogger::writeHeader() {
    if (!file_.is_open()) {
        fprintf(stderr, "Warning: Cannot write CSV header, file '%s' is not open\n",
                filename_.c_str());
        return;
    }
    file_ << "timestamp,power,frequency,bandwidth,tile_config,temperature,utilization,memory_usage\n";
    file_.flush();
}

void CsvLogger::logMetrics(const NpuMetrics& metrics) {
    if (!file_.is_open()) {
        fprintf(stderr, "Warning: Cannot log metrics, CSV file '%s' is not open\n",
                filename_.c_str());
        return;
    }

    try {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        std::tm tm_buf{};
        localtime_r(&time_t_now, &tm_buf);

        file_ << std::put_time(&tm_buf, "%H:%M:%S")
              << '.' << std::setfill('0') << std::setw(3) << ms.count() << ","
              << TerminalDisplay::formatNumber(metrics.power_watts, 3) << ","
              << TerminalDisplay::formatNumber(metrics.frequency_hz, 0) << ","
              << TerminalDisplay::formatNumber(metrics.bandwidth, 3) << ","
              << metrics.tile_config << ","
              << metrics.temperature_celsius << ","
              << metrics.utilization_percent << ","
              << TerminalDisplay::formatNumber(metrics.memory_usage, 2) << "\n";
        file_.flush();
    } catch (const std::exception& e) {
        fprintf(stderr, "Warning: Failed to write metrics to CSV: %s\n", e.what());
    }
}

}  // namespace intel::npu::smi
