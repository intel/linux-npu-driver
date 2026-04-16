/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#ifndef INTEL_NPU_SMI_PMT_TELEMETRY_HPP
#define INTEL_NPU_SMI_PMT_TELEMETRY_HPP

#include "npu_smi_common.hpp"

#include <string>
#include <vector>
#include <cstdint>

namespace intel::npu::smi {

/**
 * @brief PMT GUIDs for different CPU generations
 */
namespace PmtGuid {
    constexpr const char* MTL = "0x130670b2";       ///< Meteor Lake telemetry GUID
    constexpr const char* ARL = "0x1306a0b3";       ///< Arrow Lake telemetry GUID
    constexpr const char* ARL_H = "0x1306a0b2";     ///< Arrow Lake-H telemetry GUID
    constexpr const char* ARL_S = "0x1306a0b4";     ///< Arrow Lake-S telemetry GUID
    constexpr const char* LNL = "0x3072005";        ///< Lunar Lake telemetry GUID
    constexpr const char* PTL = "0x3086000";        ///< Panther Lake telemetry GUID
    constexpr const char* WCL = "0x308d000";        ///< Wildcat Lake telemetry GUID
}  // namespace PmtGuid

/**
 * @brief Platform Monitoring Technology (PMT) Telemetry interface
 *
 * Provides access to Intel NPU telemetry data through the PMT sysfs interface.
 * Supports multiple CPU generations including Meteor Lake, Arrow Lake,
 * Lunar Lake, Panther Lake, and Wildcat Lake.
 */
class PmtTelemetry {
public:
    /**
     * @brief Construct PMT Telemetry interface
     * @throws std::runtime_error if PMT interface is not available
     */
    PmtTelemetry();

    /**
     * @brief Destructor
     */
    ~PmtTelemetry() = default;

    // Disable copy operations
    PmtTelemetry(const PmtTelemetry&) = delete;
    PmtTelemetry& operator=(const PmtTelemetry&) = delete;

    // Enable move operations
    PmtTelemetry(PmtTelemetry&&) = default;
    PmtTelemetry& operator=(PmtTelemetry&&) = default;

    /**
     * @brief Update internal telemetry buffer from sysfs
     */
    void updateBuffer();

    /**
     * @brief Read a value from the telemetry buffer
     * @param offset Byte offset in the buffer
     * @param msb Most significant bit position
     * @param lsb Least significant bit position
     * @return Extracted value
     */
    uint64_t read(uint32_t offset, int msb, int lsb) const;

    /**
     * @brief Get NPU operating frequency
     * @return Frequency in GHz
     */
    double getFrequency() const;

    /**
     * @brief Get NPU operating voltage
     * @return Voltage value
     */
    uint32_t getVoltage() const;

    /**
     * @brief Get NPU tile configuration
     * @return Tile configuration value
     */
    uint32_t getTileConfig() const;

    /**
     * @brief Get NPU temperature
     * @return Temperature in degrees Celsius
     */
    uint32_t getTemperature() const;

    /**
     * @brief Get NPU energy consumption
     * @return Energy in Joules
     */
    double getEnergy() const;

    /**
     * @brief Get NPU NoC (Network-on-Chip) bandwidth
     * @return Bandwidth in MiB/s
     */
    double getNocBandwidth() const;

    /**
     * @brief Get detected CPU generation
     * @return CPU generation enum value
     */
    CpuGeneration getCpuGeneration() const { return cpu_gen_; }

    /**
     * @brief Check if telemetry interface is available
     * @return true if interface is valid, false otherwise
     */
    bool isValid() const { return cpu_gen_ != CpuGeneration::UNKNOWN; }

private:
    static constexpr const char* PMT_ROOT = "/sys/class/intel_pmt";

    std::string telemetry_path_;
    std::vector<unsigned char> buffer_;
    RegisterMap regs_{};
    CpuGeneration cpu_gen_ = CpuGeneration::UNKNOWN;

    /**
     * @brief Initialize telemetry interface by scanning PMT devices
     * @return true if successful, false otherwise
     */
    bool initialize();
};

}  // namespace intel::npu::smi

#endif  // INTEL_NPU_SMI_PMT_TELEMETRY_HPP
