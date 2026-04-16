/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#ifndef INTEL_NPU_SMI_FILE_UTILS_HPP
#define INTEL_NPU_SMI_FILE_UTILS_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace intel::npu::smi::utils {

/**
 * @brief Read entire file content as a string
 * @param path Path to the file
 * @return File content as string, empty if file cannot be read
 */
std::string readFile(const std::string& path);

/**
 * @brief Check if a file or directory exists
 * @param path Path to check
 * @return true if path exists, false otherwise
 */
bool fileExists(const std::string& path);

/**
 * @brief List all entries in a directory (excluding . and ..)
 * @param path Directory path
 * @return Vector of entry names
 */
std::vector<std::string> listDirectory(const std::string& path);

/**
 * @brief Run a shell command and capture its output
 * @param cmd Command to execute
 * @param output Reference to string where output will be stored
 * @return Command exit code, -1 on failure
 */
int runCommand(const std::string& cmd, std::string& output);

/**
 * @brief Format memory value with appropriate unit
 * @param mem Memory value in bytes
 * @param out_value Output formatted value
 * @param out_unit Output unit string (Bytes, KiB, MiB, GiB, TiB)
 */
void formatMemory(uint64_t mem, double& out_value, std::string& out_unit);

}  // namespace intel::npu::smi::utils

#endif  // INTEL_NPU_SMI_FILE_UTILS_HPP
