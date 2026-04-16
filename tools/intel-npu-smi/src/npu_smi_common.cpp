/*
 * Copyright 2026 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "npu_smi_common.hpp"

namespace intel::npu::smi {

std::string cpuGenerationToString(CpuGeneration gen) {
    switch (gen) {
        case CpuGeneration::MTL: return "Meteor Lake";
        case CpuGeneration::ARL: return "Arrow Lake";
        case CpuGeneration::LNL: return "Lunar Lake";
        case CpuGeneration::PTL: return "Panther Lake";
        case CpuGeneration::WCL: return "Wildcat Lake";
        case CpuGeneration::UNKNOWN: return "Unknown";
    }
    return "Unknown";
}

RegisterMap getMtlRegisterMap() {
    return {0x628, 0x98, 0x68, 0x0};
}

RegisterMap getArlRegisterMap() {
    return getMtlRegisterMap();
}

RegisterMap getLnlRegisterMap() {
    return {0x5d0, 0x70, 0x18, 0xc18};
}

RegisterMap getPtlRegisterMap() {
    return {0x670, 0x78, 0x18, 0xc18};
}

RegisterMap getWclRegisterMap() {
    return getPtlRegisterMap();
}

}  // namespace intel::npu::smi
