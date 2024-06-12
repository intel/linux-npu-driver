/*
 * Copyright (C) 2022-2024 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "umd_extensions.h"
#include "blob_params.hpp"
#include "model_params.hpp"
#include "umd_test.h"
#include "utilities/data_handle.h"
#include "utilities/graph_to_str.h"
#include "image.hpp"

#include <filesystem>
#include <random>

enum MemType {
    DEVICE_MEMORY,
    HOST_MEMORY,
    SHARED_MEMORY,
};

enum GraphFormat {
    BLOB,
    MODEL,
};

class Graph {
  public:
    Graph(ze_context_handle_t hContext, ze_device_handle_t hDevice, graph_dditable_ext_t *graphDDI)
        : hContext(hContext)
        , hDevice(hDevice)
        , graphDDI(graphDDI) {}

    static std::shared_ptr<Graph> create(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         graph_dditable_ext_t *graphDDI,
                                         UmdTest::GlobalConfig &globalConfig,
                                         const YAML::Node &node,
                                         uint32_t graphFlags = ZE_GRAPH_FLAG_NONE) {
        auto graph = std::make_shared<Graph>(hContext, hDevice, graphDDI);

        EXPECT_GT(node["path"].as<std::string>().size(), 0);
        std::filesystem::path path = node["path"].as<std::string>();

        if (path.extension() == ".xml") {
            graph->createFromModel(std::move(globalConfig.modelDir / path), node, graphFlags);
        } else {
            graph->format = GraphFormat::BLOB;
            graph->createFromBlob(std::move(globalConfig.blobDir / path), node, graph->npuBlob);
        }

        graph->queryArguments();

        return graph;
    }

    static size_t graphPrecisionToByteSize(ze_graph_argument_precision_t arg) {
        switch (arg) {
        case ZE_GRAPH_ARGUMENT_PRECISION_FP64:
        case ZE_GRAPH_ARGUMENT_PRECISION_INT64:
        case ZE_GRAPH_ARGUMENT_PRECISION_UINT64:
            return sizeof(uint64_t);

        case ZE_GRAPH_ARGUMENT_PRECISION_FP32:
        case ZE_GRAPH_ARGUMENT_PRECISION_INT32:
        case ZE_GRAPH_ARGUMENT_PRECISION_UINT32:
            return sizeof(uint32_t);

        case ZE_GRAPH_ARGUMENT_PRECISION_BF16:
        case ZE_GRAPH_ARGUMENT_PRECISION_FP16:
        case ZE_GRAPH_ARGUMENT_PRECISION_INT16:
        case ZE_GRAPH_ARGUMENT_PRECISION_UINT16:
            return sizeof(uint16_t);

        case ZE_GRAPH_ARGUMENT_PRECISION_INT8:
        case ZE_GRAPH_ARGUMENT_PRECISION_UINT8:
            return sizeof(uint8_t);

        case ZE_GRAPH_ARGUMENT_PRECISION_INT4:
        case ZE_GRAPH_ARGUMENT_PRECISION_UINT4:
        case ZE_GRAPH_ARGUMENT_PRECISION_BIN:
        case ZE_GRAPH_ARGUMENT_PRECISION_BOOLEAN:
            return 0;

        case ZE_GRAPH_ARGUMENT_PRECISION_UNKNOWN:
        case ZE_GRAPH_ARGUMENT_PRECISION_DYNAMIC:
        default:
            ADD_FAILURE() << "Unsupported graph argument precision";
            return 0;
        }
    }

    void allocateArguments(MemType memType) {
        allocateInputArguments(memType);
        allocateOutputArguments(memType);
    }

    void allocateInputArguments(MemType memType) {
        inArgs.clear();

        uint32_t argIndex = 0;
        for (auto size : inputSize) {
            auto ptr = allocMemory(size, memType);
            inArgs.push_back(ptr);
            ASSERT_EQ(setArgumentValue(argIndex++, ptr), ZE_RESULT_SUCCESS);
        }
    }

    void allocateOutputArguments(MemType memType) {
        outArgs.clear();

        uint32_t argIndex = inputSize.size();
        for (auto size : outputSize) {
            auto ptr = allocMemory(size, memType);
            outArgs.push_back(ptr);
            ASSERT_EQ(setArgumentValue(argIndex++, ptr), ZE_RESULT_SUCCESS);
        }
    }

    void setRandomInput() {
        std::vector<std::vector<char>> inputData;
        inputData.resize(inputSize.size());
        for (size_t i = 0; i < inputSize.size(); ++i) {
            DataHandle::generateRandomData(inputData[i], inputSize[i]);
            memcpy(inArgs[i], inputData[i].data(), inputData[i].size());
        }
    }

    void copyInputData() {
        if (isInputFileProvided && format == GraphFormat::BLOB) {
            for (size_t i = 0; i < inArgs.size(); i++) {
                memcpy(inArgs[i], inputBin[i].data(), inputBin[i].size());
            }
        } else {
            setRandomInput();
        }
    }

    void copyInputData(std::vector<void *> &target) {
        if (isInputFileProvided && format == GraphFormat::BLOB) {
            for (uint32_t i = 0; i < inputSize.size(); i++) {
                memcpy(target[i], inputBin[i].data(), inputBin[i].size());
            }
        } else {
            std::vector<std::vector<char>> inputData;
            inputData.resize(inputSize.size());
            for (size_t i = 0; i < inputSize.size(); ++i) {
                DataHandle::generateRandomData(inputData[i], inputSize[i]);
                memcpy(target[i], inputData[i].data(), inputData[i].size());
            }
        }
    }

    void loadInputData(std::filesystem::path path) {
        ASSERT_EQ(path.extension(), ".bmp");
        TRACE("Image: %s\n", path.c_str());

        Image image(path);
        ASSERT_EQ(inputSize[0], image.getSizeInBytes());
        memcpy(inArgs[0], image.getPtr(), inputSize[0]);
    }

    ze_result_t setArgumentValue(uint32_t argIndex, const void *pArgValue) {
        return graphDDI->pfnSetArgumentValue(handle, argIndex, pArgValue);
    }

    void checkResults() {
        if (isInputFileProvided && format == GraphFormat::BLOB) {
            for (size_t i = 0; i < outArgs.size(); i++) {
                ASSERT_EQ(memcmp(outArgs[i], outputBin[i].data(), outputBin[i].size()), 0);
            }
        }
    }

    void checkResults(std::vector<void *> &output) {
        if (isInputFileProvided && format == GraphFormat::BLOB) {
            for (size_t i = 0; i < outArgs.size(); i++) {
                ASSERT_EQ(memcmp(output[i], outputBin[i].data(), outputBin[i].size()), 0);
            }
        }
    }

    void checkResults(uint16_t imageClassIndex) {
        ze_graph_argument_precision_t outputType = outputPrecision.at(0);
        size_t elementSize = graphPrecisionToByteSize(outputType);
        if (elementSize == 0)
            FAIL() << "element size must be greater than 0";
        std::vector<float> outputData(outputSize.at(0) / elementSize);

        switch (outputType) {
        case ZE_GRAPH_ARGUMENT_PRECISION_FP32: {
            memcpy(outputData.data(), outArgs.at(0), outputSize.at(0));
            break;
        }
        case ZE_GRAPH_ARGUMENT_PRECISION_FP16: {
            auto outputPtr = reinterpret_cast<const uint16_t *>(outArgs.at(0));
            for (size_t i = 0; i < outputData.size(); i++) {
                outputData[i] = DataHandle::rawFp16ToFp32(outputPtr[i]);
            }
            break;
        }
        default:
            FAIL() << "Unsupported output precision " << zeGraphArgumentPrecisionToStr(outputType);
        }

        auto it = std::max_element(outputData.begin(), outputData.end());
        size_t index = std::distance(outputData.begin(), it);

        TRACE("Class = %li, Accuracy = %f\n", index, *it);

        ASSERT_EQ(index, imageClassIndex) << "Class index does not match!" << std::endl
                                          << "Class: " << index << "!=" << imageClassIndex;
    }

    void clearOutput() {
        for (size_t i = 0; i < outArgs.size(); i++) {
            memset(outArgs[i], 0, outputSize[i]);
        }
    }

  private:
    void queryArguments() {
        getArgumentsProperties();
        ASSERT_NE(inputSize.size(), 0);
        ASSERT_NE(outputSize.size(), 0);
        ASSERT_NE(outputPrecision.size(), 0);
    }

    void getArgumentsProperties() {
        ze_graph_properties_t properties{};
        ASSERT_EQ(graphDDI->pfnGetProperties(handle, &properties), ZE_RESULT_SUCCESS);

        for (uint32_t i = 0; i < properties.numGraphArgs; i++) {
            ze_graph_argument_properties_t argProperties{};

            ASSERT_EQ(graphDDI->pfnGetArgumentProperties(handle, i, &argProperties),
                      ZE_RESULT_SUCCESS);

            size_t size = 1u;
            for (int i = 0; i < ZE_MAX_GRAPH_ARGUMENT_DIMENSIONS_SIZE; i++)
                size *= argProperties.dims[i];
            size *= graphPrecisionToByteSize(argProperties.devicePrecision);

            ASSERT_GT(size, 0u);
            if (argProperties.type == ZE_GRAPH_ARGUMENT_TYPE_INPUT) {
                inputSize.push_back(size);
            } else {
                outputSize.push_back(size);
                outputPrecision.push_back(argProperties.devicePrecision);
            }
        }
    }

    void *allocMemory(size_t size, MemType memType) {
        if (memType == DEVICE_MEMORY) {
            mem.push_back(zeMemory::allocDevice(hContext, hDevice, size));
        } else if (memType == HOST_MEMORY) {
            mem.push_back(zeMemory::allocHost(hContext, size));
        } else {
            mem.push_back(zeMemory::allocShared(hContext, hDevice, size));
        }
        return mem.back().get();
    }

    std::vector<char> getFlagsFromString(std::string flags) {
        std::vector<char> buildFlags;

        for (auto c : flags)
            buildFlags.push_back(c);
        buildFlags.push_back('\0');
        return buildFlags;
    }

    void createGraphDescriptorForModel(const std::string &modelPath, uint32_t graphFlags) {
        std::vector<char> modelXml, modelBin;
        ze_device_graph_properties_t graphProperties;

        ASSERT_TRUE(getModelFromPath(modelPath, modelXml, modelBin));

        ASSERT_EQ(graphDDI->pfnDeviceGetGraphProperties(hDevice, &graphProperties),
                  ZE_RESULT_SUCCESS);

        ze_graph_compiler_version_info_t version = {.major = graphProperties.compilerVersion.major,
                                                    .minor = graphProperties.compilerVersion.minor};

        uint64_t xml_len = modelXml.size();
        uint64_t bin_len = modelBin.size();
        uint32_t numInputs = 2;
        uint64_t modelSize = sizeof(version) + sizeof(numInputs) + sizeof(xml_len) + xml_len +
                             sizeof(bin_len) + bin_len;

        modelIR.resize(modelSize);

        uint64_t offset = 0;
        memcpy(&modelIR[0], &version, sizeof(version));
        offset += sizeof(version);

        memcpy(&modelIR[offset], &numInputs, sizeof(numInputs));
        offset += sizeof(numInputs);

        memcpy(&modelIR[offset], &xml_len, sizeof(xml_len));
        offset += sizeof(xml_len);

        memcpy(&modelIR[offset], modelXml.data(), xml_len);
        offset += xml_len;

        memcpy(&modelIR[offset], &bin_len, sizeof(bin_len));
        offset += sizeof(bin_len);

        memcpy(&modelIR[offset], modelBin.data(), bin_len);

        desc.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES;
        desc.pNext = nullptr;
        desc.format = ZE_GRAPH_FORMAT_NGRAPH_LITE;
        desc.inputSize = modelIR.size();
        desc.pInput = modelIR.data();
        desc.pBuildFlags = buildFlags.data();
        desc.flags = graphFlags;
    }

    void createFromModel(std::string &&path, const YAML::Node &node, uint32_t graphFlags) {
        ASSERT_GT(node["flags"].as<std::string>().size(), 0);

        ze_result_t ret = ZE_RESULT_SUCCESS;

        buildFlags = getFlagsFromString(node["flags"].as<std::string>());
        TRACE("buildFlags: %s\n", std::string(buildFlags.begin(), buildFlags.end()).c_str());

        createGraphDescriptorForModel(path, graphFlags);

        scopedGraphHandle = zeScope::graphCreate2(graphDDI, hContext, hDevice, desc, ret);
        EXPECT_EQ(ret, ZE_RESULT_SUCCESS);

        handle = scopedGraphHandle.get();
    }

    void createFromBlob(std::string &&path, const YAML::Node &node, std::vector<char> &npuBlob) {
        ze_result_t ret = ZE_RESULT_SUCCESS;

        if (node["in"].IsDefined() && node["out"].IsDefined()) {
            ASSERT_TRUE(loadBlobDataFromNode(std::move(path),
                                             node["in"].as<std::vector<std::string>>(),
                                             node["out"].as<std::vector<std::string>>(),
                                             npuBlob,
                                             inputBin,
                                             outputBin));
        } else {
            ASSERT_TRUE(loadBlobFromPath(std::move(path), npuBlob));
            isInputFileProvided = false;
        }

        desc = {.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES,
                .pNext = nullptr,
                .format = ZE_GRAPH_FORMAT_NATIVE,
                .inputSize = npuBlob.size(),
                .pInput = reinterpret_cast<uint8_t *>(npuBlob.data()),
                .pBuildFlags = nullptr,
                .flags = ZE_GRAPH_FLAG_NONE};

        scopedGraphHandle = zeScope::graphCreate2(graphDDI, hContext, hDevice, desc, ret);
        EXPECT_EQ(ret, ZE_RESULT_SUCCESS);

        handle = scopedGraphHandle.get();
    }

  public:
    ze_context_handle_t hContext = nullptr;
    ze_device_handle_t hDevice = nullptr;
    graph_dditable_ext_t *graphDDI = nullptr;

    GraphFormat format = GraphFormat::MODEL;
    bool isInputFileProvided = true;

    std::vector<std::vector<char>> inputBin, outputBin;
    std::vector<char> npuBlob;

    ze_graph_handle_t handle = nullptr;

    std::vector<uint32_t> inputSize;
    std::vector<uint32_t> outputSize;
    std::vector<ze_graph_argument_precision_t> outputPrecision;

    std::vector<void *> inArgs, outArgs;

  private:
    ze_graph_desc_2_t desc = {};
    std::vector<char> buildFlags = {};
    std::vector<uint8_t> modelIR = {};

    std::vector<std::shared_ptr<void>> mem;

    zeScope::SharedPtr<ze_graph_handle_t> scopedGraphHandle = nullptr;
};
