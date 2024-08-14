/*
 * Copyright (C) 2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "vpu_driver/source/command/vpu_inference_execute.hpp"

#include <cstddef>

#include "level_zero_driver/ext/source/graph/elf_parser.hpp"
#include "vpu_driver/source/command/vpu_command_buffer.hpp"
#include "vpu_driver/source/memory/vpu_buffer_object.hpp"
#include "vpu_driver/source/utilities/log.hpp"

#include <unordered_map>

using namespace VPU;

VPUInferenceExecute::VPUInferenceExecute(
    std::shared_ptr<L0::ElfParser> &parser,
    std::shared_ptr<elf::HostParsedInference> &hpi,
    const std::vector<std::pair<const void *, uint32_t>> &inputs,
    const std::vector<std::pair<const void *, uint32_t>> &outputs,
    const std::pair<void *, uint32_t> &profiling,
    uint64_t inferenceId,
    std::vector<VPUBufferObject *> bos,
    size_t argumentPosition)
    : parser(parser)
    , hpi(hpi)
    , inputs(inputs)
    , outputs(outputs)
    , profiling(profiling)
    , argBoPosition(argumentPosition) {
    vpu_cmd_inference_execute_t cmd = {};
    cmd.header.type = VPU_CMD_INFERENCE_EXECUTE;
    cmd.header.size = sizeof(vpu_cmd_inference_execute_t);
    cmd.inference_id = inferenceId;
    cmd.host_mapped_inference.address = bos[0]->getVPUAddr();
    cmd.host_mapped_inference.width = safe_cast<uint32_t>(bos[0]->getAllocSize());
    command.emplace<vpu_cmd_inference_execute_t>(cmd);

    appendAssociateBufferObject(bos);

    for (size_t i = argBoPosition; i < bos.size(); i++)
        argHandles.push_back(bos[i]->getHandle());
}

std::shared_ptr<VPUInferenceExecute>
VPUInferenceExecute::create(std::shared_ptr<L0::ElfParser> parser,
                            std::shared_ptr<elf::HostParsedInference> &cmdHpi,
                            const std::vector<std::pair<const void *, uint32_t>> &inputPtrs,
                            const std::vector<std::pair<const void *, uint32_t>> &outputPtrs,
                            const std::pair<void *, uint32_t> &profilingPtr,
                            uint64_t inferenceId,
                            std::vector<VPUBufferObject *> bos) {
    size_t inputOutputBoPosition = bos.size();
    if (!parser->applyInputOutputs(cmdHpi, inputPtrs, outputPtrs, profilingPtr, bos)) {
        LOG_E("Failed to apply arguments to elf executor");
        return nullptr;
    }

    return std::make_shared<VPUInferenceExecute>(parser,
                                                 cmdHpi,
                                                 inputPtrs,
                                                 outputPtrs,
                                                 profilingPtr,
                                                 inferenceId,
                                                 std::move(bos),
                                                 inputOutputBoPosition);
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
            inputs[argIndex].first = newArg;
        } else {
            outputs[argIndex - numInputArgs].first = newArg;
        }
    }

    cmdNeedsUpdate = true;
    return true;
}

bool VPUInferenceExecute::update(VPUCommandBuffer *commandBuffer) {
    cmdNeedsUpdate = false;

    std::vector<VPUBufferObject *> newArgBos;
    if (!parser->applyInputOutputs(hpi, inputs, outputs, profiling, newArgBos)) {
        return false;
    }

    std::vector<uint32_t> newHandles;
    for (const auto &bo : newArgBos)
        newHandles.emplace_back(bo->getHandle());

    commandBuffer->replaceBufferHandles(argHandles, newHandles);
    argHandles = std::move(newHandles);

    eraseAssociatedBufferObjects(argBoPosition);
    appendAssociateBufferObject(newArgBos);

    return true;
}
