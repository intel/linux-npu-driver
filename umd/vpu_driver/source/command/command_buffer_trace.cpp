/*
 * Copyright (C) 2026 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/command_buffer.hpp"

#ifdef ENABLE_NPU_LOGGING

#include <stddef.h>
#include <stdint.h>

#include "api/vpu_jsm_job_cmd_api.h"
#include "vpu_driver/source/command/command.hpp"
#include "vpu_driver/source/command/inference_execute.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <deque>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace VPU {

namespace {
const char *getCmdTypeStr(uint16_t type) {
    switch (type) {
        CASE_RETURN_STR(VPU_CMD_TIMESTAMP);
        CASE_RETURN_STR(VPU_CMD_FENCE_WAIT);
        CASE_RETURN_STR(VPU_CMD_FENCE_SIGNAL);
        CASE_RETURN_STR(VPU_CMD_BARRIER);
        CASE_RETURN_STR(VPU_CMD_METRIC_QUERY_BEGIN);
        CASE_RETURN_STR(VPU_CMD_METRIC_QUERY_END);
        CASE_RETURN_STR(VPU_CMD_COPY);
        CASE_RETURN_STR(VPU_CMD_INFERENCE_EXECUTE);
        CASE_RETURN_STR(VPU_CMD_MEMORY_FILL);
        CASE_RETURN_STR(VPU_CMD_NOP);
    default:
        return "UNKNOWN";
    }
}

const char *getBufferTypeStr(VPUBufferObject::Type type) {
    switch (type) {
    case VPUBufferObject::Type::CachedFw:
        return "MAP";
    case VPUBufferObject::Type::CachedShave:
        return "SHAVE_MAP";
    case VPUBufferObject::Type::CachedDma:
        return "DMA_MAP";
    case VPUBufferObject::Type::CachedDmaUnmappable:
        return "DMA";
    case VPUBufferObject::Type::CachedDmaUnmappableReadOnly:
        return "DMA_RO";
    case VPUBufferObject::Type::UncachedFw:
        return "UC";
    case VPUBufferObject::Type::UncachedShave:
        return "SHAVE_UC";
    case VPUBufferObject::Type::UncachedDma:
        return "DMA_UC";
    case VPUBufferObject::Type::WriteCombineFw:
        return "WC_MAP";
    case VPUBufferObject::Type::WriteCombineFwUnmappable:
        return "WC";
    case VPUBufferObject::Type::WriteCombineShave:
        return "SHAVE_WC_MAP";
    case VPUBufferObject::Type::WriteCombineShaveUnmappable:
        return "SHAVE_WC";
    case VPUBufferObject::Type::WriteCombineDma:
        return "DMA_WC_MAP";
    case VPUBufferObject::Type::WriteCombineDmaUnmappable:
        return "DMA_WC";
    case VPUBufferObject::Type::ImportedMemory:
        return "IMPORTED";
    default:
        return "UNKNOWN";
    }
}

/**
 * Sanitizes a string so it can be safely embedded inside a JSON string literal.
 * Only printable, single-byte ASCII (0x20-0x7E) is passed through unchanged; JSON-sensitive
 * characters (quotes, backslashes), ASCII control characters, and any byte >= 0x80 (which may
 * be invalid/non-UTF-8 for externally supplied data such as blob names) are replaced with '_'.
 * This guarantees the result is always valid JSON, regardless of the input's encoding.
 */
std::string jsonEscape(const char *str) {
    std::string out;
    if (str == nullptr)
        return out;

    for (const unsigned char *p = reinterpret_cast<const unsigned char *>(str); *p != '\0'; ++p) {
        if (*p == '"' || *p == '\\' || *p < 0x20 || *p >= 0x7F) {
            out += '_';
        } else {
            out += static_cast<char>(*p);
        }
    }
    return out;
}
} // namespace

std::string VPUCommandBuffer::commandBufferToJson(const char *description) const {
    std::ostringstream oss;

    uint8_t *bufferPtr = buffer->getBasePointer();
    vpu_cmd_buffer_header_t *cmdHeader =
        reinterpret_cast<vpu_cmd_buffer_header_t *>(bufferPtr + offsetof(CommandHeader, header));

    oss << "{\n"
        << "  \"description\": \"" << jsonEscape(description) << "\",\n"
        << "  \"header_cpu_address\": \"" << cmdHeader << "\",\n"
        << "  \"header_vpu_address\": \"0x" << std::hex << buffer->getVPUAddr(cmdHeader) << std::dec
        << "\",\n"
        << "  \"header_handle\": " << buffer->getHandle() << ",\n"
        << "  \"cmd_buffer_size\": " << cmdHeader->cmd_buffer_size << ",\n"
        << "  \"api_version\": \"" << (cmdHeader->api_version >> 16) << "."
        << (cmdHeader->api_version & 0xFFFF) << "\",\n"
        << "  \"cmd_offset\": " << cmdHeader->cmd_offset << ",\n"
        << "  \"context_save_area_address\": \"0x" << std::hex
        << cmdHeader->context_save_area_address << std::dec << "\",\n";

    oss << "  \"preemption_buffer\": ";
    if (preemptionBuffer) {
        oss << "{\"handle\": " << preemptionBuffer->getHandle() << ", \"vpu_address\": \"0x"
            << std::hex << preemptionBuffer->getVPUAddr() << std::dec
            << "\", \"size\": " << preemptionBuffer->getAllocSize() << "}";
    } else {
        oss << "null";
    }
    oss << ",\n";

    oss << "  \"scratch_buffers\": [";
    if (!inferenceScratchBuffers.empty()) {
        bool firstScratch = true;
        for (const auto &[type, scratchBo] : inferenceScratchBuffers) {
            if (!scratchBo)
                continue;
            oss << (firstScratch ? "\n" : ",\n");
            firstScratch = false;
            oss << "    {\"type\": \"" << getBufferTypeStr(type)
                << "\", \"handle\": " << scratchBo->getHandle() << ", \"vpu_address\": \"0x"
                << std::hex << scratchBo->getVPUAddr() << std::dec
                << "\", \"size\": " << scratchBo->getAllocSize() << "}";
        }
        oss << (firstScratch ? "]" : "\n  ]");
    } else {
        oss << "]";
    }
    oss << ",\n";

    oss << "  \"commands\": [\n";

    uint32_t cmdOffset = cmdHeader->cmd_offset;
    int i = 0;
    auto cmdIt = commands.cbegin();
    while (cmdOffset < cmdHeader->cmd_buffer_size) {
        if (i > 0)
            oss << ",\n";

        if (i > static_cast<int>(commands.size()) || cmdIt == commands.cend()) {
            oss << "    {\n"
                << "      \"index\": " << i << ",\n"
                << "      \"error\": \"Command index exceeds the number of commands in the list\"\n"
                << "    }\n";
            break;
        }

        if (cmdHeader->cmd_buffer_size < sizeof(vpu_cmd_header_t) ||
            cmdOffset > cmdHeader->cmd_buffer_size - sizeof(vpu_cmd_header_t)) {
            oss << "    {\n"
                << "      \"index\": " << i << ",\n"
                << "      \"error\": \"Command header exceeds command buffer bounds\"\n"
                << "    }\n";
            break;
        }

        vpu_cmd_header_t *cmd = reinterpret_cast<vpu_cmd_header_t *>(
            bufferPtr + offsetof(CommandHeader, header) + cmdOffset);

        if (cmd->size < sizeof(vpu_cmd_header_t) ||
            cmd->size > cmdHeader->cmd_buffer_size - cmdOffset) {
            oss << "    {\n"
                << "      \"index\": " << i << ",\n"
                << "      \"error\": \"Invalid command size: " << cmd->size << "\"\n"
                << "    }\n";
            break;
        }

        oss << "    {\n"
            << "      \"index\": " << i << ",\n"
            << "      \"cmd\": {\"type\": \"" << getCmdTypeStr(cmd->type)
            << "\", \"size\": " << cmd->size;

        switch (cmd->type) {
        case VPU_CMD_TIMESTAMP: {
            if (cmd->size < sizeof(vpu_cmd_timestamp_t)) {
                oss << ", \"error\": \"command too short for VPU_CMD_TIMESTAMP\"}\n    }\n";
                cmdOffset = cmdHeader->cmd_buffer_size;
                continue;
            }
            auto *c = reinterpret_cast<vpu_cmd_timestamp_t *>(cmd);
            oss << ",\n        \"timestamp_address\": \"0x" << std::hex << c->timestamp_address
                << std::dec << "\"\n      }";
            break;
        }
        case VPU_CMD_FENCE_WAIT:
        case VPU_CMD_FENCE_SIGNAL: {
            if (cmd->size < sizeof(vpu_cmd_fence_t)) {
                oss << ", \"error\": \"command too short for VPU_CMD_FENCE\"}\n    }\n";
                cmdOffset = cmdHeader->cmd_buffer_size;
                continue;
            }
            auto *c = reinterpret_cast<vpu_cmd_fence_t *>(cmd);
            oss << ",\n        \"offset\": \"0x" << std::hex << c->offset << std::dec << "\",\n"
                << "        \"value\": \"0x" << std::hex << c->value << std::dec << "\"\n      }";
            break;
        }
        case VPU_CMD_BARRIER:
        case VPU_CMD_NOP:
            oss << "}";
            break;
        case VPU_CMD_METRIC_QUERY_BEGIN:
        case VPU_CMD_METRIC_QUERY_END: {
            if (cmd->size < sizeof(vpu_cmd_metric_query_t)) {
                oss << ", \"error\": \"command too short for VPU_CMD_METRIC_QUERY\"}\n    }\n";
                cmdOffset = cmdHeader->cmd_buffer_size;
                continue;
            }
            auto *c = reinterpret_cast<vpu_cmd_metric_query_t *>(cmd);
            oss << ",\n        \"metric_group_type\": " << c->metric_group_type << ",\n"
                << "        \"metric_data_address\": \"0x" << std::hex << c->metric_data_address
                << std::dec << "\"\n      }";
            break;
        }
        case VPU_CMD_COPY: {
            if (cmd->size < sizeof(vpu_cmd_copy_buffer_t)) {
                oss << ", \"error\": \"command too short for VPU_CMD_COPY\"}\n    }\n";
                cmdOffset = cmdHeader->cmd_buffer_size;
                continue;
            }
            auto *c = reinterpret_cast<vpu_cmd_copy_buffer_t *>(cmd);
            oss << ",\n        \"desc_start_offset\": \"0x" << std::hex << c->desc_start_offset
                << std::dec << "\",\n"
                << "        \"desc_count\": " << c->desc_count << "\n      }";
            break;
        }
        case VPU_CMD_INFERENCE_EXECUTE: {
            if (cmd->size < sizeof(vpu_cmd_inference_execute_t)) {
                oss << ", \"error\": \"command too short for VPU_CMD_INFERENCE_EXECUTE\"}\n    }\n";
                cmdOffset = cmdHeader->cmd_buffer_size;
                continue;
            }
            auto *c = reinterpret_cast<vpu_cmd_inference_execute_t *>(cmd);
            oss << ",\n        \"inference_id\": " << c->inference_id << ",\n"
                << "        \"host_mapped_inference_address\": \"0x" << std::hex
                << c->host_mapped_inference.address << std::dec << "\",\n"
                << "        \"host_mapped_inference_size\": \"0x" << std::hex
                << c->host_mapped_inference.width << std::dec << "\"\n      }";
            break;
        }
        case VPU_CMD_MEMORY_FILL: {
            if (cmd->size < sizeof(vpu_cmd_memory_fill_t)) {
                oss << ", \"error\": \"command too short for VPU_CMD_MEMORY_FILL\"}\n    }\n";
                cmdOffset = cmdHeader->cmd_buffer_size;
                continue;
            }
            auto *c = reinterpret_cast<vpu_cmd_memory_fill_t *>(cmd);
            oss << ",\n        \"fill_pattern\": \"0x" << std::hex << c->fill_pattern << std::dec
                << "\",\n"
                << "        \"start_address\": \"0x" << std::hex << c->start_address << std::dec
                << "\"\n      }";
            break;
        }
        default:
            oss << ", \"error\": \"unknown command type\"}\n    }\n";
            cmdOffset = cmdHeader->cmd_buffer_size;
            continue;
        }

        oss << ",\n      \"buffer_objects\": [";
        if (cmdIt->get() && !cmdIt->get()->getAssociateBufferObjects().empty()) {
            bool firstBo = true;
            for (const auto &bo : (*cmdIt)->getAssociateBufferObjects()) {
                oss << (firstBo ? "\n" : ",\n");
                firstBo = false;
                oss << "        {\"handle\": " << bo->getHandle() << ", \"vpu_address\": \"0x"
                    << std::hex << bo->getVPUAddr() << std::dec
                    << "\", \"size\": " << bo->getAllocSize() << "}";
            }
            oss << "\n      ]";
        } else {
            oss << "]";
        }
        if (cmdIt->get() && cmdIt->get()->getCommandType() == VPU_CMD_INFERENCE_EXECUTE) {
            auto *inferenceExecute = dynamic_cast<VPUInferenceExecute *>(cmdIt->get());
            if (inferenceExecute == nullptr) {
                oss << ",\n      \"error\": \"Failed to cast command to VPUInferenceExecute\"";
            } else {
                oss << ",\n      \"blob_name\": \"" << jsonEscape(inferenceExecute->getBlobName())
                    << "\"";
                oss << ",\n      \"user_arg_handles\": [";
                const auto &userArgHandles = inferenceExecute->getUserArgHandles();
                bool firstHandle = true;
                for (const auto &handle : userArgHandles) {
                    if (!firstHandle)
                        oss << ", ";
                    firstHandle = false;
                    oss << handle;
                }
                oss << "]";
            }
        }
        oss << "\n    }";

        cmdOffset += cmd->size;
        i++;
        cmdIt++;
    }

    oss << "\n  ]\n}";
    return oss.str();
}

void VPUCommandBuffer::printCommandBuffer(const char *description) const {
    if (getLogLevel() < LogLevel::INFO || !(VPU_CMD & getLogMask()))
        return;

    LOG(VPU_CMD, "Print command buffer: %s", commandBufferToJson(description).c_str());
}

} // namespace VPU

#else

namespace VPU {

std::string VPUCommandBuffer::commandBufferToJson(const char *description) const {
    (void)description;
    return {};
}

void VPUCommandBuffer::printCommandBuffer(const char *description) const {
    (void)description;
}

} // namespace VPU

#endif // ENABLE_NPU_LOGGING
