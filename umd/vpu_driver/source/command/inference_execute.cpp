/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/inference_execute.hpp"

#include "api/vpu_jsm_job_cmd_api.h"
#include "level_zero_driver/source/ext/elf_parser.hpp"
#include "umd_common.hpp"
#include "vpu_driver/source/command/command_buffer.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <unordered_map>
#include <utility>

using namespace VPU;

VPUInferenceExecute::VPUInferenceExecute(std::shared_ptr<L0::ElfParser> &parser,
                                         std::shared_ptr<elf::HostParsedInference> &hpi,
                                         const std::vector<const void *> &inputs,
                                         const std::vector<const void *> &outputs,
                                         L0::GraphProfilingQuery *profilingQuery,
                                         uint64_t inferenceId,
                                         std::vector<std::shared_ptr<VPUBufferObject>> &bos,
                                         std::vector<std::shared_ptr<VPUBufferObject>> &userBos)
    : parser(parser)
    , hpi(hpi)
    , inputs(inputs)
    , outputs(outputs)
    , profilingQuery(profilingQuery) {
    vpu_cmd_inference_execute_t cmd = {};
    cmd.header.type = VPU_CMD_INFERENCE_EXECUTE;
    cmd.header.size = sizeof(vpu_cmd_inference_execute_t);
    cmd.inference_id = inferenceId;
    cmd.host_mapped_inference.address = bos.at(0)->getVPUAddr();
    cmd.host_mapped_inference.width = safe_cast<uint32_t>(bos.at(0)->getAllocSize());
    command.emplace<vpu_cmd_inference_execute_t>(cmd);

    appendAssociateBufferObject(bos);
    userArgIndex = getAssociateBufferObjects().size();

    appendAssociateBufferObject(userBos);

    userArgHandles.reserve(userBos.size());
    for (const auto &bo : userBos) {
        userArgHandles.emplace_back(bo->getHandle());
    }

    LOG(VPU_CMD,
        "VPUInferenceExecute command created - hpi: %p, inference_id: %lu",
        hpi.get(),
        inferenceId);
}

std::shared_ptr<VPUInferenceExecute>
VPUInferenceExecute::create(std::shared_ptr<L0::ElfParser> parser,
                            std::shared_ptr<elf::HostParsedInference> &cmdHpi,
                            const std::vector<const void *> &inputPtrs,
                            const std::vector<const void *> &outputPtrs,
                            L0::GraphProfilingQuery *profilingQuery,
                            uint64_t inferenceId,
                            std::vector<std::shared_ptr<VPUBufferObject>> &bos) {
    std::vector<std::shared_ptr<VPUBufferObject>> userBos;
    userBos.reserve(inputPtrs.size() + outputPtrs.size());
    if (!parser->applyInputOutputs(cmdHpi, inputPtrs, outputPtrs, profilingQuery, userBos)) {
        LOG_E("Failed to apply arguments to elf executor");
        return nullptr;
    }

    return std::make_shared<VPUInferenceExecute>(parser,
                                                 cmdHpi,
                                                 inputPtrs,
                                                 outputPtrs,
                                                 profilingQuery,
                                                 inferenceId,
                                                 bos,
                                                 userBos);
}

bool VPUInferenceExecute::setUpdates(const ArgumentUpdatesMap &updatesMap) {
    uint32_t numInputArgs = safe_cast<uint32_t>(inputs.size());
    uint32_t numOutputArgs = safe_cast<uint32_t>(outputs.size());
    uint32_t numArgs = numInputArgs + numOutputArgs;

    for (const auto &[argIndex, newArg] : updatesMap) {
        if (argIndex >= numArgs) {
            LOG_E("Invalid argument index (%u). It exceeds the number of graph arguments %u",
                  argIndex,
                  numArgs);
            return false;
        }

        if (argIndex < numInputArgs) {
            inputs[argIndex] = newArg;
        } else {
            outputs[argIndex - numInputArgs] = newArg;
        }
    }

    cmdNeedsUpdate = true;
    return true;
}

bool VPUInferenceExecute::update(VPUCommandBuffer *commandBuffer) {
    if (!cmdNeedsUpdate)
        return true;

    cmdNeedsUpdate = false;

    std::vector<std::shared_ptr<VPUBufferObject>> userArgs;
    if (!parser->applyInputOutputs(hpi, inputs, outputs, profilingQuery, userArgs)) {
        return false;
    }

    std::vector<uint32_t> newHandles;
    for (const auto &bo : userArgs)
        newHandles.emplace_back(bo->getHandle());

    commandBuffer->replaceBufferHandles(userArgHandles, newHandles);
    userArgHandles = std::move(newHandles);

    eraseAssociatedBufferObjects(userArgIndex);
    appendAssociateBufferObject(std::move(userArgs));
    return true;
}

size_t VPUInferenceExecute::getSharedScratchSize() {
    return parser->getSharedScratchSize();
}

void VPUInferenceExecute::updateScratchBuffer(VPUCommandBuffer *cmdBuffer,
                                              std::shared_ptr<VPUBufferObject> scratchBuffer) {
    if (getSharedScratchSize() == 0)
        return;

    parser->updateSharedScratchBuffers(hpi, scratchBuffer);
    cmdBuffer->replaceBufferHandles({lastScratchBoHandle}, {scratchBuffer->getHandle()});
    lastScratchBoHandle = scratchBuffer->getHandle();
}
