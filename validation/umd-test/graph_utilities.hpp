/*
 * Copyright (C) 2022-2025 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "blob_params.hpp"
#include "image.hpp"
#include "model_params.hpp"
#include "umd_extensions.h"
#include "umd_test.h"
#include "utilities/data_handle.h"
#include "utilities/graph_to_str.h"

#include <filesystem>
#include <level_zero/ze_api.h>
#include <level_zero/ze_graph_ext.h>
#include <level_zero/ze_graph_profiling_ext.h>
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

enum InputType {
    BINARY,
    IMAGE,
    RANDOM,
};

class InferenceRequest {
  public:
    void setUp(ze_context_handle_t hContext,
               ze_device_handle_t hDevice,
               ze_graph_handle_t hGraph,
               graph_dditable_ext_t *dGraph) {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        ze_command_queue_desc_t desc = {.stype = ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
                                        .pNext = nullptr,
                                        .ordinal = 0,
                                        .index = 0,
                                        .flags = 0,
                                        .mode = ZE_COMMAND_QUEUE_MODE_DEFAULT,
                                        .priority = ZE_COMMAND_QUEUE_PRIORITY_NORMAL};
        scopedQueue = zeScope::commandQueueCreate(hContext, hDevice, desc, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        queue = scopedQueue.get();

        ze_command_list_desc_t descl = {.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC,
                                        .pNext = nullptr,
                                        .commandQueueGroupOrdinal = 0,
                                        .flags = 0};
        scopedList = zeScope::commandListCreate(hContext, hDevice, descl, ret);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        list = scopedList.get();

        ret = dGraph->pfnAppendGraphExecute(list, hGraph, nullptr, nullptr, 0u, nullptr);
        ASSERT_EQ(ret, ZE_RESULT_SUCCESS);
        ASSERT_EQ(zeCommandListClose(list), ZE_RESULT_SUCCESS);
    }

    ze_result_t runAsync() { return zeCommandQueueExecuteCommandLists(queue, 1, &list, nullptr); }
    ze_result_t wait(uint64_t ns) { return zeCommandQueueSynchronize(queue, ns); }

    void getResults();

  public:
    ze_command_queue_handle_t queue = nullptr;
    ze_command_list_handle_t list = nullptr;
    std::vector<void *> inArgs, outArgs;

  private:
    zeScope::SharedPtr<ze_command_queue_handle_t> scopedQueue;
    zeScope::SharedPtr<ze_command_list_handle_t> scopedList;
};

class GraphBuffer {
  public:
    static std::shared_ptr<GraphBuffer> get(ze_device_handle_t hDevice,
                                            graph_dditable_ext_t *graphDdi,
                                            UmdTest::GlobalConfig &globalConfig,
                                            const YAML::Node &node) {
        auto graphBuffer = std::make_shared<GraphBuffer>();
        if (!graphBuffer->loadFromNode(hDevice, graphDdi, globalConfig, node)) {
            return nullptr;
        }

        return graphBuffer;
    }

    static std::shared_ptr<GraphBuffer> get(ze_device_handle_t hDevice,
                                            graph_dditable_ext_t *graphDdi,
                                            const std::filesystem::path &path,
                                            const std::string &buildFlags) {
        auto graphBuffer = std::make_shared<GraphBuffer>();
        if (!graphBuffer->loadFromPath(hDevice, graphDdi, path, buildFlags)) {
            return nullptr;
        }

        return graphBuffer;
    }

    GraphBuffer() = default;
    GraphBuffer(std::vector<char> &&buf)
        : buffer(std::move(buf)) {
        desc.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES;
        desc.format = ZE_GRAPH_FORMAT_NATIVE;
        desc.inputSize = buffer.size();
        desc.pInput = reinterpret_cast<uint8_t *>(buffer.data());
        desc.pBuildFlags = nullptr;
        desc.flags = ZE_GRAPH_FLAG_NONE;
    }

  private:
    bool loadFromNode(ze_device_handle_t hDevice,
                      graph_dditable_ext_t *graphDdi,
                      UmdTest::GlobalConfig &globalconfig,
                      const YAML::Node &node) {
        std::filesystem::path path = node["path"].as<std::string>();
        std::string buildFlags = "";
        if (node["flags"].IsDefined()) {
            buildFlags = node["flags"].as<std::string>();
        }

        if (path.extension() == ".xml") {
            path = globalconfig.modelDir / path;
        } else {
            path = globalconfig.blobDir / path;
        }

        return loadFromPath(hDevice, graphDdi, path, buildFlags);
    }

    bool loadFromPath(ze_device_handle_t hDevice,
                      graph_dditable_ext_t *graphDdi,
                      const std::filesystem::path &path_,
                      const std::string &buildFlags_) {
        path = path_;
        buildFlags = buildFlags_;

        if (path.extension() == ".xml") {
            desc.format = ZE_GRAPH_FORMAT_NGRAPH_LITE;
            desc.pBuildFlags = buildFlags.c_str();
            loadNGraphModel(hDevice, graphDdi);
        } else {
            desc.format = ZE_GRAPH_FORMAT_NATIVE;
            desc.pBuildFlags = nullptr;
            loadBlobFromPath(path, buffer);
        }

        desc.stype = ZE_STRUCTURE_TYPE_GRAPH_DESC_PROPERTIES;
        desc.flags = ZE_GRAPH_FLAG_NONE;
        desc.inputSize = buffer.size();
        desc.pInput = reinterpret_cast<uint8_t *>(buffer.data());
        return true;
    }

    void loadNGraphModel(ze_device_handle_t hDevice, graph_dditable_ext_t *graphDdi) {
        std::vector<char> bufferXml, bufferBin;
        ze_device_graph_properties_t deviceGraphProp;

        ASSERT_TRUE(getModelFromPath(path, bufferXml, bufferBin));
        ASSERT_EQ(graphDdi->pfnDeviceGetGraphProperties(hDevice, &deviceGraphProp),
                  ZE_RESULT_SUCCESS);

        ze_graph_compiler_version_info_t version = {.major = deviceGraphProp.compilerVersion.major,
                                                    .minor = deviceGraphProp.compilerVersion.minor};

        uint64_t sizeXml = bufferXml.size();
        uint64_t sizeBin = bufferBin.size();
        uint32_t numInputs = 2;
        uint64_t modelSize = sizeof(version) + sizeof(numInputs) + sizeof(sizeXml) + sizeXml +
                             sizeof(sizeBin) + sizeBin;

        buffer.resize(modelSize);

        uint64_t offset = 0;
        memcpy(&buffer[0], &version, sizeof(version));
        offset += sizeof(version);

        memcpy(&buffer[offset], &numInputs, sizeof(numInputs));
        offset += sizeof(numInputs);

        memcpy(&buffer[offset], &sizeXml, sizeof(sizeXml));
        offset += sizeof(sizeXml);

        memcpy(&buffer[offset], bufferXml.data(), sizeXml);
        offset += sizeXml;

        memcpy(&buffer[offset], &sizeBin, sizeof(sizeBin));
        offset += sizeof(sizeBin);

        memcpy(&buffer[offset], bufferBin.data(), sizeBin);
    }

  public:
    std::filesystem::path path;
    ze_graph_desc_2_t desc = {};

  private:
    std::vector<char> buffer = {};
    std::string buildFlags = {};
};

class Graph {
  public:
    Graph(ze_context_handle_t hContext,
          ze_device_handle_t hDevice,
          graph_dditable_ext_t *graphDDI,
          std::shared_ptr<GraphBuffer> buffer)
        : hContext(hContext)
        , hDevice(hDevice)
        , graphDDI(graphDDI)
        , buffer(std::move(buffer)) {}

    static std::shared_ptr<Graph> create(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         graph_dditable_ext_t *graphDDI,
                                         UmdTest::GlobalConfig &globalConfig,
                                         const YAML::Node &node,
                                         std::shared_ptr<GraphBuffer> graphBuffer) {
        if (graphBuffer == nullptr)
            return nullptr;

        auto graph = std::make_shared<Graph>(hContext, hDevice, graphDDI, std::move(graphBuffer));
        if (!graph->createGraphHandle())
            return nullptr;

        graph->loadInputDataFromConfig(globalConfig, node);

        return graph;
    }

    static std::shared_ptr<Graph> create(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         graph_dditable_ext_t *graphDDI,
                                         const std::filesystem::path &path,
                                         const std::string &buildFlags) {
        auto buffer = GraphBuffer::get(hDevice, graphDDI, path, buildFlags);
        if (buffer == nullptr)
            return nullptr;

        auto graph = std::make_shared<Graph>(hContext, hDevice, graphDDI, std::move(buffer));
        if (!graph->createGraphHandle())
            return nullptr;

        return graph;
    }

    static std::shared_ptr<Graph> create(ze_context_handle_t hContext,
                                         ze_device_handle_t hDevice,
                                         graph_dditable_ext_t *graphDDI,
                                         UmdTest::GlobalConfig &globalConfig,
                                         const YAML::Node &node,
                                         uint32_t graphFlags = ZE_GRAPH_FLAG_NONE) {
        auto buffer = GraphBuffer::get(hDevice, graphDDI, globalConfig, node);
        if (buffer == nullptr)
            return nullptr;

        buffer->desc.flags = graphFlags;

        auto graph = std::make_shared<Graph>(hContext, hDevice, graphDDI, std::move(buffer));
        if (!graph->createGraphHandle())
            return nullptr;

        graph->loadArguments(globalConfig, node);
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
        for (size_t i = 0; i < inputSize.size(); ++i) {
            DataHandle::generateRandomData(inArgs[i], inputSize[i]);
        }
    }

    void copyInputData() {
        if (inputType == InputType::BINARY && format == GraphFormat::BLOB) {
            for (size_t i = 0; i < inArgs.size(); i++) {
                memcpy(inArgs[i], inputBin[i].data(), inputBin[i].size());
            }
        } else if (inputType == InputType::IMAGE) {
            if (images.empty()) {
                FAIL() << "No image provided. Check the path to the image";
            }

            TRACE("Image: %s\n", images[0].c_str());

            Image image(images[0]);
            ASSERT_EQ(inputSize[0], image.getSizeInBytes());
            memcpy(inArgs[0], image.getPtr(), inputSize[0]);
        } else {
            TRACE("No input file was provided: input will be filled with random values.\n");
            setRandomInput();
        }
    }

    void copyInputData(std::vector<void *> &target) {
        if (inputType == InputType::BINARY && format == GraphFormat::BLOB) {
            for (uint32_t i = 0; i < inputSize.size(); i++) {
                memcpy(target[i], inputBin[i].data(), inputBin[i].size());
            }
        } else if (inputType == InputType::IMAGE) {
            TRACE("Image: %s\n", images[0].c_str());

            Image image(images[0]);
            ASSERT_EQ(inputSize[0], image.getSizeInBytes());
            memcpy(target[0], image.getPtr(), inputSize[0]);
        } else {
            for (size_t i = 0; i < inputSize.size(); ++i) {
                DataHandle::generateRandomData(target[i], inputSize[i]);
            }
        }
    }

    ze_result_t setArgumentValue(uint32_t argIndex, const void *pArgValue) {
        return graphDDI->pfnSetArgumentValue(handle, argIndex, pArgValue);
    }

    void checkResults() {
        if (inputType == InputType::BINARY && format == GraphFormat::BLOB) {
            for (size_t i = 0; i < outArgs.size(); i++) {
                ASSERT_EQ(memcmp(outArgs[i], outputBin[i].data(), outputBin[i].size()), 0);
            }
        } else if (inputType == InputType::IMAGE) {
            size_t elementSize = graphPrecisionToByteSize(outputPrecision.at(0));
            if (elementSize == 0)
                FAIL() << "Element size must be greater than 0";
            std::vector<float> outputData(outputSize.at(0) / elementSize);

            switch (outputPrecision.at(0)) {
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
                FAIL() << "Unsupported output precision "
                       << zeGraphArgumentPrecisionToStr(outputPrecision.at(0));
            }

            auto it = std::max_element(outputData.begin(), outputData.end());
            size_t index = std::distance(outputData.begin(), it);

            TRACE("Class = %li, Accuracy = %f\n", index, *it);

            ASSERT_EQ(index, classIndexes[0]) << "Class index does not match!" << std::endl
                                              << "Class: " << index << "!=" << classIndexes[0];
        }
    }

    void checkResults(std::vector<void *> &output) {
        if (inputType == InputType::BINARY && format == GraphFormat::BLOB) {
            for (size_t i = 0; i < outArgs.size(); i++) {
                ASSERT_EQ(memcmp(output[i], outputBin[i].data(), outputBin[i].size()), 0);
            }
        }
    }

    void clearOutput() {
        for (size_t i = 0; i < outArgs.size(); i++) {
            memset(outArgs[i], 0, outputSize[i]);
        }
    }

    std::unique_ptr<InferenceRequest> newInferRequest() {
        auto infer = std::make_unique<InferenceRequest>();
        // Initialization is done in setUp to take advantage of ASSERT_*
        infer->setUp(hContext, hDevice, handle, graphDDI);
        return infer;
    }

    void *allocMemory(size_t size, MemType memType) {
        mem.push_back(allocManagedMemory(size, memType));
        return mem.back().get();
    }

    std::shared_ptr<void> allocManagedMemory(size_t size, MemType memType) {
        if (memType == DEVICE_MEMORY) {
            return zeMemory::allocDevice(hContext, hDevice, size);
        } else if (memType == HOST_MEMORY) {
            return zeMemory::allocHost(hContext, size);
        } else {
            return zeMemory::allocShared(hContext, hDevice, size);
        }
        return nullptr;
    }

    void *reallocArgument(size_t i, MemType memType) {
        std::unique_ptr<uint8_t[]> tmp(new uint8_t[inputSize[i]]);
        memcpy(tmp.get(), inArgs[i], inputSize[i]);

        mem[i].reset();
        mem[i] = allocManagedMemory(inputSize[i], memType);

        inArgs[i] = mem[i].get();
        memcpy(inArgs[i], tmp.get(), inputSize[i]);

        return inArgs[i];
    }

    void deallocateAllArguments() { mem.clear(); }

    std::shared_ptr<GraphBuffer> getNativeBinaryAsNewBuffer() {
        size_t size = 0;
        if (graphDDI->pfnGetNativeBinary(handle, &size, nullptr) != ZE_RESULT_SUCCESS)
            return nullptr;

        std::vector<char> buffer(size);
        if (graphDDI->pfnGetNativeBinary(handle,
                                         &size,
                                         reinterpret_cast<uint8_t *>(buffer.data())) !=
            ZE_RESULT_SUCCESS)
            return nullptr;

        return std::make_shared<GraphBuffer>(std::move(buffer));
    }

    std::shared_ptr<GraphBuffer> getNativeBinary2AsNewBuffer() {
        const uint8_t *buffer = nullptr;
        size_t size = 0;

        if (graphDDI->pfnGetNativeBinary2(handle, &size, &buffer) != ZE_RESULT_SUCCESS)
            return nullptr;

        std::vector<char> data(buffer, buffer + size);
        return std::make_shared<GraphBuffer>(std::move(data));
    }

  private:
    bool createGraphHandle() {
        ze_result_t ret = ZE_RESULT_SUCCESS;
        scopedGraphHandle = zeScope::graphCreate2(graphDDI, hContext, hDevice, buffer->desc, ret);
        if (ret != ZE_RESULT_SUCCESS)
            return false;

        handle = scopedGraphHandle.get();
        queryArguments();
        format = buffer->desc.format == ZE_GRAPH_FORMAT_NGRAPH_LITE ? GraphFormat::MODEL
                                                                    : GraphFormat::BLOB;
        return true;
    }

    void loadArguments(UmdTest::GlobalConfig &globalConfig, const YAML::Node &node) {
        if (format == GraphFormat::MODEL) {
            loadInputDataFromConfig(globalConfig, node);
        } else {
            format = GraphFormat::BLOB;
            loadBlobArgumentsFromFilesystem(node);
        }
    }

    void loadInputDataFromConfig(UmdTest::GlobalConfig &globalConfig, const YAML::Node &node) {
        if (node["in"].IsDefined() && node["class_index"].IsDefined()) {
            inputType = InputType::IMAGE;
            for (auto &image : node["in"].as<std::vector<std::string>>()) {
                std::filesystem::path imagePath = globalConfig.imageDir + image;
                if (std::filesystem::exists(imagePath)) {
                    EXPECT_EQ(imagePath.extension(), ".bmp");
                    images.push_back(imagePath);
                }
            }

            classIndexes = node["class_index"].as<std::vector<uint16_t>>();
        } else {
            inputType = InputType::RANDOM;
        }
    }

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

    void loadBlobArgumentsFromFilesystem(const YAML::Node &node) {
        if (node["in"].IsDefined() && node["out"].IsDefined()) {
            ASSERT_TRUE(loadBlobDataFromNode(buffer->path,
                                             node["in"].as<std::vector<std::string>>(),
                                             node["out"].as<std::vector<std::string>>(),
                                             inputBin,
                                             outputBin));
        } else {
            inputType = InputType::RANDOM;
        }
    }

  public:
    ze_context_handle_t hContext = nullptr;
    ze_device_handle_t hDevice = nullptr;
    graph_dditable_ext_t *graphDDI = nullptr;

    GraphFormat format = GraphFormat::MODEL;
    InputType inputType = InputType::BINARY;

    std::vector<std::vector<char>> inputBin, outputBin;
    std::vector<std::string> images;
    std::vector<uint16_t> classIndexes;
    uint32_t iterations = 1;

    ze_graph_handle_t handle = nullptr;

    std::vector<uint32_t> inputSize;
    std::vector<uint32_t> outputSize;
    std::vector<ze_graph_argument_precision_t> outputPrecision;

    std::vector<void *> inArgs, outArgs;

  private:
    std::shared_ptr<GraphBuffer> buffer;
    std::vector<std::shared_ptr<void>> mem;
    zeScope::SharedPtr<ze_graph_handle_t> scopedGraphHandle = nullptr;
};
