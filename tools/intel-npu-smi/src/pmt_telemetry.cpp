/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "pmt_telemetry.hpp"
#include "file_utils.hpp"

#include <cstdio>
#include <stdexcept>
#include <fcntl.h>
#include <unistd.h>

namespace intel::npu::smi {

PmtTelemetry::PmtTelemetry() {
    if (!initialize()) {
        throw std::runtime_error("Failed to initialize PMT telemetry interface");
    }
}

bool PmtTelemetry::initialize() {
    if (!utils::fileExists(PMT_ROOT)) {
        fprintf(stderr, "Error: PMT sysfs interface not found at %s\n", PMT_ROOT);
        return false;
    }

    auto files = utils::listDirectory(PMT_ROOT);
    for (const auto& telem_dir : files) {
        if (telem_dir.compare(0, 5, "telem") != 0) {
            continue;
        }

        std::string telem_path = std::string(PMT_ROOT) + "/" + telem_dir;
        std::string guid_path = telem_path + "/guid";
        std::string telemetry_file = telem_path + "/telem";
        std::string size_path = telem_path + "/size";
        std::string offset_path = telem_path + "/offset";

        if (!utils::fileExists(guid_path) || !utils::fileExists(telemetry_file) ||
            !utils::fileExists(size_path) || !utils::fileExists(offset_path)) {
            continue;
        }

        std::string guid = utils::readFile(guid_path);
        telemetry_path_ = std::move(telemetry_file);

        if (guid == PmtGuid::MTL) {
            cpu_gen_ = CpuGeneration::MTL;
            regs_ = getMtlRegisterMap();
            return true;
        } else if (guid == PmtGuid::ARL || guid == PmtGuid::ARL_H || guid == PmtGuid::ARL_S) {
            cpu_gen_ = CpuGeneration::ARL;
            regs_ = getArlRegisterMap();
            return true;
        } else if (guid == PmtGuid::LNL) {
            cpu_gen_ = CpuGeneration::LNL;
            regs_ = getLnlRegisterMap();
            return true;
        } else if (guid == PmtGuid::PTL) {
            cpu_gen_ = CpuGeneration::PTL;
            regs_ = getPtlRegisterMap();
            return true;
        } else if (guid == PmtGuid::WCL) {
            cpu_gen_ = CpuGeneration::WCL;
            regs_ = getWclRegisterMap();
            return true;
        }
    }

    fprintf(stderr, "Error: No CPU telemetry devices found with known GUIDs\n");
    return false;
}

void PmtTelemetry::updateBuffer() {
    // Use low-level file I/O for sysfs binary files (more reliable on Android)
    int fd = open(telemetry_path_.c_str(), O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Warning: Failed to open telemetry file '%s'\n",
                telemetry_path_.c_str());
        return;
    }

    // RAII guard to ensure fd is always closed
    auto fd_guard = [&fd]() { close(fd); };
    struct FdGuard { decltype(fd_guard)& fn; ~FdGuard() { fn(); } } guard{fd_guard};

    try {
        // Read up to 8KB (more than enough for any PMT telemetry)
        buffer_.resize(8192);
        ssize_t bytes_read = ::read(fd, buffer_.data(), buffer_.size());

        if (bytes_read > 0) {
            buffer_.resize(static_cast<size_t>(bytes_read));
        } else {
            if (bytes_read < 0) {
                fprintf(stderr, "Warning: Failed to read telemetry data from %s\n",
                        telemetry_path_.c_str());
            }
            buffer_.clear();
        }
    } catch (const std::exception& e) {
        fprintf(stderr, "Warning: Exception during telemetry buffer update: %s\n", e.what());
        buffer_.clear();
    }
}

uint64_t PmtTelemetry::read(uint32_t offset, int msb, int lsb) const {
    if (buffer_.size() < 8 || offset > buffer_.size() - 8) {
        fprintf(stderr, "Warning: Telemetry buffer too small (size=%zu, offset=%u)\n",
                buffer_.size(), offset);
        return 0;
    }

    // Validate bit range parameters
    if (msb < 0 || msb > 63 || lsb < 0 || lsb > 63 || lsb > msb) {
        fprintf(stderr, "Error: Invalid bit range [%d:%d]\n", msb, lsb);
        return 0;
    }

    // Read 8 bytes from buffer and convert to 64-bit little endian integer
    uint64_t data = 0;
    for (size_t i = 0; i < 8; i++) {
        data |= (static_cast<uint64_t>(buffer_[offset + i]) << (i * 8));
    }

    // Create mask (handle msb=63 edge case to avoid undefined behavior)
    uint64_t msb_mask = (msb == 63) ? UINT64_MAX : (1ULL << (msb + 1)) - 1;
    uint64_t lsb_mask = (1ULL << lsb) - 1;
    uint64_t mask = msb_mask & (~lsb_mask);

    // Apply mask and shift right
    return (data & mask) >> lsb;
}

double PmtTelemetry::getFrequency() const {
    uint64_t raw = read(regs_.vpu_workpoint, 7, 0);
    if (cpu_gen_ == CpuGeneration::MTL) {
        return 2.0 * static_cast<double>(raw) / 3.0 / 10.0;
    }
    return 0.05 * static_cast<double>(raw);
}

uint32_t PmtTelemetry::getVoltage() const {
    return static_cast<uint32_t>(read(regs_.vpu_workpoint, 15, 8));
}

uint32_t PmtTelemetry::getTileConfig() const {
    return static_cast<uint32_t>(read(regs_.vpu_workpoint, 23, 16));
}

uint32_t PmtTelemetry::getTemperature() const {
    return static_cast<uint32_t>(read(regs_.soc_temperatures, 47, 40));
}

double PmtTelemetry::getEnergy() const {
    // Units: joules, Cast: U32.18.14
    // For PTL, energy is a 32-bit value at the register offset
    uint64_t val = read(regs_.vpu_energy, 31, 0);
    auto int_part = static_cast<uint32_t>(val >> 14);
    auto frac_part = static_cast<uint32_t>(val & ((1 << 14) - 1));
    double float_part = static_cast<double>(frac_part) / (1 << 14);
    double energy = int_part + float_part;
    if (cpu_gen_ == CpuGeneration::WCL) {
        energy *= 100;
    }
    return energy;
}

double PmtTelemetry::getNocBandwidth() const {
    uint64_t val = read(regs_.vpu_memory_bw, 31, 0);
    return static_cast<double>(val) / 1e3;
}

}  // namespace intel::npu::smi
