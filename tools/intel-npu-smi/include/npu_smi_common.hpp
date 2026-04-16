/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#ifndef INTEL_NPU_SMI_COMMON_HPP
#define INTEL_NPU_SMI_COMMON_HPP

#include <string>
#include <cstdint>

namespace intel::npu::smi {

/**
 * @brief Supported CPU generations with NPU
 */
enum class CpuGeneration {
    MTL = 0,    ///< Meteor Lake
    ARL = 1,    ///< Arrow Lake
    LNL = 2,    ///< Lunar Lake
    PTL = 3,    ///< Panther Lake
    WCL = 4,    ///< Wildcat Lake
    UNKNOWN = -1
};

/**
 * @brief Convert CPU generation enum to human-readable string
 * @param gen CPU generation
 * @return String representation of the CPU generation
 */
std::string cpuGenerationToString(CpuGeneration gen);

/**
 * @brief PMT register map for telemetry data offsets
 */
struct RegisterMap {
    uint32_t vpu_energy;        ///< Offset for VPU energy register
    uint32_t soc_temperatures;  ///< Offset for SoC temperatures register
    uint32_t vpu_workpoint;     ///< Offset for VPU workpoint register
    uint32_t vpu_memory_bw;     ///< Offset for VPU memory bandwidth register
};

/**
 * @brief Process information for NPU usage tracking
 */
struct ProcessInfo {
    uint32_t pid;               ///< Process ID
    std::string command;        ///< Command line
    uint64_t memory_kib;        ///< Memory usage in KiB
};

/**
 * @brief Configuration for the SMI tool
 */
struct SmiConfig {
    int interval_ms = 0;        ///< Update interval in milliseconds (0 = single snapshot, minimum 200ms when set)
    bool colored = false;       ///< Enable colored output
    bool verbose = false;       ///< Enable verbose logging
    std::string csv_file;       ///< CSV output file path (empty = disabled)
};

/**
 * @brief Get register map for Meteor Lake
 * @return RegisterMap for MTL
 */
RegisterMap getMtlRegisterMap();

/**
 * @brief Get register map for Arrow Lake
 * @return RegisterMap for ARL
 */
RegisterMap getArlRegisterMap();

/**
 * @brief Get register map for Lunar Lake
 * @return RegisterMap for LNL
 */
RegisterMap getLnlRegisterMap();

/**
 * @brief Get register map for Panther Lake
 * @return RegisterMap for PTL
 */
RegisterMap getPtlRegisterMap();

/**
 * @brief Get register map for Wildcat Lake
 * @return RegisterMap for WCL
 */
RegisterMap getWclRegisterMap();

}  // namespace intel::npu::smi

#endif  // INTEL_NPU_SMI_COMMON_HPP
