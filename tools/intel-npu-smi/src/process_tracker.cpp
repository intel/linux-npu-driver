/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "process_tracker.hpp"
#include "file_utils.hpp"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <fstream>
#include <set>
#include <sstream>
#include <unistd.h>

namespace intel::npu::smi {

ProcessTracker::ProcessTracker(const std::string& device_path)
    : device_path_(device_path) {
}

uint64_t ProcessTracker::checkFdinfoForIntelNpu(uint32_t pid, const std::string& fd_name) {
    std::string fdinfo_path = "/proc/" + std::to_string(pid) + "/fdinfo/" + fd_name;
    if (!utils::fileExists(fdinfo_path)) {
        return 0;
    }

    std::ifstream file(fdinfo_path);
    if (!file.is_open()) {
        fprintf(stderr, "Warning: Failed to open fdinfo '%s'\n", fdinfo_path.c_str());
        return 0;
    }

    std::string line;
    bool found_vpu = false;
    uint64_t memory_kib = 0;

    while (std::getline(file, line)) {
        // Parse key-value pair: trim whitespace around the value for robust matching
        auto colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);
            // Trim leading whitespace/tabs from value
            auto val_start = value.find_first_not_of(" \t");
            if (val_start != std::string::npos) {
                value = value.substr(val_start);
            }

            if (key == "drm-driver" && value == "intel_vpu") {
                found_vpu = true;
            }
            if (key == "drm-resident-memory") {
                try {
                    memory_kib = std::stoull(value);
                } catch (...) {
                    memory_kib = 0;
                }
            }
        }
    }

    return found_vpu ? memory_kib : 0;
}

std::string ProcessTracker::getProcessCommand(uint32_t pid, const std::string& fallback) {
    std::ifstream file("/proc/" + std::to_string(pid) + "/cmdline");
    if (!file.is_open()) {
        return fallback;
    }

    std::string cmdline;
    std::string line;
    while (std::getline(file, line)) {
        // Replace null terminators with spaces
        for (char& c : line) {
            if (c == '\0') {
                c = ' ';
            }
        }
        cmdline += line;
    }

    // Trim trailing whitespace
    const auto last_char = cmdline.find_last_not_of(" \n\r\t");
    if (last_char != std::string::npos) {
        cmdline.erase(last_char + 1);
    }

    return cmdline.empty() ? fallback : cmdline;
}

std::unique_ptr<ProcessInfo> ProcessTracker::processPidFds(uint32_t pid, const std::string& fallback_command) {
    std::string proc_fd_dir = "/proc/" + std::to_string(pid) + "/fd";
    if (!utils::fileExists(proc_fd_dir)) {
        return nullptr;
    }

    auto fds = utils::listDirectory(proc_fd_dir);
    for (const auto& fd_name : fds) {
        std::string fd_link = proc_fd_dir + "/" + fd_name;
        char link_target[PATH_MAX];
        ssize_t len = readlink(fd_link.c_str(), link_target, sizeof(link_target) - 1);
        if (len == -1) {
            continue;
        }
        link_target[len] = '\0';

        // Match both /dev/accel/accelN (Linux) and /dev/accelN (Android)
        std::string target(link_target);
        if (target.find("/dev/accel") == std::string::npos) {
            continue;
        }

        uint64_t memory_kib = checkFdinfoForIntelNpu(pid, fd_name);
        if (memory_kib > 0) {
            // Found an NPU-using fd, return process info
            std::string command = getProcessCommand(pid, fallback_command);
            auto proc_info = std::make_unique<ProcessInfo>();
            proc_info->pid = pid;
            proc_info->command = std::move(command);
            proc_info->memory_kib = memory_kib;
            return proc_info;
        }
    }

    return nullptr;
}

std::vector<ProcessInfo> ProcessTracker::getActiveProcesses() {
    std::vector<ProcessInfo> processes;
    std::set<uint32_t> seen_pids;

    // Method 1: Try lsof (works on most Linux systems)
    std::string output;
    std::string cmd = "lsof " + device_path_ + " 2>/dev/null";
    if (utils::runCommand(cmd, output) == 0 && !output.empty()) {
        std::istringstream iss(output);
        std::string line;
        std::getline(iss, line);  // Skip header

        while (std::getline(iss, line)) {
            if (line.empty()) continue;

            std::istringstream line_iss(line);
            std::string cmd_name;
            uint32_t pid;
            line_iss >> cmd_name >> pid;

            if (seen_pids.count(pid)) continue;
            seen_pids.insert(pid);

            auto proc_info = processPidFds(pid, cmd_name);
            if (proc_info) {
                processes.push_back(std::move(*proc_info));
            }
        }
    }

    // Method 2: Direct /proc scan fallback (works on Android and when lsof is unavailable)
    if (processes.empty()) {
        auto proc_entries = utils::listDirectory("/proc");
        for (const auto& entry : proc_entries) {
            // Only process numeric directory names (PIDs)
            bool is_pid = !entry.empty() && std::all_of(entry.begin(), entry.end(), ::isdigit);
            if (!is_pid) continue;

            uint32_t pid = 0;
            try {
                pid = static_cast<uint32_t>(std::stoul(entry));
            } catch (...) {
                continue;
            }

            if (seen_pids.count(pid)) continue;

            auto proc_info = processPidFds(pid, entry);
            if (proc_info) {
                seen_pids.insert(pid);
                processes.push_back(std::move(*proc_info));
            }
        }
    }

    return processes;
}

}  // namespace intel::npu::smi
