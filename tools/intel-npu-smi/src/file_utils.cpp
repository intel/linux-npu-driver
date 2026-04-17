/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "file_utils.hpp"

#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>

namespace intel::npu::smi::utils {

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }

    std::ostringstream oss;
    std::string line;
    while (std::getline(file, line)) {
        oss << line;
    }

    std::string content = oss.str();

    // Trim trailing whitespace
    const auto last_char = content.find_last_not_of(" \n\r\t");
    if (last_char != std::string::npos) {
        content.erase(last_char + 1);
    }

    return content;
}

bool fileExists(const std::string& path) {
    return access(path.c_str(), F_OK) == 0;
}

std::vector<std::string> listDirectory(const std::string& path) {
    std::vector<std::string> files;

    DIR* dir = opendir(path.c_str());
    if (!dir) {
        fprintf(stderr, "Warning: Failed to open directory '%s'\n", path.c_str());
        return files;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_name[0] != '.') {
            files.emplace_back(entry->d_name);
        }
    }

    closedir(dir);
    return files;
}

int runCommand(const std::string& cmd, std::string& output) {
    // Note: cmd is passed to a shell via popen. Callers must ensure the command
    // string does not contain untrusted user input to avoid shell injection.
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) {
        fprintf(stderr, "Warning: Failed to execute command: %s\n", cmd.c_str());
        return -1;
    }

    std::ostringstream oss;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        oss << buffer;
    }
    output = oss.str();

    int status = pclose(pipe);
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

void formatMemory(uint64_t mem, double& out_value, std::string& out_unit) {
    constexpr uint64_t KiB = 1024;
    static const char* const units[] = {"Bytes", "KiB", "MiB", "GiB", "TiB"};
    static constexpr size_t num_units = sizeof(units) / sizeof(units[0]);

    auto mem_f = static_cast<double>(mem);
    size_t unit_idx = 0;

    for (unit_idx = 0; unit_idx < num_units - 1; unit_idx++) {
        if (mem_f < KiB) {
            break;
        }
        mem_f /= KiB;
    }

    out_value = mem_f;
    out_unit = units[unit_idx];
}

}  // namespace intel::npu::smi::utils
