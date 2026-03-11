/*
 * Copyright (C) 2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "graph_utilities.hpp"

static void dumpInference(const std::vector<void *> &outputPtrs, const Graph &graph) {
    if (graph.dumpOnFailDir.string().size() == 0) {
        return;
    }
    const auto &modelName = graph.modelName;
    const auto dumpDirPath =
        graph.dumpOnFailDir /
        ::testing::UnitTest::GetInstance()->current_test_info()->test_suite_name() /
        ::testing::UnitTest::GetInstance()->current_test_info()->name() / modelName;
    const auto &inputPtrs = graph.inArgs;
    const auto &inputSizes = graph.inputSize;
    const auto &outputSizes = graph.outputSize;
    const auto ref = graph.getReferenceOutput();
    const auto blobBinary = graph.getNativeBinaryAsNewBuffer();

    if (outputPtrs.size() != outputSizes.size()) {
        PRINTF("Something is wrong, mismatch in output pointers and sizes count, skipping dump.\n");
        return;
    }

    if (!std::filesystem::create_directories(dumpDirPath)) {
        PRINTF("Failed to create dump directory: %s\n", dumpDirPath.c_str());
        return;
    }

    for (size_t i = 0; i < inputPtrs.size(); ++i) {
        DataHandle::writeFile(dumpDirPath.string() + "/" + modelName + "-input-" +
                                  std::to_string(i) + ".in",
                              inputPtrs[i],
                              inputSizes[i]);
    }
    for (size_t i = 0; i < outputPtrs.size(); ++i) {
        DataHandle::writeFile(dumpDirPath.string() + "/" + modelName + "-output-" +
                                  std::to_string(i) + ".out",
                              outputPtrs[i],
                              outputSizes[i]);
    }
    /* Reference is dumped only when it is in binary form, class index is skipped*/
    if (std::holds_alternative<BinaryInferenceBuffer>(ref)) {
        const auto &reference = std::get<BinaryInferenceBuffer>(ref);

        for (size_t i = 0; i < reference.size(); ++i) {
            DataHandle::writeFile(dumpDirPath.string() + "/" + modelName + "-ref-output-" +
                                      std::to_string(i) + ".ref",
                                  reference[i].data(),
                                  reference[i].size());
        }
    } else {
        PRINTF("Reference output is not provided as binary or missed, skipping output reference "
               "dump.\n");
    }

    if (blobBinary != nullptr) {
        DataHandle::writeFile((dumpDirPath / (modelName + ".blob")).string(),
                              blobBinary->data().data(),
                              blobBinary->data().size());
    }
}

bool validateInferenceOutput(const std::vector<void *> &output, const Graph &reference) {
    if (std::holds_alternative<BinaryInferenceBuffer>(reference.outputRef)) {
        auto expectedOutput = std::get<BinaryInferenceBuffer>(reference.outputRef);
        if (output.size() != expectedOutput.size()) {
            EXPECT_EQ(output.size(), expectedOutput.size())
                << "Number of output arguments does not match number of reference outputs";
            return false;
        }
        for (size_t i = 0; i < output.size(); i++) {
            if (reference.outputSize[i] != expectedOutput[i].size()) {
                EXPECT_EQ(reference.outputSize[i], expectedOutput[i].size())
                    << "Output size for argument " << i << " does not match reference output size";
                return false;
            }
            auto result = memcmp(output[i], expectedOutput[i].data(), expectedOutput[i].size());
            EXPECT_EQ(result, 0);
            if (result != 0) {
                dumpInference(output, reference);
                return false;
            }
        }
        return true;
    }
    if (std::holds_alternative<ClassIndexBuffer>(reference.outputRef)) {
        auto indexes = std::get<ClassIndexBuffer>(reference.outputRef);

        if (indexes.empty()) {
            EXPECT_GT(indexes.size(), 0) << "No class indexes provided for validation";
            return false;
        }
        auto outputPrecision = reference.outputProps.at(0).networkPrecision;
        size_t elementSize = Graph::graphPrecisionToByteSize(outputPrecision);
        if (elementSize == 0) {
            EXPECT_GT(elementSize, 0) << "Element size must be greater than 0";
            dumpInference(output, reference);
            return false;
        }
        std::vector<float> outputData(reference.outputSize.at(0) / elementSize);

        switch (outputPrecision) {
        case ZE_GRAPH_ARGUMENT_PRECISION_FP32: {
            memcpy(outputData.data(), output.at(0), reference.outputSize.at(0));
            break;
        }
        case ZE_GRAPH_ARGUMENT_PRECISION_FP16: {
            auto outputPtr = reinterpret_cast<const uint16_t *>(output.at(0));
            for (size_t i = 0; i < outputData.size(); i++) {
                outputData[i] = DataHandle::rawFp16ToFp32(outputPtr[i]);
            }
            break;
        }
        default:
            EXPECT_TRUE(false) << "Unsupported output precision "
                               << zeGraphArgumentPrecisionToStr(outputPrecision);
            dumpInference(output, reference);
            return false;
        }

        auto it = std::max_element(outputData.begin(), outputData.end());
        size_t index = std::distance(outputData.begin(), it);

        TRACE("Class = %li, Accuracy = %f\n", index, *it);
        if (index != indexes[0]) {
            EXPECT_TRUE(false) << "Class index does not match!" << std::endl
                               << "Class: " << index << "!=" << indexes[0];
            dumpInference(output, reference);
            return false;
        }
        return true;
    }

    // There was no reference provided, consider it as passed
    return true;
}
