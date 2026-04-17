/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

/**
 * @file main.cpp
 * @brief Intel NPU System Monitoring Tool entry point
 *
 * A high-performance C++ implementation of the Intel Neural Processing Unit (NPU)
 * system monitoring tool. This tool provides real-time monitoring of NPU performance
 * metrics including power usage, frequency, temperature, bandwidth, and active
 * process tracking.
 */

#include "npu_smi_common.hpp"
#include "file_utils.hpp"
#include "pmt_telemetry.hpp"
#include "process_tracker.hpp"
#include "terminal_display.hpp"
#include "csv_logger.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <getopt.h>

namespace {

constexpr const char* DRIVER_PATH = "/sys/bus/pci/drivers/intel_vpu/";
constexpr const char* VERSION = "1.0.0";

void printUsage(const char* program_name) {
    std::cout << "Intel NPU System Monitoring Tool v" << VERSION << "\n\n"
              << "Usage: " << program_name << " [OPTIONS]\n\n"
              << "Options:\n"
              << "  -i, --interval <ms>   Set update interval in milliseconds (minimum 200ms)\n"
              << "                        Without -i, prints a single snapshot\n"
              << "  -c, --color           Enable colored terminal output\n"
              << "  -v, --verbose         Enable verbose logging\n"
              << "  --csv <file>          Export metrics to CSV file\n"
              << "  -h, --help            Display this help message\n"
              << "  --version             Display version information\n\n"
              << "Examples:\n"
              << "  " << program_name << "                    Single snapshot (default)\n"
              << "  " << program_name << " -i 500 -c          Continuous monitoring every 500ms with color\n"
              << "  " << program_name << " -i 1000 --csv log.csv  Continuous monitoring, export to CSV\n";
}

void printVersion() {
    std::cout << "Intel NPU System Monitoring Tool v" << VERSION << "\n"
              << "Copyright 2026 Intel Corporation\n";
}

/**
 * @brief Result of argument parsing
 */
struct ParseResult {
    intel::npu::smi::SmiConfig config;
    bool should_exit = false;
    int exit_code = 0;
};

ParseResult parseArguments(int argc, char* argv[]) {
    ParseResult result;

    static struct option long_options[] = {
        {"interval", required_argument, nullptr, 'i'},
        {"color",    no_argument,       nullptr, 'c'},
        {"verbose",  no_argument,       nullptr, 'v'},
        {"csv",      required_argument, nullptr, 1},
        {"help",     no_argument,       nullptr, 'h'},
        {"version",  no_argument,       nullptr, 2},
        {nullptr,    0,                 nullptr, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "i:cvh", long_options, nullptr)) != -1) {
        switch (opt) {
            case 'i':
                try {
                    result.config.interval_ms = std::stoi(optarg);
                } catch (const std::exception&) {
                    fprintf(stderr, "Error: Invalid interval value: %s\n", optarg);
                    return {result.config, true, 1};
                }
                if (result.config.interval_ms < 200) {
                    fprintf(stderr, "Error: Minimum interval is 200ms, got: %s\n", optarg);
                    return {result.config, true, 1};
                }
                break;
            case 'c':
                result.config.colored = true;
                break;
            case 'v':
                result.config.verbose = true;
                break;
            case 1:  // --csv
                result.config.csv_file = optarg;
                break;
            case 'h':
                printUsage(argv[0]);
                return {result.config, true, 0};
            case 2:  // --version
                printVersion();
                return {result.config, true, 0};
            default:
                printUsage(argv[0]);
                return {result.config, true, 1};
        }
    }

    return result;
}

struct DeviceInfo {
    std::string dev_file;
    std::string dev_path;
    std::string pci_bdf;      // PCI Bus:Device.Function (e.g., "0000:00:0b.0")
    std::string pciid;
    std::string fw_version;
    std::string driver_version;
    std::string npu_busy_path;
    std::string mem_util_path;
};

bool discoverDevice(DeviceInfo& info) {
    using namespace intel::npu::smi::utils;

    if (!fileExists(DRIVER_PATH)) {
        fprintf(stderr, "Error: Intel NPU driver 'intel_vpu' is not loaded.\n");
        return false;
    }

    try {
        auto pci_entries = listDirectory(DRIVER_PATH);
        for (const auto& entry : pci_entries) {
            if (entry.find("0000:") != 0) {
                continue;
            }

            info.dev_path = std::string(DRIVER_PATH) + entry;
            std::string accel_path = info.dev_path + "/accel";

            if (fileExists(accel_path)) {
                auto accel_entries = listDirectory(accel_path);
                if (!accel_entries.empty()) {
                    // Check if /dev/accel/ directory exists (Linux) or use /dev/ (Android)
                    std::string accel_dev_dir = "/dev/accel/";
                    if (!fileExists(accel_dev_dir)) {
                        accel_dev_dir = "/dev/";
                    }
                    info.dev_file = accel_dev_dir + accel_entries[0];
                    info.pci_bdf = entry;  // Save PCI BDF for debugfs access
                    break;
                }
            }
        }

        if (info.dev_file.empty()) {
            fprintf(stderr, "Error: No NPU device found.\n");
            return false;
        }

        info.npu_busy_path = info.dev_path + "/npu_busy_time_us";
        info.mem_util_path = info.dev_path + "/npu_memory_utilization";
        info.pciid = readFile(info.dev_path + "/device");

        info.fw_version = readFile("/sys/kernel/debug/accel/" + info.pci_bdf + "/fw_version");

        std::string ver_str = readFile(std::string(DRIVER_PATH) + "module/version");
        info.driver_version = ver_str.substr(0, ver_str.find(' '));
    } catch (const std::exception& e) {
        fprintf(stderr, "Error: Exception during device discovery: %s\n", e.what());
        return false;
    }

    return true;
}

}  // namespace

int main(int argc, char* argv[]) {
    using namespace intel::npu::smi;

    auto parsed = parseArguments(argc, argv);
    if (parsed.should_exit) {
        return parsed.exit_code;
    }
    auto config = parsed.config;

    // Discover NPU device
    DeviceInfo device;
    if (!discoverDevice(device)) {
        return 1;
    }

    // Initialize PMT telemetry
    std::unique_ptr<PmtTelemetry> telemetry;
    try {
        telemetry = std::make_unique<PmtTelemetry>();
    } catch (const std::exception& e) {
        fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }

    // Initialize components
    ProcessTracker process_tracker(device.dev_file);
    TerminalDisplay display(config.colored);
    std::unique_ptr<CsvLogger> csv_logger;

    if (!config.csv_file.empty()) {
        csv_logger = std::make_unique<CsvLogger>(config.csv_file);
        if (csv_logger->isOpen()) {
            csv_logger->writeHeader();
        } else {
            fprintf(stderr, "Warning: Could not open CSV file for writing.\n");
        }
    }

    // Read busy time helper
    auto readBusyTime = [&device]() -> uint64_t {
        if (!utils::fileExists(device.npu_busy_path)) {
            return 0;
        }
        try {
            return std::stoull(utils::readFile(device.npu_busy_path));
        } catch (const std::invalid_argument& e) {
            fprintf(stderr, "Warning: Invalid busy time value: %s\n", e.what());
            return 0;
        } catch (const std::out_of_range& e) {
            fprintf(stderr, "Warning: Busy time value out of range: %s\n", e.what());
            return 0;
        }
    };

    // Brief sampling window (200ms) used for the first iteration
    // so output appears quickly and power/utilization deltas are meaningful.
    constexpr double SNAPSHOT_SAMPLE_MS = 200.0;

    // Initialize baseline readings
    telemetry->updateBuffer();
    uint64_t time_1 = readBusyTime();
    double e0 = telemetry->getEnergy();
    double bw0 = telemetry->getNocBandwidth();

    // Main monitoring loop
    bool first_iteration = true;
    while (true) {
        double effective_sleep_ms;
        if (first_iteration) {
            // First iteration always uses the short sampling window
            // so output appears quickly with meaningful power/utilization deltas
            effective_sleep_ms = SNAPSHOT_SAMPLE_MS;
            first_iteration = false;
        } else {
            effective_sleep_ms = config.interval_ms;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(effective_sleep_ms)));

        // Calculate utilization
        uint64_t time_2 = readBusyTime();
        uint64_t delta = time_2 - time_1;
        int utilization = 0;
        if (effective_sleep_ms > 0) {
            utilization = std::min(100, static_cast<int>(100.0 * static_cast<double>(delta) / (effective_sleep_ms * 1e-3) / 1e6));
        }

        // Update telemetry
        telemetry->updateBuffer();

        // Calculate metrics
        double e1 = telemetry->getEnergy();
        double power = (e1 - e0) / (effective_sleep_ms * 1e-3);
        e0 = e1;

        double freq = telemetry->getFrequency();
        uint32_t tile_config = telemetry->getTileConfig();
        uint32_t temp = telemetry->getTemperature();
        double bw = telemetry->getNocBandwidth();

        // Calculate bandwidth (delta / time)
        double bandwidth = (bw - bw0) / (effective_sleep_ms * 1e-3);
        std::string bw_unit;
        if (bandwidth > 1024) {
            bandwidth /= 1024;
            bw_unit = "GiB/s";
        } else {
            bw_unit = "MiB/s";
        }

        // Get memory utilization
        uint64_t mem_util_bytes = 0;
        std::string mem_str = utils::readFile(device.mem_util_path);
        if (!mem_str.empty()) {
            try {
                mem_util_bytes = std::stoull(mem_str);
            } catch (const std::invalid_argument& e) {
                fprintf(stderr, "Warning: Invalid memory utilization value: %s\n", e.what());
                mem_util_bytes = 0;
            } catch (const std::out_of_range& e) {
                fprintf(stderr, "Warning: Memory utilization value out of range: %s\n", e.what());
                mem_util_bytes = 0;
            }
        }

        double mem_util = 0;
        std::string mem_util_unit;
        utils::formatMemory(mem_util_bytes, mem_util, mem_util_unit);

        // Build metrics structure
        NpuMetrics metrics;
        metrics.power_watts = power;
        metrics.frequency_hz = (freq * 1000) / 2;
        metrics.bandwidth = bandwidth;
        metrics.bandwidth_unit = std::move(bw_unit);
        metrics.tile_config = tile_config;
        metrics.temperature_celsius = temp;
        metrics.utilization_percent = utilization;
        metrics.memory_usage = mem_util;
        metrics.memory_unit = std::move(mem_util_unit);

        // Log to CSV if enabled
        if (csv_logger && csv_logger->isOpen()) {
            csv_logger->logMetrics(metrics);
        }

        // Clear screen only in continuous monitoring mode (not snapshot)
        if (config.interval_ms > 0) {
            TerminalDisplay::clearScreen();
        }

        const auto processes = process_tracker.getActiveProcesses();
        display.displayDashboard(metrics, device.pciid, device.driver_version,
                                  device.fw_version, processes);

        // Update baseline values
        time_1 = time_2;
        bw0 = bw;

        // Single snapshot mode
        if (config.interval_ms == 0) {
            break;
        }
    }

    return 0;
}
