/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#ifndef INTEL_NPU_SMI_CSV_LOGGER_HPP
#define INTEL_NPU_SMI_CSV_LOGGER_HPP

#include "terminal_display.hpp"

#include <string>
#include <fstream>

namespace intel::npu::smi {

/**
 * @brief CSV logger for NPU metrics
 *
 * Logs NPU telemetry data to a CSV file for analysis and monitoring.
 */
class CsvLogger {
public:
    /**
     * @brief Construct CsvLogger
     * @param filename Path to CSV file
     */
    explicit CsvLogger(const std::string& filename);

    /**
     * @brief Destructor - closes file if open
     */
    ~CsvLogger();

    // Disable copy and move operations
    CsvLogger(const CsvLogger&) = delete;
    CsvLogger& operator=(const CsvLogger&) = delete;
    CsvLogger(CsvLogger&&) = delete;
    CsvLogger& operator=(CsvLogger&&) = delete;

    /**
     * @brief Check if logger is properly initialized
     * @return true if file is open and ready
     */
    bool isOpen() const { return file_.is_open(); }

    /**
     * @brief Write CSV header row
     */
    void writeHeader();

    /**
     * @brief Log NPU metrics to CSV
     * @param metrics Current NPU metrics
     */
    void logMetrics(const NpuMetrics& metrics);

    /**
     * @brief Get the filename
     * @return Filename string
     */
    const std::string& getFilename() const { return filename_; }

private:
    std::string filename_;
    std::ofstream file_;
};

}  // namespace intel::npu::smi

#endif  // INTEL_NPU_SMI_CSV_LOGGER_HPP
